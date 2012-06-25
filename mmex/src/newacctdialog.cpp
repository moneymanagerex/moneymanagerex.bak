/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#include "newacctdialog.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "maincurrencydialog.h"
#include "defs.h"
#include "util.h"
#include "paths.h"

enum { ACCT_TYPE_CHECKING, ACCT_TYPE_INVESTMENT, ACCT_TYPE_TERM };
enum { ACCT_STATUS_OPEN, ACCT_STATUS_CLOSED };


IMPLEMENT_DYNAMIC_CLASS( mmNewAcctDialog, wxDialog )

BEGIN_EVENT_TABLE( mmNewAcctDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmNewAcctDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmNewAcctDialog::OnCancel)
    EVT_BUTTON(ID_DIALOG_NEWACCT_BUTTON_CURRENCY, mmNewAcctDialog::OnCurrency)
END_EVENT_TABLE()

mmNewAcctDialog::mmNewAcctDialog( )
{
}

mmNewAcctDialog::mmNewAcctDialog( mmCoreDB* core, bool newAcct, int accountID,
                                 wxWindow* parent, wxWindowID id, const wxString& caption,
                                 const wxPoint& pos, const wxSize& size, long style )
{
    core_ = core;
    newAcct_ = newAcct;
    accountID_ = accountID;
    currencyID_ = -1;
    Create(parent, id, caption, pos, size, style);
    termAccount_ = false;
}

bool mmNewAcctDialog::Create( wxWindow* parent, wxWindowID id,
                             const wxString& caption, const wxPoint& pos, const wxSize& size,
                             long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    Centre();

    if (!newAcct_)
    {
        fillControlsWithData();
    }

    return TRUE;
}

void mmNewAcctDialog::fillControlsWithData()
{
    boost::shared_ptr<mmAccount> pAccount = core_->getAccountSharedPtr(accountID_);
    wxASSERT(pAccount);

    textAccountName_->SetValue(pAccount->name_);

    wxTextCtrl* textCtrl;
    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNUMBER);
    textCtrl->SetValue(pAccount->accountNum_);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT);
    textCtrl->SetValue(pAccount->heldAt_);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE);
    textCtrl->SetValue(pAccount->website_);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT);
    textCtrl->SetValue(pAccount->contactInfo_);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO);
    textCtrl->SetValue(pAccount->accessInfo_);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_NOTES);
    textCtrl->SetValue(pAccount->notes_);

    account_type_->SetStringSelection(wxGetTranslation(pAccount->acctType_));
    account_type_->Enable(newAcct_);

    wxChoice* choice = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS);
    choice->SetSelection(ACCT_STATUS_OPEN);
    if (pAccount->status_ == mmAccount::MMEX_Closed)
       choice->SetSelection(ACCT_STATUS_CLOSED);

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT);
    itemCheckBox->SetValue(pAccount->favoriteAcct_);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE);
    double initBal = pAccount->initialBalance_;

    boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
    wxASSERT(pCurrency);

    wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_NEWACCT_BUTTON_CURRENCY);
    bn->SetLabel(pCurrency->currencyName_);
    currencyID_ = pCurrency->currencyID_;

    mmDBWrapper::loadSettings(core_->db_.get(), currencyID_);
    wxString dispAmount;
    mmex::formatDoubleToCurrencyEdit(initBal, dispAmount);
    textCtrl->SetValue(dispAmount);
}

void mmNewAcctDialog::CreateControls()
{
    wxSizerFlags flags_main, flags_expand;
    flags_main.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 4);
    flags_expand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 4).Expand();

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(0, 2, 6, 6);
    itemBoxSizer2->Add(itemGridSizer2, 0, wxALL, 15);

    wxStaticText* itemStaticText3 = new wxStaticText( this,
        wxID_STATIC, _("Account Name:"));
    itemGridSizer2->Add(itemStaticText3, flags_main);

    textAccountName_ = new wxTextCtrl( this, ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNAME, (""));
    itemGridSizer2->Add(textAccountName_, flags_expand);
    textAccountName_->SetToolTip(_("Enter the Name of the Account. This name can be renamed at any time."));

    wxStaticText* itemStaticText51 = new wxStaticText( this, wxID_STATIC, _("Account Type:"));
    itemGridSizer2->Add(itemStaticText51, flags_main);

    account_type_ = new wxChoice( this, ID_DIALOG_NEWACCT_COMBO_ACCTTYPE);
    account_type_->Append(wxGetTranslation(ACCOUNT_TYPE_BANK), new wxStringClientData(ACCOUNT_TYPE_BANK));
    account_type_->Append(wxGetTranslation(ACCOUNT_TYPE_STOCK), new wxStringClientData(ACCOUNT_TYPE_STOCK));
    account_type_->Append(wxGetTranslation(ACCOUNT_TYPE_TERM), new wxStringClientData(ACCOUNT_TYPE_TERM));
    itemGridSizer2->Add(account_type_, flags_expand);
    account_type_->SetToolTip(_("Specify the type of account to be created."));
    account_type_->SetSelection(0);

    wxStaticText* itemStaticText5 = new wxStaticText( this, wxID_STATIC, _("Account Number:"));
    itemGridSizer2->Add(itemStaticText5, flags_main);

    wxTextCtrl* itemTextCtrl6 = new wxTextCtrl( this,
        ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNUMBER);
    itemGridSizer2->Add(itemTextCtrl6, flags_expand);
    itemTextCtrl6->SetToolTip(_("Enter the Account Number associated with this account."));

    wxStaticText* itemStaticText7 = new wxStaticText( this, wxID_STATIC, _("Held At:"));
    itemGridSizer2->Add(itemStaticText7, flags_main);

    wxTextCtrl* itemTextCtrl8 = new wxTextCtrl( this, ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT);
    itemGridSizer2->Add(itemTextCtrl8, flags_expand);
    itemTextCtrl8->SetToolTip(_("Enter the name of the financial institution in which the account is held."));

    wxStaticText* itemStaticText9 = new wxStaticText( this, wxID_STATIC, _("Website:"));
    itemGridSizer2->Add(itemStaticText9, flags_main);

    wxTextCtrl* itemTextCtrl10 = new wxTextCtrl( this, ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE);
    itemGridSizer2->Add(itemTextCtrl10, flags_expand);
    itemTextCtrl10->SetToolTip(_("Enter the URL of the website for the financial institution."));

    wxStaticText* itemStaticText11 = new wxStaticText( this, wxID_STATIC, _("Contact:"));
    itemGridSizer2->Add(itemStaticText11, flags_main);

    wxTextCtrl* itemTextCtrl12 = new wxTextCtrl( this, ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT);
    itemGridSizer2->Add(itemTextCtrl12, flags_expand);
    itemTextCtrl12->SetToolTip(_("Enter any contact information for the financial institution."));

    wxStaticText* info_label = new wxStaticText( this,
        wxID_STATIC, _("Access Info:"));
    itemGridSizer2->Add(info_label, flags_main);

    wxTextCtrl* itemTextCtrl14 = new wxTextCtrl( this, ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO);
    itemGridSizer2->Add(itemTextCtrl14, flags_expand);
    itemTextCtrl14->SetToolTip(_("Enter any login/access information for the financial institution. This is not secure as anyone with access to the mmb file can access it."));

    wxStaticText* itemStaticText15 = new wxStaticText( this, wxID_STATIC, _("Account Status:"));
    itemGridSizer2->Add(itemStaticText15, flags_main);

    wxArrayString itemChoice6Strings;
    itemChoice6Strings.Add(_("Open"));
    itemChoice6Strings.Add(_("Closed"));

    wxChoice* itemChoice6 = new wxChoice( this,
        ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS, wxDefaultPosition, wxDefaultSize, itemChoice6Strings);
    itemGridSizer2->Add(itemChoice6, flags_expand);
    itemChoice6->SetSelection(ACCT_STATUS_OPEN);
    itemChoice6->SetToolTip(_("Specify if this account has been closed. Closed accounts are inactive in most calculations, reporting etc."));

    wxStaticText* balance_label = new wxStaticText( this,
        wxID_STATIC, _("Initial Balance:"));
    itemGridSizer2->Add(balance_label, flags_main);

    wxTextCtrl* itemTextCtrl19 = new wxTextCtrl( this,
        ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE,
        (""), wxDefaultPosition, wxDefaultSize,
        wxALIGN_RIGHT|wxTE_PROCESS_ENTER , wxFloatingPointValidator<float>(2));
    itemGridSizer2->Add(itemTextCtrl19, flags_expand);
    itemTextCtrl19->SetToolTip(_("Enter the initial balance in this account."));

    wxStaticText* currency_label = new wxStaticText( this,
        wxID_STATIC, _("Currency:"));
    itemGridSizer2->Add(currency_label, flags_main);

    currencyID_ = mmDBWrapper::getBaseCurrencySettings(core_->db_.get());
    wxString currName = _("Select Currency");
    if (currencyID_ != -1)
    {
       currName = core_->getCurrencyName(currencyID_);
    }

    wxButton* itemButton71 = new wxButton( this,
        ID_DIALOG_NEWACCT_BUTTON_CURRENCY, currName);
    itemGridSizer2->Add(itemButton71, flags_expand);
    itemButton71->SetToolTip(_("Specify the currency to be used by this account."));

    wxStaticText* itemStaticText17 = new wxStaticText( this,
        wxID_STATIC, _("Notes:"));
    itemGridSizer2->Add(itemStaticText17, flags_main);

    wxTextCtrl* itemTextCtrl18 = new wxTextCtrl( this,
        ID_DIALOG_NEWACCT_TEXTCTRL_NOTES,
        (""), wxDefaultPosition, wxSize(150, 70), wxTE_MULTILINE );
    itemGridSizer2->Add(itemTextCtrl18, flags_expand);
    itemTextCtrl18->SetToolTip(_("Enter user notes and details about this account."));

    itemGridSizer2->AddSpacer(5);

    wxCheckBox* itemCheckBox10 = new wxCheckBox( this,
        ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT, _("Favorite Account"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBox10->SetValue(TRUE);
    itemGridSizer2->Add(itemCheckBox10, flags_main);
    itemCheckBox10->SetToolTip(_("Select whether this is an account that is used often. This is used to filter accounts display view."));

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_RIGHT|wxALL, 10);

    wxButton* itemButton7 = new wxButton( this, wxID_OK);
    itemBoxSizer9->Add(itemButton7, flags_main);

    wxButton* itemButtonCancel = new wxButton( this, wxID_CANCEL);
    itemBoxSizer9->Add(itemButtonCancel, flags_main);
    itemButtonCancel->SetFocus();
}

void mmNewAcctDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void mmNewAcctDialog::OnCurrency(wxCommandEvent& /*event*/)
{
    //mmMainCurrencyDialog dlg(core_, this);
    //if ( dlg.ShowModal() == wxID_OK )
    if (mmMainCurrencyDialog::Execute(core_, this,currencyID_))
    {
        //currencyID_ = dlg.currencyID_;
        wxString currName = core_->getCurrencyName(currencyID_);
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_NEWACCT_BUTTON_CURRENCY);
        bn->SetLabel(currName);
    }
}

bool mmNewAcctDialog::termAccountActivated()
{
    return termAccount_;
}

void mmNewAcctDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString acctName = textAccountName_->GetValue().Trim();
    if (acctName.IsEmpty())
    {
        mmShowErrorMessageInvalid(this, _("Account Name "));
        return;
    }

    int checkAcctID = core_->getAccountID(acctName);
    if ((checkAcctID != -1) && (checkAcctID != accountID_))
    {
        mmShowErrorMessage(this, _("Account Name already exists"), _("Error"));
        return;
    }

    if (currencyID_ == -1)
    {
        mmShowErrorMessageInvalid(this, _("Currency"));
        return;
    }

    boost::shared_ptr<mmAccount> pAccount;
    if (newAcct_)
    {
       boost::shared_ptr<mmAccount> tAccount(new mmAccount());
       pAccount = tAccount;
    }
    else
    {
       pAccount = core_->getAccountSharedPtr(accountID_);
    }

    wxString type = ACCOUNT_TYPE_BANK;
    wxStringClientData* acc_type_obj = (wxStringClientData *)account_type_->GetClientObject(account_type_->GetSelection());
    if (acc_type_obj) type = acc_type_obj->GetData();

    pAccount->acctType_ = type;
    termAccount_ = (type == ACCOUNT_TYPE_TERM);

    wxTextCtrl* textCtrlAcctNumber = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNUMBER);
    wxTextCtrl* textCtrlHeldAt = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT);
    wxTextCtrl* textCtrlWebsite = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE);
    wxTextCtrl* textCtrlContact = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT);
    wxTextCtrl* textCtrlAccess = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO);
    wxTextCtrl* textCtrlNotes = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_NOTES);
    wxChoice* choice = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS);
    int acctStatus = choice->GetSelection();
    pAccount->status_ = mmAccount::MMEX_Open;
    if (acctStatus == ACCT_STATUS_CLOSED)
        pAccount->status_ = mmAccount::MMEX_Closed;

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT);
    if (itemCheckBox->IsChecked())
       pAccount->favoriteAcct_ = true;
    else
       pAccount->favoriteAcct_ = false;

    mmDBWrapper::loadSettings(core_->db_.get(), currencyID_);
    wxTextCtrl* textCtrlInit = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE);
    wxString bal = textCtrlInit->GetValue().Trim();
    pAccount->initialBalance_ = 0.0;
    if (!bal.IsEmpty())
    {
        if (!mmex::formatCurrencyToDouble(bal, pAccount->initialBalance_))
        {
            mmShowErrorMessageInvalid(this, _("Init Balance "));
            return;
        }
    }

    pAccount->name_ = acctName;
    pAccount->accountNum_ = textCtrlAcctNumber->GetValue();
    pAccount->notes_ = textCtrlNotes->GetValue();
    pAccount->heldAt_ = textCtrlHeldAt->GetValue();
    pAccount->website_ = textCtrlWebsite->GetValue();
    pAccount->contactInfo_ = textCtrlContact->GetValue();
    pAccount->accessInfo_ = textCtrlAccess->GetValue();
    pAccount->currency_ = core_->getCurrencySharedPtr(currencyID_);

    if (newAcct_)
    {
        core_->addAccount(pAccount);
    }
    else
    {
        core_->updateAccount(pAccount);
    }

    EndModal(wxID_OK);
}

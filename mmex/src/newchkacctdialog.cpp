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
 /*******************************************************/

#include "newchkacctdialog.h"
#include "dbwrapper.h"
#include "maincurrencydialog.h"

#include "util.h"

#define ACCT_TYPE_CHECKING 0
#define ACCT_TYPE_INVESTMENT 1

#define ACCT_STATUS_OPEN 0
#define ACCT_STATUS_CLOSED 1

IMPLEMENT_DYNAMIC_CLASS( mmNewAcctDialog, wxDialog )

BEGIN_EVENT_TABLE( mmNewAcctDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_NEWACCT_BUTTON_OK, mmNewAcctDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_NEWACCT_BUTTON_CANCEL, mmNewAcctDialog::OnCancel)
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

    wxIcon icon(mainicon_xpm);
    SetIcon(icon);
    
    Centre();

    if (!newAcct_)
    {
        fillControlsWithData();
    }    

    return TRUE;
}

void mmNewAcctDialog::fillControlsWithData()
{
    boost::shared_ptr<mmAccount> pAccount = core_->accountList_.getAccountSharedPtr(accountID_);
    wxASSERT(pAccount);

    textAccountName_->SetValue(pAccount->accountName_);

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

    wxChoice* itemAcctType = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_ACCTTYPE);
    if (pAccount->acctType_ == wxT("Checking"))
       itemAcctType->SetSelection(ACCT_TYPE_CHECKING);
    else
       itemAcctType->SetSelection(ACCT_TYPE_INVESTMENT);
    itemAcctType->Enable(false);

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
    mmCurrencyFormatter::formatDoubleToCurrencyEdit(initBal, dispAmount);
    textCtrl->SetValue(dispAmount);
}

void mmNewAcctDialog::CreateControls()
{  
    mmNewAcctDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer2->Add(itemGridSizer2, 0, wxALL);
    
    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Account Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText3, 0, 
        wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textAccountName_ = new wxTextCtrl( itemDialog1, ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNAME, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(textAccountName_, 0, wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textAccountName_->SetToolTip(_("Enter the Name of the Account. This name can be renamed at any time."));

    wxStaticText* itemStaticText51 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Account Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText51, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemAcctTypeStrings[] =  
    {
       _("Checking"),
       _("Investment"),
    };
    wxChoice* itemChoice61 = new wxChoice( itemDialog1, ID_DIALOG_NEWACCT_COMBO_ACCTTYPE, 
        wxDefaultPosition, wxDefaultSize, 2, itemAcctTypeStrings, 0 );
    itemGridSizer2->Add(itemChoice61, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemChoice61->SetSelection(ACCT_TYPE_CHECKING);
    itemChoice61->SetToolTip(_("Specify the type of account to be created."));

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Account Number:"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText5, 0, 
        wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl6 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNUMBER, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemTextCtrl6, 0, wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemTextCtrl6->SetToolTip(_("Enter the Account Number associated with this account."));

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Held At:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText7, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl8 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemTextCtrl8, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemTextCtrl8->SetToolTip(_("Enter the name of the financial institution in which the account is held."));

    wxStaticText* itemStaticText9 = new wxStaticText( itemDialog1, wxID_STATIC, _("Website:"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl10 = new wxTextCtrl( itemDialog1, ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemTextCtrl10, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemTextCtrl10->SetToolTip(_("Enter the URL of the website for the financial institution."));

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Contact:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText11, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    wxTextCtrl* itemTextCtrl12 = new wxTextCtrl( itemDialog1, ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemTextCtrl12, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemTextCtrl12->SetToolTip(_("Enter any contact information for the financial institution."));

    wxStaticText* itemStaticText13 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Access Info:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText13, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl14 = new wxTextCtrl( itemDialog1, ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemTextCtrl14, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemTextCtrl14->SetToolTip(_("Enter any login/access information for the financial institution. This is not secure as anyone with access to the mmb file can access it."));

    wxStaticText* itemStaticText15 = new wxStaticText( itemDialog1, wxID_STATIC, _("Account Status:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText15, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemChoice6Strings[] =  
    {
        _("Open"),
        _("Closed")
    };
    wxChoice* itemChoice6 = new wxChoice( itemDialog1, 
        ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS, wxDefaultPosition, wxDefaultSize, 2, itemChoice6Strings, 0 );
    itemGridSizer2->Add(itemChoice6, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemChoice6->SetSelection(ACCT_STATUS_OPEN);
    itemChoice6->SetToolTip(_("Specify if this account has been closed. Closed accounts are inactive in most calculations, reporting etc."));
    
    wxStaticText* itemStaticText18 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Initial Balance:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText18, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl19 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemTextCtrl19, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemTextCtrl19->SetToolTip(_("Enter the initial balance in this account."));

    wxStaticText* itemStaticText181 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Currency:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText181, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    currencyID_ = mmDBWrapper::getBaseCurrencySettings(core_->db_.get());
    wxString currName = _("Select Currency");
    if (currencyID_ != -1)
    {
       currName = mmDBWrapper::getCurrencyName(core_->db_.get(), currencyID_);
    }

    wxButton* itemButton71 = new wxButton( itemDialog1, 
        ID_DIALOG_NEWACCT_BUTTON_CURRENCY, currName, wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButton71, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);    
    itemButton71->SetToolTip(_("Specify the currency to be used by this account."));

    wxStaticText* itemStaticText17 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Notes:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText17, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl18 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_NEWACCT_TEXTCTRL_NOTES, 
        wxT(""), wxDefaultPosition, wxSize(150, -1), wxTE_MULTILINE );
    itemGridSizer2->Add(itemTextCtrl18, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemTextCtrl18->SetToolTip(_("Enter user notes and details about this account."));

     itemGridSizer2->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* itemCheckBox10 = new wxCheckBox( itemDialog1, 
        ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT, _("Favorite Account"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBox10->SetValue(TRUE);
    itemGridSizer2->Add(itemCheckBox10, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBox10->SetToolTip(_("Select whether this is an account that is used often. This is used to filter accounts display view."));

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
   
    wxButton* itemButton7 = new wxButton( itemDialog1, 
        ID_DIALOG_NEWACCT_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemDialog1, 
        ID_DIALOG_NEWACCT_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void mmNewAcctDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void mmNewAcctDialog::OnCurrency(wxCommandEvent& event)
{
   mmMainCurrencyDialog *dlg = new mmMainCurrencyDialog(core_, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        currencyID_ = dlg->currencyID_;
        wxString currName = mmDBWrapper::getCurrencyName(core_->db_.get(), currencyID_);
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_NEWACCT_BUTTON_CURRENCY);
        bn->SetLabel(currName);
    }

    dlg->Destroy();
}

void mmNewAcctDialog::OnOk(wxCommandEvent& event)
{
    wxString acctName = textAccountName_->GetValue().Trim();
    if (acctName.IsEmpty())
    {
        mmShowErrorMessageInvalid(this, _("Account Name "));
        return;
    }
    
    int checkAcctID = core_->accountList_.getAccountID(acctName);
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

    wxChoice* itemAcctType = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_ACCTTYPE);
    int acctType = itemAcctType->GetSelection();
   
    boost::shared_ptr<mmAccount> pAccount;
    if (newAcct_)
    {
        mmAccount* ptrBase;
       if (acctType == ACCT_TYPE_CHECKING)
          ptrBase = new mmCheckingAccount(core_->db_);
       else
          ptrBase = new mmInvestmentAccount(core_->db_);
       boost::shared_ptr<mmAccount> tAccount(ptrBase);
       pAccount = tAccount;
    }
    else
    {
       pAccount = core_->accountList_.getAccountSharedPtr(accountID_);
    }

    pAccount->acctType_ = wxT("Checking");
    if (acctType == ACCT_TYPE_INVESTMENT)
        pAccount->acctType_ = wxT("Investment");

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
    if (!bal.IsEmpty())
    {
        if (!mmCurrencyFormatter::formatCurrencyToDouble(bal, pAccount->initialBalance_))
        {
            mmShowErrorMessageInvalid(this, _("Init Balance "));
            return;
        }
    }

    pAccount->accountName_ = acctName;
    pAccount->accountNum_ = mmCleanString(textCtrlAcctNumber->GetValue());
    pAccount->notes_ = mmCleanString(textCtrlNotes->GetValue());
    pAccount->heldAt_ = mmCleanString(textCtrlHeldAt->GetValue());
    pAccount->website_ = mmCleanString(textCtrlWebsite->GetValue());
    pAccount->contactInfo_ = mmCleanString(textCtrlContact->GetValue());
    pAccount->accessInfo_ = mmCleanString(textCtrlAccess->GetValue());
    pAccount->currency_ = core_->currencyList_.getCurrencySharedPtr(currencyID_);

    if (newAcct_)
    {
        core_->accountList_.addAccount(pAccount);
    }
    else
    {
        core_->accountList_.updateAccount(pAccount);
    }

    EndModal(wxID_OK);
}


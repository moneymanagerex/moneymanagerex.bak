/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011 Nikolay & Stefano Giorgio

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

#include "constants.h"
#include "transdialog.h"
#include "wx/datectrl.h"
#include "wx/dateevt.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "splittransactionsdialog.h"
#include "defs.h"
#include "paths.h"
#include <wx/event.h>
#include <wx/choice.h>

#include <sstream>

// Defines for Transaction: (Status and Type) now located in dbWrapper.h

IMPLEMENT_DYNAMIC_CLASS( mmTransDialog, wxDialog )

BEGIN_EVENT_TABLE( mmTransDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmTransDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmTransDialog::OnCancel)
    EVT_CLOSE(mmTransDialog::OnQuit)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmTransDialog::OnCategs)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmTransDialog::OnTransTypeChanged)
    EVT_CHECKBOX(ID_DIALOG_TRANS_ADVANCED_CHECKBOX, mmTransDialog::OnAdvanceChecked)
    EVT_CHECKBOX(ID_DIALOG_TRANS_SPLITCHECKBOX, mmTransDialog::OnSplitChecked)
    EVT_CHILD_FOCUS(mmTransDialog::changeFocus)
    EVT_SPIN(wxID_ANY,mmTransDialog::OnSpin)
    EVT_DATE_CHANGED(ID_DIALOG_TRANS_BUTTONDATE, mmTransDialog::OnDateChanged)
END_EVENT_TABLE()

mmTransDialog::mmTransDialog(
    mmCoreDB* core,
    int accountID, mmBankTransaction* pBankTransaction, bool edit,
    wxWindow* parent, wxWindowID id,
    const wxString& caption, const wxPoint& pos,
    const wxSize& size, long style
) :
    core_(core),
    parent_(parent),
    pBankTransaction_(pBankTransaction),
    accountID_(accountID),
    referenceAccountID_(accountID),
    payeeUnknown_(true),
    categUpdated_(false),
    edit_(edit),
    advancedToTransAmountSet_(false),
    edit_currency_rate(1.0),
    categID_(-1),
    subcategID_(-1),
    payeeID_(-1),
    toID_(-1),
    toTransAmount_(-1),
    transAmount_(-1)

{
    Create(parent, id, caption, pos, size, style);
}

bool mmTransDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
                           const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    boost::shared_ptr<mmSplitTransactionEntries> split(new mmSplitTransactionEntries());
    split_ = split;

    dataToControls();

    Centre();
    Fit();

    return TRUE;
}

void mmTransDialog::dataToControls()
{
    // Use last date used as per user option.
    wxDateTime trx_date = wxDateTime::Now();
    if (mmIniOptions::instance().transDateDefault_ != 0)
        trx_date = core_->bTransactionList_.getLastDate(accountID_);

    dpc_->SetValue(edit_ ? pBankTransaction_->date_: trx_date);
    //process date change event for set weekday name
    wxDateEvent dateEvent(dpc_, trx_date, wxEVT_DATE_CHANGED);
    GetEventHandler()->ProcessEvent(dateEvent);

    wxString transTypeString;
    wxString dispAmount;
    if (edit_)
    {
        wxString statusString = pBankTransaction_->status_;
        if (statusString == wxT("")) statusString = wxT("N");
        choiceStatus_->SetSelection(wxString(wxT("NRVFD")).Find(statusString));

        transTypeString = pBankTransaction_->transType_;

        categID_ = pBankTransaction_->categID_;
        categoryName_ = core_->categoryList_.GetCategoryName(categID_);
        subcategID_ = pBankTransaction_->subcategID_;
        subCategoryName_ = core_->categoryList_.GetSubCategoryName(categID_, subcategID_);

        accountID_ = pBankTransaction_->accountID_;
        toID_ = pBankTransaction_->toAccountID_;

        payeeID_ = pBankTransaction_->payeeID_;
        if (payeeID_ > -1) // transaction_type != TRANS_TYPE_TRANSFER_STR
            payeeUnknown_ = false;

        textNotes_->SetValue(pBankTransaction_->notes_);
        textNumber_->SetValue(pBankTransaction_->transNum_);

        transAmount_ = pBankTransaction_->amt_;
        toTransAmount_ = pBankTransaction_->toAmt_;

        mmex::formatDoubleToCurrencyEdit(transAmount_, dispAmount);
        textAmount_->SetValue(dispAmount);
        mmex::formatDoubleToCurrencyEdit(toTransAmount_, dispAmount);
        toTextAmount_->SetValue(dispAmount);
    }
    else
    {
        choiceStatus_->SetSelection(mmIniOptions::instance().transStatusReconciled_);
        transTypeString = TRANS_TYPE_WITHDRAWAL_STR;

        notesColour_ = textNotes_->GetForegroundColour();
        textNotes_->SetForegroundColour(wxColour(wxT("GREY")));
        textNotes_->SetValue(notesTip_);
        int font_size = textNotes_->GetFont().GetPointSize();
        textNotes_->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));
    }
    // backup the original currency rate first
    if (transAmount_ > 0.0)
        edit_currency_rate = toTransAmount_ / transAmount_;

    size_t begin = 0, size = sizeof(TRANSACTION_TYPE)/sizeof(wxString);
    if (transTypeString == TRANS_TYPE_TRANSFER_STR)
    {
        begin = size-1;
        transaction_type_->Disable();
    }
    else if (core_->accountList_.getNumBankAccounts() < 2 || edit_) size--;
    
    for(size_t i = begin; i < size; ++i)
    {
        transaction_type_->Append(wxGetTranslation(TRANSACTION_TYPE[i]),
        new wxStringClientData(TRANSACTION_TYPE[i]));
    }
    transaction_type_->SetStringSelection(wxGetTranslation(transTypeString));

    cAdvanced_->SetValue(advancedToTransAmountSet_);

    wxString payeeName;
    wxString categString = _("Select Category");

    if (transTypeString == TRANS_TYPE_TRANSFER_STR)
    {
        advancedToTransAmountSet_ = (transAmount_ != toTransAmount_);
        payeeName = cbPayee_->GetValue();
        SetTransferControls();  // hide appropriate fields
    }
    else
    {
        cAdvanced_->Disable();
        if (edit_)
        {
            payeeName = pBankTransaction_->payeeStr_;
        }
        else
        {
            //If user does not want payee to be auto filled for the new transaction
            payeeName = resetPayeeString();
            payeeUnknown_ = true;

            if ( mmIniOptions::instance().transPayeeSelectionNone_ > 0)
            {
                if (payeeID_ < 0)
                {
                    payeeID_ = core_->bTransactionList_.getLastUsedPayeeID(accountID_, categID_, subcategID_);
                    payeeName = core_->payeeList_.GetPayeeName(payeeID_);
                    payeeUnknown_ = false;
                }
            }
        }
    }
    updateControlsForTransType();
    if (edit_)
    {
        *split_.get() = *core_->bTransactionList_.getBankTransactionPtr(pBankTransaction_->transactionID())->splitEntries_.get();

        if (split_->numEntries() > 0)
        {
            bCategory_->SetLabel(_("Split Category"));
            cSplit_->SetValue(true);
            cSplit_->Disable();
        }
        else
        {
            categString = pBankTransaction_->fullCatStr_;
        }
    }
    else
    {
        if (mmIniOptions::instance().transCategorySelectionNone_> 0)
            if (categID_ > -1) categString = core_->categoryList_.GetFullCategoryString(categID_, subcategID_);

    }

    bCategory_->SetLabel(categString);

    if (split_->numEntries() > 0)
        textAmount_->Enable(false);

}

void mmTransDialog::OnTransTypeChanged(wxCommandEvent& /*event*/)
{
    updateControlsForTransType();
}

void mmTransDialog::updateControlsForTransType()
{
    //FIXME:
    //SetTransferControls(transaction_type_->GetSelection()==DEF_TRANSFER);
    bool transfer_transaction = transaction_type_->GetStringSelection() == wxGetTranslation(TRANS_TYPE_TRANSFER_STR); 
    SetTransferControls(transfer_transaction);
    if (!edit_)
    {
        wxString categString = resetCategoryString();
        if (transfer_transaction && mmIniOptions::instance().transCategorySelectionNone_ != 0)
        {
            categID_ = core_->bTransactionList_.getTransferCategoryID(accountID_, subcategID_);
            categString = core_->categoryList_.GetFullCategoryString(categID_, subcategID_);
        }
        bCategory_->SetLabel(categString);
    }
}

void mmTransDialog::SetTransferControls(bool transfer)
{
    cbPayee_ -> SetEvtHandlerEnabled(false);
    cAdvanced_->SetValue(advancedToTransAmountSet_);
    cAdvanced_->Enable(transfer);
    cSplit_->Enable(!transfer);
    cSplit_->SetValue(false);
    bPayee_->Enable(!transfer);
    wxString dataStr = wxT("");
    cbPayee_->Clear();
    wxArrayString data;
    int type_num = transaction_type_->GetSelection();

    if (transfer)
    {
        textAmount_->SetToolTip(amountTransferTip_);
        toTextAmount_->SetToolTip(_("Specify the transfer amount in the To Account"));
    }
    else
    {
        textAmount_->SetToolTip(amountNormalTip_);
        toTextAmount_->UnsetToolTip();
    }

    if ((accountID_ == referenceAccountID_) && transfer)
    {
        //textAmount_->Enable(true);
        toTextAmount_->Enable(cAdvanced_->GetValue());

        if (toID_ > 0) dataStr = (core_->accountList_.GetAccountName(toID_));
        payee_label_->SetLabel(_("To"));
        data = core_->accountList_.getAccountsName(accountID_);
        cbPayee_->SetToolTip(_("Specify which account the transfer is going to"));
    }
    else if ((accountID_ != referenceAccountID_) && transfer)
    {
        //textAmount_->Enable(cAdvanced_->GetValue());
        toTextAmount_->Enable(cAdvanced_->GetValue());
        if (accountID_ > 0) dataStr = (core_->accountList_.GetAccountName(accountID_));
        payee_label_->SetLabel(_("From"));
        data = core_->accountList_.getAccountsName(toID_);
        cbPayee_->SetToolTip(_("Specify which account the transfer is comming from"));
    }
    else
    {
        if (type_num == DEF_WITHDRAWAL)
        {
            cbPayee_->SetToolTip(_("Specify where the transaction is coming from"));
            payee_label_->SetLabel(_("From"));
        }
        else
        {
            cbPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));
            payee_label_->SetLabel(_("Payee"));
        }
        data = core_->payeeList_.FilterPayees(wxT(""));
        toTextAmount_->Enable(false);
        toTextAmount_->SetValue(wxT(""));
        dataStr = core_->payeeList_.GetPayeeName(payeeID_);
    }

    for (size_t i = 0; i < data.Count(); ++i)
    {
        cbPayee_ ->Append(data[i]);
    }
#if wxCHECK_VERSION(2,9,0)
    cbPayee_->AutoComplete(data);
#endif

    if (!cbPayee_ -> SetStringSelection(dataStr))
        cbPayee_ -> SetValue(dataStr);

    cbPayee_ -> SetEvtHandlerEnabled(true);
}

void mmTransDialog::CreateControls()
{
    const int border = 5;
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, border);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, border).Expand();

    wxBoxSizer* box_sizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* box_sizer2 = new wxBoxSizer(wxVERTICAL);
    box_sizer1->Add(box_sizer2, flags);

    wxStaticBox* static_box = new wxStaticBox(this, wxID_ANY, _("Transaction Details"));
    wxStaticBoxSizer* box_sizer = new wxStaticBoxSizer(static_box, wxVERTICAL);
    box_sizer2->Add(box_sizer, flags);

    wxFlexGridSizer* flex_sizer = new wxFlexGridSizer(0, 2, 0, 0);
    box_sizer->Add(flex_sizer, flags);

    // Date --------------------------------------------
    long date_style = wxDP_DROPDOWN|wxDP_SHOWCENTURY;

    dpc_ = new wxDatePickerCtrl( this, ID_DIALOG_TRANS_BUTTONDATE, wxDateTime::Now(),
                                 wxDefaultPosition, wxSize(110, -1), date_style);

#ifdef __WXGTK__
    dpc_->Connect(ID_DIALOG_TRANS_BUTTONDATE, wxEVT_KEY_UP,
        wxKeyEventHandler(mmTransDialog::OnButtonDateChar), NULL, this);
#endif

    //Text field for day of the week
    itemStaticTextWeek_ = new wxStaticText(this, wxID_STATIC, wxT(""));
    // Display the day of the week

    spinCtrl_ = new wxSpinButton(this, wxID_STATIC,
        wxDefaultPosition, wxSize(18, wxSize(dpc_->GetSize()).GetHeight()),
        wxSP_VERTICAL|wxSP_ARROW_KEYS|wxSP_WRAP);
    spinCtrl_->SetRange (-32768, 32768);

    flex_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Date")), flags);
    wxBoxSizer* date_sizer = new wxBoxSizer(wxHORIZONTAL);
    flex_sizer->Add(date_sizer);
    date_sizer->Add(dpc_, flags);
    date_sizer->Add(spinCtrl_, flags);
    date_sizer->Add(itemStaticTextWeek_, flags);

    // Status --------------------------------------------
    choiceStatus_ = new wxChoice(this, ID_DIALOG_TRANS_STATUS,
        wxDefaultPosition, wxSize(110, -1));

    for(size_t i = 0; i < sizeof(TRANSACTION_STATUS)/sizeof(wxString); ++i)
        choiceStatus_->Append(wxGetTranslation(TRANSACTION_STATUS[i]),
        new wxStringClientData(TRANSACTION_STATUS[i]));

    flex_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Status")), flags);
    flex_sizer->Add(choiceStatus_, flags);

    // Type --------------------------------------------
    transaction_type_ = new wxChoice(this, ID_DIALOG_TRANS_TYPE,
        wxDefaultPosition, wxSize(110, -1));

    cAdvanced_ = new wxCheckBox(this,
        ID_DIALOG_TRANS_ADVANCED_CHECKBOX, _("Advanced"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

    wxBoxSizer* typeSizer = new wxBoxSizer(wxHORIZONTAL);

    flex_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Type")), flags);
    flex_sizer->Add(typeSizer);
    typeSizer->Add(transaction_type_, flags);
    typeSizer->Add(cAdvanced_, flags);

    // Amount Fields --------------------------------------------

    textAmount_ = new wxTextCtrl( this, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""),
        wxDefaultPosition, wxSize(110, -1),
        wxALIGN_RIGHT|wxTE_PROCESS_ENTER, doubleValidator());
    textAmount_->Connect(ID_DIALOG_TRANS_TEXTAMOUNT, wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(mmTransDialog::onTextEntered), NULL, this);

    toTextAmount_ = new wxTextCtrl( this, ID_DIALOG_TRANS_TOTEXTAMOUNT, wxT(""),
        wxDefaultPosition, wxSize(110, -1),
        wxALIGN_RIGHT|wxTE_PROCESS_ENTER, doubleValidator());
    toTextAmount_->Connect(ID_DIALOG_TRANS_TOTEXTAMOUNT, wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(mmTransDialog::onTextEntered), NULL, this);

    wxBoxSizer* amountSizer = new wxBoxSizer(wxHORIZONTAL);
    amountSizer->Add(textAmount_, flags);
    amountSizer->Add(toTextAmount_, flags);

    flex_sizer->Add(new wxStaticText( this, wxID_STATIC, _("Amount")), flags);
    flex_sizer->Add(amountSizer);

    // Payee ---------------------------------------------
    wxBoxSizer* payeeSizer = new wxBoxSizer(wxHORIZONTAL);
    payee_label_ = new wxStaticText(this, wxID_STATIC, _("Payee"));

    cbPayee_ = new wxComboBox(this, ID_DIALOG_TRANS_PAYEECOMBO, wxT(""),
        wxDefaultPosition, wxSize(190, -1),
        core_->payeeList_.FilterPayees(wxT("")), wxTE_PROCESS_ENTER);
    cbPayee_->Connect(wxID_ANY, wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(mmTransDialog::OnPayeeTextEnter), NULL, this);
    cbPayee_->Connect(ID_DIALOG_TRANS_PAYEECOMBO, wxEVT_COMMAND_TEXT_UPDATED,
        wxCommandEventHandler(mmTransDialog::OnPayeeUpdated), NULL, this);

    cbPayee_ -> SetEvtHandlerEnabled(!edit_);

    bPayee_ = new wxBitmapButton( this, ID_DIALOG_TRANS_BUTTONPAYEE, wxBitmap(user_edit_xpm));
    bPayee_->Connect(ID_DIALOG_TRANS_BUTTONPAYEE, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(mmTransDialog::OnPayee), NULL, this);

    flex_sizer->Add(payee_label_, flags);
    flex_sizer->Add(payeeSizer);
    payeeSizer->Add(cbPayee_, flags);
    payeeSizer->Add(bPayee_, flags);

    // Split Category -------------------------------------------
    cSplit_ = new wxCheckBox(this, ID_DIALOG_TRANS_SPLITCHECKBOX,
        _("Split"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cSplit_->SetValue(FALSE);

    flex_sizer->AddSpacer(20);  // Fill empty space.
    flex_sizer->Add(cSplit_, flags);

    // Category -------------------------------------------------
    bCategory_ = new wxButton(this, ID_DIALOG_TRANS_BUTTONCATEGS, wxT(""),
        wxDefaultPosition, wxSize(230, -1));

    flex_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Category")), flags);
    flex_sizer->Add(bCategory_, flags);

    // Number  ---------------------------------------------
    textNumber_ = new wxTextCtrl(this,
        ID_DIALOG_TRANS_TEXTNUMBER, wxT(""), wxDefaultPosition,
        cbPayee_->GetSize(), wxTE_PROCESS_ENTER);
    textNumber_->Connect(ID_DIALOG_TRANS_TEXTNUMBER, wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(mmTransDialog::onTextEntered), NULL, this);

    bAuto_ = new wxButton(this,
        ID_DIALOG_TRANS_BUTTONTRANSNUM, wxT("..."), wxDefaultPosition, bPayee_->GetSize());
    bAuto_ -> Connect(ID_DIALOG_TRANS_BUTTONTRANSNUM,
        wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mmTransDialog::OnAutoTransNum), NULL, this);

    flex_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Number")), flags);
    wxBoxSizer* number_sizer = new wxBoxSizer(wxHORIZONTAL);
    flex_sizer->Add(number_sizer);
    number_sizer->Add(textNumber_, flags);
    number_sizer->Add(bAuto_, flags);

    // Notes  ---------------------------------------------
/*
    wxNotebook* trx_notebook = new wxNotebook(this,
        wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
    wxPanel* notes_tab = new wxPanel(trx_notebook, wxID_ANY);
    trx_notebook->AddPage(notes_tab, _("Notes"));
    wxBoxSizer *notes_sizer = new wxBoxSizer(wxVERTICAL);
    notes_tab->SetSizer(notes_sizer);

    textNotes_ = new wxTextCtrl(notes_tab, ID_DIALOG_TRANS_TEXTNOTES, wxT(""),
        wxDefaultPosition, wxSize(290,120), wxTE_MULTILINE);

    box_sizer->Add(trx_notebook);
    notes_sizer->Add(textNotes_, flags);
*/

    notesTip_ = _("Notes");
    textNotes_ = new wxTextCtrl(this, ID_DIALOG_TRANS_TEXTNOTES, wxT(""),
        wxDefaultPosition, wxSize(-1,80), wxTE_MULTILINE);

    box_sizer->Add(textNotes_, flagsExpand.Border(wxLEFT|wxRIGHT|wxBOTTOM, 10));

    amountNormalTip_   = _("Specify the amount for this transaction");
    amountTransferTip_ = _("Specify the amount to be transfered");
    if (true) //TODO: Add parameter
    {
        dpc_->SetToolTip(_("Specify the date of the transaction"));
        spinCtrl_->SetToolTip(_("Retard or advance the date of the transaction"));
        choiceStatus_->SetToolTip(_("Specify the status for the transaction"));
        transaction_type_->SetToolTip(_("Specify the type of transactions to be created."));
        cAdvanced_->SetToolTip(_("Allows the setting of different amounts in the FROM and TO accounts."));
        textAmount_->SetToolTip(amountNormalTip_);
        bPayee_->SetToolTip(_("Specify where the transaction is going to"));
        cSplit_->SetToolTip(_("Use split Categories"));
        bCategory_->SetToolTip(_("Specify the category for this transaction"));
        textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));
        bAuto_->SetToolTip(_("Populate Transaction #"));
        textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));
    }

    /**********************************************************************************************
     Button Panel with OK and Cancel Buttons
    ***********************************************************************************************/
    wxPanel* buttons_panel = new wxPanel(this, wxID_ANY);
    box_sizer1->Add(buttons_panel, flags.Center().Border(0));

    wxStdDialogButtonSizer*  buttons_sizer = new wxStdDialogButtonSizer;
    buttons_panel->SetSizer(buttons_sizer);

    wxButton* itemButtonOK = new wxButton( buttons_panel, wxID_OK);
    itemButtonCancel_ = new wxButton( buttons_panel, wxID_CANCEL);

    buttons_sizer->Add(itemButtonOK, flags.Border(wxBOTTOM|wxRIGHT, 10));
    buttons_sizer->Add(itemButtonCancel_, flags);

    if (edit_)
        itemButtonCancel_->SetFocus();

    buttons_sizer->Realize();
    Center();
    this->SetSizer(box_sizer1);
}

void mmTransDialog::OnPayeeUpdated(wxCommandEvent& event)
{
    wxString value = cbPayee_->GetValue();

    if (transaction_type_->GetSelection() != DEF_TRANSFER)
        payeeID_ = core_->payeeList_.GetPayeeId(value);
    else
        toID_ = core_->accountList_.GetAccountId(value);
    event.Skip();
}

void mmTransDialog::OnPayeeTextEnter(wxCommandEvent& event)
{
    wxString value = cbPayee_->GetValue(), new_value;

    cbPayee_ -> SetEvtHandlerEnabled(false);
    cbPayee_ -> Clear();
    wxArrayString data;

    if (transaction_type_->GetSelection() != DEF_TRANSFER)
    {
        data = core_->payeeList_.FilterPayees(wxT(""));
        for (size_t i=0; i< data.Count(); ++i)
        {
            if (data[i].Lower().Matches(value.Lower() + wxT("*")))
                cbPayee_ ->Append(data[i]);
        }
        cbPayee_ ->SetSelection(0);
        new_value = cbPayee_->GetStringSelection();
        payeeID_ = core_->payeeList_.GetPayeeId(new_value);
    }
    else
    {
        data = core_->accountList_.getAccountsName(accountID_);
        for (size_t i=0; i< data.Count(); ++i)
        {
            if (data[i].Lower().Matches(value.Lower() + wxT("*")))
                cbPayee_ ->Append(data[i]);
        }
        cbPayee_ ->SetSelection(0);
        new_value = cbPayee_->GetStringSelection();
        toID_ = core_->accountList_.GetAccountId(new_value);
    }
#if wxCHECK_VERSION(2,9,0)
        cbPayee_->AutoComplete(data);
#endif

    if (value == new_value)
        bCategory_->SetFocus();

    cbPayee_ -> SetEvtHandlerEnabled(true);
    event.Skip();
}

void mmTransDialog::OnPayee(wxCommandEvent& /*event*/)
{
    mmPayeeDialog dlg(parent_, core_);

    if ( dlg.ShowModal() == wxID_OK )
    {
        payeeID_ = dlg.getPayeeId();
        if (payeeID_ == -1)
        {
            cbPayee_->SetValue(resetPayeeString());
            payeeUnknown_ = true;
            return;
        }

        boost::shared_ptr<mmPayee> pPayee = core_->payeeList_.GetPayeeSharedPtr(payeeID_);
        cbPayee_->SetValue(mmReadyDisplayString(pPayee->name_));
        payeeUnknown_ = false;

        // If this is a Split Transaction, ignore displaying last category for payee
        if (split_->numEntries())
            return;

        // Only for new transactions: if user want to autofill last category used for payee.
        if ( mmIniOptions::instance().transCategorySelectionNone_ == 1 && ( !edit_ && !categUpdated_ ) )
        {
            // if payee has memory of last category used then display last category for payee
            if (pPayee->categoryId_ != -1)
            {
                categID_ = pPayee->categoryId_;
                subcategID_ = pPayee->subcategoryId_;
                categoryName_ = core_->categoryList_.GetCategoryName(categID_);
                subCategoryName_ = core_->categoryList_.GetSubCategoryName(
                        categID_, subcategID_);
                bCategory_->SetLabel(core_->categoryList_.GetFullCategoryString(categID_, subcategID_));
            }
        }
    }
    else
    {
        if (core_->payeeList_.GetPayeeId(cbPayee_->GetValue())<0)
            cbPayee_->SetValue(resetPayeeString());
    }
}

void mmTransDialog::OnAutoTransNum(wxCommandEvent& /*event*/)
{
    wxString current_number = textNumber_->GetValue();
    wxDateTime transaction_date = dpc_->GetValue();
    wxArrayString number_strings = core_->bTransactionList_.getTransactionNumber(accountID_, transaction_date);
    int i = number_strings.GetCount();
    int s =0;
    if (number_strings.Index(current_number) != wxNOT_FOUND)
         s = number_strings.Index(current_number);
    i = (s+1)%i;

    textNumber_->SetValue( number_strings[i] );
}

void mmTransDialog::OnSpin(wxSpinEvent& event)
{
    wxString dateStr = dpc_->GetValue().FormatISODate();
    wxDateTime date = mmGetStorageStringAsDate (dateStr) ;
    int value = event.GetPosition();

    date = date.Add(wxDateSpan::Days(value));
    dpc_->SetValue (date);
    spinCtrl_->SetValue(0);

    //process date change event for set weekday name
    wxDateEvent dateEvent(dpc_, date, wxEVT_DATE_CHANGED);
    GetEventHandler()->ProcessEvent(dateEvent);

    event.Skip();
}

void mmTransDialog::OnDateChanged(wxDateEvent& event)
{
    //get weekday name
    wxDateTime date = dpc_->GetValue();
    wxString dateStr = wxT("");
    if (event.GetDate().IsValid())
        dateStr = mmGetNiceWeekDayName( date.GetWeekDay() /*event.GetDate()*/); //fix for wx2.9.x

    itemStaticTextWeek_->SetLabel(dateStr);
    event.Skip();
}

void mmTransDialog::OnAdvanceChecked(wxCommandEvent& /*event*/)
{
    advancedToTransAmountSet_ = cAdvanced_->GetValue();
    toTextAmount_->SetValue(textAmount_->GetValue());
    SetTransferControls();
}

void mmTransDialog::OnCategs(wxCommandEvent& /*event*/)
{
    if (cSplit_->GetValue())
    {
        activateSplitTransactionsDlg();
        SetSplitState();
    }
    else
    {
        mmCategDialog dlg(core_, parent_, true, false);
        dlg.setTreeSelection(categoryName_, subCategoryName_);
        if ( dlg.ShowModal() == wxID_OK )
        {
            categID_ = dlg.getCategId();
            subcategID_ = dlg.getSubCategId();
            categUpdated_ = true;

            categoryName_ = core_->categoryList_.GetCategoryName(categID_);
            subCategoryName_ = core_->categoryList_.GetSubCategoryName(categID_, subcategID_);

            bCategory_->SetLabel(core_->categoryList_.GetFullCategoryString(categID_, subcategID_));
        }
    }
}

wxString mmTransDialog::resetPayeeString(/*bool normal*/) //normal is deposits or withdrawls
{
    wxString payeeStr = wxT("");
    if (transaction_type_->GetSelection() != DEF_TRANSFER)
    {
        payeeID_ = -1;
        wxArrayString filtd = core_->payeeList_.FilterPayees(wxT(""));
        if (filtd.Count() == 1)
        {
            //only one payee present. Choose it
            payeeStr = filtd[0];
            payeeID_ = core_->payeeList_.GetPayeeId(payeeStr);
        }
    }
    else
    {
        toID_ = -1;
    }

    return payeeStr;
}

wxString mmTransDialog::resetCategoryString()
{
    const wxString catStr = _("Select Category");
    categID_ = -1;
    subcategID_ = -1;

    return catStr;
}

void mmTransDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString transaction_type = wxT("");
    wxStringClientData* type_obj = (wxStringClientData *)transaction_type_->GetClientObject(transaction_type_->GetSelection());
    if (type_obj) transaction_type = type_obj->GetData();

    if (payeeID_ == -1 && transaction_type != TRANS_TYPE_TRANSFER_STR)
    {
        wxString payee_name = cbPayee_->GetValue();
        if (payee_name.IsEmpty())
        {
            mmShowErrorMessageInvalid(this, _("Payee"));
            return;
        }
        payeeID_ = core_->payeeList_.AddPayee(payee_name);
    }

    if (toID_ == -1 && transaction_type == TRANS_TYPE_TRANSFER_STR)
    {
        mmShowErrorMessageInvalid(this, _("To Account"));
        bPayee_->SetFocus();
        return;
    }

    if (cSplit_->GetValue())
    {
        if (split_->numEntries() == 0)
        {
            mmShowErrorMessageInvalid(this, _("Category"));
            return;
        }
    }
    else // if payee just has been created categid still null
    {
        if (categID_ < 1)
        {
            mmShowErrorMessageInvalid(this, _("Category"));
            return;
        }
    }

    double amount;
    if (cSplit_->GetValue())
    {
        amount = split_->getTotalSplits();
        if (amount < 0.0)
        {
            if (transaction_type == TRANS_TYPE_TRANSFER_STR) {
                if (amount < 0)
                    amount = - amount;
            } else {
                mmShowErrorMessageInvalid(parent_, _("Amount"));
                return;
            }
        }
    }
    else
    {
        wxString amountStr = textAmount_->GetValue().Trim();
        if (!mmex::formatCurrencyToDouble(amountStr, amount) || (amount < 0.0))
        {
            textAmount_->SetBackgroundColour(wxT("RED"));
            mmShowErrorMessageInvalid(parent_, _("Amount"));
            textAmount_->SetBackgroundColour(wxNullColour);
            textAmount_->SetFocus();
            return;
        }
    }

    if (transaction_type == TRANS_TYPE_TRANSFER_STR && advancedToTransAmountSet_)
    {
        wxString amountStr = toTextAmount_->GetValue().Trim();
        if (amountStr.IsEmpty() || !mmex::formatCurrencyToDouble(amountStr, toTransAmount_) || (toTransAmount_ < 0.0))
        {
            //toTextAmount_->Enable(!advancedToTransAmountSet_);
            toTextAmount_->SetBackgroundColour(wxT("RED"));
            mmShowErrorMessageInvalid(parent_, _("Advanced Amount"));
            toTextAmount_->SetBackgroundColour(wxNullColour);
            //toTextAmount_->Enable(advancedToTransAmountSet_);
            toTextAmount_->SetFocus();
            return;
        }
        if (!advancedToTransAmountSet_)
        {
            if (accountID_ == referenceAccountID_)
                toTransAmount_ = amount;
            else
            {
                transAmount_ = toTransAmount_;
                amount = toTransAmount_;
            }
            toTextAmount_->SetValue(wxString()<<amount);
        }
    }
    else
    {
        if (transaction_type == TRANS_TYPE_TRANSFER_STR)
        {
            toTransAmount_ = amount;
        }
    }

    int toAccountID = -1;
    int fromAccountID = accountID_;
    if (transaction_type == TRANS_TYPE_TRANSFER_STR)
    {
        if (toID_ == -1)
        {
            mmShowErrorMessageInvalid(parent_, _("To Account"));
            return;
        }

        toAccountID = toID_;
        payeeID_ = -1;
    }
    else
    {
        // save the category used for this payee to allow automatic category fill at user request.
        boost::shared_ptr<mmPayee> pPayee = core_->payeeList_.GetPayeeSharedPtr(payeeID_);
        pPayee->categoryId_ = categID_;
        pPayee->subcategoryId_ = subcategID_;
        pPayee->UpdateDb(core_->db_.get());

        if (referenceAccountID_ != accountID_) // Transfer transaction has defected to other side.
        {
            fromAccountID = referenceAccountID_;
        }
    }

    if (!advancedToTransAmountSet_ || toTransAmount_ < 0)
    {
        // if we are adding a new record and the user did not touch advanced dialog
        // we are going to use the transfer amount by calculating conversion rate.
        // subsequent edits will not allow automatic update of the amount
        if (!edit_)
        {
            if(toAccountID != -1) {
                double rateFrom = core_->accountList_.getAccountBaseCurrencyConvRate(fromAccountID);
                double rateTo = core_->accountList_.getAccountBaseCurrencyConvRate(toAccountID);

                double convToBaseFrom = rateFrom * amount;
                toTransAmount_ = convToBaseFrom / rateTo;
            } else {
                toTransAmount_ = amount;
            }
        }
    }

    wxString transNum = textNumber_->GetValue();

    textNotes_->SetFocus();
    wxString notes = textNotes_->GetValue();

    wxString status;
    wxStringClientData* status_obj = (wxStringClientData *)choiceStatus_->GetClientObject(choiceStatus_->GetSelection());
    if (status_obj) status = status_obj->GetData().Left(1);
    status.Replace(wxT("N"), wxT(""));

    wxString date1 = dpc_->GetValue().FormatISODate();

    boost::shared_ptr<mmBankTransaction> pTransaction;
    if (!edit_)
    {
        boost::shared_ptr<mmBankTransaction> pTemp(new mmBankTransaction(core_->db_));
        pTransaction = pTemp;
    }
    else
    {
        pTransaction = core_->bTransactionList_.getBankTransactionPtr(accountID_, pBankTransaction_->transactionID());
    }

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
    wxASSERT(pCurrencyPtr);

    pTransaction->accountID_ = fromAccountID;
    pTransaction->toAccountID_ = toAccountID;
    pTransaction->payee_ = core_->payeeList_.GetPayeeSharedPtr(payeeID_);
    pTransaction->transType_ = transaction_type;
    pTransaction->amt_ = amount;
    pTransaction->status_ = status;
    pTransaction->transNum_ = transNum;
    pTransaction->notes_ = notes.c_str();
    pTransaction->category_ = core_->categoryList_.GetCategorySharedPtr(categID_, subcategID_);
    pTransaction->date_ = dpc_->GetValue();
    pTransaction->toAmt_ = toTransAmount_;

    *pTransaction->splitEntries_.get() = *split_.get();
    pTransaction->updateAllData(core_, fromAccountID, pCurrencyPtr, true);

    if (!edit_)
    {
        transID_ = core_->bTransactionList_.addTransaction(core_, pTransaction);
    }
    else
    {
        core_->bTransactionList_.UpdateTransaction(pTransaction);
    }

    EndModal(wxID_OK);
}

void mmTransDialog::SetSplitState()
{
    if (split_->numEntries() > 0)
        cSplit_->Disable();
    else
    {
        cSplit_->Enable();
        cSplit_->SetValue(false);
        bCategory_->SetLabel(_("Select Category"));
    }
}

void mmTransDialog::OnSplitChecked(wxCommandEvent& /*event*/)
{
    /* Reset Category */
    categID_ = -1;
    subcategID_ = -1;
    split_ = boost::shared_ptr<mmSplitTransactionEntries>(new mmSplitTransactionEntries());

    if (cSplit_->GetValue())
    {
        bCategory_->SetLabel(_("Split Category"));
        textAmount_->Enable(false);
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(split_->getTotalSplits(), dispAmount);
        textAmount_->SetValue(dispAmount);
        activateSplitTransactionsDlg();
        SetSplitState();
    }
    else
    {
        bCategory_->SetLabel(_("Select Category"));
        textAmount_->Enable(true);
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(0.0, dispAmount);
        textAmount_->SetValue(dispAmount);
    }
}

//----------------------------------------------------------------------------

void mmTransDialog::changeFocus(wxChildFocusEvent& event)
{
    wxWindow *w = event.GetWindow();
    if ( w )
        oject_in_focus_ = w->GetId();

    if (!edit_ && textNotes_->GetValue() == notesTip_ && oject_in_focus_ == ID_DIALOG_TRANS_TEXTNOTES)
    {
        textNotes_->SetValue(wxT(""));
        textNotes_->SetForegroundColour(notesColour_);
    }
    event.Skip();
}

void mmTransDialog::OnCancel(wxCommandEvent& /*event*/)
{

    if (oject_in_focus_ == textNotes_->GetId())
        return;

    if (oject_in_focus_ == cbPayee_->GetId())
    {
        if (!cbPayee_->GetValue().IsEmpty()) {
            cbPayee_->SetValue(wxT(""));
            wxCommandEvent evt(wxEVT_COMMAND_TEXT_UPDATED, ID_DIALOG_TRANS_PAYEECOMBO);
            AddPendingEvent(evt);
            return;
        }
        else {
            itemButtonCancel_->SetFocus();
            return;
        }
    }

    if (oject_in_focus_ == textAmount_->GetId())
    {
        if (!textAmount_->IsEmpty()) {
            textAmount_->SetValue(wxT(""));
            return;
        }
        else {
            itemButtonCancel_->SetFocus();
            return;
        }
    }

    if ((int)oject_in_focus_ == (int)toTextAmount_->GetId())
    {
        if (!toTextAmount_->IsEmpty()) {
            toTextAmount_->SetValue(wxT(""));
            return;
        }
        else {
            itemButtonCancel_->SetFocus();
            return;
        }
    }

    if ((int)oject_in_focus_ == (int)textNumber_->GetId())
    {
        if (!textNumber_->IsEmpty()) {
            textNumber_->SetValue(wxT(""));
            return;
        }
        else {
            itemButtonCancel_->SetFocus();
            return;
        }
    }


    EndModal(wxID_CANCEL);
}

void mmTransDialog::OnQuit(wxCloseEvent& /*event*/)
{
        EndModal(wxID_CANCEL);
}

void mmTransDialog::onTextEntered(wxCommandEvent& event)
{
    double amount;
    wxString amountStr;
    if (oject_in_focus_ == textAmount_->GetId()) {
        amountStr = textAmount_->GetValue().Trim();
        if (mmex::formatCurrencyToDouble(amountStr, amount)) {
            mmex::formatDoubleToCurrencyEdit(amount,amountStr);
            textAmount_->SetValue(amountStr);
        }
    }
    else if (oject_in_focus_ == toTextAmount_->GetId()) {
        amountStr = toTextAmount_->GetValue().Trim();
        if (mmex::formatCurrencyToDouble(amountStr, amount)) {
            mmex::formatDoubleToCurrencyEdit(amount,amountStr);
            toTextAmount_->SetValue(amountStr);
        }
    }
    else if (oject_in_focus_ == textNumber_->GetId()) {
        textNotes_->SetFocus();
    }

    event.Skip();
}

void mmTransDialog::activateSplitTransactionsDlg()
{
    SplitTransactionDialog dlg(core_, split_.get(), transaction_type_->GetSelection(), this);
    if (dlg.ShowModal() == wxID_OK)
    {
        double amount = split_->getTotalSplits();
        if (transaction_type_->GetSelection() == DEF_TRANSFER && amount < 0)
            amount = - amount;
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(amount, dispAmount);
        textAmount_->SetValue(dispAmount);
    }
}

void mmTransDialog::OnButtonDateChar(wxKeyEvent& event)
{
    if (!wxGetKeyState(WXK_COMMAND) && !wxGetKeyState(WXK_ALT)) {
        int i;
        if (event.GetKeyCode() == WXK_DOWN) i=-1;
        else if (event.GetKeyCode() == WXK_UP) i=1;
        else if (event.GetKeyCode() == WXK_TAB) i=2;
        else return;

        if (i<2)
        {
            wxDateTime date = dpc_->GetValue();
            if (!wxGetKeyState(WXK_SHIFT))
                date = date.Add(wxDateSpan::Days(i));
            else
                date = date.Add(wxDateSpan::Months(i));

            dpc_->SetValue (date);
            //process date change event for set weekday name
            wxDateEvent dateEvent(dpc_, date, wxEVT_DATE_CHANGED);
            GetEventHandler()->ProcessEvent(dateEvent);
        }

        event.Skip();
    }
}

void mmTransDialog::onChoiceTransChar(wxKeyEvent& event)
{
    int i = transaction_type_->GetSelection();
    if (event.GetKeyCode()==WXK_DOWN)
    {
        if (i < (core_->accountList_.getNumBankAccounts() > 1 ? DEF_TRANSFER : DEF_DEPOSIT))
        {
            transaction_type_->SetSelection(++i);
        }
    }
    else if (event.GetKeyCode()==WXK_UP)
    {
        if (i > DEF_WITHDRAWAL)
        {
            transaction_type_->SetSelection(--i);
        }
    }
    else
    {
        event.Skip();
    }
    updateControlsForTransType();
}

void mmTransDialog::SetDialogToDuplicateTransaction()
{
    // we want the dialog to treat the transaction as a new transaction.
    edit_ = false;
    this->SetTitle(_("Duplicate Transaction"));

   // we need to create a new pointer for Split transactions.
   boost::shared_ptr<mmSplitTransactionEntries> splitTransEntries(new mmSplitTransactionEntries());
   core_->bTransactionList_.getBankTransactionPtr(accountID_, pBankTransaction_->transactionID())->getSplitTransactions(core_, splitTransEntries.get());
   split_.get()->entries_ = splitTransEntries->entries_;
}

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

#include "transdialog.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include "categdialog.h"
#include "payeedialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "splittransactionsdialog.h"
#include "paths.h"
#include <wx/event.h>
#include <wx/choice.h>

#include <sstream>
#define _MM_EX_TRANSDIALOG_CPP_REVISION_ID    "$Revision$"

// Defines for Transaction: (Status and Type) now located in dbWrapper.h

enum 
{ 
    ID_DIALOG_TRANS_SPINNER = wxID_HIGHEST + 1,
    ID_DIALOG_TRANS_WEEK, 
    NOTES_MENU_NUMBER = 20 
};
         
IMPLEMENT_DYNAMIC_CLASS( mmTransDialog, wxDialog )

BEGIN_EVENT_TABLE( mmTransDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmTransDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmTransDialog::OnCancel)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmTransDialog::OnCategs)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONPAYEE, mmTransDialog::OnPayee)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTO, mmTransDialog::OnTo)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmTransDialog::OnTransTypeChanged)  
    EVT_CHECKBOX(ID_DIALOG_TRANS_ADVANCED_CHECKBOX, mmTransDialog::OnAdvanceChecked) 
    EVT_CHECKBOX(ID_DIALOG_TRANS_SPLITCHECKBOX, mmTransDialog::OnSplitChecked)
    EVT_MENU (wxID_ANY, mmTransDialog::onNoteSelected)
    EVT_CHILD_FOCUS(mmTransDialog::changeFocus)
    EVT_SPIN_UP(ID_DIALOG_TRANS_SPINNER,mmTransDialog::OnSpinUp)
    EVT_SPIN_DOWN(ID_DIALOG_TRANS_SPINNER,mmTransDialog::OnSpinDown)
    EVT_DATE_CHANGED(ID_DIALOG_TRANS_BUTTONDATE, mmTransDialog::OnDateChanged)
END_EVENT_TABLE()

mmTransDialog::mmTransDialog(
    boost::shared_ptr<wxSQLite3Database> db, 
    mmCoreDB* core,
    int accountID, int transID, bool edit, 
    wxWindow* parent, wxWindowID id, 
    const wxString& caption, const wxPoint& pos, 
    const wxSize& size, long style 
) :
    db_(db),
    core_(core),
    transID_(transID),
    accountID_(accountID),
    referenceAccountID_(accountID),
    payeeUnknown_(true),
    categUpdated_(false),
    edit_(edit),
    categID_(-1),
    subcategID_(-1),
    payeeID_(-1),
    toID_(-1),
    toTransAmount_(-1),
    advancedToTransAmountSet_(false),
    edit_currency_rate(1.0)
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

    if (edit_) dataToControls();
    
    Centre();
    Fit();

    return TRUE;
}

void mmTransDialog::dataToControls()
{
    static const char sql[] = 
    "select ca.CATEGID, cat.CATEGNAME, "
           "ca.SUBCATEGID, sc.SUBCATEGNAME, "
           "ca.ACCOUNTID, al.ACCOUNTNAME, "
           "ca.TRANSDATE, "
           "ca.TRANSACTIONNUMBER, "
           "ca.STATUS, "
           "ca.NOTES, "
           "ca.TRANSCODE, "
           "ca.TRANSAMOUNT, "
           "ca.TOTRANSAMOUNT, "
           "ca.PAYEEID, "
           "ca.TOACCOUNTID, al2.ACCOUNTNAME as TOACCOUNTNAME, "
           "p.PAYEENAME "

    "from CHECKINGACCOUNT_V1 ca "

    "join ACCOUNTLIST_V1 al "
    "on al.ACCOUNTID = ca.ACCOUNTID "

    "left join ACCOUNTLIST_V1 al2 "
    "on al2.ACCOUNTID = ca.TOACCOUNTID "

    "left join PAYEE_V1 p "
    "on p.PAYEEID = ca.PAYEEID "

    "left join CATEGORY_V1 cat "
    "on cat.CATEGID = ca.CATEGID "

    "left join SUBCATEGORY_V1 sc "
    "on sc.CATEGID = ca.CATEGID and "
       "sc.SUBCATEGID = ca.SUBCATEGID "

    "where ca.TRANSID = ?";

    //choiceTrans_->Disable();

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, transID_);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        categID_ = q1.GetInt("CATEGID");
        subcategID_ = q1.GetInt("SUBCATEGID");
        accountID_ = q1.GetInt("ACCOUNTID");
       
        wxDateTime trx_date_ = mmGetStorageStringAsDate(q1.GetString("TRANSDATE"));
        wxString dt = mmGetDateForDisplay(db_.get(), trx_date_);
        dpc_->SetValue(trx_date_);

        wxString dateStr = mmGetShortWeekDayName(trx_date_.GetWeekDay());
        itemStaticTextWeek_->SetLabel(dateStr);

        wxString transNumString = q1.GetString("TRANSACTIONNUMBER");
        wxString statusString  = q1.GetString("STATUS");
        if (statusString == "") statusString="N"; 
        wxString notesString  = q1.GetString("NOTES");
        wxString transTypeString = q1.GetString("TRANSCODE");
        double transAmount = q1.GetDouble("TRANSAMOUNT");
        toTransAmount_ = q1.GetDouble("TOTRANSAMOUNT");

        // backup the original currency rate first
        if (transAmount > 0.0) {
            edit_currency_rate = toTransAmount_ / transAmount;
        }        
      
        choiceStatus_->SetSelection(wxString("NRVFD").Find(statusString));
        choiceTrans_->SetStringSelection(wxGetTranslation(transTypeString));
        updateControlsForTransType();

        payeeID_ = q1.GetInt("PAYEEID");
        toID_ = q1.GetInt("TOACCOUNTID");
        payeeUnknown_ = false;

        *split_.get() = *core_->bTransactionList_.getBankTransactionPtr(transID_)->splitEntries_.get();

        if (split_->numEntries() > 0)
        {
            bCategory_->SetLabel(_("Split Category"));
            cSplit_->SetValue(true);
            cSplit_->Disable();
        }
        else
        {
            wxString categString = q1.GetString("CATEGNAME");
            categoryName_ = categString;
            if (subcategID_ != -1)
            {
                subCategoryName_ = q1.GetString("SUBCATEGNAME");
                categString += " : ";
                categString += subCategoryName_;
            }
            bCategory_->SetLabel(categString);
        }

        textNotes_->SetValue(notesString);
        textNumber_->SetValue(transNumString);

       
        if (split_->numEntries() > 0)
        {
            transAmount = split_->getTotalSplits();
            textAmount_->Enable(false);
        }
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(transAmount, dispAmount);
        textAmount_->SetValue(dispAmount);

        bPayee_->SetLabel(q1.GetString("PAYEENAME"));

        if (transTypeString == TRANS_TYPE_TRANSFER_STR)
        {
            bPayee_->SetLabel(q1.GetString("ACCOUNTNAME"));
            bTo_->SetLabel(q1.GetString("TOACCOUNTNAME"));
            payeeID_ = accountID_;   

            // When editing an advanced transaction record, we do not reset the toTransAmount_
            if (edit_ && (toTransAmount_ != transAmount))
            {
                cAdvanced_->SetValue(true);
                SetAdvancedTransferControls(true);
            }
        }
    }
    
    st.Finalize();
}

void mmTransDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);
    
    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(this, wxID_ANY, _("Transaction Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxTOP|wxRIGHT, 10);

    /************************************************************************************************************
    ItemPanel7 controlled by ItemFlexGridSizer8 - contained in the TransDetailsStaticSizer. (itemStaticBoxSizer4)
    *************************************************************************************************************/
    wxPanel* itemPanel7 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel7, 0, wxGROW|wxALL, 10);

    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(0, 2, 10, 10);
    itemPanel7->SetSizer(itemFlexGridSizer8);

    wxSizerFlags flags(1);
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 0);

    // Date --------------------------------------------
    wxStaticText* date_text = new wxStaticText(itemPanel7, wxID_STATIC, _("Date"));
    //Text field for day of the week
    itemStaticTextWeek_ = new wxStaticText(itemPanel7, ID_DIALOG_TRANS_WEEK, "");
    
    wxDateTime trx_date = mmGetStorageStringAsDate(getLastTrxDate());
    dpc_ = new wxDatePickerCtrl( itemPanel7, ID_DIALOG_TRANS_BUTTONDATE, trx_date,
                                 wxDefaultPosition, wxSize(130, -1),
                                 wxDP_DROPDOWN|wxDP_SHOWCENTURY|wxDP_ALLOWNONE);
    dpc_->SetToolTip(_("Specify the date of the transaction"));

    // Display the day of the week
    itemStaticTextWeek_->SetLabel(mmGetShortWeekDayName(trx_date.GetWeekDay()));

// change properties depending on system parameters
    wxSize spinCtrlSize = wxSize(18,22);
    int spinCtrlDirection = wxSP_VERTICAL;
    int interval = 4;
#ifdef __WXGTK__
    spinCtrlSize = wxSize(16,-1);
    interval = 0;
    dpc_->Connect(ID_DIALOG_TRANS_BUTTONDATE, wxEVT_KEY_DOWN, wxKeyEventHandler(mmTransDialog::OnButtonDateChar), NULL, this);
    dpc_->Connect(ID_DIALOG_TRANS_BUTTONDATE, wxEVT_KILL_FOCUS, wxFocusEventHandler(mmTransDialog::OnKillDateFocus), NULL, this);
#endif

    spinCtrl_ = new wxSpinButton(itemPanel7,
        ID_DIALOG_TRANS_SPINNER,wxDefaultPosition,spinCtrlSize,spinCtrlDirection|wxSP_ARROW_KEYS|wxSP_WRAP);
    spinCtrl_ -> SetRange (-32768, 32768); 
    spinCtrl_->SetToolTip(_("Retard or advance the date of the transaction"));

    itemFlexGridSizer8->Add(date_text, flags);
    wxBoxSizer* itemBoxSizer118 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer8->Add(itemBoxSizer118);
    itemBoxSizer118->Add(dpc_);
    itemBoxSizer118->Add(spinCtrl_, 0, wxLEFT, interval);   
    itemBoxSizer118->Add(itemStaticTextWeek_, 0, wxLEFT, 10);   

    // Status --------------------------------------------
    wxStaticText* status_text = new wxStaticText(itemPanel7, wxID_STATIC, _("Status"));
    choiceStatus_ = new wxChoice(itemPanel7, ID_DIALOG_TRANS_STATUS);
    wxString transaction_status[] = 
    {
        wxTRANSLATE("None"),
        wxTRANSLATE("Reconciled"),
        wxTRANSLATE("Void"),
        wxTRANSLATE("Follow up"),
        wxTRANSLATE("Duplicate")
    };
    for(size_t i = 0; i < sizeof(transaction_status)/sizeof(wxString); ++i)
        choiceStatus_->Append(wxGetTranslation(transaction_status[i]),
        new wxStringClientData(transaction_status[i]));
    choiceStatus_->SetStringSelection(wxGetTranslation(mmIniOptions::instance().transStatusReconciled_));

    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));
    
    itemFlexGridSizer8->Add(status_text, flags);
    itemFlexGridSizer8->Add(choiceStatus_);

    // Type --------------------------------------------
    wxStaticText* type_text = new wxStaticText(itemPanel7,
        wxID_STATIC, _("Type"));
    
    choiceTrans_ = new wxChoice(itemPanel7,
        ID_DIALOG_TRANS_TYPE, wxDefaultPosition, wxSize(110, -1));
    wxString transaction_type[] =
    {
        wxTRANSLATE("Withdrawal"),
        wxTRANSLATE("Deposit"),
        wxTRANSLATE("Transfer")
    };
    // Restrict choise if accounts number less than 2
    size_t size = sizeof(transaction_type)/sizeof(wxString);
    if (core_->getNumBankAccounts() < 2) size--;
    for(size_t i = 0; i < size; ++i)
    choiceTrans_->Append(wxGetTranslation(transaction_type[i]),
        new wxStringClientData(transaction_type[i]));

    choiceTrans_->SetSelection(0);
    choiceTrans_->SetToolTip(_("Specify the type of transactions to be created."));

    cAdvanced_ = new wxCheckBox(itemPanel7,
        ID_DIALOG_TRANS_ADVANCED_CHECKBOX, _("Advanced"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cAdvanced_->SetValue(FALSE);
    cAdvanced_->SetToolTip(_("Allows the setting of different amounts in the FROM and TO accounts."));

    wxBoxSizer* typeSizer = new wxBoxSizer(wxHORIZONTAL);
    typeSizer->Add(choiceTrans_);
    typeSizer->Add(cAdvanced_, 0, wxALL, 5);
    
    itemFlexGridSizer8->Add(type_text, flags);
    itemFlexGridSizer8->Add(typeSizer);

    // Amount Fields --------------------------------------------
    amountNormalTip_   = _("Specify the amount for this transaction");
    amountTransferTip_ = _("Specify the amount to be transfered"); 

    wxStaticText* amountStaticText = new wxStaticText( itemPanel7, wxID_STATIC, _("Amount"));

    textAmount_ = new wxTextCtrl( itemPanel7,
        ID_DIALOG_TRANS_TEXTAMOUNT, "", wxDefaultPosition, wxSize(110, -1), 
        wxALIGN_RIGHT, wxFloatingPointValidator<float>(2) );
    textAmount_->SetToolTip(amountNormalTip_);

    toTextAmount_ = new wxTextCtrl( itemPanel7,
        ID_DIALOG_TRANS_TOTEXTAMOUNT, "", wxDefaultPosition, wxSize(110, -1), 
        wxALIGN_RIGHT, wxFloatingPointValidator<float>(2) );
    toTextAmount_->SetToolTip(_("Specify the transfer amount in the To Account"));

    wxBoxSizer* amountSizer = new wxBoxSizer(wxHORIZONTAL);
    amountSizer->Add(textAmount_);
    amountSizer->Add(toTextAmount_, 0, wxLEFT, 5);

    itemFlexGridSizer8->Add(amountStaticText, flags);
    itemFlexGridSizer8->Add(amountSizer);

    // Payee ---------------------------------------------
    wxStaticText* itemStaticText9 = new wxStaticText(itemPanel7, ID_DIALOG_TRANS_STATIC_PAYEE, _("Payee"));

    wxString payeeName = resetPayeeString();
    wxString categString;

    if (!edit_)
    {
        //If user does not want payee to be auto filled for the new transaction
        if ( mmIniOptions::instance().transPayeeSelectionNone_ == 0)
        {
            payeeName = resetPayeeString();
            payeeUnknown_ = true;
        }
        else // determine the payee for this account
        {
            payeeName = getMostFrequentlyUsedPayee(categString);
            payeeUnknown_ = false;
        }

        if ( mmIniOptions::instance().transCategorySelectionNone_== 0 || categString.IsEmpty() )
            categString = resetCategoryString();
    }

    bPayee_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONPAYEE, payeeName, wxDefaultPosition, wxSize(225, -1));
    payeeWithdrawalTip_ = _("Specify where the transaction is going to");
    payeeDepositTip_    = _("Specify where the transaction is coming from");
    bPayee_->SetToolTip(payeeWithdrawalTip_);
    bPayee_->Connect(wxEVT_CHAR, wxKeyEventHandler(mmTransDialog::OnButtonPayeeChar), NULL, this);
    bPayee_->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(mmTransDialog::OnButtonPayeeMouse), NULL, this);

    itemFlexGridSizer8->Add(itemStaticText9, flags);
    itemFlexGridSizer8->Add(bPayee_);

    // Payee Alternate ------------------------------------------------
    wxStaticText* itemStaticText13 = new wxStaticText(itemPanel7,
        ID_DIALOG_TRANS_STATIC_FROM, " ");
    bTo_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONTO,
        _("Select To Acct"), wxDefaultPosition, wxSize(225, -1), 0 );
    bTo_->SetToolTip(_("Specify which account the transfer is going to"));
    bTo_->Connect(wxEVT_CHAR, wxKeyEventHandler(mmTransDialog::OnButtonToAccountChar), NULL, this);
    bTo_->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(mmTransDialog::OnButtonToAccountMouse), NULL, this);
    
    itemFlexGridSizer8->Add(itemStaticText13);
    itemFlexGridSizer8->Add(bTo_);

    // Split Category -------------------------------------------
    cSplit_ = new wxCheckBox(itemPanel7, ID_DIALOG_TRANS_SPLITCHECKBOX,
        _("Split"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cSplit_->SetValue(FALSE);
    cSplit_->SetToolTip(_("Use split Categories"));

    itemFlexGridSizer8->AddSpacer(20);  // Fill empty space.
    itemFlexGridSizer8->Add(cSplit_);

    // Category -------------------------------------------------
    wxStaticText* itemStaticText17 = new wxStaticText(itemPanel7, wxID_STATIC, _("Category"));
    bCategory_ = new wxButton(itemPanel7, ID_DIALOG_TRANS_BUTTONCATEGS,
        categString, wxDefaultPosition, wxSize(225, -1));
    bCategory_->SetToolTip(_("Specify the category for this transaction"));  
    
    itemFlexGridSizer8->Add(itemStaticText17, flags);
    itemFlexGridSizer8->Add(bCategory_);
    
    // Number  ---------------------------------------------
    wxStaticText* itemStaticText11 = new wxStaticText(itemPanel7, wxID_STATIC, _("Number"));
    textNumber_ = new wxTextCtrl(itemPanel7,
        ID_DIALOG_TRANS_TEXTNUMBER, "", wxDefaultPosition, wxSize(185, -1), wxTE_PROCESS_ENTER);
    textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));

    bAuto_ = new wxButton(itemPanel7,
        ID_DIALOG_TRANS_BUTTONTRANSNUM, "...", wxDefaultPosition, wxSize(40, -1), 0 );
    bAuto_->SetToolTip(_("Populate Transaction #"));
    bAuto_ -> Connect(ID_DIALOG_TRANS_BUTTONTRANSNUM,
        wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mmTransDialog::OnAutoTransNum), NULL, this);

    itemFlexGridSizer8->Add(itemStaticText11, flags);
    wxBoxSizer* itemBoxSizer550 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer8->Add(itemBoxSizer550, 0, wxGROW);
    itemBoxSizer550->Add(textNumber_, flags);
    itemBoxSizer550->Add(bAuto_);

    // Notes  ---------------------------------------------
    wxStaticText* notesStaticText = new wxStaticText(itemPanel7,
        wxID_STATIC, _("Notes"));

    textNotes_ = new wxTextCtrl(itemPanel7,
        ID_DIALOG_TRANS_TEXTNOTES, "", wxDefaultPosition, wxSize(225,80), wxTE_MULTILINE);
    textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));

    bFrequentUsedNotes_ = new wxButton(itemPanel7,
        ID_DIALOG_TRANS_BUTTON_FREQENTNOTES, ">>", wxDefaultPosition, wxSize(40, -1), 0);
    bFrequentUsedNotes_->SetToolTip(_("Select one of the frequently used notes"));
    bFrequentUsedNotes_ -> Connect(ID_DIALOG_TRANS_BUTTON_FREQENTNOTES,
        wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mmTransDialog::OnFrequentUsedNotes), NULL, this);

    wxBoxSizer* itemBoxSizer56 = new wxBoxSizer(wxVERTICAL);

    itemFlexGridSizer8->Add(itemBoxSizer56, 0, wxGROW);
    itemBoxSizer56->Add(notesStaticText, 0, wxALIGN_TOP);
    itemBoxSizer56->Add(bFrequentUsedNotes_, 0, wxALIGN_RIGHT|wxALIGN_TOP|wxALL|wxADJUST_MINSIZE, 10);
    itemFlexGridSizer8->Add(textNotes_);

    SetTransferControls();  // hide appropriate fields
    /**********************************************************************************************
     Button Panel with OK and Cancel Buttons
    ***********************************************************************************************/
    wxPanel* itemPanel25 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer2->Add(itemPanel25, 0, wxALIGN_RIGHT|wxALL, 10);

    wxStdDialogButtonSizer*  itemStdDialogButtonSizer1 = new wxStdDialogButtonSizer;
    itemPanel25->SetSizer(itemStdDialogButtonSizer1);

    wxButton* itemButtonOK = new wxButton( itemPanel25, wxID_OK);
    itemStdDialogButtonSizer1->Add(itemButtonOK, 0, wxRIGHT, 10);

    wxButton* itemButtonCancel = new wxButton( itemPanel25, wxID_CANCEL);
    itemStdDialogButtonSizer1->Add(itemButtonCancel);

    if (edit_)
        itemButtonCancel->SetFocus();

    itemStdDialogButtonSizer1->Realize();
    //If nothing in focus Alt+C should close the window
    richText_ = false;

}

void mmTransDialog::OnPayee(wxCommandEvent& /*event*/)
{
    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
    {

        wxArrayString as = core_->getAccountsName();

        wxSingleChoiceDialog scd(0, _("Account name"), _("Select Account"), as);
        if (scd.ShowModal() == wxID_OK)
        {
            wxString acctName = scd.GetStringSelection();
            payeeID_ = core_->getAccountID(acctName);
            bPayee_->SetLabel(acctName);
        }
    }
    else
    {
        mmPayeeDialog dlg(this, core_);

        if ( dlg.ShowModal() == wxID_OK )
        {
            payeeID_ = dlg.getPayeeId();
            if (payeeID_ == -1)
            {
                bPayee_->SetLabel(resetPayeeString());
                payeeUnknown_ = true;
                return;
            }

            int tempCategID = -1;
            int tempSubCategID = -1;
            wxString payeeName = mmDBWrapper::getPayee(db_.get(), payeeID_, tempCategID, tempSubCategID);
            bPayee_->SetLabel(mmReadyDisplayString(payeeName));
            payeeUnknown_ = false;

            // If this is a Split Transaction, ignore displaying last category for payee
            if (split_->numEntries())
                return;

            // Only for new transactions: if user want to autofill last category used for payee.
            if ( mmIniOptions::instance().transCategorySelectionNone_ == 1 && ( !edit_ && !categUpdated_ ) )
            {
                // if payee has memory of last category used then display last category for payee
                if (tempCategID != -1)
                {
                    wxString categString = core_->getCategoryName(tempCategID);
                    categoryName_ = categString;
                    if (tempSubCategID != -1)
                    {
                        wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(), tempCategID, tempSubCategID);
                        subCategoryName_ = subcatName; 
                        categString += " : ";
                        categString += subcatName;
                    }

                    categID_ = tempCategID;
                    subcategID_ = tempSubCategID;
                    bCategory_->SetLabel(categString);
                }
            }
        }
    }
}

void mmTransDialog::OnAutoTransNum(wxCommandEvent& /*event*/)
{
    long mID = mmDBWrapper::getTransactionNumber(db_.get(), accountID_);
    wxString wxIDstr = wxString::Format("%ld", mID);
    textNumber_->SetValue( wxIDstr );
}

void mmTransDialog::OnTo(wxCommandEvent& /*event*/)
{
    // This should only get called if we are in a transfer

    wxArrayString as = core_->getAccountsName(payeeID_);
    if (as.IsEmpty()){
        return;
    }
    wxSingleChoiceDialog scd(this, _("Account name"), _("Select Account"), as);
    if (scd.ShowModal() == wxID_OK)
    {
        wxString acctName = scd.GetStringSelection();
        toID_ = core_->getAccountID(acctName);
        bTo_->SetLabel(acctName);
    }
}

void mmTransDialog::OnSpinUp(wxSpinEvent& event)
{
    wxDateTime date = dpc_->GetValue();
    date = date.Add(wxDateSpan::Days(1));
    dpc_->SetValue (date);
    
    itemStaticTextWeek_->SetLabel(mmGetShortWeekDayName(date.GetWeekDay()));

    event.Skip();
}

void mmTransDialog::OnSpinDown(wxSpinEvent& event)
{
    wxDateTime date = dpc_->GetValue();
    date = date.Add(wxDateSpan::Days(-1));
    dpc_->SetValue (date);

    itemStaticTextWeek_->SetLabel(mmGetShortWeekDayName(date.GetWeekDay()));

    event.Skip();
}

void mmTransDialog::OnDateChanged(wxDateEvent& event)
{
    wxString dateStr = "";
    if (event.GetDate().IsValid())
        dateStr = mmGetShortWeekDayName(dpc_->GetValue().GetWeekDay());
    itemStaticTextWeek_->SetLabel(dateStr);
    event.Skip();
}

void mmTransDialog::OnAdvanceChecked(wxCommandEvent& /*event*/)
{
    if (cAdvanced_->IsChecked()) {
        SetAdvancedTransferControls(true);
    } else {
        SetAdvancedTransferControls();
        textAmount_->SetToolTip(amountTransferTip_);
    }
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
        mmCategDialog dlg(core_, this);
        dlg.setTreeSelection(categoryName_, subCategoryName_);
        if ( dlg.ShowModal() == wxID_OK )
        {
            if (dlg.categID_ == -1)
            {
                // check if categ and subcateg are now invalid
                wxString catName = core_->getCategoryName(categID_);
                if ( catName.IsEmpty())
                {
                    // cannot find category
                    categID_ = -1;
                    subcategID_ = -1;
                    bCategory_->SetLabel(_("Select Category"));
                    return;
                }

                if (dlg.subcategID_ != -1)
                {
                    wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(),
                        categID_, subcategID_);
                    if (subcatName.IsEmpty())
                    {
                        subcategID_ = -1;
                        bCategory_->SetLabel(catName);
                        return;
                    }
                }
                else
                {
                    catName.Replace("&", "&&");
                    bCategory_->SetLabel(catName);
                }

                return;
            }

            categID_ = dlg.categID_;
            subcategID_ = dlg.subcategID_;
            categUpdated_ = true;

            wxString catName = core_->getCategoryName(dlg.categID_);
            categoryName_ = catName;
            catName.Replace ("&", "&&");
            wxString categString = catName;

            if (dlg.subcategID_ != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(), dlg.categID_, dlg.subcategID_);
                subCategoryName_ = subcatName;
                subcatName.Replace ("&", "&&");
                categString += " : ";
                categString += subcatName;
            } 
            else 
            {
                subCategoryName_.Empty(); 
            }

            bCategory_->SetLabel(categString);
        }
    }
}

void mmTransDialog::OnTransTypeChanged(wxCommandEvent& /*event*/)
{
    updateControlsForTransType();
}

void mmTransDialog::updateControlsForTransType()
{
    wxStaticText* stp = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_PAYEE);
    wxStaticText* itemStaticText13 = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_FROM);

    if (choiceTrans_->GetSelection() == DEF_WITHDRAWAL) 
    {
        displayControlsToolTips(DEF_WITHDRAWAL);
        SetTransferControls();
        stp->SetLabel(_("Payee"));
        bPayee_->SetToolTip(payeeWithdrawalTip_);
        itemStaticText13->SetLabel("");
        if (payeeUnknown_) 
            bPayee_->SetLabel(resetPayeeString());
        bPayee_->Enable(true);

    } 
    else if (choiceTrans_->GetSelection() == DEF_DEPOSIT) 
    {
        displayControlsToolTips(DEF_DEPOSIT);
        SetTransferControls();
        stp->SetLabel(_("From"));
        bPayee_->SetToolTip(payeeDepositTip_);
        itemStaticText13->SetLabel("");
        if (payeeUnknown_) 
            bPayee_->SetLabel(resetPayeeString());
        bPayee_->Enable(true);

    } 
    else if (choiceTrans_->GetSelection() == DEF_TRANSFER) 
    {
        displayControlsToolTips(DEF_TRANSFER, true);
        SetTransferControls(true);
        if (cAdvanced_->IsChecked())
            SetAdvancedTransferControls(true);

        stp->SetLabel(_("From"));
        bTo_->SetLabel(_("Select To Account"));
        itemStaticText13->SetLabel(_("To"));
        toID_    = -1;
        payeeUnknown_ = true;

        wxString acctName = core_->getAccountName(accountID_);
        bPayee_->SetLabel(acctName);
        payeeID_ = accountID_;
        //from account button should be available for editing
        bPayee_->Enable(edit_);

        if (!edit_)
        {
            wxString categString;
            if (mmIniOptions::instance().transCategorySelectionNone_ == 0) // Default Category = "None"
                categString = resetCategoryString();
            else
                categString = getMostFrequentlyUsedCategory();

            wxStaticText* stc = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_BUTTONCATEGS);
            stc->SetLabel(categString);
        }
    }
}

wxString mmTransDialog::resetPayeeString(bool normal) //normal is deposits or withdrawls
{
    wxString payeeStr = _("Select Payee");
    payeeID_ = -1;
    wxArrayString filtd;
    filtd = mmDBWrapper::filterPayees(db_.get(), "");
    if (filtd.Count() == 1)
    {
        //only one payee present. Choose it
        payeeStr = filtd[0];
        payeeID_ = core_->getPayeeID(payeeStr);
    }

    if (normal) toID_ = -1;

    return payeeStr;
}

wxString mmTransDialog::resetCategoryString()
{
    wxString catStr = _("Select Category");
    categID_ = -1;
    subcategID_ = -1;

    return catStr; 
}

wxString mmTransDialog::getMostFrequentlyUsedCategory()
{
    // Determine most frequently used category name for current account for transfer
    static const char sql[] = 
        "select count (*) c,"
              " cat.categname CATEGNAME,"
              " sc.subcategname SUBCATEGNAME,"
              " ca.categid, ca.subcategid "
        "from CHECKINGACCOUNT_V1 ca "
            " left join CATEGORY_V1 cat on cat.CATEGID = ca.CATEGID"
            " left join SUBCATEGORY_V1 sc on sc.CATEGID = ca.CATEGID and sc.SUBCATEGID = ca.SUBCATEGID "
        "where ca.transcode = 'Transfer' and ca.accountid = ? or ca.toaccountid = ? "
        "group by ca.categid, ca.subcategid "
        "order by c desc "
        "limit 1";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, accountID_);
    st.Bind(2, accountID_);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    wxString categString = q1.GetString("CATEGNAME");
    wxString subcategName = q1.GetString("SUBCATEGNAME");
    categID_ = q1.GetInt("CATEGID");
    subcategID_ = q1.GetInt("SUBCATEGID");
    st.Finalize();

    //if some values is missing - set defaults
    if (categString.IsEmpty())
    {
        categString = resetCategoryString();
    }
    else 
    {
        categoryName_ = categString;
        if ( !subcategName.IsEmpty() )
        {
            categString += " : ";
            categString += subcategName;
            subCategoryName_ = subcategName;
        }
    }

    return categString;
}

wxString mmTransDialog::getMostFrequentlyUsedPayee(wxString& categString)
{
    // Determine most frequently used payee name for current account
    static const char sql[] = 
        "select count (*) c, "
              " cat.categname CATEGNAME, "
              " sc.subcategname SUBCATEGNAME, "
              " ca.categid, ca.subcategid, "
              " ca.payeeid, p.payeename PAYEENAME "
        "from CHECKINGACCOUNT_V1 ca, payee_v1 p "
            " left join CATEGORY_V1 cat on cat.CATEGID = ca.CATEGID "
            " left join SUBCATEGORY_V1 sc on sc.CATEGID = ca.CATEGID and sc.SUBCATEGID = ca.SUBCATEGID "
        "where ca.payeeid = p.payeeid and "
             " ca.transcode<>'Transfer' and ca.accountid = ? "
        "group by ca.payeeid, ca.categid, ca.subcategid "
        "order by c desc "
        "limit 1";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, accountID_);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    wxString payeeName = q1.GetString("PAYEENAME");
    payeeID_ = q1.GetInt("PAYEEID");

    // get category info
    categString = q1.GetString("CATEGNAME");
    wxString subcategName = q1.GetString("SUBCATEGNAME");
    categID_    = q1.GetInt("CATEGID");
    subcategID_ = q1.GetInt("SUBCATEGID");
    st.Finalize();

    //if some values is missing - set defaults
    if (payeeName.IsEmpty())
    {
        payeeName = resetPayeeString(false);
    } 

    if ( categString.IsEmpty() )
    {
        categString = resetCategoryString();
    } 
    else 
    {
        categoryName_ = categString;
        if (!subcategName.IsEmpty())
        {
            categString += " : ";
            categString += subcategName;
            subCategoryName_ = subcategName;
        }
    }

    return payeeName;
}

wxString mmTransDialog::getLastTrxDate()
{
    // Determine date for last transaction for current account
    wxString last_trx_date_;
    try {
    static const char sql[] = 
        "select max(ca.transdate) as LASTDATE "
        "from CHECKINGACCOUNT_V1 ca "
        "where ca.transdate <= date ('now','localtime') "
        "and ca.accountid = ? ";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, accountID_);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    last_trx_date_ = q1.GetString("LASTDATE");

    st.Finalize();
    } catch(const wxSQLite3Exception& e) {
        wxASSERT(false);
        wxLogDebug("Database::select some data from CHECKINGACCOUNT_V1: Exception", e.GetMessage().c_str());
        wxLogError("Select some data from CHECKINGACCOUNT_V1 Exception " + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }

    //if some values is missing or options parameter seted to zero - set defaults
    if (last_trx_date_.IsEmpty() || mmIniOptions::instance().transDateDefault_==0)
        last_trx_date_ = wxDateTime::Now().FormatISODate();

    return last_trx_date_;
}

void mmTransDialog::displayControlsToolTips(int transType, bool enableAdvanced /* = false */)
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_FROM);
    st->Enable(enableAdvanced);
    bTo_->Enable(enableAdvanced);
    if (transType == DEF_TRANSFER) {
        bPayee_->SetToolTip(_("Specify which account the transfer is comming from"));
        textAmount_->SetToolTip(amountTransferTip_);
    } else {
        bPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));
        textAmount_->SetToolTip(amountNormalTip_);
    }
}

void mmTransDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString transCode = "";
    int tCode = choiceTrans_->GetSelection();
    if (tCode == DEF_WITHDRAWAL)
        transCode = TRANS_TYPE_WITHDRAWAL_STR;
    else if (tCode == DEF_DEPOSIT)
        transCode = TRANS_TYPE_DEPOSIT_STR;
    else if (tCode == DEF_TRANSFER)
        transCode = TRANS_TYPE_TRANSFER_STR;

    if (payeeID_ == -1)
    {
        if (transCode != TRANS_TYPE_TRANSFER_STR)
            mmShowErrorMessageInvalid(this, _("Payee"));
        else
            mmShowErrorMessageInvalid(this, _("From Account "));
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
            if (choiceTrans_->GetSelection() == DEF_TRANSFER) {
                if (amount < 0)
                    amount = - amount;
            } else {
                mmShowErrorMessageInvalid(this, _("Amount"));
                return;
            }
        }
    }
    else
    {
        wxString amountStr = textAmount_->GetValue().Trim();
        if (!mmex::formatCurrencyToDouble(amountStr, amount) || (amount < 0.0))
        {
            textAmount_->SetBackgroundColour("RED");
            mmShowErrorMessageInvalid(this, _("Amount"));
            textAmount_->SetBackgroundColour(wxNullColour);
            return;
        }
    }

    //if (advancedToTransAmountSet_)
    {
        wxString amountStr = toTextAmount_->GetValue().Trim();
        if ((!mmex::formatCurrencyToDouble(amountStr, toTransAmount_) || (toTransAmount_ < 0.0)) && !amountStr.IsEmpty())
        {
            toTextAmount_->Enable(!advancedToTransAmountSet_);
            toTextAmount_->SetBackgroundColour("RED");
            mmShowErrorMessageInvalid(this, _("Advanced Amount"));
            toTextAmount_->SetBackgroundColour(wxNullColour);
            toTextAmount_->Enable(advancedToTransAmountSet_);
            return;
        }
    } //else
    if (!advancedToTransAmountSet_)
        toTransAmount_ = amount;

    int toAccountID = -1;
    int fromAccountID = accountID_;
    if (transCode == TRANS_TYPE_TRANSFER_STR)
    {
        if (toID_ == -1)
        {
            mmShowErrorMessageInvalid(this, _("To Account "));
            return;
        }

        if (payeeID_ == toID_)
        {
            mmShowErrorMessage(this, _("From and To Account cannot be the same."), _("Error"));
            return;
        }

        fromAccountID = payeeID_;
        toAccountID = toID_;
        payeeID_ = -1;
    }
    else
    {
        int catID, subcatID;
        wxString payeeName = mmDBWrapper::getPayee(db_.get(), payeeID_, catID, subcatID );
        // save the category used for this payee to allow automatic category fill at user request.
        mmDBWrapper::updatePayee(db_.get(), payeeName, payeeID_, categID_, subcategID_);

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
                double rateFrom = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), fromAccountID);
                double rateTo = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), toAccountID);

                double convToBaseFrom = rateFrom * amount;
                toTransAmount_ = convToBaseFrom / rateTo;
            } else {
                toTransAmount_ = amount;
            }
        }
    }

    wxString transNum = textNumber_->GetValue();
    wxString notes = textNotes_->GetValue();
    wxString status;
    wxStringClientData* status_obj = (wxStringClientData *)choiceStatus_->GetClientObject(choiceStatus_->GetSelection());
    if (status_obj) status = status_obj->GetData().Left(1);
    status.Replace("N", "");
    
    wxString date1 = dpc_->GetValue().FormatISODate();

    boost::shared_ptr<mmBankTransaction> pTransaction;
    if (!edit_)
    {
        boost::shared_ptr<mmBankTransaction> pTemp(new mmBankTransaction(core_->db_));
        pTransaction = pTemp;
    }
    else
    {
        pTransaction = core_->bTransactionList_.getBankTransactionPtr(accountID_, transID_);
    }

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(fromAccountID).lock();
    wxASSERT(pCurrencyPtr);

    pTransaction->accountID_ = fromAccountID;
    pTransaction->toAccountID_ = toAccountID;
    pTransaction->payee_ = core_->getPayeeSharedPtr(payeeID_);
    pTransaction->transType_ = transCode;
    pTransaction->amt_ = amount;
    pTransaction->status_ = status;
    pTransaction->transNum_ = transNum;
    pTransaction->notes_ = notes.c_str();
    pTransaction->category_ = core_->getCategorySharedPtr(categID_, subcategID_);
    pTransaction->date_ = dpc_->GetValue();
    pTransaction->toAmt_ = toTransAmount_;

    *pTransaction->splitEntries_.get() = *split_.get();
    pTransaction->updateAllData(core_, fromAccountID, pCurrencyPtr, true);

    if (!edit_)
    {
        core_->bTransactionList_.addTransaction(core_, pTransaction);
        mmPlayTransactionSound();
    }
    else
    {
        core_->bTransactionList_.updateTransaction(pTransaction);
    }

    EndModal(wxID_OK);
}

void mmTransDialog::SetSplitState()
{
    if (split_->getTotalSplits() > 0)
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

void mmTransDialog::SetTransferControls(bool transfers)
{
    if (transfers) 
    {
        cAdvanced_->Enable();
        bTo_->Show();
    } 
    else 
    {
        bTo_->Hide();
        cAdvanced_->Disable();
        SetAdvancedTransferControls();
    }
}

void mmTransDialog::SetAdvancedTransferControls(bool advanced)
{
    if (advanced) 
    {
        toTextAmount_->Enable();
        advancedToTransAmountSet_ = true;
        // Display the transfer amount in the toTextAmount control.
        if (toTransAmount_ >= 0)
        {
            wxString dispAmount;
            mmex::formatDoubleToCurrencyEdit(toTransAmount_, dispAmount);
            toTextAmount_->SetValue(dispAmount);
        } 
        else
        {
            toTextAmount_->SetValue(textAmount_->GetValue());
        }

        textAmount_->SetToolTip(_("Specify the transfer amount in the From Account"));

    } 
    else 
    {
        toTextAmount_->Disable();
        advancedToTransAmountSet_ = false;
    }
}

//----------------------------------------------------------------------------

void mmTransDialog::OnFrequentUsedNotes(wxCommandEvent& /*event*/)
{
    wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, ID_DIALOG_TRANS_BUTTON_FREQENTNOTES) ;
    ev.SetEventObject( this );

	wxMenu menu;

	char sql[] =
	"select max (TRANSDATE) as TRANSDATE , count (notes) COUNT, "
	"(case when accountid = ? then '1' else '2' end) as ACC "
	",replace(replace (substr (notes, 1, 30), x'0A', ' '), '&', '&&')||(case when length(notes)>30 then '...' else '' end) as NOTE, "
	"notes as NOTES "
	"from checkingaccount_v1 ca "
	"where notes is not '' "
	"and TRANSDATE< date ('now', '1 day', 'localtime') "
	"group by rtrim (notes) "
	"order by ACC, TRANSDATE desc, COUNT desc "
	"limit ? ";

	wxSQLite3Statement st = db_->PrepareStatement(sql);
	st.Bind(1, accountID_);
	st.Bind(2, NOTES_MENU_NUMBER);
	wxSQLite3ResultSet q1 = st.ExecuteQuery();
	int menu_id=1;
	while (q1.NextRow())
	{
		freqnotes_.Add(q1.GetString("NOTES"));
		wxString noteSTR = q1.GetString("NOTE");
		menu.Append(menu_id++, noteSTR);
	}
	q1.Finalize();

	if (menu_id>1)
		PopupMenu(&menu, 60, 30+((NOTES_MENU_NUMBER-menu_id-1)*23));

}

//----------------------------------------------------------------------------

void mmTransDialog::onNoteSelected(wxCommandEvent& event)
{
    int i =  event.GetId();
    if (i>0)
        *textNotes_ << freqnotes_.Item(i-1);
}

void mmTransDialog::changeFocus(wxChildFocusEvent& event)
{
    wxWindow *w = event.GetWindow();
    if ( w ) {
        object_id = w->GetId();
        richText_ = (object_id == ID_DIALOG_TRANS_TEXTNOTES);
    }
}

void mmTransDialog::OnCancel(wxCommandEvent& /*event*/)
{
    if (richText_)
    {
        wxButton* cancelButton = (wxButton*)FindWindow(wxID_CANCEL);
        cancelButton->SetFocus();        
        return;
    }
    else
        EndModal(wxID_CANCEL);
}

void mmTransDialog::activateSplitTransactionsDlg()
{
    SplitTransactionDialog dlg(core_, split_.get(), choiceTrans_->GetSelection(), this);
    if (dlg.ShowModal() == wxID_OK)
    {
        double amount = split_->getTotalSplits();
        if (choiceTrans_->GetSelection() == DEF_TRANSFER && amount < 0)
            amount = - amount;
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(amount, dispAmount);
        textAmount_->SetValue(dispAmount);
    }
}

void mmTransDialog:: OnButtonPayeeChar(wxKeyEvent& event)
{
    //it's for debuging
    //wxSafeShowMessage((":"), wxString::Format(("%i"), event.GetUnicodeKey()));
    if (event.GetKeyCode() != WXK_DOWN && event.GetKeyCode()!=WXK_UP) 
    {
        event.Skip();
        return;
    }
    size_t c = 0;
    wxString currentPayeeName = bPayee_->GetLabel();
    wxArrayString filtd;
    if (choiceTrans_->GetSelection() != DEF_TRANSFER) 
    {
        filtd = mmDBWrapper::filterPayees(db_.get(), "");
        if (filtd.IsEmpty()) //No payee present. Should be added one as minimum
            return;
        if (currentPayeeName == _("Select Payee")) 
            c = 0;            
        else 
        {
            for (size_t i = 0; i < (size_t)filtd.GetCount(); ++i) 
            {
                if (filtd.Item(i) == currentPayeeName) 
                {
                    c = i;
                    break;
                }
            }
        }
        
    } else 
    {
        filtd = core_->getAccountsName();
        if (filtd.IsEmpty()) 
            //No accounts present. Should be added one as minimum
            return;
        for (size_t i = 0; i < (size_t)filtd.GetCount(); ++i) 
        {
            if (filtd.Item(i) == currentPayeeName) 
            {
                c = i;
                break;
            }
        }
    }
    if (event.GetKeyCode()==WXK_DOWN) 
    {
        if ((c + 1) < (size_t)filtd.GetCount())
            currentPayeeName = filtd.Item(++c);
    } 
    else if (event.GetKeyCode()==WXK_UP)
    {
        if (c > 0)
            currentPayeeName = filtd.Item(--c);
    }

    currentPayeeName = filtd.Item(c);
    bPayee_->SetLabel(currentPayeeName);

    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
        payeeID_ = core_->getAccountID(currentPayeeName);
    else
        payeeID_ = core_->getPayeeID(currentPayeeName);
}

void mmTransDialog::OnButtonDateChar(wxKeyEvent& event)
{
    if (!wxGetKeyState(WXK_COMMAND) && !wxGetKeyState(WXK_ALT)) {
        int i;
        if (event.GetKeyCode() == WXK_DOWN) i = -1;
        else if (event.GetKeyCode() == WXK_UP) i = 1;
        else return;

        wxDateTime date = dpc_->GetValue();
        if (!wxGetKeyState(WXK_SHIFT))
            date = date.Add(wxDateSpan::Days(i));
        else
            date = date.Add(wxDateSpan::Months(i));
        
        dpc_->SetValue (date);
        itemStaticTextWeek_->SetLabel(mmGetShortWeekDayName(date.GetWeekDay()));

        event.Skip();
    }
}

void mmTransDialog::OnButtonToAccountChar(wxKeyEvent& event) 
{
    if (event.GetKeyCode() != WXK_DOWN && event.GetKeyCode()!=WXK_UP) 
    {
        event.Skip();
        return;
    }
    
    size_t c = 1;
    wxString toAccountName = bTo_->GetLabel();
    wxArrayString filtd;

    filtd = core_->getAccountsName(payeeID_);
    if (filtd.IsEmpty())
        return;
    if (toAccountName != _("Select To Acct")) 
    { 
        for (size_t i = 0; i < (size_t)filtd.GetCount(); ++i) 
        {
            if (filtd.Item(i) == toAccountName) 
            {
                c=i;
                break;
            }
        }
    } 
    else 
    {
        c = 0;
        toAccountName = filtd.Item(c);
    }
    
    if (event.GetKeyCode()==WXK_DOWN) 
    {
        if ((c+1) < (size_t)(filtd.GetCount())) toAccountName = filtd.Item(++c);
        bTo_->SetLabel(toAccountName);
    } 
    else if (event.GetKeyCode()==WXK_UP)
    {
        if (c > 0) toAccountName = filtd.Item(--c);
        bTo_->SetLabel(toAccountName);
    }

    toID_ = core_->getAccountID(toAccountName);
}

void mmTransDialog::OnButtonToAccountMouse(wxMouseEvent& event) 
{
    size_t c = 1;
    int i = event.GetWheelRotation(); 
    wxArrayString filtd = core_->getAccountsName(payeeID_);
    if (filtd.IsEmpty())
        return;
    wxString toAccountName = bTo_->GetLabel();
    if (toAccountName != _("Select To Acct")) 
    { 
        for (size_t i = 0; i < (size_t)filtd.GetCount(); ++i) 
        {
            if (filtd.Item(i) == toAccountName) 
            {
                c=i;
                break;
            }
        }
    } 
    else 
    {
        c = 0;
        toAccountName = filtd.Item(c);
    }
    
    if (i < 0) 
    {
        if ((c+1) < (size_t)(filtd.GetCount())) toAccountName = filtd.Item(++c);
        bTo_->SetLabel(toAccountName);
    } 
    else if (i > 0)
    {
        if (c > 0) toAccountName = filtd.Item(--c);
        bTo_->SetLabel(toAccountName);
    }

    toID_ = core_->getAccountID(toAccountName);
}

void mmTransDialog::OnButtonPayeeMouse(wxMouseEvent& event) 
{
    int i = event.GetWheelRotation();

    size_t c = 0;
    wxString currentPayeeName = bPayee_->GetLabel();
    wxArrayString filtd;
    if (choiceTrans_->GetSelection() == DEF_TRANSFER) 
    {
        filtd = core_->getAccountsName();
        if (filtd.IsEmpty()) 
            //No accounts present. Should be added one as minimum
            return;
        for (size_t i = 0; i < (size_t)filtd.GetCount(); ++i) 
        {
            if (filtd.Item(i) == currentPayeeName) 
            {
                c = i;
                break;
            }
        }
        
    } 
    else 
    {
        filtd = mmDBWrapper::filterPayees(db_.get(), "");
        if (filtd.IsEmpty()) 
            //No payee present. Should be added one as minimum
            return;
        if (currentPayeeName == _("Select Payee")) 
        {
            c = 0;            
        } 
        else 
        {
            for (size_t i = 0; i < (size_t)filtd.GetCount(); ++i) 
            {
                if (filtd.Item(i) == currentPayeeName) 
                {
                    c = i;
                    break;
                }
            }
        }
    }
    if (i < 0) 
    {
        if ((c + 1) < (size_t)filtd.GetCount()) currentPayeeName = filtd.Item(++c);
    } 
    else if (i > 0)
    {
        if (c > 0) currentPayeeName = filtd.Item(--c);
    }

    currentPayeeName = filtd.Item(c);
    bPayee_->SetLabel(currentPayeeName);

    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
        payeeID_ = core_->getAccountID(currentPayeeName);
    else 
        payeeID_ = core_->getPayeeID(currentPayeeName);
}

void mmTransDialog::SetDialogToDuplicateTransaction()
{
    // we want the dialog to treat the transaction as a new transaction.
    edit_ = false;
    this->SetTitle(_("Duplicate Transaction"));

   // we need to create a new pointer for Split transactions.
   boost::shared_ptr<mmSplitTransactionEntries> splitTransEntries(new mmSplitTransactionEntries());
   core_->bTransactionList_.getBankTransactionPtr(accountID_,
       transID_)->getSplitTransactions(core_, splitTransEntries.get());
   split_.get()->entries_ = splitTransEntries->entries_;
}

void mmTransDialog::OnKillDateFocus(wxFocusEvent& event)
{
    choiceStatus_->SetFocus();
    event.Skip();
}

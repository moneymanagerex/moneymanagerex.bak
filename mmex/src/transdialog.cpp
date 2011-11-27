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
#include "wx/datectrl.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "splittransactionsdialog.h"
#include "defs.h"
#include "paths.h"

#include <sstream>

// Defines for Transaction: (Status and Type) now located in dbWrapper.h

enum { ID_DIALOG_TRANS_SPINNER };
enum{ NOTES_MENU_NUMBER = 20 };
         
IMPLEMENT_DYNAMIC_CLASS( mmTransDialog, wxDialog )

BEGIN_EVENT_TABLE( mmTransDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmTransDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmTransDialog::OnCancel)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmTransDialog::OnCategs)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONPAYEE, mmTransDialog::OnPayee)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTO, mmTransDialog::OnTo)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmTransDialog::OnTransTypeChanged)  
    EVT_SPIN_UP(ID_DIALOG_TRANS_SPINNER,mmTransDialog::OnSpinUp)
    EVT_SPIN_DOWN(ID_DIALOG_TRANS_SPINNER,mmTransDialog::OnSpinDown)
    EVT_CHECKBOX(ID_DIALOG_TRANS_ADVANCED_CHECKBOX, mmTransDialog::OnAdvanceChecked) 
    EVT_CHECKBOX(ID_DIALOG_TRANS_SPLITCHECKBOX, mmTransDialog::OnSplitChecked)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTRANSNUM, mmTransDialog::OnAutoTransNum)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTON_FREQENTNOTES, mmTransDialog::OnFrequentUsedNotes)
    EVT_MENU (wxID_ANY, mmTransDialog::onNoteSelected)
    EVT_CHILD_FOCUS(mmTransDialog::changeFocus)
END_EVENT_TABLE()

mmTransDialog::mmTransDialog(
    boost::shared_ptr<wxSQLite3Database> db, 
    mmCoreDB* core,
    int accountID, int transID, bool edit, 
    wxSQLite3Database* inidb,
    wxWindow* parent, wxWindowID id, 
    const wxString& caption, const wxPoint& pos, 
    const wxSize& size, long style 
) :
    db_(db),
    inidb_(inidb),
    core_(core),
    transID_(transID),
    accountID_(accountID),
    edit_(edit),
    categID_(-1),
    subcategID_(-1),
    payeeID_(-1),
    toID_(-1),
    toTransAmount_(-1),
    advancedToTransAmountSet_(false),
    payeeUnknown_(true),
    categUpdated_(false),
    edit_currency_rate(1.0)
{
    referenceAccountID_ = accountID_;   // remember where dialog initiated from.
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

    if (edit_)
    {
        dataToControls();
    }
    
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
        categID_ = q1.GetInt(wxT("CATEGID"));
        subcategID_ = q1.GetInt(wxT("SUBCATEGID"));
        accountID_ = q1.GetInt(wxT("ACCOUNTID"));
       
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
        wxString dt = mmGetDateForDisplay(db_.get(), dtdt);
        dpc_->SetValue(dtdt);

        wxString transNumString = q1.GetString(wxT("TRANSACTIONNUMBER"));
        wxString statusString  = q1.GetString(wxT("STATUS"));
        wxString notesString  = q1.GetString(wxT("NOTES"));
        wxString transTypeString = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        toTransAmount_ = q1.GetDouble(wxT("TOTRANSAMOUNT"));

        // backup the original currency rate first
        if (transAmount > 0.0) {
            edit_currency_rate = toTransAmount_ / transAmount;
        }        
      
        if (statusString == wxT(""))
        {
            choiceStatus_->SetSelection(DEF_STATUS_NONE);
        }
        else if (statusString == wxT("R"))
        {
            choiceStatus_->SetSelection(DEF_STATUS_RECONCILED);
        }
        else if (statusString == wxT("V"))
        {
             choiceStatus_->SetSelection(DEF_STATUS_VOID);
        }
        else if (statusString == wxT("F"))
        {
             choiceStatus_->SetSelection(DEF_STATUS_FOLLOWUP);
        }
        else if (statusString == wxT("D"))
        {
             choiceStatus_->SetSelection(DEF_STATUS_DUPLICATE);
        }

        if (transTypeString == TRANS_TYPE_WITHDRAWAL_STR)
            choiceTrans_->SetSelection(DEF_WITHDRAWAL);
        else if (transTypeString == TRANS_TYPE_DEPOSIT_STR)
            choiceTrans_->SetSelection(DEF_DEPOSIT);
        else if (transTypeString == TRANS_TYPE_TRANSFER_STR)
            choiceTrans_->SetSelection(DEF_TRANSFER);
        updateControlsForTransType();

        payeeID_ = q1.GetInt(wxT("PAYEEID"));
        toID_ = q1.GetInt(wxT("TOACCOUNTID"));
        payeeUnknown_ = false;

        *split_.get() = *core_->bTransactionList_.getBankTransactionPtr(transID_)->splitEntries_.get();

        if (split_->numEntries() > 0)
        {
            bCategory_->SetLabel(_("Split Category"));
            cSplit_->SetValue(true);
        }
        else
        {
            wxString categString = q1.GetString(wxT("CATEGNAME"));
            categoryName_ = categString;
            if (subcategID_ != -1)
            {
                subCategoryName_ = q1.GetString(wxT("SUBCATEGNAME"));
                categString += wxT(" : ");
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

        bPayee_->SetLabel(q1.GetString(wxT("PAYEENAME")));

        if (transTypeString == TRANS_TYPE_TRANSFER_STR)
        {
            bPayee_->SetLabel(q1.GetString(wxT("ACCOUNTNAME")));
            bTo_->SetLabel(q1.GetString(wxT("TOACCOUNTNAME")));
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
    mmTransDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Transaction Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxTOP|wxRIGHT, 10);

    /************************************************************************************************************
    ItemPanel7 controlled by ItemFlexGridSizer8 - contained in the TransDetailsStaticSizer. (itemStaticBoxSizer4)
    *************************************************************************************************************/
    wxPanel* itemPanel7 = new wxPanel(itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel7, 0, wxGROW|wxALL, 10);

    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(5, 2, 10, 10);
    itemPanel7->SetSizer(itemFlexGridSizer8);

    wxSizerFlags flags;
    flags.Border(0);

    // Date --------------------------------------------
    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel7, wxID_STATIC, _("Date"));
    dpc_ = new wxDatePickerCtrl( itemPanel7, ID_DIALOG_TRANS_BUTTONDATE, wxDefaultDateTime,
                                 wxDefaultPosition, wxSize(110, -1), wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    dpc_->SetToolTip(_("Specify the date of the transaction"));

// change properties depending on system parameters
    wxSize spinCtrlSize = wxSize(16,-1);
    int spinCtrlDirection = wxSP_VERTICAL;
    int interval = 0;
#ifdef __WXMSW__
    spinCtrlSize = wxSize(40,14);
    spinCtrlDirection = wxSP_HORIZONTAL;
    interval = 4;
#endif
    spinCtrl_ = new wxSpinButton(itemPanel7,ID_DIALOG_TRANS_SPINNER,wxDefaultPosition,spinCtrlSize,spinCtrlDirection|wxSP_ARROW_KEYS|wxSP_WRAP);
    spinCtrl_ -> SetRange (-32768, 32768); 
    spinCtrl_->SetToolTip(_("Retard or advance the date of the transaction"));

    itemFlexGridSizer8->Add(itemStaticText15, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    wxBoxSizer* itemBoxSizer118 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer8->Add(itemBoxSizer118, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemBoxSizer118->Add(dpc_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemBoxSizer118->Add(spinCtrl_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, interval);   

    // Status --------------------------------------------
    wxStaticText* itemStaticText51 = new wxStaticText( itemPanel7, wxID_STATIC, _("Status"));
    wxString itemChoice7Strings[] = 
    {
        _("None"),
        _("Reconciled"),
        _("Void"),
        _("Follow up"),
        _("Duplicate"),
    };  
    choiceStatus_ = new wxChoice( itemPanel7, ID_DIALOG_TRANS_STATUS, wxDefaultPosition, wxSize(110, -1), 5, itemChoice7Strings, 0 );
    choiceStatus_->SetSelection(mmIniOptions::transStatusReconciled_);
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));

    itemFlexGridSizer8->Add(itemStaticText51, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    itemFlexGridSizer8->Add(choiceStatus_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Type --------------------------------------------
    wxStaticText* itemStaticText5 = new wxStaticText( itemPanel7, wxID_STATIC, _("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    const wxString itemChoice6Strings[] = 
    {
        _("Withdrawal"),
        _("Deposit"),
        _("Transfer")
    };  
    choiceTrans_ = new wxChoice(itemPanel7,ID_DIALOG_TRANS_TYPE,wxDefaultPosition,wxSize(110, -1),3,itemChoice6Strings,0);
    choiceTrans_->SetSelection(0);
    choiceTrans_->SetToolTip(_("Specify the type of transactions to be created."));
    cAdvanced_ = new wxCheckBox( itemPanel7, ID_DIALOG_TRANS_ADVANCED_CHECKBOX, _("Advanced"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cAdvanced_->SetValue(FALSE);
    cAdvanced_->SetToolTip(_("Allows the setting of different amounts in the FROM and TO accounts."));

    wxBoxSizer* typeSizer = new wxBoxSizer(wxHORIZONTAL);
    typeSizer->Add(choiceTrans_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    typeSizer->Add(cAdvanced_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxALL, 5);
    
    itemFlexGridSizer8->Add(itemStaticText5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    itemFlexGridSizer8->Add(typeSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Amount Fields --------------------------------------------
    amountNormalTip_   = _("Specify the amount for this transaction");
    amountTransferTip_ = _("Specify the amount to be transfered"); 

    //Validator
    wxTextValidator doubleValidator(wxFILTER_INCLUDE_CHAR_LIST); 
    wxArrayString list; 
    wxString valid_chars(wxT(" 0123456789.,")); 
    size_t len = valid_chars.Length(); 
    for (size_t i=0; i<len; i++) 
    list.Add(wxString(valid_chars.GetChar(i))); 
    doubleValidator.SetIncludes(list);

    wxStaticText* amountStaticText = new wxStaticText( itemPanel7, wxID_STATIC, _("Amount"));

    textAmount_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""), wxDefaultPosition, wxSize(110, -1), wxALIGN_RIGHT, doubleValidator );
    textAmount_->SetToolTip(amountNormalTip_);

    toTextAmount_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""), wxDefaultPosition, wxSize(110, -1), wxALIGN_RIGHT, doubleValidator );
    toTextAmount_->SetToolTip(_("Specify the transfer amount in the To Account"));

    wxBoxSizer* amountSizer = new wxBoxSizer(wxHORIZONTAL);
    amountSizer->Add(textAmount_,   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 0);
    amountSizer->Add(toTextAmount_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    itemFlexGridSizer8->Add(amountStaticText, 0, wxALIGN_LEFT|wxALL, 0);
    itemFlexGridSizer8->Add(amountSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Payee ---------------------------------------------
    wxStaticText* itemStaticText9 = new wxStaticText(itemPanel7, ID_DIALOG_TRANS_STATIC_PAYEE, _("Payee"), wxDefaultPosition, wxDefaultSize, 0 );

    wxString payeeName = resetPayeeString();
    wxString categString;

    if (!edit_)
    {
        //If user does not want payee to be auto filled for the new transaction
        if ( mmIniOptions::transPayeeSelectionNone_ == 0)
        {
            payeeName = resetPayeeString();
            payeeUnknown_ = true;
        }
        else // determine the payee for this account
        {
            payeeName = getMostFrequentlyUsedPayee(categString);
            payeeUnknown_ = false;
        }

        if ( mmIniOptions::transCategorySelectionNone_== 0 || categString.IsEmpty() )
        {
            categString = resetCategoryString();
        }
    }

    bPayee_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONPAYEE, payeeName, wxDefaultPosition, wxSize(225, -1), 0 );
    payeeWithdrawalTip_ = _("Specify where the transaction is going to");
    payeeDepositTip_    = _("Specify where the transaction is coming from");
    bPayee_->SetToolTip(payeeWithdrawalTip_);

    itemFlexGridSizer8->Add(itemStaticText9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemFlexGridSizer8->Add(bPayee_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Payee Alternate ------------------------------------------------
    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel7, ID_DIALOG_TRANS_STATIC_FROM, wxT(" "), wxDefaultPosition, wxDefaultSize, 0 );
    bTo_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONTO, _("Select To Acct"), wxDefaultPosition, wxSize(225, -1), 0 );
    bTo_->SetToolTip(_("Specify which account the transfer is going to"));
    
    itemFlexGridSizer8->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxUP, 0);
    itemFlexGridSizer8->Add(bTo_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxUP, 0);

    // Split Category -------------------------------------------
    cSplit_ = new wxCheckBox( itemPanel7, ID_DIALOG_TRANS_SPLITCHECKBOX, _("Split"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cSplit_->SetValue(FALSE);
    cSplit_->SetToolTip(_("Use split Categories"));

    itemFlexGridSizer8->AddSpacer(20);  // Fill empty space.
    itemFlexGridSizer8->Add(cSplit_, 0, wxALIGN_BOTTOM|wxALIGN_LEFT|wxALL, 0);

    // Category -------------------------------------------------
    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel7, wxID_STATIC, _("Category") );
    bCategory_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONCATEGS, categString, wxDefaultPosition, wxSize(225, -1), 0 );
    //bCategory_->SetToolTip(_("Specify the category for this transaction"));  
    
    itemFlexGridSizer8->Add(itemStaticText17, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    itemFlexGridSizer8->Add(bCategory_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    
    // Number  ---------------------------------------------
    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel7, wxID_STATIC, _("Number"), wxDefaultPosition, wxDefaultSize, 0 );
    textNumber_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNUMBER, wxT(""), wxDefaultPosition, wxSize(185, -1), 0 );
    textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));

    bAuto_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONTRANSNUM, wxT("..."), wxDefaultPosition, wxSize(40, -1), 0 );
    bAuto_->SetToolTip(_("Populate Transaction #"));

    itemFlexGridSizer8->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    wxBoxSizer* itemBoxSizer550 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer8->Add(itemBoxSizer550, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemBoxSizer550->Add(textNumber_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemBoxSizer550->Add(bAuto_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Notes  ---------------------------------------------
    wxStaticText* notesStaticText = new wxStaticText( itemPanel7, wxID_STATIC, _("Notes"), wxDefaultPosition, wxDefaultSize, 0 );

    textNotes_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNOTES, wxT(""), wxDefaultPosition, wxSize(225,80), wxTE_MULTILINE );
    textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));

    bFrequentUsedNotes_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTON_FREQENTNOTES, wxT(">>"), wxDefaultPosition, wxSize(40, -1), 0 );
    bFrequentUsedNotes_->SetToolTip(_("Select one of the frequently used notes"));

    wxBoxSizer* itemBoxSizer56 = new wxBoxSizer(wxVERTICAL);

    itemFlexGridSizer8->Add(itemBoxSizer56, 0, wxGROW|wxALIGN_RIGHT|wxALIGN_TOP|wxALL, 0);
    itemBoxSizer56->Add(notesStaticText, 0, wxALIGN_LEFT|wxALIGN_TOP|wxALL|wxADJUST_MINSIZE, 0);
    itemBoxSizer56->Add(bFrequentUsedNotes_, 0, wxALIGN_RIGHT|wxALIGN_TOP|wxALL|wxADJUST_MINSIZE, 10);
    itemFlexGridSizer8->Add(textNotes_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEAST, 0);

    SetTransferControls();  // hide appropriate fields
    /**********************************************************************************************
     Button Panel with OK and Cancel Buttons
    ***********************************************************************************************/
    wxPanel* itemPanel25 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel25, 5, wxALIGN_RIGHT|wxTOP|wxDOWN, 10);

    wxStdDialogButtonSizer*  itemStdDialogButtonSizer1 = new wxStdDialogButtonSizer;
    itemPanel25->SetSizer(itemStdDialogButtonSizer1);

    wxButton* itemButton27 = new wxButton( itemPanel25, wxID_OK, _("&OK"));
    itemStdDialogButtonSizer1->Add(itemButton27, flags);

    wxButton* itemButton28 = new wxButton( itemPanel25, wxID_CANCEL, _("&Cancel"));
    itemStdDialogButtonSizer1->Add(itemButton28,  0, wxALIGN_RIGHT|wxLEFT|wxRIGHT, 10);
    if (edit_)
        itemButton28->SetFocus();

    itemStdDialogButtonSizer1->Realize();

}

void mmTransDialog::OnPayee(wxCommandEvent& /*event*/)
{
    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
    {
        static const char sql[] =
        "select ACCOUNTNAME "
        "from ACCOUNTLIST_V1 "
        "where ACCOUNTTYPE IN ('Checking', 'Term') and STATUS <> 'Closed' "
        "order by ACCOUNTNAME";

        wxArrayString as;

        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
        while (q1.NextRow())
        {
            as.Add(q1.GetString(wxT("ACCOUNTNAME")));
        }
        q1.Finalize();

        wxSingleChoiceDialog scd(0, _("Account name"), _("Select Account"), as);
        if (scd.ShowModal() == wxID_OK)
        {
            wxString acctName = scd.GetStringSelection();
            payeeID_ = mmDBWrapper::getAccountID(db_.get(), acctName);
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
            if ( mmIniOptions::transCategorySelectionNone_ == 1 && ( !edit_ && !categUpdated_ ) )
            {
                // if payee has memory of last category used then display last category for payee
                if (tempCategID != -1)
                {
                    wxString categString = mmDBWrapper::getCategoryName(db_.get(), tempCategID);
                    categoryName_ = categString;
                    if (tempSubCategID != -1)
                    {
                        wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(), tempCategID, tempSubCategID);
                        subCategoryName_ = subcatName; 
                        categString += wxT(" : ");
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
    int mID = mmDBWrapper::getTransactionNumber(db_.get(), accountID_);
    wxString wxIDstr = wxString::Format(wxT( "%d" ), (int) mID);
    textNumber_->SetValue( wxIDstr );
}

void mmTransDialog::OnTo(wxCommandEvent& /*event*/)
{
    // This should only get called if we are in a transfer

    static const char sql[] =
        "select ACCOUNTNAME "
        "from ACCOUNTLIST_V1 "
        "where (ACCOUNTTYPE = 'Checking' or ACCOUNTTYPE = 'Term') and STATUS <> 'Closed' "
        "order by ACCOUNTNAME";

    wxArrayString as;

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    q1.Finalize();

    wxSingleChoiceDialog scd(0, _("Account name"), _("Select Account"), as);
    if (scd.ShowModal() == wxID_OK)
    {
        wxString acctName = scd.GetStringSelection();
        toID_ = mmDBWrapper::getAccountID(db_.get(), acctName);
        bTo_->SetLabel(acctName);
    }
}

void mmTransDialog::OnSpinUp(wxSpinEvent& event)
{
    wxString dateStr = dpc_->GetValue().FormatISODate();
    wxDateTime date = mmGetStorageStringAsDate (dateStr) ;
    date = date.Add(wxDateSpan::Days(1));
    //dateStr = mmGetDateForDisplay(db_.get(), date);
    dpc_->SetValue (date);
    
    event.Skip();
}

void mmTransDialog::OnSpinDown(wxSpinEvent& event)
{
    wxString dateStr = dpc_->GetValue().FormatISODate();
    wxDateTime date = mmGetStorageStringAsDate (dateStr) ;
    date = date.Add(wxDateSpan::Days(-1));
    //dateStr = mmGetDateForDisplay(db_.get(), date);
    dpc_->SetValue (date);

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
        SplitTransactionDialog dlg(core_, split_.get(), this);
        if (dlg.ShowModal() == wxID_OK)
        {
            wxString dispAmount;
            mmex::formatDoubleToCurrencyEdit(split_->getTotalSplits(), dispAmount);
            textAmount_->SetValue(dispAmount);
        }
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
                wxString catName = mmDBWrapper::getCategoryName(db_.get(), categID_);
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
                    catName.Replace(wxT("&"), wxT("&&"));
                    bCategory_->SetLabel(catName);
                }

                return;
            }

            categID_ = dlg.categID_;
            subcategID_ = dlg.subcategID_;
            categUpdated_ = true;

            wxString catName = mmDBWrapper::getCategoryName(db_.get(), dlg.categID_);
            categoryName_ = catName;
            catName.Replace (wxT("&"), wxT("&&"));
            wxString categString = catName;

            if (dlg.subcategID_ != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(), dlg.categID_, dlg.subcategID_);
                subCategoryName_ = subcatName;
                subcatName.Replace (wxT("&"), wxT("&&"));
                categString += wxT(" : ");
                categString += subcatName;
            } else {
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

    if (choiceTrans_->GetSelection() == DEF_WITHDRAWAL) {

        displayControlsToolTips(DEF_WITHDRAWAL);
        SetTransferControls();
        stp->SetLabel(_("Payee"));
        bPayee_->SetToolTip(payeeWithdrawalTip_);
        itemStaticText13->SetLabel(wxT(""));
        if (payeeUnknown_) 
            bPayee_->SetLabel(resetPayeeString());

    } else if (choiceTrans_->GetSelection() == DEF_DEPOSIT) {

        displayControlsToolTips(DEF_DEPOSIT);
        SetTransferControls();
        stp->SetLabel(_("From"));
        bPayee_->SetToolTip(payeeDepositTip_);
        itemStaticText13->SetLabel(wxT(""));
        if (payeeUnknown_) 
            bPayee_->SetLabel(resetPayeeString());

    } else if (choiceTrans_->GetSelection() == DEF_TRANSFER) {

        displayControlsToolTips(DEF_TRANSFER, true);
        SetTransferControls(true);
        if (cAdvanced_->IsChecked())
            SetAdvancedTransferControls(true);

        stp->SetLabel(_("From"));
        bTo_->SetLabel(_("Select To Account"));
        itemStaticText13->SetLabel(_("To"));
        toID_    = -1;
        payeeUnknown_ = true;

        wxString acctName = mmDBWrapper::getAccountName(db_.get(), accountID_);
        bPayee_->SetLabel(acctName);
        payeeID_ = accountID_;

        if (!edit_)
        {
            wxString categString;
            if (  mmIniOptions::transCategorySelectionNone_==0 ) // Default Category = "None"
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

    if (normal)
        toID_    = -1;

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
            " left join SUBCATEGORY_V1 sc"
            " on sc.CATEGID = ca.CATEGID and sc.SUBCATEGID = ca.SUBCATEGID "
        "where ca.transcode = 'Transfer' and ca.accountid = ? or ca.toaccountid = ? "
        "group by ca.payeeid, ca.transdate, ca.categid, ca.subcategid "
        "order by ca.transdate desc, ca.transid desc, c desc "
        "limit 1";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, accountID_);
    st.Bind(2, accountID_);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    wxString categString = q1.GetString(wxT("CATEGNAME"));
    wxString subcategName = q1.GetString(wxT("SUBCATEGNAME"));
    categID_ = q1.GetInt(wxT("CATEGID"));
    subcategID_ = q1.GetInt(wxT("SUBCATEGID"));
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
            categString += wxT(" : ");
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
            " left join SUBCATEGORY_V1 sc on "
                " sc.CATEGID = ca.CATEGID and sc.SUBCATEGID = ca.SUBCATEGID "
        "where ca.payeeid=p.payeeid and "
             " ca.transcode<>'Transfer' and "
             " ca.accountid = ? "
        "group by ca.payeeid, ca.transdate, ca.categid, ca.subcategid "
        "order by ca.transdate desc, ca.transid desc, c desc "
        "limit 1";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, accountID_);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    wxString payeeName = q1.GetString(wxT("PAYEENAME"));
    payeeID_ = q1.GetInt(wxT("PAYEEID"));

    // get category info
    categString = q1.GetString(wxT("CATEGNAME"));
    wxString subcategName = q1.GetString(wxT("SUBCATEGNAME"));
    categID_    = q1.GetInt(wxT("CATEGID"));
    subcategID_ = q1.GetInt(wxT("SUBCATEGID"));
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
            categString += wxT(" : ");
            categString += subcategName;
            subCategoryName_ = subcategName;
        }
    }

    return payeeName;
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
    wxString transCode = wxT("");
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
            mmShowErrorMessageInvalid(this, _("Amount"));
            return;
        }
    }
    else
    {
        wxString amountStr = textAmount_->GetValue().Trim();
        if (!mmex::formatCurrencyToDouble(amountStr, amount) || (amount < 0.0))
        {
            textAmount_->SetBackgroundColour(wxT("RED"));
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
            toTextAmount_->SetBackgroundColour(wxT("RED"));
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
        //mmDBWrapper::updatePayee(db_.get(), payeeName, payeeID_, categID_, subcategID_);

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
    //    else
    //    {
    //        // Since to trans amount is not set,
    //        // we use the original currency rate to calculate
    //        // toTransAmount
    //        toTransAmount_ = edit_currency_rate * amount;
    //    }
    }

    wxString transNum = textNumber_->GetValue();
    wxString notes = textNotes_->GetValue();
    wxString status = wxT(""); // nothing yet

    if (choiceStatus_->GetSelection() == DEF_STATUS_NONE)
    {
        status = wxT(""); // nothing yet
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_RECONCILED)
    {
        status = wxT("R");
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_VOID)
    {
        status = wxT("V");
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_FOLLOWUP)
    {
        status = wxT("F");
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_DUPLICATE)
    {
        status = wxT("D");
    }

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

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
    wxASSERT(pCurrencyPtr);

    pTransaction->accountID_ = fromAccountID;
    pTransaction->toAccountID_ = toAccountID;
    pTransaction->payee_ = core_->payeeList_.getPayeeSharedPtr(payeeID_);
    pTransaction->transType_ = transCode;
    pTransaction->amt_ = amount;
    pTransaction->status_ = status;
    pTransaction->transNum_ = transNum;
    pTransaction->notes_ = notes.c_str();
    pTransaction->category_ = core_->categoryList_.getCategorySharedPtr(categID_, subcategID_);
    pTransaction->date_ = dpc_->GetValue();
    pTransaction->toAmt_ = toTransAmount_;

    *pTransaction->splitEntries_.get() = *split_.get();
    pTransaction->updateAllData(core_, fromAccountID, pCurrencyPtr, true);

    if (!edit_)
    {
        core_->bTransactionList_.addTransaction(core_, pTransaction);
        mmPlayTransactionSound(inidb_);
    }
    else
    {
        core_->bTransactionList_.updateTransaction(pTransaction);
    }

    EndModal(wxID_OK);
}

void mmTransDialog::OnSplitChecked(wxCommandEvent& /*event*/)
{
    /* Reset Category */
    categID_ = -1;
    subcategID_ = -1;
    split_ = boost::shared_ptr<mmSplitTransactionEntries>(new mmSplitTransactionEntries());

    bool state = cSplit_->GetValue();
    if (state)
    {
        bCategory_->SetLabel(_("Split Category"));
        textAmount_->Enable(false);
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(split_->getTotalSplits(), dispAmount);
        textAmount_->SetValue(dispAmount);
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
    if (transfers) {
        cAdvanced_->Enable();
        bTo_->Show();
    } else {
        bTo_->Hide();
        cAdvanced_->Disable();
        SetAdvancedTransferControls();
    }
}

void mmTransDialog::SetAdvancedTransferControls(bool advanced)
{
    if (advanced) {
        toTextAmount_->Enable();
        advancedToTransAmountSet_ = true;
        // Display the transfer amount in the toTextAmount control.
        if (toTransAmount_ >= 0)
        {
            wxString dispAmount;
            mmex::formatDoubleToCurrencyEdit(toTransAmount_, dispAmount);
            toTextAmount_->SetValue(dispAmount);
        } else
            toTextAmount_->SetValue(textAmount_->GetValue());

        textAmount_->SetToolTip(_("Specify the transfer amount in the From Account"));

    } else {
        toTextAmount_->Disable();
        advancedToTransAmountSet_ = false;
    }
}

//----------------------------------------------------------------------------

void mmTransDialog::OnFrequentUsedNotes(wxCommandEvent& /*event*/)
{
    wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, ID_DIALOG_TRANS_BUTTON_FREQENTNOTES) ;
    ev.SetEventObject( this );

    wxString notes = textNotes_->GetValue();
    if (notes.IsEmpty ())
    {
        wxMenu menu;

        char sql[] =
        "select max (TRANSDATE) as TRANSDATE , count (notes) COUNT, "
        "(case when accountid = ? then '1' else '2' end) as ACC "
        ",replace(replace (substr (notes, 1, 20), x'0A', ' '), '&', '&&')||(case when length(notes)>20 then '...' else '' end) as NOTE, "
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
            freqnotes.Add(q1.GetString(wxT("NOTES")));
            wxString noteSTR = q1.GetString(wxT("NOTE"));
            menu.Append(menu_id++, noteSTR);
        }
        q1.Finalize();
    
        if (menu_id>1)
            PopupMenu(&menu, 60, 30+((NOTES_MENU_NUMBER-menu_id-1)*23));

    }
}

//----------------------------------------------------------------------------

void mmTransDialog::onNoteSelected(wxCommandEvent& event)
{
    int i =  event.GetId();
    if (i>0)
    textNotes_->SetValue (freqnotes.Item (i-1)) ;
}

void mmTransDialog::changeFocus(wxChildFocusEvent& event)
{
	wxWindow *w = event.GetWindow();
	if ( w ) 
	{
		richText = (w->GetId() == ID_DIALOG_TRANS_TEXTNOTES ? true : false);	
	}
	
}

void mmTransDialog::OnCancel(wxCommandEvent& /*event*/)
{
    if (richText){
		return;
	} else {
		EndModal(wxID_CANCEL);
    }
}

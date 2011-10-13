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

#include "billsdepositsdialog.h"
#include "wx/datectrl.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "splittransactionsdialog.h"
#include "defs.h"
#include "paths.h"

#include <limits>


IMPLEMENT_DYNAMIC_CLASS( mmBDDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBDDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTON_OK, mmBDDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmBDDialog::OnCategs)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONPAYEE, mmBDDialog::OnPayee)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTO, mmBDDialog::OnTo)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmBDDialog::OnTransTypeChanged)  
    EVT_DATE_CHANGED(ID_DIALOG_TRANS_BUTTONDATE, mmBDDialog::OnDateChanged) 
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONADVANCED, mmBDDialog::OnAdvanced)
    EVT_BUTTON(ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME, mmBDDialog::OnAccountName)
    EVT_CHECKBOX(ID_DIALOG_TRANS_SPLITCHECKBOX, mmBDDialog::OnSplitChecked)
    EVT_CHECKBOX(ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_USERACK, mmBDDialog::OnAutoExecutionUserAckChecked)
    EVT_CHECKBOX(ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_SILENT, mmBDDialog::OnAutoExecutionSilentChecked)
    EVT_CALENDAR_SEL_CHANGED(ID_DIALOG_BD_CALENDAR, mmBDDialog::OnCalendarSelChanged)
END_EVENT_TABLE()

// Defines for Transaction Status and Type now located in dbWrapper.h

const wxString REPEAT_TRANSACTIONS_MSGBOX_HEADING = _("Repeat Transaction - Auto Execution Checking");

mmBDDialog::mmBDDialog( )
{
}

mmBDDialog::mmBDDialog(wxSQLite3Database* db, mmCoreDB* core, int bdID, bool edit, bool enterOccur,
                       wxWindow* parent, wxWindowID id, const wxString& caption, 
                       const wxPoint& pos, const wxSize& size, long style )
{
    db_ = db;
    core_ = core;
    bdID_ = bdID;
    edit_ = edit;
    categID_ = -1;
    subcategID_ = -1;
    payeeID_ = -1;
    accountID_ = -1;
    toID_ = -1;
    toTransAmount_ = -1;
    enterOccur_ = enterOccur;
    advancedToTransAmountSet_ = false;
    payeeUnknown_ = true;

    autoExecuteUserAck_ = false;
    autoExecuteSilent_  = false;

    Create(parent, id, caption, pos, size, style);
}

bool mmBDDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
                           const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());
    
    fillControls();
    boost::shared_ptr<mmSplitTransactionEntries> split(new mmSplitTransactionEntries());
    split_ = split;

    if (edit_ || enterOccur_)
    {
        dataToControls();
    }

    if (advancedToTransAmountSet_)
        staticTextAdvancedActive_->Show();
    else
        staticTextAdvancedActive_->Show(false);

    Centre();
    return TRUE;
}

void mmBDDialog::dataToControls()
{
    choiceTrans_->Disable();
    if (enterOccur_)
    {
        dpcbd_->Disable();
        itemRepeats_->Disable();
        itemAccountName_->Disable();
        textAmount_->SetFocus();
    }

    if (edit_)        // Disabled on Edit because changing this date has no effect except to confuse user.
        dpc_->Disable();

    wxSQLite3Statement st_payee = db_->PrepareStatement("select PAYEENAME from PAYEE_V1 where PAYEEID = ?");

    wxSQLite3Statement st = db_->PrepareStatement("select * from BILLSDEPOSITS_V1 where BDID = ?");
    st.Bind(1, bdID_);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    if (q1.NextRow())
    {
        categID_ = q1.GetInt(wxT("CATEGID"));
        subcategID_ = q1.GetInt(wxT("SUBCATEGID"));

        wxString transNumString = q1.GetString(wxT("TRANSACTIONNUMBER"));
        wxString statusString  = q1.GetString(wxT("STATUS"));
        wxString notesString  = q1.GetString(wxT("NOTES"));
        wxString transTypeString = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        toTransAmount_ = q1.GetDouble(wxT("TOTRANSAMOUNT"));
        
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

        wxString nextOccurrString = q1.GetString(wxT("NEXTOCCURRENCEDATE"));
        wxString numRepeatStr  = q1.GetString(wxT("NUMOCCURRENCES"));
        if (numRepeatStr != wxT("-1"))
            textNumRepeats_->SetValue(numRepeatStr);

        wxDateTime dtno = mmGetStorageStringAsDate(nextOccurrString);
        wxString dtnostr = mmGetDateForDisplay(db_, dtno);
        dpcbd_->SetValue(dtno);
        dpc_->SetValue(dtno);

        int repeatSel = q1.GetInt(wxT("REPEATS"));
        // Have used repeatSel to multiplex auto repeat fields.
        if (repeatSel >= BD_REPEATS_MULTIPLEX_BASE)
        {
            repeatSel -= BD_REPEATS_MULTIPLEX_BASE;
            autoExecuteUserAck_ = true;
            itemCheckBoxAutoExeUserAck_->SetValue(true);
            itemCheckBoxAutoExeSilent_->Enable(true);

            if (repeatSel >= BD_REPEATS_MULTIPLEX_BASE)
            {
                repeatSel -= BD_REPEATS_MULTIPLEX_BASE;
                autoExecuteSilent_ = true;
                itemCheckBoxAutoExeSilent_->SetValue(true);
            }
        }

        itemRepeats_->SetSelection(repeatSel);
        if (repeatSel == 0) // if none
            textNumRepeats_->SetValue(wxT(""));
        
        if (transTypeString == TRANS_TYPE_WITHDRAWAL_STR)
            choiceTrans_->SetSelection(DEF_WITHDRAWAL);
        else if (transTypeString == TRANS_TYPE_DEPOSIT_STR)
            choiceTrans_->SetSelection(DEF_DEPOSIT);
        else if (transTypeString == TRANS_TYPE_TRANSFER_STR)
            choiceTrans_->SetSelection(DEF_TRANSFER);
        updateControlsForTransType();

        payeeID_ = q1.GetInt(wxT("PAYEEID"));
        toID_ = q1.GetInt(wxT("TOACCOUNTID"));
        accountID_ = q1.GetInt(wxT("ACCOUNTID"));
        wxString accountName = mmDBWrapper::getAccountName(db_, accountID_);
        itemAccountName_->SetLabel(accountName);

        // --

        wxString payeeString;
        
        {
            st_payee.Bind(1, payeeID_);

            wxSQLite3ResultSet rs = st_payee.ExecuteQuery();
            if (rs.NextRow())
            {
                payeeString = rs.GetString(wxT("PAYEENAME"));
            }

            rs.Finalize();
            st_payee.Reset();
        }

        // --
      
		split_->loadFromBDDB(core_, bdID_);

        if (split_->numEntries() > 0)
        {
            bCategory_->SetLabel(_("Split Category"));
            cSplit_->SetValue(true);
        }
        else
        {
            wxString catName = mmDBWrapper::getCategoryName(db_, categID_);
            wxString categString = catName;

            if (subcategID_ != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_, categID_, subcategID_);
                categString += wxT(" : ");
                categString += subcatName;
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

        bPayee_->SetLabel(payeeString);
        
        if (transTypeString == TRANS_TYPE_TRANSFER_STR)
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_, accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_, toID_);

            bPayee_->SetLabel(fromAccount);
            bTo_->SetLabel(toAccount);
            payeeID_ = accountID_;

            // When editing an advanced transaction record, we do not reset the toTransAmount_
            if ((edit_ || enterOccur_) && (toTransAmount_ != transAmount))
                advancedToTransAmountSet_ = true;
        }

    }
    
    st.Finalize();
    st_payee.Finalize();
}

void mmBDDialog::fillControls()
{
    
}

void mmBDDialog::CreateControls()
{    
    mmBDDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer20);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer20->Add(itemBoxSizer3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer3L = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer3L, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    /* Calendar */
    wxStaticBox* itemStaticBoxSizerCalendar = new wxStaticBox(itemDialog1, wxID_ANY, _("Calendar"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticBoxSizer* itemStaticBoxSizer44 = new wxStaticBoxSizer(itemStaticBoxSizerCalendar, wxHORIZONTAL);
    itemBoxSizer3L->Add(itemStaticBoxSizer44, 10, wxALIGN_CENTER|wxALL, 15);
	
	wxCalendarCtrl* itemCalendarCtrl44 = new wxCalendarCtrl( itemDialog1, ID_DIALOG_BD_CALENDAR, wxDateTime(), wxDefaultPosition, wxDefaultSize, 
	//TODO: Some users wish to have monday first in calendar!
	//wxCAL_MONDAY_FIRST
	wxCAL_SUNDAY_FIRST
	|wxSUNKEN_BORDER|wxCAL_SHOW_HOLIDAYS|wxCAL_SEQUENTIAL_MONTH_SELECTION );
    itemStaticBoxSizer44->Add(itemCalendarCtrl44, 10, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
	
    /* Bills & Deposits Details */
    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Repeating Transaction Details"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxHORIZONTAL);
    itemBoxSizer3L->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(4, 2, 10, 10);
    itemStaticBoxSizer4->Add(itemFlexGridSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Account Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText6, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    itemAccountName_ = new wxButton( itemDialog1, ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME, 
        _("Select Account"), wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer5->Add(itemAccountName_, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemAccountName_->SetToolTip(_("Specify the Account that will own the repeating transaction"));

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Next Occurrence"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText8, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    dpcbd_ = new wxDatePickerCtrl( itemDialog1, ID_DIALOG_BD_BUTTON_NEXTOCCUR, wxDefaultDateTime, 
              wxDefaultPosition, wxSize(110,-1), wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    itemFlexGridSizer5->Add(dpcbd_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    dpcbd_->SetToolTip(_("Specify the date of the next bill or deposit"));

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Repeats"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText10, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    
    wxString itemComboBox11Strings[] = 
    {
        _("None"),
        _("Weekly"),
        _("Bi-Weekly"),
        _("Monthly"),
        _("Bi-Monthly"),
        _("Quarterly"),
        _("Half-Yearly"),
        _("Yearly"),
        _("Four Months"),
        _("Four Weeks"),
        _("Daily"),
    };  
    itemRepeats_ = new wxChoice( itemDialog1, ID_DIALOG_BD_COMBOBOX_REPEATS, wxDefaultPosition, 
        wxSize(100, -1), 11, itemComboBox11Strings, 0);
    itemFlexGridSizer5->Add(itemRepeats_, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemRepeats_->SetSelection(0);

    wxStaticText* itemStaticText231 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Times Repeated"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText231, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    textNumRepeats_ = new wxTextCtrl( itemDialog1, ID_DIALOG_BD_TEXTCTRL_NUM_TIMES, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(textNumRepeats_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    textNumRepeats_->SetToolTip(_("Specify the number of times this series repeats. Leave blank if this series continues forever."));

    /* Auto Execution Status */
    itemCheckBoxAutoExeUserAck_ = new wxCheckBox( itemDialog1, 
        ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_USERACK, _("Set to 'Auto Execute' on the 'Next Occurrence' date."),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemBoxSizer3L->Add(itemCheckBoxAutoExeUserAck_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    itemCheckBoxAutoExeSilent_ = new wxCheckBox( itemDialog1, 
        ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_SILENT, _("Set 'Auto Execute' without user acknowlegement."),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBoxAutoExeSilent_->Enable(false);
    itemBoxSizer3L->Add(itemCheckBoxAutoExeSilent_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    /* Transaction Details */
    wxStaticBox* itemStaticBoxSizer14Static = new wxStaticBox(itemDialog1, 
        wxID_ANY, _("Transaction Details"), wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(itemStaticBoxSizer14Static, 
        wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer14, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer14->Add(itemBoxSizer4, 0, wxALIGN_LEFT|wxALL, 0);

    wxPanel* itemPanel7 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer14->Add(itemPanel7, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(8, 2, 10, 10);
    itemPanel7->SetSizer(itemFlexGridSizer8);

	////Date
    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel7, wxID_STATIC, _("Date"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText15, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    dpc_ = new wxDatePickerCtrl( itemPanel7, ID_DIALOG_TRANS_BUTTONDATE, wxDefaultDateTime, 
        wxDefaultPosition, wxSize(110, -1), wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    itemFlexGridSizer8->Add(dpc_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    dpc_->SetToolTip(_("Specify the date of the transaction"));

    ////////////////////////////////////////////

    wxStaticText* itemStaticText51 = new wxStaticText( itemPanel7, wxID_STATIC, _("Status"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText51, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    wxString itemChoice7Strings[] = 
    {
        _("None"),
        _("Reconciled"),
        _("Void"),
        _("Follow up")
    };  
    

    choiceStatus_ = new wxChoice( itemPanel7, ID_DIALOG_TRANS_STATUS, wxDefaultPosition, 
        wxSize(100, -1), 4, itemChoice7Strings, 0 );
    itemFlexGridSizer8->Add(choiceStatus_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);
	choiceStatus_->SetSelection(mmIniOptions::transStatusReconciled_);
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));

	////Type
    wxStaticText* itemStaticText5 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    wxString itemChoice6Strings[] = 
    {
        _("Withdrawal"),
        _("Deposit"),
        _("Transfer")
    };  
    
    choiceTrans_ = new wxChoice( itemPanel7, ID_DIALOG_TRANS_TYPE, wxDefaultPosition, 
        wxDefaultSize, 3, itemChoice6Strings, 0 );
    itemFlexGridSizer8->Add(choiceTrans_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);
    choiceTrans_->SetSelection(DEF_WITHDRAWAL);
    choiceTrans_->SetToolTip(_("Specify the type of transactions to be created."));

	//	
	staticTextAdvancedActive_ = new wxStaticText( itemPanel7, wxID_STATIC,
        _("Active"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(staticTextAdvancedActive_, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    bAdvanced_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONADVANCED, _("Advanced"), 
        wxDefaultPosition, wxSize(100, -1), 0 );
    bAdvanced_->Enable(false);
    itemFlexGridSizer8->Add(bAdvanced_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    bAdvanced_->SetToolTip(_("Specify the transfer amount in the To Account"));


	//Amount
    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Amount"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText23, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    textAmount_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(textAmount_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    textAmount_->SetToolTip(_("Specify the amount for this transaction"));


	//Payee
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel7, ID_DIALOG_TRANS_STATIC_PAYEE, 
        _("Payee"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    bPayee_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONPAYEE, _("Select Payee"), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bPayee_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    bPayee_->SetToolTip(_("Specify where the transaction is going to or coming from "));
    
    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel7, ID_DIALOG_TRANS_STATIC_FROM, _("To"), wxDefaultPosition, wxDefaultSize, 0 );
    
    bTo_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONTO, _("Select To Acct"), 
        wxDefaultPosition, wxSize(200, -1), 0 );

    itemFlexGridSizer8->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxUP, 10);
    itemFlexGridSizer8->Add(bTo_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxUP, 10);
	if (!edit_) 
    {itemFlexGridSizer8->AddSpacer(20);    itemFlexGridSizer8->AddSpacer(20);}
    itemStaticText13->Show(false);
    bTo_->Show(false);
//    bTo_->SetToolTip(_("Specify the transfer account"));
    bTo_->SetToolTip(_("Specify which account the transfer is going to"));

    if (!edit_ && !enterOccur_)
        dpc_->Enable(false);

    itemFlexGridSizer8->AddSpacer(20);
    cSplit_ = new wxCheckBox( itemPanel7, 
        ID_DIALOG_TRANS_SPLITCHECKBOX, _("Split"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cSplit_->SetValue(FALSE);
    itemFlexGridSizer8->Add(cSplit_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Category"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText17, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

     // ******************************** //
    
    bCategory_ = new wxButton( itemPanel7, 
        ID_DIALOG_TRANS_BUTTONCATEGS, _("Select Category"), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bCategory_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    bCategory_->SetToolTip(_("Specify the category for this transaction"));


    //Number
    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel7, wxID_STATIC, _("Number"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText11, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    textNumber_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNUMBER, wxT(""), 
        wxDefaultPosition, wxSize(100, -1), 0 );
    itemFlexGridSizer8->Add(textNumber_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));

    
    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText21, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    textNotes_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNOTES, wxT(""), 
        wxDefaultPosition, wxSize(200, 75), wxTE_MULTILINE );
    itemFlexGridSizer8->Add(textNotes_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));

    wxPanel* itemPanel25 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer20->Add(itemPanel25, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel25->SetSizer(itemBoxSizer26);

    wxButton* itemButton27 = new wxButton( itemPanel25, ID_DIALOG_TRANS_BUTTON_OK, _("OK"));
    itemButton27->SetForegroundColour(wxColour(wxT("FOREST GREEN")));
    itemBoxSizer26->Add(itemButton27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton28 = new wxButton( itemPanel25, wxID_CANCEL, _("Cancel"));
    itemButton28->SetForegroundColour(wxColour(wxT("RED")));
    itemBoxSizer26->Add(itemButton28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void mmBDDialog::OnCancel(wxCommandEvent& /*event*/)
{
    Close(TRUE);
}

void mmBDDialog::OnAccountName(wxCommandEvent& /*event*/)
{
	    static const char sql[] = 
    	"select ACCOUNTNAME "
    	"from ACCOUNTLIST_V1 "
       	"where (ACCOUNTTYPE = 'Checking' or ACCOUNTTYPE = 'Term') and STATUS != 'Closed' "
    	"order by ACCOUNTNAME";
	
    	wxArrayString as;
    	
    	wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    	while (q1.NextRow())
    	{
    	    as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    	}
    	q1.Finalize();
    	
    	wxSingleChoiceDialog scd(this, _("Choose Bank Account or Term Account"), _("Select Account"), as);
    	if (scd.ShowModal() == wxID_OK)
        {
            wxString acctName = scd.GetStringSelection();
            accountID_ = mmDBWrapper::getAccountID(db_, acctName);
            itemAccountName_->SetLabel(acctName);
    	}
}

void mmBDDialog::OnPayee(wxCommandEvent& /*event*/)
{
    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
	{
	    static const char sql[] = 
    	"select ACCOUNTNAME "
    	"from ACCOUNTLIST_V1 "
       	"where (ACCOUNTTYPE = 'Checking' or ACCOUNTTYPE = 'Term') and STATUS != 'Closed' "
    	"order by ACCOUNTNAME";
	
    	wxArrayString as;
    	
    	wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    	while (q1.NextRow())
    	{
    	    as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    	}
    	q1.Finalize();
    	
    	wxSingleChoiceDialog scd(this, _("Choose Bank Account or Term Account"), _("Select Account"), as);
    	if (scd.ShowModal() == wxID_OK) 
        {
            wxString acctName = scd.GetStringSelection();
		    payeeID_ = mmDBWrapper::getAccountID(db_, acctName);
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
	            bPayee_->SetLabel(wxT("Select Payee"));
                payeeUnknown_ = true;
                return;
            }
	            
            // ... If this is a Split Transaction, ignore the Payee change
            if (split_->numEntries())
                return;
	
            int tempCategID = -1;
            int tempSubCategID = -1;
            wxString payeeName = mmDBWrapper::getPayee(db_,	payeeID_, tempCategID, tempSubCategID);
            bPayee_->SetLabel(mmReadyDisplayString(payeeName));
            payeeUnknown_ = false;

    	    if (tempCategID == -1)
	        {
                return;
            }
	
            wxString catName = mmDBWrapper::getCategoryName(db_, tempCategID);
            wxString categString = catName;
	
            if (tempSubCategID != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_,
                    tempCategID, tempSubCategID);
                categString += wxT(" : ");
                categString += subcatName;
            }
	
            categID_ = tempCategID;
            subcategID_ = tempSubCategID;
            bCategory_->SetLabel(categString);
        }
	    else
	    {
            wxString payeeName = mmDBWrapper::getPayee(db_, payeeID_, categID_, subcategID_);
            if (payeeName.IsEmpty())
	        {
	            payeeID_ = -1;
	            categID_ = -1;
	            subcategID_ = -1;
	            bCategory_->SetLabel(_("Select Category"));
	            bPayee_->SetLabel(_("Select Payee"));
                payeeUnknown_ = true;
	        }
	        else
	        {
	            bPayee_->SetLabel(payeeName);
                payeeUnknown_ = false;
	        }
	        
	    }
	}
}

void mmBDDialog::OnTo(wxCommandEvent& /*event*/)
{	
    // This should only get called if we are in a transfer
    static const char sql[] = 
    "select ACCOUNTNAME "
    "from ACCOUNTLIST_V1 "
    "where (ACCOUNTTYPE = 'Checking' or ACCOUNTTYPE = 'Term') and STATUS != 'Closed' "
    "order by ACCOUNTNAME";
	
    wxArrayString as;
    	
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    q1.Finalize();
    	
    wxSingleChoiceDialog scd(this, _("Choose Bank Account or Term Account"), _("Select Account"), as);
    if (scd.ShowModal() == wxID_OK)
    {
	    wxString acctName = scd.GetStringSelection();
        toID_ = mmDBWrapper::getAccountID(db_, acctName);
        bTo_->SetLabel(acctName);
    }
}

void mmBDDialog::OnDateChanged(wxDateEvent& /*event*/)
{
    
}

void mmBDDialog::OnAdvanced(wxCommandEvent& /*event*/)
{
    wxString dispString = textAmount_->GetValue();
    if (toTransAmount_ >= 0.0)
    {
        mmex::formatDoubleToCurrencyEdit(toTransAmount_, dispString);
    }
    wxTextEntryDialog dlg(this, _("Amount to be recorded in To Account"), _("To Account Amount Entry"),  dispString);
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxString currText = dlg.GetValue().Trim();
        if (!currText.IsEmpty())
        {
            double amount;
            if (!mmex::formatCurrencyToDouble(currText, amount) || (amount < 0.0))
            {
                mmShowErrorMessage(this, _("Invalid To Amount Entered "), _("Error"));
            }
            else
            {
                toTransAmount_ = amount;
                advancedToTransAmountSet_ = true;
                staticTextAdvancedActive_->Show();
            }
        }
    }
}

void mmBDDialog::OnCategs(wxCommandEvent& /*event*/)
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
      if ( dlg.ShowModal() == wxID_OK )
      {
         if (dlg.categID_ == -1)
         {
            // check if categ and subcateg are now invalid
            wxString catName = mmDBWrapper::getCategoryName(db_, categID_);
            if (catName.IsEmpty())
            {
               // cannot find category
               categID_ = -1;
               subcategID_ = -1;
               bCategory_->SetLabel(_("Select Category"));
               return;
            }

            if (dlg.subcategID_ != -1)
            {
               wxString subcatName = mmDBWrapper::getSubCategoryName(db_, categID_, subcategID_);
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

         wxString catName = mmDBWrapper::getCategoryName(db_, dlg.categID_);
         catName.Replace(wxT("&"), wxT("&&"));
         wxString categString = catName;

         if (dlg.subcategID_ != -1)
         {
            wxString subcatName = mmDBWrapper::getSubCategoryName(db_, dlg.categID_, dlg.subcategID_);
            subcatName.Replace(wxT("&"), wxT("&&"));
            categString += wxT(" : ");
            categString += subcatName;
         }

         bCategory_->SetLabel(categString);
      }
   }
}

void mmBDDialog::OnTransTypeChanged(wxCommandEvent& /*event*/)
{
    updateControlsForTransType();
}

void mmBDDialog::updateControlsForTransType()
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_FROM);
    wxStaticText* stp = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_PAYEE);
    
     if (choiceTrans_->GetSelection() == DEF_WITHDRAWAL)
     {
        fillControls();
        st->Show(false);
        bTo_->Show(false);
        stp->SetLabel(_("Payee"));
        if (payeeUnknown_)
        {
            bPayee_->SetLabel(_("Select Payee"));
            payeeID_ = -1;
            toID_    = -1;
        }
        bAdvanced_->Enable(false);
        bPayee_->SetToolTip(_("Specify where the transaction is going to or coming from "));
        textAmount_->SetToolTip(_("Specify the amount for this transaction"));
     }
     else if (choiceTrans_->GetSelection() == DEF_DEPOSIT)
    {
        fillControls();
        bTo_->Show(false);
        st->Show(false);    
        stp->SetLabel(_("From"));
        if (payeeUnknown_)
        {
            bPayee_->SetLabel(_("Select Payee"));
            payeeID_ = -1;
            toID_    = -1;
        }
        bAdvanced_->Enable(false);
        bPayee_->SetToolTip(_("Specify where the transaction is going to or coming from "));
        textAmount_->SetToolTip(_("Specify the amount for this transaction"));
    }
    else if (choiceTrans_->GetSelection() == DEF_TRANSFER)
    {
        if (accountID_ < 0 )
        {
            bPayee_->SetLabel(_("Select From"));
            payeeID_ = -1;
        } 
        else
        {
            bPayee_->SetLabel(itemAccountName_->GetLabel());
            payeeID_ = accountID_;
        }

        bTo_->SetLabel(_("Select To"));
        toID_    = -1;
        payeeUnknown_ = true;

        bTo_->Show(true);
        st->Show(true);
        stp->SetLabel(_("From"));   
        bAdvanced_->Enable(true);
        bPayee_->SetToolTip(_("Specify which account the transfer is comming from"));
        textAmount_->SetToolTip(_("Specify the transfer amount in the From and To Account"));
    }
}

void mmBDDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString transCode; 
        
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
            mmShowErrorMessageInvalid(this, _("Payee "));
        else
            mmShowErrorMessageInvalid(this, _("From Account "));
        return;
    }

    if (cSplit_->GetValue())
    {
        if (split_->numEntries() == 0)
        {
            mmShowErrorMessageInvalid(this, _("Category "));
            return;
        }
    }
    else
    {
        if (categID_ == -1)
        {
            mmShowErrorMessageInvalid(this, _("Category "));
            return;
        }
    }

    double amount;
    if (cSplit_->GetValue())
    {
        amount = split_->getTotalSplits();
        if (amount < 0.0)
        {
            mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
            return;
        }
    }
    else
    {
        wxString amountStr = textAmount_->GetValue().Trim();
        if (!mmex::formatCurrencyToDouble(amountStr, amount) 
            || (amount < 0.0))
        {
            mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
            return;
        }
    }

    if ((transCode != TRANS_TYPE_TRANSFER_STR) && (accountID_ == -1))
    {
        mmShowErrorMessageInvalid(this, _("Account"));
        return;
    }

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
        wxString payeeName = mmDBWrapper::getPayee(db_, payeeID_, catID, subcatID );
        mmDBWrapper::updatePayee(db_, 
            payeeName, payeeID_, categID_, subcategID_);
    }

    if (!advancedToTransAmountSet_ || toTransAmount_ < 0)
    {
        // if we are adding a new record and the user did not touch advanced dialog
        // we are going to use the transfer amount by calculating conversion rate.
        // subsequent edits will not allow automatic update of the amount
        if (!edit_)
        {
            double rateFrom = mmDBWrapper::getCurrencyBaseConvRate(db_, fromAccountID);
            double rateTo = mmDBWrapper::getCurrencyBaseConvRate(db_, toAccountID);

            double convToBaseFrom = rateFrom * amount;
            toTransAmount_ = convToBaseFrom / rateTo;
        }
        else
        {
            // to trans amount not set
            toTransAmount_ = amount;
        }
    }
    
    wxString transNum = textNumber_->GetValue();
    wxString notes = textNotes_->GetValue();
    wxString status = wxT(""); // nothing yet
    int repeats = itemRepeats_->GetSelection();
    // Multiplex Auto executable onto the repeat field of the database. 
    if (autoExecuteUserAck_)
        repeats += BD_REPEATS_MULTIPLEX_BASE;
    if (autoExecuteSilent_)
        repeats += BD_REPEATS_MULTIPLEX_BASE;

    wxString numRepeatStr = textNumRepeats_->GetValue();
    int numRepeats = -1;
    
    if (!numRepeatStr.empty()) {
        long cnt = 0;
        if (numRepeatStr.ToLong(&cnt))
        {
            wxASSERT(std::numeric_limits<int>::min() <= cnt);
            wxASSERT(cnt <= std::numeric_limits<int>::max());
            numRepeats = static_cast<int>(cnt);
        }
    }
    
    wxString nextOccurDate = dpcbd_->GetValue().FormatISODate();

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
    
    wxString date1 = dpc_->GetValue().FormatISODate();

    if (!edit_ && !enterOccur_)
    {
        static const char sql[] = 
        "insert into BILLSDEPOSITS_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, "
          "TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,"
          "CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT, REPEATS, "
          "NEXTOCCURRENCEDATE, NUMOCCURRENCES) "
        "values ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, -1, ?, ?, ?, ? )";
        
        wxSQLite3Statement st = db_->PrepareStatement(sql);
        
        int i = 0;
        st.Bind(++i, fromAccountID);
        st.Bind(++i, toAccountID);
        st.Bind(++i, payeeID_);
        st.Bind(++i, transCode);
        st.Bind(++i, amount);
        st.Bind(++i, status);
        st.Bind(++i, transNum);
        st.Bind(++i, notes);
        st.Bind(++i, categID_);
        st.Bind(++i, subcategID_);
        st.Bind(++i, date1);
        st.Bind(++i, toTransAmount_);
        st.Bind(++i, repeats);
        st.Bind(++i, nextOccurDate);
        st.Bind(++i, numRepeats);

        wxASSERT(st.GetParamCount() == i);

        st.ExecuteUpdate();
		int transID = db_->GetLastRowId().ToLong();
        st.Finalize();
        
        // --

        static const char sql_ins[] = 
        "insert into BUDGETSPLITTRANSACTIONS_V1 (TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT) "
        "values (?, ?, ?, ?)";

        st = db_->PrepareStatement(sql_ins);

        for (size_t i = 0; i < split_->numEntries(); ++i)
		{
            mmSplitTransactionEntry &r = *split_->entries_[i];

            st.Bind(1, transID);
            st.Bind(2, r.categID_);
            st.Bind(3, r.subCategID_);
            st.Bind(4, r.splitAmount_);

            st.ExecuteUpdate();
			r.splitEntryID_ = db_->GetLastRowId().ToLong();
            
            st.Reset();
		}

        st.Finalize();
    }
    else if (edit_)
    {
        static const char sql[] = 
        "update BILLSDEPOSITS_V1 "
        "SET ACCOUNTID=?, TOACCOUNTID=?, PAYEEID=?, TRANSCODE=?,"
            "TRANSAMOUNT=?, STATUS=?, TRANSACTIONNUMBER=?, NOTES=?,"
            "CATEGID=?, SUBCATEGID=?, TRANSDATE=?, TOTRANSAMOUNT=?, REPEATS=?, "
            "NEXTOCCURRENCEDATE=?, NUMOCCURRENCES=? "
        "WHERE BDID=?";

        wxSQLite3Statement st = db_->PrepareStatement(sql);

        int i = 0;
        st.Bind(++i, accountID_);
        st.Bind(++i, toAccountID);
        st.Bind(++i, payeeID_);
        st.Bind(++i, transCode);
        st.Bind(++i, amount);
        st.Bind(++i, status);
        st.Bind(++i, transNum);
        st.Bind(++i, notes);
        st.Bind(++i, categID_);
        st.Bind(++i, subcategID_);
        st.Bind(++i, date1);
        st.Bind(++i, toTransAmount_);
        st.Bind(++i, repeats);
        st.Bind(++i, nextOccurDate);
        st.Bind(++i, numRepeats);
        st.Bind(++i, bdID_);

        wxASSERT(st.GetParamCount() == i);

        st.ExecuteUpdate();
        st.Finalize();

        // --

		st = db_->PrepareStatement("delete from BUDGETSPLITTRANSACTIONS_V1 where TRANSID = ?");	
        st.Bind(1, bdID_);
        st.ExecuteUpdate();
        st.Finalize();

        // --

		static const char sql_ins[] = 
        "insert into BUDGETSPLITTRANSACTIONS_V1 (TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT) "
        "values (?, ?, ?, ?)";

        st = db_->PrepareStatement(sql_ins);
        
        for (size_t i = 0; i < split_->numEntries(); ++i)
		{
            mmSplitTransactionEntry &r = *split_->entries_[i];

            st.Bind(1, bdID_);
            st.Bind(2, r.categID_);
            st.Bind(3, r.subCategID_);
            st.Bind(4, r.splitAmount_);
			
            st.ExecuteUpdate();
			r.splitEntryID_ = db_->GetLastRowId().ToLong();
            
            st.Reset();
		}

        st.Finalize();
    }
    else if (enterOccur_)
    {
        boost::shared_ptr<mmBankTransaction> pTransaction;
        boost::shared_ptr<mmBankTransaction> pTemp(new mmBankTransaction(core_->db_));
        pTransaction = pTemp;

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
        pTransaction->updateAllData(core_, fromAccountID, pCurrencyPtr);
        core_->bTransactionList_.addTransaction(core_, pTransaction);
        mmDBWrapper::completeBDInSeries(db_, bdID_);

    }

    EndModal(wxID_OK);
}

void mmBDDialog::OnSplitChecked(wxCommandEvent& /*event*/)
{
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

void mmBDDialog::OnAutoExecutionUserAckChecked(wxCommandEvent& /*event*/)
{
    autoExecuteUserAck_ = ! autoExecuteUserAck_;
    wxString msg;

    if (autoExecuteUserAck_)
    {
        itemCheckBoxAutoExeSilent_->Enable(true);
        msg = _("Automatic Execution with required user acknowledgement."); 
    }
    else
    {
        itemCheckBoxAutoExeSilent_->SetValue(false);
        itemCheckBoxAutoExeSilent_->Enable(false);
        autoExecuteSilent_ = false;
    }

    if (msg != wxT(""))
        wxMessageBox(msg ,REPEAT_TRANSACTIONS_MSGBOX_HEADING);
}

void mmBDDialog::OnAutoExecutionSilentChecked(wxCommandEvent& /*event*/)
{
    autoExecuteSilent_ = ! autoExecuteSilent_;
    wxString msg;
    if (autoExecuteSilent_)
    {
        if (autoExecuteUserAck_)
        {
            msg = _("Execution changed to occur without user interaction");
        }
    }
    if (msg != wxT(""))
    wxMessageBox(msg ,REPEAT_TRANSACTIONS_MSGBOX_HEADING);
}

void mmBDDialog::OnCalendarSelChanged(wxCalendarEvent& event)
{
	wxDateTime date = event.GetDate();
	dpcbd_->SetValue(date) ;
	//mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
}

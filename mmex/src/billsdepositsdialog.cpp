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
    EVT_BUTTON(wxID_OK, mmBDDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmBDDialog::OnCategs)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONPAYEE, mmBDDialog::OnPayee)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTO, mmBDDialog::OnTo)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmBDDialog::OnTransTypeChanged)
    EVT_SPIN_UP(ID_DIALOG_TRANS_DATE_SPINNER,mmBDDialog::OnTransDateForward)
    EVT_SPIN_DOWN(ID_DIALOG_TRANS_DATE_SPINNER,mmBDDialog::OnTransDateBack)
    EVT_SPIN_UP(ID_DIALOG_BD_REPEAT_DATE_SPINNER,mmBDDialog::OnNextOccurDateForward)
    EVT_SPIN_DOWN(ID_DIALOG_BD_REPEAT_DATE_SPINNER,mmBDDialog::OnNextOccurDateBack)
    EVT_CHECKBOX(ID_DIALOG_TRANS_ADVANCED_CHECKBOX, mmBDDialog::OnAdvanceChecked)
    EVT_BUTTON(ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME, mmBDDialog::OnAccountName)
    EVT_CHECKBOX(ID_DIALOG_TRANS_SPLITCHECKBOX, mmBDDialog::OnSplitChecked)
    EVT_CHECKBOX(ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_USERACK, mmBDDialog::OnAutoExecutionUserAckChecked)
    EVT_CHECKBOX(ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_SILENT, mmBDDialog::OnAutoExecutionSilentChecked)
    EVT_CALENDAR_SEL_CHANGED(ID_DIALOG_BD_CALENDAR, mmBDDialog::OnCalendarSelChanged)
    EVT_DATE_CHANGED(ID_DIALOG_TRANS_BUTTONDATE, mmBDDialog::OnDateChanged) 
    EVT_DATE_CHANGED(ID_DIALOG_BD_BUTTON_NEXTOCCUR, mmBDDialog::OnDateChanged)
    EVT_CHOICE(ID_DIALOG_BD_COMBOBOX_REPEATS, mmBDDialog::OnRepeatTypeChanged)  
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTRANSNUM, mmBDDialog::OnsetNextRepeatDate)
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
    
    boost::shared_ptr<mmSplitTransactionEntries> split(new mmSplitTransactionEntries());
    split_ = split;

    /**********************************************************************************************
     Ament controls according to function settings
    ***********************************************************************************************/
    if (edit_ || enterOccur_)
    {
        dataToControls();
        if (enterOccur_)
        {
            choiceTrans_->Disable();
            dpcbd_->Disable();
            itemRepeats_->Disable();
            itemAccountName_->Disable();
            textAmount_->SetFocus();
            itemCheckBoxAutoExeSilent_->Disable();
            itemCheckBoxAutoExeUserAck_->Disable();
            textNumRepeats_->Disable();
        } else
            dpc_->Disable();
    }

    Centre();
    Fit();
    return TRUE;
}

void mmBDDialog::dataToControls()
{
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
		calendarCtrl_->SetDate (dtno);
		
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
        setRepeatDetails();
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
            {
                cAdvanced_->SetValue(true);
                SetAdvancedTransferControls(true);
            }
        }
    }
    
    st.Finalize();
    st_payee.Finalize();
}

void mmBDDialog::CreateControls()
{    
    mmBDDialog* itemDialog1 = this;

    wxBoxSizer* mainBoxSizerOuter = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* mainBoxSizerInner = new wxBoxSizer(wxHORIZONTAL);

    /* Repeat Transaction Details */
    wxBoxSizer* repeatTransBoxSizer = new wxBoxSizer(wxVERTICAL);
 
    /* Transaction Details */
    wxStaticBox* transDetailsStaticBox = new wxStaticBox(itemDialog1, wxID_ANY, _("Transaction Details") );
    wxStaticBoxSizer* transDetailsStaticBoxSizer = new wxStaticBoxSizer(transDetailsStaticBox, wxVERTICAL);

    itemDialog1->SetSizer(mainBoxSizerOuter);

    /* Calendar */
    wxStaticBox* calendarStaticBox = new wxStaticBox(itemDialog1, wxID_ANY, _("Calendar") );
    wxStaticBoxSizer* calendarStaticBoxSizer = new wxStaticBoxSizer(calendarStaticBox, wxHORIZONTAL);
    repeatTransBoxSizer->Add(calendarStaticBoxSizer, 10, wxALIGN_CENTER|wxLEFT|wxBOTTOM|wxRIGHT, 15);

	//TODO: Set these up as user selectable. Some users wish to have monday first in calendar!
    bool startSunday = true;
    bool showSuroundingWeeks = true;

    int style = wxSUNKEN_BORDER| wxCAL_SHOW_HOLIDAYS| wxCAL_SEQUENTIAL_MONTH_SELECTION;
    if (startSunday)
        style = wxCAL_SUNDAY_FIRST| style;
    else
        style = wxCAL_MONDAY_FIRST| style;

    if (showSuroundingWeeks)
        style = wxCAL_SHOW_SURROUNDING_WEEKS| style;
    
    calendarCtrl_ = new wxCalendarCtrl( itemDialog1, ID_DIALOG_BD_CALENDAR, wxDateTime(), 
                                        wxDefaultPosition, wxDefaultSize, style);
    calendarStaticBoxSizer->Add(calendarCtrl_, 10, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
	
    /* Bills & Deposits Details */
    wxStaticBox* repeatDetailsStaticBox = new wxStaticBox(itemDialog1, wxID_ANY, _("Repeating Transaction Details") );
    wxStaticBoxSizer* repeatDetailsStaticBoxSizer = new wxStaticBoxSizer(repeatDetailsStaticBox, wxHORIZONTAL);
    repeatTransBoxSizer->Add(repeatDetailsStaticBoxSizer, 0, wxALIGN_CENTER|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(4, 2, 10, 10);
    repeatDetailsStaticBoxSizer->Add(itemFlexGridSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* staticTextAccName = new wxStaticText( itemDialog1, wxID_STATIC, _("Account Name") );
    itemFlexGridSizer5->Add(staticTextAccName, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    itemAccountName_ = new wxButton( itemDialog1, ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME, _("Select Account"), 
                                     wxDefaultPosition, wxSize(180, -1), 0 );
    itemFlexGridSizer5->Add(itemAccountName_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemAccountName_->SetToolTip(_("Specify the Account that will own the repeating transaction"));

// change properties depending on system parameters
    wxSize spinCtrlSize = wxSize(16,-1);
    int spinCtrlDirection = wxSP_VERTICAL;
    int interval = 0;
#ifdef __WXMSW__
    spinCtrlSize = wxSize(40,14);
    spinCtrlDirection = wxSP_HORIZONTAL;
    interval = 4;
#endif

    // Next Occur Date --------------------------------------------
    wxStaticText* staticTextNextOccur = new wxStaticText( itemDialog1, wxID_STATIC, _("Next Occurrence") );
    dpcbd_ = new wxDatePickerCtrl( itemDialog1, ID_DIALOG_BD_BUTTON_NEXTOCCUR, wxDefaultDateTime,
                                   wxDefaultPosition, wxSize(110,-1), wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    dpcbd_->SetToolTip(_("Specify the date of the next bill or deposit"));

    spinNextOccDate_ = new wxSpinButton( itemDialog1, ID_DIALOG_BD_REPEAT_DATE_SPINNER,
                                         wxDefaultPosition, spinCtrlSize,spinCtrlDirection|wxSP_ARROW_KEYS|wxSP_WRAP);
	spinNextOccDate_->SetToolTip(_("Retard or advance the date of the 'next occurrence"));

	wxBoxSizer* nextOccurDateBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    nextOccurDateBoxSizer->Add(dpcbd_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
	nextOccurDateBoxSizer->Add(spinNextOccDate_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, interval);

    itemFlexGridSizer5->Add(staticTextNextOccur, 0,
                            wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    itemFlexGridSizer5->Add(nextOccurDateBoxSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Repeats --------------------------------------------
    staticTextRepeats_ = new wxStaticText( itemDialog1, wxID_STATIC, _("Repeats") );
    itemFlexGridSizer5->Add(staticTextRepeats_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    
    wxString repeatsStrChoiceArray[] = 
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
        _("In (x) Days"),
        _("In (x) Months"),
        _("Every (x) Days"),
        _("Every (x) Months"),
    };  
    itemRepeats_ = new wxChoice( itemDialog1, ID_DIALOG_BD_COMBOBOX_REPEATS, wxDefaultPosition, 
                                 wxSize(110, -1), 15, repeatsStrChoiceArray, 0);

    wxBoxSizer* repeatBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    bSetNextOccurDate_ = new wxButton( itemDialog1, ID_DIALOG_TRANS_BUTTONTRANSNUM, _("Next"),
                                       wxDefaultPosition, wxSize(60, -1));
    bSetNextOccurDate_->SetToolTip(_("Advance the Next Occurance Date with the specified values"));
    repeatBoxSizer->Add(itemRepeats_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    repeatBoxSizer->Add(bSetNextOccurDate_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);

    itemFlexGridSizer5->Add(repeatBoxSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    itemRepeats_->SetSelection(0);

    // Repeat Times --------------------------------------------
    staticTimesRepeat_ = new wxStaticText( itemDialog1, wxID_STATIC, _("Times Repeated") );
    itemFlexGridSizer5->Add(staticTimesRepeat_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    textNumRepeats_ = new wxTextCtrl( itemDialog1, ID_DIALOG_BD_TEXTCTRL_NUM_TIMES, wxT(""),
                                      wxDefaultPosition, wxSize(110, -1), 0 );
    itemFlexGridSizer5->Add(textNumRepeats_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    setRepeatDetails();

    /* Auto Execution Status */
    itemCheckBoxAutoExeUserAck_ = new wxCheckBox( itemDialog1, ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_USERACK, 
        _("Set to 'Auto Execute' on the 'Next Occurrence' date."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBoxAutoExeUserAck_->SetToolTip(_("Automatic Execution will require user acknowledgement."));

    itemCheckBoxAutoExeSilent_ = new wxCheckBox( itemDialog1, ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_SILENT, 
        _("Set 'Auto Execute' without user acknowlegement."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBoxAutoExeSilent_->Disable();
    itemCheckBoxAutoExeSilent_->SetToolTip(_("Automatic Execution will occur without user interaction"));

    repeatTransBoxSizer->Add(itemCheckBoxAutoExeUserAck_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    repeatTransBoxSizer->Add(itemCheckBoxAutoExeSilent_, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5);

    /************************************************************************************************************
    transactionPanel controlled by transPanelSizer - is contained in the transDetailsStaticBoxSizer.
    *************************************************************************************************************/
    wxPanel* transactionPanel = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    transDetailsStaticBoxSizer->Add(transactionPanel, 0, wxGROW|wxALL, 10);

    wxFlexGridSizer* transPanelSizer = new wxFlexGridSizer(5, 2, 10, 10);
    transactionPanel->SetSizer(transPanelSizer);

    // Trans Date --------------------------------------------
    wxStaticText* staticTextDate = new wxStaticText( transactionPanel, wxID_STATIC, _("Date"));
    dpc_ = new wxDatePickerCtrl( transactionPanel, ID_DIALOG_TRANS_BUTTONDATE, wxDefaultDateTime, 
                                 wxDefaultPosition, wxSize(110, -1), wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    dpc_->SetToolTip(_("Specify the date of the transaction"));
    spinTransDate_ = new wxSpinButton( transactionPanel,ID_DIALOG_TRANS_DATE_SPINNER,
                                       wxDefaultPosition, spinCtrlSize,spinCtrlDirection|wxSP_ARROW_KEYS|wxSP_WRAP);
	spinTransDate_->SetToolTip(_("Retard or advance the date of the transaction"));

	wxBoxSizer* transDateBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    transDateBoxSizer->Add(dpc_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
	transDateBoxSizer->Add(spinTransDate_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, interval);

    transPanelSizer->Add(staticTextDate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    transPanelSizer->Add(transDateBoxSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    // Status --------------------------------------------
    wxStaticText* staticTextStatus = new wxStaticText( transactionPanel, wxID_STATIC, _("Status"));
    wxString statusChoiceStrArray[] = 
    {
        _("None"),
        _("Reconciled"),
        _("Void"),
        _("Follow up")
    };  
    choiceStatus_ = new wxChoice( transactionPanel, ID_DIALOG_TRANS_STATUS,
                                  wxDefaultPosition, wxSize(110, -1), 4, statusChoiceStrArray, 0 );
	choiceStatus_->SetSelection(mmIniOptions::transStatusReconciled_);
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));

    transPanelSizer->Add(staticTextStatus, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    transPanelSizer->Add(choiceStatus_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Type --------------------------------------------
    wxStaticText* staticTextType = new wxStaticText( transactionPanel, wxID_STATIC, _("Type"));
    wxString typeChoiceStrArray[] = 
    {
        _("Withdrawal"),
        _("Deposit"),
        _("Transfer")
    };  
    choiceTrans_ = new wxChoice( transactionPanel, ID_DIALOG_TRANS_TYPE, 
                                 wxDefaultPosition, wxSize(110, -1), 3, typeChoiceStrArray, 0 );
    choiceTrans_->SetSelection(DEF_WITHDRAWAL);
    choiceTrans_->SetToolTip(_("Specify the type of transactions to be created."));
    cAdvanced_ = new wxCheckBox( transactionPanel, ID_DIALOG_TRANS_ADVANCED_CHECKBOX, _("Advanced"),
                                 wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cAdvanced_->SetValue(FALSE);
    cAdvanced_->SetToolTip(_("Allows the setting of different amounts in the FROM and TO accounts."));    

    wxBoxSizer* typeSizer = new wxBoxSizer(wxHORIZONTAL);
    typeSizer->Add(choiceTrans_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    typeSizer->Add(cAdvanced_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxALL, 5);

    transPanelSizer->Add(staticTextType, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    transPanelSizer->Add(typeSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

	// Amount Fields --------------------------------------------
    amountNormalTip_   = _("Specify the amount for this transaction");
    amountTransferTip_ = _("Specify the amount to be transfered"); 

    wxStaticText* staticTextAmount = new wxStaticText( transactionPanel, wxID_STATIC, _("Amount"));

    textAmount_ = new wxTextCtrl( transactionPanel, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""),
                                  wxDefaultPosition, wxSize(110, -1), wxALIGN_RIGHT );
    textAmount_->SetToolTip(amountNormalTip_);

    toTextAmount_ = new wxTextCtrl( transactionPanel, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""),
                                    wxDefaultPosition, wxSize(110, -1), wxALIGN_RIGHT );
    toTextAmount_->SetToolTip(_("Specify the transfer amount in the To Account"));

    wxBoxSizer* amountSizer = new wxBoxSizer(wxHORIZONTAL);
    amountSizer->Add(textAmount_,   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 0);
    amountSizer->Add(toTextAmount_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    transPanelSizer->Add(staticTextAmount, 0, wxALIGN_LEFT|wxALL, 0);
    transPanelSizer->Add(amountSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Payee ------------------------------------------------
    wxStaticText* staticTextPayee = new wxStaticText( transactionPanel, ID_DIALOG_TRANS_STATIC_PAYEE,_("Payee") );

    bPayee_ = new wxButton( transactionPanel, ID_DIALOG_TRANS_BUTTONPAYEE, _("Select Payee"),
                            wxDefaultPosition, wxSize(225, -1), 0 );
    payeeWithdrawalTip_ = _("Specify where the transaction is going to");
    payeeDepositTip_    = _("Specify where the transaction is coming from");
    bPayee_->SetToolTip(payeeWithdrawalTip_);

    transPanelSizer->Add(staticTextPayee, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    transPanelSizer->Add(bPayee_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    
    // Payee Alternate ------------------------------------------------
    wxStaticText* staticTextTo = new wxStaticText( transactionPanel, ID_DIALOG_TRANS_STATIC_FROM, _(" ") );
    bTo_ = new wxButton( transactionPanel, ID_DIALOG_TRANS_BUTTONTO, _("Select To Acct"),
                         wxDefaultPosition, wxSize(225, -1), 0 );
    bTo_->SetToolTip(_("Specify which account the transfer is going to"));

    transPanelSizer->Add(staticTextTo, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxUP, 0);
    transPanelSizer->Add(bTo_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxUP, 0);

    // Split Category -------------------------------------------
    cSplit_ = new wxCheckBox( transactionPanel, ID_DIALOG_TRANS_SPLITCHECKBOX, _("Split"),
                              wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cSplit_->SetValue(FALSE);
    cSplit_->SetToolTip(_("Use split Categories"));

    transPanelSizer->AddSpacer(20); // Fill empty space.
    transPanelSizer->Add(cSplit_, 0, wxALIGN_BOTTOM|wxALIGN_LEFT|wxALL, 0);

    // Category ---------------------------------------------
    wxStaticText* staticTextCategory = new wxStaticText( transactionPanel, wxID_STATIC, _("Category"));
    bCategory_ = new wxButton( transactionPanel, ID_DIALOG_TRANS_BUTTONCATEGS, _("Select Category"),
                               wxDefaultPosition, wxSize(225, -1), 0 );
    //bCategory_->SetToolTip(_("Specify the category for this transaction"));
    
    transPanelSizer->Add(staticTextCategory, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    transPanelSizer->Add(bCategory_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    // Number ---------------------------------------------
    wxStaticText* staticTextNumber = new wxStaticText( transactionPanel, wxID_STATIC, _("Number"));
    textNumber_ = new wxTextCtrl( transactionPanel, ID_DIALOG_TRANS_TEXTNUMBER, wxT(""),
                                  wxDefaultPosition, wxSize(185, -1), 0 );
    textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));
    
    transPanelSizer->Add(staticTextNumber, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    transPanelSizer->Add(textNumber_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    
    // Notes ---------------------------------------------
    wxStaticText* staticTextNotes = new wxStaticText( transactionPanel, wxID_STATIC, _("Notes"));
    textNotes_ = new wxTextCtrl( transactionPanel, ID_DIALOG_TRANS_TEXTNOTES, wxT(""),
                                 wxDefaultPosition, wxSize(225, 80), wxTE_MULTILINE );
    textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));
   
    transPanelSizer->Add(staticTextNotes, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);
    transPanelSizer->Add(textNotes_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    SetTransferControls();  // hide appropriate fields
    /**********************************************************************************************
     Button Panel with OK and Cancel Buttons
    ***********************************************************************************************/
    wxPanel* buttonsPanel = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* buttonsPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsPanel->SetSizer(buttonsPanelSizer);

    wxButton* okButton = new wxButton( buttonsPanel, wxID_OK, _("OK"));
    buttonsPanelSizer->Add(okButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* cancelButton = new wxButton( buttonsPanel, wxID_CANCEL, _("Cancel"));
    buttonsPanelSizer->Add(cancelButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    cancelButton->SetFocus();

    /**********************************************************************************************
     Determining where the controls go
    ***********************************************************************************************/
    //mainBoxSizerInner will allign contents horizontally
    mainBoxSizerInner->Add(repeatTransBoxSizer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    mainBoxSizerInner->Add(transDetailsStaticBoxSizer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //mainBoxSizerOuter will allign contents vertically
    mainBoxSizerOuter->Add(mainBoxSizerInner, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxRIGHT, 5);
    mainBoxSizerOuter->Add(buttonsPanel, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM|wxRIGHT, 5);

    /**********************************************************************************************
     Adjust controls according to function settings
    ***********************************************************************************************/
    if (enterOccur_) {
        spinNextOccDate_->Disable();
    } else {
        dpc_->Disable();
        spinTransDate_->Disable();
    }
}

void mmBDDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
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
    	
    	wxString acctName = itemAccountName_->GetLabel();
    	bPayee_->SetLabel(acctName);
    	
    	wxSingleChoiceDialog scd(this, _("Choose Bank Account or Term Account"), _("Select Account"), as);
    	if (scd.ShowModal() == wxID_OK) 
        {
            acctName = scd.GetStringSelection();
		    payeeID_ = mmDBWrapper::getAccountID(db_, acctName);
		    bPayee_->SetLabel(acctName);
		    itemAccountName_->SetLabel(acctName);
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
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_, tempCategID, tempSubCategID);
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

void mmBDDialog::displayControlsForType( int transType, bool enableAdvanced )
{
    wxStaticText* stFrom = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_FROM);
    stFrom->Enable(enableAdvanced);
    bTo_->Enable(enableAdvanced);

    bPayee_->SetToolTip(_("Specify where the transaction is going to or coming from "));
    textAmount_->SetToolTip(_("Specify the amount for this transaction"));

    if (transType == DEF_TRANSFER) {
        bPayee_->SetToolTip(_("Specify which account the transfer is comming from"));
        textAmount_->SetToolTip(amountTransferTip_);
    } else {
        bPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));
        textAmount_->SetToolTip(amountNormalTip_);
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
        displayControlsForType(DEF_WITHDRAWAL);
        SetTransferControls();
        stp->SetLabel(_("Payee"));
        st->SetLabel(wxT(""));
        bPayee_->SetToolTip(payeeWithdrawalTip_);
        if (payeeUnknown_)
            resetPayeeString();
    }
    else if (choiceTrans_->GetSelection() == DEF_DEPOSIT)
    {
        displayControlsForType(DEF_DEPOSIT);
        SetTransferControls();
        stp->SetLabel(_("From"));
        st->SetLabel(wxT(""));
        bPayee_->SetToolTip(payeeDepositTip_);
        if (payeeUnknown_)
            resetPayeeString();
    }
    else if (choiceTrans_->GetSelection() == DEF_TRANSFER)
    {
        displayControlsForType(DEF_TRANSFER, true);
        if (accountID_ < 0 )
        {
            bPayee_->SetLabel(_("Select From Account"));
            payeeID_ = -1;
        }
        else
        {
            bPayee_->SetLabel(itemAccountName_->GetLabel());
            payeeID_ = accountID_;
        }

        SetTransferControls(true);
        if (cAdvanced_->IsChecked())
            SetAdvancedTransferControls(true);

        stp->SetLabel(_("From"));   
        st->SetLabel(_("To"));   
        bTo_->SetLabel(_("Select To Account"));
        toID_    = -1;
        payeeUnknown_ = true;
    }
}

void mmBDDialog::resetPayeeString(bool normal)
{
    bPayee_->SetLabel(_("Select Payee"));
    payeeID_ = -1;
    if (normal)
        toID_    = -1;
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
            mmShowErrorMessageInvalid(this, _("Amount"));
            return;
        }
    }
    else
    {
        wxString amountStr = textAmount_->GetValue().Trim();
        if (!mmex::formatCurrencyToDouble(amountStr, amount) || (amount < 0.0))
        {
            mmShowErrorMessageInvalid(this, _("Amount"));
            return;
        }
    }

    if (advancedToTransAmountSet_)
    {
        wxString amountStr = toTextAmount_->GetValue().Trim();
        if (!mmex::formatCurrencyToDouble(amountStr, toTransAmount_) || (toTransAmount_ < 0.0))
        {
            mmShowErrorMessageInvalid(this, _("Advanced Amount"));
            return;
        }
    } else
        toTransAmount_ = amount;

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
        //mmDBWrapper::updatePayee(db_, payeeName, payeeID_, categID_, subcategID_);
    }

    if (!advancedToTransAmountSet_ || toTransAmount_ < 0)
    {
        // if we are adding a new record and the user did not touch advanced dialog
        // we are going to use the transfer amount by calculating conversion rate.
        // subsequent edits will not allow automatic update of the amount
        if (!edit_)
        {
            if(toAccountID != -1) {
                double rateFrom = mmDBWrapper::getCurrencyBaseConvRate(db_, fromAccountID);
                double rateTo = mmDBWrapper::getCurrencyBaseConvRate(db_, toAccountID);

                double convToBaseFrom = rateFrom * amount;
                toTransAmount_ = convToBaseFrom / rateTo;
            } else {
                toTransAmount_ = amount;
            }
        }
    }
    
    wxString transNum = textNumber_->GetValue();
    wxString notes = textNotes_->GetValue();
    wxString status = wxT(""); // nothing yet

    // Multiplex Auto executable onto the repeat field of the database. 
    int repeats = itemRepeats_->GetSelection();
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
        // repeats now hold extra info. Need to get repeats from dialog selection
        if ( (itemRepeats_->GetSelection() < 11) || (numRepeats > 0) )
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
        }
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
    if (autoExecuteUserAck_) {
        itemCheckBoxAutoExeSilent_->Enable(true);
    } else {
        itemCheckBoxAutoExeSilent_->SetValue(false);
        itemCheckBoxAutoExeSilent_->Enable(false);
        autoExecuteSilent_ = false;
    }
}

void mmBDDialog::OnAutoExecutionSilentChecked(wxCommandEvent& /*event*/)
{
    autoExecuteSilent_ = ! autoExecuteSilent_;
}

void mmBDDialog::OnCalendarSelChanged(wxCalendarEvent& event)
{
    wxDateTime date = event.GetDate();
    if (!enterOccur_)
    {
        dpcbd_->SetValue(date) ;
        dpc_->SetValue(date) ;
    }
}

void mmBDDialog::OnDateChanged(wxDateEvent& event)
{
	wxDateTime date = event.GetDate();
	calendarCtrl_->SetDate(date) ;
}

void mmBDDialog::OnAdvanceChecked(wxCommandEvent& /*event*/)
{
    if (cAdvanced_->IsChecked()) {
        SetAdvancedTransferControls(true);
    } else {
        SetAdvancedTransferControls();
        textAmount_->SetToolTip(amountTransferTip_);
    }
}

void mmBDDialog::SetTransferControls(bool transfers)
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

void mmBDDialog::SetAdvancedTransferControls(bool advanced)
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

void mmBDDialog::SetNewDate(wxDatePickerCtrl* dpc, bool forward)
{
    int day = -1;
    if (forward)
        day = 1;

    wxString dateStr = dpc->GetValue().FormatISODate();
	wxDateTime date = mmGetStorageStringAsDate (dateStr) ;
	date = date.Add(wxDateSpan::Days(day));

	dpc_->SetValue( date );
	dpcbd_->SetValue( date );
    calendarCtrl_->SetDate(date);
}

void mmBDDialog::OnNextOccurDateForward(wxSpinEvent& /*event*/)
{
    SetNewDate(dpcbd_);
}

void mmBDDialog::OnNextOccurDateBack(wxSpinEvent& /*event*/)
{
    SetNewDate(dpcbd_, false);
}

void mmBDDialog::OnTransDateForward(wxSpinEvent& /*event*/)
{
    SetNewDate(dpc_);
}

void mmBDDialog::OnTransDateBack(wxSpinEvent& /*event*/)
{
    SetNewDate(dpc_, false);
}

void mmBDDialog::setRepeatDetails()
{
    wxString repeatLabelRepeats  = _("Repeats"); 
    wxString repeatLabelActivate = _("Activates"); 

    wxString timeLabelDays   = _("Period: Days"); 
    wxString timeLabelMonths = _("Period: Months"); 

    bSetNextOccurDate_->Disable();
    int repeats = itemRepeats_->GetSelection();
    if ( (repeats == 11) ) {
        staticTextRepeats_->SetLabel( repeatLabelActivate );
        staticTimesRepeat_->SetLabel( timeLabelDays);
        textNumRepeats_->SetToolTip(_("Specify period in Days to activate.\nBecomes blank when not active."));
        bSetNextOccurDate_->Enable();
    } else if ( (repeats == 12) ) {
        staticTextRepeats_->SetLabel( repeatLabelActivate );
        staticTimesRepeat_->SetLabel( timeLabelMonths);
        textNumRepeats_->SetToolTip(_("Specify period in Months to activate.\nBecomes blank when not active."));
        bSetNextOccurDate_->Enable();
    } else if ( (repeats == 13) ) {
        staticTextRepeats_->SetLabel( repeatLabelRepeats );
        staticTimesRepeat_->SetLabel( timeLabelDays);
        textNumRepeats_->SetToolTip(_("Specify period in Days to activate.\nLeave blank when not active."));
    } else if ( (repeats == 14) ) {
        staticTextRepeats_->SetLabel( repeatLabelRepeats );
        staticTimesRepeat_->SetLabel( timeLabelMonths);
        textNumRepeats_->SetToolTip(_("Specify period in Months to activate.\nLeave blank when not active."));
    } else {
        staticTextRepeats_->SetLabel( _("Repeats") );
        staticTimesRepeat_->SetLabel( _("Times Repeated") );
        textNumRepeats_->SetToolTip(_("Specify the number of times this series repeats.\nLeave blank if this series continues forever."));
    }
}

void mmBDDialog::OnRepeatTypeChanged(wxCommandEvent& /*event*/)
{
    setRepeatDetails();
}

void mmBDDialog::OnsetNextRepeatDate(wxCommandEvent& /*event*/)
{
    wxString valueStr = textNumRepeats_->GetValue();
    if (valueStr.IsNumber())
    {
        long value;
        valueStr.ToLong(&value);

        wxString dateStr = dpcbd_->GetValue().FormatISODate();
	    wxDateTime  date = mmGetStorageStringAsDate(dateStr);

        int repeats = itemRepeats_->GetSelection();
        if ( (repeats == 11) || (repeats == 12))
        {
             if ( (repeats == 11))
                date = date.Add(wxDateSpan::Days(value));
             else
                 date = date.Add(wxDateSpan::Months(value));

        	dpc_->SetValue( date );
	        dpcbd_->SetValue( date );
            calendarCtrl_->SetDate(date);
        }
    }
}

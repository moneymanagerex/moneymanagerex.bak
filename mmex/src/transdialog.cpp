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

// Defines for Transaction Type
enum { DEF_WITHDRAWAL, DEF_DEPOSIT, DEF_TRANSFER };

// Defines for Transaction Status
enum { DEF_STATUS_NONE, DEF_STATUS_RECONCILED, DEF_STATUS_VOID, DEF_STATUS_FOLLOWUP, DEF_STATUS_DUPLICATE };


IMPLEMENT_DYNAMIC_CLASS( mmTransDialog, wxDialog )

BEGIN_EVENT_TABLE( mmTransDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTON_OK, mmTransDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTON_CANCEL, mmTransDialog::OnCancel)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmTransDialog::OnCategs)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONPAYEE, mmTransDialog::OnPayee)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTO, mmTransDialog::OnTo)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmTransDialog::OnTransTypeChanged)  
    EVT_DATE_CHANGED(ID_DIALOG_TRANS_BUTTONDATE, mmTransDialog::OnDateChanged) 
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONADVANCED, mmTransDialog::OnAdvanced)
    EVT_CHECKBOX(ID_DIALOG_TRANS_SPLITCHECKBOX, mmTransDialog::OnSplitChecked)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTRANSNUM, mmTransDialog::OnAutoTransNum)
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
    
    fillControls();

    boost::shared_ptr<mmSplitTransactionEntries> split(new mmSplitTransactionEntries());
    split_ = split;

    if (edit_)
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
            if (mmIniOptions::transStatusReconciled_)   // This changed the selection order
                choiceStatus_->SetSelection(DEF_STATUS_RECONCILED);
        }
        else if (statusString == wxT("R"))
        {
            choiceStatus_->SetSelection(DEF_STATUS_RECONCILED);
            if (mmIniOptions::transStatusReconciled_)   // This changed the selection order
                choiceStatus_->SetSelection(DEF_STATUS_NONE);
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

        if (transTypeString == wxT("Withdrawal"))
            choiceTrans_->SetSelection(DEF_WITHDRAWAL);
        else if (transTypeString == wxT("Deposit"))
            choiceTrans_->SetSelection(DEF_DEPOSIT);
        else if (transTypeString == wxT("Transfer"))
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

            if (subcategID_ != -1)
            {
                categString += wxT(" : ");
                categString += q1.GetString(wxT("SUBCATEGNAME"));
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

        if (transTypeString == wxT("Transfer"))
        {
            bPayee_->SetLabel(q1.GetString(wxT("ACCOUNTNAME")));
            bTo_->SetLabel(q1.GetString(wxT("TOACCOUNTNAME")));
            payeeID_ = accountID_;   

            // When editing an advanced transaction record, we do not reset the toTransAmount_
            if (edit_ && (toTransAmount_ != transAmount))
                advancedToTransAmountSet_ = true;
        }
    }
    
    st.Finalize();
}

void mmTransDialog::fillControls()
{
	
}

void mmTransDialog::CreateControls()
{    
    mmTransDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_LEFT|wxALL, 5);

    wxSizerFlags flags;
    flags.Border();

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC,
        _("Transaction Type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemChoice6Strings[] = 
    {
        _("Withdrawal"),
        _("Deposit"),
        _("Transfer")
    };  
    
    choiceTrans_ = new wxChoice( itemDialog1, ID_DIALOG_TRANS_TYPE, wxDefaultPosition, 
        wxDefaultSize, 3, itemChoice6Strings, 0 );
    itemBoxSizer4->Add(choiceTrans_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceTrans_->SetSelection(0);
    choiceTrans_->SetToolTip(_("Specify the type of transactions to be created."));

    bAdvanced_ = new wxButton( itemDialog1, ID_DIALOG_TRANS_BUTTONADVANCED, _("Advanced"));
    bAdvanced_->Enable(false);
    wxFont fnt = itemDialog1->GetFont();
    itemBoxSizer4->Add(bAdvanced_, flags);
    bAdvanced_->SetToolTip(_("Specify the transfer amount in the To Account"));

    staticTextAdvancedActive_ = new wxStaticText( itemDialog1, wxID_STATIC,
        _(" is active!"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(staticTextAdvancedActive_, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    wxPanel* itemPanel7 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel7, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(4, 4, 0, 0);
    itemPanel7->SetSizer(itemFlexGridSizer8);

    // Payee button ----------------------------------- begin
    
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel7, ID_DIALOG_TRANS_STATIC_PAYEE, 
        _("Payee"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString defaultPayeeName = _("Select Payee");
    wxString defaultCategName = _("Select Category");

    wxString payeeName   = defaultPayeeName;
    wxString categString = defaultCategName;
    if ( ! mmIniOptions::transPayeeSelectionNone_ )
    {
        // Determine most frequently used payee name for current account
        static const char sql[] = 
            "select count (*) c, "
            "cat.categname CATEGNAME, sc.subcategname SUBCATEGNAME, "
            "ca.categid, ca.subcategid, "
            "ca.payeeid, p.payeename PAYEENAME "
            "from CHECKINGACCOUNT_V1 ca, payee_v1 p "
            "left join CATEGORY_V1 cat "
            "on cat.CATEGID = ca.CATEGID "

            "left join SUBCATEGORY_V1 sc "
            "on sc.CATEGID = ca.CATEGID and "
            "sc.SUBCATEGID = ca.SUBCATEGID "
 
            "where ca.payeeid=p.payeeid " 
            "and ca.transcode<>'Transfer' "
            "and ca.accountid = ? "
            "group by ca.payeeid, ca.transdate, ca.categid, ca.subcategid "
            "order by ca.transdate desc, ca.transid desc, c desc "
            "limit 1";

        wxSQLite3Statement st = db_->PrepareStatement(sql);
        st.Bind(1, accountID_);
        wxSQLite3ResultSet q1 = st.ExecuteQuery();
        payeeName = q1.GetString(wxT("PAYEENAME"));
        payeeID_ = q1.GetInt(wxT("PAYEEID"));
        categString = q1.GetString(wxT("CATEGNAME"));
        wxString subcategName = q1.GetString(wxT("SUBCATEGNAME"));
        categID_ = q1.GetInt(wxT("CATEGID"));
        subcategID_ = q1.GetInt(wxT("SUBCATEGID"));

        //if some values is missing - set defaults
        if (payeeName == wxT(""))
        {
            payeeName = defaultPayeeName;
            payeeID_ = -1;
        } 
        if (categString == wxT(""))
        {
            categString = defaultCategName;
        }
        else 
        {
            if (subcategName != wxT(""))
            {
                categString += wxT(" : ");
                categString += subcategName;
            }
        }

        st.Finalize();
    }
    bPayee_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONPAYEE, payeeName, wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bPayee_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));

    // Payee button ----------------------------------- end

    //===========================================================
    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel7, wxID_STATIC, _("Number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText11, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer550 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer8->Add(itemBoxSizer550, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    textNumber_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNUMBER, wxT(""), wxDefaultPosition, wxSize(50, -1), 0 );
    itemBoxSizer550->Add(textNumber_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));

    bAuto_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONTRANSNUM, wxT(".."), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer550->Add(bAuto_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bAuto_->SetToolTip(_("Populate Transaction #"));

    //===========================================================
    
    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel7,
        ID_DIALOG_TRANS_STATIC_FROM, _("To"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    itemStaticText13->Show(false);

    bTo_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONTO, _("Select To Acct"), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bTo_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bTo_->Show(false);
    bTo_->SetToolTip(_("Specify which account the transfer is going to"));

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel7, wxID_STATIC, _("Date"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText15, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    dpc_ = new wxDatePickerCtrl( itemPanel7, ID_DIALOG_TRANS_BUTTONDATE, wxDefaultDateTime, wxDefaultPosition, wxSize(100, -1), wxDP_DROPDOWN);
    itemFlexGridSizer8->Add(dpc_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    dpc_->SetToolTip(_("Specify the date of the transaction"));

    // Category ******************************** begin //
    if ( mmIniOptions::transCategorySelectionNone_ )
        categString = defaultCategName;

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel7, wxID_STATIC, _("Category"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText17, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer8->Add(itemBoxSizer18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    bCategory_ = new wxButton( itemPanel7, 
        ID_DIALOG_TRANS_BUTTONCATEGS, categString, wxDefaultPosition, wxSize(200, -1), 0 );
    itemBoxSizer18->Add(bCategory_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bCategory_->SetToolTip(_("Specify the category for this transaction"));
    // Category ******************************** end //

    cSplit_ = new wxCheckBox( itemPanel7, 
        ID_DIALOG_TRANS_SPLITCHECKBOX, _("Split"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cSplit_->SetValue(FALSE);
    itemBoxSizer18->Add(cSplit_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    ////////////////////////////////////////////

    wxStaticText* itemStaticText51 = new wxStaticText( itemPanel7, wxID_STATIC, _("Status"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText51, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemChoice7Strings[] = 
    {
        _("None"),
        _("Reconciled"),
        _("Void"),
        _("Follow up"),
    	_("Duplicate"),
    };  

    if (mmIniOptions::transStatusReconciled_)
    {
        itemChoice7Strings[0] = _("Reconciled");
        itemChoice7Strings[1] = _("None");
    }

    choiceStatus_ = new wxChoice( itemPanel7, ID_DIALOG_TRANS_STATUS, wxDefaultPosition, 
        wxSize(100, -1), 5, itemChoice7Strings, 0 );
    itemFlexGridSizer8->Add(choiceStatus_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceStatus_->SetSelection(0);
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel7,
        wxID_STATIC, _("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText21, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textNotes_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNOTES, wxT(""), wxDefaultPosition, wxSize(200, 75), wxTE_MULTILINE );
    itemFlexGridSizer8->Add(textNotes_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));
    
    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel7, wxID_STATIC, _("Amount"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText23, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    textAmount_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(textAmount_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textAmount_->SetToolTip(_("Specify the amount for this transaction"));
    textAmount_->SetFocus();

    wxPanel* itemPanel25 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel25, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel25->SetSizer(itemBoxSizer26);

    wxButton* itemButton27 = new wxButton( itemPanel25, ID_DIALOG_TRANS_BUTTON_OK, _("OK"));
    fnt.SetWeight(wxFONTWEIGHT_NORMAL);
    fnt.SetPointSize(fnt.GetPointSize());
    itemButton27->SetFont(fnt);
    itemButton27->SetForegroundColour(wxColour(wxT("FOREST GREEN")));
    itemBoxSizer26->Add(itemButton27, flags);

    wxButton* itemButton28 = new wxButton( itemPanel25, ID_DIALOG_TRANS_BUTTON_CANCEL, _("Cancel"));
    fnt.SetWeight(wxFONTWEIGHT_NORMAL);
    fnt.SetPointSize(fnt.GetPointSize());
    itemButton28->SetFont(fnt);
    itemButton28->SetForegroundColour(wxColour(wxT("RED")));
    itemBoxSizer26->Add(itemButton28, flags);

}

void mmTransDialog::OnCancel(wxCommandEvent& /*event*/)
{
    Close(TRUE);
}

void mmTransDialog::OnPayee(wxCommandEvent& /*event*/)
{
    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
	{
	    //It should be possible to transfer from or to Investment accounts as too.
	    static const char sql[] =
    	"select ACCOUNTNAME "
    	"from ACCOUNTLIST_V1 "
//    	"where /*(ACCOUNTTYPE = 'Checking' or ACCOUNTTYPE = 'Term') and*/ STATUS <> 'Closed' "
// restored to previous order until a proper solution is found.
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
	            bPayee_->SetLabel(wxT("Select Payee"));
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

            // if payee has no memory of category then ignore displaying last category for payee
            if (tempCategID == -1)
                return;

            wxString catName = mmDBWrapper::getCategoryName(db_.get(), tempCategID);
            wxString categString = catName;

            if (tempSubCategID != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(), tempCategID, tempSubCategID);
                categString += wxT(" : ");
                categString += subcatName;
            }

            categID_ = tempCategID;
            subcategID_ = tempSubCategID;
            if ( ! mmIniOptions::transCategorySelectionNone_ )
                bCategory_->SetLabel(categString);
        }
        else
        {
            wxString payeeName = mmDBWrapper::getPayee(db_.get(), payeeID_, categID_, subcategID_);
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
void mmTransDialog::OnAutoTransNum(wxCommandEvent& /*event*/)
{
    int mID = mmDBWrapper::getTransIDByDate(db_.get(), dpc_->GetValue().FormatISODate(), accountID_);
    wxString wxIDstr = wxString::Format(wxT( "%d" ), (int) mID);
    textNumber_->SetValue( wxIDstr );
}

void mmTransDialog::OnTo(wxCommandEvent& /*event*/)
{
    // This should only get called if we are in a transfer

    static const char sql[] =
        "select ACCOUNTNAME "
        "from ACCOUNTLIST_V1 "
//      "where STATUS <> 'Closed' "
// restored to previous order until a proper solution is found.
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

void mmTransDialog::OnDateChanged(wxDateEvent& /*event*/)
{
	fillControls();
}

void mmTransDialog::OnAdvanced(wxCommandEvent& /*event*/)
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

            wxString catName = mmDBWrapper::getCategoryName(db_.get(), dlg.categID_);
            catName.Replace (wxT("&"), wxT("&&"));
            wxString categString = catName;

            if (dlg.subcategID_ != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(), dlg.categID_, dlg.subcategID_);
                subcatName.Replace (wxT("&"), wxT("&&"));
                categString += wxT(" : ");
                categString += subcatName;
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

    if (choiceTrans_->GetSelection() == DEF_WITHDRAWAL) {

        fillControls();
        displayControlsToolTips(DEF_WITHDRAWAL);
        stp->SetLabel(_("Payee"));
        if (payeeUnknown_) 
        {
            bPayee_->SetLabel(_("Select Payee"));
            payeeID_ = -1;
            toID_    = -1;
        } 

    } else if (choiceTrans_->GetSelection() == DEF_DEPOSIT) {

        fillControls();
        displayControlsToolTips(DEF_DEPOSIT);
        stp->SetLabel(_("From"));
        if (payeeUnknown_) 
        {
            bPayee_->SetLabel(_("Select Payee"));
            payeeID_ = -1;
            toID_    = -1;
        } 

    } else if (choiceTrans_->GetSelection() == DEF_TRANSFER) {

        displayControlsToolTips(DEF_TRANSFER, true);
        stp->SetLabel(_("From"));
        bTo_->SetLabel(_("Select To Account"));
        toID_    = -1;
        payeeUnknown_ = true;

        wxString acctName = mmDBWrapper::getAccountName(db_.get(), accountID_);
        bPayee_->SetLabel(acctName);
        payeeID_ = accountID_;

    }
}

void mmTransDialog::displayControlsToolTips(int transType, bool enableAdvanced /* = false */)
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_FROM);
    st->Show(enableAdvanced);
    bTo_->Show(enableAdvanced);
    bAdvanced_->Enable(enableAdvanced);
    if (transType == DEF_TRANSFER) {
        bPayee_->SetToolTip(_("Specify which account the transfer is comming from"));
        textAmount_->SetToolTip(_("Specify the transfer amount in the From and To Account"));
    } else {
        bPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));
        textAmount_->SetToolTip(_("Specify the amount for this transaction"));
    }
}

void mmTransDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString transCode = wxT("");
    int tCode = choiceTrans_->GetSelection();
    if (tCode == DEF_WITHDRAWAL)
        transCode = wxT("Withdrawal");
    else if (tCode == DEF_DEPOSIT)
        transCode = wxT("Deposit");
    else if (tCode == DEF_TRANSFER)
        transCode = wxT("Transfer");

    if (payeeID_ == -1)
    {
        if (transCode != wxT("Transfer"))
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
    else // if payee just has been created categid still null
    {
        if (categID_ < 1)
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
        if (!mmex::formatCurrencyToDouble(amountStr, amount) || (amount < 0.0))
        {
            mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
            return;
        }
    }

    int toAccountID = -1;
    int fromAccountID = accountID_;
    if (transCode == wxT("Transfer"))
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
        else
        {
            // Since to trans amount is not set,
            // we use the original currency rate to calculate
            // toTransAmount
            toTransAmount_ = edit_currency_rate * amount;
        }
    }

    wxString transNum = textNumber_->GetValue();
    wxString notes = textNotes_->GetValue();
    wxString status = wxT(""); // nothing yet

    if (choiceStatus_->GetSelection() == DEF_STATUS_NONE)
    {
        status = wxT(""); // nothing yet
        if (mmIniOptions::transStatusReconciled_)   // This changed the selection order
            status = wxT("R");
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_RECONCILED)
    {
        status = wxT("R");
        if (mmIniOptions::transStatusReconciled_)   // This changed the selection order
            status = wxT("");
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



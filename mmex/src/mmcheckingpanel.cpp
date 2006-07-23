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

#include "mmcheckingpanel.h"
#include "transdialog.h"
#include "util.h"
#include "dbwrapper.h"
#include <algorithm>
#include <vector>
/*******************************************************/
/* Include XPM Support */
#include "../resources/exefile.xpm"
#include "../resources/flag.xpm"
#include "../resources/void.xpm"
#include "../resources/reconciled.xpm"
#include "../resources/unreconciled.xpm"
#include "../resources/uparrow.xpm"
#include "../resources/downarrow.xpm"
#include "../resources/rightarrow.xpm"
/*******************************************************/
static int sortcol = 0;
static bool asc = true;
/*******************************************************/
BEGIN_EVENT_TABLE(mmCheckingPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_NEW_TRANS,         mmCheckingPanel::OnNewTransaction)
    EVT_BUTTON(ID_BUTTON_EDIT_TRANS,        mmCheckingPanel::OnEditTransaction)
    EVT_BUTTON(ID_BUTTON_DELETE_TRANS,      mmCheckingPanel::OnDeleteTransaction)
    EVT_LEFT_DOWN( mmCheckingPanel::OnMouseLeftDown ) 

    EVT_MENU(MENU_VIEW_ALLTRANSACTIONS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_RECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_UNRECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_VOID, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_FLAGGED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LAST30, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LAST3MONTHS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DELETE_TRANS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DELETE_FLAGGED, mmCheckingPanel::OnViewPopupSelected)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnItemRightClick)
    
    EVT_MENU(MENU_TREEPOPUP_MARKRECONCILED,   MyListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKUNRECONCILED, MyListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKVOID,         MyListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP, MyListCtrl::OnMarkTransaction)

    EVT_MENU(MENU_TREEPOPUP_MARKRECONCILED_ALL,   MyListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKUNRECONCILED_ALL, MyListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKVOID_ALL,         MyListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL, MyListCtrl::OnMarkAllTransactions)

    EVT_MENU(MENU_TREEPOPUP_NEW,              MyListCtrl::OnNewTransaction)
    EVT_MENU(MENU_TREEPOPUP_DELETE,           MyListCtrl::OnDeleteTransaction)
    EVT_MENU(MENU_TREEPOPUP_EDIT,             MyListCtrl::OnEditTransaction)
    
    EVT_LIST_COL_CLICK(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnColClick)
    EVT_LIST_KEY_DOWN(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/
mmCheckingPanel::mmCheckingPanel(wxSQLite3Database* db, wxSQLite3Database* inidb,
                                 int accountID, wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name )
            : db_(db), accountID_(accountID), m_imageList(0), inidb_(inidb)
{
    wxASSERT(db_);
    wxASSERT(accountID < 10);
    currentView_ = wxT("View All Transactions");
    Create(parent, winid, pos, size, style, name);
}

bool mmCheckingPanel::Create( wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, 
            const wxSize& size,long style, const wxString& name  )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    this->Freeze();
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    
    initVirtualListControl();
    
    if (trans_.size() > 1)
    {
        listCtrlAccount_->EnsureVisible(((int)trans_.size()) - 1);
    }
    this->Thaw();
    return TRUE;
}

mmCheckingPanel::~mmCheckingPanel()
{
    if (m_imageList)
        delete m_imageList;

    long col0, col1, col2, col3, col4, col5, col6;
    col0 = listCtrlAccount_->GetColumnWidth(0);
    col1 = listCtrlAccount_->GetColumnWidth(1);
    col2 = listCtrlAccount_->GetColumnWidth(2);
    col3 = listCtrlAccount_->GetColumnWidth(3);
    col4 = listCtrlAccount_->GetColumnWidth(4);
    col5 = listCtrlAccount_->GetColumnWidth(5);
    col6 = listCtrlAccount_->GetColumnWidth(6);

    wxString col0Str = wxString::Format(wxT("%d"), col0);
    wxString col1Str = wxString::Format(wxT("%d"), col1);
    wxString col2Str = wxString::Format(wxT("%d"), col2);
    wxString col3Str = wxString::Format(wxT("%d"), col3);
    wxString col4Str = wxString::Format(wxT("%d"), col4);
    wxString col5Str = wxString::Format(wxT("%d"), col5);
    wxString col6Str = wxString::Format(wxT("%d"), col6);

    mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_COL0_WIDTH"), col0Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_COL1_WIDTH"), col1Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_COL2_WIDTH"), col2Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_COL3_WIDTH"), col3Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_COL4_WIDTH"), col4Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_COL5_WIDTH"), col5Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_COL6_WIDTH"), col6Str); 
}

void mmCheckingPanel::OnMouseLeftDown( wxMouseEvent& event )
{
    // depending on the clicked control's window id.
    switch( event.GetId() ) 
    {
    case ID_PANEL_CHECKING_STATIC_BITMAP_VIEW : 
        {
            wxMenu menu;
            menu.Append(MENU_VIEW_ALLTRANSACTIONS, _("View All Transactions"));
            menu.Append(MENU_VIEW_RECONCILED, _("View Reconciled Transactions"));
            menu.Append(MENU_VIEW_UNRECONCILED, _("View Un-Reconciled Transactions"));
            menu.Append(MENU_VIEW_VOID, _("View Void Transactions"));
            menu.Append(MENU_VIEW_FLAGGED, _("View Flagged Transactions"));
            menu.AppendSeparator();
            menu.Append(MENU_VIEW_LAST30, _("View Transactions from last 30 days"));
            menu.Append(MENU_VIEW_LAST3MONTHS, _("View Transactions from last 3 months"));
            menu.AppendSeparator();
            menu.Append(MENU_VIEW_DELETE_TRANS, _("Delete all transactions in current view"));
            menu.Append(MENU_VIEW_DELETE_FLAGGED, _("Delete all flagged transactions"));

            PopupMenu(&menu, event.GetPosition());

            break;
        }
    }
    event.Skip();
} 

void mmCheckingPanel::CreateControls()
{    
    mmCheckingPanel* itemPanel8 = this;

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemPanel8->SetSizer(itemBoxSizer9);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( itemPanel8, ID_PANEL15, wxDefaultPosition, 
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, ID_PANEL_CHECKING_STATIC_HEADER, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, 
        wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);

     wxBoxSizer* itemBoxSizerHHeader2 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader2, 0, wxALL, 1);

    wxBitmap itemStaticBitmap3Bitmap(rightarrow_xpm);
    wxStaticBitmap* itemStaticBitmap3 = new wxStaticBitmap( headerPanel, 
        ID_PANEL_CHECKING_STATIC_BITMAP_VIEW, 
        itemStaticBitmap3Bitmap, wxDefaultPosition, wxSize(16, 16), 0 );
    itemBoxSizerHHeader2->Add(itemStaticBitmap3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemStaticBitmap3->SetEventHandler( this ); 

     wxStaticText* itemStaticText18 = new wxStaticText( headerPanel, 
            ID_PANEL_CHECKING_STATIC_PANELVIEW, 
            _("Viewing All Transactions"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizerHHeader2->Add(itemStaticText18, 0, wxALL, 1);

    wxBoxSizer* itemBoxSizerHHeader = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader, 0, wxALL, 1);

    wxStaticText* itemStaticText10 = new wxStaticText( headerPanel, 
            ID_PANEL_CHECKING_STATIC_BALHEADER, 
            wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizerHHeader->Add(itemStaticText10, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( itemPanel8, 
        ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), 
        wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(reconciled_xpm));
    m_imageList->Add(wxBitmap(void_xpm));
    m_imageList->Add(wxBitmap(flag_xpm));
    m_imageList->Add(wxBitmap(unreconciled_xpm));
    m_imageList->Add(wxBitmap(uparrow_xpm));
    m_imageList->Add(wxBitmap(downarrow_xpm));

    listCtrlAccount_ = new MyListCtrl( this, itemSplitterWindow10, 
        ID_PANEL_CHECKING_LISTCTRL_ACCT, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlAccount_->SetBackgroundColour(mmColors::listBackColor);
    
    wxListItem itemCol;
    
    listCtrlAccount_->InsertColumn(0, _("Date  "));
    
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Number"));
    listCtrlAccount_->InsertColumn(1, itemCol);

    listCtrlAccount_->InsertColumn(2, _("Payee"));
    listCtrlAccount_->InsertColumn(3, _("C  "));
    
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Withdrawal"));
    listCtrlAccount_->InsertColumn(4, itemCol);
    itemCol.SetText(_("Deposit"));
    listCtrlAccount_->InsertColumn(5, itemCol);
    itemCol.SetText(_("Balance"));
    listCtrlAccount_->InsertColumn(6, itemCol);
    
    /* See if we can get data from inidb */
     long col0, col1, col2, col3, col4, col5, col6;
     mmDBWrapper::getINISettingValue(inidb_, 
        wxT("CHECK_COL0_WIDTH"), wxT("80")).ToLong(&col0); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("CHECK_COL1_WIDTH"), wxT("-2")).ToLong(&col1); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("CHECK_COL2_WIDTH"), wxT("150")).ToLong(&col2); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("CHECK_COL3_WIDTH"), wxT("-2")).ToLong(&col3); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("CHECK_COL4_WIDTH"), wxT("-2")).ToLong(&col4); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("CHECK_COL5_WIDTH"), wxT("-2")).ToLong(&col5); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("CHECK_COL6_WIDTH"), wxT("-2")).ToLong(&col6); 

    listCtrlAccount_->SetColumnWidth(0, col0);
    listCtrlAccount_->SetColumnWidth(1, col1);
    listCtrlAccount_->SetColumnWidth(2, col2);
    listCtrlAccount_->SetColumnWidth(3, col3);
    listCtrlAccount_->SetColumnWidth(4, col4);
    listCtrlAccount_->SetColumnWidth(5, col5);
    listCtrlAccount_->SetColumnWidth(6, col6);
    listCtrlAccount_->SetColumnImage(listCtrlAccount_->sortCol_, 4);

    wxPanel* itemPanel12 = new wxPanel( itemSplitterWindow10, ID_PANEL1, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

    itemSplitterWindow10->SplitHorizontally(listCtrlAccount_, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);
    itemPanel12->SetBackgroundColour(mmColors::listDetailsPanelColor);
   

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton6 = new wxButton( itemPanel12, ID_BUTTON_NEW_TRANS, _("New"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton6->SetToolTip(_("New Transaction"));
    itemBoxSizer5->Add(itemButton6, 0, wxGROW|wxALL, 1);

    wxButton* itemButton7 = new wxButton( itemPanel12, ID_BUTTON_EDIT_TRANS, _("Edit"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton7->SetToolTip(_("Edit Transaction"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton8 = new wxButton( itemPanel12, ID_BUTTON_DELETE_TRANS, _("Delete"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton8->SetToolTip(_("Delete Transaction"));
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel12, 
        ID_PANEL_CHECKING_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    itemBoxSizer4->Add(itemStaticText11, 1, wxGROW|wxALL, 5);
}

// Return whether first element is greater than the second
bool sortTransactions( mmTransactionHolder elem1, mmTransactionHolder elem2 )
{
    if (sortcol == 0)
    {
        if (asc)
            return elem1.date_ < elem2.date_;
        else
            return elem1.date_ > elem2.date_;
    }

    if (sortcol == 1)
    {
        if (asc)
            return elem1.transNum_ < elem2.transNum_;
        else
            return elem1.transNum_ > elem2.transNum_;
    }

    if (sortcol== 2)
    {
        if (asc)
            return elem1.payeeStr_ < elem2.payeeStr_;
        else
            return elem1.payeeStr_ > elem2.payeeStr_;
    }

    if ((sortcol == 4) || (sortcol == 5))
    {
        if (asc)
            return elem1.amt_ < elem2.amt_;
        else
            return elem1.amt_ > elem2.amt_;
    }

    wxASSERT(false);
    return true;
}

bool sortTransactionsByDate( mmTransactionHolder elem1, mmTransactionHolder elem2 )
{
   return elem1.date_ < elem2.date_;
}

void mmCheckingPanel::sortTable()
{
    std::sort(trans_.begin(), trans_.end(), sortTransactions);
}

void mmCheckingPanel::updateExtraTransactionData(int selIndex)
{
    wxString cat   =  _("Category         : ") + trans_[selIndex].catStr_  +   wxT("\n"); 
    wxString subcat = _("Sub Category  : ") + trans_[selIndex].subCatStr_ + wxT("\n");
    wxString notes =  _("Notes               : ") + trans_[selIndex].notes_ + wxT("\n");
    wxString text = cat + subcat + notes;
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_DETAILS);
    st->SetLabel(text);
}

void mmCheckingPanel::setAccountSummary()
{
    double total = mmDBWrapper::getTotalBalanceOnAccount(db_, accountID_);
    wxString balance;
    mmCurrencyFormatter::formatDoubleToCurrency(total, balance);

    double reconciledBal = mmDBWrapper::getReconciledBalanceOnAccount(db_, accountID_);
    wxString recbalance;
    mmCurrencyFormatter::formatDoubleToCurrency(reconciledBal, recbalance);

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);

    wxString lbl  = wxString::Format(_("Account Balance : %s      Reconciled Balance : %s"), 
        balance.c_str(), recbalance.c_str());
    header->SetLabel(lbl);
}

void mmCheckingPanel::initVirtualListControl()
{
    wxProgressDialog* pgd = new wxProgressDialog(_("Please Wait"), 
        _("Accessing Database"), 100, this, 
        wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH );

    pgd->Update(10);

    // clear everything
    trans_.clear();
    double acctInitBalance = 0.0;

    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", accountID_);
    wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL);
    if (q2.NextRow())
    {
        int currencyID = q2.GetInt(wxT("CURRENCYID"));
        mmDBWrapper::loadSettings(db_, currencyID);
        acctInitBalance = q2.GetDouble(wxT("INITIALBAL"));
    
    }
    q2.Finalize();

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_HEADER);
    mmDBWrapper dbWrapper;
    wxString str = dbWrapper.getAccountName(db_, accountID_);

    header->SetLabel(_("Account View : ") + str);
            
   setAccountSummary();

    wxString text;
    text += _("Category: \n"); 
    text += _("Sub Category: \n"); 
    text += _("Notes: \n"); 
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_DETAILS);
    st->SetLabel(text);

    if (currentView_ == wxT("View All Transactions"))
    {
        bufSQL.Format("select * from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", 
            accountID_, accountID_);
    }
    else if (currentView_ == wxT("View Reconciled"))
    {
          bufSQL.Format("select * from CHECKINGACCOUNT_V1 where \
              (ACCOUNTID=%d OR TOACCOUNTID=%d) AND (STATUS='R');", 
            accountID_, accountID_);
    }
    else if (currentView_ == wxT("View Void"))
    {
          bufSQL.Format("select * from CHECKINGACCOUNT_V1 where \
              (ACCOUNTID=%d OR TOACCOUNTID=%d) AND (STATUS='V');", 
            accountID_, accountID_);
    }
    else if (currentView_ == wxT("View Flagged"))
    {
          bufSQL.Format("select * from CHECKINGACCOUNT_V1 where \
              (ACCOUNTID=%d OR TOACCOUNTID=%d) AND (STATUS='F');", 
            accountID_, accountID_);
    }
    else if (currentView_ == wxT("View UnReconciled"))
    {
          bufSQL.Format("select * from CHECKINGACCOUNT_V1 where \
              (ACCOUNTID=%d OR TOACCOUNTID=%d) AND (STATUS='');", 
            accountID_, accountID_);
    }
    else
    {
          bufSQL.Format("select * from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", 
            accountID_, accountID_);
    }
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    int ct = 0;
    while (q1.NextRow())
    {
        mmTransactionHolder th;
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
 
        if (currentView_ == wxT("View 30 days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());

             if (!dtdt.IsBetween(dtBegin, dtEnd))
                continue; //skip
        }
        else if (currentView_ == wxT("View 90 days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(90 * wxDateSpan::Day());

             if (!dtdt.IsBetween(dtBegin, dtEnd))
                continue; //skip
        }

        th.dateStr_       = mmGetDateForDisplay(db_, dtdt);
        th.transID_        = q1.GetInt(wxT("TRANSID"));
        th.date_           = dtdt;
        th.catID_          = q1.GetInt(wxT("CATEGID"));
        th.subcategID_     = q1.GetInt(wxT("SUBCATEGID"));
        th.payeeID_        = q1.GetInt(wxT("PAYEEID"));
        th.transNum_    = q1.GetString(wxT("TRANSACTIONNUMBER"));
        th.status_         = q1.GetString(wxT("STATUS"));
        th.notes_          = q1.GetString(wxT("NOTES"));
        th.transType_      = q1.GetString(wxT("TRANSCODE"));
        th.accountID_      = q1.GetInt(wxT("ACCOUNTID"));
        th.toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));

        th.amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
        th.toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));

        wxString displayTransAmtString;
        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.amt_, displayTransAmtString))
            th.transAmtString_ = displayTransAmtString;

        wxString displayToTransAmtString;
        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.toAmt_, displayToTransAmtString))
            th.transToAmtString_ = displayToTransAmtString;

        th.catStr_ = mmDBWrapper::getCategoryName(db_, th.catID_);
        th.subCatStr_ = mmDBWrapper::getSubCategoryName(db_, th.catID_, th.subcategID_);
        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

        th.depositStr_ = wxT("");
        th.withdrawalStr_ = wxT("");
        if (th.transType_ == wxT("Deposit"))
        {
            th.depositStr_ = displayTransAmtString;
        }
        else if (th.transType_== wxT("Withdrawal"))
        {
            th.withdrawalStr_ = displayTransAmtString;
        }
        else if (th.transType_ == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_,  th.accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  th.toAccountID_ );

            if (th.accountID_ == accountID_)
            {
                th.withdrawalStr_ = displayTransAmtString;
                th.payeeStr_ = toAccount;
            }
            else if (th.toAccountID_ == accountID_)
            {
                th.depositStr_ = displayToTransAmtString;
                th.payeeStr_ = fromAccount;
            }
        }

        trans_.push_back(th);
        ct++;
    }
    q1.Finalize();
    listCtrlAccount_->SetItemCount(ct);

    pgd->Update(30);

    // sort trans_ by date
    double initBalance = acctInitBalance;
    std::sort(trans_.begin(), trans_.end(), sortTransactionsByDate);

    pgd->Update(50);

    for (unsigned int index = 0; index < trans_.size(); index++)
    {
        if (trans_[index].transType_ == wxT("Deposit"))
        {
            initBalance += trans_[index].amt_;
        }
        else if (trans_[index].transType_ == wxT("Withdrawal"))
        {
            initBalance -= trans_[index].amt_;
        }
        else if (trans_[index].transType_ == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_,  trans_[index].accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  trans_[index].toAccountID_ );

            if (trans_[index].accountID_ == accountID_)
            {
               initBalance -= trans_[index].amt_;
            }
            else if (trans_[index].toAccountID_== accountID_)
            {
                initBalance += trans_[index].toAmt_;
            }
        }
        trans_[index].balance_ = initBalance;
        wxString balanceStr;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(initBalance, balanceStr);
        trans_[index].balanceStr_ = balanceStr;
    }

    pgd->Update(70);
    std::sort(trans_.begin(), trans_.end(), sortTransactions);
    pgd->Update(90);

    mmENDSQL_LITE_EXCEPTION;

    pgd->Update(100);
    pgd->Destroy();
}

void mmCheckingPanel::OnDeleteTransaction(wxCommandEvent& event)
{
    listCtrlAccount_->OnDeleteTransaction(event);
}

void mmCheckingPanel::OnNewTransaction(wxCommandEvent& event)
{
   listCtrlAccount_->OnNewTransaction(event);
}

void mmCheckingPanel::OnEditTransaction(wxCommandEvent& event)
{
    listCtrlAccount_->OnEditTransaction(event);
}

void mmCheckingPanel::OnViewPopupSelected(wxCommandEvent& event)
{
    int evt =  event.GetId();
    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_PANELVIEW);
    if (evt ==  MENU_VIEW_ALLTRANSACTIONS)
    {
        header->SetLabel(_("Viewing all transactions"));
        currentView_ = wxT("View All Transactions");
    }
    else if (evt == MENU_VIEW_RECONCILED)
    {
        header->SetLabel(_("Viewing Reconciled transactions"));
        currentView_ = wxT("View Reconciled");
    }
    else if (evt == MENU_VIEW_VOID)
    {
        header->SetLabel(_("Viewing Void transactions"));
        currentView_ = wxT("View Void");
    }
    else if (evt == MENU_VIEW_UNRECONCILED)
    {
        header->SetLabel(_("Viewing Un-Reconciled transactions"));
        currentView_ = wxT("View UnReconciled");
    }
    else if (evt == MENU_VIEW_FLAGGED)
    {
        header->SetLabel(_("Viewing Flagged transactions"));
        currentView_ = wxT("View Flagged");
    }
    else if (evt == MENU_VIEW_LAST30)
    {
        header->SetLabel(_("Viewing transactions from last 30 days"));
        currentView_ = wxT("View 30 days");
    }
    else if (evt == MENU_VIEW_LAST3MONTHS)
    {
        header->SetLabel(_("Viewing transactions from last 3 months"));
        currentView_ = wxT("View 90 days");
    }
    else if (evt == MENU_VIEW_DELETE_TRANS)
    {
        wxMessageDialog msgDlg(this, _("Do you really want to delete all the transactions shown?"),
            _("Confirm Transaction Deletion"),
            wxYES_NO);
        if (msgDlg.ShowModal() == wxID_YES)
        {
            for (unsigned long idx = 0; idx < trans_.size(); idx++)
            {
                mmDBWrapper::deleteTransaction(db_, trans_[idx].transID_);
            }
        }
    }
    else if (evt == MENU_VIEW_DELETE_FLAGGED)
    {
        wxMessageDialog msgDlg(this, _("Do you really want to delete all the flagged transactions?"),
            _("Confirm Transaction Deletion"),
            wxYES_NO);
        if (msgDlg.ShowModal() == wxID_YES)
        {
            mmDBWrapper::deleteFlaggedTransactions(db_, accountID_);
        }
    }
    else
    {
        wxASSERT(false);
    }

    listCtrlAccount_->DeleteAllItems();
    initVirtualListControl();
    listCtrlAccount_->RefreshItems(0, ((int)trans_.size()) - 1);
}

/*******************************************************/
void MyListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    cp_->updateExtraTransactionData(selectedIndex_);
}

void MyListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Transaction"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Transaction"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Transaction"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_MARKRECONCILED, _("Mark As &Reconciled"));
    menu.Append(MENU_TREEPOPUP_MARKUNRECONCILED, _("Mark As &Unreconciled"));
    menu.Append(MENU_TREEPOPUP_MARKVOID, _("Mark As &Void"));
    menu.Append(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP, _("Mark For &Followup"));
    menu.AppendSeparator();

    wxMenu* subGlobalOpMenu = new wxMenu;
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARKRECONCILED_ALL, _("as Reconciled"));
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARKUNRECONCILED_ALL, _("as Unreconciled"));
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARKVOID_ALL, _("as Void"));
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL, _("as needing Followup"));
    menu.Append(MENU_SUBMENU_MARK_ALL, _("Mark all "), subGlobalOpMenu);

    PopupMenu(&menu, event.GetPoint());
}

void MyListCtrl::OnMarkTransactionDB(const wxString& status)
{
    int transID = cp_->trans_[selectedIndex_].transID_;
    mmDBWrapper::updateTransactionWithStatus(cp_->db_, transID, status);
    cp_->trans_[selectedIndex_].status_ = status;

    if (cp_->currentView_ != wxT("View All Transactions"))
    {
        DeleteAllItems();
        cp_->initVirtualListControl();
    }
   RefreshItem(selectedIndex_);
   cp_->setAccountSummary();
}

void MyListCtrl::OnMarkTransaction(wxCommandEvent& event)
{
     int evt =  event.GetId();
     wxString status = wxT("");
     if (evt ==  MENU_TREEPOPUP_MARKRECONCILED)
        status = wxT("R");
     else if (evt == MENU_TREEPOPUP_MARKUNRECONCILED)
         status = wxT("");
     else if (evt == MENU_TREEPOPUP_MARKVOID)
         status = wxT("V");
     else if (evt == MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP)
         status = wxT("F");
     else
     {
        wxASSERT(false);
     }
      
    OnMarkTransactionDB(status);
}

void MyListCtrl::OnMarkAllTransactions(wxCommandEvent& event)
{
     int evt =  event.GetId();
     wxString status = wxT("");
     if (evt ==  MENU_TREEPOPUP_MARKRECONCILED_ALL)
        status = wxT("R");
     else if (evt == MENU_TREEPOPUP_MARKUNRECONCILED_ALL)
         status = wxT("");
     else if (evt == MENU_TREEPOPUP_MARKVOID_ALL)
         status = wxT("V");
     else if (evt == MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL)
         status = wxT("F");
     else
     {
        wxASSERT(false);
     }
         
     for (unsigned int idx = 0; idx < cp_->trans_.size(); idx++)
     {
        int transID = cp_->trans_[idx].transID_;
        mmDBWrapper::updateTransactionWithStatus(cp_->db_, transID, status);
        cp_->trans_[idx].status_ = status;
     }

     if (cp_->currentView_ != wxT("View All Transactions"))
     {
         DeleteAllItems();
         cp_->initVirtualListControl();
         RefreshItems(0, ((int)cp_->trans_.size()) - 1); // refresh everything
     }
     cp_->setAccountSummary();
}

void MyListCtrl::OnColClick(wxListEvent& event)
{
    /* Figure out which column has to be sorted */
    if ((event.GetColumn() == 3) || (event.GetColumn() == 6))
        return;

    /* Clear previous column image */
    SetColumnImage(sortCol_, -1);

    sortCol_ = event.GetColumn();
    sortcol = sortCol_;
    asc_ = !asc_; // toggle sort order
    asc = asc_;
    SetColumnImage(sortCol_, asc_ ? 5 : 4); // decide whether top or down icon needs to be shown
    cp_->sortTable();   // sort the table
    RefreshItems(0, ((int)cp_->trans_.size()) - 1); // refresh everything
}

void MyListCtrl::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}

wxString mmCheckingPanel::getItem(long item, long column)
{
    if (column == 0)
        return trans_[item].dateStr_;

    if (column == 1)
        return trans_[item].transNum_;

    if (column == 2)
        return trans_[item].payeeStr_;

    if (column == 3)
        return trans_[item].status_;

    if (column == 4)
       return trans_[item].withdrawalStr_;

    if (column == 5)
        return trans_[item].depositStr_; 
    
    if (column == 6)
        return trans_[item].balanceStr_;

    return wxT("");
}

wxString MyListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

int MyListCtrl::OnGetItemImage(long item) const
{
   /* Returns the icon to be shown for each transaction */
   wxString status = cp_->getItem(item, 3);
   if (status == wxT("F"))
   {
        return 2;
   }
   else if (status == wxT("R"))
   {
        return 0;
   }
   else if (status == wxT("V"))
   {
        return 1;
   }
   return 3;
}

wxListItemAttr* MyListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void MyListCtrl::OnListKeyDown(wxListEvent& event)
{
  switch ( event.GetKeyCode() )
    {
        case WXK_DELETE:
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_DELETE);
                OnDeleteTransaction(evt);
            }
            break;
#if 0
        case 'V':
            {
                SetItemState(selectedIndex_, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKVOID);
                OnMarkTransaction(evt);
                SetItemState(selectedIndex_, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                //OnMarkTransactionDB(wxT("V"));
                event.Skip();
            }
            break;

        case 'R':
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKRECONCILED);
                OnMarkTransaction(evt);
                //OnMarkTransactionDB(wxT("R"));
                //event.Skip();

            }
            break;

        case 'U':
            {
                 wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKUNRECONCILED);
                 OnMarkTransaction(evt);
                //OnMarkTransactionDB(wxT(""));
                //event.Skip();
            }
            break;

        case 'F':
            {
                 wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP);
                 OnMarkTransaction(evt);
                //OnMarkTransactionDB(wxT("F"));
                //event.Skip();
            }
            break;
#endif

        default:
            event.Skip();
    }
}

void MyListCtrl::OnNewTransaction(wxCommandEvent& event)
{
    mmTransDialog *dlg = new mmTransDialog(cp_->db_, cp_->accountID_, 0, false, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void MyListCtrl::OnDeleteTransaction(wxCommandEvent& event)
{
    if (selectedIndex_ == -1)
        return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the transaction?"),
                                        _("Confirm Transaction Deletion"),
                                        wxYES_NO);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        mmDBWrapper::deleteTransaction(cp_->db_, cp_->trans_[selectedIndex_].transID_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl();
    }
}

void MyListCtrl::OnEditTransaction(wxCommandEvent& event)
{
    if (selectedIndex_ == -1)
        return;
    mmTransDialog *dlg = new mmTransDialog(cp_->db_, cp_->accountID_, 
        cp_->trans_[selectedIndex_].transID_, true, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void MyListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    mmTransDialog *dlg = new mmTransDialog(cp_->db_, cp_->accountID_, 
        cp_->trans_[selectedIndex_].transID_, true, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);

    }
    dlg->Destroy();
}

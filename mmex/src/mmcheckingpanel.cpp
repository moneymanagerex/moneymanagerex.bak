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
//----------------------------------------------------------------------------
#include "mmcheckingpanel.h"
#include "transdialog.h"
#include "util.h"
#include "dbwrapper.h"
//----------------------------------------------------------------------------
#include <algorithm>
#include <vector>
//----------------------------------------------------------------------------
/* Include XPM Support */
#include "../resources/exefile.xpm"
#include "../resources/flag.xpm"
#include "../resources/void.xpm"
#include "../resources/reconciled.xpm"
#include "../resources/unreconciled.xpm"
#include "../resources/uparrow.xpm"
#include "../resources/downarrow.xpm"
#include "../resources/rightarrow.xpm"
#include "../resources/duplicate.xpm"
//----------------------------------------------------------------------------

namespace
{

int g_sortcol = 0; // index of column to sort
bool g_asc = true; // asc\desc sorting
//----------------------------------------------------------------------------

/*
    Adds columns to list controls and setup their initial widths.
*/
void createColumns(wxSQLite3Database *inidb_, wxListCtrl &lst)
{
    wxASSERT(inidb_);

    {
        long i = 0;
        lst.InsertColumn(i++, _("Date  "));
        lst.InsertColumn(i++, _("Number"), wxLIST_FORMAT_RIGHT);
        lst.InsertColumn(i++, _("Payee"));
        lst.InsertColumn(i++, _("C  "));
        lst.InsertColumn(i++, _("Category"));
        lst.InsertColumn(i++, _("Withdrawal"), wxLIST_FORMAT_RIGHT);
        lst.InsertColumn(i++, _("Deposit"), wxLIST_FORMAT_RIGHT);
        lst.InsertColumn(i++, _("Balance"), wxLIST_FORMAT_RIGHT);
        lst.InsertColumn(i++, _("Notes"));
    }

    // adjust columns' widths

    const wxChar* def_widths[] = 
    {
        wxT("80"),  // date
        wxT("-2"),  // number
        wxT("150"), // payee
        wxT("-2"),  // C
        wxT("-2"),  // category
        wxT("-2"),  // withdrawal
        wxT("-2"),  // deposit
        wxT("-2"),  // balance
        wxT("200")  // notes
    };

    const int cnt = lst.GetColumnCount();
    wxASSERT( cnt == sizeof(def_widths)/sizeof(*def_widths) );

    for (int i = 0; i < cnt; ++i)
    {
        wxString name = wxString::Format(wxT("CHECK_COL%d_WIDTH"), i);
        wxString val = mmDBWrapper::getINISettingValue(inidb_, name, def_widths[i]);
        
        long width = -1;
        
        if (val.ToLong(&width))
        {
            lst.SetColumnWidth(i, width);    
        }
    }
}
//----------------------------------------------------------------------------

/*
    Return whether first element is greater than the second
*/
bool sortTransactions( mmBankTransaction* elem1, mmBankTransaction* elem2 )
{
    enum 
    { 
        COL_DATE_OR_TRANSACTION_ID, 
        COL_TRANSACTION_NUMBER, 
        COL_PAYEE_STR, 
        COL_STATUS, 
        COL_CATEGORY, 
        COL_WITHDRAWAL, 
        COL_DEPOSIT 
    };

    long elem1Long = 0;
    long elem2Long = 0;

    switch( g_sortcol )
    {
    case COL_DATE_OR_TRANSACTION_ID:
        if (elem1->date_ != elem2->date_)
        {
            if (g_asc)
                return elem1->date_ < elem2->date_;
            else
                return elem1->date_ > elem2->date_;
        }
        else
        {
            if (g_asc)
                return elem1->transactionID() < elem2->transactionID();
            else
                return elem1->transactionID() > elem2->transactionID();
        }
        break;

    case COL_TRANSACTION_NUMBER:
        {
            bool isOK1 = elem1->transNum_.ToLong(&elem1Long );
            bool isOK2 = elem2->transNum_.ToLong(&elem2Long );
            if (isOK1 && isOK2)
            {
                if (g_asc)
                    return elem1Long < elem2Long;
                else
                    return elem1Long > elem2Long;
            }
            else
            {
                if (g_asc)
                    return elem1->transNum_ < elem2->transNum_;
                else
                    return elem1->transNum_ > elem2->transNum_;
            }
        }
        break;

    case COL_PAYEE_STR:
        if (g_asc)
            return elem1->payeeStr_ < elem2->payeeStr_;
        else
            return elem1->payeeStr_ > elem2->payeeStr_;
        break;

    case COL_STATUS:
        if (g_asc)
            return elem1->status_ < elem2->status_;
        else
            return elem1->status_ > elem2->status_;

        break;

    case COL_CATEGORY:
        if (g_asc)
            return elem1->fullCatStr_ < elem2->fullCatStr_;
        else
            return elem1->fullCatStr_ > elem2->fullCatStr_;

        break;

    case COL_WITHDRAWAL:
    case COL_DEPOSIT:
        if (g_asc)
            return elem1->amt_ < elem2->amt_;
        else
            return elem1->amt_ > elem2->amt_;  		
        break;
    }

    wxASSERT(false);
    return true;
}
//----------------------------------------------------------------------------

bool sortTransactionsByDate(const mmBankTransaction *t1, const mmBankTransaction *t2)
{
    bool res = false;

    if (t1->date_ == t2->date_) {
        res = t1->transactionID() < t2->transactionID();
    } else {
        res = t1->date_ < t2->date_;
    }

    return res;
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

/* 
    Custom ListCtrl class that implements virtual LC style 
*/
class MyListCtrl : public wxListCtrl
{
    DECLARE_NO_COPY_CLASS(MyListCtrl)
    DECLARE_EVENT_TABLE()

public:
    MyListCtrl(mmCheckingPanel *cp, wxWindow *parent,const wxWindowID id, const wxPoint& pos,const wxSize& size, long style);

    /* required overrides for virtual style list control */
    wxString OnGetItemText(long item, long column) const;
    int OnGetItemImage(long item) const;
    wxListItemAttr *OnGetItemAttr(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    void OnMarkTransaction(wxCommandEvent& event);
    void OnMarkAllTransactions(wxCommandEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMarkTransactionDB(const wxString& status);
    void OnCopy(wxCommandEvent& WXUNUSED(event));
    void OnPaste(wxCommandEvent& WXUNUSED(event));

    /* Sort Columns */
    void OnColClick(wxListEvent& event);
    void SetColumnImage(int col, int image);

    void OnNewTransaction(wxCommandEvent& event);
    void OnDeleteTransaction(wxCommandEvent& event);
    void OnEditTransaction(wxCommandEvent& event);

public:
    long m_sortCol;
    bool m_asc;

private:
    mmCheckingPanel *m_cp;
    long m_selectedIndex;
    long m_selectedForCopy;

    wxListItemAttr m_attr1; // style1
    wxListItemAttr m_attr2; // style2
    wxListItemAttr m_attr3; // style, for future dates
    wxListItemAttr m_attr4; // style, for future dates
};
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(mmCheckingPanel, wxPanel)

    EVT_BUTTON(ID_BUTTON_NEW_TRANS,         mmCheckingPanel::OnNewTransaction)
    EVT_BUTTON(ID_BUTTON_EDIT_TRANS,        mmCheckingPanel::OnEditTransaction)
    EVT_BUTTON(ID_BUTTON_DELETE_TRANS,      mmCheckingPanel::OnDeleteTransaction)
    EVT_LEFT_DOWN( mmCheckingPanel::OnMouseLeftDown ) 

    EVT_MENU(MENU_VIEW_ALLTRANSACTIONS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_RECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_UNRECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_NOTRECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_VOID, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_FLAGGED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_TODAY, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LAST30, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LAST3MONTHS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DUPLICATE, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DELETE_TRANS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DELETE_FLAGGED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_CURRENTMONTH, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LASTMONTH, mmCheckingPanel::OnViewPopupSelected)

END_EVENT_TABLE()
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)

    EVT_LIST_ITEM_SELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnItemRightClick)

    EVT_MENU(MENU_TREEPOPUP_MARKRECONCILED,   MyListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKUNRECONCILED, MyListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKVOID,         MyListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP, MyListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKDUPLICATE,         MyListCtrl::OnMarkTransaction)

    EVT_MENU(MENU_TREEPOPUP_MARKRECONCILED_ALL,   MyListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKUNRECONCILED_ALL, MyListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKVOID_ALL,         MyListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL, MyListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKDUPLICATE_ALL,         MyListCtrl::OnMarkAllTransactions)

    EVT_MENU(MENU_TREEPOPUP_NEW,              MyListCtrl::OnNewTransaction)
    EVT_MENU(MENU_TREEPOPUP_DELETE,           MyListCtrl::OnDeleteTransaction)
    EVT_MENU(MENU_TREEPOPUP_EDIT,             MyListCtrl::OnEditTransaction)

    EVT_LIST_COL_CLICK(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnColClick)
    EVT_LIST_KEY_DOWN(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnListKeyDown)

    EVT_MENU(MENU_ON_COPY_TRANSACTION, MyListCtrl::OnCopy) 
    EVT_MENU(MENU_ON_PASTE_TRANSACTION, MyListCtrl::OnPaste) 
    EVT_MENU(MENU_ON_NEW_TRANSACTION, MyListCtrl::OnNewTransaction) 

    EVT_CHAR(MyListCtrl::OnChar)

END_EVENT_TABLE()
//----------------------------------------------------------------------------

mmCheckingPanel::mmCheckingPanel
(
    mmCoreDB* core,
    wxSQLite3Database* inidb,
    int accountID, 
    wxWindow *parent,
    wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
    const wxString& name
) : 
    m_core(core),
    m_inidb(inidb),
    m_listCtrlAccount(),
    m_AccountID(accountID)
{
    wxASSERT(m_core);
    wxASSERT(m_inidb);

    Create(parent, winid, pos, size, style, name);
}
//----------------------------------------------------------------------------

/*
    We cannon use OnClose() event because this class deletes 
    via DestroyChildren() of its parent.
*/
mmCheckingPanel::~mmCheckingPanel()
{
    try {
        saveSettings();
    } catch (...) {
        wxASSERT(false);
    }
}
//----------------------------------------------------------------------------

bool mmCheckingPanel::Create
(
    wxWindow *parent,
    wxWindowID winid, const wxPoint& pos, 
    const wxSize& size,long style, const wxString& name
)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    bool ok = wxPanel::Create(parent, winid, pos, size, style, name);

    if (ok) {
        Freeze();
        CreateControls();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);

        initVirtualListControl();
        Thaw();
    }

    return ok;
}
//----------------------------------------------------------------------------

/*
    Save data to ini database.
*/
void mmCheckingPanel::saveSettings()
{
    int cols = m_listCtrlAccount->GetColumnCount();

    for (int i = 0; i < cols; ++i)
    {
        wxString name = wxString::Format(wxT("CHECK_COL%d_WIDTH"), i);

        int width = m_listCtrlAccount->GetColumnWidth(i);
        wxString val = wxString::Format(wxT("%d"), width);

        mmDBWrapper::setINISettingValue(m_inidb, name, val); 
    }

    // sorting column index

    wxString val = wxString::Format(wxT("%d"), g_sortcol);
    mmDBWrapper::setINISettingValue(m_inidb, wxT("CHECK_SORT_COL"), val); 

    // asc\desc sorting flag

    val = wxString::Format(wxT("%d"), g_asc);
    mmDBWrapper::setINISettingValue(m_inidb, wxT("CHECK_ASC"), val);
}
//----------------------------------------------------------------------------

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
            menu.Append(MENU_VIEW_NOTRECONCILED, _("View All Except Reconciled Transactions"));
            menu.Append(MENU_VIEW_VOID, _("View Void Transactions"));
            menu.Append(MENU_VIEW_FLAGGED, _("View Flagged Transactions"));
			menu.Append(MENU_VIEW_DUPLICATE, _("View Duplicate Transactions"));
            menu.AppendSeparator();
            menu.Append(MENU_VIEW_TODAY, _("View Transactions from today"));
            menu.Append(MENU_VIEW_LAST30, _("View Transactions from last 30 days"));
            menu.Append(MENU_VIEW_CURRENTMONTH, _("View Transactions from current month"));
            menu.Append(MENU_VIEW_LASTMONTH, _("View Transactions from last month"));
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
//----------------------------------------------------------------------------

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
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 5);

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

    m_currentView = mmDBWrapper::getINISettingValue(m_inidb, 
       wxT("VIEWTRANSACTIONS"), wxT("View All Transactions"));
    initViewTransactionsHeader();

    wxBoxSizer* itemBoxSizerHHeader = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader, 0, wxALL, 1);

    wxStaticText* itemStaticText10 = new wxStaticText( headerPanel, 
            ID_PANEL_CHECKING_STATIC_BALHEADER, 
            wxT(""), wxDefaultPosition, wxSize(500, 20), 0 );
    itemBoxSizerHHeader->Add(itemStaticText10, 0, wxALL | wxEXPAND , 5);
    
    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( itemPanel8, 
        ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), 
        wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList.reset(new wxImageList(imageSize.GetWidth(), imageSize.GetHeight()));
    m_imageList->Add(wxBitmap(reconciled_xpm));
    m_imageList->Add(wxBitmap(void_xpm));
    m_imageList->Add(wxBitmap(flag_xpm));
    m_imageList->Add(wxBitmap(unreconciled_xpm));
    m_imageList->Add(wxBitmap(uparrow_xpm));
    m_imageList->Add(wxBitmap(downarrow_xpm));
	m_imageList->Add(wxBitmap(duplicate_xpm));

    m_listCtrlAccount = new MyListCtrl( this, itemSplitterWindow10, 
        ID_PANEL_CHECKING_LISTCTRL_ACCT, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    
    m_listCtrlAccount->SetImageList(m_imageList.get(), wxIMAGE_LIST_SMALL);
    m_listCtrlAccount->SetBackgroundColour(mmColors::listBackColor);
    m_listCtrlAccount->m_asc = g_asc;
    m_listCtrlAccount->m_sortCol = g_sortcol;
    m_listCtrlAccount->SetFocus();
    
    createColumns(m_inidb, *m_listCtrlAccount);

    {   // load the global variables
        long iniSortCol = 0;
        mmDBWrapper::getINISettingValue(m_inidb, wxT("CHECK_SORT_COL"), wxT("0")).ToLong(&iniSortCol);
        g_sortcol = iniSortCol;

        long iniSortAsc = 0;
        mmDBWrapper::getINISettingValue(m_inidb, wxT("CHECK_ASC"), wxT("1")).ToLong(&iniSortAsc); 
        g_asc = iniSortAsc != 0;

        m_listCtrlAccount->m_sortCol = g_sortcol;
        m_listCtrlAccount->m_asc = g_asc;
        m_listCtrlAccount->SetColumnImage(m_listCtrlAccount->m_sortCol, 5); // asc\desc sort mark (arrow)
    }

    wxPanel* itemPanel12 = new wxPanel( itemSplitterWindow10, ID_PANEL1, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

    itemSplitterWindow10->SplitHorizontally(m_listCtrlAccount, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);
    itemPanel12->SetBackgroundColour(mmColors::listDetailsPanelColor);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton6 = new wxButton( itemPanel12, ID_BUTTON_NEW_TRANS, _("&New"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton6->SetToolTip(_("New Transaction"));
    itemBoxSizer5->Add(itemButton6, 0, wxGROW|wxALL, 1);

    wxButton* itemButton7 = new wxButton( itemPanel12, ID_BUTTON_EDIT_TRANS, _("&Edit"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton7->SetToolTip(_("Edit Transaction"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton8 = new wxButton( itemPanel12, ID_BUTTON_DELETE_TRANS, _("&Delete"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton8->SetToolTip(_("Delete Transaction"));
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel12, 
        ID_PANEL_CHECKING_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    itemBoxSizer4->Add(itemStaticText11, 1, wxGROW|wxALL, 5);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::updateExtraTransactionData(int selIndex)
{
    wxString cat   =  _("Category         : ") + m_trans[selIndex]->catStr_  +   wxT("\n"); 
    wxString subcat = _("Sub Category  : ") + m_trans[selIndex]->subCatStr_ + wxT("\n");
    wxString notes =  _("Notes               : ") + mmReadyDisplayString(m_trans[selIndex]->notes_) + wxT("\n");
    wxString text = cat + subcat + notes;
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_DETAILS);
    st->SetLabel(text);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::setAccountSummary()
{
    double total = m_core->accountList_.getAccountSharedPtr(m_AccountID)->balance();
    wxString balance;
    mmCurrencyFormatter::formatDoubleToCurrency(total, balance);

    double reconciledBal = m_core->bTransactionList_.getReconciledBalance(m_AccountID);
    double acctInitBalance = m_core->accountList_.getAccountSharedPtr(m_AccountID)->initialBalance_;
    
    wxString recbalance;
    mmCurrencyFormatter::formatDoubleToCurrency(reconciledBal + acctInitBalance, recbalance);

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);

    wxString lbl  = wxString::Format(_("Account Balance : %s      Reconciled Balance : %s"), 
        balance.c_str(), recbalance.c_str());
    header->SetLabel(lbl);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::initVirtualListControl()
{
    // clear everything
    m_trans.clear();

#if defined (__WXMSW__)
    wxProgressDialog* pgd = new wxProgressDialog(_("Please Wait"), 
        _("Accessing Database"), 100, this, 
        wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH );

    pgd->Update(10);
#endif
   
    boost::shared_ptr<mmAccount> pAccount = m_core->accountList_.getAccountSharedPtr(m_AccountID);
    double acctInitBalance = pAccount->initialBalance_;
    boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
    wxASSERT(pCurrency);
    pCurrency->loadCurrencySettings();

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_HEADER);
    header->SetLabel(_("Account View : ") + pAccount->accountName_);

    setAccountSummary();

    wxString text;
    text += _("Category: \n"); 
    text += _("Sub Category: \n"); 
    text += _("Notes: \n"); 
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_DETAILS);
    st->SetLabel(text);

    int numTransactions = 0;
	double unseenBalance = 0.0;
    for (size_t i = 0; i < m_core->bTransactionList_.transactions_.size(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = m_core->bTransactionList_.transactions_[i];
        if ((pBankTransaction->accountID_ != m_AccountID) && (pBankTransaction->toAccountID_ != m_AccountID))
           continue;

        pBankTransaction->updateAllData(m_core, m_AccountID, pCurrency);

        bool toAdd = true;
		bool getBal = false;
        if (m_currentView == wxT("View Reconciled"))
        {
            if (pBankTransaction->status_ != wxT("R"))
                toAdd = false;
        }
        else if (m_currentView == wxT("View Void"))
        {
             if (pBankTransaction->status_ != wxT("V"))
                toAdd = false;
        }
        else if (m_currentView == wxT("View Flagged"))
        {
            if (pBankTransaction->status_ != wxT("F"))
                toAdd = false;
        }
        else if (m_currentView == wxT("View UnReconciled"))
        {
            if (pBankTransaction->status_ != wxT(""))
                toAdd = false;
        }
        else if (m_currentView == wxT("View Not-Reconciled"))
        {
            if (pBankTransaction->status_ == wxT("R"))
                toAdd = false;
        }
		else if (m_currentView == wxT("View Duplicates"))
        {
            if (pBankTransaction->status_ != wxT("D"))
                toAdd = false;
        }
        else  if (m_currentView == wxT("View Today"))
	   {
           wxDateTime dtBegin = wxDateTime::Now().GetDateOnly();
           wxDateTime dtEnd = dtBegin + wxTimeSpan(23, 59, 59);

           if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
               toAdd = false;

           getBal = true;
	    }
        else  if (m_currentView == wxT("View 30 days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());

            if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                toAdd = false;

			getBal = true;
        }
        else if (m_currentView == wxT("View 90 days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(90 * wxDateSpan::Day());

             if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                 toAdd = false;

			 getBal = true;
        }
        else if (m_currentView == wxT("View Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();

            if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                 toAdd = false;

			 getBal = true;
        }
        else if (m_currentView == wxT("View Last Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Month());
            today = wxDateTime::Now();
            wxDateTime dtEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));

            if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                 toAdd = false;

			 getBal = true;

        }

        if (toAdd)
        {
           m_trans.push_back(pBankTransaction.get());
           ++numTransactions;
		}
		else
		{
		   if (getBal)
		   {
			   if (pBankTransaction->status_ != wxT("V"))
			   {
				   if (pBankTransaction->transType_ == wxT("Deposit"))
				   {
					   unseenBalance += pBankTransaction->amt_;
				   }
				   else if (pBankTransaction->transType_ == wxT("Withdrawal"))
				   {
					   unseenBalance -= pBankTransaction->amt_;
				   }
				   else if (pBankTransaction->transType_ == wxT("Transfer"))
				   {
					   if (pBankTransaction->accountID_ == m_AccountID)
					   {
						   unseenBalance -= pBankTransaction->amt_;
					   }
					   else if (pBankTransaction->toAccountID_== m_AccountID)
					   {
						   unseenBalance += pBankTransaction->toAmt_;
					   }
				   }
			   }
		   }
        }
    }

#if defined (__WXMSW__)
    pgd->Update(30);
#endif

    // sort m_trans by date
    double initBalance = acctInitBalance + unseenBalance;
    if (m_currentView == wxT("View UnReconciled"))
    {
        initBalance = m_core->bTransactionList_.getReconciledBalance(m_AccountID);
    }

    std::sort(m_trans.begin(), m_trans.end(), sortTransactionsByDate);

#if defined (__WXMSW__)
    pgd->Update(50);
#endif

    for (size_t i = 0; i < m_trans.size(); ++i)
    {
        bool ok = m_trans[i] != 0;
        wxASSERT(ok);

        if (!ok) {
            continue;
        }

        mmBankTransaction &tr = *m_trans[i];

        if (tr.status_ != wxT("V"))
        {
            if (tr.transType_ == wxT("Deposit"))
            {
                initBalance += tr.amt_;
            }
            else if (tr.transType_ == wxT("Withdrawal"))
            {
                initBalance -= tr.amt_;
            }
            else if (tr.transType_ == wxT("Transfer"))
            {
                if (tr.accountID_ == m_AccountID)
                {
                    initBalance -= tr.amt_;
                }
                else if (tr.toAccountID_== m_AccountID)
                {
                    initBalance += tr.toAmt_;
                }
            }
        }
        
        tr.balance_ = initBalance;
        wxString balanceStr;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(initBalance, balanceStr);
        tr.balanceStr_ = balanceStr;
    }

#if defined (__WXMSW__)
    pgd->Update(70);
#endif

    /* Setup the Sorting */
     // decide whether top or down icon needs to be shown
    m_listCtrlAccount->SetColumnImage(g_sortcol, g_asc ? 5 : 4);
    
    // sort the table
    sortTable(); 

	m_listCtrlAccount->SetItemCount(numTransactions);

    if (m_trans.size() > 1)
    {
		if (g_asc)
		{
			m_listCtrlAccount->EnsureVisible(static_cast<long>(m_trans.size()) - 1);
		}
		else
		{
			m_listCtrlAccount->EnsureVisible(0);
		}
    }

#if defined (__WXMSW__)
    pgd->Update(100);
    pgd->Destroy();
#endif
}
//----------------------------------------------------------------------------

void mmCheckingPanel::OnDeleteTransaction(wxCommandEvent& event)
{
    m_listCtrlAccount->OnDeleteTransaction(event);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::OnNewTransaction(wxCommandEvent& event)
{
   m_listCtrlAccount->OnNewTransaction(event);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::OnEditTransaction(wxCommandEvent& event)
{
    m_listCtrlAccount->OnEditTransaction(event);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::initViewTransactionsHeader()
{
    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_PANELVIEW);
    if (m_currentView == wxT("View All Transactions"))
    {
        header->SetLabel(_("Viewing all transactions"));
    }
    else if (m_currentView == wxT("View Reconciled"))
    {
        header->SetLabel(_("Viewing Reconciled transactions"));
    }
    else if (m_currentView == wxT("View UnReconciled"))
    {
        header->SetLabel(_("Viewing Un-Reconciled transactions"));
    }
    else if (m_currentView == wxT("View Today"))
    {
       header->SetLabel(_("Viewing transactions from today"));
    }
    else if (m_currentView == wxT("View 30 days"))
    {
        header->SetLabel(_("Viewing transactions from last 30 days"));
    }
    else if (m_currentView == wxT("View 90 days"))
    {
        header->SetLabel(_("Viewing transactions from last 3 months"));
    }
    else if (m_currentView == wxT("View Current Month"))
    {
        header->SetLabel(_("Viewing transactions from current month"));
    }
    else if (m_currentView == wxT("View Last Month"))
    {
        header->SetLabel(_("Viewing transactions from last month"));
    }
    else if (m_currentView == wxT("View Not-Reconciled"))
    {
        header->SetLabel(_("Viewing All Except Reconciled Transactions"));
    }
}
//----------------------------------------------------------------------------

void mmCheckingPanel::OnViewPopupSelected(wxCommandEvent& event)
{
    int evt =  event.GetId();
    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_PANELVIEW);
    if (evt ==  MENU_VIEW_ALLTRANSACTIONS)
    {
        header->SetLabel(_("Viewing all transactions"));
        m_currentView = wxT("View All Transactions");
    }
    else if (evt == MENU_VIEW_RECONCILED)
    {
        header->SetLabel(_("Viewing Reconciled transactions"));
        m_currentView = wxT("View Reconciled");
    }
    else if (evt == MENU_VIEW_VOID)
    {
        header->SetLabel(_("Viewing Void transactions"));
        m_currentView = wxT("View Void");
    }
    else if (evt == MENU_VIEW_UNRECONCILED)
    {
        header->SetLabel(_("Viewing Un-Reconciled transactions"));
        m_currentView = wxT("View UnReconciled");
    }
    else if (evt == MENU_VIEW_FLAGGED)
    {
        header->SetLabel(_("Viewing Flagged transactions"));
        m_currentView = wxT("View Flagged");
    }
    else if (evt == MENU_VIEW_TODAY)
    {
        header->SetLabel(_("Viewing transactions from today"));
        m_currentView = wxT("View Today");
    }
    else if (evt == MENU_VIEW_LAST30)
    {
        header->SetLabel(_("Viewing transactions from last 30 days"));
        m_currentView = wxT("View 30 days");
    }
    else if (evt == MENU_VIEW_LAST3MONTHS)
    {
        header->SetLabel(_("Viewing transactions from last 3 months"));
        m_currentView = wxT("View 90 days");
    }
    else if (evt == MENU_VIEW_CURRENTMONTH)
    {
        header->SetLabel(_("Viewing transactions from current month"));
        m_currentView = wxT("View Current Month");
    }
    else if (evt == MENU_VIEW_LASTMONTH)
    {
        header->SetLabel(_("Viewing transactions from last month"));
        m_currentView = wxT("View Last Month");
    }
	else if (evt == MENU_VIEW_DUPLICATE)
    {
        header->SetLabel(_("Viewing duplicate transactions"));
        m_currentView = wxT("View Duplicates");
    }
    else if (evt == MENU_VIEW_NOTRECONCILED)
    {
        header->SetLabel(_("Viewing All except Reconciled Transactions"));
        m_currentView = wxT("View Not-Reconciled");
    }
    else if (evt == MENU_VIEW_DELETE_TRANS)
    {
        wxMessageDialog msgDlg(this, _("Do you really want to delete all the transactions shown?"),
            _("Confirm Transaction Deletion"),
            wxYES_NO);
        if (msgDlg.ShowModal() == wxID_YES)
        {
           //mmCheckingAccount* pAccount = dynamic_cast<mmCheckingAccount*>(m_core->accountList_.getAccountSharedPtr(m_AccountID).get());
            for (size_t i = 0; i < m_trans.size(); ++i)
            {
               m_core->bTransactionList_.deleteTransaction(m_AccountID, m_trans[i]->transactionID());
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
           //mmCheckingAccount* pAccount = dynamic_cast<mmCheckingAccount*>(m_core->accountList_.getAccountSharedPtr(m_AccountID).get());
            for (size_t i = 0; i < m_trans.size(); ++i)
            {
               if (m_trans[i]->status_ == wxT("F"))
               {
                  m_core->bTransactionList_.deleteTransaction(m_AccountID, m_trans[i]->transactionID());
               }
            }
        }
    }
    else
    {
        wxASSERT(false);
    }

    m_listCtrlAccount->DeleteAllItems();
    initVirtualListControl();
    m_listCtrlAccount->RefreshItems(0, static_cast<long>(m_trans.size()) - 1);
}
//----------------------------------------------------------------------------

void MyListCtrl::OnListItemSelected(wxListEvent& event)
{
    m_selectedIndex = event.GetIndex();
    m_cp->updateExtraTransactionData(m_selectedIndex);
}
//----------------------------------------------------------------------------

void MyListCtrl::OnItemRightClick(wxListEvent& event)
{
    m_selectedIndex = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Transaction"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Transaction"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Transaction"));
    menu.Append(MENU_ON_COPY_TRANSACTION, _("&Copy Transaction"));
    if (m_selectedForCopy != -1)
        menu.Append(MENU_ON_PASTE_TRANSACTION, _("&Paste Transaction"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_MARKRECONCILED, _("Mark As &Reconciled"));
    menu.Append(MENU_TREEPOPUP_MARKUNRECONCILED, _("Mark As &Unreconciled"));
    menu.Append(MENU_TREEPOPUP_MARKVOID, _("Mark As &Void"));
    menu.Append(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP, _("Mark For &Followup"));
	menu.Append(MENU_TREEPOPUP_MARKDUPLICATE, _("Mark As &Duplicate"));
    menu.AppendSeparator();

    wxMenu* subGlobalOpMenu = new wxMenu;
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARKRECONCILED_ALL, _("as Reconciled"));
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARKUNRECONCILED_ALL, _("as Unreconciled"));
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARKVOID_ALL, _("as Void"));
    subGlobalOpMenu->Append(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL, _("as needing Followup"));
	subGlobalOpMenu->Append(MENU_TREEPOPUP_MARKDUPLICATE_ALL, _("as Duplicate"));
    menu.Append(MENU_SUBMENU_MARK_ALL, _("Mark all "), subGlobalOpMenu);

    PopupMenu(&menu, event.GetPoint());
}
//----------------------------------------------------------------------------

void MyListCtrl::OnMarkTransactionDB(const wxString& status)
{
    if (m_selectedIndex == -1)
        return;
    int transID = m_cp->m_trans[m_selectedIndex]->transactionID();
    mmDBWrapper::updateTransactionWithStatus(*m_cp->getDb(), transID, status);
    m_cp->m_trans[m_selectedIndex]->status_ = status;

    if (m_cp->m_currentView != wxT("View All Transactions"))
    {
        DeleteAllItems();
        m_cp->initVirtualListControl();
    }
    RefreshItem(m_selectedIndex);
    m_cp->setAccountSummary();
}
//----------------------------------------------------------------------------

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
	 else if (evt == MENU_TREEPOPUP_MARKDUPLICATE)
         status = wxT("D");
     else
     {
        wxASSERT(false);
     }
      
    OnMarkTransactionDB(status);
}
//----------------------------------------------------------------------------

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
	 else if (evt == MENU_TREEPOPUP_MARKDUPLICATE_ALL)
         status = wxT("D");
     else
     {
        wxASSERT(false);
     }
         
     for (size_t i = 0; i < m_cp->m_trans.size(); ++i)
     {
        int transID = m_cp->m_trans[i]->transactionID();
        mmDBWrapper::updateTransactionWithStatus(*m_cp->getDb(), transID, status);
        m_cp->m_trans[i]->status_ = status;
     }

     if (m_cp->m_currentView != wxT("View All Transactions"))
     {
         DeleteAllItems();
         m_cp->initVirtualListControl();
         RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1); // refresh everything
     }
     m_cp->setAccountSummary();
}
//----------------------------------------------------------------------------

void MyListCtrl::OnColClick(wxListEvent& event)
{
    /* Figure out which column has to be sorted */
    if (event.GetColumn() == 7)
        return;

    /* Clear previous column image */
    SetColumnImage(m_sortCol, -1);

    m_sortCol = event.GetColumn();
    g_sortcol = m_sortCol;
    m_asc = !m_asc; // toggle sort order
    g_asc = m_asc;
    SetColumnImage(m_sortCol, m_asc ? 5 : 4); // decide whether top or down icon needs to be shown
    m_cp->sortTable();   // sort the table
    RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1); // refresh everything
}
//----------------------------------------------------------------------------

void MyListCtrl::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}
//----------------------------------------------------------------------------

wxString mmCheckingPanel::getItem(long item, long column)
{  
    if (m_trans.empty())
       return wxT("");

    if (item >= static_cast<long>(m_trans.size()) )
        return wxT("");

    if (!m_trans[item])
    {
        return wxT("");;
    }

    if (column == 0)
        return m_trans[item]->dateStr_;

    if (column == 1)
       return m_trans[item]->transNum_;

    if (column == 2)
        return m_trans[item]->payeeStr_;

    if (column == 3)
        return m_trans[item]->status_;

    if (column == 4)
       return m_trans[item]->fullCatStr_;

    if (column == 5)
       return m_trans[item]->withdrawalStr_;

    if (column == 6)
        return m_trans[item]->depositStr_; 
    
    if (column == 7)
        return m_trans[item]->balanceStr_;

	if (column == 8)
        return m_trans[item]->notes_;
		
    return wxT("");
}
//----------------------------------------------------------------------------

wxString MyListCtrl::OnGetItemText(long item, long column) const
{
    return m_cp->getItem(item, column);
}
//----------------------------------------------------------------------------

int MyListCtrl::OnGetItemImage(long item) const
{
   /* Returns the icon to be shown for each transaction */
   wxString status = m_cp->getItem(item, 3);
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
   else if (status == wxT("D"))
   {
        return 6;
   }
   return 3;
}
//----------------------------------------------------------------------------

/*
    Failed wxASSERT will hang application if active modal dialog presents on screen.
    Assertion's message box will be hidden until you press tab to activate one.
*/
wxListItemAttr* MyListCtrl::OnGetItemAttr(long item) const
{
    wxASSERT(m_cp);
    wxASSERT(item >= 0);

    size_t idx = item;
    bool ok = m_cp && idx < m_cp->m_trans.size();
    
    mmBankTransaction *tr = ok ? m_cp->m_trans[idx] : 0;
    bool in_the_future = tr && tr->date_ > wxDateTime::Now();

    MyListCtrl &self = *const_cast<MyListCtrl*>(this);

    if (in_the_future) // apply alternating background pattern
    {
        return item % 2 ? &self.m_attr3 : &self.m_attr4;
    }

    return item % 2 ? &self.m_attr1 : &self.m_attr2;

}
//----------------------------------------------------------------------------

void MyListCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case 'v':
        case 'V':
        case 'r':
        case 'R':
        case 'u':
        case 'U':
        case 'f':
        case 'F':
            // these are the keys we process ourselves
            break;

        default:
            event.Skip();
    }
}
//----------------------------------------------------------------------------

void MyListCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    if (m_selectedIndex == -1)
        return;

    m_selectedForCopy = m_cp->m_trans[m_selectedIndex]->transactionID();
}
//----------------------------------------------------------------------------

void MyListCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    if (m_selectedForCopy != -1)
    {
        wxString useOriginalDate =  mmDBWrapper::getINISettingValue(m_cp->m_inidb, wxT("USEORIGDATEONCOPYPASTE"), wxT("FALSE"));
        bool useOriginal = false;
        if (useOriginalDate == wxT("TRUE"))
            useOriginal = true;
        boost::shared_ptr<mmBankTransaction> pCopiedTrans = m_cp->m_core->bTransactionList_.copyTransaction(m_selectedForCopy, useOriginal);
        boost::shared_ptr<mmCurrency> pCurrencyPtr = m_cp->m_core->accountList_.getCurrencyWeakPtr(pCopiedTrans->accountID_).lock();
        pCopiedTrans->updateAllData(m_cp->m_core, pCopiedTrans->accountID_, pCurrencyPtr, true);
        m_cp->initVirtualListControl();
        RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
    }
}
//----------------------------------------------------------------------------

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

        case 'v':
        case 'V':
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKVOID);
                OnMarkTransaction(evt);
            }
            break;

        case 'r':
        case 'R':
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKRECONCILED);
                OnMarkTransaction(evt);
            }
            break;

        case 'u':
        case 'U':
            {
                 wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKUNRECONCILED);
                 OnMarkTransaction(evt);
            }
            break;

        case 'f':
        case 'F':
            {
                 wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP);
                 OnMarkTransaction(evt);
            }
            break;
		case 'd':
        case 'D':
            {
                 wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKDUPLICATE);
                 OnMarkTransaction(evt);
            }
            break;
        
      
        default:
            event.Skip();
    }
}
//----------------------------------------------------------------------------

void MyListCtrl::OnNewTransaction(wxCommandEvent& /*event*/)
{
    mmTransDialog *dlg = new mmTransDialog(m_cp->getDb(), m_cp->m_core, m_cp->accountID(), 
        0, false, m_cp->m_inidb, this );

    if ( dlg->ShowModal() == wxID_OK )
    {
        m_cp->initVirtualListControl();
        RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
        if (m_selectedIndex != -1)
        {
           SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
           SetItemState(m_selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
           EnsureVisible(m_selectedIndex);
        }
    }
    dlg->Destroy();
}
//----------------------------------------------------------------------------

void MyListCtrl::OnDeleteTransaction(wxCommandEvent& /*event*/)
{
	//check if a transaction is selected
    if (m_selectedIndex != -1)
	{
		//ask if they really want to delete
		wxMessageDialog msgDlg(this, _("Do you really want to delete the transaction?"),
											_("Confirm Transaction Deletion"),
											wxYES_NO|wxNO_DEFAULT);
		//if yes the user wants to delete the transaction then delete and refresh the 
		// transactions list
		if (msgDlg.ShowModal() == wxID_YES)
		{
			//find the topmost visible item - this will be used to set 
			// where to display the list again after refresh
			long topItemIndex = GetTopItem();
         
			//remove the transaction
			m_cp->m_core->bTransactionList_.deleteTransaction(m_cp->accountID(), m_cp->m_trans[m_selectedIndex]->transactionID());
			

         //initialize the transaction list to redo balances and images
			m_cp->initVirtualListControl();

         if (!m_cp->m_trans.empty())
         {
            //refresh the items showing from the point of the transaction delete down
            //the transactions above the deleted transaction won't change so they 
            // don't need to be refreshed
            RefreshItems(m_selectedIndex, static_cast<long>(m_cp->m_trans.size()) - 1);

            //set the deleted transaction index to the new selection and focus on it
            SetItemState(m_selectedIndex-1, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED, 
               wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED);

            //make sure the topmost item before transaction deletion is visible, otherwise 
            // the control will go back to the very top or bottom when refreshed
            EnsureVisible(topItemIndex);
         }
         else
         {
            SetItemCount(0);
            DeleteAllItems();
            m_selectedIndex = -1;
         }
		}
	}
}
//----------------------------------------------------------------------------

void MyListCtrl::OnEditTransaction(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex != -1)
	{
		mmTransDialog *dlg = new mmTransDialog(m_cp->getDb(), m_cp->m_core, m_cp->accountID(), 
		   m_cp->m_trans[m_selectedIndex]->transactionID(), true, m_cp->m_inidb, this);
		if ( dlg->ShowModal() == wxID_OK )
		{
			m_cp->initVirtualListControl();
			RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
			SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
			SetItemState(m_selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			EnsureVisible(m_selectedIndex);
		}
		dlg->Destroy();
	}
}
//----------------------------------------------------------------------------

void MyListCtrl::OnListItemActivated(wxListEvent& /*event*/)
{
    if (m_selectedIndex != -1)
	{
        //m_selectedIndex = event.GetIndex();
        mmTransDialog *dlg = new mmTransDialog(m_cp->getDb(), m_cp->m_core,  m_cp->accountID(), 
            m_cp->m_trans[m_selectedIndex]->transactionID(), true, m_cp->m_inidb, this);
        if ( dlg->ShowModal() == wxID_OK )
        {
            m_cp->initVirtualListControl();
            RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
            if (m_selectedIndex != -1)
            {
                SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
                SetItemState(m_selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                EnsureVisible(m_selectedIndex);
            }
        }
        dlg->Destroy();
    }
}
//----------------------------------------------------------------------------

MyListCtrl::MyListCtrl(
    mmCheckingPanel *cp, 
    wxWindow *parent,
    const wxWindowID id, 
    const wxPoint& pos,
    const wxSize& size, 
    long style
) : 
    wxListCtrl(parent, id, pos, size, style | wxWANTS_CHARS),
    m_sortCol(),
    m_asc(true),
    m_cp(cp),
    m_selectedIndex(-1),
    m_selectedForCopy(-1),
    m_attr1(*wxBLACK, mmColors::listAlternativeColor0, wxNullFont),
    m_attr2(*wxBLACK, mmColors::listAlternativeColor1, wxNullFont),
    m_attr3(mmColors::listFutureDateColor, mmColors::listAlternativeColor0, wxNullFont),
    m_attr4(mmColors::listFutureDateColor, mmColors::listAlternativeColor1, wxNullFont)
{
    wxASSERT(m_cp);
    
    const wxAcceleratorEntry entries[] =
    {
        wxAcceleratorEntry(wxACCEL_CTRL, 'C', MENU_ON_COPY_TRANSACTION),
        wxAcceleratorEntry(wxACCEL_CTRL, 'V', MENU_ON_PASTE_TRANSACTION),
        wxAcceleratorEntry(wxACCEL_ALT,  'N', MENU_ON_NEW_TRANSACTION)
    };

    wxAcceleratorTable tab(sizeof(entries)/sizeof(*entries), entries);
    SetAcceleratorTable(tab); 
}
//----------------------------------------------------------------------------

boost::shared_ptr<wxSQLite3Database> mmCheckingPanel::getDb() const 
{ 
    wxASSERT(m_core);
    return m_core->db_; 
}
//----------------------------------------------------------------------------

void mmCheckingPanel::sortTable()
{
    std::sort(m_trans.begin(), m_trans.end(), sortTransactions);
}
//----------------------------------------------------------------------------

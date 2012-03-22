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
#include "splittransactionsdialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "transactionfilterdialog.h"
//----------------------------------------------------------------------------
#include <wx/event.h>
#include <algorithm>
#include <vector>
#include <boost/unordered_map.hpp>
#include <wx/srchctrl.h>

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
#include "../resources/trans_from.xpm"
#include "../resources/trans_into.xpm"
#include "../resources/tipicon.xpm"
//----------------------------------------------------------------------------

namespace
{

enum EColumn
{ 
    COL_DATE_OR_TRANSACTION_ID = 0, 
    COL_TRANSACTION_NUMBER, 
    COL_PAYEE_STR, 
    COL_STATUS, 
    COL_CATEGORY, 
    COL_WITHDRAWAL, 
    COL_DEPOSIT,
    COL_BALANCE,
    COL_NOTES,
    COL_MAX, // number of columns
    COL_DEF_SORT = COL_DATE_OR_TRANSACTION_ID
};
//----------------------------------------------------------------------------

enum EIcons
{ 
    ICON_RECONCILED,
    ICON_VOID,
    ICON_FOLLOWUP,
    ICON_NONE,
    ICON_DESC,
    ICON_ASC,
    ICON_DUPLICATE,
    ICON_TRANS_WITHDRAWAL,
    ICON_TRANS_DEPOSIT,
    ICON_TRANS_TRANSFER
};
//----------------------------------------------------------------------------
EColumn g_sortcol = COL_DEF_SORT; // index of column to sort
bool g_asc = true; // asc\desc sorting
//----------------------------------------------------------------------------

EColumn toEColumn(long col)
{
    EColumn res = COL_DEF_SORT;
    if (col >= 0 && col < COL_MAX) res = static_cast<EColumn>(col);

    return res;
}
//----------------------------------------------------------------------------

/*
    Adds columns to list controls and setup their initial widths.
*/
void createColumns(wxSQLite3Database *inidb_, wxListCtrl &lst)
{
    wxASSERT(inidb_);

    lst.InsertColumn(COL_DATE_OR_TRANSACTION_ID, _("Date  "));
    lst.InsertColumn(COL_TRANSACTION_NUMBER, _("Number"), wxLIST_FORMAT_RIGHT);
    lst.InsertColumn(COL_PAYEE_STR, _("Payee"));
    lst.InsertColumn(COL_STATUS, _("Status"));
    lst.InsertColumn(COL_CATEGORY, _("Category"));
    lst.InsertColumn(COL_WITHDRAWAL, _("Withdrawal"), wxLIST_FORMAT_RIGHT);
    lst.InsertColumn(COL_DEPOSIT, _("Deposit"), wxLIST_FORMAT_RIGHT);
    lst.InsertColumn(COL_BALANCE, _("Balance"), wxLIST_FORMAT_RIGHT);
    lst.InsertColumn(COL_NOTES, _("Notes"));

    const int col_cnt = lst.GetColumnCount();
    wxASSERT(col_cnt == COL_MAX);

    // adjust columns' widths

    const wxChar* def_widths[COL_MAX] = 
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

    for (int i = 0; i < col_cnt; ++i)
    {
        const wxChar *def_width = def_widths[i];
        wxASSERT(def_width);

        wxString name = wxString::Format(wxT("CHECK_COL%d_WIDTH"), i);
        wxString val = mmDBWrapper::getINISettingValue(inidb_, name, def_width);
        
        long width = -1;
        
        if (val.ToLong(&width)) lst.SetColumnWidth(i, width);    
    }
}
//----------------------------------------------------------------------------

template<class T>
inline bool sort(const T &t1, const T &t2, bool asc)
{
    return asc ? t1 < t2 : t1 > t2;
}
//----------------------------------------------------------------------------
typedef bool (*sort_fun_t)(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc);
//----------------------------------------------------------------------------

bool sortTransByDate(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    bool res = false;

    if (t1->date_ == t2->date_) 
        res = sort(t1->transactionID(), t2->transactionID(), asc);
    else 
        res = sort(t1->date_, t2->date_, asc);

    return res;
}
//----------------------------------------------------------------------------

bool sortTransByNum(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    long v1 = 0;
    long v2 = 0;

    bool ok1 = t1->transNum_.ToLong(&v1);
    bool ok2 = t2->transNum_.ToLong(&v2);

    bool res = false;

    if (ok1 && ok2) 
        res = sort(v1, v2, asc);
    else 
        res = sort(t1->transNum_, t2->transNum_, asc);

    return res;
}
//----------------------------------------------------------------------------

bool sortTransByPayee(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
//  Primary sort by Payee, secondary sort by Date.
    bool res = false;
    if (t1->payeeStr_ == t2->payeeStr_)
        res = sort(t1->date_, t2->date_, asc);
    else 
        res = sort(t1->payeeStr_, t2->payeeStr_, asc);

    return res;
}
//----------------------------------------------------------------------------

bool sortTransByStatus(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
//  Primary sort by Status, Secondary sort by Date.
    bool res = false;
    if (t1->status_ == t2->status_)
        res = sort(t1->date_, t2->date_, asc);
    else 
        res = sort(t1->status_, t2->status_, asc);

    return res;
}
//----------------------------------------------------------------------------

bool sortTransByCateg(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
//  Primary sort by Category, Secondary sort by Date.
    bool res = false;
    if (t1->fullCatStr_ == t2->fullCatStr_)
        res = sort(t1->date_, t2->date_, asc);
    else 
        res = sort(t1->fullCatStr_, t2->fullCatStr_, asc);

    return res;
}
//----------------------------------------------------------------------------

/*
    FIXME: formatCurrencyToDouble too slow.
*/
bool sortAsCurrency(const wxString &s1, const wxString &s2, bool asc)
{
    double v1 = 0;
    double v2 = 0;

    bool ok1 = mmex::formatCurrencyToDouble(s1, v1);
    bool ok2 = mmex::formatCurrencyToDouble(s2, v2);

    bool res = false;

    if (ok1 && ok2)
        res = sort(v1, v2, asc);
    else
        res = sort(s1, s2, asc);

    return res;
}
//----------------------------------------------------------------------------

bool sortTransByWithdrowal(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sortAsCurrency(t1->withdrawalStr_, t2->withdrawalStr_, asc);
}
//----------------------------------------------------------------------------

bool sortTransByDeposit(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sortAsCurrency(t1->depositStr_, t2->depositStr_, asc);
}
//----------------------------------------------------------------------------

bool sortTransByBalanse(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sort(t1->balance_, t2->balance_, asc);
}
//----------------------------------------------------------------------------

bool sortTransByNotes(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sort(t1->notes_, t2->notes_, asc);
}
//----------------------------------------------------------------------------

sort_fun_t getSortFx(EColumn col)
{
    static sort_fun_t fx[COL_MAX] = {0};

    if (!fx[COL_DATE_OR_TRANSACTION_ID])
    {
        fx[COL_DATE_OR_TRANSACTION_ID] = sortTransByDate;
        fx[COL_TRANSACTION_NUMBER] = sortTransByNum;
        fx[COL_PAYEE_STR] = sortTransByPayee;
        fx[COL_STATUS] = sortTransByStatus;
        fx[COL_CATEGORY] = sortTransByCateg;
        fx[COL_WITHDRAWAL] = sortTransByWithdrowal;
        fx[COL_DEPOSIT] = sortTransByDeposit;
        fx[COL_BALANCE] = sortTransByBalanse;
        fx[COL_NOTES] = sortTransByNotes;
    }

    sort_fun_t f = fx[col];
    wxASSERT(f);

    return f;
}
//----------------------------------------------------------------------------

/*
    Return whether first element is greater than the second
*/
struct TransSort : public std::binary_function<const mmBankTransaction*, 
                                               const mmBankTransaction*, 
                                               bool>
{
    TransSort(EColumn col, bool asc) : m_f(getSortFx(col)), m_asc(asc) {}

    bool operator() (const mmBankTransaction *t1, const mmBankTransaction *t2) const
    {
        return m_f(t1, t2, m_asc);
    }

private:
    sort_fun_t m_f;
    bool m_asc;
};
//----------------------------------------------------------------------------

/*
    This function is not sort_fun_t.
*/
bool sortTransByDateAsc(const mmBankTransaction *t1, const mmBankTransaction *t2)
{
    bool res = false;

    if (t1->date_ == t2->date_)
        res = t1->transactionID() < t2->transactionID();
    else
        res = t1->date_ < t2->date_;

    return res;
}

} // namespace

//----------------------------------------------------------------------------
class TransactionListCtrl : public wxListCtrl
{
public:
    TransactionListCtrl(mmCheckingPanel *cp, wxWindow *parent,const wxWindowID id, const wxPoint& pos,const wxSize& size, long style);

    bool getSortOrder() const { return m_asc; }
    EColumn getSortColumn() const { return m_sortCol; }

    void setSortOrder(bool asc) { m_asc = asc; }
    void setSortColumn(EColumn col) { m_sortCol = col; }

    void setColumnImage(EColumn col, int image);

    void OnNewTransaction(wxCommandEvent& event);
    void OnDeleteTransaction(wxCommandEvent& event);
    void OnEditTransaction(wxCommandEvent& event);
    void OnMoveTransaction(wxCommandEvent& event);
    /// Displays the split categories for the selected transaction
    void OnViewSplitTransaction(wxCommandEvent& event);

private:
    DECLARE_NO_COPY_CLASS(TransactionListCtrl)
    DECLARE_EVENT_TABLE()

    mmCheckingPanel *m_cp;
    long m_selectedIndex;
    long m_selectedForCopy;

    wxListItemAttr m_attr1; // style1
    wxListItemAttr m_attr2; // style2
    wxListItemAttr m_attr3; // style, for future dates
    wxListItemAttr m_attr4; // style, for future dates

    EColumn m_sortCol;
    bool m_asc;

    /* required overrides for virtual style list control */
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemDeselected(wxListEvent& event);
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

    //  Returns the account ID by user selection
    int destinationAccountID(wxString accName);
    void refreshVisualList();
};
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(mmCheckingPanel, wxPanel)

    EVT_BUTTON(wxID_NEW,         mmCheckingPanel::OnNewTransaction)
    EVT_BUTTON(wxID_EDIT,        mmCheckingPanel::OnEditTransaction)
    EVT_BUTTON(wxID_DELETE,      mmCheckingPanel::OnDeleteTransaction)
    EVT_BUTTON(wxID_MOVE_FRAME,        mmCheckingPanel::OnMoveTransaction)

    EVT_MENU(MENU_VIEW_ALLTRANSACTIONS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_RECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_UNRECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_NOTRECONCILED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_VOID, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_FLAGGED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_TODAY, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LAST30, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LAST90, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LAST3MONTHS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DUPLICATE, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DELETE_TRANS, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_DELETE_FLAGGED, mmCheckingPanel::OnViewPopupSelected)

    EVT_MENU(MENU_TREEPOPUP_DELETE_VIEWED, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_TREEPOPUP_DELETE_FLAGGED, mmCheckingPanel::OnViewPopupSelected)

    EVT_MENU(MENU_VIEW_CURRENTMONTH, mmCheckingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_LASTMONTH, mmCheckingPanel::OnViewPopupSelected)

    //EVT_MENU(ID_PANEL_CHECKING_STATIC_BITMAP_FILTER, mmCheckingPanel::OnFilterTransactions)
    EVT_SEARCHCTRL_SEARCH_BTN(wxID_FIND, mmCheckingPanel::OnSearchTxtEntered)
    EVT_TEXT_ENTER(wxID_FIND, mmCheckingPanel::OnSearchTxtEntered)

END_EVENT_TABLE()
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TransactionListCtrl, wxListCtrl)

    EVT_LIST_ITEM_SELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, TransactionListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, TransactionListCtrl::OnListItemDeselected)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_CHECKING_LISTCTRL_ACCT, TransactionListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_CHECKING_LISTCTRL_ACCT, TransactionListCtrl::OnItemRightClick)

    EVT_MENU(MENU_TREEPOPUP_MARKRECONCILED,   TransactionListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKUNRECONCILED, TransactionListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKVOID,         TransactionListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP, TransactionListCtrl::OnMarkTransaction)
    EVT_MENU(MENU_TREEPOPUP_MARKDUPLICATE,          TransactionListCtrl::OnMarkTransaction)

    EVT_MENU(MENU_TREEPOPUP_MARKRECONCILED_ALL,         TransactionListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKUNRECONCILED_ALL,       TransactionListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKVOID_ALL,               TransactionListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL, TransactionListCtrl::OnMarkAllTransactions)
    EVT_MENU(MENU_TREEPOPUP_MARKDUPLICATE_ALL,          TransactionListCtrl::OnMarkAllTransactions)

    EVT_MENU(MENU_TREEPOPUP_NEW,                TransactionListCtrl::OnNewTransaction)
    EVT_MENU(MENU_TREEPOPUP_DELETE,             TransactionListCtrl::OnDeleteTransaction)
    EVT_MENU(MENU_TREEPOPUP_EDIT,               TransactionListCtrl::OnEditTransaction)
    EVT_MENU(MENU_TREEPOPUP_MOVE,               TransactionListCtrl::OnMoveTransaction)

    EVT_LIST_COL_CLICK(ID_PANEL_CHECKING_LISTCTRL_ACCT, TransactionListCtrl::OnColClick)
    EVT_LIST_KEY_DOWN(ID_PANEL_CHECKING_LISTCTRL_ACCT, TransactionListCtrl::OnListKeyDown)

    EVT_MENU(MENU_ON_COPY_TRANSACTION, TransactionListCtrl::OnCopy) 
    EVT_MENU(MENU_ON_PASTE_TRANSACTION, TransactionListCtrl::OnPaste) 
    EVT_MENU(MENU_ON_NEW_TRANSACTION, TransactionListCtrl::OnNewTransaction) 

    EVT_MENU(MENU_TREEPOPUP_VIEW_SPLIT_CATEGORIES, TransactionListCtrl::OnViewSplitTransaction)

    EVT_CHAR(TransactionListCtrl::OnChar)

END_EVENT_TABLE();

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
    mmPanelBase(NULL, inidb, core),
    filteredBalance_(0.0),
    m_listCtrlAccount(),
    m_AccountID(accountID)
{
    wxASSERT(core_);
    wxASSERT(inidb_);

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

bool mmCheckingPanel::Create(
    wxWindow *parent,
    wxWindowID winid, const wxPoint& pos, 
    const wxSize& size,long style, const wxString& name
)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    bool ok = wxPanel::Create(parent, winid, pos, size, style, name);

    if (ok) 
    {
        Freeze();
        CreateControls();
        initViewTransactionsHeader();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);

        /* Set up the transaction filter.  The transFilter dialog will be destroyed
           when the checking panel is destroyed. */
        transFilterActive_ = false;
        transFilterDlg_    = new TransFilterDialog(core_, this); 

        //show progress bar only when panel created 
        wxProgressDialog dlg(_("Please Wait"), _("Accessing Database"), 100, this, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH );
        initVirtualListControl(&dlg);
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

    inidb_->Begin();
    {
        for (int i = 0; i < cols; ++i)
        {
            wxString name = wxString::Format(wxT("CHECK_COL%d_WIDTH"), i);
            int width = m_listCtrlAccount->GetColumnWidth(i);

            mmDBWrapper::setINISettingValue(inidb_, name, wxString() << width); 
        }

        // sorting column index
        mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_SORT_COL"), wxString() << g_sortcol); 

        // asc\desc sorting flag
        mmDBWrapper::setINISettingValue(inidb_, wxT("CHECK_ASC"), wxString() << g_asc);
    }
    inidb_->Commit();
}
//----------------------------------------------------------------------------

void mmCheckingPanel::sortTable()
{
    std::sort(m_trans.begin(), m_trans.end(), TransSort(g_sortcol, g_asc));
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
            menu.Append(MENU_VIEW_FLAGGED, _("View \"Follow Up\" Transactions"));
            menu.Append(MENU_VIEW_DUPLICATE, _("View Duplicate Transactions"));
            menu.AppendSeparator();
            menu.Append(MENU_VIEW_TODAY, _("View Transactions for today"));
            menu.Append(MENU_VIEW_CURRENTMONTH, _("View Transactions for current month"));
            menu.Append(MENU_VIEW_LAST30, _("View Transactions for last 30 days"));
            menu.Append(MENU_VIEW_LAST90, _("View Transactions for last 90 days"));
            menu.Append(MENU_VIEW_LASTMONTH, _("View Transactions for last month"));
            menu.Append(MENU_VIEW_LAST3MONTHS, _("View Transactions for last 3 months"));
            menu.AppendSeparator();
            menu.Append(MENU_VIEW_DELETE_TRANS, _("Delete all transactions in current view"));
            menu.Append(MENU_VIEW_DELETE_FLAGGED, _("Delete all \"Follow Up\" transactions"));

            PopupMenu(&menu, event.GetPosition());

            break;
        }
        case ID_PANEL_CHECKING_STATIC_BITMAP_FILTER : 
        {
            wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_CHECKING_STATIC_BITMAP_FILTER);
            GetEventHandler()->AddPendingEvent(ev); 

            break;
        }
    }
    event.Skip();
} 
//----------------------------------------------------------------------------

void mmCheckingPanel::CreateControls()
{    
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer9);
    this->SetBackgroundColour(mmColors::listBackColor);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);
    headerPanel->SetBackgroundColour(mmColors::listBackColor);

    wxGridSizer* itemBoxSizerVHeader2 = new wxGridSizer(2,1,5,20);
    itemBoxSizerVHeader->Add(itemBoxSizerVHeader2);

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, ID_PANEL_CHECKING_STATIC_HEADER, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    itemBoxSizerVHeader2->Add(itemStaticText9, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 0);

    wxBoxSizer* itemBoxSizerHHeader2 = new wxBoxSizer(wxHORIZONTAL);
    wxFlexGridSizer* itemFlexGridSizerHHeader2 = new wxFlexGridSizer(5,1,1);
    itemBoxSizerVHeader2->Add(itemBoxSizerHHeader2);
    itemBoxSizerHHeader2->Add(itemFlexGridSizerHHeader2);

    wxBitmap itemStaticBitmap(rightarrow_xpm);
    wxStaticBitmap* itemStaticBitmap3 = new wxStaticBitmap( headerPanel, ID_PANEL_CHECKING_STATIC_BITMAP_VIEW, 
        itemStaticBitmap, wxDefaultPosition, wxSize(16, 16), 0 );
    itemFlexGridSizerHHeader2->Add(itemStaticBitmap3);
    itemStaticBitmap3->Connect(ID_PANEL_CHECKING_STATIC_BITMAP_VIEW, wxEVT_RIGHT_DOWN, wxMouseEventHandler(mmCheckingPanel::OnFilterResetToViewAll), NULL, this);
    itemStaticBitmap3->Connect(ID_PANEL_CHECKING_STATIC_BITMAP_VIEW, wxEVT_LEFT_DOWN, wxMouseEventHandler(mmCheckingPanel::OnMouseLeftDown), NULL, this);

    itemStaticTextMainFilter_ = new wxStaticText( headerPanel, ID_PANEL_CHECKING_STATIC_PANELVIEW, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizerHHeader2->Add(itemStaticTextMainFilter_);

    itemFlexGridSizerHHeader2->AddSpacer(20);
    
    itemStaticBitmap31_ = new wxStaticBitmap( headerPanel, ID_PANEL_CHECKING_STATIC_BITMAP_FILTER, 
        itemStaticBitmap, wxDefaultPosition, wxSize(16, 16), 0 );
    itemFlexGridSizerHHeader2->Add(itemStaticBitmap31_, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 150);
    itemStaticBitmap31_->Enable(false);
    itemStaticBitmap31_->Connect(ID_PANEL_CHECKING_STATIC_BITMAP_FILTER, wxEVT_LEFT_DOWN, wxMouseEventHandler(mmCheckingPanel::OnFilterTransactions), NULL, this);
    itemStaticBitmap31_->Connect(ID_PANEL_CHECKING_STATIC_BITMAP_FILTER, wxEVT_RIGHT_DOWN, wxMouseEventHandler(mmCheckingPanel::OnFilterTransactions), NULL, this);
    
    statTextTransFilter_ = new wxStaticText( headerPanel, ID_PANEL_CHECKING_STATIC_FILTER, 
        _("Transaction Filter"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizerHHeader2->Add(statTextTransFilter_);
    statTextTransFilter_->Enable(false);

    m_currentView = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWTRANSACTIONS"), VIEW_TRANS_ALL_STR);
    
    initViewTransactionsHeader();
    wxBoxSizer* itemBoxSizerHHeader = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader, 0, wxALL, 1);

    wxStaticText* itemStaticText10 = new wxStaticText( headerPanel, 
            ID_PANEL_CHECKING_STATIC_BALHEADER, wxT(""), wxDefaultPosition, wxSize(800,20), 0 );
    itemBoxSizerHHeader->Add(itemStaticText10, 0, wxRIGHT|wxEXPAND, 50);
    
    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( this, 
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
    m_imageList->Add(wxBitmap(trans_from_xpm));
    m_imageList->Add(wxBitmap(trans_into_xpm));
    //m_imageList->Add(wxBitmap(trans_transfer_xpm));

    m_listCtrlAccount = new TransactionListCtrl( this, itemSplitterWindow10, 
        ID_PANEL_CHECKING_LISTCTRL_ACCT, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    
    m_listCtrlAccount->SetImageList(m_imageList.get(), wxIMAGE_LIST_SMALL);
    m_listCtrlAccount->SetBackgroundColour(mmColors::listDetailsPanelColor);
    m_listCtrlAccount->setSortOrder(g_asc);
    m_listCtrlAccount->setSortColumn(g_sortcol);
    m_listCtrlAccount->SetFocus();
    
    createColumns(inidb_, *m_listCtrlAccount);

    {   // load the global variables
        long val = COL_DEF_SORT;
        wxString strVal = mmDBWrapper::getINISettingValue(inidb_, wxT("CHECK_SORT_COL"), wxString() << val);
        if (strVal.ToLong(&val)) g_sortcol = toEColumn(val);
        // --
        val = 1; // asc sorting default
        strVal = mmDBWrapper::getINISettingValue(inidb_, wxT("CHECK_ASC"), wxString() << val);
        if (strVal.ToLong(&val)) g_asc = val != 0;

        // --
        m_listCtrlAccount->setSortColumn(g_sortcol);
        m_listCtrlAccount->setSortOrder(g_asc);
        m_listCtrlAccount->setColumnImage(m_listCtrlAccount->getSortColumn(), m_listCtrlAccount->getSortOrder() ? ICON_ASC : ICON_DESC); // asc\desc sort mark (arrow)
        //m_listCtrlAccount->SetBackgroundColour(mmColors::listDetailsPanelColor);
    }

    wxPanel *itemPanel12 = new wxPanel(itemSplitterWindow10, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    itemPanel12->SetBackgroundColour(mmColors::listBackColor);
    itemSplitterWindow10->SplitHorizontally(m_listCtrlAccount, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);

    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton6 = new wxButton(itemPanel12, wxID_NEW, _("&New"));
    itemButton6->SetToolTip(_("New Transaction"));
    itemBoxSizer5->Add(itemButton6, 0, wxRIGHT, 5);

    wxButton* itemButton7 = new wxButton(itemPanel12, wxID_EDIT, _("&Edit"));
    itemButton7->SetToolTip(_("Edit selected transaction"));
    itemBoxSizer5->Add(itemButton7, 0, wxRIGHT, 5);
    itemButton7->Enable(false);

    wxButton* itemButton8 = new wxButton(itemPanel12, wxID_DELETE, _("&Delete"));
    itemButton8->SetToolTip(_("Delete selected transaction"));
    itemBoxSizer5->Add(itemButton8, 0, wxRIGHT, 5);
    itemButton8->Enable(false);

    wxButton* itemButton9 = new wxButton(itemPanel12, wxID_MOVE_FRAME, _("&Move"));
    itemButton9->SetToolTip(_("Move selected transaction to another account"));
    itemBoxSizer5->Add(itemButton9, 0, wxRIGHT, 5);
    itemButton9->Enable(false);
    
    wxSearchCtrl* searchCtrl = new wxSearchCtrl(itemPanel12, 
        wxID_FIND, wxEmptyString, wxDefaultPosition, wxSize(100,-1), wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB, wxDefaultValidator, _("Search"));
    itemBoxSizer5->Add(searchCtrl, 0, wxTOP, 1);
    searchCtrl->SetToolTip(_("Enter any string to find it in the nearest transaction notes"));

    //Infobar-mini 
    wxStaticText* itemStaticText44 = new wxStaticText( itemPanel12, ID_PANEL_CHECKING_STATIC_MINI, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer5->Add(itemStaticText44, 1, wxGROW|wxTOP|wxLEFT, 5);

    //Infobar 
    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel12, 
        ID_PANEL_CHECKING_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxSize(200,-1), wxTE_MULTILINE|wxTE_WORDWRAP);
    itemBoxSizer4->Add(itemStaticText11, 1, wxGROW|wxALL, 5);
    //Show tips when no transaction selected 
    showTips();
}
//----------------------------------------------------------------------------

void mmCheckingPanel::enableEditDeleteButtons(bool en)
{
    wxButton* bEdit = (wxButton*)FindWindow(wxID_EDIT);
    wxButton* bDelete = (wxButton*)FindWindow(wxID_DELETE);
    wxButton* bMove = (wxButton*)FindWindow(wxID_MOVE_FRAME);

    bEdit->Enable(en);
    bDelete->Enable(en);
    if (core_->getNumBankAccounts() > 1)
        bMove->Enable(en);
    else
        bMove->Enable(false);   
   
}
//----------------------------------------------------------------------------

void mmCheckingPanel::updateExtraTransactionData(int selIndex)
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_DETAILS); 
    wxStaticText* stm = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_MINI);
    if (selIndex > -1) 
    { 
        enableEditDeleteButtons(true);
        st->SetLabel(m_trans[selIndex]->notes_);
        wxString miniStr;
        miniStr = getMiniInfoStr(selIndex);
        
        //Show only first line but full string set as tooltip
        if (miniStr.Find(wxT("\n")) > 1 && !miniStr.IsEmpty()) 
        {
            stm->SetLabel(miniStr.substr(0,miniStr.Find(wxT("\n"))) + wxT(" ..."));
            stm->SetToolTip(miniStr);
        } 
        else 
        {
            stm->SetLabel(miniStr);
            stm->SetToolTip(miniStr);
        }

    } 
    else 
    {
        stm->SetLabel(wxT(""));
        enableEditDeleteButtons(false);
        showTips() ;
    }
}
//----------------------------------------------------------------------------
wxString mmCheckingPanel::getMiniInfoStr(int selIndex) const
{
    char sql[] =
    "select  ta.accountname  as INTOACC, "
    "a.accountname as FROMACC, c.transcode as TRANSCODE, "
    "c.TRANSAMOUNT as TRANSAMOUNT,  TOTRANSAMOUNT, "
    "cf.pfx_symbol as PFX_SYMBOL, cf.sfx_symbol as SFX_SYMBOL, "
    "tcf.pfx_symbol as TOPFX_SYMBOL, tcf.sfx_symbol as TOSFX_SYMBOL, "
    "cf.CURRENCYNAME as CURRENCYNAME,  tcf.CURRENCYNAME as TOCURRENCYNAME, "
    "cf.CURRENCYID as CURRENCYID,  tcf.CURRENCYID as TOCURRENCYID, "
    "cf.BASECONVRATE as BASECONVRATE, "
    "tcf.BASECONVRATE as TOBASECONVRATE, "
    "c.ACCOUNTID as ACCOUNTID, "
    "c.TOACCOUNTID as TOACCOUNTID, "
    "i.infovalue as BASECURRENCYID "
    "from  checkingaccount_v1 c "
    "left join  accountlist_v1  ta on ta.ACCOUNTID=c.TOACCOUNTID "
    "left join  accountlist_v1  a on a.ACCOUNTID=c.ACCOUNTID "
    "left join currencyformats_v1 tcf on tcf.currencyid=ta.currencyid "
    "left join currencyformats_v1 cf on cf.currencyid=a.currencyid "
    "left join infotable_v1 i on i.infoname='BASECURRENCYID' "
    "where c.transid = ? ";
    
    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(sql);
    st.Bind(1, m_trans[selIndex]->transactionID());

    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    wxString intoaccStr = q1.GetString(wxT("INTOACC"));
    wxString fromaccStr = q1.GetString(wxT("FROMACC"));
    int basecurrencyid = q1.GetInt(wxT("BASECURRENCYID"));
    wxString transcodeStr = q1.GetString(wxT("TRANSCODE"));

    wxString cursfxStr = q1.GetString(wxT("SFX_SYMBOL"));
    wxString tocursfxStr = q1.GetString(wxT("TOSFX_SYMBOL"));
    wxString curpfxStr = q1.GetString(wxT("PFX_SYMBOL"));
    wxString tocurpfxStr = q1.GetString(wxT("TOPFX_SYMBOL"));
    wxString currencynameStr = q1.GetString(wxT("CURRENCYNAME"));
    wxString tocurrencynameStr = q1.GetString(wxT("TOCURRENCYNAME"));
    int currencyid = q1.GetInt(wxT("CURRENCYID"));
    double amount = q1.GetDouble(wxT("TRANSAMOUNT"));
    wxString amountStr;
    double convrate = q1.GetDouble(wxT("BASECONVRATE"));
    double toconvrate = q1.GetDouble(wxT("TOBASECONVRATE"));
    int accountId = q1.GetInt(wxT("ACCOUNTID"));
    int toaccountId = q1.GetInt(wxT("TOACCOUNTID"));

    wxString infoStr = wxT("");
    if (transcodeStr == TRANS_TYPE_TRANSFER_STR)
    {
        int tocurrencyid = q1.GetInt(wxT("TOCURRENCYID"));
        double toamount = q1.GetDouble(wxT("TOTRANSAMOUNT"));
        wxString toamountStr;
        double convertion = 0.0;
        if (toamount != 0.0 && amount != 0.0)
            convertion = ( convrate < toconvrate ? amount/toamount : toamount/amount);
        wxString convertionStr;

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(toaccountId).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
        mmex::formatDoubleToCurrency(toamount, toamountStr);
        mmex::formatDoubleToCurrencyEdit(convertion, convertionStr);

        pCurrencyPtr = core_->getCurrencyWeakPtr(accountId).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
        mmex::formatDoubleToCurrency(amount, amountStr);
        //if (currencyid == basecurrencyid)
        mmex::formatDoubleToCurrencyEdit(convertion, convertionStr);

        infoStr << amountStr << wxT(" ");
        if (amount!=toamount || tocurrencyid != currencyid)
            infoStr << wxT("-> ")  << toamountStr << wxT(" ");
        infoStr << _("From") << wxT(" ") << fromaccStr << wxT(" ") << _("to ") << intoaccStr;

        if (tocurrencyid != currencyid)
        {
            infoStr << wxT(" ( "); 
            if (accountId == m_AccountID && convrate < toconvrate)
            {
                infoStr  << tocurpfxStr << wxT("1") << tocursfxStr << wxT(" = ") << curpfxStr << convertionStr << cursfxStr << wxT(" ");
            } 
            else if (accountId == m_AccountID && convrate > toconvrate) 
            {
                infoStr << curpfxStr << wxT("1") << cursfxStr << wxT(" = ") << tocurpfxStr << convertionStr << tocursfxStr << wxT(" ");
            } 
            else if (accountId != m_AccountID && convrate < toconvrate)
            {
                infoStr << tocurpfxStr << wxT("1") << tocursfxStr << wxT(" = ") << curpfxStr << convertionStr << cursfxStr << wxT(" ");
            } 
            else 
            {
                infoStr << curpfxStr << wxT("1") << cursfxStr << wxT(" = ") << tocurpfxStr << convertionStr << tocursfxStr << wxT(" ");
            } 
            infoStr << wxT(" )");
        }
    }
    else //For deposits and withdrawals calculates amount in base currency
    {
        //if (split_)
        {
            infoStr =  mmDBWrapper::getSplitTrxNotes(core_->db_.get(), m_trans[selIndex]->transactionID());
            //infoStr.RemoveLast(1);
        }
        
        if (currencyid != basecurrencyid) //Show nothing if account currency is base
        {
            //load settings for base currency
            wxString currencyName = core_->getCurrencyName(basecurrencyid);
            boost::shared_ptr<mmCurrency> pCurrency = core_->getCurrencySharedPtr(currencyName);
            wxASSERT(pCurrency);
            wxString basecuramountStr;
            mmDBWrapper::loadSettings(core_->db_.get(), pCurrency->currencyID_);
            mmex::formatDoubleToCurrency(amount*convrate, basecuramountStr);

            pCurrency = core_->getCurrencyWeakPtr(accountId).lock();
            wxASSERT(pCurrency);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrency);
            mmex::formatDoubleToCurrency(amount, amountStr);

            //output
            infoStr << amountStr << wxT(" = ") << basecuramountStr;
        }
    }
    return infoStr;
}
//---------------------------
void mmCheckingPanel::showTips()
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_DETAILS); 
    st->SetLabel(Tips(TIPS_BANKS));
}
//----------------------------------------------------------------------------
void mmCheckingPanel::setAccountSummary()
{
    double checking_bal = core_->getBalance(m_AccountID);
    double reconciledBal = core_->getReconciledBalance(m_AccountID);
    double acctInitBalance = core_->getAccountSharedPtr(m_AccountID)->initialBalance_;
    
    wxString balance, recbalance;
    mmex::formatDoubleToCurrency(checking_bal + acctInitBalance, balance);
    mmex::formatDoubleToCurrency(reconciledBal + acctInitBalance, recbalance);
    
    wxString diffbal;
    mmex::formatDoubleToCurrency(checking_bal - reconciledBal, diffbal);

    wxString filteredBalanceStr;
    mmex::formatDoubleToCurrency(filteredBalance_, filteredBalanceStr);

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);
    bool show_displayed_balance_ = (transFilterActive_ || (m_currentView != VIEW_TRANS_ALL_STR));

    wxString lbl  = wxString() << _("Account Bal: ") << balance.c_str() << wxT("      ") 
                               << _("Reconciled Bal: ") << recbalance.c_str() << wxT("      ")
                               << _("Diff: ") << diffbal.c_str() << wxT("      ")
                               << (show_displayed_balance_ ? _("Displayed Bal: ") : wxT(""))
                               << (show_displayed_balance_ ? filteredBalanceStr : wxT(""));
    
    header->SetLabel(lbl);
}
//----------------------------------------------------------------------------

typedef boost::shared_ptr<mmBankTransaction> TransactionPtr;
struct TransactionPtr_Matcher
{
    virtual ~TransactionPtr_Matcher() {}
    virtual bool Match(const TransactionPtr&) = 0;  
};
typedef boost::shared_ptr<TransactionPtr_Matcher> TransactionPtr_MatcherPtr;

template <class EqualTraits = std::equal_to<wxString> >
class MatchTransaction_Status: public TransactionPtr_Matcher
{
    wxString m_name;
    EqualTraits m_equalTraits;

public:
    MatchTransaction_Status(wxString n): m_name(n) {}

    bool Match(const TransactionPtr& pTrans)
    {
        return m_equalTraits(pTrans->status_, m_name);
    }
};

template <typename DateTimeProvider>
class MatchTransaction_DateTime: public TransactionPtr_Matcher
{
public:
    bool Match(const TransactionPtr& pTrans)
    {
        wxASSERT(pTrans);
        wxDateTime startRange = DateTimeProvider::StartRange();
        wxDateTime endRange = DateTimeProvider::EndRange();
        // ::OutputDebugStringW((wxT("- start: ") + startRange.Format(L"%x %X") + wxT(", end: ") + endRange.Format(L"%x %X") + wxT("\r\n")).c_str());
        return pTrans->date_.IsBetween(startRange, endRange);
    }
};
//---------------------------------------------------------------------------

typedef std::pair<boost::shared_ptr<TransactionPtr_Matcher>, bool> TransactionMatchData;
typedef boost::unordered_map<wxString, TransactionMatchData> TransactionMatchMap;

const TransactionMatchMap& initTransactionMatchMap()
{
    static TransactionMatchMap map;

    map[VIEW_TRANS_RECONCILED_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("R"))), false);
    map[VIEW_TRANS_UNRECONCILED_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT(""))), false);
    map[VIEW_TRANS_NOT_RECONCILED_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status< std::not_equal_to<wxString> >(wxT("R"))), false);
    map[VIEW_TRANS_VOID] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("V"))), false);
    map[VIEW_TRANS_FLAGGED] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("F"))), false);
    map[VIEW_TRANS_DUPLICATES] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("D"))), false);

    map[VIEW_TRANS_TODAY_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::Today>()), true);
    map[VIEW_TRANS_LAST_30_DAYS_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastDays<30> >()), true);
    map[VIEW_TRANS_LAST_90_DAYS_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastDays<90> >()), true);
    map[VIEW_TRANS_LAST_MONTH_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastMonths<1, 1> >()), true);
    map[VIEW_TRANS_CURRENT_MONTH_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::CurrentMonth<> >()), true);
    map[VIEW_TRANS_LAST_3MONTHS_STR] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastMonths<2> >()), true);

    return map;
}
static const TransactionMatchMap& s_transactionMatchers_Map = initTransactionMatchMap();

void mmCheckingPanel::initVirtualListControl(wxProgressDialog* pgd)
{
    // clear everything
    m_trans.clear();

    if (pgd)
    pgd->Update(20);
   
    boost::shared_ptr<mmAccount> pAccount = core_->getAccountSharedPtr(m_AccountID);
    boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
    wxASSERT(pCurrency);
    pCurrency->loadCurrencySettings();

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_HEADER);
    header->SetLabel(_("Account View : ") + pAccount->name_);

    filteredBalance_ = 0.0;

    /**********************************************************************************
     Stage 1
     For the account being viewed, we need to get:
     1. All entries for the account to determine account balances. [ v_transPtr ]
     2. All entries for the account to be displayed.               [ m_trans    ]
    **********************************************************************************/
    int numTransactions = 0;
    std::vector<mmBankTransaction*> v_transPtr;
    for (size_t i = 0; i < core_->bTransactionList_.transactions_.size(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = core_->bTransactionList_.transactions_[i];
        if ((pBankTransaction->accountID_ != m_AccountID) && (pBankTransaction->toAccountID_ != m_AccountID))
           continue;

        pBankTransaction->updateAllData(core_, m_AccountID, pCurrency);

        // Store all account transactions to determine the balances.
        v_transPtr.push_back(pBankTransaction.get());

        bool toAdd = true;
//      bool getBal = false;
        if (s_transactionMatchers_Map.count(m_currentView) > 0)
        {
            // boost::shared_ptr<TransactionPtr_Matcher> pMatcher;
            TransactionMatchMap::const_iterator it = s_transactionMatchers_Map.find(m_currentView);
            TransactionMatchMap::const_iterator end;
            if (it != end)
            {
                TransactionMatchMap::value_type pair = *it;
                
                TransactionMatchData data = pair.second;
                TransactionPtr_MatcherPtr matcher = data.first;
                wxASSERT(matcher);

                toAdd = matcher->Match(pBankTransaction);
//              getBal = data.second;
            }
        }

        if (transFilterActive_)
        {
            toAdd  = false;  // remove transaction from list and add if wanted.
            if ( transFilterDlg_->byDateRange(pBankTransaction->date_) &&
                 transFilterDlg_->byPayee(pBankTransaction->payeeStr_) &&
                 transFilterDlg_->bySplitCategory(pBankTransaction.get()) &&
                 transFilterDlg_->byCategory(pBankTransaction->catStr_, pBankTransaction->subCatStr_) &&
                 transFilterDlg_->byStatus(pBankTransaction->status_)  &&
                 transFilterDlg_->byType(pBankTransaction->transType_) &&
                 transFilterDlg_->byTransNumber(pBankTransaction->transNum_) &&
                 transFilterDlg_->byNotes(pBankTransaction->notes_)
               )
            {
                toAdd  = true;
            }
        }

        if (toAdd)
        {
            m_trans.push_back(pBankTransaction.get());
            ++numTransactions;
    
            double transBal = 0.0;
            transBal = getBalance( pBankTransaction.get(), transBal);
            filteredBalance_ += transBal;
        }
    }

    /**********************************************************************************
     Stage 2
     Sort all account transactions by date to, determine balances.
    **********************************************************************************/
    if (pgd)
    pgd->Update(40);

    std::sort(v_transPtr.begin(), v_transPtr.end(), sortTransByDateAsc);

    /**********************************************************************************
     Stage 3
     Add the account balances to all the transactions in this account.
    **********************************************************************************/
    if (pgd)
    pgd->Update(60);

    double initBalance = pAccount->initialBalance_;
    for (size_t i = 0; i < v_transPtr.size(); ++i)
    {
        bool ok = v_transPtr[i] != 0;
        wxASSERT(ok);

        if (!ok) continue;

        mmBankTransaction* transPtr = v_transPtr[i];
        initBalance = getBalance( transPtr, initBalance);
        setBalance( transPtr, initBalance);
    }

    /**********************************************************************************
     Stage 4
     Sort the list of visible transactions dependant on user preferences.
    **********************************************************************************/
    if (pgd)
    pgd->Update(80);

    // decide whether top or down icon needs to be shown
    m_listCtrlAccount->setColumnImage(g_sortcol, g_asc ? ICON_ASC : ICON_DESC);
    
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

    setAccountSummary();

    if (pgd)
    {
        pgd->Update(100);
        pgd->Destroy();
    }
}
//----------------------------------------------------------------------------

double mmCheckingPanel::getBalance(mmBankTransaction* transPtr, double currentBalance) const
{
    if (transPtr->status_ != wxT("V"))
        currentBalance += transPtr->value(m_AccountID);

    return currentBalance;
}
//----------------------------------------------------------------------------

void mmCheckingPanel::setBalance(mmBankTransaction* transPtr, double currentBalance )
{
    transPtr->balance_ = currentBalance;
    wxString balanceStr;
    mmex::formatDoubleToCurrencyEdit(currentBalance, balanceStr);
    transPtr->balanceStr_ = balanceStr;
}

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

void mmCheckingPanel::OnMoveTransaction(wxCommandEvent& event)
{
    m_listCtrlAccount->OnMoveTransaction(event);
}

//----------------------------------------------------------------------------

void mmCheckingPanel::initViewTransactionsHeader()
{
    if (m_currentView == VIEW_TRANS_ALL_STR) {
        itemStaticTextMainFilter_->SetLabel(_("Viewing all transactions"));
        itemStaticBitmap31_->Enable(true);
        statTextTransFilter_->Enable(true);
    } else {
        if (m_currentView == VIEW_TRANS_RECONCILED_STR)   itemStaticTextMainFilter_->SetLabel(_("Viewing Reconciled transactions"));
        else if (m_currentView == VIEW_TRANS_NOT_RECONCILED_STR) itemStaticTextMainFilter_->SetLabel(_("Viewing All Except Reconciled Transactions"));
        else if (m_currentView == VIEW_TRANS_UNRECONCILED_STR) itemStaticTextMainFilter_->SetLabel(_("Viewing Un-Reconciled transactions"));
        else if (m_currentView == VIEW_TRANS_TODAY_STR)        itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for today"));
        else if (m_currentView == VIEW_TRANS_CURRENT_MONTH_STR) itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for current month"));
        else if (m_currentView == VIEW_TRANS_LAST_30_DAYS_STR) itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last 30 days"));
        else if (m_currentView == VIEW_TRANS_LAST_90_DAYS_STR) itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last 90 days"));
        else if (m_currentView == VIEW_TRANS_LAST_MONTH_STR)   itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last month"));
        else if (m_currentView == VIEW_TRANS_LAST_3MONTHS_STR) itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last 3 months"));
        itemStaticBitmap31_->Enable(false);
        statTextTransFilter_->Enable(false);
    }
}
//----------------------------------------------------------------------------

void mmCheckingPanel::OnViewPopupSelected(wxCommandEvent& event)
{
    int evt =  event.GetId();
    
    if (  (evt != MENU_VIEW_ALLTRANSACTIONS 
        && evt != MENU_VIEW_DELETE_TRANS 
        && evt != MENU_VIEW_DELETE_FLAGGED 
        && evt != MENU_TREEPOPUP_DELETE_VIEWED 
        && evt != MENU_TREEPOPUP_DELETE_FLAGGED) && transFilterActive_)
    {
        wxString messageStr;
        messageStr << _("Transaction Filter")<< _("  will interfere with this filtering.") << wxT("\n\n");
        messageStr << _("Please deactivate: ") << _("Transaction Filter");
        wxMessageBox(messageStr,_("Transaction Filter"),wxICON_WARNING);
        return;
    } 

    bool show_filter_ = false;

    if (evt ==  MENU_VIEW_ALLTRANSACTIONS)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing all transactions"));
        m_currentView = VIEW_TRANS_ALL_STR;
        transFilterActive_ = false;
        show_filter_ = true;
    }
    else if (evt == MENU_VIEW_RECONCILED)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing Reconciled transactions"));
        m_currentView = VIEW_TRANS_RECONCILED_STR;
    }
    else if (evt == MENU_VIEW_NOTRECONCILED)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing All except Reconciled Transactions"));
        m_currentView = VIEW_TRANS_NOT_RECONCILED_STR;
    }
    else if (evt == MENU_VIEW_UNRECONCILED)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing Un-Reconciled transactions"));
        m_currentView = VIEW_TRANS_UNRECONCILED_STR;
    }
    else if (evt == MENU_VIEW_FLAGGED)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing Follow-Up transactions"));
        m_currentView = VIEW_TRANS_FLAGGED;
    }
    else if (evt == MENU_VIEW_DUPLICATE)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing duplicate transactions"));
        m_currentView = VIEW_TRANS_DUPLICATES;
    }
    else if (evt == MENU_VIEW_VOID)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing Void transactions"));
        m_currentView = VIEW_TRANS_VOID;
    }
    else if (evt == MENU_VIEW_TODAY)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for today"));
        m_currentView = VIEW_TRANS_TODAY_STR;
    }
    else if (evt == MENU_VIEW_CURRENTMONTH)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for current month"));
        m_currentView = VIEW_TRANS_CURRENT_MONTH_STR;
    }
    else if (evt == MENU_VIEW_LAST30)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last 30 days"));
        m_currentView = VIEW_TRANS_LAST_30_DAYS_STR;
    }
    else if (evt == MENU_VIEW_LAST90)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last 90 days"));
        m_currentView = VIEW_TRANS_LAST_90_DAYS_STR;
    }
    else if (evt == MENU_VIEW_LAST3MONTHS)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last 3 months"));
        m_currentView = VIEW_TRANS_LAST_3MONTHS_STR;
    }
    else if (evt == MENU_VIEW_LASTMONTH)
    {
        itemStaticTextMainFilter_->SetLabel(_("Viewing transactions for last month"));
        m_currentView = VIEW_TRANS_LAST_MONTH_STR;
    }
    else if (evt == MENU_VIEW_DELETE_TRANS || evt == MENU_TREEPOPUP_DELETE_VIEWED)
    {
        DeleteViewedTransactions();
        show_filter_ = true;
    }
    else if (evt == MENU_VIEW_DELETE_FLAGGED || evt == MENU_TREEPOPUP_DELETE_FLAGGED)
    {
        DeleteFlaggedTransactions();
        show_filter_ = true;    }
    else
    {
        wxASSERT(false);
    }

    itemStaticBitmap31_->Enable(show_filter_);
    statTextTransFilter_->Enable(show_filter_);
    
    m_listCtrlAccount->DeleteAllItems();

    initVirtualListControl(NULL);
    m_listCtrlAccount->RefreshItems(0, static_cast<long>(m_trans.size()) - 1);
}

void mmCheckingPanel::DeleteViewedTransactions()
{
    wxMessageDialog msgDlg(this,_("Do you really want to delete all the transactions shown?"),
                                _("Confirm Transaction Deletion"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        core_->db_.get()->Begin();
        for (size_t i = 0; i < m_trans.size(); ++i)
        {
            core_->bTransactionList_.deleteTransaction(m_AccountID, m_trans[i]->transactionID());
        }
        core_->db_.get()->Commit();
    }
}

void mmCheckingPanel::DeleteFlaggedTransactions()
{
    wxMessageDialog msgDlg(this,_("Do you really want to delete all the \"Follow Up\" transactions?"),
                                _("Confirm Transaction Deletion"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        core_->db_.get()->Begin();
        for (size_t i = 0; i < m_trans.size(); ++i)
        {
            if (m_trans[i]->status_ == wxT("F"))
            {
                core_->bTransactionList_.deleteTransaction(m_AccountID, m_trans[i]->transactionID());
            }
        }
        core_->db_.get()->Commit();
    }
}

void mmCheckingPanel::OnFilterTransactions(wxMouseEvent& event)
{
    if (m_currentView != VIEW_TRANS_ALL_STR) 
    {
        event.Skip();
        return;
    }
    
    int e = event.GetEventType();

    wxBitmap bitmapFilterIcon(rightarrow_xpm);

    if (e == wxEVT_LEFT_DOWN) {
        int dlgResult = transFilterDlg_->ShowModal();
        if ( dlgResult == wxID_OK )
        {
            transFilterActive_ = true; 
            wxBitmap activeBitmapFilterIcon(tipicon_xpm); 
            bitmapFilterIcon = activeBitmapFilterIcon;
        }
        else if ( dlgResult == wxID_CANCEL )
        {
            transFilterActive_ = false;
        }
    } else {
        if (transFilterActive_ == false) return;
        transFilterActive_ = false;
    }
    
    wxImage pic = bitmapFilterIcon.ConvertToImage();
    itemStaticBitmap31_->SetBitmap(pic);
    itemStaticBitmap31_->Enable(true);
    statTextTransFilter_->Enable(true);

    m_listCtrlAccount->DeleteAllItems();
    initVirtualListControl(NULL);
    m_listCtrlAccount->RefreshItems(0, static_cast<long>(m_trans.size()) - 1); 

}

void mmCheckingPanel::OnFilterResetToViewAll(wxMouseEvent& event) {

    if (m_currentView == VIEW_TRANS_ALL_STR) 
    {
        event.Skip();
        return;
    }
    
    itemStaticTextMainFilter_->SetLabel(_("Viewing all transactions"));
    m_currentView = VIEW_TRANS_ALL_STR;
    itemStaticBitmap31_->Enable(true);
    statTextTransFilter_->Enable(true);
    
    m_listCtrlAccount->DeleteAllItems();
    initVirtualListControl(NULL);
    m_listCtrlAccount->RefreshItems(0, static_cast<long>(m_trans.size()) - 1); 
    
}

//----------------------------------------------------------------------------

void TransactionListCtrl::OnListItemSelected(wxListEvent& event)
{
    m_selectedIndex = event.GetIndex();
    m_cp->updateExtraTransactionData(m_selectedIndex);
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
    m_selectedIndex = -1;
    m_cp->updateExtraTransactionData(m_selectedIndex);
}

void TransactionListCtrl::OnItemRightClick(wxListEvent& event)
{
    m_selectedIndex = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Transaction"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Transaction"));
    menu.Append(MENU_ON_COPY_TRANSACTION, _("&Copy Transaction"));
    menu.Append(MENU_TREEPOPUP_MOVE, _("&Move Transaction"));
    if (m_selectedForCopy != -1) menu.Append(MENU_ON_PASTE_TRANSACTION, _("&Paste Transaction"));

    if (m_selectedIndex != -1)
    {
        if (m_cp->m_trans[m_selectedIndex]->categID_ < 0)
        {
            menu.AppendSeparator();
            menu.Append(MENU_TREEPOPUP_VIEW_SPLIT_CATEGORIES, _("&View Split Categories"));
        }
    }
    menu.AppendSeparator();

    wxMenu* subGlobalOpMenuDelete = new wxMenu;
    subGlobalOpMenuDelete->Append(MENU_TREEPOPUP_DELETE, _("&Delete Transaction"));
    subGlobalOpMenuDelete->AppendSeparator();
    subGlobalOpMenuDelete->Append(MENU_TREEPOPUP_DELETE_VIEWED, _("Delete all transactions in current view"));
    subGlobalOpMenuDelete->Append(MENU_TREEPOPUP_DELETE_FLAGGED, _("Delete all \"Follow Up\" transactions"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete"), subGlobalOpMenuDelete);
    
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
    menu.Append(MENU_SUBMENU_MARK_ALL, _("Mark all being viewed"), subGlobalOpMenu);

    PopupMenu(&menu, event.GetPoint());
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnMarkTransactionDB(const wxString& status)
{
    if (m_selectedIndex == -1) return;

    int transID = m_cp->m_trans[m_selectedIndex]->transactionID();
    if (mmDBWrapper::updateTransactionWithStatus(*m_cp->getDb(), transID, status))
    m_cp->m_trans[m_selectedIndex]->status_ = status;

    // Remake the register. If user was viewing some transactions (eg void)
    //  any changes need to be reflected.  Even if we are viewing all transactions,
    //  the register needs to be updated so the balance col is correct (eg a trans
    //  was changed from unreconciled to void).
    DeleteAllItems();

    m_cp->initVirtualListControl(NULL);
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnMarkTransaction(wxCommandEvent& event)
{
    int evt = event.GetId();
    wxString status = wxT("");
    if (evt ==  MENU_TREEPOPUP_MARKRECONCILED)         status = wxT("R");
    else if (evt == MENU_TREEPOPUP_MARKUNRECONCILED)   status = wxT("");
    else if (evt == MENU_TREEPOPUP_MARKVOID)           status = wxT("V");
    else if (evt == MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP) status = wxT("F");
    else if (evt == MENU_TREEPOPUP_MARKDUPLICATE)         status = wxT("D");
    else wxASSERT(false);
      
    OnMarkTransactionDB(status);
    SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
    SetItemState(m_selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnMarkAllTransactions(wxCommandEvent& event)
{
    int evt =  event.GetId();
    wxString status = wxT("");
    if (evt ==  MENU_TREEPOPUP_MARKRECONCILED_ALL)             status = wxT("R");
    else if (evt == MENU_TREEPOPUP_MARKUNRECONCILED_ALL)       status = wxT("");
    else if (evt == MENU_TREEPOPUP_MARKVOID_ALL)               status = wxT("V");
    else if (evt == MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL) status = wxT("F");
    else if (evt == MENU_TREEPOPUP_MARKDUPLICATE_ALL)          status = wxT("D");
    else  wxASSERT(false);

    m_cp->core_->db_.get()->Begin();

    for (size_t i = 0; i < m_cp->m_trans.size(); ++i)
    {
        int transID = m_cp->m_trans[i]->transactionID();
        if (mmDBWrapper::updateTransactionWithStatus(*m_cp->getDb(), transID, status))
        m_cp->m_trans[i]->status_ = status;
    }

//     if (m_cp->m_currentView != VIEW_TRANS_ALL_STR)
    {
        DeleteAllItems();

        m_cp->initVirtualListControl(NULL);
        RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1); // refresh everything
    }

    m_cp->core_->db_.get()->Commit();

    m_cp->setAccountSummary();
    SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
    SetItemState(m_selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnColClick(wxListEvent& event)
{
    /* Clear previous column image */
    setColumnImage(m_sortCol, -1);

    m_sortCol = toEColumn(event.GetColumn());
    g_sortcol = m_sortCol;

    m_asc = !m_asc; // toggle sort order
    g_asc = m_asc;

    setColumnImage(m_sortCol, m_asc ? ICON_ASC : ICON_DESC);

    m_cp->sortTable();
    RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1); // refresh everything
}
//----------------------------------------------------------------------------

void TransactionListCtrl::setColumnImage(EColumn col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);

    SetColumn(col, item);
}
//----------------------------------------------------------------------------

wxString mmCheckingPanel::getItem(long item, long column) const
{  
    wxString s;

    bool ok = !m_trans.empty() &&
              ( item >= 0 ) &&
              ( item < static_cast<long>(m_trans.size()) ) &&
              m_trans[item];

    if (ok)
    {
        const mmBankTransaction &t = *m_trans[item];

        switch (column)
        {
        case COL_DATE_OR_TRANSACTION_ID:
            s = t.dateStr_;
            break;

        case COL_TRANSACTION_NUMBER:
            s = t.transNum_;
            break;

        case COL_PAYEE_STR:
            s = t.payeeStr_;
            break;

        case COL_STATUS:
            s = t.status_;
            break;

        case COL_CATEGORY:
            s = t.fullCatStr_;
            break;

        case COL_WITHDRAWAL:
            s = t.withdrawalStr_;
            break;

        case COL_DEPOSIT:
            s = t.depositStr_; 
            break;

        case COL_BALANCE:
            s = t.balanceStr_;
            break;

        case COL_NOTES:
            s = t.notes_;
            break;

        default:
            wxASSERT(false);
        }
    }

    return s;
}
//----------------------------------------------------------------------------

wxString TransactionListCtrl::OnGetItemText(long item, long column) const
{
    return m_cp->getItem(item, column);
}
//----------------------------------------------------------------------------

/* 
    Returns the icon to be shown for each transaction for the required column
*/
int TransactionListCtrl::OnGetItemColumnImage(long item, long column) const
{
    int res = -1;
    if(column == COL_DATE_OR_TRANSACTION_ID)
    {
        res = ICON_NONE;
        wxString status = m_cp->getItem(item, COL_STATUS);
        if ( status == wxT("F")) 
            res = ICON_FOLLOWUP;
        else if (status == wxT("R"))
            res = ICON_RECONCILED;
        else if (status == wxT("V"))
            res = ICON_VOID;
        else if (status == wxT("D"))
            res = ICON_DUPLICATE;
    }

    if(column == COL_PAYEE_STR)
    {
        size_t index = item;
        bool ok = m_cp && index < m_cp->m_trans.size();
        mmBankTransaction *tr = ok ? m_cp->m_trans[index] : 0;
        
        if (tr->transType_ == TRANS_TYPE_TRANSFER_STR) 
        {
            if ( tr->accountID_ == m_cp->accountID() )
                res = ICON_TRANS_WITHDRAWAL;
            else
                res = ICON_TRANS_DEPOSIT;
        }
    }

    return res;
}
//----------------------------------------------------------------------------

/*
    Failed wxASSERT will hang application if active modal dialog presents on screen.
    Assertion's message box will be hidden until you press tab to activate one.
*/
wxListItemAttr* TransactionListCtrl::OnGetItemAttr(long item) const
{
    wxASSERT(m_cp);
    wxASSERT(item >= 0);

    size_t idx = item;
    bool ok = m_cp && idx < m_cp->m_trans.size();
    
    mmBankTransaction *tr = ok ? m_cp->m_trans[idx] : 0;
    bool in_the_future = tr && tr->date_ > wxDateTime::Now();

    TransactionListCtrl &self = *const_cast<TransactionListCtrl*>(this);

    if (in_the_future) // apply alternating background pattern
    {
        return item % 2 ? &self.m_attr3 : &self.m_attr4;
    }
  
    return item % 2 ? &self.m_attr1 : &self.m_attr2;

}
//----------------------------------------------------------------------------
// If any of these keys are encountered, the search for the event handler
// should continue as these keys may be processed by the operating system.
void TransactionListCtrl::OnChar(wxKeyEvent& event)
{   
    if (wxGetKeyState(WXK_ALT) || 
        wxGetKeyState(WXK_COMMAND) ||
        wxGetKeyState(WXK_UP) || 
        wxGetKeyState(WXK_DOWN) || 
        wxGetKeyState(WXK_LEFT) || 
        wxGetKeyState(WXK_RIGHT) || 
        wxGetKeyState(WXK_HOME) || 
        wxGetKeyState(WXK_END) ||
        wxGetKeyState(WXK_PAGEUP) || 
        wxGetKeyState(WXK_PAGEDOWN) || 
        wxGetKeyState(WXK_NUMPAD_UP) ||
        wxGetKeyState(WXK_NUMPAD_DOWN) ||
        wxGetKeyState(WXK_NUMPAD_LEFT) ||
        wxGetKeyState(WXK_NUMPAD_RIGHT) ||
        wxGetKeyState(WXK_NUMPAD_PAGEDOWN) ||
        wxGetKeyState(WXK_NUMPAD_PAGEUP) ||
        wxGetKeyState(WXK_NUMPAD_HOME) ||
        wxGetKeyState(WXK_NUMPAD_END) ||
        wxGetKeyState(WXK_DELETE) ||
        wxGetKeyState(WXK_NUMPAD_DELETE) ||
        wxGetKeyState(WXK_TAB)||
        wxGetKeyState(WXK_RETURN)||
        wxGetKeyState(WXK_NUMPAD_ENTER)||
        wxGetKeyState(WXK_SPACE)||
        wxGetKeyState(WXK_NUMPAD_SPACE)
        )
    {
        event.Skip();
    }
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    if (m_selectedIndex != -1)
        m_selectedForCopy = m_cp->m_trans[m_selectedIndex]->transactionID();
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    if (m_selectedForCopy != -1)
    {
        wxString useOriginalDate =  mmDBWrapper::getINISettingValue(m_cp->inidb_, INIDB_USE_ORG_DATE_COPYPASTE, wxT("FALSE"));
        bool useOriginal = false;
        if (useOriginalDate == wxT("TRUE"))
            useOriginal = true;
        boost::shared_ptr<mmBankTransaction> pCopiedTrans = m_cp->core_->bTransactionList_.copyTransaction(m_selectedForCopy, useOriginal);
        boost::shared_ptr<mmCurrency> pCurrencyPtr = m_cp->core_->getCurrencyWeakPtr(pCopiedTrans->accountID_).lock();
        pCopiedTrans->updateAllData(m_cp->core_, pCopiedTrans->accountID_, pCurrencyPtr, true);
        m_cp->initVirtualListControl(NULL);
        RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
    }
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnListKeyDown(wxListEvent& event)
{
    if (m_selectedIndex == -1) //check if a transaction is selected
        return;

    // Exit if list has been reduced and selected index no longer exists.
    //int currentListSize = m_cp->m_trans.size();
    //if (currentListSize == m_selectedIndex)
    //    return;

    //find the topmost visible item - this will be used to set
    // where to display the list again after refresh
    long topItemIndex = GetTopItem();

    //Read status of the selected transaction first
    wxString status = m_cp->m_trans[m_selectedIndex]->status_;

    if (!wxGetKeyState(WXK_COMMAND) && !wxGetKeyState(WXK_ALT) && !wxGetKeyState(WXK_CONTROL))
    {
        if (wxGetKeyState(wxKeyCode('V')) && status != wxT("V")) 
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKVOID);
            OnMarkTransaction(evt);  
        }
        else if (wxGetKeyState(wxKeyCode('R')) && status != wxT("R")) 
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKRECONCILED);
            OnMarkTransaction(evt); 
        }
        else if (wxGetKeyState(wxKeyCode('U')) && status != wxT("")) 
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKUNRECONCILED);
            OnMarkTransaction(evt); 
        } 
        else if (wxGetKeyState(wxKeyCode('F')) && status != wxT("F")) 
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP);
            OnMarkTransaction(evt); 
        }
        else if (wxGetKeyState(wxKeyCode('D')) && status != wxT("D")) 
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_MARKDUPLICATE);
            OnMarkTransaction(evt); 
        }
        
        else if (wxGetKeyState(WXK_DELETE) || wxGetKeyState(WXK_NUMPAD_DELETE))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_DELETE);
            OnDeleteTransaction(evt);
        }
    } 
    else 
    {
        event.Skip();
    }

    if ((long)m_cp->m_trans.size() == m_selectedIndex)
        m_selectedIndex--;

    m_cp->updateExtraTransactionData(m_selectedIndex);

    //set the deleted transaction index to the new selection and focus on it
    SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED);
    //make sure the topmost item before transaction deletion is visible, otherwise
    // the control will go back to the very top or bottom when refreshed
    EnsureVisible(topItemIndex);
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnNewTransaction(wxCommandEvent& /*event*/)
{
    mmTransDialog dlg(m_cp->getDb(), m_cp->core_, m_cp->accountID(), 0, false, m_cp->inidb_, this );

    if ( dlg.ShowModal() == wxID_OK )
    {
        m_cp->initVirtualListControl(NULL);
        RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
        if (m_selectedIndex > -1)
        {
            m_cp->updateExtraTransactionData(m_cp->m_trans.size()-1);
            EnsureVisible(m_cp->m_trans.size()-1);           
            SetItemState(m_cp->m_trans.size()-1, wxLIST_STATE_SELECTED|wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED|wxLIST_STATE_SELECTED);
            EnsureVisible(m_selectedIndex);
            m_cp->updateExtraTransactionData(m_cp->m_trans.size()-1);
        }
    }
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnDeleteTransaction(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex == -1) //check if a transaction is selected
        return;

    //ask if they really want to delete
    wxMessageDialog msgDlg(this,_("Do you really want to delete the selected transaction?"),
                                _("Confirm Transaction Deletion"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
                          
    if (msgDlg.ShowModal() != wxID_YES)
        return;
    //find the topmost visible item - this will be used to set 
    // where to display the list again after refresh
    long topItemIndex = GetTopItem();

    //remove the transaction
    //TODO: if deletingfromdb was false do not delete trx in that case from list 
    m_cp->core_->bTransactionList_.deleteTransaction(m_cp->accountID(), m_cp->m_trans[m_selectedIndex]->transactionID());
    {
        //initialize the transaction list to redo balances and images
        m_cp->initVirtualListControl(NULL);
    
        if (!m_cp->m_trans.empty()) 
        {
            //refresh the items showing from the point of the transaction delete down
            //the transactions above the deleted transaction won't change so they 
            // don't need to be refreshed
            if (m_selectedIndex == (long)(m_cp->m_trans.size() - 1) && m_selectedIndex > 0)
                m_selectedIndex--;
            RefreshItems(m_selectedIndex, static_cast<long>(m_cp->m_trans.size()) - 1);

            //set the deleted transaction index to the new selection and focus on it
            SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED, 
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
            m_cp->updateExtraTransactionData(m_selectedIndex);
        }
        //Update info panels
        //m_cp->updateExtraTransactionData(m_selectedIndex);
    }
}
//----------------------------------------------------------------------------

void TransactionListCtrl::OnEditTransaction(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex != -1)
    {
        mmTransDialog dlg(m_cp->getDb(), m_cp->core_, m_cp->accountID(), 
           m_cp->m_trans[m_selectedIndex]->transactionID(), true, m_cp->inidb_, this);
        if ( dlg.ShowModal() == wxID_OK )
        {
            refreshVisualList();
        }
    }
}
//----------------------------------------------------------------------------

void TransactionListCtrl::refreshVisualList()
{
    m_cp->initVirtualListControl(NULL);
    RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
    if ((m_selectedIndex + 1) != (long)m_cp->m_trans.size() && (m_selectedIndex > -1))
        m_selectedIndex--;
    EnsureVisible(m_selectedIndex);
    m_cp->updateExtraTransactionData(m_selectedIndex);
    SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED, wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED);
}

//  Called only when moving a deposit/withdraw transaction to a new account.
int TransactionListCtrl::destinationAccountID(wxString accName)
{
    wxArrayString as = m_cp->core_->getAccountsName(m_cp->accountID());

    wxString headerMsg = _("Moving Transaction from ") + accName + _(" to...");
    wxSingleChoiceDialog scd(0, _("Select the destination Account "), headerMsg , as);

    int accountID = -1;
    if (scd.ShowModal() == wxID_OK)
    {
        wxString acctName = scd.GetStringSelection();
        accountID = m_cp->core_->getAccountID(acctName);
    }

    return accountID;
}

void TransactionListCtrl::OnMoveTransaction(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex != -1)
    {
        if ( m_cp->m_trans[m_selectedIndex]->transType_ == TRANS_TYPE_TRANSFER_STR )
        {
            mmTransDialog dlg(m_cp->getDb(), m_cp->core_, m_cp->accountID(), 
                              m_cp->m_trans[m_selectedIndex]->transactionID(), true, 
                              m_cp->inidb_, this);
            if ( dlg.ShowModal() == wxID_OK )
            {
                //m_selectedIndex --;
                refreshVisualList();
            }
        } 
        else
        {
            int toAccountID = destinationAccountID(m_cp->m_trans[m_selectedIndex]->fromAccountStr_);
            if ( toAccountID != -1 )
            {
                boost::shared_ptr<mmBankTransaction> pTransaction;
                pTransaction = m_cp->core_->bTransactionList_.getBankTransactionPtr(m_cp->accountID(),
                                      m_cp->m_trans[m_selectedIndex]->transactionID() );
                
                pTransaction->accountID_ = toAccountID;
                m_cp->core_->bTransactionList_.updateTransaction(pTransaction);
                //m_selectedIndex --;
                refreshVisualList();
            }
        }
    }
}

//----------------------------------------------------------------------------
void TransactionListCtrl::OnViewSplitTransaction(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex != -1)
    {
        if (m_cp->m_trans[m_selectedIndex]->categID_ < 0)
            m_cp->DisplaySplitCategories(m_cp->m_trans[m_selectedIndex]->transactionID());
    }
}

//----------------------------------------------------------------------------
void TransactionListCtrl::OnListItemActivated(wxListEvent& /*event*/)
{
    if (m_selectedIndex != -1)
    {
        //m_selectedIndex = event.GetIndex();
        mmTransDialog dlg(m_cp->getDb(), m_cp->core_,  m_cp->accountID(), 
            m_cp->m_trans[m_selectedIndex]->transactionID(), true, m_cp->inidb_, this);
        if ( dlg.ShowModal() == wxID_OK )
        {
            m_cp->initVirtualListControl(NULL);
            RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
            if (m_selectedIndex != -1)
            {
                SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
                SetItemState(m_selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                EnsureVisible(m_selectedIndex);
                //Notes and other info should be updated 
                m_cp->updateExtraTransactionData(m_selectedIndex);
            }
        }
    }
}
//----------------------------------------------------------------------------

TransactionListCtrl::TransactionListCtrl(
    mmCheckingPanel *cp, 
    wxWindow *parent,
    const wxWindowID id, 
    const wxPoint& pos,
    const wxSize& size, 
    long style
) : 
    wxListCtrl(parent, id, pos, size, style | wxWANTS_CHARS),
    m_cp(cp),
    m_selectedIndex(-1),
    m_selectedForCopy(-1),
    m_attr1(*wxBLACK, mmColors::listAlternativeColor0, wxNullFont),
    m_attr2(*wxBLACK, mmColors::listAlternativeColor1, wxNullFont),
    m_attr3(mmColors::listFutureDateColor, mmColors::listAlternativeColor0, wxNullFont),
    m_attr4(mmColors::listFutureDateColor, mmColors::listAlternativeColor1, wxNullFont),
    m_sortCol(COL_DEF_SORT),
    m_asc(true)
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
    wxASSERT(core_);
    return core_->db_; 
}

//----------------------------------------------------------------------------
void mmCheckingPanel::OnSearchTxtEntered(wxCommandEvent& /*event*/)
{
    //event.GetString() does not working. It seems wxWidgets issue
    //wxString searchString = event.GetString().c_str();

    wxSearchCtrl* st = (wxSearchCtrl*)FindWindow(wxID_FIND);
    wxString searchString = st->GetValue().Lower();

    if (!searchString.IsEmpty())
    {
        int selectedItem = m_listCtrlAccount->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)-1;
        if (selectedItem < 0)
        selectedItem = m_listCtrlAccount->GetItemCount()-1;
        
        wxString t;

        for (int i = selectedItem; i >= 0; --i)
        {
            t=getItem(i, COL_NOTES);
            if (t.Lower().Find(searchString)!=wxNOT_FOUND)
            {
                
                //First of all any items should be unselected
                int cursel = m_listCtrlAccount->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                if (cursel != wxNOT_FOUND)
                    m_listCtrlAccount->SetItemState(cursel, !wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                //Then finded item will be selected
                m_listCtrlAccount->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                m_listCtrlAccount->EnsureVisible(i);
                break;
            }
        }
    }
}

void mmCheckingPanel::DisplaySplitCategories(int transID)
{
    wxString transTypeStr = core_->bTransactionList_.getBankTransactionPtr(transID)->transType_;
    int transType = 0;
    if (transTypeStr== TRANS_TYPE_DEPOSIT_STR)  transType = 1;
    if (transTypeStr== TRANS_TYPE_TRANSFER_STR) transType = 2;

    SplitTransactionDialog splitTransDialog(
        core_,
        core_->bTransactionList_.getBankTransactionPtr(transID)->splitEntries_.get(),
        transType,
        this
    );
    splitTransDialog.SetDisplaySplitCategories();
    splitTransDialog.ShowModal();
}

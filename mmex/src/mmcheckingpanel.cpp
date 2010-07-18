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
#include <boost/unordered_map.hpp>

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

enum EColumn
{ 
    COL_DATE_OR_TRANSACTION_ID, 
    COL_TRANSACTION_NUMBER, 
    COL_PAYEE_STR, 
    COL_STATUS, 
    COL_CATEGORY, 
    COL_WITHDRAWAL, 
    COL_DEPOSIT,
    COL_BALANSE,
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
    ICON_DUPLICATE
};
//----------------------------------------------------------------------------
EColumn g_sortcol = COL_DEF_SORT; // index of column to sort
bool g_asc = true; // asc\desc sorting
//----------------------------------------------------------------------------

EColumn toEColumn(long col)
{
    EColumn res = COL_DEF_SORT;

    if (col >= 0 && col < COL_MAX) {
        res = static_cast<EColumn>(col);
    }

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
    lst.InsertColumn(COL_STATUS, _("C  "));
    lst.InsertColumn(COL_CATEGORY, _("Category"));
    lst.InsertColumn(COL_WITHDRAWAL, _("Withdrawal"), wxLIST_FORMAT_RIGHT);
    lst.InsertColumn(COL_DEPOSIT, _("Deposit"), wxLIST_FORMAT_RIGHT);
    lst.InsertColumn(COL_BALANSE, _("Balance"), wxLIST_FORMAT_RIGHT);
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
        
        if (val.ToLong(&width)) {
            lst.SetColumnWidth(i, width);    
        }
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

/*
    sort_fun_t.
*/
bool sortTransByDate(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    bool res = false;

    if (t1->date_ == t2->date_) {
        res = sort(t1->transactionID(), t2->transactionID(), asc);
    } else {
        res = sort(t1->date_, t2->date_, asc);
    }

    return res;
}
//----------------------------------------------------------------------------

/*
    This function is not sort_fun_t.
*/
bool sortTransByDateAsc(const mmBankTransaction *t1, const mmBankTransaction *t2)
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

/*
    sort_fun_t.
*/
bool sortTransByNum(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    long v1 = 0;
    long v2 = 0;

    bool ok1 = t1->transNum_.ToLong(&v1);
    bool ok2 = t2->transNum_.ToLong(&v2);

    bool res = false;

    if (ok1 && ok2) {
        res = sort(v1, v2, asc);
    } else {
        res = sort(t1->transNum_, t2->transNum_, asc);
    }

    return res;
}
//----------------------------------------------------------------------------

/*
    sort_fun_t.
*/
bool sortTransByPayee(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sort(t1->payeeStr_, t2->payeeStr_, asc);
}
//----------------------------------------------------------------------------

/*
    sort_fun_t.
*/
bool sortTransByStatus(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sort(t1->status_, t2->status_, asc);
}
//----------------------------------------------------------------------------

/*
    sort_fun_t.
*/
bool sortTransByCateg(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sort(t1->fullCatStr_, t2->fullCatStr_, asc);
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

    if (ok1 && ok2) {
        res = sort(v1, v2, asc);
    } else {
        res = sort(s1, s2, asc);
    }

    return res;
}
//----------------------------------------------------------------------------

/*
    sort_fun_t.
*/
bool sortTransByWithdrowal(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sortAsCurrency(t1->withdrawalStr_, t2->withdrawalStr_, asc);
}
//----------------------------------------------------------------------------

/*
    sort_fun_t.
*/
bool sortTransByDeposit(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sortAsCurrency(t1->depositStr_, t2->depositStr_, asc);
}
//----------------------------------------------------------------------------

/*
    sort_fun_t.
*/
bool sortTransByBalanse(const mmBankTransaction *t1, const mmBankTransaction *t2, bool asc)
{
    return sort(t1->balance_, t2->balance_, asc);
}
//----------------------------------------------------------------------------

/*
    sort_fun_t.
*/
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
        fx[COL_BALANSE] = sortTransByBalanse;
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

} // namespace

//----------------------------------------------------------------------------

/* 
    Custom ListCtrl class that implements virtual LC style 
*/
class MyListCtrl : public wxListCtrl
{
public:
    MyListCtrl(mmCheckingPanel *cp, wxWindow *parent,const wxWindowID id, const wxPoint& pos,const wxSize& size, long style);

    bool getSortOrder() const { return m_asc; }
    EColumn getSortColumn() const { return m_sortCol; }

    void setSortOrder(bool asc) { m_asc = asc; }
    void setSortColumn(EColumn col) { m_sortCol = col; }

    void setColumnImage(EColumn col, int image);

    void OnNewTransaction(wxCommandEvent& event);
    void OnDeleteTransaction(wxCommandEvent& event);
    void OnEditTransaction(wxCommandEvent& event);

private:
    DECLARE_NO_COPY_CLASS(MyListCtrl)
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
    wxString OnGetItemText(long item, long column) const;
    int OnGetItemImage(long item) const;
    wxListItemAttr *OnGetItemAttr(long item) const;

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
    EVT_LIST_ITEM_DESELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, MyListCtrl::OnListItemDeselected)
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

bool mmCheckingPanel::Create(
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

        mmDBWrapper::setINISettingValue(m_inidb, name, wxString() << width); 
    }

    // sorting column index
    mmDBWrapper::setINISettingValue(m_inidb, wxT("CHECK_SORT_COL"), wxString() << g_sortcol); 

    // asc\desc sorting flag
    mmDBWrapper::setINISettingValue(m_inidb, wxT("CHECK_ASC"), wxString() << g_asc);
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
    wxPanel* headerPanel = new wxPanel( itemPanel8, wxID_ANY, wxDefaultPosition, 
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
    m_listCtrlAccount->setSortOrder(g_asc);
    m_listCtrlAccount->setSortColumn(g_sortcol);
    m_listCtrlAccount->SetFocus();
    
    createColumns(m_inidb, *m_listCtrlAccount);

    {   // load the global variables
        long val = COL_DEF_SORT;
        wxString strVal = mmDBWrapper::getINISettingValue(m_inidb, wxT("CHECK_SORT_COL"), wxString() << val);
        
        if (strVal.ToLong(&val)) {
            g_sortcol = toEColumn(val);
        }

        // --

        val = 1; // asc sorting default
        strVal = mmDBWrapper::getINISettingValue(m_inidb, wxT("CHECK_ASC"), wxString() << val);

        if (strVal.ToLong(&val)) {
            g_asc = val != 0;
        }

        // --

        m_listCtrlAccount->setSortColumn(g_sortcol);
        m_listCtrlAccount->setSortOrder(g_asc);
        m_listCtrlAccount->setColumnImage(m_listCtrlAccount->getSortColumn(), m_listCtrlAccount->getSortOrder() ? ICON_ASC : ICON_DESC); // asc\desc sort mark (arrow)
    }

    wxPanel *itemPanel12 = new wxPanel(itemSplitterWindow10, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    itemPanel12->SetBackgroundColour(mmColors::listDetailsPanelColor);

    itemSplitterWindow10->SplitHorizontally(m_listCtrlAccount, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(itemPanel12->GetSize().GetHeight());
    itemSplitterWindow10->SetSashGravity(1.0);

    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel12->SetSizer(itemBoxSizer5);

    wxSizerFlags flags;
    flags.Center().Border().Expand().Proportion(1);

    wxButton* itemButton6 = new wxButton(itemPanel12, ID_BUTTON_NEW_TRANS, _("&New"));
    itemButton6->SetToolTip(_("New Transaction"));

    wxFont fnt = itemButton6->GetFont();
    fnt.SetWeight(wxFONTWEIGHT_BOLD);
    fnt.SetPointSize(fnt.GetPointSize()*3/2);

    itemButton6->SetFont(fnt);
    itemButton6->SetForegroundColour(wxColour(wxT("FOREST GREEN")));
    itemBoxSizer5->Add(itemButton6, flags);

    wxButton* itemButton7 = new wxButton(itemPanel12, ID_BUTTON_EDIT_TRANS, _("&Edit"));
    itemButton7->SetToolTip(_("Edit Transaction"));
    itemButton7->SetFont(fnt);
    itemButton7->SetForegroundColour(wxColour(wxT("SALMON")));
    itemBoxSizer5->Add(itemButton7, flags);
    itemButton7->Enable(false);

    wxButton* itemButton8 = new wxButton(itemPanel12, ID_BUTTON_DELETE_TRANS, _("&Delete"));
    itemButton8->SetToolTip(_("Delete Transaction"));
    itemButton8->SetFont(fnt);
    itemButton8->SetForegroundColour(wxColour(wxT("ORANGE"))); // FIREBRICK
    itemBoxSizer5->Add(itemButton8, flags);
    itemButton8->Enable(false);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::enableEditDeleteButtons(bool en)
{
	wxButton* bE = (wxButton*)FindWindow(ID_BUTTON_EDIT_TRANS);
	wxButton* bD = (wxButton*)FindWindow(ID_BUTTON_DELETE_TRANS);
	bE->Enable(en);
	bD->Enable(en);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::updateExtraTransactionData(int selIndex)
{
        enableEditDeleteButtons(selIndex >= 0);
}
//----------------------------------------------------------------------------

void mmCheckingPanel::setAccountSummary()
{
    double total = m_core->accountList_.getAccountSharedPtr(m_AccountID)->balance();
    wxString balance;
    mmex::formatDoubleToCurrency(total, balance);

    double reconciledBal = m_core->bTransactionList_.getReconciledBalance(m_AccountID);
    double acctInitBalance = m_core->accountList_.getAccountSharedPtr(m_AccountID)->initialBalance_;
    
    wxString recbalance;
    mmex::formatDoubleToCurrency(reconciledBal + acctInitBalance, recbalance);

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);

    wxString lbl  = wxString::Format(_("Account Balance : %s      Reconciled Balance : %s"), 
        balance.c_str(), recbalance.c_str());
    header->SetLabel(lbl);
}
//----------------------------------------------------------------------------

typedef boost::shared_ptr<mmBankTransaction> TransactionPtr;
struct TransactionPtr_Matcher
{
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
	virtual bool Match(const TransactionPtr& pTrans)
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

	map[wxT("View Reconciled")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("R"))), false);
	map[wxT("View Void")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("V"))), false);
	map[wxT("View Flagged")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("F"))), false);
	map[wxT("View UnReconciled")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT(""))), false);
	map[wxT("View Not-Reconciled")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status< std::not_equal_to<wxString> >(wxT("R"))), false);
	map[wxT("View Duplicates")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_Status<>(wxT("D"))), false);

	map[wxT("View Today")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::Today>()), true);
	map[wxT("View 30 days")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastDays<30> >()), true);
	map[wxT("View 90 days")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastDays<90> >()), true);
	map[wxT("View Current Month")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::CurrentMonth<> >()), true);
	map[wxT("View Last Month")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastMonths<1, 1> >()), true);
	map[wxT("View Last 3 Months")] = TransactionMatchData(TransactionPtr_MatcherPtr(new MatchTransaction_DateTime<DateTimeProviders::LastMonths<2> >()), true);

	return map;
}
static const TransactionMatchMap& s_transactionMatchers_Map = initTransactionMatchMap();

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
				getBal = data.second;
			}
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

    std::sort(m_trans.begin(), m_trans.end(), sortTransByDateAsc);

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
        mmex::formatDoubleToCurrencyEdit(initBalance, balanceStr);
        tr.balanceStr_ = balanceStr;
    }

#if defined (__WXMSW__)
    pgd->Update(70);
#endif

    /* Setup the Sorting */
     // decide whether top or down icon needs to be shown
    m_listCtrlAccount->setColumnImage(g_sortcol, g_asc ? ICON_ASC : ICON_DESC);
    
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

void MyListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
	m_selectedIndex = -1;
	m_cp->updateExtraTransactionData(m_selectedIndex);
}

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

	// Remake the register. If user was viewing some transactions (eg void)
	//  any changes need to be reflected.  Even if we are viewing all transactions,
	//  the register needs to be updated so the balance col is correct (eg a trans
	//  was changed from unreconciled to void).
	DeleteAllItems();
	m_cp->initVirtualListControl();
}
//----------------------------------------------------------------------------

void MyListCtrl::OnMarkTransaction(wxCommandEvent& event)
{
     int evt = event.GetId();
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

void MyListCtrl::setColumnImage(EColumn col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);

    SetColumn(col, item);
}
//----------------------------------------------------------------------------

wxString mmCheckingPanel::getItem(long item, long column)
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

        case COL_BALANSE:
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

wxString MyListCtrl::OnGetItemText(long item, long column) const
{
    return m_cp->getItem(item, column);
}
//----------------------------------------------------------------------------

/* 
    Returns the icon to be shown for each transaction
*/
int MyListCtrl::OnGetItemImage(long item) const
{
    wxString status = m_cp->getItem(item, COL_STATUS);

    int res = ICON_NONE;

    if (status == wxT("F"))
    {
        res = ICON_FOLLOWUP;
    }
    else if (status == wxT("R"))
    {
        res = ICON_RECONCILED;
    }
    else if (status == wxT("V"))
    {
        res = ICON_VOID;
    }
    else if (status == wxT("D"))
    {
        res = ICON_DUPLICATE;
    }

    return res;
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
{	if (wxGetKeyState(WXK_ALT) || 
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
		wxGetKeyState(WXK_NUMPAD_DELETE)
		)
	event.Skip();
  }
//----------------------------------------------------------------------------

void MyListCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    if (m_selectedIndex != -1) {
        m_selectedForCopy = m_cp->m_trans[m_selectedIndex]->transactionID();
    }
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
	if (!wxGetKeyState(WXK_COMMAND) && !wxGetKeyState(WXK_ALT))
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
}
//----------------------------------------------------------------------------

void MyListCtrl::OnNewTransaction(wxCommandEvent& /*event*/)
{
    mmTransDialog dlg(m_cp->getDb(), m_cp->m_core, m_cp->accountID(), 0, false, m_cp->m_inidb, this );

    if ( dlg.ShowModal() == wxID_OK )
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
}
//----------------------------------------------------------------------------

void MyListCtrl::OnDeleteTransaction(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex == -1) //check if a transaction is selected
        return;

    //ask if they really want to delete
    wxMessageDialog msgDlg(this, 
                           _("Do you really want to delete the transaction?"),
                           _("Delete Transaction"),
                           wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION
                          );
                          
    if (msgDlg.ShowModal() != wxID_YES)
        return;

    //find the topmost visible item - this will be used to set 
    // where to display the list again after refresh
    long topItemIndex = GetTopItem();

    //remove the transaction
    m_cp->m_core->bTransactionList_.deleteTransaction(m_cp->accountID(), m_cp->m_trans[m_selectedIndex]->transactionID());

    //initialize the transaction list to redo balances and images
    m_cp->initVirtualListControl();

    if (!m_cp->m_trans.empty()) {
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
    } else {
        SetItemCount(0);
        DeleteAllItems();
        m_selectedIndex = -1;
    }
}
//----------------------------------------------------------------------------

void MyListCtrl::OnEditTransaction(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex != -1)
	{
		mmTransDialog dlg(m_cp->getDb(), m_cp->m_core, m_cp->accountID(), 
		   m_cp->m_trans[m_selectedIndex]->transactionID(), true, m_cp->m_inidb, this);
		if ( dlg.ShowModal() == wxID_OK )
		{
			m_cp->initVirtualListControl();
			RefreshItems(0, static_cast<long>(m_cp->m_trans.size()) - 1);
			SetItemState(m_selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
			SetItemState(m_selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			EnsureVisible(m_selectedIndex);
		}
	}
}
//----------------------------------------------------------------------------

void MyListCtrl::OnListItemActivated(wxListEvent& /*event*/)
{
    if (m_selectedIndex != -1)
	{
        //m_selectedIndex = event.GetIndex();
        mmTransDialog dlg(m_cp->getDb(), m_cp->m_core,  m_cp->accountID(), 
            m_cp->m_trans[m_selectedIndex]->transactionID(), true, m_cp->m_inidb, this);
        if ( dlg.ShowModal() == wxID_OK )
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
    wxASSERT(m_core);
    return m_core->db_; 
}
//----------------------------------------------------------------------------

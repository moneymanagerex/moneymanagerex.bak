/*******************************************************
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

#include "billsdepositspanel.h"
#include "util.h"
#include "dbwrapper.h"
#include "billsdepositsdialog.h"

/* Include XPM Support */
#include "../resources/error.xpm"
#include "../resources/rt_exec_auto.xpm"
#include "../resources/rt_exec_user.xpm"

/*******************************************************/
BEGIN_EVENT_TABLE(mmBillsDepositsPanel, wxPanel)
    EVT_BUTTON(wxID_NEW,         mmBillsDepositsPanel::OnNewBDSeries)
    EVT_BUTTON(wxID_EDIT,        mmBillsDepositsPanel::OnEditBDSeries)
    EVT_BUTTON(wxID_DELETE,      mmBillsDepositsPanel::OnDeleteBDSeries)
    EVT_BUTTON(wxID_PASTE, mmBillsDepositsPanel::OnEnterBDTransaction)
    EVT_BUTTON(wxID_IGNORE,  mmBillsDepositsPanel::OnSkipBDTransaction)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(billsDepositsListCtrl, mmListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_BD_LISTCTRL,   billsDepositsListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_BD_LISTCTRL, billsDepositsListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_BD_LISTCTRL,    billsDepositsListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(ID_PANEL_BD_LISTCTRL,    billsDepositsListCtrl::OnListItemDeselected)

    EVT_MENU(MENU_TREEPOPUP_NEW,              billsDepositsListCtrl::OnNewBDSeries)
    EVT_MENU(MENU_TREEPOPUP_EDIT,             billsDepositsListCtrl::OnEditBDSeries)
    EVT_MENU(MENU_TREEPOPUP_DELETE,           billsDepositsListCtrl::OnDeleteBDSeries)
    EVT_MENU(MENU_POPUP_BD_ENTER_OCCUR,       billsDepositsListCtrl::OnEnterBDTransaction)
    EVT_MENU(MENU_POPUP_BD_SKIP_OCCUR,        billsDepositsListCtrl::OnSkipBDTransaction)

    EVT_LIST_KEY_DOWN(ID_PANEL_BD_LISTCTRL,   billsDepositsListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/

namespace
{

inline bool sortTransactionsByRemainingDays(const mmBDTransactionHolder &p1, const mmBDTransactionHolder &p2)
{
    return p1.daysRemaining_ < p2.daysRemaining_;
};

} // namespace

mmBillsDepositsPanel::mmBillsDepositsPanel(wxSQLite3Database* db, wxSQLite3Database* inidb,
            mmCoreDB* core,
            wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name )
            : mmPanelBase(db, inidb, core), m_imageList(), listCtrlAccount_()
{
    Create(parent, winid, pos, size, style, name);
}

bool mmBillsDepositsPanel::Create( wxWindow *parent,
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
        listCtrlAccount_->EnsureVisible(((int)trans_.size()) - 1);

    this->Thaw();
    return TRUE;
}

mmBillsDepositsPanel::~mmBillsDepositsPanel()
{
    if (m_imageList) delete m_imageList;
    this->save_config(listCtrlAccount_, wxT("BD"));
}

void mmBillsDepositsPanel::CreateControls()
{
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer9);
    //this->SetBackgroundColour(mmColors::listBackColor);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);
    //headerPanel->SetBackgroundColour(mmColors::listBackColor);

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, ID_PANEL_BD_STATIC_HEADER,
        _("Repeating Transactions"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);

    /* ---------------------- */
    wxSplitterWindow* itemSplitterWindowBillsDeposit = new wxSplitterWindow( this,
        ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100),
        wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(error_xpm));
    m_imageList->Add(wxBitmap(rt_exec_auto_xpm));
    m_imageList->Add(wxBitmap(rt_exec_user_xpm));

    listCtrlAccount_ = new billsDepositsListCtrl( this, itemSplitterWindowBillsDeposit,
        ID_PANEL_BD_LISTCTRL, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    //listCtrlAccount_->SetBackgroundColour(mmColors::listDetailsPanelColor);
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlAccount_->InsertColumn(0, _("Payee"));
    listCtrlAccount_->InsertColumn(1, _("Account"));
    listCtrlAccount_->InsertColumn(2, _("Type"));
    wxListItem itemCol;
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Amount"));
    listCtrlAccount_->InsertColumn(3, itemCol);
    listCtrlAccount_->InsertColumn(4, _("Next Due Date"));
    listCtrlAccount_->InsertColumn(5, _("Frequency"));
    listCtrlAccount_->InsertColumn(6, _("Remaining Days"));
    listCtrlAccount_->InsertColumn(7, _("Notes"));

    /* See if we can get data from inidb */
    long col0, col1, col2, col3, col4, col5, col6, col7;
    mmDBWrapper::getINISettingValue(inidb_, wxT("BD_COL0_WIDTH"), wxT("150")).ToLong(&col0);
    mmDBWrapper::getINISettingValue(inidb_, wxT("BD_COL1_WIDTH"), wxT("-2")).ToLong(&col1);
    mmDBWrapper::getINISettingValue(inidb_, wxT("BD_COL2_WIDTH"), wxT("-2")).ToLong(&col2);
    mmDBWrapper::getINISettingValue(inidb_, wxT("BD_COL3_WIDTH"), wxT("-2")).ToLong(&col3);
    mmDBWrapper::getINISettingValue(inidb_, wxT("BD_COL4_WIDTH"), wxT("-2")).ToLong(&col4);
    mmDBWrapper::getINISettingValue(inidb_, wxT("BD_COL5_WIDTH"), wxT("-2")).ToLong(&col5);
    mmDBWrapper::getINISettingValue(inidb_, wxT("BD_COL6_WIDTH"), wxT("-2")).ToLong(&col6);
    mmDBWrapper::getINISettingValue(inidb_,  wxT("BD_COL7_WIDTH"), wxT("-2")).ToLong(&col7);


    listCtrlAccount_->SetColumnWidth(0, col0);
    listCtrlAccount_->SetColumnWidth(1, col1);
    listCtrlAccount_->SetColumnWidth(2, col2);
    listCtrlAccount_->SetColumnWidth(3, col3);
    listCtrlAccount_->SetColumnWidth(4, col4);
    listCtrlAccount_->SetColumnWidth(5, col5);
    listCtrlAccount_->SetColumnWidth(6, col6);
    listCtrlAccount_->SetColumnWidth(7, col7);

    wxPanel* itemPanel12 = new wxPanel( itemSplitterWindowBillsDeposit, ID_PANEL1,
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

    itemSplitterWindowBillsDeposit->SplitHorizontally(listCtrlAccount_, itemPanel12);
    itemSplitterWindowBillsDeposit->SetMinimumPaneSize(100);
    itemSplitterWindowBillsDeposit->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindowBillsDeposit, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);
    //itemPanel12->SetBackgroundColour(mmColors::listBackColor);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton6 = new wxButton( itemPanel12, wxID_NEW, _("&New"));
    itemButton6->SetToolTip(_("New Bills & Deposit Series"));
    itemBoxSizer5->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    wxButton* itemButton81 = new wxButton( itemPanel12, wxID_EDIT, _("&Edit"));
    itemButton81->SetToolTip(_("Edit Bills & Deposit Series"));
    itemBoxSizer5->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton81->Enable(false);

    wxButton* itemButton7 = new wxButton( itemPanel12, wxID_DELETE, _("&Delete"));
    itemButton7->SetToolTip(_("Delete Bills & Deposit Series"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton7->Enable(false);

    wxButton* itemButton8 = new wxButton( itemPanel12, wxID_PASTE, _("En&ter"),
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton8->SetToolTip(_("Enter Next Bills & Deposit Occurrence"));
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton8->Enable(false);

    wxButton* buttonSkipTrans = new wxButton( itemPanel12, wxID_IGNORE, _("&Skip"));
    buttonSkipTrans->SetToolTip(_("Skip Next Bills & Deposit Occurrence"));
    itemBoxSizer5->Add(buttonSkipTrans, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    buttonSkipTrans->Enable(false);

    //Infobar-mini
    wxStaticText* itemStaticText444 = new wxStaticText(itemPanel12, ID_PANEL_BD_STATIC_MINI, wxT(""),
                                                       wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer5->Add(itemStaticText444, 1, wxGROW|wxTOP, 12);

    //Infobar
    wxStaticText* text = new wxStaticText( itemPanel12, ID_PANEL_BD_STATIC_DETAILS, wxT(""),
    wxPoint(-1,-1), wxSize(200, -1), wxNO_BORDER|wxTE_MULTILINE|wxTE_WORDWRAP|wxST_NO_AUTORESIZE);
    itemBoxSizer4->Add(text, 1, wxGROW|wxLEFT|wxRIGHT, 14);

    mmBillsDepositsPanel::updateBottomPanelData(-1);
}

void mmBillsDepositsPanel::initVirtualListControl()
{
    static const char sql[] =
       "select c.categname, "
       "sc.subcategname, "
       "b.BDID, "
       "b.NUMOCCURRENCES, "
       "b.NEXTOCCURRENCEDATE, "
       "b.REPEATS, "
       "b.PAYEEID, "
       "b.TRANSCODE, "
       "b.ACCOUNTID, "
       "b.TOACCOUNTID, "
       "b.TRANSAMOUNT, "
       "b.TOTRANSAMOUNT, "
       "b.NOTES, "
       "b.CATEGID, "
       "b.SUBCATEGID "

        "from BILLSDEPOSITS_V1 b "

        "left join category_v1 c "
        "on c.categid = b.categid "

        "left join subcategory_v1 sc "
        "on sc.subcategid = b.subcategid";

    trans_.clear();

    mmDBWrapper::loadBaseCurrencySettings(db_);

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

    while (q1.NextRow())
    {
        mmBDTransactionHolder th;

        th.id_           = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));
        th.nextOccurStr_   = mmGetDateForDisplay(db_, th.nextOccurDate_);
        int repeats        = q1.GetInt(wxT("REPEATS"));
        th.payeeID_        = q1.GetInt(wxT("PAYEEID"));
        th.transType_      = q1.GetString(wxT("TRANSCODE"));
        th.accountID_      = q1.GetInt(wxT("ACCOUNTID"));
        th.toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));
        th.accountName_    = core_->getAccountName(th.accountID_);
        th.amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
        th.toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));
        th.notes_           = q1.GetString(wxT("NOTES"));
        th.categID_           = q1.GetInt(wxT("CATEGID"));
        th.categoryStr_       = q1.GetString(wxT("CATEGNAME"));
        th.subcategID_       = q1.GetInt(wxT("SUBCATEGID"));
        th.subcategoryStr_ = q1.GetString(wxT("SUBCATEGNAME"));

        int numRepeats     = q1.GetInt(wxT("NUMOCCURRENCES"));

        th.bd_repeat_user_ = false;
        th.bd_repeat_auto_ = false;

        // DeMultiplex the Auto Executable fields.
        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute User Acknowlegement required
        {
            repeats -= BD_REPEATS_MULTIPLEX_BASE;
            th.bd_repeat_user_ = true;
        }

        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute Silent mode
        {
            repeats -= BD_REPEATS_MULTIPLEX_BASE;
            th.bd_repeat_auto_ = true;
        }

        if (repeats == 0)
           th.repeatsStr_ = _("None");
        else if (repeats == 1)
           th.repeatsStr_ = _("Weekly");
        else if (repeats == 2)
           th.repeatsStr_ = _("Bi-Weekly");
        else if (repeats == 3)
           th.repeatsStr_ = _("Monthly");
        else if (repeats == 4)
           th.repeatsStr_ = _("Bi-Monthly");
        else if (repeats == 5)
           th.repeatsStr_ = _("Quarterly");
        else if (repeats == 6)
           th.repeatsStr_ = _("Half-Yearly");
        else if (repeats == 7)
           th.repeatsStr_ = _("Yearly");
        else if (repeats == 8)
           th.repeatsStr_ = _("Four Months");
        else if (repeats == 9)
           th.repeatsStr_ = _("Four Weeks");
        else if (repeats == 10)
           th.repeatsStr_ = _("Daily");
        else if ( (repeats == 11) && (numRepeats >= 0 ) )
           th.repeatsStr_ = wxString() << _("In ") <<  numRepeats << _(" Days");
        else if ( (repeats == 12) && (numRepeats >= 0 ) )
           th.repeatsStr_ = wxString() << _("In ") <<  numRepeats << _(" Months");
        else if ( (repeats == 13) && (numRepeats >= 0 ) )
           th.repeatsStr_ = wxString() << _("Every ") <<  numRepeats << _(" Days");
        else if ( (repeats == 14) && (numRepeats >= 0 ) )
           th.repeatsStr_ = wxString() << _("Every ") <<  numRepeats << _(" Months");

        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = th.nextOccurDate_.Subtract(today);
        th.daysRemaining_ = ts.GetDays();
        int minutesRemaining = ts.GetMinutes();

        if (minutesRemaining > 0)
            th.daysRemaining_ += 1;

        th.daysRemainingStr_ = wxString::Format(wxT("%d"), th.daysRemaining_) + _(" days remaining");

        if (th.daysRemaining_ == 0)
        {
            if ((repeats > 10) && (numRepeats < 0) )
                th.daysRemainingStr_ = _("Inactive");
        }

        if (th.daysRemaining_ < 0)
        {
            th.daysRemainingStr_ = wxString::Format(wxT("%d"), abs(th.daysRemaining_)) + _(" days overdue!");
            if ((repeats > 10) && (numRepeats < 0) )
                th.daysRemainingStr_ = _("Inactive");
        }

        mmex::formatDoubleToCurrencyEdit(th.amt_, th.transAmtString_);
        mmex::formatDoubleToCurrencyEdit(th.toAmt_, th.transToAmtString_);

        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

        if (th.transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            wxString fromAccount = core_->getAccountName(th.accountID_);
            wxString toAccount = core_->getAccountName(th.toAccountID_ );

            th.payeeStr_ = toAccount;
        }

        trans_.push_back(th);
    }

    q1.Finalize();

    std::sort(trans_.begin(), trans_.end(), sortTransactionsByRemainingDays);
    listCtrlAccount_->SetItemCount(static_cast<long>(trans_.size()));
}

void mmBillsDepositsPanel::OnNewBDSeries(wxCommandEvent& event)
{
  listCtrlAccount_->OnNewBDSeries(event);
}

void mmBillsDepositsPanel::OnEditBDSeries(wxCommandEvent& event)
{
    listCtrlAccount_->OnEditBDSeries(event);
}

void mmBillsDepositsPanel::OnDeleteBDSeries(wxCommandEvent& event)
{
    listCtrlAccount_->OnDeleteBDSeries(event);
}

void mmBillsDepositsPanel::OnEnterBDTransaction(wxCommandEvent& event)
{
    listCtrlAccount_->OnEnterBDTransaction(event);
}

void mmBillsDepositsPanel::OnSkipBDTransaction(wxCommandEvent& event)
{
    listCtrlAccount_->OnSkipBDTransaction(event);
    listCtrlAccount_->SetFocus();
}

/*******************************************************/

void billsDepositsListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_POPUP_BD_ENTER_OCCUR, _("Enter next Occurrence..."));
    menu.AppendSeparator();
    menu.Append(MENU_POPUP_BD_SKIP_OCCUR, _("Skip next Occurrence"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Bills && Deposit Series..."));
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Bills && Deposit Series..."));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Bills && Deposit Series..."));
    PopupMenu(&menu, event.GetPoint());
}


wxString mmBillsDepositsPanel::getItem(long item, long column)
{
    if (column == 0) return trans_[item].payeeStr_;
    if (column == 1) return trans_[item].accountName_;

    if (column == 2)
    {
        if (trans_[item].transType_ == TRANS_TYPE_WITHDRAWAL_STR) return _("Withdrawal");
        else if (trans_[item].transType_ == TRANS_TYPE_DEPOSIT_STR) return _("Deposit");
        else if (trans_[item].transType_ == TRANS_TYPE_TRANSFER_STR) return _("Transfer");
    }

    if (column == 3) return trans_[item].transAmtString_;
    if (column == 4) return trans_[item].nextOccurStr_;
    if (column == 5) return trans_[item].repeatsStr_;
    if (column == 6) return trans_[item].daysRemainingStr_;
    if (column == 7) return trans_[item].notes_;

    return wxT("");
}

wxString billsDepositsListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

void billsDepositsListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    cp_->updateBottomPanelData(selectedIndex_);
}

void billsDepositsListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
    selectedIndex_ = -1;
    cp_->updateBottomPanelData(selectedIndex_);
}

int billsDepositsListCtrl::OnGetItemImage(long item) const
{
    /* Returns the icon to be shown for each entry */
    if (cp_->trans_[item].daysRemainingStr_ == _("Inactive")) return -1;
    if (cp_->trans_[item].daysRemaining_ < 0) return 0;
    if (cp_->trans_[item].bd_repeat_auto_) return 1;
    if (cp_->trans_[item].bd_repeat_user_) return 2;

    return -1;
}

void billsDepositsListCtrl::OnListKeyDown(wxListEvent& event)
{
  switch ( event.GetKeyCode() )
    {
        case WXK_DELETE:
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,
                    MENU_TREEPOPUP_DELETE);
                OnDeleteBDSeries(evt);
            }
            break;

        default:
            event.Skip();
    }
}

int billsDepositsListCtrl::LastSelected(int transID)
{
    int foundIndex = -1;
    int id = 0;
    bool searching = true;
    while (searching && (id < (int)cp_->trans_.size()))
    {
        if (cp_->trans_[id].id_ == transID)
        {
            searching = false;
            foundIndex = id;
        }
        ++ id;
    }

    return foundIndex;
}

void billsDepositsListCtrl::RefreshList()
{
    int refID = cp_->trans_[selectedIndex_].id_;
    cp_->initVirtualListControl();
    RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    int newIndex = LastSelected(refID);
    if (newIndex < 0)      // item deleted from list.
        ++ selectedIndex_; // Advance to next item in list
    else                   // set the new position in the list.
        selectedIndex_ = newIndex;   

    // Ensure item is within range.
    if (selectedIndex_ > (int)(cp_->trans_.size() - 1))
        selectedIndex_ = cp_->trans_.size() - 1;

    cp_->updateBottomPanelData(selectedIndex_);
}

void billsDepositsListCtrl::OnNewBDSeries(wxCommandEvent& /*event*/)
{
    mmBDDialog dlg(cp_->db_, cp_->core_, 0, false, false, this );
    if ( dlg.ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
}

void billsDepositsListCtrl::OnEditBDSeries(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;
    if (!cp_->db_) return;

    mmBDDialog dlg(cp_->db_, cp_->core_, cp_->trans_[selectedIndex_].id_, true, false, this );
    if ( dlg.ShowModal() == wxID_OK )
    {
        RefreshList();
    }
}

void billsDepositsListCtrl::OnDeleteBDSeries(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;
    if (!cp_->db_) return;
    if (cp_->trans_.size() == 0) return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the series?"),
                                        _("Confirm Series Deletion"),
                                        wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        mmDBWrapper::deleteBDSeries(cp_->db_, cp_->trans_[selectedIndex_].id_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl();
        if (cp_->trans_.size() == 0)
        {
            selectedIndex_ = -1;
            cp_->updateBottomPanelData(selectedIndex_);
        }
    }
}

void billsDepositsListCtrl::OnEnterBDTransaction(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;
    if (!cp_->db_) return;

    mmBDDialog dlg(cp_->db_, cp_->core_, cp_->trans_[selectedIndex_].id_, false, true, this );
    if ( dlg.ShowModal() == wxID_OK )
    {
        RefreshList();
    }
}

void billsDepositsListCtrl::OnSkipBDTransaction(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1 || !cp_->db_) return;

    mmDBWrapper::completeBDInSeries(cp_->db_, cp_->trans_[selectedIndex_].id_);
    RefreshList();
}

void billsDepositsListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    if (selectedIndex_ == -1) return;
    if (!cp_->db_) return;

    mmBDDialog dlg(cp_->db_, cp_->core_, cp_->trans_[selectedIndex_].id_, true, false, this );
    if ( dlg.ShowModal() == wxID_OK )
    {
        RefreshList();
    }
}

void mmBillsDepositsPanel::updateBottomPanelData(int selIndex)
{
    enableEditDeleteButtons(selIndex >= 0);
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_BD_STATIC_DETAILS);
    wxStaticText* stm = (wxStaticText*)FindWindow(ID_PANEL_BD_STATIC_MINI);

    if (selIndex !=-1)
    {
        wxListItem selectedItem;
        selectedItem.SetId(selIndex);
        listCtrlAccount_->GetItem(selectedItem);
        listCtrlAccount_->SetItemState(selectedItem, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);

        wxString addInfo;
        addInfo << trans_[selIndex].categoryStr_ << (trans_[selIndex].subcategoryStr_ == wxT ("") ? wxT ("") : wxT (":") + trans_[selIndex].subcategoryStr_);
        stm->SetLabel(addInfo);
        st ->SetLabel (trans_[selIndex].notes_ );
    }
    else
    {
        st-> SetLabel(Tips(TIPS_BILLS));
        stm-> SetLabel(wxT(""));
    }
}

void mmBillsDepositsPanel::enableEditDeleteButtons(bool en)
{
    wxButton* bE = (wxButton*)FindWindow(wxID_EDIT);
    wxButton* bD = (wxButton*)FindWindow(wxID_DELETE);
    wxButton* bN = (wxButton*)FindWindow(wxID_PASTE);
    wxButton* bS = (wxButton*)FindWindow(wxID_IGNORE);
    bE->Enable(en);
    bD->Enable(en);
    bN->Enable(en);
    bS->Enable(en);
}

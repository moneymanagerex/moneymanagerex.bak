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

#include "billsdeposits.h"
#include "util.h"
#include "dbwrapper.h"
#include "bddialog.h"

#include <algorithm>
#include <vector>

/* Include XPM Support */
#include "../resources/error.xpm"

/*******************************************************/
BEGIN_EVENT_TABLE(mmBillsDepositsPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_NEW_BD,         mmBillsDepositsPanel::OnNewBDSeries)
    EVT_BUTTON(ID_BUTTON_EDIT_BD,        mmBillsDepositsPanel::OnEditBDSeries)
    EVT_BUTTON(ID_BUTTON_DELETE_BD,      mmBillsDepositsPanel::OnDeleteBDSeries)
    EVT_BUTTON(ID_BUTTON_ENTEROCCUR_BD,  mmBillsDepositsPanel::OnEnterBDSeriesOccurrence)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(billsDepositsListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_BD_LISTCTRL,   billsDepositsListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_BD_LISTCTRL, billsDepositsListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_BD_LISTCTRL,    billsDepositsListCtrl::OnListItemSelected)
        
    EVT_MENU(MENU_TREEPOPUP_NEW,              billsDepositsListCtrl::OnNewBDSeries)
    EVT_MENU(MENU_TREEPOPUP_EDIT,             billsDepositsListCtrl::OnEditBDSeries)
    EVT_MENU(MENU_TREEPOPUP_DELETE,           billsDepositsListCtrl::OnDeleteBDSeries)
    EVT_MENU(MENU_TREEPOPUP_ENTEROCCUR,       billsDepositsListCtrl::OnEnterBDSeriesOccurrence)
    
    EVT_LIST_KEY_DOWN(ID_PANEL_BD_LISTCTRL,   billsDepositsListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/
mmBillsDepositsPanel::mmBillsDepositsPanel(wxSQLite3Database* db, wxSQLite3Database* inidb,
            wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name )
            : db_(db), inidb_(inidb), m_imageList(0)
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
   if (m_imageList)
        delete m_imageList;

    long col0, col1, col2, col3, col4, col5;
    col0 = listCtrlAccount_->GetColumnWidth(0);
    col1 = listCtrlAccount_->GetColumnWidth(1);
    col2 = listCtrlAccount_->GetColumnWidth(2);
    col3 = listCtrlAccount_->GetColumnWidth(3);
    col4 = listCtrlAccount_->GetColumnWidth(4);
    col5 = listCtrlAccount_->GetColumnWidth(5);

    wxString col0Str = wxString::Format(wxT("%d"), col0);
    wxString col1Str = wxString::Format(wxT("%d"), col1);
    wxString col2Str = wxString::Format(wxT("%d"), col2);
    wxString col3Str = wxString::Format(wxT("%d"), col3);
    wxString col4Str = wxString::Format(wxT("%d"), col4);
    wxString col5Str = wxString::Format(wxT("%d"), col5);

    mmDBWrapper::setINISettingValue(inidb_, wxT("BD_COL0_WIDTH"), col0Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BD_COL1_WIDTH"), col1Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BD_COL2_WIDTH"), col2Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BD_COL3_WIDTH"), col3Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BD_COL4_WIDTH"), col4Str); 
}

void mmBillsDepositsPanel::CreateControls()
{    
    mmBillsDepositsPanel* itemPanel8 = this;

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemPanel8->SetSizer(itemBoxSizer9);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( itemPanel8, ID_PANEL15, wxDefaultPosition, 
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, ID_PANEL_BD_STATIC_HEADER, 
        wxT("Bills && Deposits"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, 
        wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( itemPanel8, 
        ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), 
        wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(error_xpm));

    listCtrlAccount_ = new billsDepositsListCtrl( this, itemSplitterWindow10, 
        ID_PANEL_BD_LISTCTRL, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    listCtrlAccount_->SetBackgroundColour(mmColors::listBackColor);
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlAccount_->InsertColumn(0, _("Payee"));
    wxListItem itemCol;
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Amount"));
    listCtrlAccount_->InsertColumn(1, itemCol);
    listCtrlAccount_->InsertColumn(2, _("Next Due Date"));
    listCtrlAccount_->InsertColumn(3, _("Frequency"));
    listCtrlAccount_->InsertColumn(4, _("Remaining Days"));
    
    /* See if we can get data from inidb */
     long col0, col1, col2, col3, col4;
     mmDBWrapper::getINISettingValue(inidb_, 
        wxT("BD_COL0_WIDTH"), wxT("150")).ToLong(&col0); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BD_COL1_WIDTH"), wxT("-2")).ToLong(&col1); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BD_COL2_WIDTH"), wxT("-2")).ToLong(&col2); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BD_COL3_WIDTH"), wxT("-2")).ToLong(&col3); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BD_COL4_WIDTH"), wxT("-2")).ToLong(&col4); 
     
    listCtrlAccount_->SetColumnWidth(0, col0);
    listCtrlAccount_->SetColumnWidth(1, col1);
    listCtrlAccount_->SetColumnWidth(2, col2);
    listCtrlAccount_->SetColumnWidth(3, col3);
    listCtrlAccount_->SetColumnWidth(4, col4);
    
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

    wxButton* itemButton6 = new wxButton( itemPanel12, ID_BUTTON_NEW_BD, _("New Series"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton6->SetToolTip(_("New Bills && Deposit Series"));
    itemBoxSizer5->Add(itemButton6, 0, wxGROW|wxALL, 1);

    wxButton* itemButton81 = new wxButton( itemPanel12, ID_BUTTON_EDIT_BD, _("Edit Series"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton81->SetToolTip(_("Edit Bills && Deposit Series"));
    itemBoxSizer5->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton7 = new wxButton( itemPanel12, ID_BUTTON_DELETE_BD, _("Delete Series"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton7->SetToolTip(_("Delete Bills && Deposit Series"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton8 = new wxButton( itemPanel12, ID_BUTTON_ENTEROCCUR_BD, _("Enter Transaction"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton8->SetToolTip(_("Enter Next Bills && Deposit Occurrence"));
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel12, 
        ID_PANEL_CHECKING_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    itemBoxSizer4->Add(itemStaticText11, 1, wxGROW|wxALL, 5);
}

bool sortTransactionsByRemainingDays( mmBDTransactionHolder elem1, mmBDTransactionHolder elem2 )
{
    return elem1.daysRemaining_ < elem2.daysRemaining_;
}


void mmBillsDepositsPanel::initVirtualListControl()
{
    /* Clear all the records */
    trans_.clear();

    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from BILLSDEPOSITS_V1;");
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);

    int ct = 0;
    while (q1.NextRow())
    {
        mmBDTransactionHolder th;

        th.bdID_           = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));
        th.nextOccurStr_   = mmGetDateForDisplay(db_, th.nextOccurDate_);
        int repeats        = q1.GetInt(wxT("REPEATS"));

        if (repeats == 0)
        {
            th.repeatsStr_ = _("None");
        }
        else if (repeats == 1)
        {
           th.repeatsStr_ = _("Weekly");
        }
        else if (repeats == 2)
        {
            th.repeatsStr_ = _("Bi-Weekly");
        }
        else if (repeats == 3)
        {
            th.repeatsStr_ = _("Monthly");
        }
        else if (repeats == 4)
        {
            th.repeatsStr_ = _("Bi-Monthly");
        }
        else if (repeats == 5)
        {
            th.repeatsStr_ = _("Quarterly");
        }
        else if (repeats == 6)
        {
             th.repeatsStr_ = _("Half-Yearly");
        }
        else if (repeats == 7)
        {
           th.repeatsStr_ = _("Yearly");
        }

        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = th.nextOccurDate_.Subtract(today);
        th.daysRemaining_ = ts.GetDays();
        if (th.daysRemaining_ >= 0)
        {
            th.daysRemainingStr_ = wxString::Format(wxT("%d"), th.daysRemaining_) + 
            _(" days remaining");
        }
        else
        {
            th.daysRemainingStr_ = wxString::Format(wxT("%d"), abs(th.daysRemaining_)) + 
            _(" days overdue!");
        }

        th.payeeID_        = q1.GetInt(wxT("PAYEEID"));
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

        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

        if (th.transType_ == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_,  th.accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  th.toAccountID_ );

            th.payeeStr_ = toAccount;
        }

        trans_.push_back(th);
        ct++;
    }
    q1.Finalize();
    std::sort(trans_.begin(), trans_.end(), sortTransactionsByRemainingDays);
    listCtrlAccount_->SetItemCount(ct);

    mmENDSQL_LITE_EXCEPTION;
}

void mmBillsDepositsPanel::OnDeleteBDSeries(wxCommandEvent& event)
{
    listCtrlAccount_->OnDeleteBDSeries(event);
}

void mmBillsDepositsPanel::OnEnterBDSeriesOccurrence(wxCommandEvent& event)
{
    listCtrlAccount_->OnEnterBDSeriesOccurrence(event);
}

void mmBillsDepositsPanel::OnNewBDSeries(wxCommandEvent& event)
{
  listCtrlAccount_->OnNewBDSeries(event);
}

void mmBillsDepositsPanel::OnEditBDSeries(wxCommandEvent& event)
{
    listCtrlAccount_->OnEditBDSeries(event);
}

/*******************************************************/

void billsDepositsListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Bills && Deposit Series"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Bills && Deposit Series"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Bills && Deposit Series"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_ENTEROCCUR, _("Enter next Occurrence"));
    PopupMenu(&menu, event.GetPoint());
}


wxString mmBillsDepositsPanel::getItem(long item, long column)
{
    if (column == 0)
        return trans_[item].payeeStr_;

    if (column == 1)
        return trans_[item].transAmtString_;

    if (column == 2)
        return trans_[item].nextOccurStr_;

    if (column == 3)
        return trans_[item].repeatsStr_;

    if (column == 4)
        return trans_[item].daysRemainingStr_;

    return wxT("");
}

wxString billsDepositsListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

void billsDepositsListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
}

int billsDepositsListCtrl::OnGetItemImage(long item) const
{
   /* Returns the icon to be shown for each entry */
   if (cp_->trans_[item].daysRemaining_ < 0)
       return 0;

   return -1;
}

wxListItemAttr* billsDepositsListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
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

void billsDepositsListCtrl::OnNewBDSeries(wxCommandEvent& event)
{
    mmBDDialog *dlg = new mmBDDialog(cp_->db_, 0, false, false, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void billsDepositsListCtrl::OnDeleteBDSeries(wxCommandEvent& event)
{
    if (selectedIndex_ == -1)
        return;

    if (!cp_->db_)
        return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the series?"),
                                        _("Confirm Series Deletion"),
                                        wxYES_NO);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        mmDBWrapper::deleteBDSeries(cp_->db_, cp_->trans_[selectedIndex_].bdID_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl();
    }
}

void billsDepositsListCtrl::OnEnterBDSeriesOccurrence(wxCommandEvent& event)
{
    if (selectedIndex_ == -1)
        return;

    if (!cp_->db_)
        return;

    mmBDDialog *dlg = new mmBDDialog(cp_->db_, 
        cp_->trans_[selectedIndex_].bdID_, false, true, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void billsDepositsListCtrl::OnEditBDSeries(wxCommandEvent& event)
{
    if (selectedIndex_ == -1)
        return;

    if (!cp_->db_)
        return;

    mmBDDialog *dlg = new mmBDDialog(cp_->db_, 
        cp_->trans_[selectedIndex_].bdID_, true, false, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void billsDepositsListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    if (selectedIndex_ == -1)
        return;

    if (!cp_->db_)
        return;

    mmBDDialog *dlg = new mmBDDialog(cp_->db_, 
        cp_->trans_[selectedIndex_].bdID_, true, false, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();

}

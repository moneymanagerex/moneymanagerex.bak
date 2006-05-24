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

#include "stocks.h"
#include "util.h"
#include "dbwrapper.h"
#include "stockdialog.h"

#include <algorithm>
#include <vector>

/* Include XPM Support */
#include "../resources/uparrow.xpm"
#include "../resources/downarrow.xpm"

/*******************************************************/
BEGIN_EVENT_TABLE(mmStocksPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_NEW_STOCK,         mmStocksPanel::OnNewStocks)
    EVT_BUTTON(ID_BUTTON_EDIT_STOCK,        mmStocksPanel::OnEditStocks)
    EVT_BUTTON(ID_BUTTON_DELETE_STOCK,      mmStocksPanel::OnDeleteStocks)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(stocksListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_STOCKS_LISTCTRL,   stocksListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_STOCKS_LISTCTRL, stocksListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_STOCKS_LISTCTRL,    stocksListCtrl::OnListItemSelected)
        
    EVT_MENU(MENU_TREEPOPUP_NEW,              stocksListCtrl::OnNewStocks)
    EVT_MENU(MENU_TREEPOPUP_EDIT,             stocksListCtrl::OnEditStocks)
    EVT_MENU(MENU_TREEPOPUP_DELETE,           stocksListCtrl::OnDeleteStocks)
    
    EVT_LIST_KEY_DOWN(ID_PANEL_STOCKS_LISTCTRL,   stocksListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/
mmStocksPanel::mmStocksPanel(wxSQLite3Database* db, wxSQLite3Database* inidb,
							 int accountID,
            wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name )
            : db_(db), inidb_(inidb), m_imageList(0), accountID_(accountID)
{
    
    Create(parent, winid, pos, size, style, name);
}

bool mmStocksPanel::Create( wxWindow *parent,
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

mmStocksPanel::~mmStocksPanel()
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

    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL0_WIDTH"), col0Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL1_WIDTH"), col1Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL2_WIDTH"), col2Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL3_WIDTH"), col3Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL4_WIDTH"), col4Str); 
}

void mmStocksPanel::CreateControls()
{    
    mmStocksPanel* itemPanel8 = this;

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemPanel8->SetSizer(itemBoxSizer9);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( itemPanel8, ID_PANEL15, wxDefaultPosition, 
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, ID_PANEL_BD_STATIC_HEADER, 
        _("Stock Investments"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, 
        wxT("")));

	wxStaticText* itemStaticText10 = new wxStaticText( headerPanel, 
            ID_PANEL_CHECKING_STATIC_BALHEADER, 
			_("Total:"), wxDefaultPosition, wxDefaultSize, 0 );

    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);
	itemBoxSizerVHeader->Add(itemStaticText10, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( itemPanel8, 
        ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), 
        wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(uparrow_xpm));
    m_imageList->Add(wxBitmap(downarrow_xpm));

    listCtrlAccount_ = new stocksListCtrl( this, itemSplitterWindow10, 
        ID_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    listCtrlAccount_->SetBackgroundColour(mmColors::listBackColor);
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlAccount_->InsertColumn(0, _("Held At"));
    wxListItem itemCol;
    itemCol.SetImage(-1);
    //itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    itemCol.SetText(_("Share Name"));
    listCtrlAccount_->InsertColumn(1, itemCol);
	
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Number of Shares"));
    
    listCtrlAccount_->InsertColumn(2, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Gain/Loss"));

    listCtrlAccount_->InsertColumn(3, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Value"));

    listCtrlAccount_->InsertColumn(4, itemCol);
    
    /* See if we can get data from inidb */
     long col0, col1, col2, col3, col4;
     mmDBWrapper::getINISettingValue(inidb_, 
        wxT("STOCKS_COL0_WIDTH"), wxT("150")).ToLong(&col0); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("STOCKS_COL1_WIDTH"), wxT("-2")).ToLong(&col1); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("STOCKS_COL2_WIDTH"), wxT("-2")).ToLong(&col2); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("STOCKS_COL3_WIDTH"), wxT("-2")).ToLong(&col3); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("STOCKS_COL4_WIDTH"), wxT("-2")).ToLong(&col4); 
     
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

    wxButton* itemButton6 = new wxButton( itemPanel12, ID_BUTTON_NEW_STOCK, _("New"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton6->SetToolTip(_("New Stock Investment"));
    itemBoxSizer5->Add(itemButton6, 0, wxGROW|wxALL, 1);

    wxButton* itemButton81 = new wxButton( itemPanel12, ID_BUTTON_EDIT_STOCK, _("Edit"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton81->SetToolTip(_("Edit Stock Investment"));
    itemBoxSizer5->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton7 = new wxButton( itemPanel12, ID_BUTTON_DELETE_STOCK, _("Delete"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton7->SetToolTip(_("Delete Stock Investment"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel12, 
        ID_PANEL_CHECKING_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    itemBoxSizer4->Add(itemStaticText11, 1, wxGROW|wxALL, 5);
}

void mmStocksPanel::initVirtualListControl()
{
    /* Clear all the records */
    trans_.clear();

    mmBEGINSQL_LITE_EXCEPTION;

	if (accountID_ != -1)
	{
		wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_BD_STATIC_HEADER);
		wxString str = mmDBWrapper::getAccountName(db_, accountID_);
		header->SetLabel(_("Stock Investments: ") + str);
	}
    
	double total = mmDBWrapper::getStockInvestmentBalance(db_);
	if (accountID_ != -1)
	{
		total = mmDBWrapper::getStockInvestmentBalance(db_, accountID_);
	}

    wxString balance;
    mmCurrencyFormatter::formatDoubleToCurrency(total, balance);
    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);
    wxString lbl  = wxString::Format(_("Total: %s"), balance);
    header->SetLabel(lbl);

	wxSQLite3StatementBuffer bufSQL;
	if (accountID_ == -1)    
		bufSQL.Format("select * from STOCK_V1;");
	else
		bufSQL.Format("select * from STOCK_V1 where HELDAT=%d;", accountID_);
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);

    int ct = 0;
    while (q1.NextRow())
    {
        mmStockTransactionHolder th;

        th.stockID_           = q1.GetInt(wxT("STOCKID"));
        int accountID         = q1.GetInt(wxT("HELDAT"));
        th.heldAt_            = mmDBWrapper::getAccountName(db_, accountID);
        th.shareName_         = q1.GetString(wxT("STOCKNAME"));
        th.numSharesStr_      = q1.GetString(wxT("NUMSHARES"));
        th.numShares_         = q1.GetInt(wxT("NUMSHARES"));

        th.currentPrice_      = q1.GetDouble(wxT("CURRENTPRICE"));
        th.purchasePrice_      = q1.GetDouble(wxT("PURCHASEPRICE"));
        th.value_             = q1.GetDouble(wxT("VALUE"));
        double commission     = q1.GetDouble(wxT("COMMISSION"));

        th.gainLoss_        = th.value_ - ((th.numShares_ * th.purchasePrice_) + commission);

        wxString tempString;
        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.gainLoss_, tempString))
            th.gainLossStr_ = tempString;

        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.value_, tempString))
            th.valueStr_ = tempString;

        trans_.push_back(th);
        ct++;
    }
    q1.Finalize();
    listCtrlAccount_->SetItemCount(ct);

    mmENDSQL_LITE_EXCEPTION;
}

void mmStocksPanel::OnDeleteStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnDeleteStocks(event);
}

void mmStocksPanel::OnNewStocks(wxCommandEvent& event)
{
  listCtrlAccount_->OnNewStocks(event);
}

void mmStocksPanel::OnEditStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnEditStocks(event);
}

/*******************************************************/

void stocksListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Stock Investment"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Stock Investment"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Stock Investment"));
    PopupMenu(&menu, event.GetPoint());
}

wxString mmStocksPanel::getItem(long item, long column)
{
    if (column == 0)
        return trans_[item].heldAt_;

    if (column == 1)
        return trans_[item].shareName_;

    if (column == 2)
        return trans_[item].numSharesStr_;

    if (column == 3)
        return trans_[item].gainLossStr_;

    if (column == 4)
        return trans_[item].valueStr_;

    return wxT("");
}

wxString stocksListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

void stocksListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
}

int stocksListCtrl::OnGetItemImage(long item) const
{
   /* Returns the icon to be shown for each entry */
   if (cp_->trans_[item].gainLoss_ > 0)
       return 0;
   return 1;
}

wxListItemAttr* stocksListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void stocksListCtrl::OnListKeyDown(wxListEvent& event)
{
  switch ( event.GetKeyCode() )
    {
        case WXK_DELETE:
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, 
                    MENU_TREEPOPUP_DELETE);
                OnDeleteStocks(evt);
            }
            break;

        default:
            event.Skip();
    }
}

void stocksListCtrl::OnNewStocks(wxCommandEvent& event)
{
    mmStockDialog *dlg = new mmStockDialog(cp_->db_, 0, false, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void stocksListCtrl::OnDeleteStocks(wxCommandEvent& event)
{
	if (selectedIndex_ == -1)
		return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the stock investment?"),
                                        _("Confirm Stock Investment Deletion"),
                                        wxYES_NO);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        mmDBWrapper::deleteStockInvestment(cp_->db_, cp_->trans_[selectedIndex_].stockID_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl();
    }
}

void stocksListCtrl::OnEditStocks(wxCommandEvent& event)
{
	if (selectedIndex_ == -1)
		return;
    mmStockDialog *dlg = new mmStockDialog(cp_->db_, 
        cp_->trans_[selectedIndex_].stockID_, true, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void stocksListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    mmStockDialog *dlg = new mmStockDialog(cp_->db_, 
        cp_->trans_[selectedIndex_].stockID_, true, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();

}

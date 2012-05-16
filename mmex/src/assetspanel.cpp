/*******************************************************
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

#include "assetspanel.h"
#include "util.h"
#include "dbwrapper.h"
#include "assetdialog.h"
#include <boost/foreach.hpp>

/* Include XPM Support */
#include "../resources/assets.xpm"
enum 
{ 
  IDC_PANEL_ASSETS_LISTCTRL = wxID_HIGHEST + 1,
  IDC_PANEL_CHECKING_STATIC_BALHEADER,
  IDC_PANEL_CHECKING_STATIC_BAL,
  MENU_TREEPOPUP_NEW,
  MENU_TREEPOPUP_EDIT,
  MENU_TREEPOPUP_DELETE,
  IDC_PANEL_ASSET_STATIC_DETAILS,
  IDC_PANEL_ASSET_STATIC_DETAILS_MINI
};
enum EColumn
{ 
    COL_NAME = 0, 
    COL_DATE,
    COL_TYPE, 
    COL_VALUE,
    COL_NOTES,
    COL_MAX, // number of columns
};

/*******************************************************/
BEGIN_EVENT_TABLE(mmAssetsPanel, wxPanel)
    EVT_BUTTON(wxID_NEW, mmAssetsPanel::OnNewAsset)
    EVT_BUTTON(wxID_EDIT, mmAssetsPanel::OnEditAsset)
    EVT_BUTTON(wxID_DELETE, mmAssetsPanel::OnDeleteAsset)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(assetsListCtrl, mmListCtrl)
    EVT_LIST_ITEM_ACTIVATED(IDC_PANEL_ASSETS_LISTCTRL,   assetsListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(IDC_PANEL_ASSETS_LISTCTRL, assetsListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(IDC_PANEL_ASSETS_LISTCTRL,    assetsListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(IDC_PANEL_ASSETS_LISTCTRL,    assetsListCtrl::OnListItemDeselected)
    EVT_LIST_COL_CLICK(IDC_PANEL_ASSETS_LISTCTRL,       assetsListCtrl::OnColClick)
        
    EVT_MENU(MENU_TREEPOPUP_NEW, assetsListCtrl::OnNewAsset)
    EVT_MENU(MENU_TREEPOPUP_EDIT, assetsListCtrl::OnEditAsset)
    EVT_MENU(MENU_TREEPOPUP_DELETE, assetsListCtrl::OnDeleteAsset)
    
    EVT_LIST_KEY_DOWN(IDC_PANEL_ASSETS_LISTCTRL, assetsListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/

mmAssetsPanel::mmAssetsPanel(wxWindow *parent, wxSQLite3Database* db, wxSQLite3Database* inidb, mmCoreDB* core) : 
    mmPanelBase(db, inidb, core)
{
    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxPanelNameStr);
}

bool mmAssetsPanel::Create(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);

    if (!wxPanel::Create(parent, winid, pos, size, style, name)) return false;

    Freeze();
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    
    initVirtualListControl(0, true);
    if (!all_assets_.empty())
        m_listCtrlAssets->EnsureVisible(all_assets_.size() - 1);

    Thaw();
    return true;
}

mmAssetsPanel::~mmAssetsPanel()
{
    try {
        destroy();
    } catch (...) {
        wxASSERT(false);
    }
}

void mmAssetsPanel::destroy()
{
    inidb_->Begin();
    for (int i = 0; i < COL_MAX-1; ++i) 
    {
        int width = m_listCtrlAssets->GetColumnWidth(i);
        mmDBWrapper::setINISettingValue(inidb_, wxString::Format(wxT("ASSETS_COL%d_WIDTH"), i), wxString() << width); 
    }
    inidb_->Commit();
}

void mmAssetsPanel::CreateControls()
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

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, wxID_ANY, _("Assets"),
        wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxGetEmptyString()));

    wxStaticText* itemStaticText10 = new wxStaticText( headerPanel, IDC_PANEL_CHECKING_STATIC_BALHEADER, _("Total:"),
        wxDefaultPosition, wxDefaultSize, 0 );

    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);
    itemBoxSizerVHeader->Add(itemStaticText10, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( this, IDC_PANEL_CHECKING_STATIC_BAL,
        wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    m_listCtrlAssets = new assetsListCtrl( this, itemSplitterWindow10, 
        IDC_PANEL_ASSETS_LISTCTRL, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    m_listCtrlAssets->m_imageList->Add(wxBitmap(assets_xpm));

    m_listCtrlAssets->InsertColumn(COL_NAME, _("Name"), wxLIST_FORMAT_RIGHT);
    m_listCtrlAssets->InsertColumn(COL_TYPE, _("Type"), wxLIST_FORMAT_RIGHT);
    m_listCtrlAssets->InsertColumn(COL_VALUE, _("Value"), wxLIST_FORMAT_RIGHT);
    m_listCtrlAssets->InsertColumn(COL_DATE, _("Date"), wxLIST_FORMAT_RIGHT);
    m_listCtrlAssets->InsertColumn(COL_NOTES, _("Notes"));

    /* See if we can get data from inidb */
    long col0, col1, col2, col3, col4;
    mmDBWrapper::getINISettingValue(inidb_, wxT("ASSETS_COL0_WIDTH"), wxT("150")).ToLong(&col0); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("ASSETS_COL1_WIDTH"), wxT("-2")).ToLong(&col1); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("ASSETS_COL2_WIDTH"), wxT("-2")).ToLong(&col2); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("ASSETS_COL3_WIDTH"), wxT("-2")).ToLong(&col3); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("ASSETS_COL4_WIDTH"), wxT("450")).ToLong(&col4); 
     
    m_listCtrlAssets->SetColumnWidth(COL_NAME, col0);
    m_listCtrlAssets->SetColumnWidth(COL_TYPE, col1);
    m_listCtrlAssets->SetColumnWidth(COL_VALUE, col2);
    m_listCtrlAssets->SetColumnWidth(COL_DATE, col3);
    m_listCtrlAssets->SetColumnWidth(COL_NOTES, col4);
    
    wxPanel* assets_panel = new wxPanel( itemSplitterWindow10, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

    itemSplitterWindow10->SplitHorizontally(m_listCtrlAssets, assets_panel);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    assets_panel->SetSizer(itemBoxSizer4);
    //assets_panel->SetBackgroundColour(mmColors::listBackColor);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 3);

    wxButton* itemButton6 = new wxButton( assets_panel, wxID_NEW, _("&New"));
    itemButton6->SetToolTip(_("New Asset"));
    itemBoxSizer5->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    wxButton* itemButton81 = new wxButton( assets_panel, wxID_EDIT, _("&Edit"));
    itemButton81->SetToolTip(_("Edit Asset"));
    itemBoxSizer5->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton81->Enable(false);
    
    wxButton* itemButton7 = new wxButton( assets_panel, wxID_DELETE, _("&Delete"));
    itemButton7->SetToolTip(_("Delete Asset"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton7->Enable(false);

    //Infobar-mini 
    wxStaticText* itemStaticText44 = new wxStaticText( assets_panel, IDC_PANEL_ASSET_STATIC_DETAILS_MINI, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer5->Add(itemStaticText44, 1, wxGROW|wxTOP, 12);

    //Infobar 
    wxStaticText* itemStaticText33 = new wxStaticText( assets_panel, 
        IDC_PANEL_ASSET_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxSize(200,-1), wxTE_MULTILINE|wxTE_WORDWRAP);
    itemBoxSizer4->Add(itemStaticText33, 1, wxGROW|wxLEFT|wxRIGHT, 14);
            
    updateExtraAssetData(-1);
}

void mmAssetsPanel::initVirtualListControl(int col, bool asc)
{
    mmDBWrapper::loadBaseCurrencySettings(db_);

    double total = mmDBWrapper::getAssetBalance(db_);
    wxString balance;
    mmex::formatDoubleToCurrency(total, balance);
    wxStaticText* header = (wxStaticText*)FindWindow(IDC_PANEL_CHECKING_STATIC_BALHEADER);
    wxASSERT(header);
    wxString lbl  = wxString::Format(_("Total: %s"), balance.c_str());
    header->SetLabel(lbl);

    all_assets_.clear();
    DB_View_ASSETS_V1::COLUMN column = DB_View_ASSETS_V1::COL_ASSETID;
    if (col == COL_NAME) column = DB_View_ASSETS_V1::COL_ASSETNAME;
    else if (col == COL_TYPE) column = DB_View_ASSETS_V1::COL_ASSETTYPE;
    else if (col == COL_VALUE) column = DB_View_ASSETS_V1::COL_VALUE;
    else if (col == COL_DATE) column = DB_View_ASSETS_V1::COL_STARTDATE;
    else if (col == COL_NOTES) column = DB_View_ASSETS_V1::COL_NOTES;

    m_listCtrlAssets->SetColumnImage(col, asc ? ICON_ASC : ICON_DESC);
    all_assets_ = ASSETS_V1.all(db_, column, asc);
    m_listCtrlAssets->SetItemCount(all_assets_.size());
}

void mmAssetsPanel::OnDeleteAsset(wxCommandEvent& event)
{
    m_listCtrlAssets->OnDeleteAsset(event);
}

void mmAssetsPanel::OnNewAsset(wxCommandEvent& event)
{
    m_listCtrlAssets->OnNewAsset(event);
}

void mmAssetsPanel::OnEditAsset(wxCommandEvent& event)
{
    m_listCtrlAssets->OnEditAsset(event);
}

/*******************************************************/

void assetsListCtrl::OnItemRightClick(wxListEvent& event)
{
    m_selectedIndex = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Asset"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Asset"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Asset"));
    PopupMenu(&menu, event.GetPoint());
}

wxString mmAssetsPanel::getItem(long item, long column)
{
    const DB_View_ASSETS_V1::Data& asset = all_assets_.at(item);

    if (column == COL_NAME)  return asset.ASSETNAME;
    if (column == COL_TYPE)  return wxGetTranslation(asset.ASSETTYPE);
    if (column == COL_VALUE) 
    {
        double val = mmDBWrapper::getAssetValue(asset);
        wxString val_str;
        mmex::formatDoubleToCurrencyEdit(val, val_str);
        return val_str;
    }
    if (column == COL_DATE)  
    {
        wxDateTime dt;
        dt.ParseDate(asset.STARTDATE);
        return dt.Format(mmOptions::instance().dateFormat);
    }
    if (column == COL_NOTES) return asset.NOTES;

    return wxGetEmptyString();
}

wxString assetsListCtrl::OnGetItemText(long item, long column) const
{
    return m_cp->getItem(item, column);
}

void assetsListCtrl::OnListItemSelected(wxListEvent& event)
{
    m_selectedIndex = event.GetIndex();
    m_cp->updateExtraAssetData(m_selectedIndex);
}

void assetsListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
    m_selectedIndex = -1;
    m_cp->updateExtraAssetData(m_selectedIndex);
}
//----------------------------------------------------------------------------

void mmAssetsPanel::updateExtraAssetData(int selIndex)
{
    wxStaticText* st = (wxStaticText*)FindWindow(IDC_PANEL_ASSET_STATIC_DETAILS);
    wxStaticText* stm = (wxStaticText*)FindWindow(IDC_PANEL_ASSET_STATIC_DETAILS_MINI);
    if (selIndex > -1 && selIndex < all_assets_.size())
    {
        const DB_View_ASSETS_V1::Data& asset = all_assets_.at(selIndex);
        enableEditDeleteButtons(true);
        wxString miniInfo;
        miniInfo << wxT("\t") << _("Change in Value") << wxT(": ") << asset.VALUECHANGE;
        if (asset.VALUECHANGE != _("None"))
            miniInfo<< wxT(" = ") << asset.VALUECHANGERATE << wxT("%");
        st->SetLabel(asset.NOTES);
        stm->SetLabel(miniInfo);
    }
    else
    {
        stm->SetLabel(wxT(""));
        st->SetLabel(Tips(TIPS_ASSETS));
        enableEditDeleteButtons(false);
    }
}

void mmAssetsPanel::enableEditDeleteButtons(bool enable)
{
    wxButton* btn = static_cast<wxButton*>(FindWindow(wxID_EDIT));
    wxASSERT(btn);
    btn->Enable(enable);

    btn = static_cast<wxButton*>(FindWindow(wxID_DELETE));
    wxASSERT(btn);
    btn->Enable(enable);
}

int assetsListCtrl::OnGetItemImage(long /*item*/) const
{
    return this->m_imageList->GetImageCount() - 1; // last one
}

wxListItemAttr* assetsListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&m_attr2 : (wxListItemAttr *)&m_attr1;
}

void assetsListCtrl::OnListKeyDown(wxListEvent& event)
{
    if (event.GetKeyCode() == WXK_DELETE) 
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_DELETE);
        OnDeleteAsset(evt);
    } 
    else 
    {
        event.Skip();
    }
}

void assetsListCtrl::OnNewAsset(wxCommandEvent& /*event*/)
{
    mmAssetDialog dlg(this, m_cp->getDb(), 0, false);
    if (dlg.ShowModal() == wxID_OK)    doRefreshItems();
}

void assetsListCtrl::doRefreshItems()
{
    m_cp->initVirtualListControl(m_selected_col, m_asc);
    RefreshItems(0, m_cp->all_assets_.empty() ? 0: m_cp->all_assets_.size() - 1);
}
 
void assetsListCtrl::OnDeleteAsset(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex == -1)    return;
    if (m_cp->all_assets_.empty()) return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the Asset?"), _("Confirm Asset Deletion"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES) 
    {
        DB_View_ASSETS_V1::Data& asset = m_cp->all_assets_[m_selectedIndex];
        asset.remove(m_cp->getDb());
        DeleteItem(m_selectedIndex);
        m_cp->initVirtualListControl(m_selected_col, m_asc);
        
        //if (m_cp->getTrans().empty()) //after delition of asset - index and extra panel should be erased
        {
            m_selectedIndex = -1;
            m_cp->updateExtraAssetData(m_selectedIndex);
        }
    }
}

void assetsListCtrl::OnColClick(wxListEvent& event)
{
    m_asc = !m_asc;
    if(0 <= event.GetColumn() && event.GetColumn() < COL_MAX)
        m_selected_col = event.GetColumn();

    doRefreshItems();
}

void assetsListCtrl::OnEditAsset(wxCommandEvent& /*event*/)
{
    if (m_selectedIndex == -1)     return;

    const DB_View_ASSETS_V1::Data& asset = m_cp->all_assets_.at(m_selectedIndex);
    mmAssetDialog dlg(this, m_cp->getDb(), asset.id(), true);

    if (dlg.ShowModal() == wxID_OK) doRefreshItems();
}

void assetsListCtrl::OnListItemActivated(wxListEvent& event)
{
    m_selectedIndex = event.GetIndex();

    const DB_View_ASSETS_V1::Data& asset = m_cp->all_assets_.at(m_selectedIndex);
    mmAssetDialog dlg(this, m_cp->getDb(), asset.id(), true);

    if (dlg.ShowModal() == wxID_OK) doRefreshItems();
}

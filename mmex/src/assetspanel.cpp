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

#include <algorithm>
#include <vector>

/* Include XPM Support */
#include "../resources/assets.xpm"

/*******************************************************/
BEGIN_EVENT_TABLE(mmAssetsPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_NEW_ASSET,         mmAssetsPanel::OnNewAsset)
    EVT_BUTTON(ID_BUTTON_EDIT_ASSET,        mmAssetsPanel::OnEditAsset)
    EVT_BUTTON(ID_BUTTON_DELETE_ASSET,      mmAssetsPanel::OnDeleteAsset)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(assetsListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_STOCKS_LISTCTRL,   assetsListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_STOCKS_LISTCTRL, assetsListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_STOCKS_LISTCTRL,    assetsListCtrl::OnListItemSelected)
	EVT_LIST_ITEM_DESELECTED(ID_PANEL_STOCKS_LISTCTRL,    assetsListCtrl::OnListItemDeselected)
        
    EVT_MENU(MENU_TREEPOPUP_NEW,              assetsListCtrl::OnNewAsset)
    EVT_MENU(MENU_TREEPOPUP_EDIT,             assetsListCtrl::OnEditAsset)
    EVT_MENU(MENU_TREEPOPUP_DELETE,           assetsListCtrl::OnDeleteAsset)
    
    EVT_LIST_KEY_DOWN(ID_PANEL_ASSETS_LISTCTRL,   assetsListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/
mmAssetsPanel::mmAssetsPanel(wxSQLite3Database* db, wxSQLite3Database* inidb,
            wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name )
            : db_(db), inidb_(inidb), m_imageList(0)
{
    
    Create(parent, winid, pos, size, style, name);
}

bool mmAssetsPanel::Create( wxWindow *parent,
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
        listCtrlAssets_->EnsureVisible(((int)trans_.size()) - 1);

    this->Thaw();
    return TRUE;
}

mmAssetsPanel::~mmAssetsPanel()
{
   if (m_imageList)
        delete m_imageList;

    long col0, col1, col2;
    col0 = listCtrlAssets_->GetColumnWidth(0);
    col1 = listCtrlAssets_->GetColumnWidth(1);
    col2 = listCtrlAssets_->GetColumnWidth(2);

    wxString col0Str = wxString::Format(wxT("%d"), col0);
    wxString col1Str = wxString::Format(wxT("%d"), col1);
    wxString col2Str = wxString::Format(wxT("%d"), col2);
    
    mmDBWrapper::setINISettingValue(inidb_, wxT("ASSETS_COL0_WIDTH"), col0Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("ASSETS_COL1_WIDTH"), col1Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("ASSETS_COL2_WIDTH"), col2Str); 
}

void mmAssetsPanel::CreateControls()
{    
    mmAssetsPanel* itemPanel8 = this;

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemPanel8->SetSizer(itemBoxSizer9);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( itemPanel8, ID_PANEL15, wxDefaultPosition, 
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, ID_PANEL_BD_STATIC_HEADER, 
        _("Assets"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, 
        wxT("")));

	wxStaticText* itemStaticText10 = new wxStaticText( headerPanel, 
            ID_PANEL_CHECKING_STATIC_BALHEADER, 
			_("Total:"), wxDefaultPosition, wxSize(500, 20), 0 );

    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);
	itemBoxSizerVHeader->Add(itemStaticText10, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( itemPanel8, 
        ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), 
        wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(assets_xpm));

    listCtrlAssets_ = new assetsListCtrl( this, itemSplitterWindow10, 
        ID_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    listCtrlAssets_->SetBackgroundColour(mmColors::listBackColor);
    listCtrlAssets_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlAssets_->InsertColumn(0, _("Name"));
    wxListItem itemCol;
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    itemCol.SetText(_("Type"));
    listCtrlAssets_->InsertColumn(1, itemCol);
	
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Value"));
    
    listCtrlAssets_->InsertColumn(2, itemCol);

    /* See if we can get data from inidb */
     long col0, col1, col2;
     mmDBWrapper::getINISettingValue(inidb_, 
        wxT("ASSETS_COL0_WIDTH"), wxT("150")).ToLong(&col0); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("ASSETS_COL1_WIDTH"), wxT("-2")).ToLong(&col1); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("ASSETS_COL2_WIDTH"), wxT("-2")).ToLong(&col2); 
     
    listCtrlAssets_->SetColumnWidth(0, col0);
    listCtrlAssets_->SetColumnWidth(1, col1);
    listCtrlAssets_->SetColumnWidth(2, col2);
    
    wxPanel* itemPanel12 = new wxPanel( itemSplitterWindow10, ID_PANEL1, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

    itemSplitterWindow10->SplitHorizontally(listCtrlAssets_, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);
    itemPanel12->SetBackgroundColour(mmColors::listDetailsPanelColor);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton6 = new wxButton( itemPanel12, ID_BUTTON_NEW_ASSET, _("&New"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton6->SetToolTip(_("New Asset"));
    itemBoxSizer5->Add(itemButton6, 0, wxGROW|wxALL, 1);

    wxButton* itemButton81 = new wxButton( itemPanel12, ID_BUTTON_EDIT_ASSET, _("&Edit"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton81->SetToolTip(_("Edit Asset"));
    itemBoxSizer5->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
	itemButton81->Enable(false);
	
    wxButton* itemButton7 = new wxButton( itemPanel12, ID_BUTTON_DELETE_ASSET, _("&Delete"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton7->SetToolTip(_("Delete Asset"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
	itemButton7->Enable(false);
	
    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel12, 
        ID_PANEL_CHECKING_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    itemBoxSizer4->Add(itemStaticText11, 1, wxGROW|wxALL, 5);
}

void mmAssetsPanel::initVirtualListControl()
{
    /* Clear all the records */
    trans_.clear();

    mmDBWrapper::loadBaseCurrencySettings(db_);

    double total = mmDBWrapper::getAssetBalance(db_);
    wxString balance;
    mmCurrencyFormatter::formatDoubleToCurrency(total, balance);
    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);
    wxString lbl  = wxString::Format(_("Total: %s"), balance.c_str());
    header->SetLabel(lbl);

    static const char sql[] = 
    "select ASSETID, "
           "ASSETNAME, "
           "ASSETTYPE "
    "from ASSETS_V1";

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    long cnt = 0;
    
    for ( ; q1.NextRow(); ++cnt)
    {
        mmAssetHolder th;

        th.assetID_ = q1.GetInt(wxT("ASSETID"));
        th.value_ = mmDBWrapper::getAssetValue(db_, th.assetID_);
        th.assetName_ = q1.GetString(wxT("ASSETNAME"));

        wxString assetTypeStr = q1.GetString(wxT("ASSETTYPE"));
        th.assetType_ =  wxGetTranslation(assetTypeStr); // string should be marked for translation

        wxString tempString;
        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.value_, tempString))
            th.valueStr_ = tempString;

        trans_.push_back(th);
    }

    listCtrlAssets_->SetItemCount(cnt);
    q1.Finalize();
}

void mmAssetsPanel::OnDeleteAsset(wxCommandEvent& event)
{
    listCtrlAssets_->OnDeleteAsset(event);
}

void mmAssetsPanel::OnNewAsset(wxCommandEvent& event)
{
  listCtrlAssets_->OnNewAsset(event);
}

void mmAssetsPanel::OnEditAsset(wxCommandEvent& event)
{
    listCtrlAssets_->OnEditAsset(event);
}

/*******************************************************/

void assetsListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Asset"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Asset"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Asset"));
    PopupMenu(&menu, event.GetPoint());
}

wxString mmAssetsPanel::getItem(long item, long column)
{
    if (column == 0)
        return trans_[item].assetName_;

    if (column == 1)
        return trans_[item].assetType_;

    if (column == 2)
        return trans_[item].valueStr_;

    return wxT("");
}

wxString assetsListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

void assetsListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
	cp_->enableEditDeleteButtons(true);
}

void assetsListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
    selectedIndex_ = -1;
	cp_->enableEditDeleteButtons(false);
}

void mmAssetsPanel::enableEditDeleteButtons(bool en)
{
	wxButton* bE = (wxButton*)FindWindow(ID_BUTTON_EDIT_ASSET);
	wxButton* bD = (wxButton*)FindWindow(ID_BUTTON_DELETE_ASSET);
	bE->Enable(en);
	bD->Enable(en);
}

int assetsListCtrl::OnGetItemImage(long /*item*/) const
{
   /* Returns the icon to be shown for each entry */
   return 0;
}

wxListItemAttr* assetsListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void assetsListCtrl::OnListKeyDown(wxListEvent& event)
{
  switch ( event.GetKeyCode() )
    {
        case WXK_DELETE:
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, 
                    MENU_TREEPOPUP_DELETE);
                OnDeleteAsset(evt);
            }
            break;

        default:
            event.Skip();
    }
}

void assetsListCtrl::OnNewAsset(wxCommandEvent& /*event*/)
{
    mmAssetDialog *dlg = new mmAssetDialog(cp_->db_, 0, false, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();
}

void assetsListCtrl::OnDeleteAsset(wxCommandEvent& /*event*/)
{
	if (selectedIndex_ == -1)
		return;

    if (cp_->trans_.size() == 0)
        return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the Asset?"),
                                        _("Confirm Asset Deletion"),
                                        wxYES_NO);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        mmDBWrapper::deleteAsset(cp_->db_, cp_->trans_[selectedIndex_].assetID_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl();
        if (cp_->trans_.size() == 0)
        {
            selectedIndex_ = -1;
        }
    }
}

void assetsListCtrl::OnEditAsset(wxCommandEvent& /*event*/)
{
	if (selectedIndex_ == -1)
		return;
    mmAssetDialog *dlg = new mmAssetDialog(cp_->db_, 
        cp_->trans_[selectedIndex_].assetID_, true, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();

}

void assetsListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    mmAssetDialog *dlg = new mmAssetDialog(cp_->db_, 
        cp_->trans_[selectedIndex_].assetID_, true, this );
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
    dlg->Destroy();

}

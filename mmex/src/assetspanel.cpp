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
#include "guiid.h"
#include "dbwrapper.h"
#include "assetdialog.h"
#include "constants.h"


namespace
{

    enum EColumn
    {
        COL_NAME,
        COL_DATE,
        COL_TYPE,
        COL_VALUE,
        COL_NOTES,
        COL_MAX, // number of columns
    };

} // namespace

/*******************************************************/
BEGIN_EVENT_TABLE(mmAssetsPanel, wxPanel)
    EVT_BUTTON(wxID_NEW, mmAssetsPanel::OnNewAsset)
    EVT_BUTTON(wxID_EDIT, mmAssetsPanel::OnEditAsset)
    EVT_BUTTON(wxID_DELETE, mmAssetsPanel::OnDeleteAsset)
    EVT_MENU(wxID_ANY, mmAssetsPanel::OnViewPopupSelected)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(assetsListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(IDC_PANEL_STOCKS_LISTCTRL,   assetsListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(IDC_PANEL_STOCKS_LISTCTRL, assetsListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(IDC_PANEL_STOCKS_LISTCTRL,    assetsListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(IDC_PANEL_STOCKS_LISTCTRL,  assetsListCtrl::OnListItemDeselected)
    EVT_LIST_COL_END_DRAG(IDC_PANEL_STOCKS_LISTCTRL,     assetsListCtrl::OnItemResize)
    EVT_LIST_COL_CLICK(IDC_PANEL_STOCKS_LISTCTRL,        assetsListCtrl::OnColClick)

    EVT_MENU(MENU_TREEPOPUP_NEW, assetsListCtrl::OnNewAsset)
    EVT_MENU(MENU_TREEPOPUP_EDIT, assetsListCtrl::OnEditAsset)
    EVT_MENU(MENU_TREEPOPUP_DELETE, assetsListCtrl::OnDeleteAsset)

    EVT_LIST_KEY_DOWN(wxID_ANY, assetsListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/

mmAssetsPanel::mmAssetsPanel(wxWindow *parent, mmCoreDB* core)
: mmPanelBase(NULL, core)
{
    Create(parent, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxPanelNameStr);
}

bool mmAssetsPanel::Create(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);

    if (!wxPanel::Create(parent, winid, pos, size, style, name)) return false;

    windowsFreezeThaw(this);

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    m_listCtrlAssets->InitVariables();
    initVirtualListControl();
    if (!m_trans.empty())
        m_listCtrlAssets->EnsureVisible(static_cast<long>(m_trans.size()) - 1);

    windowsFreezeThaw(this);

    return true;
}

mmAssetsPanel::~mmAssetsPanel()
{
}

void assetsListCtrl::OnItemResize(wxListEvent& event)
{
    int i = event.GetColumn();
    int width = cp_->m_listCtrlAssets->GetColumnWidth(i);
    cp_->core_->iniSettings_->SetIntSetting(wxString::Format(wxT("ASSETS_COL%d_WIDTH"), i), width);
}

void assetsListCtrl::InitVariables()
{
    m_selected_col = 0;
    m_asc = true;
    cp_->filter_ = wxT(" 'Property','Automobile','Household Object','Art','Jewellery','Cash','Other' ");
}

void mmAssetsPanel::CreateControls()
{
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer9);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxGROW, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    int font_size = this->GetFont().GetPointSize() + 2;

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, wxID_STATIC, _("Assets"));
    itemStaticText9->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);

    wxBoxSizer* itemBoxSizerHHeader2 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader2);

    wxBitmap itemStaticBitmap(rightarrow_xpm);
    wxStaticBitmap* itemStaticBitmap3 = new wxStaticBitmap( headerPanel, wxID_STATIC, itemStaticBitmap);
    itemBoxSizerHHeader2->Add(itemStaticBitmap3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    //itemStaticBitmap3->Connect(ID_PANEL_CHECKING_STATIC_BITMAP_VIEW, wxEVT_RIGHT_DOWN, wxMouseEventHandler(mmAssetsPanel::OnFilterResetToViewAll), NULL, this);
    itemStaticBitmap3->Connect(wxID_STATIC, wxEVT_LEFT_DOWN, wxMouseEventHandler(mmAssetsPanel::OnMouseLeftDown), NULL, this);

    itemStaticTextMainFilter_ = new wxStaticText( headerPanel, wxID_STATIC, _("All"));
    itemBoxSizerHHeader2->Add(itemStaticTextMainFilter_, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxGROW, 5);

    header_text_ = new wxStaticText( headerPanel, wxID_STATIC, _("Total:"));
    itemBoxSizerVHeader->Add(header_text_, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( this, wxID_STATIC,
        wxDefaultPosition, wxSize(200, 200), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList.reset(new wxImageList(imageSize.GetWidth(), imageSize.GetHeight()));
    m_imageList->Add(wxBitmap(wxImage(assets_xpm).Scale(16, 16)));
    //TODO: Provide more icons
    //m_imageList->Add(wxBitmap(wxImage(car_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(assets_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(assets_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(assets_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(assets_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(assets_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(assets_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(uparrow_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(downarrow_xpm).Scale(16, 16)));

    m_listCtrlAssets = new assetsListCtrl( this, itemSplitterWindow10,
        IDC_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );

    //m_imageList.get()->Add(wxBitmap(assets_xpm));
    m_listCtrlAssets->SetImageList(m_imageList.get(), wxIMAGE_LIST_SMALL);

    m_listCtrlAssets->InsertColumn(COL_NAME, _("Name"));

    wxListItem itemCol;
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    itemCol.SetText(_("Type"));
    m_listCtrlAssets->InsertColumn(COL_TYPE, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Value"));
    m_listCtrlAssets->InsertColumn(COL_VALUE, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Date"));
    m_listCtrlAssets->InsertColumn(COL_DATE, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    itemCol.SetText(_("Notes"));
    m_listCtrlAssets->InsertColumn(COL_NOTES, itemCol);

    /* See if we can get data from inidb */
    int col0 = core_->iniSettings_->GetIntSetting(wxT("ASSETS_COL0_WIDTH"), 150);
    int col1 = core_->iniSettings_->GetIntSetting(wxT("ASSETS_COL1_WIDTH"), -2);
    int col2 = core_->iniSettings_->GetIntSetting(wxT("ASSETS_COL2_WIDTH"), -2);
    int col3 = core_->iniSettings_->GetIntSetting(wxT("ASSETS_COL3_WIDTH"), -2);
    int col4 = core_->iniSettings_->GetIntSetting(wxT("ASSETS_COL4_WIDTH"), 450);

    m_listCtrlAssets->SetColumnWidth(COL_NAME, col0);
    m_listCtrlAssets->SetColumnWidth(COL_DATE, col1);
    m_listCtrlAssets->SetColumnWidth(COL_TYPE, col2);
    m_listCtrlAssets->SetColumnWidth(COL_VALUE, col3);
    m_listCtrlAssets->SetColumnWidth(COL_NOTES, col4);

    wxPanel* assets_panel = new wxPanel( itemSplitterWindow10, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

    itemSplitterWindow10->SplitHorizontally(m_listCtrlAssets, assets_panel);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    assets_panel->SetSizer(itemBoxSizer4);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 3);

    wxButton* itemButton6 = new wxButton( assets_panel, wxID_NEW);
    itemButton6->SetToolTip(_("New Asset"));
    itemBoxSizer5->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    wxButton* itemButton81 = new wxButton( assets_panel, wxID_EDIT);
    itemButton81->SetToolTip(_("Edit Asset"));
    itemBoxSizer5->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton81->Enable(false);

    wxButton* itemButton7 = new wxButton( assets_panel, wxID_DELETE);
    itemButton7->SetToolTip(_("Delete Asset"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton7->Enable(false);

    //Infobar-mini
    wxStaticText* itemStaticText44 = new wxStaticText( assets_panel, IDC_PANEL_ASSET_STATIC_DETAILS_MINI, wxT(""));
    itemBoxSizer5->Add(itemStaticText44, 1, wxGROW|wxTOP, 12);

    //Infobar
    wxStaticText* itemStaticText33 = new wxStaticText( assets_panel,
        IDC_PANEL_ASSET_STATIC_DETAILS, wxT(""), wxDefaultPosition, wxSize(200,-1), wxTE_MULTILINE|wxTE_WORDWRAP);
    itemBoxSizer4->Add(itemStaticText33, 1, wxGROW|wxLEFT|wxRIGHT, 14);

    updateExtraAssetData(-1);
}

int mmAssetsPanel::initVirtualListControl(int id, int col, bool asc)
{
    /* Clear all the records */
    m_trans.clear();
    m_listCtrlAssets->DeleteAllItems();

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(asc ? 8 : 7);

    m_listCtrlAssets->SetColumn(col, item);

    core_->currencyList_.LoadBaseCurrencySettings();

    const  wxString sql = wxString::FromUTF8(SELECT_ALL_FROM_ASSETS_V1)
        + wxString::Format(wxT(" where ASSETTYPE in ( %s ) "), filter_.c_str())
        + wxT(" order by ") + (wxString()<<col+1)
        + (!asc ? wxT(" desc") : wxT(" "));

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(sql);

    int cnt = 0, selected_item = -1;
    double total = 0.0;

    for (; q1.NextRow(); ++cnt)
    {
        mmAssetHolder th;

        th.id_ = q1.GetInt(wxT("ASSETID"));
        th.assetName_ = q1.GetString(wxT("ASSETNAME"));

        wxString dateString = q1.GetString(wxT("STARTDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
        th.assetDate_ = mmGetDateForDisplay(core_->db_.get(), dtdt);

        th.assetType_ =  q1.GetString(wxT("ASSETTYPE"));
        th.value_ = mmDBWrapper::getAssetValue(core_->db_.get(), th.id_);
        th.assetValueChange_ =  q1.GetString(wxT("VALUECHANGE"));

        th.assetNotes_ = q1.GetString(wxT("NOTES"));

        th.valueChange_ = q1.GetDouble(wxT("VALUECHANGERATE"));
        total += q1.GetDouble(wxT("VALUE"));

        mmex::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);
        mmex::formatDoubleToCurrencyEdit(th.valueChange_, th.valueChangeStr_);

        m_trans.push_back( new mmAssetHolder (th));

        if (th.id_ == id) selected_item = cnt;
    }

    m_listCtrlAssets->SetItemCount(cnt);
    q1.Finalize();

    wxString balance;
    mmex::formatDoubleToCurrencyEdit(total, balance);
    wxString lbl  = wxString::Format(_("Total: %s"), balance.c_str());
    header_text_->SetLabel(lbl);

    return selected_item;
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
    if (column == COL_NAME)  return m_trans[item]->assetName_;
    if (column == COL_TYPE)  return wxGetTranslation(m_trans[item]->assetType_);
    if (column == COL_VALUE) return m_trans[item]->valueStr_;
    if (column == COL_DATE)  return m_trans[item]->assetDate_;
    if (column == COL_NOTES) return m_trans[item]->assetNotes_;

    return wxGetEmptyString();
}

wxString assetsListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

void assetsListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    cp_->updateExtraAssetData(selectedIndex_);
}

void assetsListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
    selectedIndex_ = -1;
    cp_->updateExtraAssetData(selectedIndex_);
}
//----------------------------------------------------------------------------

void mmAssetsPanel::updateExtraAssetData(int selIndex)
{
    wxStaticText* st = (wxStaticText*)FindWindow(IDC_PANEL_ASSET_STATIC_DETAILS);
    wxStaticText* stm = (wxStaticText*)FindWindow(IDC_PANEL_ASSET_STATIC_DETAILS_MINI);
    if (selIndex > -1)
    {
        enableEditDeleteButtons(true);
        wxString miniInfo;

        miniInfo << wxT("\t") << _("Change in Value") << wxT(": ")
            << wxGetTranslation(m_trans[selIndex]->assetValueChange_);
        if (m_trans[selIndex]->assetValueChange_ != wxT("None"))
            miniInfo<< wxT(" = ") << m_trans[selIndex]->valueChange_ << wxT("%");
        st->SetLabel(m_trans[selIndex]->assetNotes_);
        stm->SetLabel(miniInfo);
    }
    else
    {
        stm -> SetLabel(wxT(""));
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

int assetsListCtrl::OnGetItemImage(long item) const
{
    int image_id = 0;
    size_t size = sizeof(ASSET_TYPE)/sizeof(wxString);
    for(size_t i = 0; i < size; ++i)
    {
        if (ASSET_TYPE[i] == OnGetItemText(item, COL_TYPE))
            image_id = i;
    }

    return item;
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
    mmAssetDialog dlg(this, cp_->core_, 0, false);
    if (dlg.ShowModal() == wxID_OK)
    {
        doRefreshItems(dlg.transID_);
    }
}

void assetsListCtrl::doRefreshItems(int trx_id)
{
    int selectedIndex = cp_->initVirtualListControl(trx_id, m_selected_col, m_asc);

    long cnt = static_cast<long>(cp_->getTrans().size());

    if (selectedIndex >= cnt || selectedIndex < 0)
        selectedIndex = m_asc ? cnt - 1 : 0;

    if (cnt>0)
        RefreshItems(0, cnt > 0 ? --cnt : 0);
    else
        selectedIndex = -1;

    if (selectedIndex >= 0 && cnt>0)
    {
        SetItemState(selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        SetItemState(selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        EnsureVisible(selectedIndex);
    }
    selectedIndex_ = selectedIndex;
}

void assetsListCtrl::OnDeleteAsset(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1)    return;
    if (cp_->getTrans().empty()) return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the Asset?"),
        _("Confirm Asset Deletion"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        mmDBWrapper::deleteAsset(cp_->core_->db_.get(), cp_->getTrans()[selectedIndex_]->id_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl(m_selected_col, m_asc);

        selectedIndex_ = -1;
        cp_->updateExtraAssetData(selectedIndex_);
    }
}

void assetsListCtrl::OnEditAsset(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ < 0)     return;

    wxListEvent evt(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, IDC_PANEL_STOCKS_LISTCTRL);
    AddPendingEvent(evt);
}

void assetsListCtrl::OnListItemActivated(wxListEvent& /*event*/)
{
    //selectedIndex_ = event.GetIndex();
    mmAssetDialog dlg(this, cp_->core_, cp_->getTrans()[selectedIndex_], true);

    if (dlg.ShowModal() == wxID_OK)
        doRefreshItems(dlg.transID_);

    cp_->updateExtraAssetData(selectedIndex_);
}

void assetsListCtrl::OnColClick(wxListEvent& event)
{
    if(0 > event.GetColumn() || event.GetColumn() >= COL_MAX) return;

    if (m_selected_col == event.GetColumn()) m_asc = !m_asc;

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(-1);
    cp_->m_listCtrlAssets->SetColumn(m_selected_col, item);

    m_selected_col = event.GetColumn();

    item.SetImage(m_asc ? 8 : 7);
    SetColumn(m_selected_col, item);

    int trx_id = -1;
    if (selectedIndex_>=0) trx_id = cp_->getTrans()[selectedIndex_]->id_;

    doRefreshItems(trx_id);
}

void mmAssetsPanel::OnMouseLeftDown ( wxMouseEvent& event )
{
    wxMenu* menu = new wxMenu;
    menu->Append(new wxMenuItem(menu, 0, wxGetTranslation(wxTRANSLATE("All"))));

    size_t size = sizeof(ASSET_TYPE)/sizeof(wxString);
    for(size_t i = 0; i < size; ++i)
    {
        wxMenuItem* menuItem = new wxMenuItem(menu, i+1, wxGetTranslation(ASSET_TYPE[i]));
        menu->Append(menuItem);
    }
    PopupMenu(menu);

    event.Skip();
}

void mmAssetsPanel::OnViewPopupSelected(wxCommandEvent& event)
{
    int evt =  event.GetId();

    filter_ = wxT("");
    wxString label;

    int size = sizeof(ASSET_TYPE)/sizeof(wxString);
    for(int i = 0; i < size; ++i)
    {
        if (evt == 0 || evt == i+1)
        {
            filter_ << wxT("'") << ASSET_TYPE[i] << wxT("'") << wxT(",");
            if (evt == i+1) label = ASSET_TYPE[i];
        }
    }
    filter_.RemoveLast(1);

    if (evt == 0)
        itemStaticTextMainFilter_->SetLabel(_("All"));
    else
        itemStaticTextMainFilter_->SetLabel(wxGetTranslation(label));

    int trx_id = -1;
    m_listCtrlAssets->doRefreshItems(trx_id);
    updateExtraAssetData(trx_id);

/*    core_->db_.get()->Begin();
    mmDBWrapper::setInfoSettingValue(core_->db_.get(),
         wxString::Format(wxT("ASSET_FILTER_ID_%ld"), (long)m_AccountID), m_currentView);
    core_->db_.get()->Commit();
*/
}

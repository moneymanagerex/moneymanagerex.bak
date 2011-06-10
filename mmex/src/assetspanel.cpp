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

namespace
{

enum { 
  IDC_BUTTON_NEW_ASSET = wxID_HIGHEST + 1,
  IDC_BUTTON_EDIT_ASSET,
  IDC_BUTTON_DELETE_ASSET,
  IDC_PANEL_STOCKS_LISTCTRL,
  IDC_PANEL_ASSETS_LISTCTRL,
  IDC_PANEL_CHECKING_STATIC_BALHEADER,
  MENU_TREEPOPUP_NEW,
  MENU_TREEPOPUP_EDIT,
  MENU_TREEPOPUP_DELETE,
  IDC_PANEL_ASSET_STATIC_DETAILS,
  IDC_PANEL_ASSET_STATIC_DETAILS_MINI
};
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
    EVT_BUTTON(IDC_BUTTON_NEW_ASSET, mmAssetsPanel::OnNewAsset)
    EVT_BUTTON(IDC_BUTTON_EDIT_ASSET, mmAssetsPanel::OnEditAsset)
    EVT_BUTTON(IDC_BUTTON_DELETE_ASSET, mmAssetsPanel::OnDeleteAsset)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(assetsListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(IDC_PANEL_STOCKS_LISTCTRL,   assetsListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(IDC_PANEL_STOCKS_LISTCTRL, assetsListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(IDC_PANEL_STOCKS_LISTCTRL,    assetsListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(IDC_PANEL_STOCKS_LISTCTRL,    assetsListCtrl::OnListItemDeselected)
        
    EVT_MENU(MENU_TREEPOPUP_NEW, assetsListCtrl::OnNewAsset)
    EVT_MENU(MENU_TREEPOPUP_EDIT, assetsListCtrl::OnEditAsset)
    EVT_MENU(MENU_TREEPOPUP_DELETE, assetsListCtrl::OnDeleteAsset)
    
    EVT_LIST_KEY_DOWN(IDC_PANEL_ASSETS_LISTCTRL, assetsListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/

mmAssetsPanel::mmAssetsPanel(wxWindow *parent, wxSQLite3Database* db, wxSQLite3Database* inidb)	: 
	m_db(db), 
	m_inidb(inidb)
{
	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxPanelNameStr);
}

bool mmAssetsPanel::Create(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);

    if (!wxPanel::Create(parent, winid, pos, size, style, name)) {
    	return false;
    }

    Freeze();
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    
    initVirtualListControl();
    if (!m_trans.empty()) {
        m_listCtrlAssets->EnsureVisible(static_cast<long>(m_trans.size()) - 1);
    }

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
    mmDBWrapper::begin(m_inidb);
    for (int i = 0; i < COL_MAX-1; ++i) {
        int width = m_listCtrlAssets->GetColumnWidth(i);
        mmDBWrapper::setINISettingValue(m_inidb, wxString::Format(wxT("ASSETS_COL%d_WIDTH"), i), wxString() << width); 
    }
    mmDBWrapper::commit(m_inidb);
}

void mmAssetsPanel::CreateControls()
{    
    mmAssetsPanel* itemPanel8 = this;

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemPanel8->SetSizer(itemBoxSizer9);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel( itemPanel8, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, wxID_ANY, _("Assets"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, 
        wxGetEmptyString()));

	wxStaticText* itemStaticText10 = new wxStaticText( headerPanel, IDC_PANEL_CHECKING_STATIC_BALHEADER, _("Total:"), wxDefaultPosition, wxSize(500, 20), 0 );

    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);
	itemBoxSizerVHeader->Add(itemStaticText10, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow( itemPanel8, wxID_ANY, wxDefaultPosition, wxSize(100, 100), 
        wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxSize imageSize(16, 16);
    m_imageList.reset(new wxImageList(imageSize.GetWidth(), imageSize.GetHeight()));
    m_imageList->Add(wxBitmap(assets_xpm));

    m_listCtrlAssets = new assetsListCtrl( this, itemSplitterWindow10, 
        IDC_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    m_listCtrlAssets->SetBackgroundColour(mmColors::listBackColor);
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
     long col0, col1, col2, col3, col4;
     mmDBWrapper::getINISettingValue(m_inidb, 
        wxT("ASSETS_COL0_WIDTH"), wxT("150")).ToLong(&col0); 
     mmDBWrapper::getINISettingValue(m_inidb, 
         wxT("ASSETS_COL1_WIDTH"), wxT("-2")).ToLong(&col1); 
     mmDBWrapper::getINISettingValue(m_inidb, 
         wxT("ASSETS_COL2_WIDTH"), wxT("-2")).ToLong(&col2); 
     mmDBWrapper::getINISettingValue(m_inidb, 
         wxT("ASSETS_COL3_WIDTH"), wxT("-2")).ToLong(&col3); 
     mmDBWrapper::getINISettingValue(m_inidb, 
         wxT("ASSETS_COL4_WIDTH"), wxT("450")).ToLong(&col4); 
     
    m_listCtrlAssets->SetColumnWidth(COL_NAME, col0);
    m_listCtrlAssets->SetColumnWidth(COL_TYPE, col1);
    m_listCtrlAssets->SetColumnWidth(COL_VALUE, col2);
    m_listCtrlAssets->SetColumnWidth(COL_DATE, col3);
    m_listCtrlAssets->SetColumnWidth(COL_NOTES, col4);
    
    wxPanel* itemPanel12 = new wxPanel( itemSplitterWindow10, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

    itemSplitterWindow10->SplitHorizontally(m_listCtrlAssets, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);
    itemPanel12->SetBackgroundColour(mmColors::listDetailsPanelColor);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 3);

    wxSizerFlags flags;
    flags.Border();

    wxButton* itemButton6 = new wxButton( itemPanel12, IDC_BUTTON_NEW_ASSET, _("&New"));
    itemButton6->SetToolTip(_("New Asset"));
    wxFont fnt = itemButton6->GetFont();
    itemButton6->SetFont(fnt);
    itemButton6->SetForegroundColour(wxColour(wxT("FOREST GREEN")));
    itemBoxSizer5->Add(itemButton6, flags);

    wxButton* itemButton81 = new wxButton( itemPanel12, IDC_BUTTON_EDIT_ASSET, _("&Edit"));
    itemButton81->SetToolTip(_("Edit Asset"));
    itemButton81->SetFont(fnt);
    itemButton81->SetForegroundColour(wxColour(wxT("ORANGE")));
    itemBoxSizer5->Add(itemButton81, flags);
    itemButton81->Enable(false);
	
    wxButton* itemButton7 = new wxButton( itemPanel12, IDC_BUTTON_DELETE_ASSET, _("&Delete"));
    itemButton7->SetToolTip(_("Delete Asset"));
    itemButton7->SetFont(fnt);
    itemButton7->SetForegroundColour(wxColour(wxT("RED"))); // FIREBRICK
    itemBoxSizer5->Add(itemButton7, flags);
    itemButton7->Enable(false);


    //Infobar-mini 
    wxStaticText* itemStaticText44 = new wxStaticText( itemPanel12, IDC_PANEL_ASSET_STATIC_DETAILS_MINI, wxT(""), 
    wxPoint(-1,-1), wxSize(450, -1), wxNO_BORDER|wxST_NO_AUTORESIZE);
    itemBoxSizer5->Add(itemStaticText44, 1, wxGROW|wxALL, 10);

    //Infobar 
    wxStaticText* itemStaticText33 = new wxStaticText( itemPanel12, 
    IDC_PANEL_ASSET_STATIC_DETAILS, wxT(""), 
    wxPoint(-1,-1), wxSize(350, -1), wxNO_BORDER|wxTE_MULTILINE|wxTE_WORDWRAP|wxST_NO_AUTORESIZE);
    itemBoxSizer4->Add(itemStaticText33, 1, wxGROW|wxALL, 0);
            
            updateExtraAssetData(-1);

}

void mmAssetsPanel::initVirtualListControl()
{
    /* Clear all the records */
    m_trans.clear();

    mmDBWrapper::loadBaseCurrencySettings(m_db);

    double total = mmDBWrapper::getAssetBalance(m_db);
    wxString balance;
    mmex::formatDoubleToCurrency(total, balance);
    wxStaticText* header = (wxStaticText*)FindWindow(IDC_PANEL_CHECKING_STATIC_BALHEADER);
    wxASSERT(header);
    wxString lbl  = wxString::Format(_("Total: %s"), balance.c_str());
    header->SetLabel(lbl);

    static const char sql[] = 
    "select a.ASSETID as ASSETID, "
           "a.ASSETNAME as ASSETNAME, "
           "a.ASSETTYPE as ASSETTYPE, "
           "a.STARTDATE as STARTDATE, "
           "strftime(INFOVALUE, a.STARTDATE) as BEGINDATE, "
           "a.NOTES as NOTES, "
           "VALUECHANGE as VALUECHANGE, "
           "VALUECHANGERATE as VALUECHANGERATE "
    "from ASSETS_V1 a "
    "left join infotable_v1 i on i.INFONAME='DATEFORMAT' "
    "order by a.STARTDATE " ;

    wxSQLite3ResultSet q1 = m_db->ExecuteQuery(sql);
    long cnt = 0;
    
    for ( ; q1.NextRow(); ++cnt)
    {
        mmAssetHolder th;

        th.assetID_ = q1.GetInt(wxT("ASSETID"));
        th.value_ = mmDBWrapper::getAssetValue(m_db, th.assetID_);
        th.assetName_ = q1.GetString(wxT("ASSETNAME"));
        th.assetDate_ = q1.GetString(wxT("BEGINDATE"));
        //sqlite does not support %y date mask therefore null value should be replaces
        if (th.assetDate_ == wxT(""))
            {
            wxString dateString = q1.GetString(wxT("STARTDATE"));
            wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
            th.assetDate_ = mmGetDateForDisplay(m_db, dtdt);
            }
     
        wxString assetStr = q1.GetString(wxT("ASSETTYPE"));
        th.assetType_ =  wxGetTranslation(assetStr); // string should be marked for translation
        assetStr = q1.GetString(wxT("VALUECHANGE"));
        th.assetValueChange_ =  wxGetTranslation(assetStr); 

        th.assetNotes_ = q1.GetString(wxT("NOTES"));

        th.valueChange_ = q1.GetDouble(wxT("VALUECHANGERATE"));

        mmex::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);
        mmex::formatDoubleToCurrencyEdit(th.valueChange_, th.valueChangeStr_);

        m_trans.push_back(th);
    }

    m_listCtrlAssets->SetItemCount(cnt);
    q1.Finalize();
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
    if (column == COL_NAME)
        return m_trans[item].assetName_;

    if (column == COL_TYPE)
        return m_trans[item].assetType_;

    if (column == COL_VALUE)
        return m_trans[item].valueStr_;

    if (column == COL_DATE)
        return m_trans[item].assetDate_;

    if (column == COL_NOTES)
        return m_trans[item].assetNotes_;

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
	if (selIndex!=-1) { 
        enableEditDeleteButtons(true);
        wxString miniInfo;
        wxString infoStr;
        infoStr = getItem(selIndex, COL_NOTES);
        miniInfo << wxT("\t") << _("Change in Value") << wxT(": ") << m_trans[selIndex].assetValueChange_ << wxT(" = ") << m_trans[selIndex].valueChange_ << wxT("%");
        st->SetLabel(infoStr);
        stm->SetLabel(miniInfo);
        }
        else
        {
        stm -> SetLabel(wxT(""));
        st->SetLabel(_("MMEX allows you to track fixed assets like cars, houses, land and others. Each asset can have its value appreciate by a certain rate per year, depreciate by a certain rate per year, or not change in value. The total assets are added to your total financial worth."));
        enableEditDeleteButtons(false);
        }
}

void mmAssetsPanel::enableEditDeleteButtons(bool enable)
{
	wxButton* btn = static_cast<wxButton*>(FindWindow(IDC_BUTTON_EDIT_ASSET));
	wxASSERT(btn);
	btn->Enable(enable);

	btn = static_cast<wxButton*>(FindWindow(IDC_BUTTON_DELETE_ASSET));
	wxASSERT(btn);
	btn->Enable(enable);
}

int assetsListCtrl::OnGetItemImage(long /*item*/) const
{
   return 0; // Returns the icon to be shown for each entry
}

wxListItemAttr* assetsListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&m_attr2 : (wxListItemAttr *)&m_attr1;
}

void assetsListCtrl::OnListKeyDown(wxListEvent& event)
{
	if (event.GetKeyCode() == WXK_DELETE) {
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_DELETE);
                OnDeleteAsset(evt);
	} else {
        	event.Skip();
	}
}

void assetsListCtrl::OnNewAsset(wxCommandEvent& /*event*/)
{
	mmAssetDialog dlg(this, m_cp->getDb(), 0, false);
	if (dlg.ShowModal() == wxID_OK) {
		doRefreshItems();
	}
}

void assetsListCtrl::doRefreshItems()
{
	m_cp->initVirtualListControl();

	long cnt = static_cast<long>(m_cp->getTrans().size());
	RefreshItems(0, cnt > 0 ? --cnt : 0);
}
 
void assetsListCtrl::OnDeleteAsset(wxCommandEvent& /*event*/)
{
	if (m_selectedIndex == -1)
		return;

    if (m_cp->getTrans().empty())
        return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the Asset?"), _("Confirm Asset Deletion"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES) {
        mmDBWrapper::deleteAsset(m_cp->getDb(), m_cp->getTrans()[m_selectedIndex].assetID_);
        DeleteItem(m_selectedIndex);
        m_cp->initVirtualListControl();
        //if (m_cp->getTrans().empty()) //after delition of asset - index and extra panel should be erased
        {
            m_selectedIndex = -1;
            m_cp->updateExtraAssetData(m_selectedIndex);
        }
    }
}

void assetsListCtrl::OnEditAsset(wxCommandEvent& /*event*/)
{
	if (m_selectedIndex == -1) {
		return;
	}

	mmAssetDialog dlg(this, m_cp->getDb(), m_cp->getTrans()[m_selectedIndex].assetID_, true);

	if (dlg.ShowModal() == wxID_OK) {
		doRefreshItems();
	}
}

void assetsListCtrl::OnListItemActivated(wxListEvent& event)
{
	m_selectedIndex = event.GetIndex();

	mmAssetDialog dlg(this, m_cp->getDb(), m_cp->getTrans()[m_selectedIndex].assetID_, true);

	if (dlg.ShowModal() == wxID_OK) {
		doRefreshItems();
	}
}

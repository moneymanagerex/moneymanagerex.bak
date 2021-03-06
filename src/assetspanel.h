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

#pragma once
#include "mmpanelbase.h"
#include "model/Model_Asset.h"

class wxListEvent;
class mmAssetsPanel;

/* Custom ListCtrl class that implements virtual LC style */
class mmAssetsListCtrl: public mmListCtrl
{
    DECLARE_NO_COPY_CLASS(mmAssetsListCtrl)
    DECLARE_EVENT_TABLE()

public:
    mmAssetsListCtrl(mmAssetsPanel* cp, wxWindow *parent, wxWindowID winid = wxID_ANY);

    void OnNewAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void OnDuplicateAsset(wxCommandEvent& event);
	void OnOrganizeAttachments(wxCommandEvent& event);
	void OnOpenAttachment(wxCommandEvent& event);

    void doRefreshItems(int trx_id = -1);

private:
    mmAssetsPanel* ap_;

    /* required overrides for virtual style list control */
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;

    void OnMouseRightClick(wxMouseEvent& event);
    void OnMouseLeftClick(wxMouseEvent& event);
    void OnListItemActivated(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemDeselected(wxListEvent& event);
    void OnColClick(wxListEvent& event);
    void OnEndLabelEdit(wxListEvent& event);
    void OnItemResize(wxListEvent& event);
    bool EditAsset(Model_Asset::Data* pEntry);

    enum {
        MENU_TREEPOPUP_NEW = wxID_HIGHEST + 1200,
        MENU_TREEPOPUP_EDIT,
        MENU_TREEPOPUP_DELETE,
        MENU_ON_DUPLICATE_TRANSACTION,
		MENU_TREEPOPUP_ORGANIZE_ATTACHMENTS,
    };
};

class mmAssetsPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmAssetsPanel(wxWindow *parent);

    void updateExtraAssetData(int selIndex);
    int initVirtualListControl(int trx_id = -1, int col = 0, bool asc = true);
    wxString getItem(long item, long column);

    Model_Asset::Data_Set m_assets;
    Model_Asset::TYPE m_filter_type;
    int col_max() { return COL_MAX; }

    wxString BuildPage() const { return m_listCtrlAssets->BuildPage(_("Assets")); }

private:
    void enableEditDeleteButtons(bool enable);
    void OnSearchTxtEntered(wxCommandEvent& event);
    mmAssetsListCtrl* m_listCtrlAssets;

    wxStaticText* itemStaticTextMainFilter_;
    wxStaticText* header_text_;

    wxScopedPtr<wxImageList> m_imageList;

    bool Create(wxWindow *parent
        , wxWindowID winid
        , const wxPoint& pos
        , const wxSize& size
        , long style
        , const wxString &name);
    void CreateControls();

    /* Event handlers for Buttons */
    void OnNewAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);
	void OnOpenAttachment(wxCommandEvent& event);
    void OnMouseLeftDown ( wxMouseEvent& event );

    void OnViewPopupSelected(wxCommandEvent& event);
    void sortTable();

private:
    wxString tips_;
    enum {
        IDC_PANEL_ASSET_STATIC_DETAILS = wxID_HIGHEST + 1220,
        IDC_PANEL_ASSET_STATIC_DETAILS_MINI,
    };
    enum EColumn
    {
        COL_NAME = 0,
        COL_DATE,
        COL_TYPE,
        COL_VALUE_INITIAL,
        COL_VALUE_CURRENT,
        COL_NOTES,
        COL_MAX, // number of columns
    };
};

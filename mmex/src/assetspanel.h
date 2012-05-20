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

#ifndef _MM_EX_ASSETSPANEL_H_
#define _MM_EX_ASSETSPANEL_H_

#include "mmpanelbase.h"
#include "util.h"

#include "mmex_db_view.h"

class wxListEvent;
class mmAssetsPanel;

/* Custom ListCtrl class that implements virtual LC style */
class assetsListCtrl: public mmListCtrl
{
    DECLARE_NO_COPY_CLASS(assetsListCtrl)
    DECLARE_EVENT_TABLE()

public:
    assetsListCtrl(mmAssetsPanel* cp, wxWindow *parent,
        const wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : mmListCtrl(parent, id, pos, size, style)
        , m_cp(cp)
        , m_selectedIndex(-1)
        , m_selected_col(0)
        , m_asc(true)
    {}

    void OnNewAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void OnColClick(wxListEvent& event);

private:
    mmAssetsPanel* m_cp;
    long m_selectedIndex;
    int  m_selected_col;
    bool m_asc;

    /* required overrides for virtual style list control */
    wxString OnGetItemText(long item, long column) const;
    int OnGetItemImage(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemDeselected(wxListEvent& event);
    void doRefreshItems();
};

class mmAssetsPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmAssetsPanel(wxWindow *parent, wxSQLite3Database* db, wxSQLite3Database* inidb, mmCoreDB* core);
   ~mmAssetsPanel();

    void enableEditDeleteButtons(bool enable);
    void initVirtualListControl(int col, bool asc);

    wxString getItem(long item, long column);
    wxSQLite3Database* getDb() const { return db_; }
    void updateExtraAssetData(int selIndex);
    DB_View_ASSETS_V1::Data_Set all_assets_;

private:
    assetsListCtrl* m_listCtrlAssets;
    wxButton* m_delete_button;
    wxButton* m_edit_button;
    wxButton* m_new_button;

    bool Create(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString &name);
    void CreateControls();

    /* Event handlers for Buttons */
    void OnNewAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);

    void OnViewPopupSelected(wxCommandEvent& event);
    void sortTable();
    void destroy();
};

#endif


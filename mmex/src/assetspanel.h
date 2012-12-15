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

#include <boost/scoped_ptr.hpp>

class wxListCtrl;
class wxListEvent;
class mmAssetsPanel;

/* Custom ListCtrl class that implements virtual LC style */
class assetsListCtrl: public wxListCtrl
{
    DECLARE_NO_COPY_CLASS(assetsListCtrl)
    DECLARE_EVENT_TABLE()

public:
    assetsListCtrl(mmAssetsPanel* cp, wxWindow *parent,
        const wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : wxListCtrl(parent, id, pos, size, style),
        m_attr1(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont),
        m_attr2(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont),
        cp_(cp),
        selectedIndex_(-1)
    {}

    void OnNewAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void InitVariables();
    void doRefreshItems(int trx_id = -1);

private:
    wxListItemAttr m_attr1; // style1
    wxListItemAttr m_attr2; // style2
    mmAssetsPanel* cp_;
    long selectedIndex_;
    int  m_selected_col;
    bool m_asc;

    /* required overrides for virtual style list control */
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemDeselected(wxListEvent& event);
    void OnColClick(wxListEvent& event);
    void OnItemResize(wxListEvent& event);
};

/* Holds a single transaction */
struct mmAssetHolder: public mmHolderBase
{
    wxString assetName_;
    wxString assetType_;
    wxDateTime assetDate_;
    wxString assetNotes_;
    wxString sAssetValueChange_;
    wxString asset_notes_;

    double valueChange_;
    wxString valueChangeStr_;
	double value_;
	wxString valueStr_;
	double todayValue_;
	wxString todayValueStr_;
};

class mmAssetsPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmAssetsPanel(wxWindow *parent, mmCoreDB* core);
    ~mmAssetsPanel();
    const std::vector<mmAssetHolder*>& getTrans() const { return m_trans; }
    void updateExtraAssetData(int selIndex);
    int initVirtualListControl(int trx_id = -1, int col = 0, bool asc = true);
    wxString getItem(long item, long column);
    void SetFilter(wxString filter) {filter_=filter; }
    int GetListCtrlWidth(int id) {return m_listCtrlAssets->GetColumnWidth(id);}
    void SetListCtrlColumn(int m_selected_col, wxListItem item)
        {m_listCtrlAssets->SetColumn(m_selected_col, item);}

private:
    void enableEditDeleteButtons(bool enable);
    assetsListCtrl* m_listCtrlAssets;

    wxStaticText* itemStaticTextMainFilter_;
    wxString filter_;
    wxStaticText* header_text_;

    boost::scoped_ptr<wxImageList> m_imageList;
    std::vector<mmAssetHolder*> m_trans;

    bool Create(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString &name);
    void CreateControls();

    /* Event handlers for Buttons */
    void OnNewAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);
    void OnMouseLeftDown ( wxMouseEvent& event );

    void OnViewPopupSelected(wxCommandEvent& event);
    void sortTable();
};

#endif


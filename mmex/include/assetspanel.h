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
 /*******************************************************/
#ifndef _MM_EX_ASSETSPANEL_H_
#define _MM_EX_ASSETSPANEL_H_

#include "mmpanelbase.h"
#include "guiid.h"
#include "util.h"

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
        attr1_(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont),
        attr2_(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont),
        cp_(cp),
        selectedIndex_(-1)
    {}

public:
    /* required overrides for virtual style list control */
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);

    void OnNewAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);

private:
    wxListItemAttr attr1_; // style1
    wxListItemAttr attr2_; // style2
    mmAssetsPanel* cp_;
    long selectedIndex_;
};

/* Holds a single transaction */
class mmAssetHolder
{
public:
    int assetID_;

    wxString assetName_;
    wxString assetType_;
    wxString valueStr_;

    double value_;
};

class mmAssetsPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmAssetsPanel( wxSQLite3Database* db, wxSQLite3Database* inidb, 
            wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr );
    ~mmAssetsPanel();

    bool Create( wxWindow *parent, wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);
      
    void CreateControls();

    /* updates the checking panel data */
    void initVirtualListControl();

    /* Getter for Virtual List Control */
    wxString getItem(long item, long column);
  
    /* Event handlers for Buttons */
    void OnNewAsset(wxCommandEvent& event);
    void OnDeleteAsset(wxCommandEvent& event);
    void OnEditAsset(wxCommandEvent& event);

    void OnViewPopupSelected(wxCommandEvent& event);

    /* Helper Functions/data */
    std::vector<mmAssetHolder> trans_;
    void sortTable();

public:
    wxSQLite3Database* db_;
    assetsListCtrl* listCtrlAssets_;
    wxSQLite3Database* inidb_;
    wxImageList* m_imageList;
};

#endif


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
 ********************************************************/

#ifndef _MM_EX_BUDGETINGPANEL_H_
#define _MM_EX_BUDGETINGPANEL_H_

#include "mmpanelbase.h"
#include "guiid.h"
#include "defs.h"
#include "wx/wxprec.h"
#include "util.h"

class wxListEvent;
class mmBudgetingPanel;
class mmGUIFrame;

/* Custom ListCtrl class that implements virtual LC style */
class budgetingListCtrl: public mmListCtrl
{
    DECLARE_NO_COPY_CLASS(budgetingListCtrl)
    DECLARE_EVENT_TABLE()

public:
    budgetingListCtrl(mmBudgetingPanel* cp, wxWindow *parent,
        const wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : mmListCtrl(parent, id, pos, size, style),
        attr1_(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont),
        attr2_(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont),
        attr3_(mmColors::listAlternativeColor1, mmColors::listFutureDateColor, wxNullFont),
//      Forground,background,font
//      attr3_(wxColour( 255, 0, 0 ), wxColour( 0, 255, 0 ), wxNullFont),
        cp_(cp),
        selectedIndex_(-1)
    {}

public:
    /* required overrides for virtual style list control */
    virtual wxString OnGetItemText(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;
    virtual int OnGetItemImage(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    

private:
    wxListItemAttr attr1_; // style1
    wxListItemAttr attr2_; // style2
    wxListItemAttr attr3_; // style3
    mmBudgetingPanel* cp_;
    long selectedIndex_;
};

struct mmBudgetEntryHolder: public mmHolderBase
{
    int categID_;
    wxString catStr_;

    int subcategID_;
    wxString subCatStr_;

    wxString period_;

    wxString amtString_;
    double amt_;

    wxString estimatedStr_;
    double estimated_;

    wxString actualStr_;
    double actual_;
};


class mmBudgetingPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmBudgetingPanel( wxSQLite3Database* db, wxSQLite3Database* inidb, mmCoreDB* core,
            mmGUIFrame* mainFrame, int budgetYearID, wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr );
    ~mmBudgetingPanel();

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
    void OnNewTransaction(wxCommandEvent& event);
    void OnDeleteTransaction(wxCommandEvent& event);
    void OnEditTransaction(wxCommandEvent& event);
    void OnViewPopupSelected(wxCommandEvent& event);
    void OnMouseLeftDown( wxMouseEvent& event );

    std::vector<mmBudgetEntryHolder> trans_;

public:
    wxImageList* m_imageList;
    budgetingListCtrl* listCtrlAccount_;
    wxString currentView_;
    int budgetYearID_;
    mmGUIFrame* mainFrame_;

    bool displayEntryAllowed(mmBudgetEntryHolder& budgetEntry);
};

#endif


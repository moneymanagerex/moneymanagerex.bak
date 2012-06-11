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
        : mmListCtrl(parent, id, pos, size, style)
        , m_attr3(new wxListItemAttr(mmColors::listAlternativeColor1, mmColors::listFutureDateColor, wxNullFont))
        , cp_(cp)
        , selectedIndex_(-1)
    {}
    ~budgetingListCtrl()
    {
        if (m_attr3) delete m_attr3;
    }

public:
    wxString OnGetItemText(long item, long column) const;
    wxListItemAttr *OnGetItemAttr(long item) const;
    int OnGetItemImage(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    
private:
    wxListItemAttr* m_attr3; // style3
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
    mmBudgetingPanel( wxSQLite3Database* db, mmCoreDB* core,
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
    budgetingListCtrl* listCtrlBudget_;
    wxString currentView_;
    int budgetYearID_;
    mmGUIFrame* mainFrame_;

    bool displayEntryAllowed(mmBudgetEntryHolder& budgetEntry);
};

#endif


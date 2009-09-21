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

#ifndef _MM_EX_CHECKINGPANEL_H_
#define _MM_EX_CHECKINGPANEL_H_

#include "mmpanelbase.h"
#include "guiid.h"
#include "defs.h"
#include "wx/wxprec.h"
#include "util.h"
#include "mmcoredb.h"

class wxListCtrl;
class wxListEvent;
class mmCheckingPanel;

/* Custom ListCtrl class that implements virtual LC style */
class MyListCtrl: public wxListCtrl
{
    DECLARE_NO_COPY_CLASS(MyListCtrl)
    DECLARE_EVENT_TABLE()

public:
    MyListCtrl(mmCheckingPanel* cp, wxWindow *parent,
        const wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : wxListCtrl(parent, id, pos, size, style | wxWANTS_CHARS),
        attr1_(*wxBLACK, mmColors::listAlternativeColor0, wxNullFont),
        attr2_(*wxBLACK, mmColors::listAlternativeColor1, wxNullFont),
        attr3_(mmColors::listFutureDateColor, mmColors::listAlternativeColor0, wxNullFont),
        attr4_(mmColors::listFutureDateColor, mmColors::listAlternativeColor1, wxNullFont),
        cp_(cp),
        sortCol_(0),
        asc_(true),
        selectedIndex_(-1),
        m_selectedForCopy_(-1)
    {
        /* Setup the Accelorator Table */
        wxAcceleratorEntry entries[3];
        entries[0].Set(wxACCEL_CTRL,  (int) 'C', MENU_ON_COPY_TRANSACTION);
        entries[1].Set(wxACCEL_CTRL,  (int) 'V', MENU_ON_PASTE_TRANSACTION);
        entries[2].Set(wxACCEL_ALT,  (int) 'N', MENU_ON_NEW_TRANSACTION);
   
        wxAcceleratorTable accel(3, entries);
        this->SetAcceleratorTable(accel); 
    }

public:
    /* required overrides for virtual style list control */
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    void OnMarkTransaction(wxCommandEvent& event);
    void OnMarkAllTransactions(wxCommandEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMarkTransactionDB(const wxString& status);
    void OnCopy(wxCommandEvent& WXUNUSED(event));
    void OnPaste(wxCommandEvent& WXUNUSED(event));

    /* Sort Columns */
    void OnColClick(wxListEvent& event);
    void SetColumnImage(int col, int image);
    
    void OnNewTransaction(wxCommandEvent& event);
    void OnDeleteTransaction(wxCommandEvent& event);
    void OnEditTransaction(wxCommandEvent& event);

private:
    wxListItemAttr attr1_; // style1
    wxListItemAttr attr2_; // style2
    wxListItemAttr attr3_; // style, for future dates
    wxListItemAttr attr4_; // style, for future dates
    mmCheckingPanel* cp_;
    long selectedIndex_;
    long m_selectedForCopy_;

public:
    long sortCol_;
    bool asc_;
};

/* Holds a single transaction */
class mmTransactionHolder
{
public:
    int transID_;

    wxDateTime date_;
    wxString dateStr_;

    int catID_;
    wxString catStr_;
    int subcategID_;
    wxString subCatStr_;

    int payeeID_;
    wxString payeeStr_;

    wxString transNum_;
    wxString status_;
    wxString notes_;
    wxString transType_;
    
    wxString transAmtString_;
    double amt_;

    wxString transToAmtString_;
    double toAmt_;
    
    int accountID_;
    int toAccountID_;

    wxString fromAccountStr_;
    
    wxString withdrawalStr_;
    wxString depositStr_;

    double balance_;
    wxString balanceStr_;
};

class mmCheckingPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmCheckingPanel( mmCoreDB* core,
        wxSQLite3Database* inidb, 
        int accountID, wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr );
    
    ~mmCheckingPanel();

    bool Create( wxWindow *parent, wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);
      
    void CreateControls();

    /* updates the checking panel data */
    void initVirtualListControl();
    void updateExtraTransactionData(int selIndex);

    /* Getter for Virtual List Control */
    wxString getItem(long item, long column);
  
    /* Event handlers for Buttons */
    void OnNewTransaction(wxCommandEvent& event);
    void OnDeleteTransaction(wxCommandEvent& event);
    void OnEditTransaction(wxCommandEvent& event);
    void OnMouseLeftDown( wxMouseEvent& event );
    void OnViewPopupSelected(wxCommandEvent& event);
    void initViewTransactionsHeader();

    void setAccountSummary();

    /* Helper Functions/data */
    int accountID() const { return accountID_; }
    std::vector<mmBankTransaction*> trans_;
    void sortTable();

public:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    wxSQLite3Database* inidb_;
    MyListCtrl* listCtrlAccount_;
    wxImageList* m_imageList;
    wxString currentView_;
    int accountID_;
};

#endif


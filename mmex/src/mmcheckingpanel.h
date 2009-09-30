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

//----------------------------------------------------------------------------
#ifndef _MM_EX_CHECKINGPANEL_H_
#define _MM_EX_CHECKINGPANEL_H_
//----------------------------------------------------------------------------
#include "defs.h"
#include "mmpanelbase.h"
//----------------------------------------------------------------------------
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------
class mmCoreDB;
class mmBankTransaction;
class MyListCtrl;
//----------------------------------------------------------------------------

/* 
    Holds a single transaction 
*/
struct mmTransactionHolder
{
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
//----------------------------------------------------------------------------

class mmCheckingPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmCheckingPanel(
        mmCoreDB* core,
        wxSQLite3Database* inidb, 
        int accountID, 
        wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr 
    );
    
    ~mmCheckingPanel();

    bool Create( 
        wxWindow *parent, 
        wxWindowID winid,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr
    );
      
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

    int accountID() const { return m_AccountID; }
    void sortTable();

    std::vector<mmBankTransaction*> trans_;

private:
    mmCoreDB* core_;
    wxSQLite3Database* inidb_;
    wxString currentView_;

    MyListCtrl *listCtrlAccount_;
    const int m_AccountID;
    boost::scoped_ptr<wxImageList> m_imageList;

    friend class MyListCtrl;

    boost::shared_ptr<wxSQLite3Database> getDb() const;
};
//----------------------------------------------------------------------------
#endif // _MM_EX_CHECKINGPANEL_H_
//----------------------------------------------------------------------------


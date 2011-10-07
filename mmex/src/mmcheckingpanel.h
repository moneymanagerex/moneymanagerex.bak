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
#include <vector>
//----------------------------------------------------------------------------
class mmCoreDB;
class mmBankTransaction;
class MyListCtrl;
class TransFilterDialog;
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

private:
    DECLARE_EVENT_TABLE()
    friend class MyListCtrl; // needs access to m_core, initdb_, ...

    mmCoreDB* m_core;
    wxSQLite3Database* m_inidb;
    wxString m_currentView;
    wxStaticText* statTextTransFilter_;
    TransFilterDialog* transFilterDlg_;
    bool transFilterActive_;

    MyListCtrl *m_listCtrlAccount;
    const int m_AccountID;
    boost::scoped_ptr<wxImageList> m_imageList;
    std::vector<mmBankTransaction*> m_trans;

    boost::shared_ptr<wxSQLite3Database> getDb() const;
    void initViewTransactionsHeader();
    void setAccountSummary();
    void sortTable();
    void CreateControls();

    bool Create( 
        wxWindow *parent, 
        wxWindowID winid,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr
    );
	void enableEditDeleteButtons(bool en);

    void OnNewTransaction(wxCommandEvent& event);
    void OnDeleteTransaction(wxCommandEvent& event);
    void OnEditTransaction(wxCommandEvent& event);
    void OnMoveTransaction(wxCommandEvent& event);
    void OnMouseLeftDown( wxMouseEvent& event );
    void OnViewPopupSelected(wxCommandEvent& event);
    void OnFilterTransactions(wxCommandEvent& event);

    void DeleteViewedTransactions();
    void DeleteFlaggedTransactions();

    /* updates the checking panel data */
    void initVirtualListControl(wxProgressDialog* pgd);
    void Tips();
    void updateExtraTransactionData(int selIndex);
    void saveSettings();

    /* Getter for Virtual List Control */
    wxString getItem(long item, long column);

    wxString getMiniInfoStr(int selIndex);

    int accountID() const { return m_AccountID; }

    double getBalance(mmBankTransaction* transPtr, double currentBalance );
    void setBalance(mmBankTransaction* transPtr, double currentBalance );

};
//----------------------------------------------------------------------------
#endif // _MM_EX_CHECKINGPANEL_H_
//----------------------------------------------------------------------------


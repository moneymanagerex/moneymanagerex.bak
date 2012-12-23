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
#include "filtertransdialog.h"
#include "defs.h"
#include "mmpanelbase.h"
//----------------------------------------------------------------------------
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
//----------------------------------------------------------------------------
class mmGUIFrame;
class mmBankTransaction;
class TransactionListCtrl;
class mmFilterTransactionsDialog;
//----------------------------------------------------------------------------

// This structure is currently not being used.
// temporarily removed for now
#if 0
/* 
    Holds a single transaction 
*/
struct mmTransactionHolder: public mmHolderBase
{
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
#endif
//----------------------------------------------------------------------------

class mmCheckingPanel : public mmPanelBase
{
public:

    mmCheckingPanel(
        mmCoreDB* core,
        mmGUIFrame* mainFrame,
        int accountID, 
        wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr 
    );

    ~mmCheckingPanel();

    /// Display the split categories for the selected transaction.
    void DisplaySplitCategories(int transID);
    /// Refresh account screen with new details
    void DisplayAccountDetails(int accountID); 

private:
    DECLARE_EVENT_TABLE()
    friend class TransactionListCtrl; // needs access to m_core, initdb_, ...

    wxButton* bNew_;
    wxButton* bEdit_;
    wxButton* bMove_;
    wxButton* bDelete_;
    wxStaticText* header_text_;
    wxStaticText* info_panel_;
    wxStaticText* info_panel_mini_;
    wxStaticText* stxtMainFilter_;
    wxStaticText* statTextTransFilter_;
    wxStaticBitmap* bitmapTransFilter_;
	wxStaticBitmap* bitmapMainFilter_;
    mmFilterTransactionsDialog* transFilterDlg_;

    wxString m_currentView;
    mmGUIFrame* mainFrame_;
    bool transFilterActive_;
    double filteredBalance_;

    TransactionListCtrl *m_listCtrlAccount;
    int m_AccountID;
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
    void OnDuplicateTransaction(wxCommandEvent& event);
    void OnMoveTransaction(wxCommandEvent& event);
    void OnMouseLeftDown( wxMouseEvent& event );
    void OnViewPopupSelected(wxCommandEvent& event);
    void OnFilterTransactions(wxMouseEvent& event);
    void OnSearchTxtEntered(wxCommandEvent& event);
    void OnFilterResetToViewAll(wxMouseEvent& event);

    void DeleteViewedTransactions();
    void DeleteFlaggedTransactions(const wxString status);
    void SetTransactionFilterState(bool active);

    /* updates the checking panel data */
    void initVirtualListControl(const int trans_id = -1);
    void showTips();
    void updateExtraTransactionData(int selIndex);

    /* Getter for Virtual List Control */
    wxString getItem(long item, long column) const;

    wxString getMiniInfoStr(int selIndex) const;

    int accountID() const { return m_AccountID; }

    double getBalance(mmBankTransaction* transPtr, double currentBalance) const;
    void setBalance(mmBankTransaction* transPtr, double currentBalance );

};
//----------------------------------------------------------------------------
#endif // _MM_EX_CHECKINGPANEL_H_
//----------------------------------------------------------------------------


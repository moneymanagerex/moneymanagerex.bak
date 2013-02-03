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

#ifndef _MM_EX_BILLSDEPOSITSPANEL_H_
#define _MM_EX_BILLSDEPOSITSPANEL_H_

#include "filtertransdialog.h"
#include "mmpanelbase.h"
#include "guiid.h"
#include "defs.h"
#include "wx/wxprec.h"
#include "util.h"
#include "mmcoredb.h"

class wxListCtrl;
class wxListEvent;
class mmBillsDepositsPanel;

/* Custom ListCtrl class that implements virtual LC style */
class billsDepositsListCtrl: public wxListCtrl
{
    DECLARE_NO_COPY_CLASS(billsDepositsListCtrl)
    DECLARE_EVENT_TABLE()

public:
    billsDepositsListCtrl(mmBillsDepositsPanel* cp, wxWindow *parent,
        const wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : wxListCtrl(parent, id, pos, size, style),
        attr1_(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont),
        attr2_(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont),
        cp_(cp),
        selectedIndex_(-1)
    {}

    void OnNewBDSeries(wxCommandEvent& event);
    void OnEditBDSeries(wxCommandEvent& event);
    void OnDeleteBDSeries(wxCommandEvent& event);
    void OnEnterBDTransaction(wxCommandEvent& event);
    void OnSkipBDTransaction(wxCommandEvent& event);

private:
    /* required overrides for virtual style list control */
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

    void OnItemRightClick(wxListEvent& event);
    void OnListItemActivated(wxListEvent& event);
    void OnMarkTransaction(wxCommandEvent& event);
    void OnMarkAllTransactions(wxCommandEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemDeselected(wxListEvent& event);
    void OnItemResize(wxListEvent& event);

    void refreshVisualList(int selected_index = -1);

    wxListItemAttr attr1_; // style1
    wxListItemAttr attr2_; // style2
    mmBillsDepositsPanel* cp_;
    long selectedIndex_;
};

/* Holds a single transaction */
struct mmBDTransactionHolder: public mmHolderBase
{
    wxDateTime nextOccurDate_;
    wxString nextOccurStr_;
    wxString repeatsStr_;

    int daysRemaining_;
    wxString daysRemainingStr_;

    int payeeID_;
    wxString payeeStr_;

    wxString sStatus_;

    wxString transType_;
    wxString transAmtString_;
    double amt_;

    wxString transToAmtString_;
    double toAmt_;

    int accountID_;
    int toAccountID_;

    wxString accountName_;
    wxString sNumber_;
    wxString notes_;
    int categID_;
    wxString categoryStr_;
    int subcategID_;
    wxString subcategoryStr_;

    bool bd_repeat_user_;
    bool bd_repeat_auto_;
};

class mmBillsDepositsPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmBillsDepositsPanel(
        mmCoreDB* core,
        wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr
    );
    ~mmBillsDepositsPanel();

    bool Create( wxWindow *parent, wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);
    /* Helper Functions/data */
    std::vector<mmBDTransactionHolder> trans_;
    void updateBottomPanelData(int selIndex);
    /* updates the Repeating transactions panel data */
    int initVirtualListControl(int id = -1);
    /* Getter for Virtual List Control */
    wxString getItem(long item, long column);
    int GetListCtrlWidth(int id) {return listCtrlAccount_->GetColumnWidth(id);}

private:
    void CreateControls();

    /* Event handlers for Buttons */
    void OnNewBDSeries(wxCommandEvent& event);
    void OnEditBDSeries(wxCommandEvent& event);
    void OnDeleteBDSeries(wxCommandEvent& event);

    void OnEnterBDTransaction(wxCommandEvent& event);
    void OnSkipBDTransaction(wxCommandEvent& event);

    void enableEditDeleteButtons(bool en);

    void OnViewPopupSelected(wxCommandEvent& event);

    void sortTable();

    wxImageList* m_imageList;
    billsDepositsListCtrl* listCtrlAccount_;

    bool transFilterActive_;
    void OnFilterTransactions(wxMouseEvent& event);
    mmFilterTransactionsDialog* transFilterDlg_;
    wxStaticBitmap* bitmapTransFilter_;
};
#endif


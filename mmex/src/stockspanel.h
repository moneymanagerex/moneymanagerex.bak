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

#ifndef _MM_EX_STOCKSPANEL_H_
#define _MM_EX_STOCKSPANEL_H_

#include "mmpanelbase.h"
#include "util.h"
#include "led.h"
#include <wx/tglbtn.h>

class wxListEvent;
class mmStocksPanel;
class mmYahoo;

/* Custom ListCtrl class that implements virtual LC style */
class stocksListCtrl: public mmListCtrl
{
    DECLARE_NO_COPY_CLASS(stocksListCtrl)
    DECLARE_EVENT_TABLE()

public:
    stocksListCtrl(mmStocksPanel* cp, wxWindow *parent,
        const wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : mmListCtrl(parent, id, pos, size, style),
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
    void OnMarkTransaction(wxCommandEvent& event);
    void OnMarkAllTransactions(wxCommandEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnListItemDeselected(wxListEvent& event);

    void OnNewStocks(wxCommandEvent& event);
    void OnDeleteStocks(wxCommandEvent& event);
    void OnEditStocks(wxCommandEvent& event);

private:
    wxListItemAttr attr1_; // style1
    wxListItemAttr attr2_; // style2
    mmStocksPanel* cp_;
    long selectedIndex_;
};

/* Holds a single transaction */
struct mmStockTransactionHolder: public mmHolderBase
{
    wxString heldAt_;
    wxString stockPDate_;
    wxString shareName_;
    wxString stockSymbol_;
    wxString stockPercentagePerYearStr_;
    wxString shareNotes_;
    wxString numSharesStr_;
    wxString totalnumSharesStr_;
    wxString gainLossStr_;
    wxString cPriceStr_;
    wxString pPriceStr_;
    wxString avgpurchasePriceStr_;
    wxString symbol_;

    double currentPrice_;
    double purchasePrice_;
    double avgpurchasePrice_;
    double gainLoss_;
    double numShares_;
    double totalnumShares_;
    double stockDays_;
    int purchasedTime_ ;
};

class mmStocksPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmStocksPanel( wxSQLite3Database* db,
            wxSQLite3Database* inidb,
            mmCoreDB* core,
            int accountID,
            wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr );
    ~mmStocksPanel();

    bool Create( wxWindow *parent, wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);

    void CreateControls();

    /* updates thstockide checking panel data */
    void initVirtualListControl();

    /* Getter for Virtual List Control */
    wxString getItem(long item, long column);

    /* Event handlers for Buttons */
    void OnNewStocks(wxCommandEvent& event);
    void OnDeleteStocks(wxCommandEvent& event);
    void OnEditStocks(wxCommandEvent& event);
    void OnRefreshQuotes(wxCommandEvent& event);
    void OnHTTPSettings(wxCommandEvent& event);
    void enableEditDeleteButtons(bool en);
    void updateExtraStocksData(int selIndex);

    void OnViewPopupSelected(wxCommandEvent& event);

    /* Helper Functions/data */
    std::vector<mmStockTransactionHolder> trans_;
    void sortTable();


public:
    stocksListCtrl* listCtrlAccount_;
    wxImageList* m_imageList;
    int accountID_;
    /************************************************************/
    // Greg Newton
private:
    mmYahoo* yahoo_;
    awxLed* m_LED;
    wxTimer* StatusRefreshTimer_;
    wxTimer* DownloadScheduleTimer_;

    // Timer to refresh the state of the LED & its tooltip
    void OnRefreshTimer( wxTimerEvent& event );
    // Timer to kick off download when required
    void OnScheduleTimer( wxTimerEvent &event );
    void OrderDownloadIfRequired(void);

    bool DownloadIsRequired(void);
    void OrderQuoteRefresh(void);

    wxToggleButton* TempProxyButton;
};

#endif


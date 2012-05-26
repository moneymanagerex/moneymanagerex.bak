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

enum
{
    ID_PANEL_STOCKS_STATIC_DETAILS = wxID_HIGHEST + 500,
    ID_PANEL_STOCKS_STATIC_DETAILS_MINI,
    ID_TIMER_SCHEDULE_STOCK,
    ID_DPC_STOCK_PDATE,
    ID_BUTTON_STOCKS_HELDAT,
    ID_TEXTCTRL_STOCKNAME,
    ID_TEXTCTRL_STOCK_SYMBOL,
    ID_TEXTCTRL_NUMBER_SHARES,
    ID_TEXTCTRL_STOCK_PP,
    ID_TEXTCTRL_STOCK_NOTES,
    ID_TEXTCTRL_STOCK_CP,
    ID_BUTTON_STOCK_CURRENCY,
    ID_STATIC_STOCK_VALUE,
    ID_TEXTCTRL_STOCK_COMMISSION,
    ID_BUTTON_STOCK_WEBPRICE,
    ID_PANEL_STOCK_UPDATE_LED,
    ID_DIALOG_STOCKS,
    ID_PANEL_STOCKS_LISTCTRL
};

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
        cp_(cp),
        selectedIndex_(-1)
    {}

public:
    wxString OnGetItemText(long item, long column) const;
    int OnGetItemImage(long item) const;

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
    stocksListCtrl* listCtrlStock_;
    int accountID_;

    /************************************************************/
    // Greg Newton

private:
    mmYahoo* yahoo_;
    wxStaticBitmap* m_LED;
    wxTimer* StatusRefreshTimer_;
    wxTimer* DownloadScheduleTimer_;

    void OnScheduleTimer( wxTimerEvent &event );
    void OrderDownloadIfRequired(void);

    bool DownloadIsRequired(void);
    void OrderQuoteRefresh(void);

    wxStaticText* stock_details_;
    wxStaticText* stock_details_short_;
    wxString strLastUpdate_;
};

#endif

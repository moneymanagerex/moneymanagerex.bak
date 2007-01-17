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
 /*******************************************************/
#ifndef _MM_EX_STOCKSPANEL_H_
#define _MM_EX_STOCKSPANEL_H_

#include "mmpanelbase.h"
#include "guiid.h"
#include "util.h"

class wxListCtrl;
class wxListEvent;
class mmStocksPanel;

/* Custom ListCtrl class that implements virtual LC style */
class stocksListCtrl: public wxListCtrl
{
    DECLARE_NO_COPY_CLASS(stocksListCtrl)
    DECLARE_EVENT_TABLE()

public:
    stocksListCtrl(mmStocksPanel* cp, wxWindow *parent,
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
    void OnMarkTransaction(wxCommandEvent& event);
    void OnMarkAllTransactions(wxCommandEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);

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
class mmStockTransactionHolder
{
public:
    int stockID_;

    wxString heldAt_;
    wxString shareName_;
    wxString numSharesStr_;
    wxString gainLossStr_;
    wxString valueStr_;
    wxString cPriceStr_;
    wxString pPriceStr_;
    wxString symbol_;

    double currentPrice_;
    double purchasePrice_;
    double value_;
    double gainLoss_;
    double numShares_;
};

class mmStocksPanel : public mmPanelBase
{
    DECLARE_EVENT_TABLE()

public:
    mmStocksPanel( wxSQLite3Database* db, 
		    wxSQLite3Database* inidb, 
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

    /* updates the checking panel data */
    void initVirtualListControl();

    /* Getter for Virtual List Control */
    wxString getItem(long item, long column);
  
    /* Event handlers for Buttons */
    void OnNewStocks(wxCommandEvent& event);
    void OnDeleteStocks(wxCommandEvent& event);
    void OnEditStocks(wxCommandEvent& event);

    void OnViewPopupSelected(wxCommandEvent& event);

    /* Helper Functions/data */
    std::vector<mmStockTransactionHolder> trans_;
    void sortTable();

public:
    wxSQLite3Database* db_;
    stocksListCtrl* listCtrlAccount_;
    wxSQLite3Database* inidb_;
    wxImageList* m_imageList;
	int accountID_;
};

#endif


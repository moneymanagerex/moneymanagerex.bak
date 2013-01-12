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

#include "stockspanel.h"
#include "util.h"
#include "dbwrapper.h"
#include "stockdialog.h"
#include "constants.h"
#include <boost/scoped_array.hpp>
#include <string>


/*******************************************************/
namespace
{
    enum EColumn
    {
        COL_DATE,
        COL_NAME,
        COL_NUMBER,
        COL_VALUE,
        COL_GAIN_LOSS,
        COL_CURRENT,
        COL_NOTES,
        COL_MAX, // number of columns
    };
}
BEGIN_EVENT_TABLE(mmStocksPanel, wxPanel)
    EVT_BUTTON(wxID_NEW,         mmStocksPanel::OnNewStocks)
    EVT_BUTTON(wxID_EDIT,        mmStocksPanel::OnEditStocks)
    EVT_BUTTON(wxID_DELETE,      mmStocksPanel::OnDeleteStocks)
    EVT_BUTTON(wxID_MOVE_FRAME,  mmStocksPanel::OnMoveStocks)
    EVT_BUTTON(wxID_REFRESH,     mmStocksPanel::OnRefreshQuotes)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(stocksListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_STOCKS_LISTCTRL,   stocksListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_STOCKS_LISTCTRL, stocksListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_STOCKS_LISTCTRL,    stocksListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(ID_PANEL_STOCKS_LISTCTRL,  stocksListCtrl::OnListItemDeselected)
    EVT_LIST_COL_END_DRAG(ID_PANEL_STOCKS_LISTCTRL,     stocksListCtrl::OnItemResize)
    EVT_LIST_COL_CLICK(ID_PANEL_STOCKS_LISTCTRL,        stocksListCtrl::OnColClick)
    EVT_LIST_KEY_DOWN(ID_PANEL_STOCKS_LISTCTRL,         stocksListCtrl::OnListKeyDown)

    EVT_MENU(MENU_TREEPOPUP_NEW,     stocksListCtrl::OnNewStocks)
    EVT_MENU(MENU_TREEPOPUP_EDIT,    stocksListCtrl::OnEditStocks)
    EVT_MENU(MENU_TREEPOPUP_DELETE,  stocksListCtrl::OnDeleteStocks)

END_EVENT_TABLE()
/*******************************************************/
mmStocksPanel::mmStocksPanel(mmCoreDB* core,
                             int accountID,
                             wxWindow *parent,
                             wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
: mmPanelBase(NULL, core)
, m_imageList(0)
, accountID_(accountID)
{
    Create(parent, winid, pos, size, style, name);
}

void stocksListCtrl::InitVariables()
{
    m_selected_col = 0;
    m_asc = true;
}

bool mmStocksPanel::Create(wxWindow *parent,
                            wxWindowID winid, const wxPoint& pos,
                            const wxSize& size,long style, const wxString& name)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    strLastUpdate_ = core_->dbInfoSettings_->GetStringSetting(wxT("STOCKS_LAST_REFRESH_DATETIME"), wxT(""));
    windowsFreezeThaw(this);
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    listCtrlAccount_->InitVariables();
    initVirtualListControl();
    if (trans_.size() > 1)
        listCtrlAccount_->EnsureVisible(((int)trans_.size()) - 1);

    windowsFreezeThaw(this);
    return TRUE;
}

mmStocksPanel::~mmStocksPanel()
{
    if (m_imageList) delete m_imageList;
}

void mmStocksPanel::save_column_width(const int width)
{
    int i = width;
    int col_x = listCtrlAccount_->GetColumnWidth(i);
    core_->iniSettings_->SetIntSetting(wxString::Format(wxT("STOCKS_COL%d_WIDTH"), i),col_x);
}

void stocksListCtrl::OnItemResize(wxListEvent& event)
{
    int i = event.GetColumn();
    cp_->save_column_width(i);
}

void mmStocksPanel::CreateControls()
{
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer9);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    int font_size = this->GetFont().GetPointSize() + 2;
    header_text_ = new wxStaticText(headerPanel, wxID_STATIC, wxT(""));
    header_text_->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));

    header_total_ = new wxStaticText(headerPanel, wxID_STATIC, wxT(""));

    wxBitmap pic(led_off_xpm);
    refresh_button_ = new wxBitmapButton( headerPanel, wxID_REFRESH, pic);
    refresh_button_->SetToolTip(_("Refresh Stock Prices from Yahoo"));

    wxBoxSizer* itemBoxSizerHHeader = new wxBoxSizer(wxHORIZONTAL);

    itemBoxSizerHHeader->Add(refresh_button_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemBoxSizerHHeader->Add(header_text_, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    itemBoxSizerVHeader->Add(itemBoxSizerHHeader, 1, wxEXPAND, 1);
    itemBoxSizerVHeader->Add(header_total_, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    /* ---------------------- */
    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow(this,
            ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(200, 200),
            wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER);

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
    m_imageList->Add(wxBitmap(wxImage(uparrow_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(downarrow_red_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(uparrow_xpm).Scale(16, 16)));
    m_imageList->Add(wxBitmap(wxImage(downarrow_xpm).Scale(16, 16)));

    listCtrlAccount_ = new stocksListCtrl(this, itemSplitterWindow10,
                                           ID_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize,
                                           wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL);
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    wxListItem itemCol;

    const wxChar* columns[] = {
        _("Purchase Date"),
        _("Share Name"),
        _("Number of Shares"),
        _("Value"),
        _("Gain/Loss"),
        _("Current"),
        _("Notes") };

    for (int i = 0; i < COL_MAX; ++i)
    {
        itemCol.SetText(wxString() << columns[i]);
        listCtrlAccount_->InsertColumn(i, columns[i], (i<2 || i>5 ? wxLIST_FORMAT_LEFT : wxLIST_FORMAT_RIGHT));

        int col_x = core_->iniSettings_->GetIntSetting(wxString::Format(wxT("STOCKS_COL%d_WIDTH"), i), -2);
        listCtrlAccount_->SetColumnWidth(i, col_x);
    }

    wxPanel* BottomPanel = new wxPanel(itemSplitterWindow10, wxID_ANY,
                                        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    itemSplitterWindow10->SplitHorizontally(listCtrlAccount_, BottomPanel);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* BoxSizerVBottom = new wxBoxSizer(wxVERTICAL);
    BottomPanel->SetSizer(BoxSizerVBottom);

    wxBoxSizer* BoxSizerHBottom = new wxBoxSizer(wxHORIZONTAL);
    BoxSizerVBottom->Add(BoxSizerHBottom, 1, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton6 = new wxButton(BottomPanel, wxID_NEW);
    itemButton6->SetToolTip(_("New Stock Investment"));
    BoxSizerHBottom->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    wxButton* itemButton81 = new wxButton(BottomPanel, wxID_EDIT);
    itemButton81->SetToolTip(_("Edit Stock Investment"));
    BoxSizerHBottom->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton81->Enable(false);

    wxButton* itemButton7 = new wxButton(BottomPanel, wxID_DELETE);
    itemButton7->SetToolTip(_("Delete Stock Investment"));
    BoxSizerHBottom->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton7->Enable(false);

    wxButton* bMove = new wxButton(BottomPanel, wxID_MOVE_FRAME, _("&Move"));
    bMove->SetToolTip(_("Move selected transaction to another account"));
    BoxSizerHBottom->Add(bMove, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    bMove->Enable(false);

    //Infobar-mini
    stock_details_short_ = new wxStaticText(BottomPanel, wxID_STATIC, strLastUpdate_);
    BoxSizerHBottom->Add(stock_details_short_, 1, wxGROW|wxTOP, 12);
    //Infobar
    stock_details_ = new wxStaticText(BottomPanel, wxID_STATIC, wxT(""),
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP);
    BoxSizerVBottom->Add(stock_details_, 1, wxGROW|wxLEFT|wxRIGHT, 14);

    updateExtraStocksData(-1);
}

int mmStocksPanel::initVirtualListControl(int id, int col, bool asc)
{
    /* Clear all the records */
    trans_.clear();
    listCtrlAccount_->DeleteAllItems();

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(asc ? 3 : 2);
    listCtrlAccount_->SetColumn(col, item);

    wxString str = core_->accountList_.GetAccountName(accountID_);
    header_text_->SetLabel(wxString::Format(_("Stock Investments: %s"), str.c_str()));

    core_->currencyList_.LoadBaseCurrencySettings(core_->dbInfoSettings_.get());
    double originalVal = 0.0;

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(accountID_).lock();
    wxASSERT(pCurrencyPtr);
    mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

    //Get Init Value of the account
    double initVal = core_->accountList_.GetAccountSharedPtr(accountID_)->initialBalance_;
    // + Transfered from other accounts - Transfered to other accounts

    //Get Stock Investment Account Balance as Init Amount + sum (Value) - sum (Purchase Price)
    double total = mmDBWrapper::getStockInvestmentBalance(core_->db_.get(), accountID_, false, originalVal);
    wxString balance;
    mmex::formatDoubleToCurrency(total+initVal, balance);

    wxString original;
    mmex::formatDoubleToCurrency(originalVal, original);

    //
    wxString diffStr;
    mmex::formatDoubleToCurrency((total > originalVal ? total - originalVal : originalVal - total), diffStr);

    wxString lbl;
    lbl << _("Total: ") << balance << wxT ("     ") << _("Invested: ") << original;

    //Percent
    wxString diffStrPercents;
    if (originalVal!= 0.0) {
        if (total > originalVal)
            lbl << wxT ("     ") << _("Gain: ");
        else
            lbl << wxT ("     ") << _("Loss: ");
        double diffPercents = (total > originalVal ? total/originalVal*100.0-100.0 : -(total/originalVal*100.0-100.0));
        mmex::formatDoubleToCurrencyEdit(diffPercents, diffStrPercents);
        lbl << diffStr << wxT("  ( ") << diffStrPercents << wxT(" %)");
    }

    header_total_->SetLabel(lbl);

    const  wxString sql =  wxString::FromUTF8(SELECT_ROW_HELDAT_FROM_STOCK_V1)
        + wxT(" order by ") + (wxString()<<col+1)
        + (!asc ? wxT(" desc") : wxT(" "));

    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(sql);
    st.Bind(1, accountID_);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    int cnt = 0, selected_item = -1;

    for ( ; q1.NextRow(); ++cnt)
    {
        mmStockTransactionHolder th;

        th.id_                = q1.GetInt(wxT("STOCKID"));
        th.stockPDate_        = q1.GetString(wxT("PURCHDATE"));
        int accountID         = q1.GetInt(wxT("HELDAT"));
        th.stockSymbol_       = q1.GetString (wxT ("SYMBOL"));
        th.heldAt_            = core_->accountList_.GetAccountName(accountID);
        th.shareName_         = q1.GetString(wxT("STOCKNAME"));
        th.shareNotes_        = q1.GetString(wxT("NOTES"));
        th.numSharesStr_      = wxT("");
        th.numShares_         = q1.GetDouble(wxT("NUMSHARES"));
        th.totalnumShares_    = q1.GetDouble(wxT("TOTAL_NUMSHARES"));
        th.purchasedTime_     = q1.GetInt (wxT ("PURCHASEDTIME"));

        th.currentPrice_      = q1.GetDouble(wxT("CURRENTPRICE"));
        th.purchasePrice_     = q1.GetDouble(wxT("PURCHASEPRICE"));
        th.avgpurchasePrice_  = q1.GetDouble (wxT ("AVG_PURCHASEPRICE"));
        th.value_             = q1.GetDouble(wxT("VALUE"));
        th.commission_        = q1.GetDouble(wxT("COMMISSION"));
        th.stockDays_         = q1.GetDouble (wxT ("DAYSOWN"));

        if (th.id_ == id) selected_item = cnt;
        th.gainLoss_          = th.value_ - ((th.numShares_ * th.purchasePrice_) + th.commission_);
        mmex::formatDoubleToCurrencyEdit(((th.value_ / ((th.numShares_ * th.purchasePrice_) + th.commission_)-1.0)*100.0 * 365.0 / th.stockDays_), th.stockPercentagePerYearStr_);

        mmex::formatDoubleToCurrencyEdit(th.gainLoss_, th.gainLossStr_);
        mmex::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);
        mmex::formatDoubleToCurrencyEdit(th.currentPrice_, th.cPriceStr_);
        mmex::formatDoubleToCurrencyEdit(th.avgpurchasePrice_, th.avgpurchasePriceStr_);

        //I wish see integer if it integer else double
        if ((th.numShares_ - static_cast<long>(th.numShares_)) != 0.0)
            th.numSharesStr_=wxString::Format(wxT("%.4f"),th.numShares_);
        else
            th.numSharesStr_ <<  static_cast<long>(th.numShares_);

        if ((th.totalnumShares_ - static_cast<long>(th.totalnumShares_)) != 0.0)
            mmex::formatDoubleToCurrencyEdit(th.totalnumShares_, th.totalnumSharesStr_);
        else
            th.totalnumSharesStr_ <<  static_cast<long>(th.totalnumShares_);

        //sqlite does not support %y date mask therefore null value should be replaces
        if (th.stockPDate_ == wxT(""))
        {
            wxString dateString = q1.GetString(wxT("PURCHASEDATE"));
            wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
            th.stockPDate_ = mmGetDateForDisplay(core_->db_.get(), dtdt);
        }

        trans_.push_back(new mmStockTransactionHolder(th));
    }

    st.Finalize();
    listCtrlAccount_->SetItemCount(cnt);
    return selected_item;
}

void mmStocksPanel::OnRefreshQuotes(wxCommandEvent& WXUNUSED(event))
{
    OrderQuoteRefresh();
}

void mmStocksPanel::OnDeleteStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnDeleteStocks(event);
}

void mmStocksPanel::OnMoveStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnMoveStocks(event);
}

void mmStocksPanel::OnNewStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnNewStocks(event);
}

void mmStocksPanel::OnEditStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnEditStocks(event);
}

void mmStocksPanel::OrderDownloadIfRequired(void)
{
    OrderQuoteRefresh();
}

/*** Trigger a quote download ***/
void mmStocksPanel::OrderQuoteRefresh(void)
{
    if(trans_.size() < 1) return;

    wxString YSymbols = wxT("");
    wxSortedArrayString symbols_array;

    for (size_t i = 0; i < trans_.size(); ++i)
    {
        wxString symbol = trans_[i]->stockSymbol_.Upper();
        if (symbols_array.Index(symbol) == wxNOT_FOUND) {
            symbols_array.Add(symbol);
            YSymbols += symbol + wxT("+");
        }
    }
    YSymbols.RemoveLast(1);

    //http://finance.yahoo.com/d/quotes.csv?s=SBER03.ME+GZPR.ME&f=sl1n&e=.csv
    wxString site = wxString::Format(wxT("http://download.finance.yahoo.com/d/quotes.csv?s=%s&f=sl1n&e=.csv"),
        YSymbols.c_str());

    refresh_button_->SetBitmapLabel(wxBitmap(wxImage(led_yellow_xpm).Scale(16,16)));
    stock_details_->SetLabel(_("Connecting..."));
    wxString quotes;

    int err_code = site_content(site, quotes);
    if (err_code != wxURL_NOERR)
    {
        refresh_button_->SetBitmapLabel(wxBitmap(wxImage(led_red_xpm).Scale(16,16)));
        stock_details_->SetLabel(quotes);
        stock_details_short_->SetLabel(wxT(""));
        return;
    }

    //--//
    wxStringTokenizer tkz(quotes, wxT("\r\n"));
    wxString StockSymbolWithSuffix, dName;
    bool updated;
    double dPrice = 0.0;
    //Symbol, Amount, Name
    std::map<wxString, std::pair<double, wxString> > stocks_data;

    while (tkz.HasMoreTokens())
    {
        wxString csvline = tkz.GetNextToken();
        updated = true;
        /*** Grab the relevant bits (for now only the symbol and the current price) */
        wxStringTokenizer csvsimple(csvline,wxT("\","),wxTOKEN_STRTOK);
        if (csvsimple.HasMoreTokens())
        {
            StockSymbolWithSuffix = csvsimple.GetNextToken();
            if (csvsimple.HasMoreTokens())
            {
                csvsimple.GetNextToken().ToDouble(&dPrice);
                if (csvsimple.HasMoreTokens())
                    dName = csvsimple.GetNextToken();
                else
                    updated = false;
            }
            else
                updated = false;
        }
        else
            updated = false;

        //**** HACK HACK HACK
        // Note:
        // 1. If the share is a UK share (e.g. HSBA.L), its downloaded value in pence
        // 2. If the share is not a UK share (e.g. 0005.HK) while we are using UK Yahoo finance, we do not need
        //    to modify the price

        //// UK finance apparently downloads values in pence
        //if (!yahoo_->Server_.CmpNoCase(wxT("uk.finance.yahoo.com")))
        //    dPrice = dPrice / 100;
        //// ------------------
        if (updated && dPrice > 0)
        {
            if(StockSymbolWithSuffix.substr(StockSymbolWithSuffix.Find(wxT("."))) == wxT(".L"))
                dPrice = dPrice / 100;
            stocks_data.insert(std::make_pair(StockSymbolWithSuffix, std::make_pair(dPrice, dName)));
        }
    }

    typedef std::vector<mmStockTransactionHolder> vec_t;
    vec_t stockVec;

    for (size_t i = 0; i < trans_.size(); ++i)
    {
        mmStockTransactionHolder sh;

        std::pair<double, wxString> data = stocks_data[trans_[i]->stockSymbol_.Upper()];
        dPrice = data.first;

        sh.id_ = trans_[i]->id_;
        sh.numShares_ = trans_[i]->numShares_; //q1.GetDouble(wxT("NUMSHARES"));
        // If the stock's symbol is not found, Yahoo CSV will return 0 for the current price.
        // Therefore, we assume the current price of all existing stock's symbols are greater
        // than zero and we will not update any stock if its curreny price is zero.
        if(dPrice == 0 || sh.numShares_ < 0.0) dPrice = trans_[i]->currentPrice_ /*q1.GetDouble(wxT("CURRENTPRICE")*/;
        sh.shareName_ = trans_[i]->shareName_; //q1.GetString (wxT ("STOCKNAME"));
        if (sh.shareName_.IsEmpty()) sh.shareName_ = data.second;

        sh.currentPrice_ = dPrice;
        sh.value_ = sh.numShares_ * dPrice;
        stockVec.push_back(sh);
    }

    //--//

    core_->db_.get()->Begin();
    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(UPDATE_STOCK_V1);

    for (vec_t::const_iterator i = stockVec.begin(); i != stockVec.end(); ++i)
    {
        st.Bind(1, i->currentPrice_);
        st.Bind(2, i->value_);
        st.Bind(3, i->shareName_);
        st.Bind(4, i->id_);

        st.ExecuteUpdate();
        st.Reset();
    }
    st.Finalize();
    core_->db_.get()->Commit();

    // Now refresh the display
    int selected_id = -1;
    if (listCtrlAccount_->get_selectedIndex() > -1)
        selected_id = trans_[listCtrlAccount_->get_selectedIndex()]->id_;
    listCtrlAccount_->doRefreshItems(selected_id);

    // We are done!
    LastRefreshDT_       = wxDateTime::Now();
    StocksRefreshStatus_ = true;
    refresh_button_->SetBitmapLabel(wxBitmap(wxImage(led_green_xpm).Scale(16,16)));

    strLastUpdate_.Printf(_("%s on %s"), LastRefreshDT_.FormatTime().c_str(),
                             LastRefreshDT_.FormatDate().c_str());
    core_->dbInfoSettings_->SetStringSetting(wxT("STOCKS_LAST_REFRESH_DATETIME"), strLastUpdate_);

    stock_details_->SetLabel(_("Stock prices successfully updated"));
    stock_details_short_->SetLabel(wxString::Format(_("Last updated %s"), strLastUpdate_.c_str()));

}

void stocksListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Stock Investment"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Stock Investment"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Stock Investment"));
    PopupMenu(&menu, event.GetPoint());
}

wxString mmStocksPanel::getItem(long item, long column)
{
    if (column == COL_DATE)         return trans_[item]->stockPDate_;
    if (column == COL_NAME)         return trans_[item]->shareName_;
    if (column == COL_NUMBER)       return trans_[item]->numSharesStr_;
    if (column == COL_GAIN_LOSS)    return trans_[item]->gainLossStr_;
    if (column == COL_VALUE)        return trans_[item]->valueStr_;
    if (column == COL_CURRENT)      return trans_[item]->cPriceStr_;
    if (column == COL_NOTES)        return trans_[item]->shareNotes_;

    return wxT("");
}

wxString stocksListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

void stocksListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    cp_->updateExtraStocksData(selectedIndex_);
    cp_->enableEditDeleteButtons(true); //Unhide the Edit and Delete buttons if any record selected
}

void stocksListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
    selectedIndex_ = -1;
    cp_->enableEditDeleteButtons(false); //Hide the Edit and Delete buttons if no records selected
    cp_->updateExtraStocksData(selectedIndex_);
}

void mmStocksPanel::updateExtraStocksData(int selectedIndex)
{
    if (selectedIndex == -1)
    {
        stock_details_->SetLabel(Tips(TIPS_STOCK));
        stock_details_short_->SetLabel(wxString::Format(_("Last updated %s"), strLastUpdate_.c_str()));
    }
    else
    {
        wxString stockPurchasePriceStr;
        wxString stockCurrentPriceStr;
        wxString stockDifferenceStr;
        wxString stocktotalDifferenceStr;
        wxString stockPercentageStr;
        wxString stocktotalPercentageStr;
        wxString stockPercentagePerYearStr;
        wxString stockavgPurchasePriceStr;
        wxString stocknumSharesStr = trans_[selectedIndex]->numSharesStr_;
        wxString stocktotalnumSharesStr = trans_[selectedIndex]->totalnumSharesStr_;
        wxString stockgainlossStr = trans_[selectedIndex]->gainLossStr_;
        wxString stocktotalgainlossStr;

        double stockPurchasePrice = trans_[selectedIndex]->purchasePrice_;
        double stockCurrentPrice = trans_[selectedIndex]->currentPrice_;
        double stockDifference = stockCurrentPrice - stockPurchasePrice;

        double stockavgPurchasePrice = trans_[selectedIndex]->avgpurchasePrice_;
        double stocktotalDifference = stockCurrentPrice - stockavgPurchasePrice;
        double stockDaysOwn = trans_[selectedIndex]->stockDays_;
        //Commision don't calculates here
        double stockPercentage = (stockCurrentPrice/stockPurchasePrice-1.0)*100.0;
        double stockPercentagePerYear = stockPercentage * 365.0 / stockDaysOwn;
        double stocktotalPercentage = (stockCurrentPrice/stockavgPurchasePrice-1.0)*100.0;
        //  double stocknumShares = trans_[selectedIndex]->numShares_;
        double stocktotalnumShares = trans_[selectedIndex]->totalnumShares_;
        double stocktotalgainloss = stocktotalDifference * stocktotalnumShares;

        mmex::formatDoubleToCurrencyEdit(stockPurchasePrice, stockPurchasePriceStr);
        mmex::formatDoubleToCurrencyEdit(stockavgPurchasePrice, stockavgPurchasePriceStr);
        mmex::formatDoubleToCurrencyEdit(stockCurrentPrice, stockCurrentPriceStr);
        mmex::formatDoubleToCurrencyEdit(stockDifference , stockDifferenceStr);
        mmex::formatDoubleToCurrencyEdit(stocktotalDifference , stocktotalDifferenceStr);
        mmex::formatDoubleToCurrencyEdit(stockPercentage, stockPercentageStr);
        mmex::formatDoubleToCurrencyEdit(stockPercentagePerYear, stockPercentagePerYearStr);
        mmex::formatDoubleToCurrencyEdit(stocktotalPercentage, stocktotalPercentageStr);
        mmex::formatDoubleToCurrencyEdit(stocktotalgainloss, stocktotalgainlossStr);
        //mmex::formatDoubleToCurrencyEdit(stocknumShares, stocknumSharesStr);

        wxString miniInfo = wxT("");
        if (trans_[selectedIndex]->stockSymbol_ != wxT(""))
        miniInfo << wxT("\t") << _("Symbol: ") << trans_[selectedIndex]->stockSymbol_ << wxT ("\t\t");
        miniInfo << _ ("Total:") << wxT (" (") << trans_[selectedIndex]->totalnumSharesStr_ << wxT (") ");
        //If some share has been bot for a short period we don't need that info because the forecast may be too optimistic
        //if (stockDaysOwn > 182.5)
        //miniInfo << wxT ("\t\t") << _("Percent/Year: ") << trans_[selectedIndex]->stockPercentagePerYearStr_;
        stock_details_short_->SetLabel(miniInfo);

        wxString additionInfo = wxT("");
        //Selected share info
        additionInfo
        << wxT("|") << stockCurrentPriceStr << wxT(" - ") << stockPurchasePriceStr << wxT("|") << wxT(" = ") << stockDifferenceStr
        << wxT (" * ") << stocknumSharesStr << wxT (" = ") << stockgainlossStr << wxT (" ( ") << stockPercentageStr << wxT ('%')
        //<< wxT (" | ")<< stockPercentagePerYearStr << wxT("% ")  << _("Yearly")
        << wxT (" )") << wxT ("\n");
        //Summary for account for selected symbol
        if (trans_[selectedIndex]->purchasedTime_ > 1)
        {
            additionInfo << wxT("|") << stockCurrentPriceStr << wxT(" - ") << stockavgPurchasePriceStr << wxT("|") << wxT(" = ") << stocktotalDifferenceStr
            << wxT (" * ") << stocktotalnumSharesStr << wxT (" = ") << stocktotalgainlossStr << wxT (" ( ") << stocktotalPercentageStr << wxT ('%')
            //<< wxT (" | ")<< stockPercentagePerYearStr << wxT("% ") << _("Yearly")
            << wxT (" )") //<< wxT ("\n")
            << wxT ("\n") << getItem(selectedIndex, COL_NOTES);
        }

        stock_details_->SetLabel(additionInfo);
    }
}

void mmStocksPanel::enableEditDeleteButtons(bool en)
{
    wxButton* bE = (wxButton*)FindWindow(wxID_EDIT);
    wxButton* bD = (wxButton*)FindWindow(wxID_DELETE);
    wxButton* bM = (wxButton*)FindWindow(wxID_MOVE_FRAME);
    bE->Enable(en);
    bD->Enable(en);
    bM->Enable(en);
}

int stocksListCtrl::OnGetItemImage(long item) const
{
    /* Returns the icon to be shown for each entry */
    if (cp_->trans_[item]->gainLoss_ > 0) return 0;
    return 1;
}

wxListItemAttr* stocksListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void stocksListCtrl::OnListKeyDown(wxListEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_DELETE:
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,  MENU_TREEPOPUP_DELETE);
                OnDeleteStocks(evt);
            }
            break;

        default:
            event.Skip();
            break;
    }
}

void stocksListCtrl::OnNewStocks(wxCommandEvent& /*event*/)
{
    mmStockDialog dlg(cp_->core_, 0, false, cp_->accountID_, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        doRefreshItems(dlg.transID_);
    }
}

void stocksListCtrl::OnDeleteStocks(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the stock investment?"),
                           _("Confirm Stock Investment Deletion"),wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        mmDBWrapper::deleteStockInvestment(cp_->core_->db_.get(), cp_->trans_[selectedIndex_]->id_);
        DeleteItem(selectedIndex_);
        doRefreshItems(-1);
    }
}

void stocksListCtrl::OnMoveStocks(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;

    wxArrayString accounts_type;
    accounts_type.Add(ACCOUNT_TYPE_STOCK);
    wxArrayInt accounts_id = cp_->core_->accountList_.getAccountsID(accounts_type, cp_->accountID_);
    if (accounts_id.Count() < 1) return;

    wxArrayString accounts_name;
    for (size_t i = 0; i < accounts_id.Count(); ++i)
    {
        accounts_name.Add(cp_->core_->accountList_.GetAccountName(accounts_id[i]));
    }

    wxString headerMsg = wxString::Format(_("Moving Transaction from %s to...")
        ,cp_->core_->accountList_.GetAccountName(cp_->accountID_).c_str());
    wxSingleChoiceDialog scd(this, _("Select the destination Account "), headerMsg , accounts_name);

    int toAccountID = -1;
    int error_code = scd.ShowModal();
    if (error_code == wxID_OK)
    {
        wxString acctName = scd.GetStringSelection();
        toAccountID = cp_->core_->accountList_.GetAccountId(acctName);
    }

    if ( toAccountID != -1 )
    {
        cp_->core_->db_.get()->Begin();
        if (mmDBWrapper::moveStockInvestment(cp_->core_->db_.get(), cp_->trans_[selectedIndex_]->id_, toAccountID))
            DeleteItem(selectedIndex_);
        cp_->core_->db_.get()->Commit();
    }

    if (error_code == wxID_OK)
        doRefreshItems(-1);
}

void stocksListCtrl::OnEditStocks(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ < 0) return;

    wxListEvent evt(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, ID_PANEL_STOCKS_LISTCTRL);
    AddPendingEvent(evt);
}

void stocksListCtrl::OnListItemActivated(wxListEvent& /*event*/)
{
    cp_->call_dialog(selectedIndex_);
    cp_->updateExtraStocksData(selectedIndex_);
}

void mmStocksPanel::call_dialog(const int selectedIndex)
{
    mmStockDialog dlg(core_, trans_[selectedIndex], true, accountID_, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        listCtrlAccount_->doRefreshItems(dlg.transID_);
    }
}

void stocksListCtrl::OnColClick(wxListEvent& event)
{
    if(0 > event.GetColumn() || event.GetColumn() >= COL_MAX) return;

    if (m_selected_col == event.GetColumn()) m_asc = !m_asc;

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(-1);
    cp_->listCtrlAccount_->SetColumn(m_selected_col, item);

    m_selected_col = event.GetColumn();

    item.SetImage(m_asc ? 3 : 2);
    SetColumn(m_selected_col, item);

    int trx_id = -1;
    if (selectedIndex_>=0) trx_id = cp_->trans_[selectedIndex_]->id_;
    doRefreshItems(trx_id);
    cp_->updateExtraStocksData(-1);
}

void stocksListCtrl::doRefreshItems(int trx_id)
{
    int selectedIndex = cp_->initVirtualListControl(trx_id, m_selected_col, m_asc);
    long cnt = static_cast<long>(cp_->trans_.size());

    if (selectedIndex >= cnt || selectedIndex < 0)
        selectedIndex = m_asc ? cnt - 1 : 0;

    if (cnt>0)
    {
        RefreshItems(0, cnt > 0 ? cnt - 1 : 0);
    }
    else
        selectedIndex = -1;

    if (selectedIndex >= 0 && cnt>0)
    {
        SetItemState(selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        SetItemState(selectedIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        EnsureVisible(selectedIndex);
    }
}

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
#include "dbwrapper.h"
#include "stockdialog.h"
#include "mmyahoo.h"
#include "yahoosettingsdialog.h"

#include "mmex_db_view.h"
#include <boost/foreach.hpp>

#include "../resources/leds.xpm"

/*******************************************************/

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

BEGIN_EVENT_TABLE(mmStocksPanel, wxPanel)
    EVT_BUTTON(wxID_NEW,         mmStocksPanel::OnNewStocks)
    EVT_BUTTON(wxID_EDIT,        mmStocksPanel::OnEditStocks)
    EVT_BUTTON(wxID_DELETE,      mmStocksPanel::OnDeleteStocks)
    EVT_BUTTON(wxID_REFRESH,     mmStocksPanel::OnRefreshQuotes)
    EVT_BUTTON(wxID_SETUP,     mmStocksPanel::OnHTTPSettings)
    EVT_TIMER(ID_TIMER_SCHEDULE_STOCK,      mmStocksPanel::OnScheduleTimer)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(stocksListCtrl, mmListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_STOCKS_LISTCTRL,   stocksListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_STOCKS_LISTCTRL, stocksListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_STOCKS_LISTCTRL,    stocksListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(ID_PANEL_STOCKS_LISTCTRL,    stocksListCtrl::OnListItemDeselected)

    EVT_MENU(wxID_NEW,              stocksListCtrl::OnNewStocks)
    EVT_MENU(wxID_EDIT,             stocksListCtrl::OnEditStocks)
    EVT_MENU(wxID_DELETE,           stocksListCtrl::OnDeleteStocks)

    EVT_LIST_KEY_DOWN(ID_PANEL_STOCKS_LISTCTRL,   stocksListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/
mmStocksPanel::mmStocksPanel(wxSQLite3Database* db, mmCoreDB* core,
                             int accountID,
                             wxWindow *parent,
                             wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
        : mmPanelBase(db, core), accountID_(accountID)
{
    Create(parent, winid, pos, size, style, name);
}

bool mmStocksPanel::Create(wxWindow *parent,
                            wxWindowID winid, const wxPoint& pos,
                            const wxSize& size,long style, const wxString& name)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

#ifdef __WXMSW__
    Freeze();
#endif

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    initVirtualListControl();
    if (trans_.size() > 1)
        listCtrlStock_->EnsureVisible(((int)trans_.size()) - 1);

    // Greg Newton

    wxConfigBase *config = wxConfigBase::Get();
    strLastUpdate_ = config->Read("STOCKS_LAST_REFRESH_DATETIME", "");

    updateExtraStocksData(-1);

    DownloadScheduleTimer_ = NULL;
    StatusRefreshTimer_ = NULL;

    DownloadScheduleTimer_ = new wxTimer(this, ID_TIMER_SCHEDULE_STOCK);
    DownloadScheduleTimer_->Start(config->ReadLong("STOCKS_REFRESH_MINUTES", 30) * 60000, wxTIMER_CONTINUOUS);

#ifdef __WXMSW__
    Thaw();
#endif

    return TRUE;
}

mmStocksPanel::~mmStocksPanel()
{
    if (DownloadScheduleTimer_)
    {
        DownloadScheduleTimer_->Stop();
        delete DownloadScheduleTimer_;
    }

    if (StatusRefreshTimer_)
    {
        StatusRefreshTimer_->Stop();
        delete StatusRefreshTimer_;
    }

    //if (yahoo_) delete yahoo_;
    this->save_config(listCtrlStock_, "STOCKS");
}

void mmStocksPanel::CreateControls()
{
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer9);

    wxSizerFlags flags;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 4);
    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel(this,
        wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer9->Add(headerPanel);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    wxStaticText* header_text = new wxStaticText(headerPanel,
        ID_PANEL_BD_STATIC_HEADER, _("Stock Investments"));
    int font_size = this->GetFont().GetPointSize() + 2;
    header_text->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxBOLD, FALSE, ("")));

    wxStaticText* itemStaticText10 = new wxStaticText(headerPanel,
                                     ID_PANEL_CHECKING_STATIC_BALHEADER,
                                     _("Total:"));
    wxBitmap pic(led_off_xpm);
    m_LED = new wxStaticBitmap(headerPanel, ID_PANEL_STOCK_UPDATE_LED, pic);

    wxBoxSizer* itemBoxSizerHHeader = new wxBoxSizer(wxHORIZONTAL);

    itemBoxSizerHHeader->Add(m_LED, flags);
    itemBoxSizerHHeader->Add(header_text, flags);

    itemBoxSizerVHeader->Add(itemBoxSizerHHeader);
    itemBoxSizerVHeader->Add(itemStaticText10, flags);

    /* ---------------------- */
    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow(this,
            ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(200, 200),
            wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER);

    listCtrlStock_ = new stocksListCtrl(this, itemSplitterWindow10,
                                           ID_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize,
                                           wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL);
    wxListItem itemCol;
    itemCol.SetImage(-1);
    const wxChar* columns[] = { _("Purchase Date"),
                             _("Share Name"),
                             _("Number of Shares"),
                             _("Gain/Loss"),
                             _("Value"),
                             _("Current"),
                             _("Notes")};

    wxConfigBase *config = wxConfigBase::Get();
    for (int i = 0; i < COL_MAX; ++i)
    {
        itemCol.SetText(wxString() << columns[i]);
        listCtrlStock_->InsertColumn(i, columns[i], (i<2 || i>5 ? wxLIST_FORMAT_LEFT : wxLIST_FORMAT_RIGHT));

        long col_x = config->ReadLong(wxString::Format("STOCKS_COL%i_WIDTH", i), -2);
        listCtrlStock_->SetColumnWidth(i, col_x);
    }

    wxPanel* itemPanel12 = new wxPanel(itemSplitterWindow10,
        ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    itemSplitterWindow10->SplitHorizontally(listCtrlStock_, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5);

    wxButton* itemButton6 = new wxButton(itemPanel12, wxID_NEW);
    itemBoxSizer5->Add(itemButton6, flags);

    wxButton* itemButton81 = new wxButton(itemPanel12, wxID_EDIT);
    itemButton81->SetToolTip(_("Edit Stock Investment"));
    itemBoxSizer5->Add(itemButton81, flags);
    itemButton81->Enable(false);

    wxButton* itemButton7 = new wxButton(itemPanel12, wxID_DELETE);
    itemButton7->SetToolTip(_("Delete Stock Investment"));
    itemBoxSizer5->Add(itemButton7, flags);
    itemButton7->Enable(false);

    wxButton* itemButton8 = new wxButton(itemPanel12, wxID_REFRESH);
    itemButton8->SetToolTip(_("Refresh Stock Prices from Yahoo"));
    itemBoxSizer5->Add(itemButton8, flags);

    wxButton* itemButton9 = new wxButton(itemPanel12, wxID_SETUP, _("&Settings"));
    itemButton9->SetToolTip(_("Change settings for automatic refresh"));
    itemBoxSizer5->Add(itemButton9, flags);

    //Infobar-mini
    stock_details_short_ = new wxStaticText(itemPanel12,
         ID_PANEL_STOCKS_STATIC_DETAILS_MINI, strLastUpdate_);
    itemBoxSizer5->Add(stock_details_short_, flags);
    //Infobar
    stock_details_ = new wxStaticText(itemPanel12,
    ID_PANEL_STOCKS_STATIC_DETAILS, ("                                         "),
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP);
    itemBoxSizer4->Add(stock_details_, 1, wxGROW|wxALL, 5);
}

void mmStocksPanel::initVirtualListControl()
{
    /* Clear all the records */
    trans_.clear();

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_BD_STATIC_HEADER);
    wxString str = core_->getAccountName(accountID_);
    header->SetLabel(_("Stock Investments: ") + str);

    mmDBWrapper::loadBaseCurrencySettings(db_);
    double originalVal = 0.0;

    mmDBWrapper::loadSettings(accountID_, db_);

    //Get Init Value of the account
    double initVal = core_->getAccountSharedPtr(accountID_)->initialBalance_;
    // + Transfered from other accounts - Transfered to other accounts

    //Get Stock Investment Account Balance as Init Amount + sum (Value) - sum (Purchase Price)
    double total = mmDBWrapper::getStockInvestmentBalance(db_, accountID_, false, originalVal);
    wxString balance;
    mmex::formatDoubleToCurrency(total+initVal, balance);

    wxString original;
    mmex::formatDoubleToCurrency(originalVal, original);

    //
    wxString diffStr;
    mmex::formatDoubleToCurrency((total > originalVal ? total - originalVal : originalVal - total), diffStr);

    header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);
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
        lbl << diffStr << ("  ( ") << diffStrPercents << (" %)");
    }

    header->SetLabel(lbl);

    char sql[] =
    "select S.STOCKID, S.HELDAT,  S.STOCKNAME, UPPER(S.SYMBOL) SYMBOL, "
    "S.NUMSHARES as NUMSHARES, t.TOTAL_NUMSHARES, T.PURCHASEDTIME, "
    "T.AVG_PURCHASEPRICE as AVG_PURCHASEPRICE, "
    "S.PURCHASEPRICE as PURCHASEPRICE, "
    "S.CURRENTPRICE as CURRENTPRICE, "
    "S.VALUE as VALUE, S.COMMISSION as COMMISSION, "
    "ifnull (strftime(INFOVALUE, S.PURCHASEDATE),strftime(replace (i.infovalue, '%y', SubStr (strftime('%Y', S.PURCHASEDATE),3,2)),S.PURCHASEDATE)) as PURCHDATE, "
    "S.PURCHASEDATE as PURCHASEDATE, "
    "julianday('now', 'localtime')-julianday (S.PURCHASEDATE, 'localtime') as DAYSOWN, "
    "S.NOTES "
    "from STOCK_V1 S "
    "left join infotable_v1 i on i.INFONAME='DATEFORMAT' "
    "left join ( "
    "select count (UPPER (SYMBOL)) as PURCHASEDTIME, "
    "HELDAT, UPPER (SYMBOL) as SYMBOL, "
    "total (NUMSHARES) as TOTAL_NUMSHARES, "
    "total(PURCHASEPRICE*NUMSHARES)/total(NUMSHARES) as AVG_PURCHASEPRICE "
    "from STOCK_V1 "
    "left join infotable_v1 i on i.INFONAME='DATEFORMAT' "
    "group by HELDAT, UPPER (SYMBOL) "
    "order by julianday(min (PURCHASEDATE),'localtime'), SYMBOL, STOCKNAME "
    ") T on UPPER (T.SYMBOL)=UPPER (S.SYMBOL) and T.HELDAT=S.HELDAT "
    "where S.HELDAT = ? "
    "order by julianday((S.PURCHASEDATE),'localtime'), S.SYMBOL, S.STOCKNAME ";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, accountID_);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    int cnt = 0;

    for ( ; q1.NextRow(); ++cnt)
    {
        mmStockTransactionHolder th;

        th.id_           = q1.GetInt(("STOCKID"));
        th.stockPDate_        = q1.GetString(("PURCHDATE"));
        int accountID         = q1.GetInt(("HELDAT"));
        th.stockSymbol_       = q1.GetString (wxT ("SYMBOL"));
        th.heldAt_            = core_->getAccountName(accountID);
        th.shareName_         = q1.GetString(("STOCKNAME"));
        th.shareNotes_        = q1.GetString(("NOTES"));
        th.numSharesStr_      = ("");
        th.numShares_         = q1.GetDouble(("NUMSHARES"));
        th.totalnumShares_    = q1.GetDouble(("TOTAL_NUMSHARES"));
        th.purchasedTime_     = q1.GetInt (wxT ("PURCHASEDTIME"));

        th.currentPrice_      = q1.GetDouble(("CURRENTPRICE"));
        th.purchasePrice_     = q1.GetDouble(("PURCHASEPRICE"));
        th.avgpurchasePrice_  = q1.GetDouble (wxT ("AVG_PURCHASEPRICE"));
        th.value_             = q1.GetDouble(("VALUE"));
        double commission     = q1.GetDouble(("COMMISSION"));
        th.stockDays_         = q1.GetDouble (wxT ("DAYSOWN"));

        th.gainLoss_          = th.value_ - ((th.numShares_ * th.purchasePrice_) + commission);
        mmex::formatDoubleToCurrencyEdit(((th.value_ / ((th.numShares_ * th.purchasePrice_) + commission)-1.0)*100.0 * 365.0 / th.stockDays_), th.stockPercentagePerYearStr_);

        mmex::formatDoubleToCurrencyEdit(th.gainLoss_, th.gainLossStr_);
        mmex::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);
        mmex::formatDoubleToCurrencyEdit(th.currentPrice_, th.cPriceStr_);
        mmex::formatDoubleToCurrencyEdit(th.avgpurchasePrice_, th.avgpurchasePriceStr_);

        //I wish see integer if it integer else double
        if ((th.numShares_ - static_cast<long>(th.numShares_)) != 0.0)
            th.numSharesStr_=wxString::Format(("%.4f"),th.numShares_);
        else
            th.numSharesStr_ <<  static_cast<long>(th.numShares_);

        if ((th.totalnumShares_ - static_cast<long>(th.totalnumShares_)) != 0.0)
            mmex::formatDoubleToCurrencyEdit(th.totalnumShares_, th.totalnumSharesStr_);
        else
            th.totalnumSharesStr_ <<  static_cast<long>(th.totalnumShares_);

        //sqlite does not support %y date mask therefore null value should be replaces
        if (th.stockPDate_ == (""))
        {
            wxString dateString = q1.GetString(("PURCHASEDATE"));
            wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
            th.stockPDate_ = mmGetDateForDisplay(db_, dtdt);
        }

        trans_.push_back(th);
    }

    st.Finalize();
    listCtrlStock_->SetItemCount(cnt);
}

void mmStocksPanel::OnRefreshQuotes(wxCommandEvent& WXUNUSED(event))
{
    OrderQuoteRefresh();
}

void mmStocksPanel::OnHTTPSettings(wxCommandEvent& WXUNUSED(event))
{
    YahooSettingsDialog dlg(this);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxConfigBase *config = wxConfigBase::Get();
        // Copy dialog values back to yahoo_->
        //yahoo_->UpdatingEnabled_ = dlg.m_checkBoxRefreshPrices->GetValue();
        config->Write("STOCKS_REFRESH_ENABLED", dlg.m_checkBoxRefreshPrices->GetValue());
        //yahoo_->UpdateIntervalMinutes_ = dlg.m_RefreshInterval->GetValue();
        config->Write("STOCKS_REFRESH_MINUTES", dlg.m_RefreshInterval->GetValue());
        //yahoo_->Suffix_ = dlg.m_YahooSuffix->GetValue();
        config->Write("HTTP_YAHOO_SUFFIX", dlg.m_YahooSuffix->GetValue());
        //yahoo_->Server_ = dlg.m_YahooServer->GetValue();
        config->Write("HTTP_YAHOO_SERVER", dlg.m_YahooServer->GetValue());
        //yahoo_->OpenTimeStr_ = wxString::Format(("%02d:%02d:00"), dlg.m_MarketOpenHour->GetValue(),dlg.m_MarketOpenMinute->GetValue());
        config->Write("STOCKS_MARKET_OPEN_TIME",
            wxString::Format(("%02d:%02d:00"),
            dlg.m_MarketOpenHour->GetValue(),
            dlg.m_MarketOpenMinute->GetValue()));

        //yahoo_->CloseTimeStr_ = wxString::Format(("%02d:%02d:00"), dlg.m_MarketCloseHour->GetValue(),dlg.m_MarketCloseMinute->GetValue());
        config->Write("STOCKS_MARKET_CLOSE_TIME", wxString::Format(("%02d:%02d:00"),
            dlg.m_MarketCloseHour->GetValue(),
            dlg.m_MarketCloseMinute->GetValue()));
    }
}

void mmStocksPanel::OnDeleteStocks(wxCommandEvent& event)
{
    listCtrlStock_->OnDeleteStocks(event);
}

void mmStocksPanel::OnNewStocks(wxCommandEvent& event)
{
    listCtrlStock_->OnNewStocks(event);
}

void mmStocksPanel::OnEditStocks(wxCommandEvent& event)
{
    listCtrlStock_->OnEditStocks(event);
}

// The timer for scheduling quote updates
void mmStocksPanel::OnScheduleTimer(wxTimerEvent& WXUNUSED(event))
{
    OrderDownloadIfRequired();
}

void mmStocksPanel::OrderDownloadIfRequired(void)
{
    if (yahoo_->UpdatingEnabled_)
    {
        if (DownloadIsRequired())
            OrderQuoteRefresh();
        else {
            //yahoo_->StocksRefreshStatus_ = mmYahoo::DS_OUTOFHOURS;
            wxBitmap pic(led_yellow_xpm);
            m_LED->SetBitmap(pic);
        }
    }
    else
    {
        wxBitmap pic(led_off_xpm);
        m_LED->SetBitmap(pic);
        //yahoo_->StocksRefreshStatus_ = mmYahoo::DS_NOTAUTOMATIC;
    }
}

/** Compare current time with market hours, and with last update time **/
bool mmStocksPanel::DownloadIsRequired(void)
{
   wxConfigBase *config = wxConfigBase::Get();
   // If automatic updates are disabled, don't start a download.
    if (!config->ReadBool("STOCKS_REFRESH_ENABLED", false)) return false;\
    wxString datetime_str = config->Read("STOCKS_LAST_REFRESH_DATETIME", "");
    wxDateTime LastRefreshDT;
#if wxCHECK_VERSION(2,9,0)
    if (!LastRefreshDT.ParseDateTime(datetime_str))
#else 
    if (!LastRefreshDT.ParseDateTime(datetime_str.GetData()))
#endif
    LastRefreshDT = wxInvalidDateTime;
    if (LastRefreshDT.IsValid()) return true; // Invalid last update time, so let's do it!

    wxDateTime workingDateTime = wxDateTime::Now();

    // Get today's market open and close times
    wxDateTime MarketOpen = wxDateTime::Now();
    wxDateTime MarketClose = wxDateTime::Now();
    MarketOpen.ParseTime(config->Read("STOCKS_MARKET_OPEN_TIME", "10:15:00"));
    MarketClose.ParseTime(config->Read("STOCKS_MARKET_CLOSE_TIME", "16:40:00"));

    // If today is a work day,
    if (workingDateTime.IsWorkDay())
    {
        // and we're between open and close times...
        if(workingDateTime.IsBetween(MarketOpen, MarketClose)) return true;

        // If it's before the market opens, use yesterday
        if(workingDateTime.IsEarlierThan (MarketOpen))
        {
            // Go to yesterday for remaining calculations
            workingDateTime.Subtract(wxTimeSpan::Day());
        }
    }

    // Get the most recent work day (could be today)
    while (!workingDateTime.IsWorkDay())
        workingDateTime.Subtract(wxTimeSpan::Day());

    // If the last update was before the last market close
    MarketOpen.ParseDate(workingDateTime.FormatISODate().GetData());
    MarketClose.ParseDate(workingDateTime.FormatISODate().GetData());
    // ParseDate resets time to 00:00, so we re-ParseTime
    MarketOpen.ParseTime(config->Read("STOCKS_MARKET_OPEN_TIME", "10:15:00"));
    MarketClose.ParseTime(config->Read("STOCKS_MARKET_CLOSE_TIME", "16:40:00"));
    wxDateTime refresh_datetime;
    //FIXME:
    //if (refresh_datetime.ParseTime(config->Read("STOCKS_LAST_REFRESH_DATETIME", "")).IsEarlierThan(MarketClose)) return true;

    return false;
}

/*** Trigger a quote download ***/
void mmStocksPanel::OrderQuoteRefresh(void)
{
     /// Yahoo csv format key AFAIK
    /****************************************************************************************************************
     * s:  the symbol name                                                                                          *
     * l:  last value (or current price). If you use l alone, you will get back a string similar to                 *
     *                                                                                                              *
     *      Mar 22 - <b>31.26</b>                                                                                   *
     *                                                                                                              *
     * l1: (letter l and the number 1) to just get the last value of the symbol                                     *
     * d:  d alone will give you 0, while d1 will return the current date (e.g. 3/22/2007)                          *
     * t:  t by itself will request the yahoo-generated chart. However, you will get back the                       *
     *     chart image with a whole bunch of other HTML garbage, e.g.                                               *
     *                                                                                                              *
     *     <img border=0  width=512 height=288 src=http://chart.yahoo.com/c//y/yhoo.gif"                            *
     *     alt="Chart"><br><table><tr><td width=512 align=center><font face=arial size=-1></font></td></tr></table> *
     *                                                                                                              *
     * t1: the time of last update, for example 4:00pm.                                                             *
     * c:  the change amount. Can either be used as c or c1.                                                        *
     * o:  opening value                                                                                            *
     * h:  high value                                                                                               *
     * g:  low value                                                                                                *
     * v:  volume                                                                                                   *
     * j:  52-week low.                                                                                             *
     * j1: the market cap. This is string like 42.405B for $42 billion.                                       *
     * p:  after hour price (?)                                                                                     *
     * p1: (?)                                                                                                      *
     * p2: change percentage e.g.    -0.10                                                                          *
     * w:  52-week range                                                                                            *
     * e:  EPS (Earning per share)                                                                                  *
     * r:  P/E (Prince/Earning) ratio                                                                               *
     * n:  Company name                                                                                             *
     *                                                                                                              *
     ****************************************************************************************************************/

    wxConfigBase *config = wxConfigBase::Get();
    DB_View_STOCK_V1::Data_Set all_stocks = STOCK_V1.all(db_);
    if (all_stocks.empty())
        return;

    wxString suffix, site;
    wxSortedArrayString symbols_array;

    suffix = config->Read("HTTP_YAHOO_SUFFIX", "").Upper();
    bool suffix_available = !suffix.IsEmpty();

    BOOST_FOREACH(const DB_View_STOCK_V1::Data &stock, all_stocks)
    {
        if (wxNOT_FOUND == symbols_array.Index(stock.SYMBOL.Upper())) {
            symbols_array.Add(stock.SYMBOL.Upper());
            site << stock.SYMBOL.Upper() << suffix << ("+");
        }
    }
    site.RemoveLast(1);

    //Sample:
    //http://finance.yahoo.com/d/quotes.csv?s=SBER03.ME+GZPR.ME&f=sl1n&e=.csv
    site = wxString() << ("http://") << config->Read("HTTP_YAHOO_SERVER","download.finance.yahoo.com")
        << ("/d/quotes.csv?s=") << site;
    site << ("&f=") << ("sl1n") << ("&e=.csv");

    //yahoo_->StocksRefreshStatus_ = mmYahoo::DS_INPROGRESS;
    wxString quotes;

    int err_code = site_content(site, quotes);
    if (err_code != wxID_OK)
    {
        //yahoo_->StocksRefreshStatus_ = mmYahoo::DS_FAILED;
        wxBitmap pic(led_red_xpm);
        m_LED->SetBitmap(pic);
        if (err_code != wxID_OK) {
            if (err_code == 2)
                stock_details_->SetLabel(_("Cannot get data from WWW!"));
            else if (err_code == 1)
                stock_details_->SetLabel(_("Unable to connect!"));
            stock_details_short_->SetLabel((""));
        }
        return;
    }

    wxString StockSymbol, dName;
    double dPrice = 0;

    std::map<wxString, std::pair<double, wxString> > stock_data;

    // Break it up into lines
    wxStringTokenizer tkz(quotes, ("\r\n"));

    while (tkz.HasMoreTokens())
    {
        wxString csvline = tkz.GetNextToken();

        /*** Grab the relevant bits (for now only the symbol and the current price) */
        wxStringTokenizer csvsimple(csvline,("\","),wxTOKEN_STRTOK);
        if (csvsimple.HasMoreTokens())
        {
            StockSymbol = csvsimple.GetNextToken();
            if (suffix_available)
                StockSymbol.Replace(suffix, (""));
            if (csvsimple.HasMoreTokens())
            {
                csvsimple.GetNextToken().ToDouble(&dPrice);
                if (csvsimple.HasMoreTokens())
                    dName = csvsimple.GetNextToken();
            }
        }

        //**** HACK HACK HACK
        // Note:
        // 1. If the share is a UK share (e.g. HSBA.L), its downloaded value in pence
        // 2. If the share is not a UK share (e.g. 0005.HK) while we are using UK Yahoo finance, we do not need
        //    to modify the price

        //// UK finance apparently downloads values in pence
        //if (!yahoo_->Server_.CmpNoCase(("uk.finance.yahoo.com")))
        //    dPrice = dPrice / 100;
        //// ------------------
        if((!suffix_available && StockSymbol.Contains((".L"))) ||
             (suffix_available && suffix == (".L")))
            dPrice = dPrice / 100;
        stock_data.insert(std::make_pair(StockSymbol, std::make_pair(dPrice, dName)));
    }

    STOCK_V1.begin(db_);

    BOOST_FOREACH(DB_View_STOCK_V1::Data &stock, all_stocks)
    {
        wxString stock_symbol = stock.SYMBOL.Upper();
        std::pair<double, wxString> data = stock_data[stock_symbol];

        double price = data.first;
        if (price > 0.000001 || stock.NUMSHARES > 0.0)
            stock.CURRENTPRICE = price;

        // update stock price and value
        if (stock.STOCKNAME.IsEmpty())
            stock.STOCKNAME = data.second;

        stock.VALUE = stock.NUMSHARES * price;

        // store into db
        stock.save(db_);
    }

    STOCK_V1.commit(db_);

    // Now refresh the display
    initVirtualListControl();
    listCtrlStock_->RefreshItems(0, ((int)trans_.size()) - 1);

    // We are done!
    //yahoo_->LastRefreshDT_       = wxDateTime::Now();

    config->Write("STOCKS_LAST_REFRESH_DATETIME", 
        wxString::Format(("%s %s"),
        wxDateTime::Now().FormatISODate().c_str(),
        wxDateTime::Now().FormatISOTime().c_str()));
    //yahoo_->StocksRefreshStatus_ = mmYahoo::DS_SUCCESSFUL;
    wxBitmap pic(led_green_xpm);
    m_LED->SetBitmap(pic);
}

void stocksListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(wxID_NEW);
    menu.AppendSeparator();
    menu.Append(wxID_EDIT);
    menu.Append(wxID_DELETE);
    PopupMenu(&menu, event.GetPoint());
}

wxString mmStocksPanel::getItem(long item, long column)
{
    if (column == COL_DATE)         return trans_[item].stockPDate_;
    if (column == COL_NAME)         return trans_[item].shareName_;
    if (column == COL_NUMBER)       return trans_[item].numSharesStr_;
    if (column == COL_GAIN_LOSS)    return trans_[item].gainLossStr_;
    if (column == COL_VALUE)        return trans_[item].valueStr_;
    if (column == COL_CURRENT)      return trans_[item].cPriceStr_;
    if (column == COL_NOTES)        return trans_[item].shareNotes_;

    return ("");
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

void mmStocksPanel::updateExtraStocksData(int selectedIndex_)
{
    if (selectedIndex_ == -1)
    {
        stock_details_->SetLabel(Tips(TIPS_STOCK));
        stock_details_short_->SetLabel(strLastUpdate_);
    }
    else
    {
        wxString stockPurchasePriceStr, stockCurrentPriceStr, stockDifferenceStr;
        wxString stocktotalDifferenceStr, stockPercentageStr, stocktotalPercentageStr;
        wxString stockPercentagePerYearStr, stockavgPurchasePriceStr;
        wxString stocknumSharesStr = trans_[selectedIndex_].numSharesStr_;
        wxString stocktotalnumSharesStr = trans_[selectedIndex_].totalnumSharesStr_;
        wxString stockgainlossStr = trans_[selectedIndex_].gainLossStr_;
        wxString stocktotalgainlossStr;

        double stockPurchasePrice = trans_[selectedIndex_].purchasePrice_;
        double stockCurrentPrice = trans_[selectedIndex_].currentPrice_;
        double stockDifference = stockCurrentPrice - stockPurchasePrice;

        double stockavgPurchasePrice = trans_[selectedIndex_].avgpurchasePrice_;
        double stocktotalDifference = stockCurrentPrice - stockavgPurchasePrice;
        double stockDaysOwn = trans_[selectedIndex_].stockDays_;
        // Commision don't calculates here
        double stockPercentage = (stockCurrentPrice/stockPurchasePrice-1.0)*100.0;
        double stockPercentagePerYear = stockPercentage * 365.0 / stockDaysOwn;
        double stocktotalPercentage = (stockCurrentPrice/stockavgPurchasePrice-1.0)*100.0;
        // double stocknumShares = trans_[selectedIndex_].numShares_;
        double stocktotalnumShares = trans_[selectedIndex_].totalnumShares_;
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

        wxString miniInfo = ("");
        if (trans_[selectedIndex_].stockSymbol_ != (""))
        miniInfo << ("\t") << _("Symbol: ") << trans_[selectedIndex_].stockSymbol_ << wxT ("\t\t");
        miniInfo << _ ("Total:") << wxT (" (") << trans_[selectedIndex_].totalnumSharesStr_ << wxT (") ");
        stock_details_short_->SetLabel(miniInfo);

        wxString additionInfo =("");
        //Selected share info
        additionInfo
        << ("|") << stockCurrentPriceStr << (" - ") << stockPurchasePriceStr << ("|") << (" = ") << stockDifferenceStr
        << wxT (" * ") << stocknumSharesStr << wxT (" = ") << stockgainlossStr << wxT (" ( ") << stockPercentageStr << wxT ('%')
        << wxT (" )") << wxT ("\n");
        //Summary for account for selected symbol
        if (trans_[selectedIndex_].purchasedTime_ > 1)
        {
            additionInfo << ("|") << stockCurrentPriceStr << (" - ") << stockavgPurchasePriceStr << ("|") << (" = ") << stocktotalDifferenceStr
            << wxT (" * ") << stocktotalnumSharesStr << wxT (" = ") << stocktotalgainlossStr << wxT (" ( ") << stocktotalPercentageStr << wxT ('%')
            << wxT (" )") << wxT ("\n");
        }
        additionInfo << getItem(selectedIndex_, COL_NOTES);

        stock_details_->SetLabel(additionInfo);
    }
}

void mmStocksPanel::enableEditDeleteButtons(bool en)
{
    wxButton* bE = (wxButton*)FindWindow(wxID_EDIT);
    wxButton* bD = (wxButton*)FindWindow(wxID_DELETE);
    bE->Enable(en);
    bD->Enable(en);
}

int stocksListCtrl::OnGetItemImage(long item) const
{
    if (cp_->trans_[item].gainLoss_ > 0) return ICON_DESC;
    return ICON_ASC;
}

void stocksListCtrl::OnListKeyDown(wxListEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_DELETE:
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,  wxID_DELETE);
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
    mmStockDialog dlg(cp_->db_, cp_->core_, 0, false, cp_->accountID_, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
}

void stocksListCtrl::OnDeleteStocks(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;

    if (cp_->trans_.size() == 0) selectedIndex_ = -1;

    wxMessageDialog msgDlg(this, _("Do you really want to delete the stock investment?"),
                           _("Confirm Stock Investment Deletion"),wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        STOCK_V1.remove(cp_->trans_[selectedIndex_].id_, cp_->db_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl();
        if (cp_->trans_.size() == 0) selectedIndex_ = -1;
    }
}

void stocksListCtrl::OnEditStocks(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;

    mmStockDialog dlg(cp_->db_, cp_->core_, cp_->trans_[selectedIndex_].id_, true, -1, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
}

void stocksListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    mmStockDialog dlg(cp_->db_, cp_->core_, cp_->trans_[selectedIndex_].id_, true, -1, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
        cp_->updateExtraStocksData(selectedIndex_);
    }
}

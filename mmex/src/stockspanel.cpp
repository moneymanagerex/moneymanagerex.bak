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
#include "mmyahoo.h"
#include "yahoosettingsdialog.h"

#include <algorithm>
#include <vector>

/* Include XPM Support */
#include "../resources/uparrow.xpm"
#include "../resources/downarrow_red.xpm"

/*******************************************************/
namespace
{
enum
{
    ID_PANEL_STOCKS_STATIC_DETAILS = 0,
    ID_PANEL_STOCKS_STATIC_DETAILS_MINI
};
enum EColumn
{ 
    //COL_HELDAT,
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
    EVT_BUTTON(wxID_REFRESH,     mmStocksPanel::OnRefreshQuotes)
    EVT_BUTTON(wxID_SETUP,     mmStocksPanel::OnHTTPSettings)
    EVT_TIMER(ID_TIMER_REFRESH_STOCK,       mmStocksPanel::OnRefreshTimer)
    EVT_TIMER(ID_TIMER_SCHEDULE_STOCK,      mmStocksPanel::OnScheduleTimer)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(stocksListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_STOCKS_LISTCTRL,   stocksListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_STOCKS_LISTCTRL, stocksListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_STOCKS_LISTCTRL,    stocksListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(ID_PANEL_STOCKS_LISTCTRL,    stocksListCtrl::OnListItemDeselected)

    EVT_MENU(MENU_TREEPOPUP_NEW,              stocksListCtrl::OnNewStocks)
    EVT_MENU(MENU_TREEPOPUP_EDIT,             stocksListCtrl::OnEditStocks)
    EVT_MENU(MENU_TREEPOPUP_DELETE,           stocksListCtrl::OnDeleteStocks)

    EVT_LIST_KEY_DOWN(ID_PANEL_STOCKS_LISTCTRL,   stocksListCtrl::OnListKeyDown)
END_EVENT_TABLE()
/*******************************************************/
mmStocksPanel::mmStocksPanel(wxSQLite3Database* db, wxSQLite3Database* inidb,
                             int accountID,
                             wxWindow *parent,
                             wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
        : db_(db), inidb_(inidb), m_imageList(0), accountID_(accountID)
{

    Create(parent, winid, pos, size, style, name);
}

bool mmStocksPanel::Create(wxWindow *parent,
                            wxWindowID winid, const wxPoint& pos,
                            const wxSize& size,long style, const wxString& name)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    this->Freeze();
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    initVirtualListControl();
    if (trans_.size() > 1)
        listCtrlAccount_->EnsureVisible(((int)trans_.size()) - 1);

    // Greg Newton
    yahoo_ = new mmYahoo(inidb_, db_);

    DownloadScheduleTimer_=NULL;
    StatusRefreshTimer_=NULL;

    // Speed the LED flash rate up. Crude, eh?
    // m_LED->SetTimerInterval(m_LED->GetTimerInterval()/2);

    StatusRefreshTimer_ = new wxTimer(this,ID_TIMER_REFRESH_STOCK);
    StatusRefreshTimer_->Start(250, wxTIMER_CONTINUOUS);

    DownloadScheduleTimer_ = new wxTimer(this, ID_TIMER_SCHEDULE_STOCK);
    DownloadScheduleTimer_->Start(yahoo_->UpdateIntervalMinutes_ * 60000, wxTIMER_CONTINUOUS);

    this->Thaw();
    return TRUE;
}

mmStocksPanel::~mmStocksPanel()
{
    if (m_imageList) delete m_imageList;

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

    if (yahoo_) delete yahoo_;

    if (m_LED)
        delete m_LED;

    long col0, col1, col2, col3, col4, col5, col6;
    col0 = listCtrlAccount_->GetColumnWidth(COL_DATE);
    col1 = listCtrlAccount_->GetColumnWidth(COL_NAME);
    col2 = listCtrlAccount_->GetColumnWidth(COL_NUMBER);
    col3 = listCtrlAccount_->GetColumnWidth(COL_VALUE);
    col4 = listCtrlAccount_->GetColumnWidth(COL_GAIN_LOSS);
    col5 = listCtrlAccount_->GetColumnWidth(COL_CURRENT);
    col6 = listCtrlAccount_->GetColumnWidth(COL_NOTES);

    wxString col0Str = wxString::Format(wxT("%d"), col0);
    wxString col1Str = wxString::Format(wxT("%d"), col1);
    wxString col2Str = wxString::Format(wxT("%d"), col2);
    wxString col3Str = wxString::Format(wxT("%d"), col3);
    wxString col4Str = wxString::Format(wxT("%d"), col4);
    wxString col5Str = wxString::Format(wxT("%d"), col5);
    wxString col6Str = wxString::Format(wxT("%d"), col6);

    mmDBWrapper::begin(inidb_);
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL0_WIDTH"), col0Str);
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL1_WIDTH"), col1Str);
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL2_WIDTH"), col2Str);
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL3_WIDTH"), col3Str);
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL4_WIDTH"), col4Str);
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL5_WIDTH"), col5Str);
    mmDBWrapper::setINISettingValue(inidb_, wxT("STOCKS_COL6_WIDTH"), col6Str);
    mmDBWrapper::commit(inidb_);
}

void mmStocksPanel::CreateControls()
{
    mmStocksPanel* itemPanel8 = this;

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemPanel8->SetSizer(itemBoxSizer9);
    itemPanel8->SetBackgroundColour(mmColors::listBackColor);

    /* ---------------------- */
    wxPanel* headerPanel = new wxPanel(itemPanel8, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    itemBoxSizer9->Add(headerPanel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);
    headerPanel->SetBackgroundColour(mmColors::listBackColor);

    wxStaticText* itemStaticText9 = new wxStaticText(headerPanel, ID_PANEL_BD_STATIC_HEADER,
                                    _("Stock Investments"), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));

    wxStaticText* itemStaticText10 = new wxStaticText(headerPanel,
                                     ID_PANEL_CHECKING_STATIC_BALHEADER,
                                     _("Total:"), wxDefaultPosition, wxDefaultSize, 0);

    m_LED = new awxLed(headerPanel, ID_PANEL_STOCK_UPDATE_LED, wxDefaultPosition, wxDefaultSize, awxLED_GREEN, 1, 5);
    //m_LED = new awxLed(headerPanel, ID_PANEL_STOCK_UPDATE_LED, wxDefaultPosition, wxDefaultSize, awxLED_GREEN);
    m_LED->SetState(awxLED_OFF);
    m_LED->SetBackgroundColour(mmColors::listBackColor);
    m_LED->SetToolTip(_("Idle"));

    wxBoxSizer* itemBoxSizerHHeader = new wxBoxSizer(wxHORIZONTAL);

    itemBoxSizerHHeader->Add(m_LED, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
    itemBoxSizerHHeader->Add(itemStaticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    itemBoxSizerVHeader->Add(itemBoxSizerHHeader, 1, wxEXPAND, 1);
    itemBoxSizerVHeader->Add(itemStaticText10, 0, wxALL, 1);

    /* ---------------------- */

    wxSplitterWindow* itemSplitterWindow10 = new wxSplitterWindow(itemPanel8,
            ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100),
            wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER);

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList(imageSize.GetWidth(),
                                   imageSize.GetHeight());
    m_imageList->Add(wxBitmap(uparrow_xpm));
    m_imageList->Add(wxBitmap(downarrow_xpm));

    listCtrlAccount_ = new stocksListCtrl(this, itemSplitterWindow10,
                                           ID_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize,
                                           wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL);
    listCtrlAccount_->SetBackgroundColour(mmColors::listDetailsPanelColor);
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlAccount_->InsertColumn(COL_DATE, _("Purchase Date"));
    wxListItem itemCol;
    itemCol.SetImage(-1);
    //itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    itemCol.SetText(_("Share Name"));
    listCtrlAccount_->InsertColumn(COL_NAME, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Number of Shares"));

    listCtrlAccount_->InsertColumn(COL_NUMBER, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Gain/Loss"));

    listCtrlAccount_->InsertColumn(COL_GAIN_LOSS, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Value"));
    listCtrlAccount_->InsertColumn(COL_VALUE, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Current"));
    listCtrlAccount_->InsertColumn(COL_CURRENT, itemCol);

    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    itemCol.SetText(_("Notes"));
    listCtrlAccount_->InsertColumn(COL_NOTES, itemCol);

    /* See if we can get data from inidb */
    long col0, col1, col2, col3, col4, col5, col6;
    mmDBWrapper::getINISettingValue(inidb_,
                                    wxT("STOCKS_COL0_WIDTH"), wxT("150")).ToLong(&col0);
    mmDBWrapper::getINISettingValue(inidb_,
                                    wxT("STOCKS_COL1_WIDTH"), wxT("-2")).ToLong(&col1);
    mmDBWrapper::getINISettingValue(inidb_,
                                    wxT("STOCKS_COL2_WIDTH"), wxT("-2")).ToLong(&col2);
    mmDBWrapper::getINISettingValue(inidb_,
                                    wxT("STOCKS_COL3_WIDTH"), wxT("-2")).ToLong(&col3);
    mmDBWrapper::getINISettingValue(inidb_,
                                    wxT("STOCKS_COL4_WIDTH"), wxT("-2")).ToLong(&col4);
    mmDBWrapper::getINISettingValue(inidb_,
                                    wxT("STOCKS_COL5_WIDTH"), wxT("-2")).ToLong(&col5);
    mmDBWrapper::getINISettingValue(inidb_,
                                    wxT("STOCKS_COL6_WIDTH"), wxT("-2")).ToLong(&col6);

    listCtrlAccount_->SetColumnWidth(COL_DATE, col0);
    listCtrlAccount_->SetColumnWidth(COL_NAME, col1);
    listCtrlAccount_->SetColumnWidth(COL_NUMBER, col2);
    listCtrlAccount_->SetColumnWidth(COL_VALUE, col3);
    listCtrlAccount_->SetColumnWidth(COL_GAIN_LOSS, col4);
    listCtrlAccount_->SetColumnWidth(COL_CURRENT, col5);
    listCtrlAccount_->SetColumnWidth(COL_NOTES, col6);

    wxPanel* itemPanel12 = new wxPanel(itemSplitterWindow10, ID_PANEL1,
                                        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    itemSplitterWindow10->SplitHorizontally(listCtrlAccount_, itemPanel12);
    itemSplitterWindow10->SetMinimumPaneSize(100);
    itemSplitterWindow10->SetSashGravity(1.0);
    itemBoxSizer9->Add(itemSplitterWindow10, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel12->SetSizer(itemBoxSizer4);
    itemPanel12->SetBackgroundColour(mmColors::listBackColor);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton6 = new wxButton(itemPanel12, wxID_NEW, _("&New"));
    itemButton6->SetToolTip(_("New Stock Investment"));
    itemBoxSizer5->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    wxButton* itemButton81 = new wxButton(itemPanel12, wxID_EDIT, _("&Edit"));
    itemButton81->SetToolTip(_("Edit Stock Investment"));
    itemBoxSizer5->Add(itemButton81, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton81->Enable(false);

    wxButton* itemButton7 = new wxButton(itemPanel12, wxID_DELETE, _("&Delete"));
    itemButton7->SetToolTip(_("Delete Stock Investment"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButton7->Enable(false);
    
    wxButton* itemButton8 = new wxButton(itemPanel12, wxID_REFRESH, _("&Refresh"),
                                          wxDefaultPosition, wxDefaultSize, 0);
    itemButton8->SetToolTip(_("Refresh Stock Prices from Yahoo"));
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    wxButton* itemButton9 = new wxButton(itemPanel12, wxID_SETUP, _("&Settings"),
                                          wxDefaultPosition, wxDefaultSize, 0);
    itemButton9->SetToolTip(_("Change settings for automatic refresh"));
    itemBoxSizer5->Add(itemButton9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    
    //Infobar-mini 
    wxStaticText* itemStaticText44 = new wxStaticText(itemPanel12, ID_PANEL_STOCKS_STATIC_DETAILS_MINI, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer5->Add(itemStaticText44, 1, wxGROW|wxALL, 12);
    //Infobar 
    wxStaticText* itemStaticText33 = new wxStaticText(itemPanel12, 
    ID_PANEL_STOCKS_STATIC_DETAILS, _("Using MMEX it is possible to track stocks/mutual funds investments."), 
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP);
    itemBoxSizer4->Add(itemStaticText33, 1, wxGROW|wxALL, 1);
    
    //updateExtraStocksData(-1);
}

void mmStocksPanel::initVirtualListControl()
{
    /* Clear all the records */
    trans_.clear();

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_BD_STATIC_HEADER);
    wxString str = mmDBWrapper::getAccountName(db_, accountID_);
    header->SetLabel(_("Stock Investments: ") + str);

    mmDBWrapper::loadBaseCurrencySettings(db_);
    double originalVal = 0.0;
//    double value = 0.0;
    
    mmDBWrapper::loadSettings(accountID_, db_);
        
    //Get Init Value of the account
    double initVal = mmDBWrapper::getInitBalanceOnAccount(db_, accountID_);
    // + Transfered from other accounts - Transfered to other accounts
    //double initVal = mmDBWrapper::getTotalBalanceOnAccount(db_, accountID_, true);

    //Get Stock Investment Account Balance as Init Amount + sum (Value) - sum (Purchase Price)
    double total = mmDBWrapper::getStockInvestmentBalance(db_, accountID_, false, originalVal);
    wxString balance;
    mmex::formatDoubleToCurrency(total+initVal, balance);
    
    wxString original;
    mmex::formatDoubleToCurrency(originalVal, original);

    //
    wxString diffStr;
    mmex::formatDoubleToCurrency((total > originalVal ? total - originalVal : originalVal - total), diffStr);
    //Percent
    wxString diffStrPercents;
    double diffPercents = (total > originalVal ? total/originalVal*100.0-100.0 : -(total/originalVal*100.0-100.0));
    mmex::formatDoubleToCurrencyEdit(diffPercents, diffStrPercents);
    
    header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_BALHEADER);
    wxString lbl;
    lbl << _("Total: ") << balance << wxT ("     ") << _("Invested: ") << original;
    if (total > originalVal)
        lbl << wxT ("     ") << _("Gain: "); 
    else
        lbl << wxT ("     ") << _("Loss: ");
    lbl << diffStr << wxT("  ( ") << diffStrPercents << wxT(" %)");

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

        th.stockID_           = q1.GetInt(wxT("STOCKID"));
        th.stockPDate_        = q1.GetString(wxT("PURCHDATE"));
        int accountID         = q1.GetInt(wxT("HELDAT"));
        th.stockSymbol_       = q1.GetString (wxT ("SYMBOL"));
        th.heldAt_            = mmDBWrapper::getAccountName(db_, accountID);
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
        double commission     = q1.GetDouble(wxT("COMMISSION"));
        th.stockDays_         = q1.GetDouble (wxT ("DAYSOWN"));

        th.gainLoss_          = th.value_ - ((th.numShares_ * th.purchasePrice_) + commission);
        mmex::formatDoubleToCurrencyEdit(((th.value_ / ((th.numShares_ * th.purchasePrice_) + commission)-1.0)*100.0 * 365.0 / th.stockDays_), th.stockPercentagePerYearStr_);

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
            th.stockPDate_ = mmGetDateForDisplay(db_, dtdt);
        }

        trans_.push_back(th);
    }

    st.Finalize();
    listCtrlAccount_->SetItemCount(cnt);
}

void mmStocksPanel::OnRefreshQuotes(wxCommandEvent& WXUNUSED(event))
{
    OrderQuoteRefresh();
}

void mmStocksPanel::OnHTTPSettings(wxCommandEvent& WXUNUSED(event))
{
    YahooSettingsDialog dlg(yahoo_, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        // Copy dialog values back to yahoo_->
        yahoo_->UpdatingEnabled_ = dlg.m_checkBoxRefreshPrices->GetValue();
        yahoo_->UpdateIntervalMinutes_ = dlg.m_RefreshInterval->GetValue();
        yahoo_->Suffix_ = dlg.m_YahooSuffix->GetValue();
        yahoo_->Server_ = dlg.m_YahooServer->GetValue();
        yahoo_->OpenTimeStr_ = wxString::Format(wxT("%02d:%02d:00"), dlg.m_MarketOpenHour->GetValue(),dlg.m_MarketOpenMinute->GetValue());
        yahoo_->CloseTimeStr_ = wxString::Format(wxT("%02d:%02d:00"), dlg.m_MarketCloseHour->GetValue(),dlg.m_MarketCloseMinute->GetValue());
    }
}

void mmStocksPanel::OnDeleteStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnDeleteStocks(event);
}

void mmStocksPanel::OnNewStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnNewStocks(event);
}

void mmStocksPanel::OnEditStocks(wxCommandEvent& event)
{
    listCtrlAccount_->OnEditStocks(event);
}

/** Thread and timer events **/
// The timer for refreshing the LED and status bar (every second?)
void mmStocksPanel::OnRefreshTimer(wxTimerEvent& WXUNUSED(event))
{
    wxDateTime      LastRefreshDT;
    wxString        strLastUpdate;

    awxLedColour    LEDcolour;
    awxLedState     LEDstate;
    bool            LEDblink;

    wxString sStatusText;
    if (yahoo_->LastRefreshDT_.IsValid())
        strLastUpdate.Printf(_("%s on %s"),yahoo_->LastRefreshDT_.FormatTime().c_str(),
                             yahoo_->LastRefreshDT_.FormatDate().c_str());
    else
        strLastUpdate = _("never!");

    switch (yahoo_->StocksRefreshStatus_)
    {
    case mmYahoo::DS_NOTAUTOMATIC:
        sStatusText.Printf(_("Automatic stock quote refresh is disabled. \nLast update: %s"),strLastUpdate.c_str());
        LEDcolour = awxLED_LUCID;
        LEDstate = awxLED_OFF;
        LEDblink = false;
        break;
    case mmYahoo::DS_OUTOFHOURS:
        sStatusText.Printf(_("Automatic stock quote refresh idle - Out of Hours. \nLast update: %s"),strLastUpdate.c_str());
        LEDcolour = awxLED_YELLOW;
        LEDstate = awxLED_ON;
        LEDblink = false;
        break;
    case mmYahoo::DS_INPROGRESS:
        sStatusText.Printf(_("Automatic stock quote refresh in progress..."));
        LEDstate = awxLED_ON;
        LEDcolour = awxLED_GREEN;
        LEDblink = true;
        break;
    case mmYahoo::DS_SUCCESSFUL:
        sStatusText.Printf(_("Automatic stock quote refresh successful. \nLast update: %s"),strLastUpdate.c_str());
        LEDstate = awxLED_ON;
        LEDcolour = awxLED_GREEN;
        LEDblink = false;
        break;
    case mmYahoo::DS_FAILED:
        sStatusText.Printf(_("Automatic stock quote refresh Failed. \nLast successful update: %s"),strLastUpdate.c_str());
        LEDstate = awxLED_ON;
        LEDcolour = awxLED_RED;
        LEDblink = false;
        break;
    default:
     case mmYahoo::DS_NOTSTARTED:
        sStatusText.Printf(_("Automatic stock quote refresh not started. \nLast update: %s"),strLastUpdate.c_str());
        LEDcolour = awxLED_LUCID;
        LEDstate = awxLED_OFF;
        LEDblink = false;
        break;
    }

    //SetStatusText(sStatusText,1);

    m_LED->SetColour(LEDcolour);
    m_LED->SetState(LEDstate);
    if (LEDblink)  m_LED->SetState(awxLED_BLINK);
    m_LED->SetToolTip(sStatusText);
    m_LED->Redraw();
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
        else
            yahoo_->StocksRefreshStatus_ = mmYahoo::DS_OUTOFHOURS;
    }
    else
    {
        yahoo_->StocksRefreshStatus_ = mmYahoo::DS_NOTAUTOMATIC;
    }
}

/** Compare current time with market hours, and with last update time **/
bool mmStocksPanel::DownloadIsRequired(void)
{
   // If automatic updates are disabled, don't start a download.
    if (!yahoo_->UpdatingEnabled_) return false;
    if (!yahoo_->LastRefreshDT_.IsValid()) return true; // Invalid last update time, so let's do it!

    wxDateTime workingDateTime = wxDateTime::Now();

    // Get today's market open and close times
    wxDateTime MarketOpen = wxDateTime::Now();
    wxDateTime MarketClose = wxDateTime::Now();
    MarketOpen.ParseTime(yahoo_->OpenTimeStr_);
    MarketClose.ParseTime(yahoo_->CloseTimeStr_);

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
    MarketOpen.ParseTime(yahoo_->OpenTimeStr_);
    MarketClose.ParseTime(yahoo_->CloseTimeStr_);
    if (yahoo_->LastRefreshDT_.IsEarlierThan(MarketClose)) return true;

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

    wxString YSymbols;
    wxSortedArrayString symbolarray;

    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select UPPER (SYMBOL) as SYMBOL from STOCK_V1");
    while (q1.NextRow())
    {
        wxString strSymbol = q1.GetString(wxT("SYMBOL"));
        if (wxNOT_FOUND == symbolarray.Index(strSymbol.GetData()))
            symbolarray.Add(strSymbol);
    }
    q1.Finalize();

    if(symbolarray.GetCount() < 1) 
    {
        return;
    }

    YSymbols = symbolarray.Item(0)+yahoo_->Suffix_;
    for (size_t i = 1; i < symbolarray.GetCount(); i++)
    {
        YSymbols = YSymbols+wxT("+")+symbolarray.Item(i)+yahoo_->Suffix_;
    }
    //http://finance.yahoo.com/d/quotes.csv?s=SBER03.ME&f=sl1n&e=.csv
    wxString site = wxT("http://")+
                   yahoo_->Server_+ 
                   wxT("/d/quotes.csv?s=")+
                   YSymbols+
                   wxT("&f=")+
                   yahoo_->CSVColumns_+
                   wxT("&e=.csv");
    
    wxURL url(site);
    if (url.GetError()!=wxURL_NOERR) 
    { 
        mmShowErrorMessage(this, _("Unable to connect!"), _("Stock Download"));
        return;
    }
    url.GetProtocol().SetTimeout(10); // 10 secs
    unsigned char buf[16084];
    wxInputStream* in_stream = url.GetInputStream();
    if (!in_stream)
    {
        mmShowErrorMessage(this, _("Unable to connect!"), _("Stock Download"));
        return;
    }
    in_stream->Read(buf, 16084);
    size_t bytes_read=in_stream->LastRead();
    delete in_stream;
    buf[bytes_read] = '\0';
    wxString quotes = wxString::FromAscii((const char *)buf);
    
    yahoo_->StocksRefreshStatus_ = mmYahoo::DS_INPROGRESS;

    // Break it up into lines
    wxStringTokenizer tkz(quotes, wxT("\r\n"));

    wxString StockSymbolWithSuffix;
    double dPrice = 0;
    wxString dName;
    while (tkz.HasMoreTokens())
    {
        wxString csvline = tkz.GetNextToken();
    
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
                    yahoo_->StocksRefreshStatus_ = mmYahoo::DS_FAILED;
                
            }
            else
            {
                yahoo_->StocksRefreshStatus_ = mmYahoo::DS_FAILED;
            }
        }
        else
        {
            yahoo_->StocksRefreshStatus_ = mmYahoo::DS_FAILED;
        }

        /****** Update all that match this symbol ******/
        // wxString Suffix_ = mmDBWrapper::getINISettingValue(inidb_,wxT("HTTP_YAHOO_SUFFIX"), wxT(".AX"));
        wxString StockSymbolNoSuffix;
        if (yahoo_->Suffix_.IsEmpty())
        {
            StockSymbolNoSuffix = wxString(StockSymbolWithSuffix.c_str());
        }
        else
        {
            if (!StockSymbolWithSuffix.EndsWith(yahoo_->Suffix_.GetData(), &StockSymbolNoSuffix))
            {
                wxSafeShowMessage(wxT("Big Oops"),wxT("How did I get here?"));
            }
        }

        //**** HACK HACK HACK
        // Note:        
        // 1. If the share is a UK share (e.g. HSBA.L), its downloaded value in pence
        // 2. If the share is not a UK share (e.g. 0005.HK) while we are using UK Yahoo finance, we do not need
        //    to modify the price

        //// UK finance apparently downloads values in pence
        //if (!yahoo_->Server_.CmpNoCase(wxT("uk.finance.yahoo.com")))
        //    dPrice = dPrice / 100;
        //// ------------------        
        if(StockSymbolNoSuffix.Find(wxT(".L")) != wxNOT_FOUND) 
            dPrice = dPrice / 100;

        static const char sql[] = 
        "select STOCKID, "
               "CURRENTPRICE, "
               "NUMSHARES, "
               "STOCKNAME "
        "from STOCK_V1 "
        "where UPPER(SYMBOL) = ?";

        typedef std::vector<mmStockTransactionHolder> vec_t;
        vec_t stockVec;

        wxSQLite3Statement st = db_->PrepareStatement(sql);
        st.Bind(1, StockSymbolNoSuffix.Upper());

        wxSQLite3ResultSet q1 = st.ExecuteQuery();

        while (q1.NextRow())
        {
            mmStockTransactionHolder sh;

            sh.stockID_ = q1.GetInt(wxT("STOCKID"));
            sh.numShares_ = q1.GetDouble(wxT("NUMSHARES"));
            // If the stock's symbol is not found, Yahoo CSV will return 0 for the current price.
            // Therefore, we assume the current price of all existing stock's symbols are greater
            // than zero and we will not update any stock if its curreny price is zero.
            if(dPrice == 0 || sh.numShares_<0.0) dPrice = q1.GetDouble(wxT("CURRENTPRICE"));
            sh.shareName_ = q1.GetString (wxT ("STOCKNAME"));
            if (sh.shareName_.IsEmpty()) sh.shareName_ = dName;
            
            sh.currentPrice_ = dPrice;
            sh.value_ = sh.numShares_ * dPrice;
            stockVec.push_back(sh);
        }
        st.Finalize();

        // --
        static const char sql_upd[] = 
        "update STOCK_V1 "
        "SET CURRENTPRICE = ?, VALUE = ?, STOCKNAME = ? "
        "WHERE STOCKID = ?";

        st = db_->PrepareStatement(sql_upd);

        for (vec_t::const_iterator i = stockVec.begin(); i != stockVec.end(); ++i)
        {
            st.Bind(1, i->currentPrice_);
            st.Bind(2, i->value_);
            st.Bind(3, i->shareName_);
            st.Bind(4, i->stockID_);
            
            st.ExecuteUpdate();
            st.Reset();
        }
        
        st.Finalize();
    }

    // Now refresh the display
    initVirtualListControl();
    listCtrlAccount_->RefreshItems(0, ((int)trans_.size()) - 1);

    // We are done!
    yahoo_->LastRefreshDT_       = wxDateTime::Now();
    yahoo_->StocksRefreshStatus_ = mmYahoo::DS_SUCCESSFUL;
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
    if (column == COL_DATE)         return trans_[item].stockPDate_;
    if (column == COL_NAME)         return trans_[item].shareName_;
    if (column == COL_NUMBER)       return trans_[item].numSharesStr_;
    if (column == COL_GAIN_LOSS)    return trans_[item].gainLossStr_;
    if (column == COL_VALUE)        return trans_[item].valueStr_;
    if (column == COL_CURRENT)      return trans_[item].cPriceStr_;
    if (column == COL_NOTES)        return trans_[item].shareNotes_;
        
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

void mmStocksPanel::updateExtraStocksData(int selectedIndex_)
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_PANEL_STOCKS_STATIC_DETAILS);
    wxStaticText* stm = (wxStaticText*)FindWindow(ID_PANEL_STOCKS_STATIC_DETAILS_MINI);
    if (selectedIndex_ == -1) 
    {
        st->SetLabel(_("Using MMEX it is possible to track stocks/mutual funds investments."));
        stm->SetLabel(wxT(""));
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
        //Commision don't calculates here
        double stockPercentage = (stockCurrentPrice/stockPurchasePrice-1.0)*100.0;
        double stockPercentagePerYear = stockPercentage * 365.0 / stockDaysOwn;
        double stocktotalPercentage = (stockCurrentPrice/stockavgPurchasePrice-1.0)*100.0;
    //  double stocknumShares = trans_[selectedIndex_].numShares_;
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
        //mmex::formatDoubleToCurrencyEdit(stocknumShares, stocknumSharesStr);

        wxString miniInfo = wxT("");
        if (trans_[selectedIndex_].stockSymbol_ != wxT(""))
        miniInfo << wxT("\t") << _("Symbol: ") << trans_[selectedIndex_].stockSymbol_ << wxT ("\t\t"); 
        miniInfo << _ ("Total:") << wxT (" (") << trans_[selectedIndex_].totalnumSharesStr_ << wxT (") ");
        //If some share has been bot for a short period we don't need that info because the forecast may be too optimistic
        //if (stockDaysOwn > 182.5)
        //miniInfo << wxT ("\t\t") << _("Percent/Year: ") << trans_[selectedIndex_].stockPercentagePerYearStr_;
        stm->SetLabel(miniInfo);
        
        wxString additionInfo =wxT("");
        //Selected share info
        additionInfo 
        << wxT("|") << stockCurrentPriceStr << wxT(" - ") << stockPurchasePriceStr << wxT("|") << wxT(" = ") << stockDifferenceStr 
        << wxT (" * ") << stocknumSharesStr << wxT (" = ") << stockgainlossStr << wxT (" ( ") << stockPercentageStr << wxT ('%')
        //<< wxT (" | ")<< stockPercentagePerYearStr << wxT("% ")  << _("Yearly") 
        << wxT (" )") << wxT ("\n");
        //Summary for account for selected symbol
        if (trans_[selectedIndex_].purchasedTime_ > 1)
        {
            additionInfo << wxT("|") << stockCurrentPriceStr << wxT(" - ") << stockavgPurchasePriceStr << wxT("|") << wxT(" = ") << stocktotalDifferenceStr 
            << wxT (" * ") << stocktotalnumSharesStr << wxT (" = ") << stocktotalgainlossStr << wxT (" ( ") << stocktotalPercentageStr << wxT ('%')
            //<< wxT (" | ")<< stockPercentagePerYearStr << wxT("% ") << _("Yearly") 
            << wxT (" )") //<< wxT ("\n")
            << wxT ("\n") << getItem(selectedIndex_, COL_NOTES);
        }

        st->SetLabel(additionInfo);
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
    /* Returns the icon to be shown for each entry */
    if (cp_->trans_[item].gainLoss_ > 0) return 0;
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
    mmStockDialog dlg(cp_->db_, 0, false, this);
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
        mmDBWrapper::deleteStockInvestment(cp_->db_, cp_->trans_[selectedIndex_].stockID_);
        DeleteItem(selectedIndex_);
        cp_->initVirtualListControl();
        if (cp_->trans_.size() == 0) selectedIndex_ = -1;
    }
}

void stocksListCtrl::OnEditStocks(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ == -1) return;

    mmStockDialog dlg(cp_->db_, cp_->trans_[selectedIndex_].stockID_, true, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
    }
}

void stocksListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    mmStockDialog dlg(cp_->db_, cp_->trans_[selectedIndex_].stockID_, true, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        cp_->initVirtualListControl();
        RefreshItems(0, ((int)cp_->trans_.size()) - 1);
        cp_->updateExtraStocksData(selectedIndex_);

    }
}

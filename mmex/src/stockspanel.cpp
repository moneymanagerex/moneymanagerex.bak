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
#include "stockdialog.h"
#include "constants.h"
#include "mmCurrencyFormatter.h"
#include "util.h"

/*******************************************************/
BEGIN_EVENT_TABLE(StocksListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_STOCKS_LISTCTRL,   StocksListCtrl::OnListItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PANEL_STOCKS_LISTCTRL, StocksListCtrl::OnItemRightClick)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_STOCKS_LISTCTRL,    StocksListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(ID_PANEL_STOCKS_LISTCTRL,  StocksListCtrl::OnListItemDeselected)
    EVT_LIST_COL_END_DRAG(ID_PANEL_STOCKS_LISTCTRL,     StocksListCtrl::OnItemResize)
    EVT_LIST_COL_CLICK(ID_PANEL_STOCKS_LISTCTRL,        StocksListCtrl::OnColClick)
    EVT_LIST_KEY_DOWN(ID_PANEL_STOCKS_LISTCTRL,         StocksListCtrl::OnListKeyDown)

    EVT_MENU(MENU_TREEPOPUP_NEW,     StocksListCtrl::OnNewStocks)
    EVT_MENU(MENU_TREEPOPUP_EDIT,    StocksListCtrl::OnEditStocks)
    EVT_MENU(MENU_TREEPOPUP_DELETE,  StocksListCtrl::OnDeleteStocks)

END_EVENT_TABLE()
/*******************************************************/

StocksListCtrl::StocksListCtrl(mmStocksPanel* cp, wxWindow *parent,
const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxListCtrl(parent, id, pos, size, style)
, attr1_(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont)
, attr2_(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont)
, cp_(cp)
, selectedIndex_(-1)
{}

void StocksListCtrl::InitVariables()
{
    m_selected_col = 0;
    m_asc = true;
}

void StocksListCtrl::OnItemResize(wxListEvent& event)
{
    int i = event.GetColumn();
    cp_->save_column_width(i);
}

void StocksListCtrl::OnItemRightClick(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    wxMenu menu;
    menu.Append(MENU_TREEPOPUP_NEW, _("&New Stock Investment"));
    menu.AppendSeparator();
    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Stock Investment"));
    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Stock Investment"));
    PopupMenu(&menu, event.GetPoint());
}

wxString StocksListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

void StocksListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    cp_->updateExtraStocksData(selectedIndex_);
    cp_->enableEditDeleteButtons(true); //Unhide the Edit and Delete buttons if any record selected
}

void StocksListCtrl::OnListItemDeselected(wxListEvent& /*event*/)
{
    selectedIndex_ = -1;
    cp_->enableEditDeleteButtons(false); //Hide the Edit and Delete buttons if no records selected
    cp_->updateExtraStocksData(selectedIndex_);
}

int StocksListCtrl::OnGetItemImage(long item) const
{
    /* Returns the icon to be shown for each entry */
    if (cp_->trans_[item]->gainLoss_ > 0) return 0;
    return 1;
}

wxListItemAttr* StocksListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void StocksListCtrl::OnListKeyDown(wxListEvent& event)
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

void StocksListCtrl::OnNewStocks(wxCommandEvent& /*event*/)
{
    mmStockDialog dlg(cp_->core_, 0, false, cp_->accountID_, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        doRefreshItems(dlg.transID_);
    }
}

void StocksListCtrl::OnDeleteStocks(wxCommandEvent& /*event*/)
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

void StocksListCtrl::OnMoveStocks(wxCommandEvent& /*event*/)
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
        ,cp_->core_->accountList_.GetAccountName(cp_->accountID_));
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

void StocksListCtrl::OnEditStocks(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ < 0) return;

    wxListEvent evt(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, ID_PANEL_STOCKS_LISTCTRL);
    AddPendingEvent(evt);
}

void StocksListCtrl::OnListItemActivated(wxListEvent& /*event*/)
{
    cp_->call_dialog(selectedIndex_);
    cp_->updateExtraStocksData(selectedIndex_);
}

void StocksListCtrl::OnColClick(wxListEvent& event)
{
    if(0 > event.GetColumn() || event.GetColumn() >= mmStocksPanel::COL_MAX) return;

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

void StocksListCtrl::doRefreshItems(int trx_id)
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

/*******************************************************/
BEGIN_EVENT_TABLE(mmStocksPanel, wxPanel)
    EVT_BUTTON(wxID_NEW,         mmStocksPanel::OnNewStocks)
    EVT_BUTTON(wxID_EDIT,        mmStocksPanel::OnEditStocks)
    EVT_BUTTON(wxID_DELETE,      mmStocksPanel::OnDeleteStocks)
    EVT_BUTTON(wxID_MOVE_FRAME,  mmStocksPanel::OnMoveStocks)
    EVT_BUTTON(wxID_REFRESH,     mmStocksPanel::OnRefreshQuotes)
END_EVENT_TABLE()
/*******************************************************/
mmStocksPanel::mmStocksPanel(mmCoreDB* core,
                             int accountID,
                             wxWindow *parent,
                             wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
: mmPanelBase(core)
, m_imageList(0)
, accountID_(accountID)
{
    Create(parent, winid, pos, size, style, name);
}

bool mmStocksPanel::Create(wxWindow *parent,
                            wxWindowID winid, const wxPoint& pos,
                            const wxSize& size,long style, const wxString& name)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    strLastUpdate_ = core_->dbInfoSettings_->GetStringSetting("STOCKS_LAST_REFRESH_DATETIME", "");
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
    core_->iniSettings_->SetIntSetting(wxString::Format("STOCKS_COL%d_WIDTH", i),col_x);
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
    header_text_ = new wxStaticText(headerPanel, wxID_STATIC, "");
    header_text_->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxBOLD, FALSE, ""));

    header_total_ = new wxStaticText(headerPanel, wxID_STATIC, "");

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

    listCtrlAccount_ = new StocksListCtrl(this, itemSplitterWindow10,
                                           ID_PANEL_STOCKS_LISTCTRL, wxDefaultPosition, wxDefaultSize,
                                           wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL);
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    wxListItem itemCol;

    const wxString columns[] = {
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

        int col_x = core_->iniSettings_->GetIntSetting(wxString::Format("STOCKS_COL%d_WIDTH", i), -2);
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
    stock_details_ = new wxStaticText(BottomPanel, wxID_STATIC, "",
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
    header_text_->SetLabel(wxString::Format(_("Stock Investments: %s"), str));

    //mmDBWrapper::loadCurrencySettings(core_->db_.get(), accountID_);
    double originalVal = 0.0;

    std::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencySharedPtr(accountID_);
    wxASSERT(pCurrencyPtr);
    CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

    //Get Init Value of the account
    double initVal = core_->accountList_.GetAccountSharedPtr(accountID_)->initialBalance_;
    // + Transfered from other accounts - Transfered to other accounts

    //Get Stock Investment Account Balance as Init Amount + sum (Value) - sum (Purchase Price)
    double total = mmDBWrapper::getStockInvestmentBalance(core_->db_.get(), accountID_, originalVal);
    wxString balance;
     CurrencyFormatter::formatDoubleToCurrency(total+initVal, balance);

    wxString original;
     CurrencyFormatter::formatDoubleToCurrency(originalVal, original);

    //
    wxString diffStr;
     CurrencyFormatter::formatDoubleToCurrency((total > originalVal ? total - originalVal : originalVal - total), diffStr);

    wxString lbl;
    lbl << _("Total: ") << balance << "     " << _("Invested: ") << original;

    //Percent
    wxString diffStrPercents;
    if (originalVal!= 0.0) {
        if (total > originalVal)
            lbl << "     " << _("Gain: ");
        else
            lbl << "     " << _("Loss: ");
        double diffPercents = (total > originalVal ? total/originalVal*100.0-100.0 : -(total/originalVal*100.0-100.0));
         CurrencyFormatter::formatDoubleToCurrencyEdit(diffPercents, diffStrPercents);
        lbl << diffStr << "  ( " << diffStrPercents << " %)";
    }

    header_total_->SetLabel(lbl);

    const  wxString sql =  wxString::FromUTF8(SELECT_ROW_HELDAT_FROM_STOCK_V1)
        + " order by " + (wxString()<<col+1)
        + (!asc ? " desc" : " ");

    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(sql);
    st.Bind(1, accountID_);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    int cnt = 0, selected_item = -1;

    for ( ; q1.NextRow(); ++cnt)
    {
        mmStockTransactionHolder th;

        th.id_                = q1.GetInt("STOCKID");
        th.stockPDate_        = q1.GetString("PURCHDATE");
        int accountID         = q1.GetInt("HELDAT");
        th.stockSymbol_       = q1.GetString ("SYMBOL");
        th.heldAt_            = core_->accountList_.GetAccountName(accountID);
        th.shareName_         = q1.GetString("STOCKNAME");
        th.shareNotes_        = q1.GetString("NOTES");
        th.numSharesStr_      = "";
        th.numShares_         = q1.GetDouble("NUMSHARES");
        th.totalnumShares_    = q1.GetDouble("TOTAL_NUMSHARES");
        th.purchasedTime_     = q1.GetInt ("PURCHASEDTIME");

        th.currentPrice_      = q1.GetDouble("CURRENTPRICE");
        th.purchasePrice_     = q1.GetDouble("PURCHASEPRICE");
        th.avgpurchasePrice_  = q1.GetDouble ("AVG_PURCHASEPRICE");
        th.value_             = q1.GetDouble("VALUE");
        th.commission_        = q1.GetDouble("COMMISSION");
        th.stockDays_         = q1.GetDouble ("DAYSOWN");

        if (th.id_ == id) selected_item = cnt;
        th.gainLoss_          = th.value_ - ((th.numShares_ * th.purchasePrice_) + th.commission_);
        CurrencyFormatter::formatDoubleToCurrencyEdit(((th.value_ / ((th.numShares_ * th.purchasePrice_)
            + th.commission_)-1.0)*100.0 * 365.0 / th.stockDays_), th.sPercentagePerYear_);

        CurrencyFormatter::formatDoubleToCurrencyEdit(th.gainLoss_, th.gainLossStr_);
        CurrencyFormatter::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);
        CurrencyFormatter::formatDoubleToCurrencyEdit(th.currentPrice_, th.cPriceStr_);
        CurrencyFormatter::formatDoubleToCurrencyEdit(th.avgpurchasePrice_, th.avgPurchasePriceStr_);

        //I wish see integer if it integer else double
        if ((th.numShares_ - static_cast<long>(th.numShares_)) != 0.0)
            th.numSharesStr_=wxString::Format("%.4f",th.numShares_);
        else
            th.numSharesStr_ <<  static_cast<long>(th.numShares_);

        if ((th.totalnumShares_ - static_cast<long>(th.totalnumShares_)) != 0.0)
             CurrencyFormatter::formatDoubleToCurrencyEdit(th.totalnumShares_, th.totalnumSharesStr_);
        else
            th.totalnumSharesStr_ <<  static_cast<long>(th.totalnumShares_);

        //sqlite does not support %y date mask therefore null value should be replaces
        if (th.stockPDate_ == "")
        {
            wxDateTime dtdt = q1.GetDate("PURCHASEDATE");
            th.stockPDate_ = mmGetDateForDisplay(dtdt);
        }

        trans_.push_back(new mmStockTransactionHolder(th));
    }

    st.Finalize();
    listCtrlAccount_->SetItemCount(cnt);
    return selected_item;
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

void mmStocksPanel::OnRefreshQuotes(wxCommandEvent& WXUNUSED(event))
{
    wxString sError = "";
    if (onlineQuoteRefresh(sError))
    {
        const wxString header = _("Stock prices successfully updated");
        stock_details_->SetLabel(header);
        stock_details_short_->SetLabel(wxString::Format(_("Last updated %s"), strLastUpdate_));
        wxMessageDialog msgDlg(this, sError, header, wxOK|wxICON_EXCLAMATION);
        msgDlg.ShowModal();
    }
    else
    {
        refresh_button_->SetBitmapLabel(wxBitmap(wxImage(led_red_xpm).Scale(16,16)));
        stock_details_->SetLabel(sError);
        stock_details_short_->SetLabel(_("Error"));
        mmShowErrorMessage(this, sError, _("Error"));
    }
}

/*** Trigger a quote download ***/
bool mmStocksPanel::onlineQuoteRefresh(wxString& sError)
{
    if(trans_.size() < 1)
    {
        sError = _("Nothing to update");
        return false;
    }

    //Symbol, (Amount, Name)
    std::map<wxString, std::pair<double, wxString> > stocks_data;
    wxString site = "";
    wxSortedArrayString symbols_array;

    for (const auto &stock : trans_)
    {
        const wxString symbol = stock->stockSymbol_.Upper();
        if (!symbol.IsEmpty())
        {
            if (stocks_data.find(symbol) == stocks_data.end())
            {
                stocks_data[symbol] = std::make_pair(stock->currentPrice_, "");
                site << symbol << "+";
            }
        }
    }
    if (site.Right(1).Contains("+")) site.RemoveLast(1);

    //Sample : http://finance.yahoo.com/d/quotes.csv?s=SBER.ME+GAZP.ME&f=sl1n&e=.csv
    site = wxString::Format("http://download.finance.yahoo.com/d/quotes.csv?s=%s&f=sl1n&e=.csv"
        , site);

    refresh_button_->SetBitmapLabel(wxBitmap(wxImage(led_yellow_xpm).Scale(16,16)));
    stock_details_->SetLabel(_("Connecting..."));
    wxString sOutput;

    int err_code = site_content(site, sOutput);
    if (err_code != wxURL_NOERR)
    {
        sError = sOutput;
        return false;
    }

    //--//
    wxString StockSymbolWithSuffix, sName;
    bool updated = false;
    double dPrice = 0.0;

    wxStringTokenizer tkz(sOutput, "\r\n");
    while (tkz.HasMoreTokens())
    {
        const wxString csvline = tkz.GetNextToken();
        StockSymbolWithSuffix = "";
        wxRegEx pattern("\"([^\"]+)\",([^,][0-9.]+),\"([^\"]*)\"");
        if (pattern.Matches(csvline))
        {
            StockSymbolWithSuffix = pattern.GetMatch(csvline, 1);
            pattern.GetMatch(csvline, 2).ToDouble(&dPrice);
            sName = pattern.GetMatch(csvline, 3);
        }
        else
            updated = false;

        updated = !StockSymbolWithSuffix.IsEmpty();

        //**** HACK HACK HACK
        // Note:
        // 1. If the share is a UK share (e.g. HSBA.L), its downloaded value in pence
        // 2. If the share is not a UK share (e.g. 0005.HK) while we are using UK Yahoo finance, we do not need
        //    to modify the price

        //// UK finance apparently downloads values in pence
        //if (!yahoo_->Server_.CmpNoCase("uk.finance.yahoo.com"))
        //    dPrice = dPrice / 100;
        //// ------------------
        if (updated && dPrice > 0)
        {
            if(StockSymbolWithSuffix.substr(StockSymbolWithSuffix.Find(".")) == ".L")
                dPrice = dPrice / 100;
            stocks_data[StockSymbolWithSuffix].first = dPrice;
            stocks_data[StockSymbolWithSuffix].second = sName;
            sError << wxString::Format(_("%s\t -> %s\n")
                , StockSymbolWithSuffix, wxString::Format("%0.4f", dPrice));
        }
    }

    typedef std::vector<mmStockTransactionHolder> vec_t;
    vec_t stockVec;

    for (const auto &stock : trans_)
    {
        mmStockTransactionHolder sh;

        std::pair<double, wxString> &data = stocks_data[stock->stockSymbol_.Upper()];
        dPrice = data.first;

        sh.id_ = stock->id_;
        sh.numShares_ = stock->numShares_;
        // If the stock's symbol is not found, Yahoo CSV will return 0 for the current price.
        // Therefore, we assume the current price of all existing stock's symbols are greater
        // than zero and we will not update any stock if its curreny price is zero.
        if(dPrice == 0 || sh.numShares_ < 0.0) dPrice = stock->currentPrice_;
        sh.shareName_ = stock->shareName_;
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

    strLastUpdate_.Printf(_("%s on %s"), LastRefreshDT_.FormatTime(),
                             LastRefreshDT_.FormatDate());
    core_->dbInfoSettings_->SetStringSetting("STOCKS_LAST_REFRESH_DATETIME", strLastUpdate_);

    return true;
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

    return "";
}

void mmStocksPanel::updateExtraStocksData(int selectedIndex)
{
    if (selectedIndex == -1)
    {
        stock_details_->SetLabel(Tips(TIPS_STOCK));
        stock_details_short_->SetLabel(wxString::Format(_("Last updated %s"), strLastUpdate_));
    }
    else
    {
        wxString sPurchasePrice;
        wxString sCurrentPrice;
        wxString sDifference;
        wxString sTotalDifference;
        wxString sPercentage;
        wxString sTotalPercentage;
        wxString sPercentagePerYear;
        wxString sAvgPurchasePrice;
        wxString sNumShares = trans_[selectedIndex]->numSharesStr_;
        wxString sTotalNumShares = trans_[selectedIndex]->totalnumSharesStr_;
        wxString sGainLoss = trans_[selectedIndex]->gainLossStr_;
        wxString sTotalGainLoss;

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

        CurrencyFormatter::formatDoubleToCurrencyEdit(stockPurchasePrice, sPurchasePrice);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stockavgPurchasePrice, sAvgPurchasePrice);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stockCurrentPrice, sCurrentPrice);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stockDifference , sDifference);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stocktotalDifference , sTotalDifference);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stockPercentage, sPercentage);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stockPercentagePerYear, sPercentagePerYear);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stocktotalPercentage, sTotalPercentage);
        CurrencyFormatter::formatDoubleToCurrencyEdit(stocktotalgainloss, sTotalGainLoss);
        // CurrencyFormatter::formatDoubleToCurrencyEdit(stocknumShares, sNumShares);

        wxString miniInfo = "";
        if (trans_[selectedIndex]->stockSymbol_ != "")
        miniInfo << "\t" << _("Symbol: ") << trans_[selectedIndex]->stockSymbol_ << "\t\t";
        miniInfo << _ ("Total:") << " (" << trans_[selectedIndex]->totalnumSharesStr_ << ") ";
        //If some share has been bot for a short period we don't need that info because the forecast may be too optimistic
        //if (stockDaysOwn > 182.5)
        //miniInfo << "\t\t" << _("Percent/Year: ") << trans_[selectedIndex]->sPercentagePerYear_;
        stock_details_short_->SetLabel(miniInfo);

        wxString additionInfo = "";
        //Selected share info
        additionInfo
        << "|" << sCurrentPrice << " - " << sPurchasePrice << "|" << " = " << sDifference
        << " * " << sNumShares << " = " << sGainLoss << " ( " << sPercentage << "%"
        //<< " | "<< sPercentagePerYear << "% "  << _("Yearly")
        << " )" << "\n";
        //Summary for account for selected symbol
        if (trans_[selectedIndex]->purchasedTime_ > 1)
        {
            additionInfo << "|" << sCurrentPrice << " - " << sAvgPurchasePrice << "|" << " = " << sTotalDifference
            << " * " << sTotalNumShares << " = " << sTotalGainLoss << " ( " << sTotalPercentage << "%"
            //<< " | "<< sPercentagePerYear << "% " << _("Yearly")
            << " )" //<< "\n"
            << "\n" << getItem(selectedIndex, COL_NOTES);
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

void mmStocksPanel::call_dialog(const int selectedIndex)
{
    mmStockDialog dlg(core_, trans_[selectedIndex], true, accountID_, this);
    if (dlg.ShowModal() == wxID_OK)
    {
        listCtrlAccount_->doRefreshItems(dlg.transID_);
    }
}


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

#include "constants.h"
#include "mmhomepagepanel.h"
#include "mmex.h"
#include "util.h"

#include "transdialog.h"
#include "newacctdialog.h"
#include "htmlbuilder.h"
#include "billsdepositspanel.h"
#include "mmgraphincexpensesmonth.h"


BEGIN_EVENT_TABLE( mmHomePagePanel, wxPanel )
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mmHtmlWindow, wxHtmlWindow)
END_EVENT_TABLE()


mmHomePagePanel::mmHomePagePanel(mmGUIFrame* frame,
            mmCoreDB* core,
            const wxString& topCategories,
            wxWindow *parent,
            wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name )
: mmPanelBase(core)
, frame_(frame)
, topCategories_(topCategories)
{
    Create(parent, winid, pos, size, style, name);
    frame_->setHomePageActive();
    frame_->menuPrintingEnable(true);
}

mmHomePagePanel::~mmHomePagePanel()
{
    frame_->setHomePageActive(false);
    frame_->menuPrintingEnable(false);
}

wxString mmHomePagePanel::GetHomePageText()
{
    return html_text_;
}

bool mmHomePagePanel::Create( wxWindow *parent,
            wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name  )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    createFrames();

    return TRUE;
}

void mmHomePagePanel::createFrames()
{
    if (!core_->db_.get())
        return;

    mmHTMLBuilder hb;
    hb.init();
    wxDateTime today = wxDateTime::Now();
    wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
    wxDateTime dtBegin = prevMonthEnd;
    wxDateTime dtEnd = wxDateTime::Now().GetLastMonthDay();

    double tBalance = 0.0;
    double tIncome = 0.0;
    double tExpenses = 0.0;

    hb.startCenter();
    hb.startTable(wxT("100%"), wxT("top"));
    hb.startTableRow();

    hb.startTableCell(wxT("50%\" valign=\"top\" align=\"center"));

    hb.addText(getCalendarWidget());

    hb.addText(displayCheckingAccounts(tBalance, tIncome, tExpenses, dtBegin, dtEnd));

    if ( frame_->hasActiveTermAccounts())
         hb.addText(displayTermAccounts(tBalance,tIncome,tExpenses, dtBegin, dtEnd));

    if (core_->accountList_.has_stock_account())
         hb.addText(displayStocks(tBalance /*,tIncome,tExpenses */));

    hb.addText(displayAssets(tBalance));

    hb.addText(displayGrandTotals(tBalance));

    hb.addParaText(displayCurrencies()); // Will display Currency summary when more than one currency is used.

    hb.addText(displayTopTransactions());
    hb.endTableCell();

    hb.startTableCell(wxT("50%\" valign=\"top\" align=\"center"));
    hb.addText(displayIncomeVsExpenses(tIncome, tExpenses)); //Also displays the Income vs Expenses graph.
    hb.addText(displayBillsAndDeposits());
    hb.addText(getStatWidget());

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();
    hb.end();

    html_text_ = hb.getHTMLText();
    htmlWindow_->SetPage(html_text_);
}

bool sortTransactionsByRemainingDaysHP(const mmBDTransactionHolder& elem1,
                                       const mmBDTransactionHolder& elem2)
{
    return elem1.daysRemaining_ < elem2.daysRemaining_;
}

wxString mmHomePagePanel::displaySummaryHeader(wxString summaryTitle)
{
    mmHTMLBuilder hb;
    hb.startTableRow();
    hb.addTableHeaderCell(summaryTitle, false);
    hb.addTableHeaderCell(_("Reconciled"), true);
    hb.addTableHeaderCell(_("Balance"), true);
    hb.endTableRow();
    return hb.getHTMLText();
}

wxString mmHomePagePanel::displaySectionTotal(wxString totalsTitle, double tRecBalance, double& tBalance)
{
    mmHTMLBuilder hb;
    // format the totals for display
    core_->currencyList_.LoadBaseCurrencySettings();

    wxString tRecBalanceStr;
    mmex::formatDoubleToCurrency(tRecBalance, tRecBalanceStr);
    wxString tBalanceStr;
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    // stocks don't have reconciled balances
    if (totalsTitle == _("Stocks Total:")) tRecBalanceStr = wxEmptyString;

    std::vector<wxString> data;
    data.push_back(tRecBalanceStr);
    data.push_back(tBalanceStr);

    hb.startTableRow();
    hb.addTotalRow(totalsTitle, 3, data);
    hb.endTableRow();
    return hb.getHTMLText();
}

/* Checking Accounts */
wxString mmHomePagePanel::displayCheckingAccounts(double& tBalance, double& tIncome, double& tExpenses, const wxDateTime& dtBegin, const wxDateTime& dtEnd)
{
    mmHTMLBuilder hb;
    hb.startTable(wxT("98%"), wxT("top"), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();

    hb.startTable(wxT("100%"));
    // Only Show the account titles if we want to display Bank accounts.
    if ( frame_->expandedBankAccounts() )
        hb.addText(displaySummaryHeader(_("Bank Account")));

    double tRecBalance = 0.0;

    // Get account balances and display accounts if we want them displayed
    wxString vAccts = core_->iniSettings_->GetStringSetting(wxT("VIEWACCOUNTS"), wxT("ALL"));
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = core_->accountList_.range();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        mmAccount* pCA = it->get();

        if (pCA->acctType_ != ACCOUNT_TYPE_BANK || pCA->status_ == mmAccount::MMEX_Closed) continue;

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pCA->id_).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

        double bal = pCA->initialBalance_ + core_->bTransactionList_.getBalance(pCA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
        double reconciledBal = pCA->initialBalance_ + core_->bTransactionList_.getReconciledBalance(pCA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
        double rate = pCurrencyPtr->baseConv_;
        tBalance += bal * rate; // actual amount in that account in the original rate
        tRecBalance += reconciledBal * rate;

        double income = 0.0;
        double expenses = 0.0;
        // Display the individual Checking account links if we want to display them
        if ( frame_->expandedBankAccounts() 
            || (!frame_->expandedBankAccounts() && !frame_->expandedTermAccounts()) )
        {
            core_->bTransactionList_.getExpensesIncome(core_, pCA->id_, expenses, income, false, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);

            // show the actual amount in that account
            wxString balanceStr;
            wxString reconciledBalanceStr;
            mmex::formatDoubleToCurrency(bal, balanceStr);
            mmex::formatDoubleToCurrency(reconciledBal, reconciledBalanceStr);

            if (((vAccts == wxT("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pCA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
                && frame_->expandedBankAccounts())
            {
                hb.startTableRow();
                hb.addTableCellLink(wxString::Format(wxT("ACCT:%d"), pCA->id_), pCA->name_, false, true);
                hb.addTableCell(reconciledBalanceStr, true);
                hb.addTableCell(balanceStr, true);
                hb.endTableRow();
            }
            // if bank accounts being displayed or no accounts displayed, include income/expense totals on home page.
            tIncome += income;
            tExpenses += expenses;
        }
    }
    hb.addText(displaySectionTotal(_("Bank Accounts Total:"), tRecBalance, tBalance));
    hb.endTable();

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();

    return hb.getHTMLText();
}

/* Term Accounts */
wxString mmHomePagePanel::displayTermAccounts(double& tBalance, double& tIncome, double& tExpenses, const wxDateTime& dtBegin, const wxDateTime& dtEnd)
{
    mmHTMLBuilder hb;
    double tTermBalance = 0.0;
    double tRecBalance  = 0.0;

    hb.startTable(wxT("98%"), wxT("top"), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();

    hb.startTable(wxT("100%"));
    // Only Show the account titles if Term accounts are active and we want them displayed.
    if ( frame_->expandedTermAccounts() )
        hb.addText(displaySummaryHeader(_("Term Account")));

    // Get account balances and add to totals, and display accounts if we want them displayed
    wxString vAccts = core_->iniSettings_->GetStringSetting(wxT("VIEWACCOUNTS"), wxT("ALL"));
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = core_->accountList_.range();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* pTA= it->get();
        if (pTA && pTA->status_== mmAccount::MMEX_Open && pTA->acctType_ == ACCOUNT_TYPE_TERM)
        {
            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pTA->id_).lock();
            wxASSERT(pCurrencyPtr);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

            double bal = pTA->initialBalance_ + core_->bTransactionList_.getBalance(pTA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
            double reconciledBal = pTA->initialBalance_ + core_->bTransactionList_.getReconciledBalance(pTA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
            double rate = pCurrencyPtr->baseConv_;
            tTermBalance += bal * rate; // actual amount in that account in the original rate
            tRecBalance  += reconciledBal * rate;

            // Display the individual Term account links if we want to display them
            if ( frame_->expandedTermAccounts() )
            {
                double income = 0;
                double expenses = 0;
                core_->bTransactionList_.getExpensesIncome(core_, pTA->id_, expenses, income, false, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);

                // show the actual amount in that account
                wxString balanceStr;
                wxString reconciledBalStr;
                mmex::formatDoubleToCurrency(bal, balanceStr);
                mmex::formatDoubleToCurrency(reconciledBal, reconciledBalStr);

                if ((vAccts == wxT("Open") && pTA->status_ == mmAccount::MMEX_Open) ||
                    (vAccts == wxT("Favorites") && pTA->favoriteAcct_) ||
                    (vAccts == wxT("ALL")))
                {
                    hb.startTableRow();
                    hb.addTableCellLink(wxString::Format(wxT("ACCT:%d"), pTA->id_), pTA->name_, false, true);
                    hb.addTableCell(reconciledBalStr, true);
                    hb.addTableCell(balanceStr, true);
                    hb.endTableRow();
                }

                // if Term accounts being displayed, include income/expense totals on home page.
                tIncome += income;
                tExpenses += expenses;
            }
        }
    }

    hb.addText(displaySectionTotal(_("Term Accounts Total:"), tRecBalance, tTermBalance));

    // Add Term balance to Grand Total balance
    tBalance += tTermBalance;
    hb.endTable();

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();

    return hb.getHTMLText();
}

//* Stocks *//
wxString mmHomePagePanel::displayStocks(double& tBalance /*, double& tIncome, double& tExpenses */)
{
    mmHTMLBuilder hb;
    double stTotalBalance = 0.0;
    wxString tBalanceStr;
    wxString tGainStr;

    hb.startTable(wxT("98%"), wxT(""), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();

    hb.startTable(wxT("100%"));
    if (frame_->expandedStockAccounts())
    {
        hb.startTableRow();
        hb.addTableHeaderCell(_("Stocks"), false);
        hb.addTableHeaderCell(_("Gain/Loss"), true);
        hb.addTableHeaderCell(_("Total"), true);
        hb.endTableRow();
    }

    static const char sql[] =
    "select "
    "c.BASECONVRATE, "
    "st.heldat as ACCOUNTID, a.accountname as ACCOUNTNAME, "
    "a.initialbal + "
    "total((st.CURRENTPRICE)*st.NUMSHARES) as BALANCE, "
    "total ((st.CURRENTPRICE-st.PURCHASEPRICE)*st.NUMSHARES-st.COMMISSION) as GAIN "
    "from  stock_v1 st "
    "left join accountlist_v1 a on a.accountid=st.heldat "
    "left join currencyformats_v1 c on c.currencyid=a.currencyid "
    "    where st.purchasedate<=date ('now','localtime') "
    "and a.status='Open' "
    "group by st.heldat ";

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(sql);
    while(q1.NextRow())
    {
        int stockaccountId = q1.GetInt(wxT("ACCOUNTID"));
        double stockBalance = q1.GetDouble(wxT("BALANCE"));
        wxString stocknameStr = q1.GetString(wxT("ACCOUNTNAME"));
        //double income = q1.GetDouble(wxT("INCOME"));
        //double expenses = q1.GetDouble(wxT("EXPENCES"));
        double baseconvrate = q1.GetDouble(wxT("BASECONVRATE"));
        double stockGain = q1.GetDouble(wxT("GAIN"));

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(stockaccountId).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

        mmex::formatDoubleToCurrency(stockBalance, tBalanceStr);
        mmex::formatDoubleToCurrency(stockGain, tGainStr);

        // if Stock accounts being displayed, include income/expense totals on home page.
        //tIncome += income * baseconvrate;
        //tExpenses += expenses * baseconvrate;
        stTotalBalance += stockBalance * baseconvrate;
        //We can hide or show Stocks on Home Page
        if (frame_->expandedStockAccounts())
        {
            hb.startTableRow();
            //////
            //hb.addTableCell(stocknameStr, false,true);
            hb.addTableCellLink(wxString::Format(wxT("STOCK:%d"), stockaccountId), stocknameStr, false, true);
            hb.addTableCell(tGainStr, true);
            hb.addTableCell(tBalanceStr, true);
            hb.endTableRow();
        }
    }
    q1.Finalize();

    hb.addText(displaySectionTotal(_("Stocks Total:"), 0.0, stTotalBalance));
    hb.endTable();

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();

    // Add Stock balance to Grand Total balance
    tBalance += stTotalBalance;
    return hb.getHTMLText();
}

//* Assets *//
wxString mmHomePagePanel::displayAssets(double& tBalance)
{
    mmHTMLBuilder hb;
    double assetBalance = mmDBWrapper::getAssetBalance(core_->db_.get());
    wxString assetBalanceStr;
    core_->currencyList_.LoadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(assetBalance, assetBalanceStr);

    if (mmIniOptions::instance().enableAssets_)
    {
        hb.startTable(wxT("98%"), wxT(""), wxT("1"));
        hb.startTableRow();
        hb.startTableCell();

        hb.startTable(wxT("100%"));
        hb.startTableRow();
        hb.addTableCellLink(wxT("Assets"), _("Assets"), false, true);
        hb.addTableCell(wxT (""), true);
        hb.addTableCell(assetBalanceStr, true, true, true);
        hb.endTableRow();
        hb.endTable();

        hb.endTableCell();
        hb.endTableRow();
        hb.endTable();
    }
    tBalance += assetBalance;
    return hb.getHTMLText();
}

//* Currencies *//
wxString mmHomePagePanel::displayCurrencies()
{
    mmHTMLBuilder hb;

    static const char sql2[] =
        "select ACCOUNTID, CURRENCYNAME, BALANCE, BASECONVRATE from ( "
        "select t.accountid as ACCOUNTID, c.currencyname as CURRENCYNAME, "
        "total (t.BALANCE) as BALANCE, "
        "c.BASECONVRATE as BASECONVRATE "
        "from ( "
        "select  acc.accountid as ACCOUNTID, acc.INITIALBAL as BALANCE "
        "from ACCOUNTLIST_V1 ACC "
        "where ACC.STATUS='Open' "
        "group by acc.accountid  "
        "union all "
        "select  "
        "st.heldat as ACCOUNTID, "
        "total((st.CURRENTPRICE)*st.NUMSHARES-st.COMMISSION) as BALANCE "
        "from  stock_v1 st "
        "where st.purchasedate<=date ('now','localtime') "
        "group by st.heldat "
        "union all "
        "select ca.toaccountid,  total(ca.totransamount) "
        "from checkingaccount_v1 ca "
        "where ca.transcode ='Transfer' and ca.STATUS<>'V' and ca.transdate<=date ('now','localtime') "
        "group by ca.toaccountid "
        "union all "
        "select ca.accountid,  total(case ca.transcode when 'Deposit' then ca.transamount else -ca.transamount end)  "
        "from checkingaccount_v1 ca "
        "where ca.STATUS<>'V' and ca.transdate<=date ('now','localtime') "
        "group by ca.accountid) t "
        "left join accountlist_v1 a on a.accountid=t.accountid "
        "left join  currencyformats_v1 c on c.currencyid=a.currencyid "
        "where a.status='Open' and balance<>0 "
        "group by c.currencyid) order by CURRENCYNAME ";

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(sql2);
    q1 = core_->db_.get()->ExecuteQuery(sql2);

    //Determine how many currencies used
    int curnumber = 0;
    while(q1.NextRow())
        curnumber+=1;

    if (curnumber > 1 )
    {

        hb.startTable(wxT("98%"), wxT(""), wxT("1"));
        hb.startTableRow();
        hb.startTableCell();

        // display the currency header
        hb.startTable(wxT("100%"));
        hb.startTableRow();
        hb.addTableHeaderCell(_("Currency"), false);
        hb.addTableHeaderCell(_("Base Rate"), true);
        hb.addTableHeaderCell(_("Summary"), true);
        hb.endTableRow();

        // display the totals for each currency value
        while(q1.NextRow())
        {
            int accountId = q1.GetInt(wxT("ACCOUNTID"));
            double currBalance = q1.GetDouble(wxT("BALANCE"));
            wxString currencyStr = q1.GetString(wxT("CURRENCYNAME"));
            double convRate = q1.GetDouble(wxT("BASECONVRATE"));
            wxString convRateStr;

            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(accountId).lock();
            wxASSERT(pCurrencyPtr);

            wxString tBalanceStr;
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
            mmex::formatDoubleToCurrency(currBalance, tBalanceStr);
            mmex::formatDoubleToCurrencyEdit(convRate, convRateStr);

            hb.startTableRow();
            hb.addTableCell(currencyStr);
            hb.addTableCell(convRateStr, true);
            hb.addTableCell(tBalanceStr, true);
            hb.endTableRow();
        }
        hb.endTable();
        q1.Finalize();

        hb.endTableCell();
        hb.endTableRow();
        hb.endTable();
    }

    if (!hb.getHTMLText().IsEmpty())
    {
        hb.addLineBreak();
        hb.addLineBreak();
    }
    return hb.getHTMLText();
}

//* Income vs Expenses *//
wxString mmHomePagePanel::displayIncomeVsExpenses(double& tincome, double& texpenses)
{
    mmHTMLBuilder hb;

    wxString incStr, expStr, difStr;
    mmex::formatDoubleToCurrency(tincome, incStr); // must use LoadBaseCurrencySettings (called above)
    mmex::formatDoubleToCurrency(texpenses, expStr);
    mmex::formatDoubleToCurrency(tincome-texpenses, difStr);

    mmGraphIncExpensesMonth gg;
    gg.init(tincome, texpenses);
    gg.Generate(wxT(""));

    hb.startTable(wxT("98%"), wxT("top"), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();

        hb.startTable(wxT("100%"));
        
            wxString monthHeading = _("Current Month");
            if (mmIniOptions::instance().ignoreFutureTransactions_) monthHeading = _("Current Month to Date");
        
            hb.addTableHeaderRow(wxString::Format(_("Income vs Expenses: %s"), monthHeading.c_str()), 2);
        
            hb.startTableRow();
            hb.startTableCell();
            hb.addImage(gg.getOutputFileName());
            hb.endTableCell();
        
            hb.startTableCell();
        
            hb.startTable();
            hb.startTableRow();
            hb.addTableHeaderCell(_("Type"));
            hb.addTableHeaderCell(_("Amount"), true);
            hb.endTableRow();
        
            hb.startTableRow();
            hb.addTableCell(_("Income:"), false, true);
            hb.addTableCell(incStr, true);
            hb.endTableRow();
        
            hb.startTableRow();
            hb.addTableCell(_("Expenses:"), false, true);
            hb.addTableCell(expStr, true);
            hb.endTableRow();
        
            hb.startTableRow();
            hb.addTableCell(_("Difference:"), false, true, true);
            hb.addTableCell(difStr, true, true, true, (tincome-texpenses < 0.0 ? wxT("RED"):wxT("")));
            hb.endTableRow();
        
            hb.endTable();

        hb.endTableCell();
        hb.endTableRow();
        hb.endTable();

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();
    return hb.getHTMLText();
}

//* bills & deposits *//
wxString mmHomePagePanel::displayBillsAndDeposits()
{
    mmHTMLBuilder hb;
    std::vector<mmBDTransactionHolder> trans_;
    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(
        "select BDID, NEXTOCCURRENCEDATE, NUMOCCURRENCES, REPEATS, PAYEEID, TRANSCODE, ACCOUNTID, TOACCOUNTID, TRANSAMOUNT, TOTRANSAMOUNT from BILLSDEPOSITS_V1");

    bool visibleEntries = false;
    while (q1.NextRow())
    {
        mmBDTransactionHolder th;

        th.id_           = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));
        th.nextOccurStr_   = mmGetDateForDisplay(th.nextOccurDate_);
        int numRepeats     = q1.GetInt(wxT("NUMOCCURRENCES"));

        int repeats        = q1.GetInt(wxT("REPEATS"));
        // DeMultiplex the Auto Executable fields.
        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute User Acknowlegement required
            repeats -= BD_REPEATS_MULTIPLEX_BASE;

        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute Silent mode
            repeats -= BD_REPEATS_MULTIPLEX_BASE;

        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = th.nextOccurDate_.Subtract(today);
        th.daysRemaining_ = ts.GetDays();
        int minutesRemaining_ = ts.GetMinutes();

        if (minutesRemaining_ > 0)
            th.daysRemaining_ += 1;

        th.daysRemainingStr_ = wxString::Format(wxT("%d"), th.daysRemaining_) + _(" days remaining");

        if (th.daysRemaining_ == 0)
        {
            if ( ((repeats > 10) && (repeats < 15)) && (numRepeats < 0) )
                th.daysRemainingStr_ = _("Inactive");
        }

        if (th.daysRemaining_ < 0)
        {
            th.daysRemainingStr_ = wxString::Format(wxT("%d"), abs(th.daysRemaining_)) + _(" days overdue!");
            if ( ((repeats > 10) && (repeats < 15)) && (numRepeats < 0) )
                th.daysRemainingStr_ = _("Inactive");
        }

        th.payeeID_        = q1.GetInt(wxT("PAYEEID"));
        th.transType_      = q1.GetString(wxT("TRANSCODE"));
        th.accountID_      = q1.GetInt(wxT("ACCOUNTID"));
        th.toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));

        th.amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
        th.toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));

        mmex::formatDoubleToCurrencyEdit(th.amt_, th.transAmtString_);
        //for Withdrawal amount should be negative
        if (th.transType_== TRANS_TYPE_WITHDRAWAL_STR)
        {
            th.transAmtString_= wxT ("-") + th.transAmtString_;
            th.amt_ = -th.amt_;
        }

        mmex::formatDoubleToCurrencyEdit(th.toAmt_, th.transToAmtString_);

        th.payeeStr_ = core_->payeeList_.GetPayeeName(th.payeeID_);

        if (th.transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            wxString fromAccount = core_->accountList_.GetAccountName(th.accountID_);
            wxString toAccount = core_->accountList_.GetAccountName(th.toAccountID_ );

            th.payeeStr_ = toAccount;
        }

        if (th.daysRemaining_ <= 14)
            visibleEntries = true;

        trans_.push_back(th);
    }
    q1.Finalize();
    std::sort(trans_.begin(), trans_.end(), sortTransactionsByRemainingDaysHP);

    ////////////////////////////////////
    if ( visibleEntries )
    {
        wxString colorStr;

        hb.addLineBreak();
        hb.addLineBreak();

        hb.startTable(wxT("98%"), wxT(""), wxT("1"));
        hb.startTableRow();
        hb.startTableCell();

        hb.startTable(wxT("100%"));
        hb.addTableHeaderRowLink(wxT("billsdeposits"), _("Upcoming Transactions"), 3);

        std::vector<wxString> data4;
        for (size_t bdidx = 0; bdidx < trans_.size(); ++bdidx)
        {
            data4.clear();
            wxDateTime today = wxDateTime::Now();
            wxTimeSpan ts = trans_[bdidx].nextOccurDate_.Subtract(today);
            //int hoursRemaining_ = ts.GetHours();

            if (trans_[bdidx].daysRemaining_ <= 14)
            {
                wxString daysRemainingStr;
                colorStr = wxT("#9999FF");

                daysRemainingStr = trans_[bdidx].daysRemainingStr_;
                if (trans_[bdidx].daysRemaining_ < 0)
                    colorStr = wxT("#FF6600");

                // Load the currency for this BD
                boost::weak_ptr<mmCurrency> wpCurrency = core_->accountList_.getCurrencyWeakPtr(trans_[bdidx].accountID_);
                boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
                wxASSERT(pCurrency);
                if (pCurrency)
                    pCurrency->loadCurrencySettings();

                wxString displayBDAmtString;
                mmex::formatDoubleToCurrency(trans_[bdidx].amt_, displayBDAmtString);

                hb.startTableRow();
                hb.addTableCell(trans_[bdidx].payeeStr_, false, true);
                hb.addTableCell(displayBDAmtString, true);
                //Draw it as numeric that mean align right
                hb.addTableCell(daysRemainingStr, true, false, false, colorStr);
                hb.endTableRow();
            }
        }
        hb.endTable();

        hb.endTableCell();
        hb.endTableRow();
        hb.endTable();
    }
    return hb.getHTMLText();
}

wxString mmHomePagePanel::displayTopTransactions()
{
    mmHTMLBuilder hb;
    mmex::CurrencyFormatter::instance().loadDefaultSettings();
    core_->currencyList_.LoadBaseCurrencySettings();

    static const char sql3[] =
        "select NUMBER, "
        "CATEG|| (Case SUBCATEG when '' then '' else ':'||  SUBCATEG end ) as SUBCATEGORY "
        ", AMOUNT "
        "from ( "
        "select coalesce(CAT.CATEGNAME, SCAT.CATEGNAME) as CATEG, "
        "CANS.ACCOUNTID as ACCOUNTID, "
        "coalesce(SUBCAT.SUBCATEGNAME, SSCAT.SUBCATEGNAME,'') as SUBCATEG, "
        "count(*) as NUMBER, "
        "total((ROUND((case CANS.TRANSCODE when 'Withdrawal' then -1 else 1 end) "
        "* (case CANS.CATEGID when -1 then ST.SPLITTRANSAMOUNT else CANS.TRANSAMOUNT end),2) "
        "* CF.BASECONVRATE "
        ")) as AMOUNT "
        "from  CHECKINGACCOUNT_V1 CANS "
        "left join CATEGORY_V1 CAT on CAT.CATEGID = CANS.CATEGID "
        "left join SUBCATEGORY_V1 SUBCAT on SUBCAT.SUBCATEGID = CANS.SUBCATEGID and SUBCAT.CATEGID = CANS.CATEGID "
        "left join ACCOUNTLIST_V1 ACC on ACC.ACCOUNTID = CANS.ACCOUNTID "
        "left join SPLITTRANSACTIONS_V1 ST on CANS.TRANSID = ST.TRANSID "
        "left join CATEGORY_V1 SCAT on SCAT.CATEGID = ST.CATEGID and CANS.TRANSID=ST.TRANSID "
        "left join SUBCATEGORY_V1 SSCAT on SSCAT.SUBCATEGID = ST.SUBCATEGID and SSCAT.CATEGID = ST.CATEGID and CANS.TRANSID=ST.TRANSID "
        "left join CURRENCYFORMATS_V1 CF on CF.CURRENCYID = ACC.CURRENCYID "
        "where CANS.TRANSCODE <> 'Transfer' "
        "and CANS.TRANSDATE > date('now','localtime', '-1 month') and CANS.TRANSDATE <= date('now','localtime') "
        "and CANS.STATUS <> 'V' "
        "group by CATEG, SUBCATEG "
        "order by ABS (AMOUNT) DESC, CATEG, SUBCATEG "
        ") where AMOUNT < 0 "
        "limit 7 ";

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(sql3);

    std::vector<CategInfo> categList;

    wxString headerMsg = wxString() << _("Top Withdrawals: ") << _("Last 30 Days");

    hb.startTable(wxT("98%"), wxT(""), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();
    hb.startTable(wxT("100%"));
    hb.addTableHeaderRow(headerMsg, 3);
    hb.startTableRow();
    hb.addTableCell(_("Category"), false, false, true);
    hb.addTableCell(_("Quantity"), true, false, true);
    hb.addTableCell(_("Summary"), true, false, true);
    hb.endTableRow();

    while(q1.NextRow())
    {
        double category_total = q1.GetDouble(wxT("AMOUNT"));
        wxString category_total_str = wxEmptyString;
        core_->currencyList_.LoadBaseCurrencySettings();
        mmex::formatDoubleToCurrency(category_total, category_total_str);

        hb.startTableRow();
        hb.addTableCell(q1.GetString(wxT("SUBCATEGORY")), false, true);
        hb.addTableCell(q1.GetString(wxT("NUMBER")), true, true);
        hb.addTableCell(category_total_str, true);
        hb.endTableRow();
    }
    q1.Finalize();
    hb.endTable();

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();

    return hb.getHTMLText();
/*
    // Commented because there is not enough vertical space to show main page
    // without vertical scrollbar on 19-20" monitors.

    // Top 10 Graph.
    mmGraphTopCategories gtp;
    hb.addImage(gtp.getOutputFileName());
*/
}

wxString mmHomePagePanel::getCalendarWidget()
{
    mmHTMLBuilder hb;
    hb.startTable(wxT("98%"), wxT(""), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();

    wxString sDate = mmGetNiceDateString(wxDateTime::Now());    
    sDate = wxString::Format(_("Today's Date: %s"), sDate.c_str());
    hb.addHeaderItalic(1, sDate);
    
    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();
    hb.addLineBreak();
    hb.addLineBreak();
    return hb.getHTMLText();
}

wxString mmHomePagePanel::getStatWidget()
{
    mmHTMLBuilder hb;
    int countFollowUp = core_->bTransactionList_.countFollowupTransactions();
    hb.addLineBreak();
    hb.addLineBreak();

    hb.startTable(wxT("98%"), wxT(""), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();

    hb.startTable(wxT("100%"));

    hb.addTableHeaderRow(_("Transaction Statistics"), 2);

    if (countFollowUp > 0)
    {
        hb.startTableRow();
        hb.addTableCell(_("Follow Up On Transactions: "));
        hb.addTableCell(wxString::Format(wxT("%d"), countFollowUp), true, true, true);
        hb.endTableRow();
    }

    hb.startTableRow();
    hb.addTableCell( _("Total Transactions: "));
    hb.addTableCell(wxString::Format(wxT("%ld"), core_->bTransactionList_.transactions_.size()), true, true, true);
    hb.endTableRow();
    hb.endTable();

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();
    return hb.getHTMLText();
}

wxString mmHomePagePanel::displayGrandTotals(double& tBalance)
{
    mmHTMLBuilder hb;
    //  Display the grand total from all sections
    wxString tBalanceStr;
    core_->currencyList_.LoadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    hb.startTable(wxT("98%"), wxT(""), wxT("1"));
    hb.startTableRow();
    hb.startTableCell();

    hb.startTable(wxT("100%"));
    hb.addTotalRow(_("Grand Total:"), 2, tBalanceStr);
    hb.endTable();

    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();
    return hb.getHTMLText();
}

void mmHomePagePanel::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    htmlWindow_ = new mmHtmlWindow( this, frame_, core_,
        ID_PANEL_HOMEPAGE_HTMLWINDOW,
        wxDefaultPosition, wxDefaultSize,
        wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    itemBoxSizer2->Add(htmlWindow_, 1, wxGROW|wxALL, 0);
}

void mmHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
    wxString href = link.GetHref();
    wxString number;
    bool isAcct = href.StartsWith(wxT("ACCT:"), &number);
    bool isStock = href.StartsWith(wxT("STOCK:"), &number);
    if (href == wxT("billsdeposits"))
    {
        frame_->setNavTreeSection(_("Repeating Transactions"));
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BILLSDEPOSITS);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    else if (href == wxT("Assets"))
    {
        frame_->setNavTreeSection(_("Assets"));
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ASSETS);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    else if (isAcct)
    {
        long id = -1;
        number.ToLong(&id);
        frame_->setGotoAccountID(id);
        frame_->setAccountNavTreeSection(core_->accountList_.GetAccountName(id));
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_GOTOACCOUNT);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    else if (isStock)
    {
        long id = -1;
        number.ToLong(&id);
        frame_->setGotoAccountID(id);
        frame_->setAccountNavTreeSection(core_->accountList_.GetAccountName(id));
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_STOCKS);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
}

//----------------------------------------------------------------------------

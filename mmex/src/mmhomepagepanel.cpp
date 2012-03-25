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

#include "mmhomepagepanel.h"
#include "mmex.h"
#include "util.h"

#include "transdialog.h"
#include "newacctdialog.h"
#include "htmlbuilder.h"
#include "dbwrapper.h"
#include "billsdepositspanel.h"
#include "mmgraphincexpensesmonth.h"


BEGIN_EVENT_TABLE( mmHomePagePanel, wxPanel )
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mmHtmlWindow, wxHtmlWindow)
END_EVENT_TABLE()


mmHomePagePanel::mmHomePagePanel(mmGUIFrame* frame, 
            wxSQLite3Database* db, 
            wxSQLite3Database* inidb, 
            mmCoreDB* core, 
            const wxString& topCategories,
            wxWindow *parent,
            wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name )
: mmPanelBase(db, inidb, core), frame_(frame), topCategories_(topCategories)
{
    frame_->setHomePageActive();
    Create(parent, winid, pos, size, style, name);
}

mmHomePagePanel::~mmHomePagePanel()
{
    frame_->setHomePageActive(false);
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

    updateAccounts();

    return TRUE;
}

bool sortTransactionsByRemainingDaysHP(const mmBDTransactionHolder& elem1, 
                                       const mmBDTransactionHolder& elem2)
{
    return elem1.daysRemaining_ < elem2.daysRemaining_;
}

void mmHomePagePanel::displaySummaryHeader(mmHTMLBuilder& hb, wxString summaryTitle)
{
    hb.startTableRow();
    hb.addTableHeaderCell(summaryTitle, false);
    hb.addTableHeaderCell(_("Reconciled"), true);
    hb.addTableHeaderCell(_("Balance"), true);
    hb.endTableRow();
}
void mmHomePagePanel::displayStocksHeader(mmHTMLBuilder& hb, wxString summaryTitle)
{
    hb.startTableRow();
    hb.addTableHeaderCell(summaryTitle, false);
    hb.addTableHeaderCell(_("Gain/Loss"), true);
    hb.addTableHeaderCell(_("Total"), true);
    hb.endTableRow();
}

void mmHomePagePanel::displaySectionTotal(mmHTMLBuilder& hb, wxString totalsTitle, double tRecBalance, double& tBalance, bool showSeparator)
{
    // format the totals for display
    mmDBWrapper::loadBaseCurrencySettings(db_);
    wxString tRecBalanceStr;
    mmex::formatDoubleToCurrency(tRecBalance, tRecBalanceStr);
    wxString tBalanceStr;
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    // stocks don't have reconciled balances
    if (totalsTitle == _("Stocks Total:")) tRecBalanceStr = wxEmptyString;

    std::vector<wxString> data;
    data.push_back(tRecBalanceStr);
    data.push_back(tBalanceStr);

    // Show account totals with top and bottom separators
    if (showSeparator)
        hb.addRowSeparator(3);
    hb.startTableRow();
    hb.addTotalRow(totalsTitle, 3, data);
    hb.endTableRow();
    hb.addRowSeparator(3);
}

/* Checking Accounts */
void mmHomePagePanel::displayCheckingAccounts(mmHTMLBuilder& hb, double& tBalance, double& tIncome, double& tExpenses, const wxDateTime& dtBegin, const wxDateTime& dtEnd)
{
    // Only Show the account titles if we want to display Bank accounts.
    if ( frame_->expandedBankAccounts() ) 
        displaySummaryHeader(hb, _("Bank Account"));

    double tRecBalance = 0.0;

    // Get account balances and display accounts if we want them displayed 
    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = core_->rangeAccount();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        mmAccount* pCA = it->get();

        if (pCA->acctType_ != ACCOUNT_TYPE_BANK || pCA->status_ == mmAccount::MMEX_Closed) continue;

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(pCA->id_).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

        double bal = pCA->initialBalance_ + core_->getBalance(pCA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
        double reconciledBal = pCA->initialBalance_ + core_->getReconciledBalance(pCA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
        double rate = pCurrencyPtr->baseConv_;
        tBalance += bal * rate; // actual amount in that account in the original rate
        tRecBalance += reconciledBal * rate;

        // Display the individual Checking account links if we want to display them
        if ( frame_->expandedBankAccounts() ) 
        {
            double income = 0.0;
            double expenses = 0.0;
            core_->getExpensesIncome(pCA->id_, expenses, income, false, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);

            // show the actual amount in that account
            wxString balanceStr;
            wxString reconciledBalanceStr;
            mmex::formatDoubleToCurrency(bal, balanceStr);
            mmex::formatDoubleToCurrency(reconciledBal, reconciledBalanceStr);

            if ((vAccts == wxT("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pCA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
            {
                hb.startTableRow();
                hb.addTableCellLink(wxT("ACCT:") + wxString::Format(wxT("%d"), pCA->id_), pCA->name_, false, true);
                hb.addTableCell(reconciledBalanceStr, true);
                hb.addTableCell(balanceStr, true);
                hb.endTableRow();
            }

            // if bank accounts being displayed, include income/expense totals on home page.
            tIncome += income;
            tExpenses += expenses;
        }
    }
    displaySectionTotal(hb, _("Bank Accounts Total:"), tRecBalance, tBalance);
}

/* Term Accounts */
void mmHomePagePanel::displayTermAccounts(mmHTMLBuilder& hb, double& tBalance, double& tIncome, double& tExpenses, const wxDateTime& dtBegin, const wxDateTime& dtEnd)
{
    double tTermBalance = 0.0;
    double tRecBalance  = 0.0;

    // Only Show the account titles if Term accounts are active and we want them displayed.
    if ( frame_->expandedTermAccounts() )
        displaySummaryHeader(hb, _("Term Account"));

    // Get account balances and add to totals, and display accounts if we want them displayed 
    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = core_->rangeAccount(); 
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* pTA= it->get();
        if (pTA && pTA->status_== mmAccount::MMEX_Open && pTA->acctType_ == ACCOUNT_TYPE_TERM)
        {
            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(pTA->id_).lock();
            wxASSERT(pCurrencyPtr);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

            double bal = pTA->initialBalance_ + core_->getBalance(pTA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
            double reconciledBal = pTA->initialBalance_ + core_->getReconciledBalance(pTA->id_, mmIniOptions::instance().ignoreFutureTransactions_);
            double rate = pCurrencyPtr->baseConv_;
            tTermBalance += bal * rate; // actual amount in that account in the original rate
            tRecBalance  += reconciledBal * rate;

            // Display the individual Term account links if we want to display them
            if ( frame_->expandedTermAccounts() )
            {
                double income = 0;
                double expenses = 0;
                core_->getExpensesIncome(pTA->id_, expenses, income, false, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);

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
                    hb.addTableCellLink(wxT("ACCT:") + wxString::Format(wxT("%d"), pTA->id_), pTA->name_, false, true);
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

    displaySectionTotal(hb, _("Term Accounts Total:"), tRecBalance, tTermBalance, frame_->expandedTermAccounts());

    // Add Term balance to Grand Total balance
    tBalance += tTermBalance;
}

//* Stocks *//
void mmHomePagePanel::displayStocks(mmHTMLBuilder& hb, double& tBalance /*, double& tIncome, double& tExpenses */)
{
    double stTotalBalance = 0.0;
    wxString tBalanceStr;
    wxString tGainStr;

    if (frame_->expandedStockAccounts())
        displayStocksHeader(hb,_("Stocks"));

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

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    while(q1.NextRow())
    {
        int stockaccountId = q1.GetInt(wxT("ACCOUNTID"));
        double stockBalance = q1.GetDouble(wxT("BALANCE"));
        wxString stocknameStr = q1.GetString(wxT("ACCOUNTNAME"));
        //double income = q1.GetDouble(wxT("INCOME"));
        //double expenses = q1.GetDouble(wxT("EXPENCES"));
        double baseconvrate = q1.GetDouble(wxT("BASECONVRATE"));
        double stockGain = q1.GetDouble(wxT("GAIN"));

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(stockaccountId).lock();
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
            hb.addTableCellLink(wxT("STOCK:") + wxString::Format(wxT("%d"), stockaccountId), stocknameStr, false, true);
            hb.addTableCell(tGainStr, true);
            hb.addTableCell(tBalanceStr, true);
            hb.endTableRow();
        }
    }
    q1.Finalize();

    displaySectionTotal(hb, _("Stocks Total:"), 0.0, stTotalBalance, frame_->expandedStockAccounts());

    // Add Stock balance to Grand Total balance
    tBalance += stTotalBalance;
}

//* Assets *//
void mmHomePagePanel::displayAssets(mmHTMLBuilder& hb, double& tBalance)
{
    double assetBalance = mmDBWrapper::getAssetBalance(db_);
    wxString assetBalanceStr;
    mmDBWrapper::loadBaseCurrencySettings(db_);
    mmex::formatDoubleToCurrency(assetBalance, assetBalanceStr);

    if (mmIniOptions::instance().enableAssets_)
    {
        hb.startTableRow();
        hb.addTableCellLink(wxT("Assets"), _("Assets"), false, true);
        hb.addTableCell(wxT (""), true);
        hb.addTableCell(assetBalanceStr, true);
        hb.endTableRow();
        hb.addRowSeparator(3);
    }

    tBalance += assetBalance;
}

//* Currencies *//
void mmHomePagePanel::displayCurrencies(mmHTMLBuilder& hb)
{
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

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql2);
    q1 = db_->ExecuteQuery(sql2);
        
    //Determine how many currencies used
    int curnumber = 0;
    while(q1.NextRow())
        curnumber+=1;   

    if (curnumber > 1 )
    {
        // display the currency header
        hb.startTable(wxT("95%"));
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

            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(accountId).lock();
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
        hb.addRowSeparator(3);
        hb.endTable();
        q1.Finalize();
    }
}

//* Income vs Expenses *//
void mmHomePagePanel::displayIncomeVsExpenses(mmHTMLBuilder& hb, double& tincome, double& texpenses)
{
    hb.startTable(wxT("95%"));

    wxString incStr, expStr, difStr;
    mmex::formatDoubleToCurrency(tincome, incStr); // must use loadBaseCurrencySettings (called above)
    mmex::formatDoubleToCurrency(texpenses, expStr);
    mmex::formatDoubleToCurrency(tincome-texpenses, difStr);

    mmGraphIncExpensesMonth gg;
    gg.init(tincome, texpenses);
    gg.Generate(wxT(""));

    wxString monthHeading = _("Current Month");
    if (mmIniOptions::instance().ignoreFutureTransactions_) monthHeading = _("Current Month to Date");

    hb.addTableHeaderRow(wxString() << _("Income vs Expenses: ") << monthHeading, 2);
    
    hb.startTableRow();
    //hb.startTableCell(wxT("50%\" align=\"center"));
    hb.startTableCell();
    hb.addImage(gg.getOutputFileName());
    hb.endTableCell();

    //hb.startTableCell(wxT("50%\" align=\"center"));
    hb.startTableCell();
    //start table in table
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

    hb.addRowSeparator(2);
    hb.startTableRow();
    hb.addTableCell(_("Difference:"), false, true, true);
    hb.addTableCell(difStr, true, true, true, (tincome-texpenses < 0.0 ? wxT("RED"):wxT("")));

    hb.endTableRow();

    hb.endTable();
    hb.endTableCell();
    //end table in table

    hb.endTableRow();

    hb.addRowSeparator(2);
    hb.endTable();

}

//* bills & deposits *//
void mmHomePagePanel::displayBillsAndDeposits(mmHTMLBuilder& hb)
{
    std::vector<mmBDTransactionHolder> trans_;
    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select BDID, NEXTOCCURRENCEDATE, NUMOCCURRENCES, REPEATS, PAYEEID, TRANSCODE, ACCOUNTID, TOACCOUNTID, TRANSAMOUNT, TOTRANSAMOUNT from BILLSDEPOSITS_V1");

    bool visibleEntries = false;
    while (q1.NextRow())
    {
        mmBDTransactionHolder th;

        th.id_           = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));
        th.nextOccurStr_   = mmGetDateForDisplay(db_, th.nextOccurDate_);
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
            if ((repeats > 10) && (numRepeats < 0) )
                th.daysRemainingStr_ = _("Inactive");
        }

        if (th.daysRemaining_ < 0)
        {
            th.daysRemainingStr_ = wxString::Format(wxT("%d"), abs(th.daysRemaining_)) + _(" days overdue!");
            if ((repeats > 10) && (numRepeats < 0) )
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

        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

        if (th.transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            wxString fromAccount = core_->getAccountName(th.accountID_);
            wxString toAccount = core_->getAccountName(th.toAccountID_ );

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
        hb.startTable(wxT("95%"));
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
                boost::weak_ptr<mmCurrency> wpCurrency = core_->getCurrencyWeakPtr(trans_[bdidx].accountID_);
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
        hb.addRowSeparator(3);
        hb.endTable();
    }
}

void mmHomePagePanel::displayTopTransactions(mmHTMLBuilder& hb)
{
    mmex::CurrencyFormatter::instance().loadDefaultSettings();
    mmDBWrapper::loadBaseCurrencySettings(db_);
    
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

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql3);

    std::vector<CategInfo> categList;

    wxString headerMsg = wxString() << _("Top Withdrawals: ") << _("Last 30 Days");

    hb.startTable(wxT("95%"));
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
        mmDBWrapper::loadBaseCurrencySettings(core_->db_.get());
        mmex::formatDoubleToCurrency(category_total, category_total_str);

        hb.startTableRow();
        hb.addTableCell(q1.GetString(wxT("SUBCATEGORY")), false, true);
        hb.addTableCell(q1.GetString(wxT("NUMBER")), true, true);
        hb.addTableCell(category_total_str, true);
        hb.endTableRow();
    }
    q1.Finalize();
    hb.addRowSeparator(3);
    hb.endTable();

    hb.getHTMLText();
/*
    // Commented because there is not enough vertical space to show main page
    // without vertical scrollbar on 19-20" monitors.

    // Top 10 Graph.
    mmGraphTopCategories gtp;
    hb.addImage(gtp.getOutputFileName());
*/
}

void mmHomePagePanel::displayStatistics(mmHTMLBuilder& hb) 
{
    int countFollowUp = core_->countFollowupTransactions();
    hb.addLineBreak();
    hb.addLineBreak();
    hb.startTable(wxT("95%"));

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
    hb.addTableCell(wxString::Format(wxT("%d"), core_->bTransactionList_.transactions_.size()), true, true, true);
    hb.endTableRow();
    hb.addRowSeparator(2);
    hb.endTable();
}

void mmHomePagePanel::displayGrandTotals(mmHTMLBuilder& hb, double& tBalance)
{
//  Display the grand total from all sections
    wxString tBalanceStr;
    mmDBWrapper::loadBaseCurrencySettings(db_);
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    hb.startTable(wxT("95%"));
    hb.addTotalRow(_("Grand Total:"), 2, tBalanceStr);
    //hb.addRowSeparator(2);
    hb.endTable();
    hb.addLineBreak();
    hb.addLineBreak();
}

void mmHomePagePanel::updateAccounts()
{
    if (!db_)
        return;

    mmHTMLBuilder hb;
    hb.init();
    wxDateTime today = wxDateTime::Now();
    wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
    wxDateTime dtBegin = prevMonthEnd;
    wxDateTime dtEnd = wxDateTime::Now().GetLastMonthDay();

    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(5, dt);
    hb.addLineBreak();

    hb.startCenter();
    hb.startTable(wxT("98%"), wxT("top")); 
    hb.startTableRow();

    double tBalance = 0.0;
    double tIncome = 0.0;
    double tExpenses = 0.0;

    hb.startTableCell(wxT("50%\" align=\"center")); 

    hb.startTable(wxT("95%")); 
    displayCheckingAccounts(hb,tBalance,tIncome,tExpenses, dtBegin, dtEnd);
    if ( frame_->hasActiveTermAccounts() )
        displayTermAccounts(hb,tBalance,tIncome,tExpenses, dtBegin, dtEnd);
    if (core_->accountList_.has_stock_account()) displayStocks(hb,tBalance /*,tIncome,tExpenses */);
    displayAssets(hb, tBalance);
    hb.endTable(); 
 
    displayCurrencies(hb); // Will display Currency summary when more than one currency is used.

    displayGrandTotals(hb, tBalance);
 
    displayTopTransactions(hb); 
    hb.endTableCell();

    hb.startTableCell(wxT("50%\" align=\"center")); 
    displayIncomeVsExpenses(hb, tIncome, tExpenses); //Also displays the Income vs Expenses graph.
    displayBillsAndDeposits(hb); 
    displayStatistics(hb);
    hb.endTableCell();
    hb.endTableRow();
    hb.endTable();
    hb.end();

    wxString htmlText = hb.getHTMLText();
    htmlWindow_->SetPage(htmlText);
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
        frame_->setAccountNavTreeSection(core_->getAccountName(id));
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_GOTOACCOUNT);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    else if (isStock)
    {
        long id = -1;
        number.ToLong(&id);
        frame_->setGotoAccountID(id);
        frame_->setAccountNavTreeSection(core_->getAccountName(id));
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_STOCKS);
        frame_->GetEventHandler()->AddPendingEvent(evt); 
    }
}

//----------------------------------------------------------------------------

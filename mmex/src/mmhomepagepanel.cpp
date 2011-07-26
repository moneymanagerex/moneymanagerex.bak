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

#include <map>

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


namespace
{

inline boost::shared_ptr<mmCurrency> getCurrency(mmCoreDB* core, int currencyID)
{
    wxASSERT(core);
    return core->currencyList_.getCurrencySharedPtr(currencyID);
}


/*
    Returns true if _Left precedes and is not equal to _Right in the sort order.
*/
struct CurrencyCompare : public std::binary_function<boost::shared_ptr<mmCurrency>, 
                                                     boost::shared_ptr<mmCurrency>, bool> 
{
    bool operator() (const boost::shared_ptr<mmCurrency> &_Left, const boost::shared_ptr<mmCurrency> &_Right) const
    {
        return _Left->currencyName_ < _Right->currencyName_; // order by currency name
    }
};


} // namespace


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
{
    db_ = db;
    inidb_ = inidb;
    core_ = core;
    frame_ = frame;
    topCategories_ = topCategories;
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

bool sortTransactionsByRemainingDaysHP( mmBDTransactionHolder elem1, 
                                       mmBDTransactionHolder elem2 )
{
    return elem1.daysRemaining_ < elem2.daysRemaining_;
}

void mmHomePagePanel::displaySummaryHeader(mmHTMLBuilder& hb, wxString summaryTitle)
{
    hb.startTableRow();
	hb.addTableHeaderCell(summaryTitle);
	hb.addTableHeaderCell(_("Summary"));
    hb.endTableRow();
}

void mmHomePagePanel::displaySectionTotal(mmHTMLBuilder& hb, wxString totalsTitle, double& tBalance, bool showSeparator)
{
    // format the totals for display
    mmDBWrapper::loadBaseCurrencySettings(db_);
    wxString tBalanceStr;
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    // Show account totals with top and bottom separators
    if (showSeparator)
        hb.addRowSeparator(2);
    hb.startTableRow();
    hb.addTotalRow(totalsTitle, 2, tBalanceStr);
    hb.endTableRow();
    hb.addRowSeparator(2);
}

/* Checking Accounts */
void mmHomePagePanel::displayCheckingAccounts(mmHTMLBuilder& hb, double& tBalance, double& tIncome, double& tExpenses, wxDateTime& dtBegin, wxDateTime& dtEnd)
{
    // Only Show the account titles if we want to display Bank accounts.
    if ( frame_->expandedBankAccounts() ) 
    {
        displaySummaryHeader(hb, _("Bank Account"));
    }

    // Get account balances and display accounts if we want them displayed 
    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));
    for (size_t iAdx = 0; iAdx < core_->accountList_.accounts_.size(); ++iAdx)
    {
        mmCheckingAccount* pCA = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
        if (pCA && pCA->status_== mmAccount::MMEX_Open)
        {
            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pCA->accountID_).lock();
            wxASSERT(pCurrencyPtr);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

            double bal = pCA->initialBalance_ + core_->bTransactionList_.getBalance(pCA->accountID_);
            double rate = pCurrencyPtr->baseConv_;
            tBalance += bal * rate; // actual amount in that account in the original rate
            
            // Display the individual Checking account links if we want to display them
            if ( frame_->expandedBankAccounts() ) 
            {
                double income = 0.0;
                double expenses = 0.0;
                core_->bTransactionList_.getExpensesIncome(pCA->accountID_, expenses, income, false,dtBegin, dtEnd);

                // show the actual amount in that account
                wxString balance;
                mmex::formatDoubleToCurrency(bal, balance);

                if ((vAccts == wxT("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
                    (vAccts == wxT("Favorites") && pCA->favoriteAcct_) ||
                    (vAccts == wxT("ALL")))
                {
                    hb.startTableRow();
                    hb.addTableCellLink(wxT("ACCT:") + wxString::Format(wxT("%d"), pCA->accountID_), pCA->accountName_, false, true);
                    hb.addTableCell(balance, true);
                    hb.endTableRow();
                }

                // if bank accounts being displayed, include income/expense totals on home page.
                tIncome += income;
                tExpenses += expenses;
            }
        }
	}
    displaySectionTotal(hb, _("Bank Accounts Total:"), tBalance);
}

/* Term Accounts */
void mmHomePagePanel::displayTermAccounts(mmHTMLBuilder& hb, double& tBalance, double& tIncome, double& tExpenses, wxDateTime& dtBegin, wxDateTime& dtEnd)
{
    double tTermBalance = 0.0;

    // Only Show the account titles if Term accounts are active and we want them displayed.
    if ( frame_->expandedTermAccounts() )
    {
        displaySummaryHeader(hb, _("Term Account"));
    }

    // Get account balances and add to totals, and display accounts if we want them displayed 
    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));
    for (size_t iAdx = 0; iAdx < core_->accountList_.accounts_.size(); ++iAdx)
    {
        mmTermAccount* pTA = dynamic_cast<mmTermAccount*>(core_->accountList_.accounts_[iAdx].get());
        if (pTA && pTA->status_== mmAccount::MMEX_Open)
        {
            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pTA->accountID_).lock();
            wxASSERT(pCurrencyPtr);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

            double bal = pTA->initialBalance_ + core_->bTransactionList_.getBalance(pTA->accountID_);
            double rate = pCurrencyPtr->baseConv_;
            tTermBalance += bal * rate; // actual amount in that account in the original rate

            // Display the individual Term account links if we want to display them
            if ( frame_->expandedTermAccounts() )
            {
                double income = 0;
                double expenses = 0;
                core_->bTransactionList_.getExpensesIncome(pTA->accountID_, expenses, income, false,dtBegin, dtEnd);

                // show the actual amount in that account
                wxString balance;
                mmex::formatDoubleToCurrency(bal, balance);

                if ((vAccts == wxT("Open") && pTA->status_ == mmAccount::MMEX_Open) ||
                    (vAccts == wxT("Favorites") && pTA->favoriteAcct_) ||
                    (vAccts == wxT("ALL")))
                {
                    hb.startTableRow();
                    hb.addTableCellLink(wxT("ACCT:") + wxString::Format(wxT("%d"), pTA->accountID_), pTA->accountName_, false, true);
                    hb.addTableCell(balance, true);
                    hb.endTableRow();
                }

                // if Term accounts being displayed, include income/expense totals on home page.
                tIncome += income;
                tExpenses += expenses;
            }
        }
	}

    displaySectionTotal(hb, _("Term Accounts Total:"), tTermBalance, frame_->expandedTermAccounts());

    // Add Term balance to Grand Total balance
    tBalance += tTermBalance;
}

//* Stocks *//
void mmHomePagePanel::displayStocks(mmHTMLBuilder& hb, double& tBalance, double& tIncome, double& tExpenses)
{
    double stTotalBalance = 0.0;
    wxString tBalanceStr;

    if (frame_->expandedStockAccounts())
        displaySummaryHeader(hb,_("Stocks"));

    char sql[] = 
        "select INCOME, EXPENCES, t.accountid as ACCOUNTID, a.accountname as ACCOUNTNAME, "
        "sum (t.BALANCE) as BALANCE, c.BASECONVRATE as BASECONVRATE "
        "from (  "
        "select 0 as INCOME,0 as EXPENCES, acc.accountid as ACCOUNTID, acc.INITIALBAL as BALANCE "
        "from ACCOUNTLIST_V1 ACC "
        "where ACC.ACCOUNTTYPE = 'Investment' and ACC.STATUS='Open' "
        "group by acc.accountid  "
        "union all "
        "select  0, 0, "
        "st.heldat as ACCOUNTID,  "
        "total((st.CURRENTPRICE)*st.NUMSHARES-st.COMMISSION) as BALANCE "
        "from  stock_v1 st "
        "where st.purchasedate<=date ('now','localtime') "
        "group by st.heldat "
        "union all "
        "select 0, 0, ca.toaccountid,  total(ca.totransamount)  "
        "from checkingaccount_v1 ca "
        "inner join  accountlist_v1 acc on acc.accountid=ca.toaccountid "
        "where ca.transcode ='Transfer' and ca.STATUS<>'V' and ca.transdate<=date ('now','localtime') "
        "and acc.accounttype='Investment' "
        "group by ca.toaccountid "
        "union all "
        "select total(case ca.transcode when 'Deposit' then ca.transamount else 0 end), "
        "total(case ca.transcode when 'Withdrawal' then ca.transamount else 0 end) , "
        "ca.accountid,  total(case ca.transcode when 'Deposit' then ca.transamount else -ca.transamount end)  "
        "from checkingaccount_v1 ca "
        "inner join  accountlist_v1 acc on acc.accountid=ca.accountid "
        "where ca.STATUS<>'V' and ca.transdate<=date ('now','localtime') "
        "and acc.accounttype='Investment' "
        "group by ca.accountid) t "
        "left join accountlist_v1 a on a.accountid=t.accountid "
        "left join currencyformats_v1 c on c.currencyid=a.currencyid "
        "where a.status='Open' "
        "group by t.accountid ";

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    while(q1.NextRow())
    {
        int stockaccountId = q1.GetInt(wxT("ACCOUNTID"));
        double stockBalance = q1.GetDouble(wxT("BALANCE"));
        wxString stocknameStr = q1.GetString(wxT("ACCOUNTNAME"));
        double income = q1.GetDouble(wxT("INCOME"));
        double expenses = q1.GetDouble(wxT("EXPENCES"));
        double baseconvrate = q1.GetDouble(wxT("BASECONVRATE"));

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(stockaccountId).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

        mmex::formatDoubleToCurrency(stockBalance, tBalanceStr);
        
        // if Stock accounts being displayed, include income/expense totals on home page.
        tIncome += income * baseconvrate;
        tExpenses += expenses * baseconvrate;
        stTotalBalance += stockBalance * baseconvrate; 
        //We can hide or show Stocks on Home Page
        if (frame_->expandedStockAccounts())
        {
            hb.startTableRow();
            //////
            hb.addTableCell(stocknameStr, false,true);
            //hb.addTableCellLink(wxT("STOCK:") + wxString::Format(wxT("%d"), stockaccountId), stocknameStr, false, true);
            hb.addTableCell(tBalanceStr, true);
            hb.endTableRow();
        }
    }
    q1.Finalize();

    displaySectionTotal(hb, _("Stocks Total:"), stTotalBalance, frame_->expandedStockAccounts());

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

    if (mmIniOptions::enableAssets_)
    {
		hb.startTableRow();
		hb.addTableCellLink(wxT("Assets"), _("Assets"), false, true);
		hb.addTableCell(assetBalanceStr, true);
		hb.endTableRow();
		hb.addRowSeparator(2);
    }

    tBalance += assetBalance;
}

//* Currencies *//
void mmHomePagePanel::displayCurrencies(mmHTMLBuilder& hb)
{
    char sql2[] = 
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
    {
        curnumber+=1;   
    }

    if (curnumber > 1 )
    {
        // display the currency header
        hb.startTable(wxT("95%"));
        hb.startTableRow();
        hb.addTableHeaderCell(_("Currency"));
        hb.addTableHeaderCell(_("Base Rate"));
        hb.addTableHeaderCell(_("Summary"));
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
        hb.addRowSeparator(3);
        hb.endTable();
        q1.Finalize();
    }
}

//* Income vs Expenses *//
void mmHomePagePanel::displayIncomeVsExpenses(mmHTMLBuilder& hb, double& tincome, double& texpenses)
{
    hb.startTable(wxT("95%"));

    wxString incStr, expStr, difStr, colorStr;
    mmex::formatDoubleToCurrency(tincome, incStr); // must use loadBaseCurrencySettings (called above)
    mmex::formatDoubleToCurrency(texpenses, expStr);
    mmex::formatDoubleToCurrency(tincome-texpenses, difStr);
    if (tincome-texpenses<0)
    {
        colorStr = wxT("#FF6600");
    }
    mmGraphIncExpensesMonth gg;
    gg.init(tincome, texpenses);
    gg.Generate(wxT(""));

	hb.addTableHeaderRow(_("Income vs Expenses: Current Month"), 2);
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
	hb.addTableCell(difStr, true, true, true, colorStr);
	hb.endTableRow();
	hb.endTable();

    // Add the graph
    hb.startTable(wxT("50%"), wxT("top\" align=\"center"));
	hb.startTableRow();
	hb.startTableCell(wxT("50%\" align=\"center"));
    hb.addImage(gg.getOutputFileName());
    hb.endTableCell();
	hb.endTableRow();
    hb.endTable();
}

//* bills & deposits *//
void mmHomePagePanel::displayBillsAndDeposits(mmHTMLBuilder& hb)
{
    std::vector<mmBDTransactionHolder> trans_;
    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select BDID, NEXTOCCURRENCEDATE, PAYEEID, TRANSCODE, ACCOUNTID, TOACCOUNTID, TRANSAMOUNT, TOTRANSAMOUNT from BILLSDEPOSITS_V1");

    bool visibleEntries = false;
    while (q1.NextRow())
    {
        mmBDTransactionHolder th;

        th.bdID_           = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));
        th.nextOccurStr_   = mmGetDateForDisplay(db_, th.nextOccurDate_);

        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = th.nextOccurDate_.Subtract(today);
        th.daysRemaining_ = ts.GetDays();
        int hoursRemaining_ = ts.GetHours();

        if (hoursRemaining_ > 0)
            th.daysRemaining_ += 1;

        if (th.daysRemaining_ >= 0)
        {
            th.daysRemainingStr_ = wxString::Format(wxT("%d"), th.daysRemaining_) + 
            _(" days remaining");
        }
        else
        {
            th.daysRemainingStr_ = wxString::Format(wxT("%d"), abs(th.daysRemaining_)) + 
            _(" days overdue!");
        }

        th.payeeID_        = q1.GetInt(wxT("PAYEEID"));
        th.transType_      = q1.GetString(wxT("TRANSCODE"));
        th.accountID_      = q1.GetInt(wxT("ACCOUNTID"));
        th.toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));

        th.amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
        th.toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));

        mmex::formatDoubleToCurrencyEdit(th.amt_, th.transAmtString_);
        //for Withdrawal amount should be negative
		if (th.transType_== wxT("Withdrawal"))
		{
			th.transAmtString_= wxT ("-") + th.transAmtString_;
			th.amt_ = -th.amt_;
		}

        mmex::formatDoubleToCurrencyEdit(th.toAmt_, th.transToAmtString_);

        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

        if (th.transType_ == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_,  th.accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  th.toAccountID_ );

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
                wxString daysRemainingStr_;
                colorStr = wxT("#9999FF");

                daysRemainingStr_ = trans_[bdidx].daysRemainingStr_;
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
			    hb.addTableCell(daysRemainingStr_, true, false, false, colorStr);
			    hb.endTableRow();
            }
        }

        hb.endTable();
    }
}

void mmHomePagePanel::displayTopTransactions(mmHTMLBuilder& hb)
{
    mmex::CurrencyFormatter::instance().loadDefaultSettings();
    mmDBWrapper::loadBaseCurrencySettings(db_);
    
    char sql3[] = 
        "select ACCOUNTNAME, "
        "CATEG|| (Case SUBCATEG when '' then '' else ':' end )||  SUBCATEG as SUBCATEGORY "
        ", AMOUNT "
        "from ( "
        "select coalesce(CAT.CATEGNAME, SCAT.CATEGNAME) as CATEG, "
        "coalesce(SUBCAT.SUBCATEGNAME, SSCAT.SUBCATEGNAME,'') as SUBCATEG, "
        "ACC.ACCOUNTNAME as ACCOUNTNAME, "
        "(ROUND((case CANS.TRANSCODE when 'Withdrawal' then -1 else 1 end) "
        "* (case CANS.CATEGID when -1 then ST.SPLITTRANSAMOUNT else CANS.TRANSAMOUNT end),2) "
        "* CF.BASECONVRATE "
        ") as AMOUNT "
        "from  CHECKINGACCOUNT_V1 CANS "
        "left join CATEGORY_V1 CAT on CAT.CATEGID = CANS.CATEGID "
        "left join SUBCATEGORY_V1 SUBCAT on SUBCAT.SUBCATEGID = CANS.SUBCATEGID and SUBCAT.CATEGID = CANS.CATEGID "
        "left join ACCOUNTLIST_V1 ACC on ACC.ACCOUNTID = CANS.ACCOUNTID "
        "left join SPLITTRANSACTIONS_V1 ST on CANS.TRANSID = ST.TRANSID "
        "left join CATEGORY_V1 SCAT on SCAT.CATEGID = ST.CATEGID and CANS.TRANSID=ST.TRANSID "
        "left join SUBCATEGORY_V1 SSCAT on SSCAT.SUBCATEGID = ST.SUBCATEGID and SSCAT.CATEGID = ST.CATEGID and CANS.TRANSID=ST.TRANSID "
        "left join CURRENCYFORMATS_V1 CF on CF.CURRENCYID = ACC.CURRENCYID "
        "where  CANS.TRANSCODE<>'Transfer' and CANS.TRANSDATE > date('now','localtime', '-1 month') and CANS.TRANSDATE <= date('now','localtime') "
        //"group by CATEG, SUBCATEG "
        "order by ABS (AMOUNT) DESC, ACCOUNTNAME, CATEG, SUBCATEG, AMOUNT "
        "limit 10 "
        ") where AMOUNT <> 0" ;

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql3);

    wxString catAmountStr;
    std::vector<CategInfo> categList;

    hb.startTable(wxT("95%"));
    hb.addTableHeaderRow(_("Top Transactions Last 30 Days"), 3);
    while(q1.NextRow())
    {
        double catAmount = q1.GetDouble(wxT("AMOUNT"));
        wxString subcategString = q1.GetString(wxT("SUBCATEGORY"));
        wxString accountString = q1.GetString(wxT("ACCOUNTNAME"));

        mmex::formatDoubleToCurrency(catAmount, catAmountStr);

		hb.startTableRow();
		hb.addTableCell(accountString, false, true);
		hb.addTableCell(subcategString, false, true);
		hb.addTableCell(catAmountStr, true);
		hb.endTableRow();
    }
    q1.Finalize();
    hb.endTable();

    hb.getHTMLText();
/*
    // Commented because there is not enough vertical space to show main page
    // without vertical scrollbar on 19-20" monitors.

    // Top 10 Graph.
    mmGraphTopCategories gtp;
    hb.addImage(gtp.getOutputFileName());
*/

    int countFollowUp = core_->bTransactionList_.countFollowupTransactions();

//	hb.addRowSeparator(2);
    hb.addLineBreak();
    hb.addLineBreak();
    hb.startTable(wxT("95%"));


    hb.addTableHeaderRow(_("Transaction Statistics"), 2);

    if (countFollowUp > 0)
    {
        hb.startTableRow();
        hb.addTableCell(_("Follow Up On Transactions: "));
        //Draw it as numeric that mean align right
        hb.addTableCell(wxString::Format(wxT("<b>%d</b> "), countFollowUp), true);
        hb.endTableRow();
    }

    hb.startTableRow();
    hb.addTableCell( _("Total Transactions: "));  
    hb.addTableCell(wxString::Format(wxT("<b>%d</b> "), core_->bTransactionList_.transactions_.size()), true);
    hb.endTableRow();
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
    wxDateTime dtEnd = wxDateTime::Now();

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

    displayStocks(hb,tBalance,tIncome,tExpenses);

    displayAssets(hb, tBalance);
    hb.endTable(); 
 
    displayCurrencies(hb); // Will display Currency summary when more than one currency is used.

    displayGrandTotals(hb, tBalance);
 
    displayBillsAndDeposits(hb); 

    hb.endTableCell();
    hb.startTableCell(wxT("50%\" align=\"center")); 

    displayIncomeVsExpenses(hb, tIncome, tExpenses); //Also displays the Income vs Expenses graph.

    displayTopTransactions(hb); 

    hb.endTableCell();
    hb.endTableRow();
	hb.endTable();
    hb.end();

    wxString htmlText = hb.getHTMLText();
    htmlWindow_->SetPage(htmlText);
}

void mmHomePagePanel::CreateControls()
{    
    mmHomePagePanel* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    htmlWindow_ = new mmHtmlWindow( itemDialog1, frame_, core_, 
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
    //bool isStock = href.StartsWith(wxT("STOCK:"), &number);
    if (href == wxT("billsdeposits"))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BILLSDEPOSITS);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    else if (isAcct)
    {
        long id = -1;
        number.ToLong(&id);
        frame_->setGotoAccountID(id);
        frame_->setAccountNavTreeSection(core_->accountList_.getAccountName(id));
//        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_GOTOACCOUNT);
//        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    //else if (isStock)
    //{
    //    wxString msgStr =
    //        wxString()  << _("Please use the stock link on the Navigation Tree") 
    //                    << _("\n\n") 
    //                    << _("Waiting Correct Implementation");
    //    wxMessageBox(msgStr,_("Stock Investment Page Selection"));
    //}
    else if (href == wxT("Assets"))
    {
        frame_->setNavTreeSection(wxT("Assets"));

    //  wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ASSETS);
    //  frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    //else if (href == wxT("Stocks"))
    //{
    //    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_STOCKS);
    //    frame_->GetEventHandler()->AddPendingEvent(evt);
    //}
    // Deselect any selections in the tree ctrl, so tree works
//    frame_->unselectNavTree();
}

//----------------------------------------------------------------------------

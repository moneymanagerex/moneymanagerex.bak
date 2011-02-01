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
    Create(parent, winid, pos, size, style, name);
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
	hb.startTable(wxT("75%"), wxT("top"));
	hb.startTableRow();
	hb.startTableCell();

	hb.startTable(wxT("95%"));

	hb.startTableRow();
	hb.addTableHeaderCell(_("Account"));
	hb.addTableHeaderCell(_("Summary"));
	hb.endTableRow();

    /////////////////   

    
    double tincome = 0.0;
    double texpenses = 0.0;
    double tBalance = 0.0;

    typedef std::map<boost::shared_ptr<mmCurrency>, double, CurrencyCompare> balances_t;
    balances_t tBalances;
    bool print_bal4cur = false;

    /* Checking Accounts */
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

            // Add account to currency differences
            tBalances[pCurrencyPtr] += bal;
            print_bal4cur |= (bal != 0.0 && tBalances[pCurrencyPtr] != 0.0);

            // show the actual amount in that account
            wxString balance;
            mmex::formatDoubleToCurrency(bal, balance);

            double income = 0;
            double expenses = 0;

            core_->bTransactionList_.getExpensesIncome(pCA->accountID_, expenses, income, false,dtBegin, dtEnd);

            if ( frame_->expandedBankAccounts() ) 
            {
                if ((vAccts == wxT("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
                    (vAccts == wxT("Favorites") && pCA->favoriteAcct_) ||
                    (vAccts == wxT("ALL")))
                {
                    hb.startTableRow();
                    hb.addTableCellLink(wxT("ACCT:") + wxString::Format(wxT("%d"), pCA->accountID_), pCA->accountName_, false, true);
                    hb.addTableCell(balance, true);
                    hb.endTableRow();
                }
            }

            tincome += income;
            texpenses += expenses;
        }
	}

    mmDBWrapper::loadBaseCurrencySettings(db_);
    wxString tBalanceStr;
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    hb.startTableRow();
	hb.addTotalRow(_("Bank Accounts Total:"), 2, tBalanceStr);
    hb.endTableRow();
    hb.addRowSeparator(2);

    double tTermBalance = 0.0;

    /* Term Accounts */
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

            // Add account to currency differences
            tBalances[pCurrencyPtr] += bal;
            print_bal4cur |= (bal != 0.0 && tBalances[pCurrencyPtr] != 0.0);

            // show the actual amount in that account
            wxString balance;
            mmex::formatDoubleToCurrency(bal, balance);

            double income = 0;
            double expenses = 0;

            core_->bTransactionList_.getExpensesIncome(pTA->accountID_, expenses, income, false,dtBegin, dtEnd);

            if ( frame_->expandedTermAccounts() )
            {
                if ((vAccts == wxT("Open") && pTA->status_ == mmAccount::MMEX_Open) ||
                    (vAccts == wxT("Favorites") && pTA->favoriteAcct_) ||
                    (vAccts == wxT("ALL")))
                {
                    hb.startTableRow();
                    hb.addTableCellLink(wxT("ACCT:") + wxString::Format(wxT("%d"), pTA->accountID_), pTA->accountName_, false, true);
                    hb.addTableCell(balance, true);
                    hb.endTableRow();
                }
            }

            tincome += income;
            texpenses += expenses;
        }
	}

    mmDBWrapper::loadBaseCurrencySettings(db_);
    wxString tTermBalanceStr;
    mmex::formatDoubleToCurrency(tTermBalance, tTermBalanceStr);

    if ( frame_->hasActiveTermAccounts() )
    {
        hb.startTableRow();
	    hb.addTotalRow(_("Term Accounts Total:"), 2, tTermBalanceStr);
        hb.endTableRow();
   		hb.addRowSeparator(2);
    }

    // Add Term balance to total account balances
    tBalance += tTermBalance;

    const int BaseCurrencyID = mmDBWrapper::getBaseCurrencySettings(db_);
    boost::shared_ptr<mmCurrency> BaseCurrency(BaseCurrencyID != -1 ? getCurrency(core_, BaseCurrencyID) : boost::shared_ptr<mmCurrency>());

    if (mmIniOptions::enableStocks_)
    {
        /* Stocks */
        double invested;
        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_, invested);
        wxString stockBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmex::formatDoubleToCurrency(stockBalance, stockBalanceStr);

		hb.startTableRow();
		hb.addTableCellLink(wxT("Stocks"), _("Stock Investments"), false, true);
		hb.addTableCell(stockBalanceStr, true);
		hb.endTableRow();
		hb.addRowSeparator(2);

        tBalance += stockBalance;
        
        if (BaseCurrency)
        {
            print_bal4cur |= (stockBalance != 0.0 && tBalances[BaseCurrency] != 0.0);
            tBalances[BaseCurrency] += stockBalance;
        }
    }

    if (mmIniOptions::enableAssets_)
    {
        /* Assets */
        double assetBalance = mmDBWrapper::getAssetBalance(db_);
        wxString assetBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmex::formatDoubleToCurrency(assetBalance, assetBalanceStr);

		hb.startTableRow();
		hb.addTableCellLink(wxT("Assets"), _("Assets"), false, true);
		hb.addTableCell(assetBalanceStr, true);
		hb.endTableRow();
		hb.addRowSeparator(2);

        tBalance += assetBalance;

        if (BaseCurrency)
        {
            print_bal4cur |= (assetBalance != 0.0 && tBalances[BaseCurrency] != 0.0);
            tBalances[BaseCurrency] += assetBalance;
        }
    }
    
    if (print_bal4cur && tBalances.size() > 1) // print total balance for every currency
    {
        hb.startTableRow();
        hb.addTableHeaderCell(_("Currency"));
        hb.addTableHeaderCell(_("Summary"));
        hb.endTableRow();

        for (balances_t::const_iterator i = tBalances.begin(); i != tBalances.end(); ++i)
        {
            wxString tBalanceStr;
            mmex::CurrencyFormatter::instance().loadSettings(*i->first);
            mmex::formatDoubleToCurrency(i->second, tBalanceStr);

            hb.startTableRow();
            hb.addTableCell(i->first->currencyName_);
            hb.addTableCell(tBalanceStr, true);
            hb.endTableRow();
        }
    } 

    tBalances.clear();

    mmDBWrapper::loadBaseCurrencySettings(db_);
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

	hb.addTotalRow(_("Total of Accounts:"), 2, tBalanceStr);
	hb.addRowSeparator(2);
    hb.endTable();

	hb.endTableCell();
	hb.startTableCell();

	hb.startTable(wxT("95%"));

    wxString incStr, expStr;
    mmex::formatDoubleToCurrency(tincome, incStr); // must use loadBaseCurrencySettings (called above)
    mmex::formatDoubleToCurrency(texpenses, expStr);

    wxString baseInc = _("Income");
    wxString baseExp = _("Expense");
    wxString income = baseInc + incStr;
    wxString expense = baseExp + expStr;

    mmGraphIncExpensesMonth gg;
    gg.init(tincome, texpenses);
    gg.Generate(wxT(""));

	hb.addTableHeaderRow(_("Income vs Expenses: Current Month"), 2);
	hb.startTableRow();
	hb.addTableCell(baseInc, false, true);
	hb.addTableCell(incStr, true);
	hb.endTableRow();
    
	hb.startTableRow();
	hb.addTableCell(baseExp, false, true);
	hb.addTableCell(expStr, true);
	hb.endTableRow();

    hb.endTable();

    // Add the graph
    hb.addImage(gg.getOutputFileName());

	hb.endTableCell();
	hb.endTableRow();
	hb.startTableRow();
	hb.startTableCell();

    // bills & deposits
    bool isHeaderAdded = false;

    /////////////////////////////////////
    std::vector<mmBDTransactionHolder> trans_;

    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * from BILLSDEPOSITS_V1");

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
        mmex::formatDoubleToCurrencyEdit(th.toAmt_, th.transToAmtString_);

        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

        if (th.transType_ == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_,  th.accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  th.toAccountID_ );

            th.payeeStr_ = toAccount;
        }
	
        trans_.push_back(th);
    }
    q1.Finalize();
    std::sort(trans_.begin(), trans_.end(), sortTransactionsByRemainingDaysHP);

    ////////////////////////////////////
    std::vector<wxString> data4;
    for (size_t bdidx = 0; bdidx < trans_.size(); ++bdidx)
    {
        data4.clear();
        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = trans_[bdidx].nextOccurDate_.Subtract(today);
        //int hoursRemaining_ = ts.GetHours();

        if (trans_[bdidx].daysRemaining_ <= 14)
        {
            if (!isHeaderAdded)
            {
				hb.startTable(wxT("95%"));
				hb.addTableHeaderRowLink(wxT("billsdeposits"), _("Upcoming Transactions"), 3);
                isHeaderAdded = true;
            }            
            
            wxString daysRemainingStr_;
            wxString colorStr = wxT("#9999FF");
           		
            daysRemainingStr_ = trans_[bdidx].daysRemainingStr_;
            if (trans_[bdidx].daysRemaining_ > 0)
            {
            }
            else 
            {
                colorStr = wxT("#FF6600");
            }

            // Load the currency for this BD
            boost::weak_ptr<mmCurrency> wpCurrency = core_->accountList_.getCurrencyWeakPtr(trans_[bdidx].accountID_);
            boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
            wxASSERT(pCurrency);
            if (pCurrency)
            {
                 pCurrency->loadCurrencySettings();
            }

			wxString displayBDAmtString;
			mmex::formatDoubleToCurrency(trans_[bdidx].amt_, displayBDAmtString);
           
			hb.startTableRow();
			hb.addTableCell(trans_[bdidx].payeeStr_, false, true);
			hb.addTableCell(displayBDAmtString, true);
			hb.addTableCell(wxT("&nbsp;&nbsp;&nbsp;&nbsp;") + daysRemainingStr_, false, false, false, colorStr);
			hb.endTableRow();
        }
    }

    if (isHeaderAdded)
        hb.endTable();

	hb.endTableCell();
	hb.startTableCell();

    mmex::CurrencyFormatter::instance().loadDefaultSettings();

    mmDBWrapper::loadBaseCurrencySettings(db_);
    
        static const char sql30[] = 
        "select "
        "CATEG|| (Case SUBCATEG when '' then '' else ':' end )||  SUBCATEG as SUBCATEGORY "
        ", AMOUNT "
        "from ( "
        "select coalesce(CAT.CATEGNAME, SCAT.CATEGNAME) as CATEG, "
        "coalesce(SUBCAT.SUBCATEGNAME, SSCAT.SUBCATEGNAME,'') as SUBCATEG, "
        "SUM(ROUND((case CANS.TRANSCODE when 'Withdrawal' then -1 else 1 end) "
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
        "where  CANS.TRANSCODE<>'Transfer' and CANS.TRANSDATE > date('now', '-1 month') and CANS.TRANSDATE <= date('now') "
        "group by CATEG, SUBCATEG "
        "order by AMOUNT "
        "limit 10 "
        ") where AMOUNT < 0" ;

    wxSQLite3ResultSet q30 = db_->ExecuteQuery(sql30);

        //if (db_)
 
    std::vector<CategInfo> categList;
	hb.startTable(wxT("95%"));
	hb.addTableHeaderRow(_("Top Categories Last 30 Days"), 2);

        while(q30.NextRow())
        {
            int categBalance = q30.GetInt(wxT("AMOUNT"));
            wxString subcategString = q30.GetString(wxT("SUBCATEGORY"));

            wxString categBalanceStr;
            mmex::formatDoubleToCurrency(categBalance, categBalanceStr);

		hb.startTableRow();
		hb.addTableCell(subcategString, false, true);
		hb.addTableCell(categBalanceStr, true);
		hb.endTableRow();
       
       }
    q30.Finalize();
 
    hb.endTable();

    hb.getHTMLText();

/*
    // Commented because there is not enough vertical space to show main page
    // without vertical scrollbar on 19-20" monitors.

    // Top 10 Graph.
    mmGraphTopCategories gtp;
    hb.addImage(gtp.getOutputFileName());
*/

    hb.addLineBreak();
    int countFollowUp = core_->bTransactionList_.countFollowupTransactions();

    if (countFollowUp > 0)
    {
        wxString str = wxT("<br><i>");
        str << _("Follow Up On Transactions: ") << wxString::Format(wxT("<b>%d</b> "), countFollowUp) << wxT("</i><br>");
        hb.addHTML(str);
    }

    wxString tup = wxT("<i>");
    tup << _("Total Transactions: ") << wxString::Format(wxT("<b>%d</b> "), 
        core_->bTransactionList_.transactions_.size()) << wxT("</i>");
    hb.addHTML(tup);

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

    htmlWindow_ = new mmHtmlWindow( itemDialog1, frame_, 
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
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_GOTOACCOUNT);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    else if (href == wxT("Assets"))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ASSETS);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    else if (href == wxT("Stocks"))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_STOCKS);
        frame_->GetEventHandler()->AddPendingEvent(evt);
    }
    // Deselect any selections in the tree ctrl, so tree works
    frame_->unselectNavTree();
}

//----------------------------------------------------------------------------

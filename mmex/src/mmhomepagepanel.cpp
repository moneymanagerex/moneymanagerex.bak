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
#include "mmgraphtopcategories.h"


namespace
{

inline boost::shared_ptr<mmCurrency> getCurrency(mmCoreDB* core, int currencyID)
{
    wxASSERT(core);
    return core->currencyList_.getCurrencySharedPtr(currencyID);
}

} // namespace


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

    typedef std::map<boost::shared_ptr<mmCurrency>, double> balances_t;
    balances_t tBalances;

    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));

    for (int iAdx = 0; iAdx < (int) core_->accountList_.accounts_.size(); iAdx++)
    {
        mmCheckingAccount* pCA 
           = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
        if (pCA && pCA->status_== mmAccount::MMEX_Open)
        {
          
           boost::shared_ptr<mmCurrency> pCurrencyPtr 
			   = core_->accountList_.getCurrencyWeakPtr(pCA->accountID_).lock();
           wxASSERT(pCurrencyPtr);
           mmCurrencyFormatter::loadSettings(pCurrencyPtr);

           double bal = pCA->initialBalance_ 
              + core_->bTransactionList_.getBalance(pCA->accountID_);
           double rate = pCurrencyPtr->baseConv_;

           // show the actual amount in that account in the original rate
           tBalances[pCurrencyPtr] += bal * rate;
           wxString balance;
           mmCurrencyFormatter::formatDoubleToCurrency(bal, balance);

           double income = 0.0, expenses = 0.0;
           core_->bTransactionList_.getExpensesIncome(pCA->accountID_, expenses, income, 
              false,dtBegin, dtEnd);

           if ((vAccts == wxT("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
               (vAccts == wxT("Favorites") && pCA->favoriteAcct_) ||
               (vAccts == wxT("ALL")))
           {
               hb.startTableRow();
               hb.addTableCellLink(wxT("ACCT:") + wxString::Format(wxT("%d"), pCA->accountID_), pCA->accountName_, false, true);
               hb.addTableCell(balance, true);
               hb.endTableRow();
           }

           tincome += income;
           texpenses += expenses;
        }

	}

    if (mmIniOptions::enableStocks_)
    {
        /* Stocks */
        double invested;
        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_, invested);
        wxString stockBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(stockBalance, stockBalanceStr);

		hb.addRowSeparator(2);
		hb.startTableRow();
		hb.addTableCellLink(wxT("Stocks"), _("Stock Investments"), false, true);
		hb.addTableCell(stockBalanceStr, true);
		hb.endTableRow();

	int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
	tBalances[getCurrency(core_, currencyID)] += stockBalance;
    }

    if (mmIniOptions::enableAssets_)
    {
        /* Assets */
        double assetBalance = mmDBWrapper::getAssetBalance(db_);
        wxString assetBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(assetBalance, assetBalanceStr);

		hb.addRowSeparator(2);
		hb.startTableRow();
		hb.addTableCellLink(wxT("Assets"), _("Assets"), false, true);
		hb.addTableCell(assetBalanceStr, true);
		hb.endTableRow();

	int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
	tBalances[getCurrency(core_, currencyID)] += assetBalance;
    }
    
    hb.addRowSeparator(2);

    if (tBalances.size() > 1) // print total balance for every currency
    {
	hb.startTableRow();
	hb.addTableHeaderCell(_("Currency"));
	hb.addTableHeaderCell(_("Summary"));
	hb.endTableRow();
		
	for (balances_t::const_iterator i = tBalances.begin(); i != tBalances.end(); ++i)
	{
		wxString tBalanceStr;
		mmCurrencyFormatter::loadSettings(i->first);
		mmCurrencyFormatter::formatDoubleToCurrency(i->second, tBalanceStr);
			
		hb.startTableRow();
		hb.addTableCell(i->first->currencyName_);
		hb.addTableCell(tBalanceStr, true);
		hb.endTableRow();
	}
    } 
    else if (!tBalances.empty())
    {
	balances_t::const_iterator it = tBalances.begin();
 
	wxString tBalanceStr;
	mmCurrencyFormatter::loadSettings(it->first);
	mmCurrencyFormatter::formatDoubleToCurrency(it->second, tBalanceStr);

	hb.addTotalRow(_("Total of Accounts:"), 2, tBalanceStr);
    }

    tBalances.clear();

    hb.endTable();

	hb.endTableCell();
	hb.startTableCell();

	hb.startTable(wxT("95%"));

    wxString incStr, expStr;
    mmCurrencyFormatter::formatDoubleToCurrency(tincome, incStr);
    mmCurrencyFormatter::formatDoubleToCurrency(texpenses, expStr);

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
    hb.addImage(gg.GetOutputFileName());

	hb.endTableCell();
	hb.endTableRow();
	hb.startTableRow();
	hb.startTableCell();

    // bills & deposits
    bool isHeaderAdded = false;

    /////////////////////////////////////
    std::vector<mmBDTransactionHolder> trans_;
    wxString bufSQLStr = wxString::Format(wxT("select * from BILLSDEPOSITS_V1")); 
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQLStr);

    int ct = 0;
    while (q1.NextRow())
    {
        mmBDTransactionHolder th;

        th.bdID_           = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));
        th.nextOccurStr_   = mmGetDateForDisplay(db_, th.nextOccurDate_);
        int repeats        = q1.GetInt(wxT("REPEATS"));

        if (repeats == 0)
        {
            th.repeatsStr_ = _("None");
        }
        else if (repeats == 1)
        {
           th.repeatsStr_ = _("Weekly");
        }
        else if (repeats == 2)
        {
            th.repeatsStr_ = _("Bi-Weekly");
        }
        else if (repeats == 3)
        {
            th.repeatsStr_ = _("Monthly");
        }
        else if (repeats == 4)
        {
            th.repeatsStr_ = _("Bi-Monthly");
        }
        else if (repeats == 5)
        {
            th.repeatsStr_ = _("Quarterly");
        }
        else if (repeats == 6)
        {
             th.repeatsStr_ = _("Half-Yearly");
        }
        else if (repeats == 7)
        {
           th.repeatsStr_ = _("Yearly");
        }
        else if (repeats == 8)
        {
           th.repeatsStr_ = _("Four Months");
        }
        else if (repeats == 9)
        {
            th.repeatsStr_ = _("Four Weeks");
        }

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

        wxString displayTransAmtString;
        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.amt_, displayTransAmtString))
            th.transAmtString_ = displayTransAmtString;

        wxString displayToTransAmtString;
        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.toAmt_, displayToTransAmtString))
            th.transToAmtString_ = displayToTransAmtString;

        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

        if (th.transType_ == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_,  th.accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  th.toAccountID_ );

            th.payeeStr_ = toAccount;
        }
	
		//th.payeeStr_ += wxT("[");
		//th.payeeStr_ += displayBDAmtString;
		//th.payeeStr_ += wxT("]");

        trans_.push_back(th);
        ct++;
    }
    q1.Finalize();
    std::sort(trans_.begin(), trans_.end(), sortTransactionsByRemainingDaysHP);

    ////////////////////////////////////
    std::vector<wxString> data4;
    for (unsigned int bdidx = 0; bdidx < trans_.size(); bdidx++)
    {
        data4.clear();
        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = trans_[bdidx].nextOccurDate_.Subtract(today);
        int hoursRemaining_ = ts.GetHours();

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
			mmCurrencyFormatter::formatDoubleToCurrency(trans_[bdidx].amt_, displayBDAmtString);
           
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

    mmCurrencyFormatter::loadDefaultSettings();
	//--------------------------------------------------------
    // update category List
    topCategories_ = frame_->createCategoryList();
    hb.addHTML(topCategories_);
#if 0
   // ... Doesn't work under negative values, currently
   mmGraphTopCategories gtp;
   hb.addImage(gtp.GetOutputFileName());
#endif

	//--------------------------------------------------------
    hb.addLineBreak();
    int countFollowUp = core_->bTransactionList_.countFollowupTransactions();

    if (countFollowUp > 0)
    {
        wxString fup = _("Follow Up On ");
        wxString fullStr = wxT("<i>") + fup;
        wxString str = wxString::Format(wxT("<b>%d</b> "), countFollowUp);
        str = fullStr + str;
        str += _("Transactions");
        str += wxT("</i>");
        hb.addHTML(str);
    }

    wxString tup = _("Total Transactions");
    wxString tullStr = tup + wxT("</i>");
    wxString tstr = wxString::Format(wxT("<br><i><b>%d</b> "), 
        core_->bTransactionList_.transactions_.size());
    tstr = tstr + tullStr;
    hb.addHTML(tstr);

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
        frame_->gotoAccountID_ = id;
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
    frame_->navTreeCtrl_->Unselect();
}




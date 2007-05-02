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

#include "mmhomepagepanel.h"
#include "mmex.h"
#include "util.h"

#include "transdialog.h"
#include "newacctdialog.h"
#include "htmlbuilder.h"
#include "dbwrapper.h"
#include "billsdepositspanel.h"

BEGIN_EVENT_TABLE( mmHomePagePanel, wxPanel )
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mmHtmlWindow, wxHtmlWindow)
END_EVENT_TABLE()



mmHomePagePanel::mmHomePagePanel(mmGUIFrame* frame, 
            wxSQLite3Database* db, 
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

    hb.addHTML(wxT("<table border=\"0\"><tr><td VALIGN=\"top\">"));

    std::vector<wxString> data;

    wxString str1 = _("Account") + wxString(wxT("</b></th><th align=\"right\" width=\"100\" ><b>"));
    wxString str2 = _("Summary") + wxString(wxT("</b></th></tr>"));
    hb.addHTML(wxT("<table cellspacing=\"0\" cellpadding=\"1\" border=\"0\"><tr  bgcolor=\"#80B9E8\"><th align=\"left\" width=\"200\"><b>") 
        +  str1 + str2 );

    /////////////////   
   
    int ct = 0;  
    double tincome = 0.0;
    double texpenses = 0.0;
    double tBalance = 0.0;

    for (int iAdx = 0; iAdx < (int) core_->accountList_.accounts_.size(); iAdx++)
    {
        mmCheckingAccount* pCA 
           = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
        if (pCA && pCA->status_== mmAccount::MMEX_Open)
        {
           std::vector<wxString> data1;
           hb.addHTML(wxT("<tr><td><a href=\"ACCT: "));
           hb.addHTML(wxString::Format(wxT("%d"), pCA->accountID_));
           hb.addHTML(wxT("\" >"));
           hb.addHTML(pCA->accountName_);
           hb.addHTML(wxT("</a></td><td align=\"right\">"));

           boost::shared_ptr<mmCurrency> pCurrencyPtr 
			   = core_->accountList_.getCurrencyWeakPtr(pCA->accountID_).lock();
           wxASSERT(pCurrencyPtr);
           mmCurrencyFormatter::loadSettings(pCurrencyPtr);

           double bal = pCA->initialBalance_ 
              + core_->bTransactionList_.getBalance(pCA->accountID_);
           double rate = pCurrencyPtr->baseConv_;
           // show the actual amount in that account in the original rate
           tBalance += bal * rate;
           wxString balance;
           mmCurrencyFormatter::formatDoubleToCurrency(bal, balance);
           hb.addHTML(balance);
           hb.addHTML(wxT("</td></tr>"));
           //data1.push_back(balance);

           //hb.addRow(data1, wxT("align=\"left\" "));
           double income = 0.0, expenses = 0.0;
           core_->bTransactionList_.getExpensesIncome(pCA->accountID_, expenses, income, 
              false,dtBegin, dtEnd);

           tincome += income;
           texpenses += expenses;
           ct++;
        }
    }

    /* Stocks */
    double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_);
    wxString stockBalanceStr;
    mmDBWrapper::loadBaseCurrencySettings(db_);
    mmCurrencyFormatter::formatDoubleToCurrency(stockBalance, stockBalanceStr);

    data.clear();
    hb.addHTML(wxT("<tr bgcolor=\"#D3EFF6\" ><td><a href=\"Stocks\">"));
    hb.addHTML(_("Stock Investments"));
    hb.addHTML(wxT("</a></td><td align=\"right\">"));
    hb.addHTML(stockBalanceStr);
    hb.addHTML(wxT("</td></tr>"));

    tBalance += stockBalance;

    /* Assets */
    double assetBalance = mmDBWrapper::getAssetBalance(db_);
    wxString assetBalanceStr;
    mmDBWrapper::loadBaseCurrencySettings(db_);
    mmCurrencyFormatter::formatDoubleToCurrency(assetBalance, assetBalanceStr);

    data.clear();
    hb.addHTML(wxT("<tr bgcolor=\"#D3EFF4\" ><td><a href=\"Assets\">"));
    hb.addHTML(_("Assets"));
    hb.addHTML(wxT("</a></td><td align=\"right\">"));
    hb.addHTML(assetBalanceStr);
    hb.addHTML(wxT("</td></tr>"));

    tBalance += assetBalance;
    
    wxString tBalanceStr;
    mmCurrencyFormatter::formatDoubleToCurrency(tBalance, tBalanceStr);


    hb.addHTML(wxT("<tr bgcolor=\"#DCEDD5\" ><td>") + wxString(_("Total of Accounts :")) 
       + wxString(wxT("</td><td align=\"right\"><b>")));
    hb.addHTML(tBalanceStr);
    hb.addHTML(wxT("</b></td></tr>"));
    hb.endTable();

    hb.addHTML(wxT("</td><td >&nbsp;</td><td width=\"200\" ALIGN=\"left\" VALIGN=\"top\">"));
    //hb.beginTable();

    hb.addHTML(wxT("<table cellspacing=\"0\" cellpadding=\"1\" border=\"0\">"));
    wxString incStr, expStr;
    mmCurrencyFormatter::formatDoubleToCurrency(tincome, incStr);
    mmCurrencyFormatter::formatDoubleToCurrency(texpenses, expStr);

    wxString baseInc = _("Income");
    wxString baseExp = _("Expense");
    wxString income = baseInc + incStr;
    wxString expense = baseExp + expStr;

    std::vector<wxString> data2;
    data2.push_back(_("Income vs Expenses: Current Month"));
    hb.addTableHeaderRow(data2, wxT(" BGCOLOR=\"#80B9E8\" "), 
       wxT(" nowrap width=\"130\" COLSPAN=\"2\" "));
    
    hb.addHTML(wxT("<tr><td>") + baseInc + wxString(wxT("</td><td align=\"right\">")));
    hb.addHTML(incStr);
    hb.addHTML(wxT("</td></tr>"));
    
    hb.addHTML(wxT("<tr><td>") + baseExp + wxString(wxT("</td><td align=\"right\">")));
    hb.addHTML(expStr);
    hb.addHTML(wxT("</td></tr>"));
    
    hb.endTable();

    hb.addHTML(wxT("<BR> <BR>"));

    // bills & deposits
    bool isHeaderAdded = false;

    /////////////////////////////////////
    std::vector<mmBDTransactionHolder> trans_;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from BILLSDEPOSITS_V1;");
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);

    ct = 0;
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
                hb.addHTML(wxT("<table cellspacing=\"0\" cellpadding=\"1\" border=\"0\">"));

                std::vector<wxString> data3;
                data3.push_back(_("Upcoming Transactions"));

                hb.addHTML(wxT("<tr BGCOLOR=\"#80B9E8\" > <th width=\"130\" COLSPAN=\"3\" > <b>"));
                hb.addHTML(wxT("<a href=\"billsdeposits\" >"));
                hb.addHTML(data3[0]);
                hb.addHTML(wxT("</a></b></th></tr>"));
                isHeaderAdded = true;
            }

            
            
            wxString daysRemainingStr_;
            wxString colorStr = wxT("\"#9999FF\" ");
           		
            daysRemainingStr_ = trans_[bdidx].daysRemainingStr_;
            if (trans_[bdidx].daysRemaining_ > 0)
            {
            }
            else 
            {
                colorStr = wxT("\"#FF6600\" ");
            }

            // Load the currency for this BD
            boost::weak_ptr<mmCurrency> wpCurrency = core_->accountList_.getCurrencyWeakPtr(trans_[bdidx].accountID_);
            boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
            wxASSERT(pCurrency);
            if (pCurrency)
            {
                 mmCurrencyFormatter::loadSettings(pCurrency);
            }

			wxString displayBDAmtString;
			mmCurrencyFormatter::formatDoubleToCurrency(trans_[bdidx].amt_, displayBDAmtString);
           
	
			hb.addHTML(wxT("<tr> <font size=\"-2\" ><td width=\"100\"> "));
			hb.addHTML(trans_[bdidx].payeeStr_);
			hb.addHTML(wxT("</td><td width=\"50\" align=\"right\">"));
			hb.addHTML(displayBDAmtString);
			hb.addHTML(wxT("</td><td width=\"100\" align=\"right\" >"));
            hb.addHTML(wxT("<font color="));
			hb.addHTML(colorStr);
			hb.addHTML(wxT(" > "));
			hb.addHTML(daysRemainingStr_);
            hb.addHTML(wxT("</font>"));
			hb.addHTML(wxT("</td>"));

			hb.addHTML(wxT("</font></tr>"));

            //hb.addRow(data4, wxT(" "), wxT("WIDTH=\"130\"  ") + colorStr);
        }
    }

    if (isHeaderAdded)
        hb.endTable();

    mmCurrencyFormatter::loadDefaultSettings();
	//--------------------------------------------------------

    hb.addHTML(topCategories_);

	//--------------------------------------------------------
	hb.addHTML(wxT("<br><br>"));

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
    wxString tullStr = tup + wxT("<i>");
    wxString tstr = wxString::Format(wxT("<br><i><b>%d</b> "), 
        core_->bTransactionList_.transactions_.size());
    tstr = tstr + tullStr;
    hb.addHTML(tstr);

    hb.endTable();

    hb.addHTML(wxT(" </td></tr></table>"));
    
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


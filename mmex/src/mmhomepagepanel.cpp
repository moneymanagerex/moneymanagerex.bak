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
#include "newchkacctdialog.h"
#include "htmlbuilder.h"
#include "dbwrapper.h"

BEGIN_EVENT_TABLE( mmHomePagePanel, wxPanel )
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mmHtmlWindow, wxHtmlWindow)
END_EVENT_TABLE()

mmHomePagePanel::mmHomePagePanel(mmGUIFrame* frame, 
            wxSQLite3Database* db, 
            mmCoreDB* core, 
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

    wxString str1 = _("Account") + wxString(wxT("</b></th><th   width=\"100\" ><b>"));
    wxString str2 = _("Summary") + wxString(wxT("</b></th></tr>"));
    hb.addHTML(wxT("<table border=\"1\"><tr  bgcolor=\"#80B9E8\"><th width=\"200\"><b>") 
        +  str1 + str2 );

    /////////////////   
   
    int ct = 0;  
    double tincome = 0.0;
    double texpenses = 0.0;
    double tBalance = 0.0;

    for (int iAdx = 0; iAdx < core_->accountList_.accounts_.size(); iAdx++)
    {
        mmCheckingAccount* pCA = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
        if (pCA)
        {
        std::vector<wxString> data1;
        hb.addHTML(wxT("<tr> <td> <a href=\"ACCT: "));
        hb.addHTML(wxString::Format(wxT("%d"), pCA->accountID_));
        hb.addHTML(wxT("\" >"));
        hb.addHTML(pCA->accountName_);
        hb.addHTML(wxT(" </a></td><td align=\"right\">"));
        
        double bal = pCA->balance();

        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", pCA->accountID_);
        wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL);
        if (q2.NextRow())
        {
            int currencyID = q2.GetInt(wxT("CURRENCYID"));
            mmDBWrapper::loadSettings(db_, currencyID);

        }
        q2.Finalize();

        double rate = mmDBWrapper::getCurrencyBaseConvRate(db_,pCA->accountID_);
        // show the actual amount in that account in the original rate
        tBalance += bal * rate;
        wxString balance;
        mmCurrencyFormatter::formatDoubleToCurrency(bal, balance);
        hb.addHTML(balance);
        hb.addHTML(wxT(" </td></tr>"));
        //data1.push_back(balance);
        
        //hb.addRow(data1, wxT("align=\"left\" "));
        double income = 0.0, expenses = 0.0;
        mmDBWrapper::getExpensesIncome(db_, pCA->accountID_, expenses, income, 
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
    //data.push_back(_("Assets :"));
    //data.push_back(assetBalanceStr);
    //hb.addRow(data, wxT(" bgcolor=\"#D3EFF4\" "));
    hb.addHTML(wxT("<tr bgcolor=\"#D3EFF4\" ><td><a href=\"Assets\">"));
    hb.addHTML(_("Assets"));
    hb.addHTML(wxT("</a></td><td align=\"right\">"));
    hb.addHTML(assetBalanceStr);
    hb.addHTML(wxT("</td></tr>"));

    tBalance += assetBalance;
    
    wxString tBalanceStr;
    mmCurrencyFormatter::formatDoubleToCurrency(tBalance, tBalanceStr);


    hb.addHTML(wxT("<tr bgcolor=\"#DCEDD5\" ><td>") + wxString(_("Total of Accounts :")) + wxString(wxT("</td><td align=\"right\"><b>")));
    hb.addHTML(tBalanceStr);
    hb.addHTML(wxT("</b></td></tr>"));
    hb.endTable();

    hb.addHTML(wxT("</td><td >&nbsp;</td><td width=\"200\" ALIGN=\"left\" VALIGN=\"top\">"));
    hb.beginTable();

    hb.beginTable();
    wxString incStr, expStr;
    mmCurrencyFormatter::formatDoubleToCurrency(tincome, incStr);
    mmCurrencyFormatter::formatDoubleToCurrency(texpenses, expStr);

    wxString baseInc = _("Income");
    wxString baseExp = _("Expense");
    wxString income = baseInc + incStr;
    wxString expense = baseExp + expStr;

    std::vector<wxString> data2;
    data2.push_back(_("Income vs Expenses: Current Month"));
    hb.addTableHeaderRow(data2, wxT(" BGCOLOR=\"#80B9E8\" "), wxT(" width=\"100\" COLSPAN=\"2\" "));
    
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

 
    wxSQLite3ResultSet q2 = db_->ExecuteQuery("select * from BILLSDEPOSITS_V1;");

    while (q2.NextRow())
    {
        std::vector<wxString> data4;
        wxDateTime nextOccurDate_  = mmGetStorageStringAsDate(q2.GetString(wxT("NEXTOCCURRENCEDATE")));
        wxString nextOccurStr_   = mmGetDateForDisplay(db_, nextOccurDate_);

        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = nextOccurDate_.Subtract(today);
        int hoursRemaining_ = ts.GetHours();
        int daysRemaining_ = ts.GetDays();
      

        int payeeID_        = q2.GetInt(wxT("PAYEEID"));
        wxString transType_      = q2.GetString(wxT("TRANSCODE"));
        int accountID_      = q2.GetInt(wxT("ACCOUNTID"));
        int toAccountID_    = q2.GetInt(wxT("TOACCOUNTID"));

        int cid = 0, sid = 0;
        wxString payeeStr_ = mmDBWrapper::getPayee(db_, payeeID_, cid, sid);

        if (transType_ == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_,  accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  toAccountID_ );

            payeeStr_ = toAccount;
        }
        
        if (daysRemaining_ <= 14)
        {
            if (!isHeaderAdded)
            {
                hb.beginTable();
                std::vector<wxString> data3;
                data3.push_back(_("Upcoming Transactions"));

                hb.addHTML(wxT("<tr BGCOLOR=\"#80B9E8\" > <th width=\"100\" COLSPAN=\"2\" > <b>"));
                hb.addHTML(wxT("<a href=\"billsdeposits\" >"));
                hb.addHTML(data3[0]);
                hb.addHTML(wxT("</a></b></th></tr>"));
                isHeaderAdded = true;
            }

            data4.push_back(payeeStr_);

            wxString daysRemainingStr_;
            wxString colorStr = wxT(" BGCOLOR=\"#FFFFCC\" ");
           
            if ((daysRemaining_ == 0) && (hoursRemaining_ > 0))
            {
                daysRemaining_ = 1;
            }
            else if ((daysRemaining_ == 0) && (hoursRemaining_ <= 0))
            {
                daysRemaining_ = 0;
            }

            if (daysRemaining_ > 0)
            {
                daysRemainingStr_ = wxString::Format(wxT("%d"), daysRemaining_) + 
                    _(" days remaining");
            }
            else 
            {
                daysRemainingStr_ = wxString::Format(wxT("%d"), abs(daysRemaining_)) + 
                    _(" days overdue!");
                colorStr = wxT(" BGCOLOR=\"#FF6600\" ");
            }
            
            data4.push_back(daysRemainingStr_);
            hb.addRow(data4, wxT(""), wxT("WIDTH=\"130\"  ") + colorStr);
        }
    }
    q2.Finalize();

    if (isHeaderAdded)
        hb.endTable();

    
    
    q2 = db_->ExecuteQuery("select * from CHECKINGACCOUNT_V1 where STATUS='F';");
    int countFollowUp = 0;
    while (q2.NextRow())
    {   
        countFollowUp++;
    }
    if (countFollowUp > 0)
    {
        wxString fup = _("Follow Up On ");
        wxString fullStr = wxT("<br><i>") + fup;
        wxString str = wxString::Format(wxT("<b>%d</b> Transactions</i>"), countFollowUp);
        str = fullStr + str;
        hb.addHTML(str);
    }

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
}


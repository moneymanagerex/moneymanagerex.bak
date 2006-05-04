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
#ifndef _MM_EX_REPORTSUMMARY_H_
#define _MM_EX_REPORTSUMMARY_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

class mmReportSummary : public mmPrintableBase 
{
public:
    mmReportSummary(wxSQLite3Database* db) : db_(db) {}

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Summary of Accounts"));

        wxDateTime now = wxDateTime::Now();
        wxString dt(wxDateTime::GetWeekDayName(now.GetWeekDay()) + wxString(wxT(", ")));
        dt += wxDateTime::GetMonthName(now.GetMonth()) + wxString(wxT(" "));
        dt += wxString::Format(wxT("%d"), now.GetDay()) + wxT(", ") 
            + wxString::Format(wxT("%d"), now.GetYear());
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();
        hb.addLineBreak();

        mmDBWrapper::loadBaseCurrencySettings(db_);
        int ct = 0;  
        double tincome = 0.0;
        double texpenses = 0.0;
        double tBalance = 0.0;
        mmBEGINSQL_LITE_EXCEPTION;
        wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * from ACCOUNTLIST_V1 where ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
        hb.beginTable();

        std::vector<wxString> headerR;
        headerR.push_back(_("Account Name  "));
        headerR.push_back(_("Balance   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        while (q1.NextRow())
        {
            double bal = mmDBWrapper::getTotalBalanceOnAccount(db_, q1.GetInt(wxT("ACCOUNTID")), true);

            wxSQLite3StatementBuffer bufSQL;
            bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", q1.GetInt(wxT("ACCOUNTID")));
            wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL);
            if (q2.NextRow())
            {
                int currencyID = q2.GetInt(wxT("CURRENCYID"));
                //mmDBWrapper::loadSettings(db_, currencyID);
            }
            q2.Finalize();

            double rate = mmDBWrapper::getCurrencyBaseConvRate(db_, q1.GetInt(wxT("ACCOUNTID")));
            bal = bal * rate;
            tBalance += bal;

            wxString balance;
            mmCurrencyFormatter::formatDoubleToCurrency(bal, balance);

            std::vector<wxString> row;
            row.push_back(q1.GetString(wxT("ACCOUNTNAME")));
            row.push_back(balance);
            hb.addRow(row);
        }
        hb.endTable();
        mmENDSQL_LITE_EXCEPTION

        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_);
        wxString stockBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(stockBalance, stockBalanceStr);
        hb.addLineBreak();
        hb.addLineBreak();
        wxString dispStr = _("Stock Investments :") + stockBalanceStr; 
        hb.addHeader(7, dispStr);
        hb.addLineBreak();

        tBalance += stockBalance;
        wxString tBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(tBalance, tBalanceStr);
        dispStr = _("Total Balance on all Accounts :") + tBalanceStr; 
        hb.addHeader(7, dispStr);

        hb.end();
        return hb.getHTMLText();

    }

private:
    wxSQLite3Database* db_;


};

#endif

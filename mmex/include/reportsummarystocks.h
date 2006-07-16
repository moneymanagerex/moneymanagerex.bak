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
#ifndef _MM_EX_REPORTSUMMARYSTOCKS_H_
#define _MM_EX_REPORTSUMMARYSTOCKS_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "stocks.h"

class mmReportSummaryStocks : public mmPrintableBase 
{
public:
    mmReportSummaryStocks(wxSQLite3Database* db) : db_(db) {}

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Summary of Stocks"));

        wxDateTime now = wxDateTime::Now();
        wxString dt(wxDateTime::GetWeekDayName(now.GetWeekDay()) + wxString(wxT(", ")));
        dt += wxDateTime::GetMonthName(now.GetMonth()) + wxString(wxT(" "));
        dt += wxString::Format(wxT("%d"), now.GetDay()) + wxT(", ") 
            + wxString::Format(wxT("%d"), now.GetYear());
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        hb.addHTML(wxT("<font size=\"-2\">"));

        mmDBWrapper::loadBaseCurrencySettings(db_);
        int ct = 0;  
        double tincome = 0.0;
        double texpenses = 0.0;
        double tBalance = 0.0;
        mmBEGINSQL_LITE_EXCEPTION;
        
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from STOCK_V1;");
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);

        hb.beginTable();

        std::vector<wxString> headerR;
        headerR.push_back(_("Company  "));
        headerR.push_back(_("Symbol   "));
        headerR.push_back(_("Number Shares  "));
        headerR.push_back(_("Purchase Date "));
        headerR.push_back(_("Purchase Price "));
        headerR.push_back(_("Current Price  "));
        headerR.push_back(_("Commission  "));
        headerR.push_back(_("Value  "));
        headerR.push_back(_("Gain Loss  "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        int ct = 0;
        while (q1.NextRow())
        {
            mmStockTransactionHolder th;

            th.stockID_           = q1.GetInt(wxT("STOCKID"));
            int accountID         = q1.GetInt(wxT("HELDAT"));
            th.heldAt_            = mmDBWrapper::getAccountName(db_, accountID);
            th.shareName_         = q1.GetString(wxT("STOCKNAME"));
            th.numSharesStr_      = q1.GetString(wxT("NUMSHARES"));
            th.numShares_         = q1.GetDouble(wxT("NUMSHARES"));
            th.symbol_            = q1.GetString(wxT("SYMBOL"));

            th.currentPrice_      = q1.GetDouble(wxT("CURRENTPRICE"));
            th.purchasePrice_      = q1.GetDouble(wxT("PURCHASEPRICE"));
            th.valueStr_          = q1.GetString(wxT("VALUE"));
            th.value_             = q1.GetDouble(wxT("VALUE"));
            double commission     = q1.GetDouble(wxT("COMMISSION"));
            wxString dateString = q1.GetString(wxT("PURCHASEDATE"));
            wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
            wxString dt = mmGetDateForDisplay(db_, dtdt);
            

            th.gainLoss_        = th.value_ - ((th.numShares_ * th.purchasePrice_) + commission);

            wxString tempString;
            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.gainLoss_, tempString))
                th.gainLossStr_ = tempString;

            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.currentPrice_, tempString))
                th.cPriceStr_ = tempString;

            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.purchasePrice_, tempString))
                th.pPriceStr_ = tempString;

            wxString commString;
            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(commission, tempString))
                commString = tempString;

            std::vector<wxString> data;
            data.push_back(th.shareName_);
            data.push_back(th.symbol_);
            data.push_back(th.numSharesStr_);
            data.push_back(dt);
            data.push_back(th.pPriceStr_);
            data.push_back(th.cPriceStr_);
            data.push_back(commString);
            data.push_back(th.valueStr_);
            data.push_back(th.gainLossStr_);
            hb.addRow(data);
        }
        q1.Finalize();
        
        hb.endTable();
        mmENDSQL_LITE_EXCEPTION

        /* Stocks */
        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_);
        wxString stockBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(stockBalance, stockBalanceStr);
        hb.endTable();
        hb.addHTML(wxT("</font>"));
        hb.addLineBreak();
        hb.addLineBreak();
        
        wxString dispStr = _("Total Stock Investments :") + stockBalanceStr; 
        hb.addHeader(7, dispStr);
        

        hb.end();
        return hb.getHTMLText();
    }

private:
    wxSQLite3Database* db_;


};

#endif

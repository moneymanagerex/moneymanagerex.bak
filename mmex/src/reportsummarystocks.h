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

#ifndef _MM_EX_REPORTSUMMARYSTOCKS_H_
#define _MM_EX_REPORTSUMMARYSTOCKS_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "stockspanel.h"

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
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        hb.startCenter();

        mmDBWrapper::loadBaseCurrencySettings(db_);

        hb.startTable();
		hb.startTableRow();
		hb.addTableHeaderCell(_("Company"));
		hb.addTableHeaderCell(_("Symbol"));
		hb.addTableHeaderCell(_("Number Shares"));
		hb.addTableHeaderCell(_("Purchase Date"));
		hb.addTableHeaderCell(_("Purchase Price"));
		hb.addTableHeaderCell(_("Current Price"));
		hb.addTableHeaderCell(_("Commission"));
		hb.addTableHeaderCell(_("Value"));
		hb.addTableHeaderCell(_("Gain Loss"));
		hb.endTableRow();

        wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * from STOCK_V1");

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
            wxString dateString   = q1.GetString(wxT("PURCHASEDATE"));
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

			hb.startTableRow();
			hb.addTableCell(th.shareName_, false, true);
			hb.addTableCell(th.symbol_);
            hb.addTableCell(th.numSharesStr_, true);
            hb.addTableCell(dt);
            hb.addTableCell(th.pPriceStr_, true);
            hb.addTableCell(th.cPriceStr_, true);
            hb.addTableCell(commString, true);
            hb.addTableCell(th.valueStr_, true);

			if(th.gainLoss_ < 0)
			{
				hb.addTableCell(th.gainLossStr_, true, true, true, wxT("#ff0000"));
			}
			else if(th.gainLoss_ > 0)
			{
				hb.addTableCell(th.gainLossStr_, true, false, true);
			}
			else
			{
				hb.addTableCell(th.gainLossStr_, true, true);
			}
            
            hb.endTableRow();
        }
        q1.Finalize();
        
        /* Stocks */
        double invested = 0;
        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_, invested);
        wxString stockBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(stockBalance, stockBalanceStr);

		hb.addRowSeparator(9);
		hb.addTotalRow(_("Total Stock Investments: "), 9, stockBalanceStr);
        hb.endTable();

		hb.endCenter();

        hb.end();
        return hb.getHTMLText();
    }

private:
    wxSQLite3Database* db_;


};

#endif

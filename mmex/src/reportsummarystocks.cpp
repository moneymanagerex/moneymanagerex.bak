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

#include "reportsummarystocks.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "util.h"
#include "dbwrapper.h"
#include "stockspanel.h"

wxString mmReportSummaryStocks::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, _("Summary of Stocks"));
    
    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeaderItalic(0, dt);
    hb.startCenter();

    double gain_loss_sum_total = 0.0;

    static const char sql[] = 
    "select ACCOUNTID "
    "from ACCOUNTLIST_V1 "
    "where ACCOUNTTYPE = 'Investment' "
    "and STATUS <> 'Closed' ";

    static const char sql2[] = 
    "select "
           "STOCKNAME, "
           "total(NUMSHARES) as NUMSHARES, "
           "SYMBOL, "
           "total(CURRENTPRICE*NUMSHARES)/total(NUMSHARES) as CURRENTPRICE, "
           "total(PURCHASEPRICE*NUMSHARES)/total(NUMSHARES) as PURCHASEPRICE, "
           "total(VALUE) as VALUE, "
           "total(COMMISSION) as COMMISSION, "
           "min(PURCHASEDATE) as PURCHASEDATE "
    "from STOCK_V1 "
    "where HELDAT = ? "
    "group by UPPER(SYMBOL) ";

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    hb.startTable(("95%"));
    while (q1.NextRow())
    {
        double gain_loss_sum = 0.0;
        int accountID  = q1.GetInt(("ACCOUNTID"));
        wxString heldAt_ = core_->getAccountName(accountID);

        hb.addTotalRow((""), 9, (""));
        hb.addTotalRow(heldAt_, 9, (""));

        display_header(hb);
        wxSQLite3Statement st = db_->PrepareStatement(sql2);
        st.Bind(1, accountID);
        wxSQLite3ResultSet q2 = st.ExecuteQuery();

        while (q2.NextRow())
        {
            mmStockTransactionHolder th;

            //th.id_              = q2.GetInt(("STOCKID"));
            th.shareName_       = q2.GetString(("STOCKNAME"));
            th.numShares_       = q2.GetDouble(("NUMSHARES"));
            th.numSharesStr_    = q2.GetString(("NUMSHARES"));
            th.symbol_          = q2.GetString(("SYMBOL"));

            th.currentPrice_    = q2.GetDouble(("CURRENTPRICE"));
            th.purchasePrice_   = q2.GetDouble(("PURCHASEPRICE"));
            th.value_           = q2.GetDouble(("VALUE"));
            double commission   = q2.GetDouble(("COMMISSION"));
            wxString dateString = q2.GetString(("PURCHASEDATE"));
            wxDateTime dtdt     = mmGetStorageStringAsDate(dateString);
            wxString dt         = mmGetDateForDisplay(db_, dtdt);

            th.gainLoss_        = th.value_ - ((th.numShares_ * th.purchasePrice_) + commission);
            double base_conv_rate = mmDBWrapper::getCurrencyBaseConvRate(db_, accountID);
            gain_loss_sum += th.gainLoss_;
            gain_loss_sum_total += th.gainLoss_ * base_conv_rate;

            wxString commString;
            mmex::formatDoubleToCurrencyEdit(commission, commString);
            mmex::formatDoubleToCurrencyEdit(th.gainLoss_, th.gainLossStr_);
            mmex::formatDoubleToCurrencyEdit(th.currentPrice_, th.cPriceStr_);
            mmex::formatDoubleToCurrencyEdit(th.purchasePrice_, th.pPriceStr_);
            mmex::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);

            hb.startTableRow();
            hb.addTableCell(th.shareName_, false, true);
            hb.addTableCell(th.symbol_);
            hb.addTableCell(dt);
            hb.addTableCell(th.numSharesStr_, true);
            hb.addTableCell(th.pPriceStr_, true);
            hb.addTableCell(th.cPriceStr_, true);
            hb.addTableCell(commString, true);

            if(th.gainLoss_ < 0.0)
                hb.addTableCell(th.gainLossStr_, true, true, true, ("RED"));
            else
                hb.addTableCell(th.gainLossStr_, true, false, true);
            
            hb.addTableCell(th.valueStr_, true);
            hb.endTableRow();
            
        }
        q2.Finalize();

        double invested = 0;
        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_, accountID, false, invested);
        wxString stockBalanceStr;
        wxString gain_loss_sum_str;

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(accountID).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);

        mmex::formatDoubleToCurrency(stockBalance, stockBalanceStr);
        mmex::formatDoubleToCurrency(gain_loss_sum, gain_loss_sum_str);
    
        hb.addRowSeparator(9);
        hb.addTotalRow(_("Total:"), 8, gain_loss_sum_str);
        hb.addTableCell(stockBalanceStr, true, true, true); //numeric, italic, bold

    }
    q1.Finalize();

    mmDBWrapper::loadBaseCurrencySettings(db_);

    double invested = 0;
    double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_, invested);
    wxString stockBalanceStr;
    wxString gain_loss_sum_total_str;
    mmex::formatDoubleToCurrency(gain_loss_sum_total, gain_loss_sum_total_str);
    mmex::formatDoubleToCurrency(stockBalance, stockBalanceStr);

    hb.addRowSeparator(9);
    hb.addTotalRow(_("Grand Total:"), 8, gain_loss_sum_total_str);
    hb.addTableCell(stockBalanceStr, true, true, true); //numeric, italic, bold
    hb.endTableRow();
    hb.endTable();

    hb.endCenter();
    hb.end();
    return hb.getHTMLText();
}

void mmReportSummaryStocks::display_header(mmHTMLBuilder& hb) {
	hb.startTableRow();
	hb.addTableHeaderCell(_("Name"));
	hb.addTableHeaderCell(_("Symbol"));
	hb.addTableHeaderCell(_("Purchase Date"));
	hb.addTableHeaderCell(_("Quantity"), true);
	hb.addTableHeaderCell(_("Purchase Price"), true);
	hb.addTableHeaderCell(_("Current Price"), true);
	hb.addTableHeaderCell(_("Commission"), true);
	hb.addTableHeaderCell(_("Gain/Loss"), true);
	hb.addTableHeaderCell(_("Value"), true);
	hb.endTableRow();
}

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
    mmReportSummary(mmCoreDB* core) : core_(core), db_(core_->db_.get()) {}

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Summary of Accounts"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();
        hb.addLineBreak();

        double tBalance = 0.0;

		hb.startCenter();

		hb.startTable(wxT("50%"));
		hb.startTableRow();
		hb.addTableHeaderCell(_("Account Name"));
		hb.addTableHeaderCell(_("Balance"));
		hb.endTableRow();

        for (int iAdx = 0; iAdx < (int) core_->accountList_.accounts_.size(); iAdx++)
        {
           mmCheckingAccount* pCA 
              = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
           if (pCA && pCA->status_== mmAccount::MMEX_Open)
           {
              double bal = pCA->initialBalance_ 
                  + core_->bTransactionList_.getBalance(pCA->accountID_);
              
              boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pCA->accountID_).lock();
              wxASSERT(pCurrencyPtr);
              mmCurrencyFormatter::loadSettings(pCurrencyPtr);
              double rate = pCurrencyPtr->baseConv_;

              tBalance += bal * rate;;

              wxString balance;
              mmCurrencyFormatter::formatDoubleToCurrency(bal, balance);

			  hb.startTableRow();
			  hb.addTableCell(pCA->accountName_, false, true);
			  hb.addTableCell(balance, true);
			  hb.endTableRow();
           }
        }

        // all sums below will be in base currency!
        mmDBWrapper::loadBaseCurrencySettings(db_);

                wxString tBalanceStr;
		mmCurrencyFormatter::formatDoubleToCurrency(tBalance, tBalanceStr);

		hb.startTableRow();
		hb.addTotalRow(_("Total:"), 2, tBalanceStr);
		hb.endTableRow();

		hb.addRowSeparator(2);

        /* Stocks */
        double invested;
        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_, invested);
        wxString stockBalanceStr;
        mmCurrencyFormatter::formatDoubleToCurrency(stockBalance, stockBalanceStr);
        wxString dispStr =  + stockBalanceStr; 

		hb.startTableRow();
		hb.addTableCell(_("Stock Investments"));
		hb.addTableCell(stockBalanceStr, true);
		hb.endTableRow();

        /* Assets */
        double assetBalance = mmDBWrapper::getAssetBalance(db_);
        wxString assetBalanceStr;
        mmCurrencyFormatter::formatDoubleToCurrency(assetBalance, assetBalanceStr);

		hb.startTableRow();
		hb.addTableCell(_("Assets"));
		hb.addTableCell(assetBalanceStr, true);
		hb.endTableRow();

        tBalance += stockBalance;
        tBalance += assetBalance;
        mmCurrencyFormatter::formatDoubleToCurrency(tBalance, tBalanceStr);

		hb.addRowSeparator(2);

		hb.addTotalRow(_("Total Balance on all Accounts"), 2, tBalanceStr);
		hb.endTable();

		hb.endCenter();

        hb.end();

        return hb.getHTMLText();
    }

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
};

#endif

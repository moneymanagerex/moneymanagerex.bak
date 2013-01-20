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

#include "reportsummary.h"
#include "constants.h"
#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"

mmReportSummary::mmReportSummary(mmCoreDB* core, mmGUIFrame* frame) :
    mmPrintableBase(core),
    frame_(frame),
    db_(core_->db_.get())
{
}

wxString mmReportSummary::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, _("Summary of Accounts"));
    hb.addDateNow();

    double tBalance = 0.0;

    hb.startCenter();

    hb.startTable(wxT("50%"));
    hb.startTableRow();
    hb.addTableHeaderCell(_("Account Name"));
    hb.addTableHeaderCell(_("Balance"));
    hb.endTableRow();

    /* Checking */
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = core_->accountList_.range();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* pCA = it->get();

        if (pCA->acctType_ == ACCOUNT_TYPE_BANK && pCA->status_ == mmAccount::MMEX_Open)
        {
            double bal = pCA->initialBalance_ + core_->bTransactionList_.getBalance(pCA->id_, mmIniOptions::instance().ignoreFutureTransactions_);

            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pCA->id_).lock();
            wxASSERT(pCurrencyPtr);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
            double rate = pCurrencyPtr->baseConv_;

            tBalance += bal * rate;

            wxString balance;
            mmex::formatDoubleToCurrency(bal, balance);

            hb.startTableRow();
            hb.addTableCell(pCA->name_, false, true);
            hb.addTableCell(balance, true);
            hb.endTableRow();
        }
    }

    // all sums below will be in base currency!
    core_->currencyList_.LoadBaseCurrencySettings(core_->dbInfoSettings_.get());

    wxString tBalanceStr;
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    hb.startTableRow();
    hb.addTotalRow(_("Bank Accounts Total:"), 2, tBalanceStr);
    hb.endTableRow();

    hb.addRowSeparator(2);

    /* Terms */
    double tTBalance = 0.0;

    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* pTA = it->get();

        if (pTA->status_== mmAccount::MMEX_Open && pTA->acctType_ == ACCOUNT_TYPE_TERM)
        {
            double bal = pTA->initialBalance_ + core_->bTransactionList_.getBalance(pTA->id_, mmIniOptions::instance().ignoreFutureTransactions_);

            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pTA->id_).lock();
            wxASSERT(pCurrencyPtr);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
            double rate = pCurrencyPtr->baseConv_;

            tTBalance += bal * rate;

            wxString balance;
            mmex::formatDoubleToCurrency(bal, balance);

            hb.startTableRow();
            hb.addTableCell(pTA->name_, false, true);
            hb.addTableCell(balance, true);
            hb.endTableRow();
        }
    }

    // all sums below will be in base currency!
    core_->currencyList_.LoadBaseCurrencySettings(core_->dbInfoSettings_.get());

    wxString tTBalanceStr;
    mmex::formatDoubleToCurrency(tTBalance, tTBalanceStr);

    if ( frame_->hasActiveTermAccounts() )
    {
        hb.startTableRow();
        hb.addTotalRow(_("Term Accounts Total:"), 2, tTBalanceStr);
        hb.endTableRow();
        hb.addRowSeparator(2);
    }

    tBalance += tTBalance;

    /* Stocks */

    wxArrayString accounts_type;
    accounts_type.Add(ACCOUNT_TYPE_STOCK);
    wxArrayInt accounts_id = core_->accountList_.getAccountsID(accounts_type);

    double original_val, stockBalance = 0;
    int accountId = 0;
    for (size_t i = 0; i < accounts_id.Count(); ++i)
    {
        double base_conv_rate = core_->accountList_.getAccountBaseCurrencyConvRate(accounts_id[i]);
        double amount = mmDBWrapper::getStockInvestmentBalance(core_->db_.get(), accounts_id[i], original_val);
        stockBalance += amount * base_conv_rate;
    }

    wxString stockBalanceStr;
    mmex::formatDoubleToCurrency(stockBalance, stockBalanceStr);

    hb.startTableRow();
    hb.addTableCell(_("Stock Investments"));
    hb.addTableCell(stockBalanceStr, true);
    hb.endTableRow();

    /* Assets */
    double assetBalance = mmDBWrapper::getAssetBalance(db_);
    wxString assetBalanceStr;
    mmex::formatDoubleToCurrency(assetBalance, assetBalanceStr);

    hb.startTableRow();
    hb.addTableCell(_("Assets"));
    hb.addTableCell(assetBalanceStr, true);
    hb.endTableRow();

    tBalance += stockBalance;
    tBalance += assetBalance;
    mmex::formatDoubleToCurrency(tBalance, tBalanceStr);

    hb.addRowSeparator(2);

    hb.addTotalRow(_("Total Balance on all Accounts"), 2, tBalanceStr);
    hb.endTable();

    hb.endCenter();

    hb.end();

    return hb.getHTMLText();
}

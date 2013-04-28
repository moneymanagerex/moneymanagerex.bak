/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011, 2012 Nikolay & Stefano Giorgio

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

#include "transactions.h"
#include "../constants.h"
#include "../htmlbuilder.h"
#include "../util.h"
#include <algorithm>


mmReportTransactions::mmReportTransactions( std::vector< wxSharedPtr<mmBankTransaction> > trans,
    mmCoreDB* core, int refAccountID, mmFilterTransactionsDialog* transDialog)
: mmPrintableBase(core)
, trans_(trans)
, refAccountID_(refAccountID)
, transDialog_(transDialog)
{
    std::sort(trans_.begin(), trans_.end(),
        [] (wxSharedPtr<mmBankTransaction> i, wxSharedPtr<mmBankTransaction> j)
    { return (i.get()->date_ < j.get()->date_); });
}

mmReportTransactions::~mmReportTransactions()
{
    // incase the user wants to print a report, we maintain the transaction dialog
    // until we are finished with the report.
    transDialog_->Destroy();
}

wxString addFilterDetailes(wxString sHeader, wxString sValue)
{
    wxString sData;
    sData << "<b>" << sHeader << " </b>" << sValue << "<br>";
    return sData;
}

wxString mmReportTransactions::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();

    wxString transHeading = _("Transaction List ");
    if (refAccountID_ > -1)
    {
        transHeading = wxString::Format(_("Transaction List for Account: %s")
            ,core_->accountList_.GetAccountName(refAccountID_));
    }
    hb.addHeader(2, transHeading);

    hb.addDateNow();
    hb.addLineBreak();

    hb.startTable();
    hb.startTable("95%");

    // Display the data Headings
    hb.startTableRow();
    hb.addTableHeaderCell(_("Date"));
    hb.addTableHeaderCell(_("Account"));
    hb.addTableHeaderCell(_("Payee"));
    hb.addTableHeaderCell(_("Status"));
    hb.addTableHeaderCell(_("Category"));
    hb.addTableHeaderCell(_("Type"));
    hb.addTableHeaderCell(_("Amount"), true);
    hb.addTableHeaderCell(_("Number"));
    hb.addTableHeaderCell(_("Notes"));
    hb.endTableRow();

    // Display the data for each row
    bool unknownnReferenceAccount = true;
    bool transferTransactionFound = false;
    double total = 0;

    for (const auto& it: trans_)
    {
        // For transfer transactions, we need to fix the data reference point first.
        if ( refAccountID_ > -1 && it->transType_ == TRANS_TYPE_TRANSFER_STR &&
             (refAccountID_ == it->accountID_ || refAccountID_ == it->toAccountID_) )
        {
            const wxSharedPtr<mmAccount> pAccount = core_->accountList_.GetAccountSharedPtr(refAccountID_);
            const wxSharedPtr<mmCurrency> pCurrency = pAccount->currency_;
            wxASSERT(pCurrency);
            pCurrency->loadCurrencySettings();
        }

        bool negativeTransAmount = false;   // this can be either a transfer or withdrawl

        // Display the data for the selected row
        hb.startTableRow();
        hb.addTableCell(it->dateStr_, false);
        hb.addTableCellLink(wxString::Format("TRXID:%d"
            , it->transactionID()), it->fromAccountStr_, false);
        hb.addTableCell(it->payeeStr_, false, true);
        hb.addTableCell(it->status_);
        hb.addTableCell(it->fullCatStr_, false, true);

        if (it->transType_ == TRANS_TYPE_DEPOSIT_STR)
            hb.addTableCell(_("Deposit"));
        else if (it->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
        {
            hb.addTableCell(_("Withdrawal"));
            negativeTransAmount = true;
        }
        else if (it->transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            hb.addTableCell(_("Transfer"));
            if (refAccountID_ >= 0 )
            {
                unknownnReferenceAccount = false;
                if (it->accountID_ == refAccountID_)
                    negativeTransAmount   = true;  // transfer is a withdrawl from account
            }
            else if (it->fromAccountStr_ == it->payeeStr_)
                negativeTransAmount = true;
        }

        // Get the exchange rate for the selected account
        double dbRate = core_->accountList_.getAccountBaseCurrencyConvRate(it->accountID_);
        double transAmount = it->amt_ * dbRate;
        if (it->reportCategAmountStr_ != "")
        {
            transAmount = it->reportCategAmount_ * dbRate;
            if (it->transType_ == TRANS_TYPE_WITHDRAWAL_STR && transAmount < 0)
                negativeTransAmount = false;
            else if (it->transType_ == TRANS_TYPE_DEPOSIT_STR && transAmount < 0)
                negativeTransAmount = true;
        }

        wxString amtColour = negativeTransAmount ? "RED" : "BLACK";

        if (it->reportCategAmountStr_ == "")
            hb.addTableCell(it->transAmtString_, true, false,false, amtColour);
        else
            hb.addTableCell(it->reportCategAmountStr_, true, false,false, amtColour);
        hb.addTableCell(it->transNum_);
        hb.addTableCell(it->notes_, false, true);
        hb.endTableRow();

        if (it->status_ != "V")
        {
            if (it->transType_ == TRANS_TYPE_DEPOSIT_STR)
                total += transAmount;
            else if (it->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                total -= transAmount;
            else if (it->transType_ == TRANS_TYPE_TRANSFER_STR)
            {
                transferTransactionFound = true;
                if (negativeTransAmount)
                    total -= transAmount;
                else
                    total += transAmount;
            }
        }
    }

    // work out the total balance for all the data at base rate
    wxString balanceStr;
    core_->currencyList_.LoadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(total, balanceStr);

    // display the total balance.
    hb.addRowSeparator(9);
    hb.addTotalRow(_("Total Amount: "), 7, balanceStr);

    hb.endTable();
    hb.endCenter();

    if (unknownnReferenceAccount && transferTransactionFound)
    {
        hb.addHorizontalLine();
        hb.addHeader(1, _("<b>Note:</b> Transactions contain <b>'transfers'</b> may either be added or subtracted to the <b>'Total Amount'</b> depending on last selected account."));
    }

    // Extract the parameters from the transaction dialog and add them to the report.
    wxString filterDetails;

    if ( transDialog_->getAccountCheckBox())
        filterDetails << addFilterDetailes(_("Account:"), transDialog_->getAccountName());

    //Date range
    if ( transDialog_->getDateRangeCheckBox())
        filterDetails << addFilterDetailes(_("Date Range:"), transDialog_->userDateRangeStr());

    //Payees
    if ( transDialog_->getPayeeCheckBox())
        filterDetails << addFilterDetailes(_("Payee:"), transDialog_->userPayeeStr());

    //Category
    if ( transDialog_->getCategoryCheckBox())
        filterDetails << "<b>" << _("Category:") << " </b>" <<transDialog_->userCategoryStr()
        << (transDialog_->getExpandStatus() ? wxString(" <b> ") << _("Subcategory:") << " </b>" << _("Any"): "")
        << "<br>";
    //Status
    if ( transDialog_->getStatusCheckBox())
        filterDetails << addFilterDetailes(_("Status:"), transDialog_->userStatusStr());
    //Type
    if ( transDialog_->getTypeCheckBox() )
        filterDetails << addFilterDetailes(_("Type:"), transDialog_->userTypeStr());
    //Amount Range
    if ( transDialog_->getAmountRangeCheckBox())
        filterDetails << addFilterDetailes(_("Amount Range:"), transDialog_->userAmountRangeStr());
    //Number
    if ( transDialog_->getNumberCheckBox())
        filterDetails << addFilterDetailes(_("Number:"), transDialog_->getNumber());
    //Notes
    if ( transDialog_->getNotesCheckBox())
        filterDetails << addFilterDetailes(_("Notes:"), transDialog_->getNotes());

    if ( !filterDetails.IsEmpty())
    {
        hb.addHorizontalLine();
        filterDetails.Prepend( wxString()<< "<b>" << _("Filtering Details: ") << "</b><br>");
        hb.addParaText(filterDetails );
    }

    hb.end();

    return hb.getHTMLText();
}

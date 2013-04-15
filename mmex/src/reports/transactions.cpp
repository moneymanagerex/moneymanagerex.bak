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

mmReportTransactions::mmReportTransactions( std::vector< wxSharedPtr<mmBankTransaction> > trans,
    mmCoreDB* core, int refAccountID, mmFilterTransactionsDialog* transDialog)
: mmPrintableBase(core)
, trans_(trans)
, refAccountID_(refAccountID)
, transDialog_(transDialog)
{}

mmReportTransactions::~mmReportTransactions()
{
    // incase the user wants to print a report, we maintain the transaction dialog
    // until we are finished with the report.
    transDialog_->Destroy();
}

wxString addFilterDetailes(wxString sHeader, wxString sValue)
{
    wxString sData;
    sData << wxT("<b>") << sHeader << wxT(" </b>") << sValue << wxT("<br>");
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
            ,core_->accountList_.GetAccountName(refAccountID_).c_str());
    }
    hb.addHeader(2, transHeading);

    hb.addDateNow();
    hb.addLineBreak();

    hb.startTable();
    hb.startTable(wxT("95%"));

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

    for (std::vector<wxSharedPtr<mmBankTransaction> >::const_iterator it = trans_.begin();
        it != trans_.end(); ++ it)
    {
        // For transfer transactions, we need to fix the data reference point first.
        if ( refAccountID_ > -1 && it->get()->transType_ == TRANS_TYPE_TRANSFER_STR &&
             (refAccountID_ == it->get()->accountID_ || refAccountID_ == it->get()->toAccountID_) )
        {
            const wxSharedPtr<mmAccount> pAccount = core_->accountList_.GetAccountSharedPtr(refAccountID_);
            const wxSharedPtr<mmCurrency> pCurrency = pAccount->currency_;
            wxASSERT(pCurrency);
            pCurrency->loadCurrencySettings();
        }

        bool negativeTransAmount = false;   // this can be either a transfer or withdrawl

        // Display the data for the selected row
        hb.startTableRow();
        hb.addTableCell(it->get()->dateStr_, false);
        hb.addTableCellLink(wxString::Format(wxT("TRXID:%d")
            , it->get()->transactionID()), it->get()->fromAccountStr_, false);
        hb.addTableCell(it->get()->payeeStr_, false, true);
        hb.addTableCell(it->get()->status_);
        hb.addTableCell(it->get()->fullCatStr_, false, true);

        if (it->get()->transType_ == TRANS_TYPE_DEPOSIT_STR)
            hb.addTableCell(_("Deposit"));
        else if (it->get()->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
        {
            hb.addTableCell(_("Withdrawal"));
            negativeTransAmount = true;
        }
        else if (it->get()->transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            hb.addTableCell(_("Transfer"));
            if (refAccountID_ >= 0 )
            {
                unknownnReferenceAccount = false;
                if (it->get()->accountID_ == refAccountID_)
                    negativeTransAmount   = true;  // transfer is a withdrawl from account
            }
            else if (it->get()->fromAccountStr_ == it->get()->payeeStr_)
                negativeTransAmount = true;
        }

        // Get the exchange rate for the selected account
        double dbRate = core_->accountList_.getAccountBaseCurrencyConvRate(it->get()->accountID_);
        double transAmount = it->get()->amt_ * dbRate;
        if (it->get()->reportCategAmountStr_ != wxT(""))
        {
            transAmount = it->get()->reportCategAmount_ * dbRate;
            if (it->get()->transType_ == TRANS_TYPE_WITHDRAWAL_STR && transAmount < 0)
                negativeTransAmount = false;
            else if (it->get()->transType_ == TRANS_TYPE_DEPOSIT_STR && transAmount < 0)
                negativeTransAmount = true;
        }

        wxString amtColour = negativeTransAmount ? wxT("RED") : wxT("BLACK");

        if (it->get()->reportCategAmountStr_ == wxT(""))
            hb.addTableCell(it->get()->transAmtString_, true, false,false, amtColour);
        else
            hb.addTableCell(it->get()->reportCategAmountStr_, true, false,false, amtColour);
        hb.addTableCell(it->get()->transNum_);
        hb.addTableCell(it->get()->notes_, false, true);
        hb.endTableRow();

        if (it->get()->status_ != wxT("V"))
        {
            if (it->get()->transType_ == TRANS_TYPE_DEPOSIT_STR)
                total += transAmount;
            else if (it->get()->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                total -= transAmount;
            else if (it->get()->transType_ == TRANS_TYPE_TRANSFER_STR)
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
        filterDetails << wxT("<b>") << _("Category:") << wxT(" </b>") <<transDialog_->userCategoryStr()
        << (transDialog_->getExpandStatus() ? wxString(wxT(" <b> ")) << _("Subcategory:") << wxT(" </b>") << _("Any"): wxT(""))
        << wxT("<br>");
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
        filterDetails.Prepend( wxString()<< wxT("<b>") << _("Filtering Details: ") << wxT ("</b><br>"));
        hb.addParaText(filterDetails );
    }

    hb.end();

    return hb.getHTMLText();
}

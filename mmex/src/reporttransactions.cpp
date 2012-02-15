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

#include "reporttransactions.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcheckingpanel.h"
#include "mmtransaction.h"
#include "reportbudget.h"

mmReportTransactions::mmReportTransactions( std::vector< boost::shared_ptr<mmBankTransaction> >* trans, 
    mmCoreDB* core, int refAccountID, wxString refAccountStr, mmFilterTransactionsDialog* transDialog)
:trans_(trans), mmPrintableBase(core), refAccountID_(refAccountID), refAccountStr_(refAccountStr), transDialog_(transDialog)
{
}

mmReportTransactions::~mmReportTransactions()
{
    delete trans_;
    // incase the user wants to print a report, we maintain the transaction dialog
	// until we are finished with the report.
    transDialog_->Destroy();
}

wxString mmReportTransactions::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();

    wxString transHeading = _("Transaction List ");
    if (refAccountID_ > -1)
        transHeading += wxString() << _("for Account: ") << refAccountStr_;
    hb.addHeader(3, transHeading);

    bool includeDateRange = false; 
    wxDateTime startDate = wxDateTime(wxDateTime::Now());
    wxDateTime endDate = wxDateTime(wxDateTime::Now());
    if (transDialog_->getDateRange(startDate, endDate))
    {
        includeDateRange = true;
    }
    mmCommonReportDetails dateDisplay(NULL);
    dateDisplay.DisplayDateHeading(hb, startDate, endDate, includeDateRange);
    
    hb.startCenter();
    hb.startTable();
    hb.startTable(wxT("92%"));

    // Display the data Headings
    hb.startTableRow();
    hb.addTableHeaderCell(_("Date"));
    hb.addTableHeaderCell(_("Number"));
    hb.addTableHeaderCell(_("Account"));
    hb.addTableHeaderCell(_("Payee"));
    hb.addTableHeaderCell(_("Type"));
    hb.addTableHeaderCell(_("Category"));
    hb.addTableHeaderCell(_("Notes"));
    hb.addTableHeaderCell(_("Status"));
    hb.addTableHeaderCell(_("Amount"));
    hb.endTableRow();

    // Display the data for each row
    bool unknownnReferenceAccount = true;
    bool transferTransactionFound = false;
    double total = 0;
    for (unsigned int index = 0; index < trans_->size(); index++)
    {
        // std::vector<wxString> data;
        std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = *trans_;

        // For transfer transactions, we need to fix the data reference point first.
        if ( refAccountID_ > -1 && refTrans[index]->transType_ == TRANS_TYPE_TRANSFER_STR && 
             (refAccountID_ == refTrans[index]->accountID_ || refAccountID_ == refTrans[index]->toAccountID_) )
        {
            boost::shared_ptr<mmAccount> pAccount = core_->accountList_.getAccountSharedPtr(refAccountID_);
            boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
            wxASSERT(pCurrency);
            pCurrency->loadCurrencySettings();
//XXX check
//            refTrans[index]->updateAllData(core_,refAccountID_,pCurrency);
        }

        bool negativeTransAmount = false;   // this can be either a transfer or withdrawl

        // Display the data for the selected row
        hb.startTableRow();
        hb.addTableCell(refTrans[index]->dateStr_);
        hb.addTableCell(refTrans[index]->transNum_);
        hb.addTableCell(refTrans[index]->fromAccountStr_, false, true);
        hb.addTableCell(refTrans[index]->payeeStr_, false, true);
        if (refTrans[index]->transType_ == TRANS_TYPE_DEPOSIT_STR)
        {
            hb.addTableCell(_("Deposit"));
        }
        else if (refTrans[index]->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
        {
            hb.addTableCell(_("Withdrawal"));
            negativeTransAmount = true;
        }
        else if (refTrans[index]->transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            hb.addTableCell(_("Transfer"));
            if (refAccountID_ >= 0 )
            {
                unknownnReferenceAccount = false;
                if (refTrans[index]->accountID_ == refAccountID_)
                    negativeTransAmount   = true;  // transfer is a withdrawl from account
            }
            else if (refTrans[index]->fromAccountStr_ == refTrans[index]->payeeStr_)
            {
                negativeTransAmount = true;
            }
        }

        wxString amtColour = wxT("#000000"); // black
        if (negativeTransAmount)
            amtColour = wxT("#ff0000");      //  red

        hb.addTableCell(refTrans[index]->fullCatStr_, false, true);
        hb.addTableCell(refTrans[index]->notes_, false, true);
        hb.addTableCell(refTrans[index]->status_);
        if (refTrans[index]->reportCategAmountStr_ == wxT(""))
            hb.addTableCell(refTrans[index]->transAmtString_, true, false,false, amtColour);
        else
            hb.addTableCell(refTrans[index]->reportCategAmountStr_, true, false,false, amtColour);
        hb.endTableRow();

        // Get the exchange rate for the selected account
        double dbRate = core_->accountList_.getAccountBaseCurrencyConvRate(refTrans[index]->accountID_);
        double transAmount = refTrans[index]->amt_ * dbRate;
        if (refTrans[index]->reportCategAmountStr_ != wxT(""))
        {
            transAmount = refTrans[index]->reportCategAmount_ * dbRate;
        }

        if (refTrans[index]->status_ != wxT("V"))
        {
            if (refTrans[index]->transType_ == TRANS_TYPE_DEPOSIT_STR)
            {
                total += transAmount;
            }
            else if (refTrans[index]->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
            {
                total -= transAmount;
            }
            else if (refTrans[index]->transType_ == TRANS_TYPE_TRANSFER_STR)
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
    core_->loadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(total, balanceStr);

    // display the total balance.
	hb.addRowSeparator(9);
	hb.addTotalRow(_("Total Amount: "), 9, balanceStr);

    hb.endTable();
	hb.endCenter();

    if (unknownnReferenceAccount && transferTransactionFound)
    {
        hb.addHorizontalLine();
        hb.addHeader(7, _("<b>Note:</b> Transactions contain <b>'transfers'</b> may either be added or subtracted to the <b>'Total Amount'</b> depending on last selected account."));
    }

    // Extract the parameters from the transaction dialog and add them to the report.
    wxString filterDetails;

    if ( !transDialog_->refAccountStr_.IsEmpty())
        filterDetails << wxT("<b>") << _("Account: ") << wxT("</b>") << transDialog_->refAccountStr_ << wxT("<br>");
    //Date range
    if ( !transDialog_->userDateRangeStr().IsEmpty())
		filterDetails << wxT("<b>") << _("Date Range: ") << wxT("</b>") << transDialog_->userDateRangeStr() << wxT("<br>");

    //Payees
    if ( !transDialog_->userPayeeStr().IsEmpty())
        filterDetails << wxT("<b>") << _("Payee: ") << wxT("</b>") <<transDialog_->userPayeeStr() << wxT("<br>");

    //Category
    if ( !transDialog_->userCategoryStr().IsEmpty())
        filterDetails << wxT("<b>") << _("Category: ") << wxT("</b>") <<transDialog_->userCategoryStr() << wxT("<br>");
	
	//Status
    if ( !transDialog_->userStatusStr().IsEmpty())
        filterDetails << wxT("<b>") << _("Status: ") << wxT("</b>") <<transDialog_->userStatusStr() << wxT("<br>");
	//Type
    if ( !transDialog_->userTypeStr().IsEmpty())
        filterDetails << wxT("<b>") << wxT("Type: ") << wxT("</b>") <<transDialog_->userTypeStr() << wxT("<br>");
	//Amount Range
    if ( !transDialog_->userAmountRangeStr().IsEmpty())
        filterDetails << wxT("<b>") << wxT("Amount Range: ") << wxT("</b>") <<transDialog_->userAmountRangeStr() << wxT("<br>");
	//Number
    if ( !transDialog_->userTransNumberStr().IsEmpty())
        filterDetails << wxT("<b>") << wxT("Number: ") << wxT("</b>") <<transDialog_->userTransNumberStr() << wxT("<br>");
	//Notes
    if ( !transDialog_->userNotesStr().IsEmpty())
        filterDetails << wxT("<b>") << wxT("Notes: ") << wxT("</b>") <<transDialog_->userNotesStr() << wxT("<br>");
	
    if ( !filterDetails.IsEmpty())
    {
        wxString filterDetailsStr;
        hb.addHorizontalLine();
        filterDetailsStr << wxT("<b>") << _("Filtering Details: ") << wxT ("</b><br>") << filterDetails;
        hb.addHeader(7, filterDetailsStr );
    }

    hb.end();
    
    return hb.getHTMLText();
}

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
#include "util.h"
#include "mmcheckingpanel.h"
#include "mmtransaction.h"
#include "reportbudget.h"
#include "dbwrapper.h" //XXX

mmReportTransactions::mmReportTransactions( std::vector< boost::shared_ptr<mmBankTransaction> >* trans, 
    mmCoreDB* core, int refAccountID, wxString refAccountStr, mmFilterTransactionsDialog* transDialog)
:mmPrintableBase(core), trans_(trans), refAccountID_(refAccountID), refAccountStr_(refAccountStr), transDialog_(transDialog)
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
    hb.addHeader(2, transHeading);

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
    hb.startTable(("95%"));

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
    for (unsigned int index = 0; index < trans_->size(); index++)
    {
        // std::vector<wxString> data;
        std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = *trans_;

        // For transfer transactions, we need to fix the data reference point first.
        if ( refAccountID_ > -1 && refTrans[index]->transType_ == TRANS_TYPE_TRANSFER_STR && 
             (refAccountID_ == refTrans[index]->accountID_ || refAccountID_ == refTrans[index]->toAccountID_) )
        {
            boost::shared_ptr<mmAccount> pAccount = core_->getAccountSharedPtr(refAccountID_);
            boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
            wxASSERT(pCurrency);
            pCurrency->loadCurrencySettings();
//XXX check
//            refTrans[index]->updateAllData(core_,refAccountID_,pCurrency);
        }

        bool negativeTransAmount = false;   // this can be either a transfer or withdrawl

        // Display the data for the selected row
        hb.startTableRow();
        hb.addTableCell(refTrans[index]->dateStr_, true);
        hb.addTableCell(refTrans[index]->fromAccountStr_, false, true);
        hb.addTableCell(refTrans[index]->payeeStr_, false, true);
        hb.addTableCell(refTrans[index]->status_);
        hb.addTableCell(refTrans[index]->fullCatStr_, false, true);

        if (refTrans[index]->transType_ == TRANS_TYPE_DEPOSIT_STR)
            hb.addTableCell(_("Deposit"));
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
                negativeTransAmount = true;
        }

        wxString amtColour = negativeTransAmount ? ("RED") : ("BLACK");

        if (refTrans[index]->reportCategAmountStr_ == (""))
            hb.addTableCell(refTrans[index]->transAmtString_, true, false,false, amtColour);
        else
            hb.addTableCell(refTrans[index]->reportCategAmountStr_, true, false,false, amtColour);
        hb.addTableCell(refTrans[index]->transNum_);
        hb.addTableCell(refTrans[index]->notes_, false, true);
        hb.endTableRow();

        // Get the exchange rate for the selected account
        double dbRate = core_->accountList_.getAccountBaseCurrencyConvRate(refTrans[index]->accountID_);
        double transAmount = refTrans[index]->amt_ * dbRate;
        if (refTrans[index]->reportCategAmountStr_ != (""))
        {
            transAmount = refTrans[index]->reportCategAmount_ * dbRate;
        }

        if (refTrans[index]->status_ != ("V"))
        {
            if (refTrans[index]->transType_ == TRANS_TYPE_DEPOSIT_STR)
                total += transAmount;
            else if (refTrans[index]->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                total -= transAmount;
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
	hb.addTotalRow(_("Total Amount: "), 7, balanceStr);

    hb.endTable();
	hb.endCenter();

    if (unknownnReferenceAccount && transferTransactionFound)
    {
        hb.addHorizontalLine();
        hb.addHeaderItalic(0, _("<b>Note:</b> Transactions contain <b>'transfers'</b> may either be added or subtracted to the <b>'Total Amount'</b> depending on last selected account."));
    }

    // Extract the parameters from the transaction dialog and add them to the report.
    wxString filterDetails;

    if ( !transDialog_->refAccountStr_.IsEmpty())
        filterDetails << ("<b>") << _("Account: ") << ("</b>") << transDialog_->refAccountStr_ << ("<br>");
    //Date range
    if ( !transDialog_->userDateRangeStr().IsEmpty())
		filterDetails << ("<b>") << _("Date Range: ") << ("</b>") << transDialog_->userDateRangeStr() << ("<br>");

    //Payees
    if ( !transDialog_->userPayeeStr().IsEmpty())
        filterDetails << ("<b>") << _("Payee: ") << ("</b>") <<transDialog_->userPayeeStr() << ("<br>");

    //Category
    if ( !transDialog_->userCategoryStr().IsEmpty())
        filterDetails << ("<b>") << _("Category: ") << ("</b>") <<transDialog_->userCategoryStr() << ("<br>");
	
	//Status
    if ( !transDialog_->userStatusStr().IsEmpty())
        filterDetails << ("<b>") << _("Status: ") << ("</b>") <<transDialog_->userStatusStr() << ("<br>");
	//Type
    if ( !transDialog_->userTypeStr().IsEmpty())
        filterDetails << ("<b>") << ("Type: ") << ("</b>") <<transDialog_->userTypeStr() << ("<br>");
	//Amount Range
    if ( !transDialog_->userAmountRangeStr().IsEmpty())
        filterDetails << ("<b>") << ("Amount Range: ") << ("</b>") <<transDialog_->userAmountRangeStr() << ("<br>");
	//Number
    if ( !transDialog_->userTransNumberStr().IsEmpty())
        filterDetails << ("<b>") << ("Number: ") << ("</b>") <<transDialog_->userTransNumberStr() << ("<br>");
	//Notes
    if ( !transDialog_->userNotesStr().IsEmpty())
        filterDetails << ("<b>") << ("Notes: ") << ("</b>") <<transDialog_->userNotesStr() << ("<br>");
	
    if ( !filterDetails.IsEmpty())
    {
        wxString filterDetailsStr;
        hb.addHorizontalLine();
        filterDetailsStr << ("<b>") << _("Filtering Details: ") 
        << wxT ("</b><br>") << filterDetails;
        hb.addHeaderItalic(0, filterDetailsStr );
    }

    hb.end();
    
    return hb.getHTMLText();
}

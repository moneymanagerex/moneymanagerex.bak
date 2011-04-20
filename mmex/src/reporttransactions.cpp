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

#include "reporttransactions.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcheckingpanel.h"
#include "mmtransaction.h"
#include "mmcoredb.h"

#include <algorithm>

mmReportTransactions::mmReportTransactions( std::vector< boost::shared_ptr<mmBankTransaction> >* trans, 
    mmCoreDB* core, int refAccountID, wxString refAccountStr)
:trans_(trans), core_(core), refAccountID_(refAccountID), refAccountStr_(refAccountStr)
{
}

mmReportTransactions::~mmReportTransactions()
{
    delete trans_;
}

wxString mmReportTransactions::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();

    wxString transHeading = _("Transaction List ");
    if (refAccountID_ > -1)
        transHeading += wxString() << _("for Account: ") << refAccountStr_;
    hb.addHeader(3, transHeading);

    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(7, dt);
    hb.addLineBreak();
    hb.addLineBreak();
    
    hb.startCenter();
    hb.startTable();

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
        std::vector<wxString> data;
        std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = *trans_;

        bool negativeTransAmount = false;   // this can be either a transfer or withdrawl

        // Display the data for the selected row
        hb.startTableRow();
        hb.addTableCell(refTrans[index]->dateStr_);
        hb.addTableCell(refTrans[index]->transNum_);
        hb.addTableCell(refTrans[index]->fromAccountStr_, false, true);
        hb.addTableCell(refTrans[index]->payeeStr_, false, true);
        if (refTrans[index]->transType_ == wxT("Deposit"))
        {
            hb.addTableCell(_("Deposit"));
        }
        else if (refTrans[index]->transType_ == wxT("Withdrawal"))
        {
            hb.addTableCell(_("Withdrawal"));
            negativeTransAmount = true;
        }
        else if (refTrans[index]->transType_ == wxT("Transfer"))
        {
            hb.addTableCell(_("Transfer"));
            if (refAccountID_ >= 0 )
            {
                unknownnReferenceAccount = false;
                if (refTrans[index]->accountID_ == refAccountID_)
                    negativeTransAmount   = true;  // transfer is a withdrawl from account
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
            transAmount = refTrans[index]->reportCategAmount_ * dbRate;

        if (refTrans[index]->transType_ == wxT("Deposit"))
        {
            total += transAmount;
        }
        else if (refTrans[index]->transType_ == wxT("Withdrawal"))
        {
            total -= transAmount;
        }
        else if (refTrans[index]->transType_ == wxT("Transfer"))
        {
            transferTransactionFound = true;
            if (negativeTransAmount)
                total -= transAmount;
            else
                total += transAmount;
        }
    }

    // work out the total balance for all the data at base rate
    wxString balanceStr;
    core_->currencyList_.loadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(total, balanceStr);

    // display the total balance.
	hb.addRowSeparator(9);
	hb.addTotalRow(_("Total Amount: "), 9, balanceStr);

    hb.endTable();
	hb.endCenter();

    if (unknownnReferenceAccount && transferTransactionFound)
    {
        hb.addHorizontalLine();
        hb.addHeader(7, _("<b>Note:</b> Total Amount may be incorrect as <b>transfers</B> have been added to the total."));
    }
    hb.end();


//TODO: We want to know what parameters have been selected
    //dt = _("Filter: ");

    //hb.addLineBreak();
    //hb.addLineBreak();
    //hb.addHeader(7, dt);

    return hb.getHTMLText();
}

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

mmReportTransactions::mmReportTransactions(std::vector< boost::shared_ptr<mmBankTransaction> >* trans, mmCoreDB* core) : 
        trans_(trans), 
        core_(core)
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
        hb.addHeader(3, _("Transaction List"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();
     
		hb.startCenter();

        hb.startTable();

		hb.startTableRow();
		hb.addTableHeaderCell(_("Date"));
		hb.addTableHeaderCell(_("Account"));
		hb.addTableHeaderCell(_("Payee"));
		hb.addTableHeaderCell(_("Type"));
		hb.addTableHeaderCell(_("Category"));
        hb.addTableHeaderCell(_("Notes"));
		hb.addTableHeaderCell(_("Status"));
		hb.addTableHeaderCell(_("Amount"));
		hb.endTableRow();

        double total = 0;
        for (unsigned int index = 0; index < trans_->size(); index++)
        {
            std::vector<wxString> data;
            std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = *trans_;

			hb.startTableRow();
			hb.addTableCell(refTrans[index]->dateStr_);
			hb.addTableCell(refTrans[index]->fromAccountStr_, false, true);
			hb.addTableCell(refTrans[index]->payeeStr_, false, true);
			if (refTrans[index]->transType_ == wxT("Deposit"))
				hb.addTableCell(_("Deposit"));
			else if (refTrans[index]->transType_ == wxT("Withdrawal"))
				hb.addTableCell(_("Withdrawal"));
			else if (refTrans[index]->transType_ == wxT("Transfer"))
				hb.addTableCell(_("Transfer"));
			hb.addTableCell(refTrans[index]->fullCatStr_, false, true);
            hb.addTableCell(refTrans[index]->notes_, false, true);
			hb.addTableCell(refTrans[index]->status_);

            if (refTrans[index]->reportCategAmountStr_ == wxT(""))  
    			hb.addTableCell(refTrans[index]->transAmtString_, true);
            else
            {
                hb.addTableCell(refTrans[index]->reportCategAmountStr_, true);
            }
			hb.endTableRow();

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
                total += transAmount;
            }
        }

        wxString balanceStr;
        core_->currencyList_.loadBaseCurrencySettings();
        mmex::formatDoubleToCurrency(total, balanceStr);

	hb.addRowSeparator(8);
	hb.addTotalRow(_("Total Amount: "), 8, balanceStr);
        hb.endTable();

	hb.endCenter();
        hb.end();

        return hb.getHTMLText();
}

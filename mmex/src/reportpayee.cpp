#include "reportpayee.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmgraphpie.h"
#include "mmcoredb.h"

mmReportPayeeExpenses::mmReportPayeeExpenses(
        mmCoreDB* core, 
        bool ignoreDate, 
        wxDateTime dtBegin, 
        wxDateTime dtEnd
) : 
        core_(core),
        dtBegin_(dtBegin),
        dtEnd_(dtEnd),
        ignoreDate_(ignoreDate)
{
}

wxString mmReportPayeeExpenses::getHTMLText()
{
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("To Whom the Money Goes"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        wxDateTime tBegin = dtBegin_;
        if (!ignoreDate_)
        {
            wxString dtRange = _("From: ") 
                + mmGetNiceDateSimpleString(tBegin.Add(wxDateSpan::Day())) 
                + _(" To: ") 
                + mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        hb.addLineBreak();

		hb.startCenter();

        int numPayees = (int)core_->payeeList_.payees_.size();
        // Add the graph
        mmGraphPie gg;
        if (numPayees)
           hb.addImage(gg.getOutputFileName());

		hb.startTable(wxT("50%"));
		hb.startTableRow();
		hb.addTableHeaderCell(_("Payee"));
		hb.addTableHeaderCell(_("Amount"));
		hb.endTableRow();

        core_->currencyList_.loadBaseCurrencySettings();
        
        double total = 0.0;
        double positiveTotal = 0.0;
        double negativeTotal = 0.0;
        std::vector<ValuePair> valueList;
        for (int idx = 0; idx < numPayees; idx++)
        {
            wxString balance;
            double amt = core_->bTransactionList_.getAmountForPayee(
               core_->payeeList_.payees_[idx]->payeeID_, ignoreDate_, 
                dtBegin_, dtEnd_);

            mmex::formatDoubleToCurrency(amt, balance);

            if (amt != 0.0)
            {
                total += amt;
                if (amt>0.0)
                positiveTotal += amt;
                if (amt<0.0)
                negativeTotal += amt;
                ValuePair vp;
                vp.label = core_->payeeList_.payees_[idx]->payeeName_;
                vp.amount = amt;
                valueList.push_back(vp);

                hb.startTableRow();
                hb.addTableCell(core_->payeeList_.payees_[idx]->payeeName_, false, true);

                hb.addTableCell(balance, true, true, true, (amt<0.0 ? wxT("RED") : wxT("BLACK")));
                hb.endTableRow();
			}
        }

        wxString payeetotalStr;
        wxString positiveTotalStr;
        wxString negativeTotalStr;
        wxString colorStr = (total<0.0 ? wxT("RED") : wxT("BLACK"));
        mmex::formatDoubleToCurrency(total, payeetotalStr);
        mmex::formatDoubleToCurrency(positiveTotal, positiveTotalStr);
        mmex::formatDoubleToCurrency(negativeTotal, negativeTotalStr);

        hb.addRowSeparator(2);
        hb.startTableRow();
        hb.addTableCell(_("Income:"),false, true, true, wxT("BLACK"));
        hb.addTableCell(positiveTotalStr, true, false, true, wxT("BLACK"));
		hb.endTableRow();
        hb.startTableRow();
        hb.addTableCell(_("Expenses:"),false, true, true, wxT("BLACK"));
        hb.addTableCell(negativeTotalStr, true, false, true, wxT("RED"));
		hb.endTableRow();
        hb.addRowSeparator(2);
        hb.startTableRow();
        hb.addTableCell(_("Payees Total: "),false, true, true, wxT("BLACK"));
        hb.addTableCell(payeetotalStr, true, false, true, colorStr);
		hb.endTableRow();

        hb.endTable();
		hb.endCenter();

        hb.end();

        gg.init(valueList);
        gg.Generate(_("To Whom the Money Goes"));

        return hb.getHTMLText();
}

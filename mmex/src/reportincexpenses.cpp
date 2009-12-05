#include "reportincexpenses.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmgraphincexpensesmonth.h"
#include "mmcoredb.h"


mmReportIncomeExpenses::mmReportIncomeExpenses(
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
        wxASSERT(dtBegin_ == dtBegin);
}

wxString mmReportIncomeExpenses::getHTMLText()
{
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Income vs Expenses"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        wxDateTime tBegin = dtBegin_;
        if (!ignoreDate_)
        {
            wxString dtRange = _("From: ") 
                + mmGetNiceDateSimpleString(tBegin.Add(wxDateSpan::Day())) + _(" To: ") 
                + mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        hb.addLineBreak();

		hb.startCenter();

        core_->currencyList_.loadBaseCurrencySettings();

        double expenses = 0.0;
        double income = 0.0;
        core_->bTransactionList_.getExpensesIncome(-1,expenses, income,  ignoreDate_, dtBegin_,dtEnd_);

        mmGraphIncExpensesMonth gg;
        gg.init(income, expenses);
        gg.Generate(_("Income vs Expenses"));
        hb.addImage(gg.GetOutputFileName());

		hb.startTable(wxT("50%"));
		hb.startTableRow();
		hb.addTableHeaderCell(_("Type"));
		hb.addTableHeaderCell(_("Amount"));
		hb.endTableRow();

        wxString incString;
        wxString expString;
		wxString diffString;
        mmCurrencyFormatter::formatDoubleToCurrency(expenses, expString);
        mmCurrencyFormatter::formatDoubleToCurrency(income, incString);
		mmCurrencyFormatter::formatDoubleToCurrency(income - expenses, diffString);

		hb.startTableRow();
		hb.addTableCell(_("Income:"), false, true);
		hb.addTableCell(incString, true, false, true);
		hb.endTableRow();

		hb.startTableRow();
		hb.addTableCell(_("Expenses:"), false, true);
		if(expenses > income)
		{
			hb.addTableCell(expString, true, true, true, wxT("#ff0000"));
		}
		else
		{
			hb.addTableCell(expString, true, false, true);
		}

	hb.endTableRow();

	hb.addRowSeparator(2);
	hb.addTotalRow(_("Difference:"), 2, diffString);

        hb.endTable();
	hb.endCenter();

        hb.end();
        return hb.getHTMLText();
}

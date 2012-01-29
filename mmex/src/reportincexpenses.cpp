#include "reportincexpenses.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmgraphincexpensesmonth.h"
#include "reportbudget.h"

mmReportIncomeExpenses::mmReportIncomeExpenses(
    mmCoreDB* core, 
    bool ignoreDate, 
    wxDateTime dtBegin, 
    wxDateTime dtEnd,
    wxString title
) : mmPrintableBase(core), dtBegin_(dtBegin), dtEnd_(dtEnd), ignoreDate_(ignoreDate), title_(title)
{
    wxASSERT(dtBegin_ == dtBegin);
}

wxString mmReportIncomeExpenses::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(3, title_);

    mmCommonReportDetails dateDisplay(NULL);
    wxDateTime tBegin = dtBegin_;    // date needs to be adjusted
    dateDisplay.DisplayDateHeading(hb, tBegin.Add(wxDateSpan::Day()), dtEnd_, !ignoreDate_);

    //wxDateTime now = wxDateTime::Now();
    //wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    //hb.addHeader(7, dt);
    //hb.addLineBreak();
    //hb.addLineBreak();

    //wxDateTime tBegin = dtBegin_;
    //if (!ignoreDate_)
    //{
    //    wxString dtRange = _("From: ") 
    //        + mmGetNiceDateSimpleString(tBegin.Add(wxDateSpan::Day())) + _(" To: ") 
    //        + mmGetNiceDateSimpleString(dtEnd_);
    //    hb.addHeader(7, dtRange);
    //    hb.addLineBreak();
    //}

    hb.addLineBreak();

	hb.startCenter();

    core_->currencyList_.loadBaseCurrencySettings();

    double expenses = 0.0;
    double income = 0.0;
    core_->bTransactionList_.getExpensesIncome(-1,expenses, income,  ignoreDate_, dtBegin_,dtEnd_, mmIniOptions::ignoreFutureTransactions_);

    mmGraphIncExpensesMonth gg;
    gg.init(income, expenses);
    gg.Generate(_("Income vs Expenses"));
    hb.addImage(gg.getOutputFileName());

	hb.startTable(wxT("60%"));
	hb.startTableRow();
	hb.addTableHeaderCell(_("Type"));
	hb.addTableHeaderCell(_("Amount"));
	hb.endTableRow();

    wxString incString;
    wxString expString;
    wxString diffString;

    mmex::formatDoubleToCurrency(expenses, expString);
    mmex::formatDoubleToCurrency(income, incString);
    mmex::formatDoubleToCurrency(income - expenses, diffString);

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

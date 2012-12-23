#include "reportincexpenses.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "mmgraphincexpensesmonth.h"
#include "reportbudget.h"

mmReportIncomeExpenses::mmReportIncomeExpenses(
    mmCoreDB* core, 
    bool ignoreDate, 
    const wxDateTime& dtBegin, 
    const wxDateTime& dtEnd,
    const wxString& title
) : mmPrintableBase(core), dtBegin_(dtBegin), dtEnd_(dtEnd), ignoreDate_(ignoreDate), title_(title)
{
    wxASSERT(dtBegin_ == dtBegin);
}

wxString mmReportIncomeExpenses::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, title_);

    mmCommonReportDetails dateDisplay(NULL);
    wxDateTime tBegin = dtBegin_;    // date needs to be adjusted
    dateDisplay.DisplayDateHeading(hb, tBegin.Add(wxDateSpan::Day()), dtEnd_, !ignoreDate_);

    hb.addLineBreak();

	hb.startCenter();

    core_->currencyList_.LoadBaseCurrencySettings(core_->dbInfoSettings_.get());

    double expenses = 0.0;
    double income = 0.0;
    core_->bTransactionList_.getExpensesIncome(core_, -1,expenses, income,  ignoreDate_, dtBegin_,dtEnd_, mmIniOptions::instance().ignoreFutureTransactions_);

	hb.startTable(wxT("75%"));
	hb.addTableHeaderRow(wxT(""), 2);
	hb.startTableRow();
	hb.startTableCell();

    mmGraphIncExpensesMonth gg;
    gg.init(income, expenses);
    gg.Generate(_("Income vs Expenses"));
    hb.addImage(gg.getOutputFileName());

	hb.endTableCell();
	hb.startTableCell();

	hb.startTable(wxT("95%"));
	hb.startTableRow();
	hb.addTableHeaderCell(_("Type"));
	hb.addTableHeaderCell(_("Amount"), true);
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
	hb.addTableCell(expString, true, true, true, (expenses > income ? wxT("RED") : wxT("")));
    hb.endTableRow();

    hb.addRowSeparator(2);
    hb.addTotalRow(_("Difference:"), 2, diffString);

    hb.endTable();

	hb.endTableCell();
	hb.endTableRow();
	hb.addRowSeparator(2);
    hb.endTable();

    hb.endCenter();

    hb.end();
    return hb.getHTMLText();
}

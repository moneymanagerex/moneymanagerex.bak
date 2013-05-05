#include "incexpenses.h"
#include "budget.h"

#include "../htmlbuilder.h"
#include "../util.h"
#include "../mmCurrencyFormatter.h"
#include "../mmgraphincexpensesmonth.h"

mmReportIncomeExpenses::mmReportIncomeExpenses(mmCoreDB* core, mmDateRange* date_range)
: mmPrintableBase(core)
, date_range_(date_range)
, title_(_("Income vs Expenses"))
{
}

wxString mmReportIncomeExpenses::title() const
{
	return this->title_ + " - " + date_range_->title();
}

wxString mmReportIncomeExpenses::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, this->title());

    hb.DisplayDateHeading(date_range_->start_date(), date_range_->end_date(), date_range_->is_with_date());

    hb.addLineBreak();

	hb.startCenter();

    core_->currencyList_.LoadBaseCurrencySettings();

    double expenses = 0.0;
    double income = 0.0;
    core_->bTransactionList_.getExpensesIncome(core_, -1, expenses, income, date_range_->is_with_date(), date_range_->start_date(), date_range_->end_date(), mmIniOptions::instance().ignoreFutureTransactions_);

	hb.startTable("75%");
	hb.addTableHeaderRow("", 2);
	hb.startTableRow();
	hb.startTableCell();

    mmGraphIncExpensesMonth gg;
    gg.init(income, expenses);
    gg.Generate(_("Income vs Expenses"));
    hb.addImage(gg.getOutputFileName());

	hb.endTableCell();
	hb.startTableCell();

	hb.startTable("95%");
	hb.startTableRow();
	hb.addTableHeaderCell(_("Type"));
	hb.addTableHeaderCell(_("Amount"), true);
	hb.endTableRow();

    wxString incString;
    wxString expString;
    wxString diffString;

     CurrencyFormatter::formatDoubleToCurrency(expenses, expString);
     CurrencyFormatter::formatDoubleToCurrency(income, incString);
     CurrencyFormatter::formatDoubleToCurrency(income - expenses, diffString);

	hb.startTableRow();
	hb.addTableCell(_("Income:"), false, true);
	hb.addTableCell(incString, true, false, true);
	hb.endTableRow();

	hb.startTableRow();
	hb.addTableCell(_("Expenses:"), false, true);
	hb.addTableCell(expString, true, true, true, (expenses > income ? "RED" : ""));
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

wxString mmReportIncomeExpensesAllTime::getHTMLText()
{
    int year = wxDateTime::Now().GetYear();
    core_->currencyList_.LoadBaseCurrencySettings();

    wxDateTime yearBegin(1, wxDateTime::Jan, year);
    wxDateTime yearEnd(31, wxDateTime::Dec, year);

    wxString yearStr = wxString::Format(_("Income vs Expenses for Year: %d"), year);

    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, yearStr );
    hb.addDateNow();

    hb.startCenter();

    hb.startTable("75%");
    hb.startTableRow();
    hb.addTableHeaderCell(_("Year"));
    hb.addTableHeaderCell(_("Month"));
    hb.addTableHeaderCell(_("Income"), true);
    hb.addTableHeaderCell(_("Expenses"), true);
    hb.addTableHeaderCell(_("Difference"), true);
    hb.endTableRow();

    double income = 0.0;
    double expenses = 0.0;
    double balance = 0.0;
        
    for (int yidx = 0; yidx < 12; yidx++)
    {
        wxDateTime dtBegin = wxDateTime(yearBegin).Add(wxDateSpan::Months(yidx));
        wxDateTime dtEnd = dtBegin.GetLastMonthDay();

        yearStr = wxString()<< dtBegin.GetYear();
        wxString monName = mmGetNiceMonthName(dtBegin.GetMonth());
            
        bool ignoreDate = false;
        income = 0.0;
        expenses = 0.0;
        core_->bTransactionList_.getExpensesIncome(core_, -1, expenses, income, ignoreDate, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);
            
        wxString actualExpStr;
        CurrencyFormatter::formatDoubleToCurrencyEdit(expenses, actualExpStr);
            
        wxString actualIncStr;
        CurrencyFormatter::formatDoubleToCurrencyEdit(income, actualIncStr);

        hb.startTableRow();
        hb.addTableCell(yearStr, false, true);
        hb.addTableCell(monName, false, true);
            
        balance = income - expenses;
        wxString actualBalStr;
        CurrencyFormatter::formatDoubleToCurrencyEdit(balance, actualBalStr);

        hb.addTableCell(actualIncStr, true, true, true);
        hb.addTableCell(actualExpStr, true, true, true);
        hb.addTableCell(actualBalStr, true, true, true, (balance < 0.0 ? "RED" : ""));

        hb.endTableRow();
    }

    wxDateTime today = wxDateTime::Now();
    wxDateTime prevYearEnd = wxDateTime(today);
    prevYearEnd.SetYear(year);
    prevYearEnd.SetMonth(wxDateTime::Dec);
    prevYearEnd.SetDay(31);
        
    wxDateTime dtEnd = prevYearEnd;
    wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
        
    expenses = 0.0;
    income = 0.0;
    core_->bTransactionList_.getExpensesIncome(core_, -1, expenses, income, false, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);
        
    wxString actualExpStr;
     CurrencyFormatter::formatDoubleToCurrencyEdit(expenses, actualExpStr);

    wxString actualIncStr;
     CurrencyFormatter::formatDoubleToCurrencyEdit(income, actualIncStr);

    balance = income - expenses;
    wxString actualBalStr;
     CurrencyFormatter::formatDoubleToCurrencyEdit(balance, actualBalStr);

    std::vector<wxString> data;
    data.push_back(actualIncStr);
    data.push_back(actualExpStr);
    data.push_back(actualBalStr);

    hb.addRowSeparator(5);
    hb.addTotalRow(_("Total:"), 5, data);

    hb.endTable();
    hb.endCenter();
    hb.end();

    return hb.getHTMLText();
}

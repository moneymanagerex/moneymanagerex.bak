#include "reportincexpesestime.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"

mmReportIncExpensesOverTime::mmReportIncExpensesOverTime(mmCoreDB* core, int year, int shift) : 
    mmPrintableBase(core),
    year_(year),
    shift_(shift)
{
}

wxString mmReportIncExpensesOverTime::getHTMLText()
{
    core_->currencyList_.LoadBaseCurrencySettings(core_->dbInfoSettings_.get());

    wxDateTime yearBegin(1, wxDateTime::Jan, year_);
    wxDateTime yearEnd(31, wxDateTime::Dec, year_);

    wxString yearStr = wxString::Format(_("Income vs Expenses for Year: %d"), year_);
    if (shift_ > 0)
    {
        yearStr = _("Income vs Expenses - Last 12 monthes");
        yearBegin.Subtract(wxDateSpan::Months(shift_));
        yearEnd.Subtract(wxDateSpan::Months(shift_));
    }

    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, yearStr );
    hb.addDateNow();

    hb.startCenter();

    hb.startTable(wxT("75%"));
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
        mmex::formatDoubleToCurrencyEdit(expenses, actualExpStr);
            
        wxString actualIncStr;
        mmex::formatDoubleToCurrencyEdit(income, actualIncStr);

        hb.startTableRow();
        hb.addTableCell(yearStr, false, true);
        hb.addTableCell(monName, false, true);
            
        balance = income - expenses;
        wxString actualBalStr;
        mmex::formatDoubleToCurrencyEdit(balance, actualBalStr);

        hb.addTableCell(actualIncStr, true, true, true);
        hb.addTableCell(actualExpStr, true, true, true);
        hb.addTableCell(actualBalStr, true, true, true, (balance < 0.0 ? wxT("RED") : wxT("")));

        hb.endTableRow();
    }

    wxDateTime today = wxDateTime::Now();
    wxDateTime prevYearEnd = wxDateTime(today);
    prevYearEnd.SetYear(year_);
    prevYearEnd.SetMonth(wxDateTime::Dec);
    prevYearEnd.SetDay(31);
        
    wxDateTime dtEnd = prevYearEnd;
    wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
        
    expenses = 0.0;
    income = 0.0;
    core_->bTransactionList_.getExpensesIncome(core_, -1, expenses, income, false, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);
        
    wxString actualExpStr;
    mmex::formatDoubleToCurrencyEdit(expenses, actualExpStr);

    wxString actualIncStr;
    mmex::formatDoubleToCurrencyEdit(income, actualIncStr);

    balance = income - expenses;
    wxString actualBalStr;
    mmex::formatDoubleToCurrencyEdit(balance, actualBalStr);

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

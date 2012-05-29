#include "reportincexpesestime.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"

mmReportIncExpensesOverTime::mmReportIncExpensesOverTime(mmCoreDB* core, int year) : 
    mmPrintableBase(core),
    year_(year)
{
}

wxString mmReportIncExpensesOverTime::getHTMLText()
{
    core_->loadBaseCurrencySettings();

    wxString yearStr = wxString::Format(wxT("%d"), year_);

    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, _("Income vs Expenses for Year: ") + yearStr );

    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(1, dt);
    hb.addLineBreak();
    hb.addLineBreak();

    wxDateTime yearBegin(1, wxDateTime::Jan, year_);
    wxDateTime yearEnd(31, wxDateTime::Dec, year_);

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
        wxString monName = mmGetNiceMonthName(yidx);

        wxDateTime dtBegin(1, (wxDateTime::Month)yidx, year_);
        wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_);
            
        bool ignoreDate = false;
        income = 0.0;
        expenses = 0.0;
        core_->getExpensesIncome(-1, expenses, income, ignoreDate, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);
            
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
    core_->getExpensesIncome(-1, expenses, income, false, dtBegin, dtEnd, mmIniOptions::instance().ignoreFutureTransactions_);
        
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

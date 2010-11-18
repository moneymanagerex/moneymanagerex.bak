///////////////////////////////////////////////////////////////////////////////
///     This class tabulates the income and expense for a 12 month period.
///     It is similar to the class: mmReportIncExpensesOverTime except that it
///     covers a financial year from 1 July YYYY to 30 June YYYY over 2 years.
///
///////////////////////////////////////////////////////////////////////////////
#include "reportincexpensesfinancialperiod.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"
#include "mmcoredb.h"

mmReportIncExpensesOverFinancialPeriod::mmReportIncExpensesOverFinancialPeriod(mmCoreDB* core, int year):
        core_(core),
        year_(year)
{
}

wxString mmReportIncExpensesOverFinancialPeriod::getHTMLText()
{
        core_->currencyList_.loadBaseCurrencySettings();

        wxString yearStr = wxString::Format(wxT("%d"), year_);
        wxString finYearStr = yearStr + wxT(" - ") + wxString::Format(wxT("%d"), (year_ + 1));

        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Income vs Expenses for Financial Year: ") + finYearStr );

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        wxDateTime yearBegin(1, wxDateTime::Jul, year_);
        wxDateTime yearEnd(30, wxDateTime::Jun, (year_ + 1));

        hb.startCenter();

        hb.startTable(wxT("50%"));
        hb.startTableRow();
        hb.addTableHeaderCell(_("Month"));
        hb.addTableHeaderCell(_("Income"));
        hb.addTableHeaderCell(_("Expenses"));
        hb.addTableHeaderCell(_("Difference"));
        hb.endTableRow();

        double income = 0.0;
        double expenses = 0.0;
        double balance = 0.0;

        int yidx = wxDateTime::Jun;
        for (int yearIndex = wxDateTime::Jan; yearIndex < wxDateTime::Dec + 1; yearIndex++)
        {
            yidx++;				// Set month to July
            if (yidx > wxDateTime::Dec)
            {
                yidx = wxDateTime::Jan;
                year_ ++ ;
                yearStr = wxString::Format(wxT("%d"), year_);
            }

            wxString monName = mmGetNiceMonthName(yidx) + wxT(" ") + yearStr;

            wxDateTime dtBegin(1, (wxDateTime::Month)yidx, year_);
            wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_);

            bool ignoreDate = false;
            income = 0.0;
            expenses = 0.0;
            core_->bTransactionList_.getExpensesIncome(-1, expenses, income,  ignoreDate, dtBegin, dtEnd);

            wxString actualExpStr;
            mmex::formatDoubleToCurrencyEdit(expenses, actualExpStr);

            wxString actualIncStr;
            mmex::formatDoubleToCurrencyEdit(income, actualIncStr);

            hb.startTableRow();
            hb.addTableCell(monName, false, true);

            balance = income - expenses;
            wxString actualBalStr;
            mmex::formatDoubleToCurrencyEdit(balance, actualBalStr);

            if (balance < 0.0)
            {
                hb.addTableCell(actualIncStr, true, true, true);
                hb.addTableCell(actualExpStr, true, true, true, wxT("#ff0000"));
                hb.addTableCell(actualBalStr, true, true, true, wxT("#ff0000"));
            }
            else
            {
                hb.addTableCell(actualIncStr, true, false, true);
                hb.addTableCell(actualExpStr, true, false, true);
                hb.addTableCell(actualBalStr, true, false, true);
            }
            hb.endTableRow();
        }

        wxDateTime today = wxDateTime::Now();
        wxDateTime prevYearEnd = wxDateTime(today);
        // Date setting needs to be in Day/Month/Year else error will occure in wxWidgets
        prevYearEnd.SetDay(30);
        prevYearEnd.SetMonth(wxDateTime::Jun);
        prevYearEnd.SetYear(year_);

        wxDateTime dtEnd = prevYearEnd;
        wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());

        expenses = 0.0;
        income = 0.0;
        core_->bTransactionList_.getExpensesIncome(-1, expenses, income,  false, dtBegin, dtEnd);

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

        hb.addRowSeparator(4);
        hb.addTotalRow(_("Total"), 4, data);

        hb.endTable();

        hb.endCenter();

        hb.end();
        return hb.getHTMLText();
}

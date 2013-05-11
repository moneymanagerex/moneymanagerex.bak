/*************************************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Modified: Copyright (C) 2010 Stefano Giorgio

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

 Purpose:
 This class tabulates the income and expense for a 12 month period.
 It is similar to the class: mmReportIncExpensesOverTime except that it
 covers a financial year period defined by the user.
 Typically from 1 July YYYY to 30 June YYYY over 2 years but can be any
 period within year.
*************************************************************************/

#include "incexpensesfinancialperiod.h"
#include "budget.h"

#include "../htmlbuilder.h"
#include "../mmex.h"

mmReportIncExpensesOverFinancialPeriod::mmReportIncExpensesOverFinancialPeriod(mmCoreDB* core, mmDateRange* date_range)
: mmPrintableBase(core)
, date_range_(date_range)
{}

wxString mmReportIncExpensesOverFinancialPeriod::getHTMLText()
{
    date_range_ = new mmLast12Months();
    std::map<int, std::pair<double, double> > incomeExpensesStats;
    core_->bTransactionList_.getExpensesIncomeStats(incomeExpensesStats
        , date_range_
        , -1
    );

    core_->currencyList_.LoadBaseCurrencySettings();

    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, wxString::Format(_("Income vs Expenses for Financial Year: %s"), date_range_->title()) );

    hb.DisplayDateHeading(date_range_->start_date(), date_range_->start_date(), true);

    hb.startCenter();

    hb.startTable("75%");
    hb.startTableRow();
    hb.addTableHeaderCell(_("Year"));
    hb.addTableHeaderCell(_("Month"));
    hb.addTableHeaderCell(_("Income"), true);
    hb.addTableHeaderCell(_("Expenses"), true);
    hb.addTableHeaderCell(_("Difference"), true);
    hb.endTableRow();

    double tIncome = 0.0, tExpenses = 0.0;

    for (const auto &stats : incomeExpensesStats)
    {
        double income = 0, expenses = 0;
        wxDateTime dummy = wxDateTime::Now();
        wxString monName = wxGetTranslation(wxDateTime::GetMonthName(wxDateTime::Month(stats.first%100)));
        wxString yearStr = wxString()<<((int)stats.first/100);

        hb.startTableRow();
        hb.addTableCell(yearStr, false, true);
        hb.addTableCell(monName, false, true);

        income = stats.second.first;
        expenses = stats.second.second;
        tIncome += income;
        tExpenses += expenses;
        hb.addMoneyCell(income);
        hb.addMoneyCell(expenses);
        hb.addMoneyCell(income - expenses);

        hb.endTableRow();
    }

    // Now we get the totals for the financial year period

    std::vector<double> data;
    data.push_back(tIncome);
    data.push_back(tExpenses);
    data.push_back(tIncome - tExpenses);

    hb.addRowSeparator(5);
    hb.addTotalRow(_("Total:"), 5, data);

    hb.endTable();
    hb.endCenter();
    hb.end();

    // restore year value for printing purposes.
    //year_ = printYear_;
    return hb.getHTMLText();
}

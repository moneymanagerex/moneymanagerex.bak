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

#include "reportincexpensesfinancialperiod.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"
#include "reportbudget.h"

mmReportIncExpensesOverFinancialPeriod::mmReportIncExpensesOverFinancialPeriod(mmGUIFrame* frame, mmCoreDB* core, int year):
    frame_(frame),
    mmPrintableBase(core),
    year_(year),
    printYear_(year)
{
}

wxString mmReportIncExpensesOverFinancialPeriod::getHTMLText()
{
    core_->loadBaseCurrencySettings();

    wxString yearStr = wxString::Format(wxT("%d"), year_);
    wxString finYearStr = yearStr + wxT(" - ") + wxString::Format(wxT("%d"), (year_ + 1));

    wxDateTime sofy = wxDateTime( frame_->getUserDefinedFinancialYear() );
    int startDay = sofy.GetDay();

    wxDateTime yearBegin(sofy.GetDay(), sofy.GetMonth(), year_);
    wxDateTime yearEnd(sofy.GetDay(), sofy.GetMonth(), (year_ + 1));
    yearEnd.Subtract(wxDateSpan::Day());

    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(3, _("Income vs Expenses for Financial Year: ") + finYearStr );

    mmCommonReportDetails dateDisplay(NULL);
    dateDisplay.DisplayDateHeading(hb, yearBegin, yearEnd, true);

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

    int yidx = sofy.GetMonth() -1 ;
    int monthCorrection = 1;    
    if ( sofy.GetDay() != 1)    // allow to display 13 months - first and last are part months
        monthCorrection ++;     

    int dayStart = 1;           // correct day for when not first of month
    for (int yearIndex = wxDateTime::Jan; yearIndex < wxDateTime::Dec + monthCorrection; yearIndex++)
    {
        yidx++;
        if (yidx > wxDateTime::Dec)
        {
            yidx = wxDateTime::Jan;
            year_ ++ ;
            yearStr = wxString::Format(wxT("%d"), year_);
        }

        wxString monName = mmGetNiceMonthName(yidx);

        if (yearIndex == 0)
            dayStart = startDay;
        else
            dayStart = 1;

        wxDateTime dtBegin(dayStart, (wxDateTime::Month)yidx, year_);
        wxDateTime dtEnd;
        if (yearIndex > 11)
            dtEnd = yearEnd;
        else
            dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_);

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

    // Now we get the totals for the financial year period
    expenses = 0.0;
    income = 0.0;
    core_->getExpensesIncome(-1, expenses, income,  false, yearBegin, yearEnd, mmIniOptions::instance().ignoreFutureTransactions_);

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

    // restore year value for printing purposes.
    year_ = printYear_;
    return hb.getHTMLText();
}

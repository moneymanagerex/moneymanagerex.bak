/*************************************************************************
 Copyright (C) 2012 Stefano Giorgio

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
 *************************************************************************/
#include "reportbudget.h"
#include "mmex.h"
#include "budgetingpanel.h"
#include "htmlbuilder.h"

mmReportBudget::mmReportBudget(mmGUIFrame* mainFrame)
    : mainFrame_(mainFrame)
{}

/**************************************************************************************
 This will set the budget estimate for a single day.
 This can then be used for the month or year as necessary.
 ***************************************************************************************/
void mmReportBudget::setBudgetDailyEstimateAmount(mmBudgetEntryHolder& budEntry, int month)
{
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    int numDays = 365;

    if (budEntry.period_ == wxT("Monthly")) {
        numDays = daysInMonth[month];

    } else if (budEntry.period_ == wxT("Yearly")) {
        numDays = 365;

    } else if (budEntry.period_ == wxT("Weekly")) {
        numDays = 7;

    } else if (budEntry.period_ == wxT("Bi-Weekly")) {
        numDays = 14;

    } else if (budEntry.period_ == wxT("Bi-Monthly")) {
        numDays = 61;

    } else if (budEntry.period_ == wxT("Quarterly")) {
        numDays = 92;

    } else if (budEntry.period_ == wxT("Half-Yearly")) {
        numDays = 183;

    } else if (budEntry.period_ == wxT("Daily")) {
        numDays = 1;

    } else {
        wxASSERT(true);
    }

    budEntry.estimated_ = budEntry.amt_ /numDays;
}

void mmReportBudget::setBudgetEstimate(mmBudgetEntryHolder& budEntry, bool monthBudget, wxDateTime beginDate, wxDateTime endDate)
{
    if (monthBudget) {
        setBudgetDailyEstimateAmount(budEntry, beginDate.GetMonth());
        int days = endDate.GetDay();
        budEntry.estimated_ = budEntry.estimated_*days;

    } else {
        setBudgetDailyEstimateAmount(budEntry);
        budEntry.estimated_ = budEntry.estimated_* 365;
    }
}

void mmReportBudget::initBudgetEntryFields(mmBudgetEntryHolder& budEntry)
{
    budEntry.subcategID_ = -1;
    budEntry.subCatStr_  = wxT("");
    budEntry.amt_ = 0.0;
    budEntry.period_ = wxT("None");
    budEntry.estimated_ = 0.0;
    budEntry.estimatedStr_ = wxT("0.0");
    budEntry.actual_ = 0.0;
    budEntry.actualStr_ = wxT("0.0");
}

void mmReportBudget::DisplayDateHeading(mmHTMLBuilder& hb, wxDateTime startYear, wxDateTime endYear)
{
    wxString dt = wxString() 
        << _("Today's Date: ") << wxT("<b>") << mmGetNiceDateString(wxDateTime::Now())
        << wxT("<br>")
        << _("From: ") << wxT("<b>") << mmGetNiceDateSimpleString(startYear) <<  wxT("</b> ") 
        << _("to: ") 
        << wxT ("<b>") << mmGetNiceDateSimpleString(endYear) << wxT ("</b>");

    hb.addHeader(7, dt);
    hb.addLineBreak();
    hb.addLineBreak();
}

void mmReportBudget::SetDateToEndOfMonth(int month, wxDateTime& date)
{
    date.SetDay(28);
    date.SetMonth((wxDateTime::Month)month);
    date.SetToLastMonthDay();
}

void mmReportBudget::SetDateToEndOfYear(int day, int month, wxDateTime& date, bool isEndDate)
{
    date.SetDay(day);
    date.SetMonth((wxDateTime::Month)month);
    if (isEndDate)
    {
        date.Subtract(wxDateSpan::Day());
        date.Add(wxDateSpan::Year());
    }
}

void mmReportBudget::SetBudgetMonth(wxString budgetYearStr, wxDateTime& startDate, wxDateTime& endDate)
{
    wxStringTokenizer tz(budgetYearStr,wxT("-")); 
    wxString yearStr = tz.GetNextToken();
    wxString monthStr = tz.GetNextToken();
    int month = wxAtoi(monthStr) - 1;
    startDate.SetMonth((wxDateTime::Month)month);
    SetDateToEndOfMonth(month,endDate);
}

void mmReportBudget::GetFinancialYearValues(int& day, int& month)
{
    day = wxAtoi(mmOptions::financialYearStartDayString_);
    month = wxAtoi(mmOptions::financialYearStartMonthString_) - 1;
    if ( (day > 28) && (month == wxDateTime::Feb) )
        day = 28;
    else if ( (day > 30) && (month == wxDateTime::Sep) ||
              (day > 30) && (month == wxDateTime::Apr) ||
              (day > 30) && (month == wxDateTime::Jun) ||
              (day > 30) && (month == wxDateTime::Nov) )
    {
        day = 30;
    }
}

wxString mmReportBudget::AdjustYearValues(int& day, int& month, long year, wxString yearStr)
{
    if ((yearStr.length() < 5)) {
        if ( mainFrame_->budgetFinancialYears() ) {
            GetFinancialYearValues(day, month);
            yearStr = wxString() << _("Financial Year: ") << yearStr << wxT(" - ") << (year + 1);
        } else
            yearStr = wxString() << _("Year: ") << yearStr;
    } else {
        yearStr = wxString() << _("Month: ") << yearStr;
    }

    return yearStr;
}

void mmReportBudget::AdjustYearValues(int& day, int& month, wxDateTime& year)
{
    if ( mainFrame_->budgetFinancialYears() ) {
        GetFinancialYearValues(day, month);
        SetDateToEndOfYear(day, month, year, false);
	}
}

void mmReportBudget::AdjustDateForEndFinancialYear(wxDateTime& date)
{
    if ( mainFrame_->budgetFinancialYears() ) 
    {
		int day, month;
        GetFinancialYearValues(day, month);
        SetDateToEndOfYear(day, month, date);
    }
}

/*************************************************************************
 This is a helper class to allow access to common methods.
 *************************************************************************/
mmCommonReportDetails::mmCommonReportDetails(mmGUIFrame* mainFrame)
    :mmReportBudget(mainFrame)
{
}

wxString mmCommonReportDetails::getHTMLText()
{
    // This method should never be called;
    wxASSERT(true);

    return wxEmptyString;
}

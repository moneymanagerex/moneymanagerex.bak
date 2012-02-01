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
#ifndef _MM_EX_REPORTBUDGETING_H_
#define _MM_EX_REPORTBUDGETING_H_

#include "reportbase.h"

class mmGUIFrame;
struct mmBudgetEntryHolder;
class mmHTMLBuilder;
class wxDateTime;

/*************************************************************************
 Class: mmReportBudget : This class is a base class for Budget Reports.
        This class should not be used to create objects.
 *************************************************************************/
class mmReportBudget : public mmPrintableBase 
{
public:
    mmReportBudget(mmGUIFrame* mainFrame, mmCoreDB* core = NULL);

    /// Initialise index and secondary fields for a budget entry
    void initBudgetEntryFields(mmBudgetEntryHolder& budEntry, int id = -1);
  
    /// Set budget estimate for a day based on the required period
    void setBudgetDailyEstimateAmount(mmBudgetEntryHolder& budEntry, int month = 5 /*wxDateTime::Jun*/);

    /// Set budget estimate for the appropriate prriod
    void setBudgetEstimate(mmBudgetEntryHolder& budEntry, bool monthBudget, wxDateTime beginDate, wxDateTime endDate);
    
    /// Used in reports to display today's date
    void DisplayDateHeading(mmHTMLBuilder& hb, wxDateTime startYear, wxDateTime endYear, bool withDateRange = true);

    /// Returns correct values for day and month, adjusted to financial year if required.
	void AdjustYearValues(int& day, int& month, wxDateTime& year);

    /// Returns correct values for day and month, adjusted to financial year if required.
	/// Also returns a heading string for Month or Year reports.
    wxString AdjustYearValues(int& day, int& month, long year, wxString yearStr);

    /// Sets date to end of financial year if required by user.
    void AdjustDateForEndFinancialYear(wxDateTime& date);

    /// Return day and month values to user defined financial year.
    void GetFinancialYearValues(int& day, int& month);

    /// Advance the given date to the end of the current month.
    void SetDateToEndOfMonth(int month, wxDateTime& date);

    /// Advance the given date by one year.
    void SetDateToEndOfYear(int day, int month, wxDateTime& date, bool isEndDate = true);
	
	/// sets the start and end dates for a budget month
    void SetBudgetMonth(wxString budgetYearStr, wxDateTime& startDate, wxDateTime& endDate);

protected:
    mmGUIFrame* mainFrame_;
};


/*************************************************************************
 Class: mmCommonReportDetails :
        Allows access to common methods for Reports.
 *************************************************************************/
class mmCommonReportDetails : public mmReportBudget 
{
public:
    mmCommonReportDetails(mmGUIFrame* mainFrame);

    wxString getHTMLText();
};

#endif // _MM_EX_REPORTBUDGETING_H_

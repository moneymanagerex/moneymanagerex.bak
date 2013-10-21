/*******************************************************
 Copyright (C) 2006-2012

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
 ********************************************************/

#ifndef _MM_EX_REPORTINCEXP_H_
#define _MM_EX_REPORTINCEXP_H_

#include "reportbase.h"
#include "mmDateRange.h"

class mmReportIncomeExpenses : public mmPrintableBase
{
public:
    mmReportIncomeExpenses(mmDateRange* date_range = new mmCurrentMonth());
    virtual ~mmReportIncomeExpenses();
    wxString getHTMLText();

protected:
    mmDateRange* date_range_;
    wxString title_;
    virtual wxString title() const;

};

class mmReportIncomeExpensesAllTime: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesAllTime(): mmReportIncomeExpenses(new mmAllTime())
    {
    }
};

class mmReportIncomeExpensesCurrentMonth: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesCurrentMonth(): mmReportIncomeExpenses(new mmCurrentMonth())
    {
    }
};

class mmReportIncomeExpensesCurrentMonthToDate: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesCurrentMonthToDate(): mmReportIncomeExpenses(new mmCurrentMonthToDate())
    {
    }
};

class mmReportIncomeExpensesLastMonth: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesLastMonth(): mmReportIncomeExpenses(new mmLastMonth())
    {
    }
};

class mmReportIncomeExpensesLast30Days: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesLast30Days(): mmReportIncomeExpenses(new mmLast30Days())
    {
    }
};

/////////////////////////////////////////////////////////////////////////////////////
class mmReportIncomeExpensesMonthly: public mmPrintableBase
{
public:
    mmReportIncomeExpensesMonthly(int day_ = 1
                                         , int month_ = 1
                                                 , mmDateRange* date_range = new mmCurrentMonth());
    virtual ~mmReportIncomeExpensesMonthly();
    wxString getHTMLText();
protected:

    mmDateRange* date_range_;
    int day_;
    int month_;
    wxString title_;
    virtual wxString title() const
    {
        return wxString::Format(title_, date_range_->title());
    }
};

class mmReportIncomeExpensesCurrentYear: public mmReportIncomeExpensesMonthly
{
public:
    mmReportIncomeExpensesCurrentYear(): mmReportIncomeExpensesMonthly(1, 1, new mmCurrentYear())
    {
    }
};

class mmReportIncomeExpensesLastYear: public mmReportIncomeExpensesMonthly
{
public:
    mmReportIncomeExpensesLastYear(): mmReportIncomeExpensesMonthly(1, 1, new mmLastYear())
    {
    }
};

class mmReportIncomeExpensesCurrentFinancialYear: public mmReportIncomeExpensesMonthly
{
public:
    mmReportIncomeExpensesCurrentFinancialYear(const int day, const int month)
        : mmReportIncomeExpensesMonthly(day, month, new mmCurrentFinancialYear(day, month))
    {
    }
};

class mmReportIncomeExpensesLastFinancialYear: public mmReportIncomeExpensesMonthly
{
public:
    mmReportIncomeExpensesLastFinancialYear(const int day, const int month)
        : mmReportIncomeExpensesMonthly(day, month, new mmLastFinancialYear(day, month))
    {
    }
};

#endif // _MM_EX_REPORTINCEXP_H_


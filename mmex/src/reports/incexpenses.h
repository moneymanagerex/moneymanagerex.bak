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

#include "../reportbase.h"
#include "mmDateRange.h"

class mmReportIncomeExpenses : public mmPrintableBase 
{
public:
    mmReportIncomeExpenses(mmCoreDB* core, mmDateRange* date_range = new mmAllTime());

    wxString getHTMLText();

protected:
    mmDateRange* date_range_;
    wxString title_;
	virtual wxString title() const;

};

class mmReportIncomeExpensesAllTime: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesAllTime(mmCoreDB* core): mmReportIncomeExpenses(core, new mmAllTime())
    {}
    wxString getHTMLText();
};

class mmReportIncomeExpensesCurrentMonth: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesCurrentMonth(mmCoreDB* core): mmReportIncomeExpenses(core, new mmCurrentMonth())
    {
    }
};

class mmReportIncomeExpensesCurrentMonthToDate: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesCurrentMonthToDate(mmCoreDB* core): mmReportIncomeExpenses(core, new mmCurrentMonthToDate())
    {
    }
};

class mmReportIncomeExpensesLastMonth: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesLastMonth(mmCoreDB* core): mmReportIncomeExpenses(core, new mmLastMonth())
    {
    }
};

class mmReportIncomeExpensesCurrentYear: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesCurrentYear(mmCoreDB* core): mmReportIncomeExpenses(core, new mmCurrentYear())
    {
    }
};

class mmReportIncomeExpensesLastYear: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesLastYear(mmCoreDB* core): mmReportIncomeExpenses(core, new mmLastYear())
    {
    }
};

class mmReportIncomeExpensesCurrentFinancialYear: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesCurrentFinancialYear(mmCoreDB* core, int day, int month): mmReportIncomeExpenses(core, new mmCurrentFinancialYear(day, month))
    {
    }
};

class mmReportIncomeExpensesLastFinancialYear: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesLastFinancialYear(mmCoreDB* core, int day, int month): mmReportIncomeExpenses(core, new mmLastFinancialYear(day, month))
    {
    }
};

class mmReportIncomeExpensesLast30Days: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesLast30Days(mmCoreDB* core): mmReportIncomeExpenses(core, new mmLast30Days())
    {
    }
};

#endif // _MM_EX_REPORTINCEXP_H_


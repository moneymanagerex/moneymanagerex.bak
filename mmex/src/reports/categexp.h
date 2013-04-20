/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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

#ifndef _MM_EX_REPORTCATEGEXP_H_
#define _MM_EX_REPORTCATEGEXP_H_

#include "../reportbase.h"
#include "mmDateRange.h"

class mmReportCategoryExpenses : public mmPrintableBase 
{
public:
    mmReportCategoryExpenses(mmCoreDB* core
        , bool ignoreDate, mmDateRange* date_range, const wxString& title, int type);

    wxString getHTMLText();

protected:
    mmDateRange* date_range_;
    bool ignoreDate_;
    bool ignoreFutureDate_;
    wxString title_;
    int type_;
};

class mmReportCategoryExpensesGoes: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoes(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonthToDate(), _("Where the Money Goes"), 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentMonth: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesCurrentMonth(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonth(), _("Where the Money Goes - Current Month"), 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentMonthToDate: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesCurrentMonthToDate(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonthToDate(), _("Where the Money Goes - Current Month to Date"), 2)
    {}
};

class mmReportCategoryExpensesGoesLastMonth: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesLastMonth(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastMonth(), _("Where the Money Goes - Last Month"), 2)
    {}
};

class mmReportCategoryExpensesGoesLast30Days: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesLast30Days(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLast30Days(), _("Where the Money Goes - Last 30 Days"), 2)
    {}
};

class mmReportCategoryExpensesGoesLastYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesLastYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastYear(),  _("Where the Money Goes - Last Year"), 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesCurrentYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentYear(), _("Where the Money Goes - Current Year"), 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentYearToDate: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesCurrentYearToDate(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentYearToDate(), _("Where the Money Goes - Current Year to Date"), 2)
    {}
};

class mmReportCategoryExpensesGoesLastFinancialYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesLastFinancialYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastFinancialYear(), _("Where the Money Goes - Last Financial Year"), 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentFinancialYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoesCurrentFinancialYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentFinancialYear(), _("Where the Money Goes - Current Financial Year"), 2)
    {}
};

class mmReportCategoryExpensesComes: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComes(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonthToDate()
        , _("Where the Money Comes From"), 1)
    {}
};

class mmReportCategoryExpensesComesCurrentMonth: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesCurrentMonth(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new  mmCurrentMonth(), _("Where the Money Comes From - Current Month"), 1)
    {}
};

class mmReportCategoryExpensesComesCurrentMonthToDate: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesCurrentMonthToDate(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonthToDate(), _("Where the Money Comes From - Current Month to Date"), 1)
    {}
};

class mmReportCategoryExpensesComesLastMonth: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesLastMonth(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastMonth(),  _("Where the Money Comes From - Last Month"), 1)
    {}
};

class mmReportCategoryExpensesComesLast30Days: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesLast30Days(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLast30Days(), _("Where the Money Comes From - Last Month"),1)
    {}
};

class mmReportCategoryExpensesComesLastYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesLastYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastYear(), _("Where the Money Comes From - Last Year"), 1)
    {}
};

class mmReportCategoryExpensesComesCurrentYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesCurrentYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentYear(), _("Where the Money Comes From - Current Year"), 1)
    {}
};

class mmReportCategoryExpensesComesCurrentYearToDate: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesCurrentYearToDate(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentYearToDate(), _("Where the Money Comes From - Current Year to Date"), 1)
    {}
};

class mmReportCategoryExpensesComesLastFinancialYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesLastFinancialYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastFinancialYear(), _("Where the Money Comes From - Last Financial Year"), 1)
    {}
};

class mmReportCategoryExpensesComesCurrentFinancialYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComesCurrentFinancialYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentFinancialYear(), _("Where the Money Comes From - Current Financial Year"), 1)
    {}
};

class mmReportCategoryExpensesCategories: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategories(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonthToDate()
        , _("Where the Money Categories From"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesCurrentMonth: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesCurrentMonth(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonth(), _("Where the Money Categories From - Current Month"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesCurrentMonthToDate: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesCurrentMonthToDate(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentMonthToDate(), _("Where the Money Categories From - Current Month to Date"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesLastMonth: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesLastMonth(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastMonth(), _("Where the Money Categories From - Last Month"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesLast30Days: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesLast30Days(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLast30Days(), _("Where the Money Categories From - Last 30 Days"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesLastYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesLastYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastYear(), _("Where the Money Categories From - Last Year"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesCurrentYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesCurrentYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentYear(), _("Where the Money Categories From - Current Year"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesCurrentYearToDate: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesCurrentYearToDate(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentYearToDate(), _("Where the Money Categories From - Current Year to Date"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesLastFinancialYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesLastFinancialYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmLastFinancialYear(), _("Where the Money Categories From - Last Financial Year"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesCurrentFinancialYear: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategoriesCurrentFinancialYear(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, new mmCurrentFinancialYear(), _("Where the Money Categories From - Last Financial Year"), 0)
    {}
};

#endif // _MM_EX_REPORTCATEGEXP_H_

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
        , mmDateRange* date_range, const wxString& title, int type);

    wxString getHTMLText();

public:
    enum TYPE { NONE = 0, GOES, COME, CATEGORY};

protected:
    mmDateRange* date_range_;
    bool ignoreFutureDate_;
    wxString title_;
    int type_;
};

class mmReportCategoryExpensesGoes: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoes(mmCoreDB* core, mmDateRange* date_range = new mmAllTime()
        , const wxString& title = _("Where the Money Goes"))
        : mmReportCategoryExpenses(core, date_range, title, 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentMonth: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentMonth(mmCoreDB* core): mmReportCategoryExpensesGoes(core
        , new mmCurrentMonth(), _("Where the Money Goes - Current Month"))
    {}
};

class mmReportCategoryExpensesGoesCurrentMonthToDate: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentMonthToDate(mmCoreDB* core): mmReportCategoryExpensesGoes(core
        , new mmCurrentMonthToDate(), _("Where the Money Goes - Current Month to Date"))
    {}
};

class mmReportCategoryExpensesGoesLastMonth: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastMonth(mmCoreDB* core): mmReportCategoryExpensesGoes(core
        , new mmLastMonth(), _("Where the Money Goes - Last Month"))
    {}
};

class mmReportCategoryExpensesGoesLast30Days: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLast30Days(mmCoreDB* core): mmReportCategoryExpensesGoes(core
        , new mmLast30Days(), _("Where the Money Goes - Last 30 Days"))
    {}
};

class mmReportCategoryExpensesGoesLastYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core
        , new mmLastYear(),  _("Where the Money Goes - Last Year"))
    {}
};

class mmReportCategoryExpensesGoesCurrentYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core
        , new mmCurrentYear(), _("Where the Money Goes - Current Year"))
    {}
};

class mmReportCategoryExpensesGoesCurrentYearToDate: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentYearToDate(mmCoreDB* core): mmReportCategoryExpensesGoes(core
        , new mmCurrentYearToDate(), _("Where the Money Goes - Current Year to Date"))
    {}
};

class mmReportCategoryExpensesGoesLastFinancialYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastFinancialYear(mmCoreDB* core, int day, int month): mmReportCategoryExpensesGoes(core
        , new mmLastFinancialYear(day, month), _("Where the Money Goes - Last Financial Year"))
    {}
};

class mmReportCategoryExpensesGoesCurrentFinancialYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentFinancialYear(mmCoreDB* core, int day, int month): mmReportCategoryExpensesGoes(core
        , new mmCurrentFinancialYear(day, month), _("Where the Money Goes - Current Financial Year"))
    {}
};

class mmReportCategoryExpensesComes: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComes(mmCoreDB* core, mmDateRange* date_range = new mmAllTime(), const wxString& title = _("Where the Money Comes From"))
        : mmReportCategoryExpenses(core, date_range, title, 1)
    {}
};

class mmReportCategoryExpensesComesCurrentMonth: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentMonth(mmCoreDB* core): mmReportCategoryExpensesComes(core
        , new  mmCurrentMonth(), _("Where the Money Comes From - Current Month"))
    {}
};

class mmReportCategoryExpensesComesCurrentMonthToDate: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentMonthToDate(mmCoreDB* core): mmReportCategoryExpensesComes(core
        , new mmCurrentMonthToDate(), _("Where the Money Comes From - Current Month to Date"))
    {}
};

class mmReportCategoryExpensesComesLastMonth: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLastMonth(mmCoreDB* core): mmReportCategoryExpensesComes(core
        , new mmLastMonth(),  _("Where the Money Comes From - Last Month"))
    {}
};

class mmReportCategoryExpensesComesLast30Days: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLast30Days(mmCoreDB* core): mmReportCategoryExpensesComes(core
        , new mmLast30Days(), _("Where the Money Comes From - Last Month"))
    {}
};

class mmReportCategoryExpensesComesLastYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLastYear(mmCoreDB* core): mmReportCategoryExpensesComes(core
        , new mmLastYear(), _("Where the Money Comes From - Last Year"))
    {}
};

class mmReportCategoryExpensesComesCurrentYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesComes(core
        , new mmCurrentYear(), _("Where the Money Comes From - Current Year"))
    {}
};

class mmReportCategoryExpensesComesCurrentYearToDate: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentYearToDate(mmCoreDB* core): mmReportCategoryExpensesComes(core
        , new mmCurrentYearToDate(), _("Where the Money Comes From - Current Year to Date"))
    {}
};

class mmReportCategoryExpensesComesLastFinancialYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLastFinancialYear(mmCoreDB* core, int day, int month): mmReportCategoryExpensesComes(core
        , new mmLastFinancialYear(day, month), _("Where the Money Comes From - Last Financial Year"))
    {}
};

class mmReportCategoryExpensesComesCurrentFinancialYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentFinancialYear(mmCoreDB* core, int day, int month): mmReportCategoryExpensesComes(core
        , new mmCurrentFinancialYear(day, month), _("Where the Money Comes From - Current Financial Year"))
    {}
};

class mmReportCategoryExpensesCategories: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategories(mmCoreDB* core, mmDateRange* date_range = new mmAllTime(), const wxString& title = _("Categories"))
    : mmReportCategoryExpenses(core, date_range, title, 0)
    {}
};

class mmReportCategoryExpensesCategoriesCurrentMonth: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentMonth(mmCoreDB* core): mmReportCategoryExpensesCategories(core
        , new mmCurrentMonth(), _("Categories - Current Month"))
    {}
};

class mmReportCategoryExpensesCategoriesCurrentMonthToDate: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentMonthToDate(mmCoreDB* core): mmReportCategoryExpensesCategories(core
        , new mmCurrentMonthToDate(), _("Categories - Current Month to Date"))
    {}
};

class mmReportCategoryExpensesCategoriesLastMonth: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLastMonth(mmCoreDB* core): mmReportCategoryExpensesCategories(core
        , new mmLastMonth(), _("Categories - Last Month"))
    {}
};

class mmReportCategoryExpensesCategoriesLast30Days: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLast30Days(mmCoreDB* core): mmReportCategoryExpensesCategories(core
        , new mmLast30Days(), _("Categories - Last 30 Days"))
    {}
};

class mmReportCategoryExpensesCategoriesLastYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLastYear(mmCoreDB* core): mmReportCategoryExpensesCategories(core
        , new mmLastYear(), _("Categories - Last Year"))
    {}
};

class mmReportCategoryExpensesCategoriesCurrentYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesCategories(core
        , new mmCurrentYear(), _("Categories - Current Year"))
    {}
};

class mmReportCategoryExpensesCategoriesCurrentYearToDate: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentYearToDate(mmCoreDB* core): mmReportCategoryExpensesCategories(core
        , new mmCurrentYearToDate(), _("Categories - Current Year to Date"))
    {}
};

class mmReportCategoryExpensesCategoriesLastFinancialYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLastFinancialYear(mmCoreDB* core, int day, int month): mmReportCategoryExpensesCategories(core
        , new mmLastFinancialYear(day, month), _("Categories - Last Financial Year"))
    {}
};

class mmReportCategoryExpensesCategoriesCurrentFinancialYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentFinancialYear(mmCoreDB* core, int day, int month): mmReportCategoryExpensesCategories(core
        , new mmCurrentFinancialYear(day, month), _("Categories - Last Financial Year"))
    {}
};

#endif // _MM_EX_REPORTCATEGEXP_H_

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

class mmReportCategoryExpenses : public mmPrintableBase 
{
public:
    mmReportCategoryExpenses(mmCoreDB* core
        , bool ignoreDate, const wxDateTime& dtBegin, const wxDateTime& dtEnd, const wxString& title, int type);

    wxString getHTMLText();

protected:
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
    wxString title_;
    int type_;
};

class mmReportCategoryExpensesGoes: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesGoes(mmCoreDB* core): mmReportCategoryExpenses(core, false, wxDateTime::Now(), wxDateTime(), _("Where the Money Goes"), 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentMonth: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentMonth(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Current Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesGoesLastMonth: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastMonth(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesGoesLast30Days: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLast30Days(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesGoesLastYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesGoesCurrentYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesGoesLastFinancialYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastFinancialYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesGoesCurrentFinancialYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentFinancialYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesComes: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComes(mmCoreDB* core): mmReportCategoryExpenses(core, false, wxDateTime::Now(), wxDateTime(), _("Where the Money Comes From"), 1)
    {}
};

class mmReportCategoryExpensesComesCurrentMonth: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentMonth(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Current Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesComesLastMonth: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLastMonth(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesComesLast30Days: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLast30Days(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesComesLastYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLastYear(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesComesCurrentYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesComesLastFinancialYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLastFinancialYear(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesComesCurrentFinancialYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentFinancialYear(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

#endif // _MM_EX_REPORTCATEGEXP_H_

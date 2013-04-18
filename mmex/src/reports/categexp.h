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
#include "../mmex.h"

class mmReportCategoryExpenses : public mmPrintableBase 
{
public:
    mmReportCategoryExpenses(mmCoreDB* core
        , bool ignoreDate, const wxDateTime& dtBegin
        , const wxDateTime& dtEnd, const wxString& title, int type);

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
    mmReportCategoryExpensesGoes(mmCoreDB* core): mmReportCategoryExpenses(core
        , false, wxDateTime::Now(), wxDateTime::Now(), _("Where the Money Goes"), 2)
    {}
};

class mmReportCategoryExpensesGoesCurrentMonth: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentMonth(mmCoreDB* core);
};

class mmReportCategoryExpensesGoesLastMonth: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastMonth(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Month");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Months(1)).SetDay(1).GetDateOnly();
        this->dtEnd_ = wxDateTime(dtBegin_).GetLastMonthDay();
    }
};

class mmReportCategoryExpensesGoesLast30Days: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLast30Days(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last 30 Days");
        this->dtEnd_ = wxDateTime::Now().GetDateOnly();
        this->dtBegin_ = wxDateTime(dtEnd_).Subtract(wxDateSpan::Months(1)).Add(wxDateSpan::Days(1));
    }
};

class mmReportCategoryExpensesGoesLastYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->title_ = _("Where the Money Goes - Last Year");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Years(1))
            .SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();
        this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);
    }
};

class mmReportCategoryExpensesGoesCurrentYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->dtBegin_ = wxDateTime::Now().SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Goes - Current Year to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();  
        }
        else
        {
            this->title_ = _("Where the Money Goes - Current Year");
            this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);  
        }
    }
};

class mmReportCategoryExpensesGoesLastFinancialYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesLastFinancialYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Years(1))
            .SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Goes - Last Financial Year to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();  
        }
        else
        {
            this->title_ = _("Where the Money Goes - Last Financial Year");
            this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);  
        }
    }
};

class mmReportCategoryExpensesGoesCurrentFinancialYear: public mmReportCategoryExpensesGoes
{
public:
    mmReportCategoryExpensesGoesCurrentFinancialYear(mmCoreDB* core): mmReportCategoryExpensesGoes(core)
    {
        int day = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_DAY"), 1);
        int monthItem = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_MONTH"), 7);
        this->dtBegin_ = wxDateTime::Now().SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();

        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Goes - Current Financial Year to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();  
        }
        else
        {
            this->title_ = _("Where the Money Goes - Current Financial Year");
            this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);  
        }
        dtBegin_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));
        dtEnd_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));
        if (wxDateTime::Now().GetDateOnly() < dtBegin_)
        {
            dtEnd_.Subtract(wxDateSpan::Years(1));
            dtBegin_.Subtract(wxDateSpan::Years(1));
        }
    }
};

class mmReportCategoryExpensesComes: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesComes(mmCoreDB* core): mmReportCategoryExpenses(core
, false, wxDateTime::Now(), wxDateTime(), _("Where the Money Comes From"), 1)
    {}
};

class mmReportCategoryExpensesComesCurrentMonth: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentMonth(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->dtBegin_ = wxDateTime::Now().SetDay(1).GetDateOnly();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Comes From - Current Month to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();
        }
        else
        {
            this->title_ = _("Where the Money Comes From - Current Month");
            this->dtEnd_ = wxDateTime(dtBegin_).GetLastMonthDay();
        }
    }
};

class mmReportCategoryExpensesComesLastMonth: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesLastMonth(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->title_ = _("Where the Money Comes From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Months(1)).SetDay(1).GetDateOnly();
        this->dtEnd_ = wxDateTime(dtBegin_).GetLastMonthDay();
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
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Years(1))
            .SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();
        this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);
    }
};

class mmReportCategoryExpensesComesCurrentYear: public mmReportCategoryExpensesComes
{
public:
    mmReportCategoryExpensesComesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesComes(core)
    {
        this->dtBegin_ = wxDateTime::Now().SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Comes From - Current Year to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();  
        }
        else
        {
            this->title_ = _("Where the Money Comes From - Current Year");
            this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);  
        }
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
        int day = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_DAY"), 1);
        int monthItem = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_MONTH"), 7);
        this->dtBegin_ = wxDateTime::Now().SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();

        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Comes From - Current Financial Year to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();  
        }
        else
        {
            this->title_ = _("Where the Money Comes From - Current Financial Year");
            this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);  
        }
        dtBegin_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));
        dtEnd_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));
        if (wxDateTime::Now().GetDateOnly() < dtBegin_)
        {
            dtEnd_.Subtract(wxDateSpan::Years(1));
            dtBegin_.Subtract(wxDateSpan::Years(1));
        }
    }
};

class mmReportCategoryExpensesCategories: public mmReportCategoryExpenses
{
public:
    mmReportCategoryExpensesCategories(mmCoreDB* core): mmReportCategoryExpenses(core, false, wxDateTime::Now(), wxDateTime(), _("Where the Money Categories From"), 0)
    {}
};

class mmReportCategoryExpensesCategoriesCurrentMonth: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentMonth(mmCoreDB* core): mmReportCategoryExpensesCategories(core)
    {
        this->dtBegin_ = wxDateTime::Now().SetDay(1).GetDateOnly();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Categories From - Current Month to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();
        }
        else
        {
            this->title_ = _("Where the Money Categories From - Current Month");
            this->dtEnd_ = wxDateTime(dtBegin_).GetLastMonthDay();
        }
    }
};

class mmReportCategoryExpensesCategoriesLastMonth: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLastMonth(mmCoreDB* core): mmReportCategoryExpensesCategories(core)
    {
        this->title_ = _("Where the Money Categories From - Last Month");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Months(1)).SetDay(1).GetDateOnly();
        this->dtEnd_ = wxDateTime(dtBegin_).GetLastMonthDay();
    }
};

class mmReportCategoryExpensesCategoriesLast30Days: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLast30Days(mmCoreDB* core): mmReportCategoryExpensesCategories(core)
    {
        this->title_ = _("Where the Money Categories From - Last 30 Days");
        this->dtEnd_ = wxDateTime::Now().GetDateOnly();
        this->dtBegin_ = wxDateTime(dtEnd_).Subtract(wxDateSpan::Months(1)).Add(wxDateSpan::Days(1));

    }
};

class mmReportCategoryExpensesCategoriesLastYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLastYear(mmCoreDB* core): mmReportCategoryExpensesCategories(core)
    {
        this->title_ = _("Where the Money Categories From - Last Year");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Years(1))
            .SetMonth(wxDateTime::Jan).SetDay(1).GetDateOnly();
        this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);
    }
};

class mmReportCategoryExpensesCategoriesCurrentYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentYear(mmCoreDB* core): mmReportCategoryExpensesCategories(core)
    {
        this->dtBegin_ = wxDateTime::Now().SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Categories From - Current Year to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();  
        }
        else
        {
            this->title_ = _("Where the Money Categories From - Current Year");
            this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);  
        }
    }
};

class mmReportCategoryExpensesCategoriesLastFinancialYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesLastFinancialYear(mmCoreDB* core): mmReportCategoryExpensesCategories(core)
    {
        this->title_ = _("Where the Money Categories From - Last Month to Date");
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmReportCategoryExpensesCategoriesCurrentFinancialYear: public mmReportCategoryExpensesCategories
{
public:
    mmReportCategoryExpensesCategoriesCurrentFinancialYear(mmCoreDB* core): mmReportCategoryExpensesCategories(core)
    {
        int day = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_DAY"), 1);
        int monthItem = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_MONTH"), 7);
        this->dtBegin_ = wxDateTime::Now().SetDay(1).SetMonth(wxDateTime::Jan).GetDateOnly();

        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Categories From - Current Financial Year to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();  
        }
        else
        {
            this->title_ = _("Where the Money Categories From - Current Financial Year");
            this->dtEnd_ = wxDateTime(dtBegin_).SetMonth(wxDateTime::Dec).SetDay(31);  
        }
        dtBegin_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));
        dtEnd_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));
        if (wxDateTime::Now().GetDateOnly() < dtBegin_)
        {
            dtEnd_.Subtract(wxDateSpan::Years(1));
            dtBegin_.Subtract(wxDateSpan::Years(1));
        }
    }
};


#endif // _MM_EX_REPORTCATEGEXP_H_

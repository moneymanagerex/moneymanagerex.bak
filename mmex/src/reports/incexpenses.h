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

class mmReportIncomeExpenses : public mmPrintableBase 
{
public:
    mmReportIncomeExpenses(mmCoreDB* core, bool ignoreDate, const wxDateTime& dtBegin, const wxDateTime& dtEnd, const wxString& title);

    wxString getHTMLText();

protected:
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
    wxString title_;
};

class mmReportIncomeExpensesAllTime: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesAllTime(mmCoreDB* core): mmReportIncomeExpenses(core, true, wxDateTime::Now(), wxDateTime::Now(), _("Income vs Expenses - All Time"))
    {}
};

class mmReportIncomeExpensesCurrentMonth: public mmReportIncomeExpenses
{
public:
    mmReportIncomeExpensesCurrentMonth(mmCoreDB* core): mmReportIncomeExpenses(core, true, wxDateTime::Now(), wxDateTime::Now(), _("Income vs Expenses - All Time"))
    {
        this->dtBegin_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->dtEnd_ = wxDateTime::Now().GetLastMonthDay();
    }
};
#endif // _MM_EX_REPORTINCEXP_H_

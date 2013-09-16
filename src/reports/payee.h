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

#ifndef _MM_EX_REPORTPAYEE_H_
#define _MM_EX_REPORTPAYEE_H_

#include "reportbase.h"
#include "mmDateRange.h"

class mmReportPayeeExpenses : public mmPrintableBase
{
public:
    mmReportPayeeExpenses(mmCoreDB* core, const wxString& title = _("Payee Report"), mmDateRange* date_range = new mmAllTime(), bool by_name = false);
    ~mmReportPayeeExpenses();

    wxString getHTMLText();

protected:
    mmDateRange* date_range_;

    wxString title_;
    enum TYPE {INCOME = 0, EXPENCES, MAX};
	bool byName_;
    //static const wxString type_names[] = {_("Incomes"), _("Expences")};
};

class mmReportPayeeExpensesCurrentMonth: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesCurrentMonth(mmCoreDB* core, bool by_name): mmReportPayeeExpenses(core
                ,  wxString::Format(_("Payee report - %s"), _("Current Month")), new mmCurrentMonth(), by_name)
    {}
};

class mmReportPayeeExpensesCurrentMonthToDate: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesCurrentMonthToDate(mmCoreDB* core, bool by_name): mmReportPayeeExpenses(core
                ,  wxString::Format(_("Payee report - %s"), _("Current Month to Date")), new mmCurrentMonthToDate(), by_name)
    {}
};

class mmReportPayeeExpensesLastMonth: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesLastMonth(mmCoreDB* core, bool by_name): mmReportPayeeExpenses(core
                , wxString::Format(_("Payee report - %s"), _("Last Month")), new mmLastMonth(), by_name)
    {}
};

class mmReportPayeeExpensesLast30Days: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesLast30Days(mmCoreDB* core, bool by_name): mmReportPayeeExpenses(core
                , wxString::Format(_("Payee report - %s"), _("Last 30 Days")), new mmLast30Days(), by_name)
    {}
};

class mmReportPayeeExpensesLastYear: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesLastYear(mmCoreDB* core, bool by_name): mmReportPayeeExpenses(core
                , wxString::Format(_("Payee report - %s"), _("Last Year")), new mmLastYear(), by_name)
    {}
};

class mmReportPayeeExpensesCurrentYear: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesCurrentYear(mmCoreDB* core, bool by_name): mmReportPayeeExpenses(core
                , wxString::Format(_("Payee report - %s"), _("Current Year")), new mmCurrentYear(), by_name)
    {}
};

class mmReportPayeeExpensesLastFinancialYear: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesLastFinancialYear(mmCoreDB* core, int day, int month, bool by_name): mmReportPayeeExpenses(core
                , wxString::Format(_("Payee report - %s"), _("Last Financial Year")), new mmLastFinancialYear(day, month), by_name)
    {}
};

class mmReportPayeeExpensesCurrentFinancialYear: public mmReportPayeeExpenses
{
public:
    mmReportPayeeExpensesCurrentFinancialYear(mmCoreDB* core, int day, int month, bool by_name): mmReportPayeeExpenses(core
                , wxString::Format(_("Payee report - %s"), _("Current Financial Year")), new mmCurrentFinancialYear(day, month), by_name)
    {}
};

#endif //_MM_EX_REPORTPAYEE_H_

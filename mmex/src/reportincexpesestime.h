#ifndef _MM_EX_REPORTINCEXPENSESTIME_H_
#define _MM_EX_REPORTINCEXPENSESTIME_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"

class mmReportIncExpensesOverTime : public mmPrintableBase 
{
public:
    mmReportIncExpensesOverTime(mmCoreDB* core, int year) 
        : core_(core),
          year_(year)
    {
    }

    virtual wxString getHTMLText()
    {
        core_->currencyList_.loadBaseCurrencySettings();

        wxString yearStr = wxString::Format(wxT("%d"), year_);

        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Income vs Expenses for Year: ") + yearStr );

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        wxDateTime yearBegin(1, wxDateTime::Jan, year_);
        wxDateTime yearEnd(31, wxDateTime::Dec, year_);

		hb.startCenter();

        hb.startTable(wxT("50%"));
		hb.startTableRow();
		hb.addTableHeaderCell(_("Month"));
		hb.addTableHeaderCell(_("Income"));
		hb.addTableHeaderCell(_("Expenses"));
		hb.addTableHeaderCell(_("Difference"));
		hb.endTableRow();

        double income = 0.0;
        double expenses = 0.0;
		double balance = 0.0;
        
        for (int yidx = 0; yidx < 12; yidx++)
        {
            wxString monName = mmGetNiceMonthName(yidx) + wxT(" ") + yearStr;

            wxDateTime dtBegin(1, (wxDateTime::Month)yidx, year_);
            wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_);
            
            bool ignoreDate = false;
            income = 0.0;
            expenses = 0.0;
            core_->bTransactionList_.getExpensesIncome(-1, expenses, income,  ignoreDate, dtBegin, dtEnd);
            
            wxString actualExpStr;
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(expenses, actualExpStr);
            
            wxString actualIncStr;
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(income, actualIncStr);

            hb.startTableRow();
			hb.addTableCell(monName, false, true);
			
			balance = income - expenses;
			wxString actualBalStr;
			mmCurrencyFormatter::formatDoubleToCurrencyEdit(balance, actualBalStr);

			if (balance < 0.0)
			{
				hb.addTableCell(actualIncStr, true, true, true);
				hb.addTableCell(actualExpStr, true, true, true, wxT("#ff0000"));
				hb.addTableCell(actualBalStr, true, true, true, wxT("#ff0000"));
			}
			else
			{
				hb.addTableCell(actualIncStr, true, false, true);
				hb.addTableCell(actualExpStr, true, false, true);
				hb.addTableCell(actualBalStr, true, false, true);
			}
            hb.endTableRow();
        }

        wxDateTime today = wxDateTime::Now();
        wxDateTime prevYearEnd = wxDateTime(today);
        prevYearEnd.SetYear(year_);
        prevYearEnd.SetMonth(wxDateTime::Dec);
        prevYearEnd.SetDay(31);
        
        wxDateTime dtEnd = prevYearEnd;
        wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
        
        expenses = 0.0;
        income = 0.0;
        core_->bTransactionList_.getExpensesIncome(-1, expenses, income,  false, dtBegin, dtEnd);
        
        wxString actualExpStr;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(expenses, actualExpStr);

        wxString actualIncStr;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(income, actualIncStr);

		balance = income - expenses;
        wxString actualBalStr;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(balance, actualBalStr);

        std::vector<wxString> data;
        data.push_back(actualIncStr);
        data.push_back(actualExpStr);
		data.push_back(actualBalStr);

		hb.addRowSeparator(4);
		hb.addTotalRow(_("Total"), 4, data);
          
        hb.endTable();

		hb.endCenter();

        hb.end();
        return hb.getHTMLText();
    }

private:
    mmCoreDB* core_;
    int year_;
};

#endif

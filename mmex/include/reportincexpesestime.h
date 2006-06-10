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
    mmReportIncExpensesOverTime(wxSQLite3Database* db, int year) 
        : db_(db),
          year_(year)
    {
    }

    virtual wxString getHTMLText()
    {
        mmDBWrapper::loadBaseCurrencySettings(db_);

        wxString yearStr = wxString::Format(wxT("%d"), year_);

        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Income vs Expenses for Year : ") + yearStr );

        wxDateTime now = wxDateTime::Now();
        wxString dt = wxT("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        hb.addHTML(wxT("<font size=\"-2\">"));


        wxDateTime yearBegin(1, wxDateTime::Jan, year_);
        wxDateTime yearEnd(31, wxDateTime::Dec, year_);

        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(_("Month"));
        headerR.push_back(_("Income"));
        headerR.push_back(_("Expenses"));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));
        double income = 0.0;
        double expenses = 0.0;
        
        for (int yidx = 0; yidx < 12; yidx++)
        {
            wxString monName = wxDateTime::GetMonthName((wxDateTime::Month)yidx) + wxT(" ") + yearStr;


            wxDateTime dtBegin(1, (wxDateTime::Month)yidx, year_);
            wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_);
            
            bool ignoreDate = false;
            income = 0.0;
            expenses = 0.0;
            mmDBWrapper::getExpensesIncome(db_, -1, expenses, income,  ignoreDate, dtBegin, dtEnd);
            wxString actualExpStr;
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(expenses, actualExpStr);
            wxString actualIncStr;
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(income, actualIncStr);

            std::vector<wxString> data;
            data.push_back(monName);
            data.push_back(actualIncStr);
            data.push_back(actualExpStr);
            
            if (expenses > income)
                hb.addRow(data,  wxT(" bgcolor=\"#FDE4E4\" "));
            else
                hb.addRow(data,  wxT(" bgcolor=\"#FFFFFF\" "));
        }

        wxDateTime today = wxDateTime::Now();
        wxDateTime prevYearEnd = wxDateTime(today);
        prevYearEnd.SetYear(year_);
        prevYearEnd.SetMonth(wxDateTime::Dec);
        prevYearEnd.SetDay(31);
        wxDateTime dtEnd = prevYearEnd;
        wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
        mmDBWrapper::getExpensesIncome(db_, -1, expenses, income,  false, dtBegin, dtEnd);
        wxString actualExpStr;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(expenses, actualExpStr);
        wxString actualIncStr;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(income, actualIncStr);

        std::vector<wxString> data;
        data.push_back(_("Total"));

        data.push_back(actualIncStr);
        data.push_back(actualExpStr);

        hb.addRow(data,  wxT(" bgcolor=\"#DCEDD5\" "));
          
        hb.endTable();

        hb.addHTML(wxT("</font>"));

        hb.end();
        return hb.getHTMLText();
    }

private:
    wxSQLite3Database* db_;
    int year_;
};

#endif

#ifndef _MM_EX_REPORTINCEXP_H_
#define _MM_EX_REPORTINCEXP_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

class mmReportIncomeExpenses : public mmPrintableBase 
{
public:
    mmReportIncomeExpenses(wxSQLite3Database* db, bool ignoreDate, 
        wxDateTime dtBegin, 
        wxDateTime dtEnd) 
        : db_(db),
          ignoreDate_(ignoreDate),
          dtBegin_(dtBegin),
          dtEnd_(dtEnd)
    {
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, wxT("Income vs Expenses"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = wxT("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        if (!ignoreDate_)
        {
            wxString dtRange = wxT("From: ") + mmGetNiceDateSimpleString(dtBegin_) + wxT(" To: ") +
                mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        hb.addLineBreak();

        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(wxT("Type  "));
        headerR.push_back(wxT("Amount   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        mmDBWrapper::loadBaseCurrencySettings(db_);

        double expenses = 0.0;
        double income = 0.0;
        mmDBWrapper::getExpensesIncome(db_, -1,expenses, income,  ignoreDate_, dtBegin_,dtEnd_);

        wxString incString;
        wxString expString;
        mmCurrencyFormatter::formatDoubleToCurrency(expenses, expString);
        mmCurrencyFormatter::formatDoubleToCurrency(income, incString);

        std::vector<wxString> data;
        data.push_back(_T("Income: "));
        data.push_back(incString);
        hb.addRow(data);

        std::vector<wxString> data1;
        data1.push_back(_T("Expenses: "));
        data1.push_back(expString);
        hb.addRow(data1);

        hb.endTable();

        hb.end();
        return hb.getHTMLText();

    }

private:
    wxSQLite3Database* db_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;


};

#endif

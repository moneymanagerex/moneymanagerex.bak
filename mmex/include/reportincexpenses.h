#ifndef _MM_EX_REPORTINCEXP_H_
#define _MM_EX_REPORTINCEXP_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmgraphincexpensesmonth.h"

class mmReportIncomeExpenses : public mmPrintableBase 
{
public:
    mmReportIncomeExpenses(mmCoreDB* core, bool ignoreDate, 
        wxDateTime dtBegin, 
        wxDateTime dtEnd) 
        : core_(core),
          ignoreDate_(ignoreDate),
          dtBegin_(dtBegin),
          dtEnd_(dtEnd)
    {
        wxASSERT(dtBegin_ == dtBegin);
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Income vs Expenses"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        wxDateTime tBegin = dtBegin_;
        if (!ignoreDate_)
        {
            wxString dtRange = _("From: ") 
                + mmGetNiceDateSimpleString(tBegin.Add(wxDateSpan::Day())) + _(" To: ") 
                + mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        //hb.addLineBreak();

        core_->currencyList_.loadBaseCurrencySettings();

        double expenses = 0.0;
        double income = 0.0;
        core_->bTransactionList_.getExpensesIncome(-1,expenses, income,  ignoreDate_, dtBegin_,dtEnd_);

        mmGraphIncExpensesMonth gg;
        gg.init(income, expenses);
        gg.generate();
        hb.addHTML(gg.getHTML());

        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(_("Type  "));
        headerR.push_back(_("Amount   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));
   
        wxString incString;
        wxString expString;
        mmCurrencyFormatter::formatDoubleToCurrency(expenses, expString);
        mmCurrencyFormatter::formatDoubleToCurrency(income, incString);

        hb.addHTML(wxT("<tr><td>")); 
        hb.addHTML(_("Income: "));
        hb.addHTML(wxT("</td><td align=\"right\">"));
        hb.addHTML(incString);
        hb.addHTML(wxT("</td></tr>"));

        hb.addHTML(wxT("<tr><td>")); 
        hb.addHTML(_("Expenses: "));
        hb.addHTML(wxT("</td><td align=\"right\">"));
        hb.addHTML(expString);
        hb.addHTML(wxT("</td></tr>"));

        hb.endTable();

        hb.end();
        return hb.getHTMLText();
    }

private:
    mmCoreDB* core_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;


};

#endif

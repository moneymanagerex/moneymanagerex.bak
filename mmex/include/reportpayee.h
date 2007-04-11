#ifndef _MM_EX_REPORTPAYEE_H_
#define _MM_EX_REPORTPAYEE_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

class mmReportPayeeExpenses : public mmPrintableBase 
{
public:
    mmReportPayeeExpenses(mmCoreDB* core, bool ignoreDate, 
        wxDateTime dtBegin, 
        wxDateTime dtEnd) 
        : core_(core),
          ignoreDate_(ignoreDate),
          dtBegin_(dtBegin),
          dtEnd_(dtEnd)
    {
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("To Whom the Money Goes"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        wxDateTime tBegin = dtBegin_;
        if (!ignoreDate_)
        {
            wxString dtRange = _("From: ") 
                + mmGetNiceDateSimpleString(tBegin.Add(wxDateSpan::Day())) 
                + _T(" To: ") 
                + mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        hb.addLineBreak();

        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(_("Payee    "));
        headerR.push_back(_("Amount   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        core_->currencyList_.loadBaseCurrencySettings();
        

        int numPayees = (int)core_->payeeList_.payees_.size();
        for (int idx = 0; idx < numPayees; idx++)
        {
            wxString balance;
            double amt = core_->bTransactionList_.getAmountForPayee(
               core_->payeeList_.payees_[idx]->payeeID_, ignoreDate_, 
                dtBegin_, dtEnd_);

            mmCurrencyFormatter::formatDoubleToCurrency(amt, balance);

            if (amt != 0.0)
            {
                hb.addHTML(wxT("<tr><td>")); 
                hb.addHTML(core_->payeeList_.payees_[idx]->payeeName_);
                hb.addHTML(wxT("</td><td align=\"right\">"));
                hb.addHTML(balance);
                hb.addHTML(wxT("</td></tr>"));
            }
        }
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

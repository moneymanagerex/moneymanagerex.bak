#ifndef _MM_EX_REPORTTRANSSTATS_H_
#define _MM_EX_REPORTTRANSSTATS_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"

class mmReportTransactionStats : public mmPrintableBase 
{
public:
    mmReportTransactionStats(mmCoreDB* core, int year) 
        : core_(core),
          year_(year)
    {
    }

    virtual wxString getHTMLText()
    {
        core_->currencyList_.loadBaseCurrencySettings();

        wxString yearStr = wxString::Format(wxT("%d - %d"), year_, year_+1);
        wxString lastYearStr = wxString::Format(wxT("%d"), year_);
        wxString thisYearStr = wxString::Format(wxT("%d"), year_+1);

        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Transaction Statistics for ") + yearStr );

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
        headerR.push_back(lastYearStr);
        headerR.push_back(thisYearStr);
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        double income = 0.0;
        double expenses = 0.0;
        
        for (int yidx = 0; yidx < 12; yidx++)
        {
            wxString monName = wxDateTime::GetMonthName((wxDateTime::Month)yidx);

            wxDateTime dtPrevBegin(1, (wxDateTime::Month)yidx, year_);
            wxDateTime dtPrevEnd = dtPrevBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_);
            
            wxDateTime dtThisBegin(1, (wxDateTime::Month)yidx, year_+1);
            wxDateTime dtThisEnd  = dtThisBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_+1);
            

            bool ignoreDate = false;
            int numPrev = 0;
            int numThis = 0;
            core_->bTransactionList_.getTransactionStats(-1, numPrev,  ignoreDate, dtPrevBegin, dtPrevEnd);
            core_->bTransactionList_.getTransactionStats(-1, numThis,  ignoreDate, dtThisBegin, dtThisEnd);
            
            wxString numPrevStr = wxString::Format(wxT("%d"), numPrev);
            wxString numThisStr = wxString::Format(wxT("%d"), numThis);

            std::vector<wxString> data;
            data.push_back(monName);
            data.push_back(numPrevStr);
            data.push_back(numThisStr);
            
            hb.addRow(data,  wxT(" bgcolor=\"#FFFFFF\" "));
        }

        wxDateTime today = wxDateTime::Now();
        wxDateTime prevYearEnd = wxDateTime(today);
        prevYearEnd.SetYear(year_);
        prevYearEnd.SetMonth(wxDateTime::Dec);
        prevYearEnd.SetDay(31);
        
        wxDateTime dtEnd = prevYearEnd;
        wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
        
        int numLastYear = 0;
        core_->bTransactionList_.getTransactionStats(-1, numLastYear,  false, dtBegin, dtEnd);
        
        std::vector<wxString> data;
        data.push_back(_("Total"));

        wxString numLastYearStr = wxString::Format(wxT("%d"), numLastYear);
        data.push_back(numLastYearStr);

        wxDateTime thisYearEnd = wxDateTime(today);
        thisYearEnd.SetYear(year_+1);
        thisYearEnd.SetMonth(wxDateTime::Dec);
        thisYearEnd.SetDay(31);
        
        wxDateTime dtThisEnd = thisYearEnd;
        wxDateTime dtThisBegin = thisYearEnd.Subtract(wxDateSpan::Year());
        
        int numThisYear = 0;
        core_->bTransactionList_.getTransactionStats(-1, numThisYear,  false, dtThisBegin, dtThisEnd);

        wxString numThisYearStr = wxString::Format(wxT("%d"), numThisYear);
        data.push_back(numThisYearStr);

        hb.addRow(data,  wxT(" bgcolor=\"#DCEDD5\" "));
          
        hb.endTable();

        hb.addHTML(wxT("</font>"));

        hb.end();
        return hb.getHTMLText();
    }

private:
    mmCoreDB* core_;
    int year_;
};

#endif

#include "reporttransstats.h"
#include "defs.h"
#include "htmlbuilder.h"
#include "util.h"
#include "budgetingpanel.h"
#include "mmcoredb.h"

mmReportTransactionStats::mmReportTransactionStats(mmCoreDB* core, int year)  : 
        mmPrintableBase(core),
        year_(year + 1)
{
}

wxString mmReportTransactionStats::getHTMLText()
{
    int yearsHist = 5; //How many years should show the report 
    core_->loadBaseCurrencySettings();

    wxString rangeStr = wxString::Format(wxT("%d - %d"), year_ - yearsHist + 1, year_);

    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, _("Transaction Statistics for ") + rangeStr );

    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(1, dt);
    hb.addLineBreak();
    hb.addLineBreak();

    wxDateTime yearBegin(1, wxDateTime::Jan, year_ - yearsHist);
    wxDateTime yearEnd(31, wxDateTime::Dec, year_ - yearsHist);

    hb.startCenter();
    hb.startTable(wxT("50%"));
    //Header 
    // Month 2011 2010 2009.....
    hb.startTableRow();
    hb.addTableHeaderCell(_("Month"));
    for (int y = 1; y <= yearsHist; y++) 
    {
        hb.addTableHeaderCell(wxString::Format(wxT("%d"), year_ - y + 1));
    }
    hb.endTableRow();
    //Table
    std::map<int, int> grand_total;
    for (int yidx = 0; yidx < 12; yidx++)
    {
        wxString monName = mmGetNiceMonthName(yidx);
        //
        hb.startTableRow();
        hb.addTableCell(monName, false, true);
        // Totals for monthes
        for (int y = 1; y <= yearsHist; y++) 
        {
            wxDateTime dtThisBegin(1, (wxDateTime::Month)yidx, year_- y + 1);
            wxDateTime dtThisEnd  = dtThisBegin.GetLastMonthDay((wxDateTime::Month)yidx, year_-y + 1);

            bool ignoreDate = false;
            int numThis = 0;
            core_->bTransactionList_.getTransactionStats(-1, numThis, ignoreDate, dtThisBegin, dtThisEnd);
            hb.addTableCell(wxString::Format(wxT("%d"), numThis), true);
            grand_total[y] += numThis;
        }
        hb.endTableRow();
    }
    hb.addRowSeparator(yearsHist + 1);
    hb.startTableRow();
    hb.addTableCell(_("Total"), true);

    //Grand Totals
    for (std::map<int, int>::const_iterator it = grand_total.begin(); it != grand_total.end(); ++ it)
    {
        hb.addTableCell(wxString::Format(wxT("%d"), it->second), true);
    }
    //------------
    hb.endTable();
    hb.endCenter();
    hb.end();

    return hb.getHTMLText();
}

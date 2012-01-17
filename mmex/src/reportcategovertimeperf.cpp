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

//----------------------------------------------------------------------------
#include "htmlbuilder.h"
#include "util.h"
#include "mmcoredb.h"
#include "reportcategovertimeperf.h"
//----------------------------------------------------------------------------
#include <vector>
#include <math.h>
//----------------------------------------------------------------------------

namespace
{

const int g_NullCategory = -1;
const int g_NullSubCat = g_NullCategory;
//----------------------------------------------------------------------------
enum EWhat { INCOME, EXPENSES, OVERALL, WHAT_MAX };
//----------------------------------------------------------------------------

const char g_sql[] = 
"select c.CATEGID, "
       "c.CATEGNAME, "
       "sc.SUBCATEGID, "
       "sc.SUBCATEGNAME "

"from CATEGORY_V1 c "

"left join SUBCATEGORY_V1 sc "
"on sc.CATEGID = c.CATEGID "

"order by c.CATEGNAME, "
         "sc.SUBCATEGNAME";

//----------------------------------------------------------------------------

/*
    Set last day of months 23:59:59.999
*/
wxDateTime& prepareEndDate(wxDateTime &dt)
{
    wxDateTime::Month mon = dt.GetMonth();
    int year = dt.GetYear();

    dt.Set(dt.GetNumberOfDays(mon, year), mon, year, 23, 59, 59, 999);

    return dt;
}
//----------------------------------------------------------------------------
typedef std::vector<std::pair<wxDateTime, wxDateTime> > periods_t;
typedef std::vector<std::pair<double, double> > columns_totals_t;
//----------------------------------------------------------------------------

void prepareAndPrintPeriods
(
    const wxDateTime &periodBegin,
    const wxDateTime &periodEnd,
    mmHTMLBuilder &hb,
    periods_t &periods
)
{
    for (periods_t::iterator i = periods.begin(); i != periods.end(); ++i)
    {
        wxDateTime &dtBegin = i->first;
        wxDateTime &dtEnd = i->second;

        dtBegin = periodBegin;
        dtBegin += wxDateSpan::Months(std::distance(periods.begin(), i));

        if (i == periods.begin()) {
            wxASSERT(dtBegin == periodBegin);
        }

        // --

        dtEnd = dtBegin;
        prepareEndDate(dtEnd);

        if (i == --periods.end()) {
            bool ok = dtEnd == periodEnd; 
            wxASSERT(ok);
            wxUnusedVar(ok);
        }

        // --

        wxString yyyy;
        yyyy << dtBegin.GetYear();

        hb.addTableHeaderCell(mmGetNiceShortMonthName(dtBegin.GetMonth()) + wxT(" ") + yyyy);
    }
}
//----------------------------------------------------------------------------

void printRow
(
    const wxDateTime &periodBegin,
    const wxDateTime &periodEnd,
    const periods_t &periods,
    wxSQLite3ResultSet &q1,
    int cat_id,
    int subcat_id,
    mmCoreDB &core,
    mmHTMLBuilder &hb,
    columns_totals_t &columns_totals
)
{
    double period_amount = core.bTransactionList_.getAmountForCategory(cat_id, subcat_id, false, periodBegin, periodEnd, false, false, mmIniOptions::ignoreFutureTransactions_);

    if (period_amount == 0) {
        return;
    }

    hb.startTableRow();

    wxString categ = q1.GetString(wxT("CATEGNAME"));

    if (subcat_id != g_NullSubCat) {
        categ += wxT(": ");
        categ += q1.GetString(wxT("SUBCATEGNAME"));
    }

    hb.addTableCell(categ, false, true);

    double period_amount_sum = 0; // row summary

    for (periods_t::const_iterator i = periods.begin(); i != periods.end(); ++i)
    {
        const wxDateTime &dtBegin = i->first;
        const wxDateTime &dtEnd = i->second;

        // wxLogDebug(wxT("begin=%s, end=%s"), dtBegin.Format().c_str(), dtEnd.Format().c_str());

        double month_amount = core.bTransactionList_.getAmountForCategory(cat_id, subcat_id, false,  dtBegin, dtEnd, false, false, mmIniOptions::ignoreFutureTransactions_);

        wxString month_amount_str;

        if (month_amount != 0)
        {
            period_amount_sum += month_amount;
            mmex::formatDoubleToCurrencyEdit(month_amount, month_amount_str);

            periods_t::const_iterator::difference_type j = std::distance(periods.begin(), i);
            columns_totals_t::reference r = columns_totals[j];
            
            (month_amount < 0 ? r.second : r.first) += month_amount;
        }

        hb.addTableCell(month_amount_str, true);
    }

    wxASSERT(fabs(period_amount_sum - period_amount) < 0.01);

    // summary of period for category\subcategory

    wxString period_amount_str;
    mmex::formatDoubleToCurrencyEdit(period_amount, period_amount_str);
    hb.addTableCell(period_amount_str, true);

    hb.endTableRow();
}
//----------------------------------------------------------------------------

void printColumnsTotals
(
    const columns_totals_t &columns_totals, 
    EWhat what, 
    mmHTMLBuilder &hb
)
{
    hb.startTableRow();

    const wxString title[WHAT_MAX] = { _("Income"), _("Expenses"), _("Overall") };
    hb.addTableHeaderCell(title[what]);

    for (columns_totals_t::const_iterator i = columns_totals.begin(); i != columns_totals.end(); ++i)
    {
        double val = what == INCOME   ?  i->first  :
                     what == EXPENSES ? -i->second : // print as positive value in report
                     what == OVERALL  ? (i->first + i->second) :
                     0;

        wxString str;

        if (val != 0) {
            mmex::formatDoubleToCurrencyEdit(val, str);
        }

        hb.addTableCell(str, true, false, true);
    }

    hb.addTableHeaderCell(wxGetEmptyString());
    hb.endTableRow();
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

mmReportCategoryOverTimePerformance::mmReportCategoryOverTimePerformance(mmCoreDB *core) : 
    m_core(core)
{ 
    wxASSERT(m_core);
}
//----------------------------------------------------------------------------

wxSQLite3Database& mmReportCategoryOverTimePerformance::getDb() const
{
    return *m_core->db_;
}
//----------------------------------------------------------------------------

wxString mmReportCategoryOverTimePerformance::getHTMLText()
{
    const int MONTHS_IN_PERIOD = 12; // including current month

    mmHTMLBuilder hb;
    hb.init();
    
    hb.addHeader(3, wxString::Format(_("Category Income/Expenses Over Last %d Months "), MONTHS_IN_PERIOD));

    const wxDateTime now = wxDateTime::Now();

    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(7, dt);
    hb.addLineBreak();
    hb.addLineBreak();

    // setup period

    wxDateTime dummy = now;
    const wxDateTime periodEnd = prepareEndDate(dummy);

    dummy.SetDay(1);
    dummy -= wxDateSpan::Months(MONTHS_IN_PERIOD - 1); // X months before current one

    const wxDateTime periodBegin = dummy.ResetTime();

    // wxLogDebug(wxT("yea_begin=%s, yea_end=%s"), periodBegin.Format().c_str(), periodEnd.Format().c_str());

    // print header of report

    hb.startCenter();
    hb.startTable();

	hb.startTableRow();
	hb.addTableHeaderCell(_("Category"));

    periods_t periods(MONTHS_IN_PERIOD);
    prepareAndPrintPeriods(periodBegin, periodEnd, hb, periods);

    columns_totals_t columns_totals(periods.size());

    hb.addTableHeaderCell(_("Overall"));
	hb.endTableRow();

    // begin of table

    mmDBWrapper::loadBaseCurrencySettings(&getDb());

    wxSQLite3ResultSet q1 = getDb().ExecuteQuery(g_sql);

    for (int last_cat_id = g_NullCategory; q1.NextRow();)
    {
        int cat_id = q1.GetInt(wxT("CATEGID"));
        
        if (last_cat_id != cat_id) { // category changed
            last_cat_id = cat_id;
            printRow(periodBegin, periodEnd, periods, q1, cat_id, g_NullSubCat, *m_core, hb, columns_totals);
        }

        int sc_idx = q1.FindColumnIndex(wxT("SUBCATEGID"));
        
        if (!q1.IsNull(sc_idx)) {
            int subcat_id = q1.GetInt(sc_idx);
            printRow(periodBegin, periodEnd, periods, q1, cat_id, subcat_id, *m_core, hb, columns_totals);
        }
    }

    q1.Finalize();

    printColumnsTotals(columns_totals, INCOME, hb);
    printColumnsTotals(columns_totals, EXPENSES, hb);
    printColumnsTotals(columns_totals, OVERALL, hb);

    hb.endTable();
	hb.endCenter();

    hb.end();
    return hb.getHTMLText();
}
//----------------------------------------------------------------------------

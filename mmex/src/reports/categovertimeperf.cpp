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
#include "categovertimeperf.h"
#include "../htmlbuilder.h"
#include "../util.h"

//----------------------------------------------------------------------------

mmReportCategoryOverTimePerformance::mmReportCategoryOverTimePerformance(mmCoreDB *core
, mmDateRange* date_range) :
    mmPrintableBase(core)
    , date_range_(date_range)
    , title_(_("Category Income/Expenses: %s (UNDER CONSTRACTION)"))
{
    wxASSERT(core_);
}
//----------------------------------------------------------------------------

wxString mmReportCategoryOverTimePerformance::getHTMLText()
{
    const int MONTHS_IN_PERIOD = 12; // including current month

    mmHTMLBuilder hb;
    hb.init();

    hb.addHeader(2, wxString::Format(title_, date_range_->title()));
    hb.addDateNow();
    hb.addLineBreak();

    hb.startCenter();
    hb.startTable();

    hb.startTableRow();
    hb.addTableHeaderCell(_("Category"));
    wxDateTime start_date = date_range_->start_date();
    for (int i = 0; i < MONTHS_IN_PERIOD; i++)
    {
        wxDateTime d = wxDateTime(start_date).Add(wxDateSpan::Months(i));
        hb.addTableHeaderCell(wxGetTranslation(wxDateTime::GetMonthName(d.GetMonth(), wxDateTime::Name_Abbr))
            + wxString::Format("<br>%i", d.GetYear()));
    }
    hb.addTableHeaderCell(_("Overall"));
    hb.endTableRow();

    // begin of table

    std::map<int, std::map<int, std::map<int, double> > > categoryStats;
    core_->bTransactionList_.getCategoryStats(categoryStats
        , date_range_
        , mmIniOptions::instance().ignoreFutureTransactions_);
    core_->currencyList_.LoadBaseCurrencySettings();

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(SELECT_ALL_CATEGORIES);

    double overall = 0;
    for (int last_cat_id = -1; q1.NextRow();)
    {
        int categID = q1.GetInt("CATEGID");
        int subcategID = q1.GetInt("SUBCATEGID");
        if (last_cat_id != categID)
        {
            last_cat_id = categID;
            hb.startTableRow();
            hb.addTableCell(core_->categoryList_.GetFullCategoryString(categID, -1));
            overall = 0;
            for (const auto &i : categoryStats[categID][-1])
            {
                double value = i.second;
                hb.addMoneyCell(value);
                overall += value;
            }
            hb.addMoneyCell(overall);
            hb.endTableRow();
        }

        hb.startTableRow();
        hb.addTableCell(core_->categoryList_.GetFullCategoryString(categID, subcategID));
        overall = 0;
        for (const auto &i : categoryStats[categID][subcategID])
        {
            double value = i.second;
            hb.addMoneyCell(value);
            overall += value;
        }
        hb.addMoneyCell(overall);
        hb.endTableRow();
    }

    q1.Finalize();

    hb.endTable();
    hb.endCenter();
    hb.end();
    return hb.getHTMLText();
}
//----------------------------------------------------------------------------

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

#include "reportcategexp.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "mmgraphpie.h"
#include "reportbudget.h"

mmReportCategoryExpenses::mmReportCategoryExpenses(
    mmCoreDB* core, 
    bool ignoreDate, 
    wxDateTime dtBegin, 
    wxDateTime dtEnd,
    const wxString& title,
    int type
) : 
    mmPrintableBase(core),
    db_(core_->db_.get()),
    dtBegin_(dtBegin),
    dtEnd_(dtEnd),
    ignoreDate_(ignoreDate),
    title_(title),
    type_(type)
{
}

wxString mmReportCategoryExpenses::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(3, title_);

    mmCommonReportDetails dateDisplay(NULL);
    wxDateTime tBegin = dtBegin_;    // date needs to be adjusted
    dateDisplay.DisplayDateHeading(hb, tBegin.Add(wxDateSpan::Day()), dtEnd_, !ignoreDate_);

    hb.startCenter();

    // Add the graph
    mmGraphPie gg;
    hb.addImage(gg.getOutputFileName());

    hb.startTable(wxT("50%"));
    hb.startTableRow();
    hb.addTableHeaderCell(_("Category"));
    hb.addTableHeaderCell(_("Amount"));
    hb.endTableRow();

    core_->loadBaseCurrencySettings();

    std::vector<ValuePair> valueList;
        
    static const char sql[] =
    "select CATEGID, CATEGNAME "
    "from CATEGORY_V1 "
    "order by CATEGNAME";

    static const char sql_sub[] = 
    "select SUBCATEGID, SUBCATEGNAME "
    "from SUBCATEGORY_V1 "
    "where CATEGID = ?"
    "order by SUBCATEGNAME";

    wxSQLite3Statement st = db_->PrepareStatement(sql_sub);
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

    double grandtotal = 0.0;
    bool grandtotalseparator = true;
    while (q1.NextRow())
    {
        int categs = 0;
        //grandtotalseparator = true;
        double categtotal = 0.0;
        int categID          = q1.GetInt(wxT("CATEGID"));
        wxString categString = q1.GetString(wxT("CATEGNAME"));
        wxString balance;
        double amt = core_->bTransactionList_.getAmountForCategory(categID, -1, ignoreDate_,
            dtBegin_, dtEnd_, false, false, mmIniOptions::instance().ignoreFutureTransactions_
        );
        mmex::formatDoubleToCurrency(amt, balance);

        if ((type_ == 0) || ((type_ == 1 && amt > 0.0) ||
            (type_ == 2 && amt < 0.0)))
        {
            if (amt != 0.0)
            {
                ValuePair vp;
                vp.label = categString;
                vp.amount = amt;
                grandtotal += amt;
                categtotal += amt;
                categs++;
                valueList.push_back(vp);

                hb.startTableRow();
                hb.addTableCell(categString, false, true);
                hb.addTableCell(balance, true, false, true);
                hb.endTableRow();
            }
        }

        st.Bind(1, categID);
        wxSQLite3ResultSet q2 = st.ExecuteQuery(); 
           
        while(q2.NextRow())
        {
            int subcategID = q2.GetInt(wxT("SUBCATEGID"));
            wxString subcategString = q2.GetString(wxT("SUBCATEGNAME"));

            amt = core_->bTransactionList_.getAmountForCategory(categID, subcategID, ignoreDate_,
                dtBegin_, dtEnd_, false, false, mmIniOptions::instance().ignoreFutureTransactions_
            );
            mmex::formatDoubleToCurrency(amt, balance);

            // if we want only income
            if (type_ == 1 && amt < 0.0)
                continue;

            // if we want only expenses
            if (type_ == 2 && amt > 0.0)
                continue;

            if (amt != 0.0)
            {
                ValuePair vp;
                vp.label = categString + wxT(" : ") + subcategString;
                vp.amount = amt;
                grandtotal += amt;
                categtotal += amt;
                categs++;
                valueList.push_back(vp);

                hb.startTableRow();
                hb.addTableCell(categString + wxT(" : ") + subcategString, false, true);
                hb.addTableCell(balance, true, false, true);
                hb.endTableRow();
            }
        }
        if (categs>1)
        {
            wxString categtotalStr;
            mmex::formatDoubleToCurrency(categtotal, categtotalStr);hb.startTableRow();
            hb.addTableCell(_("Category Total: "),false, true, true, wxT("GRAY"));
            hb.addTableCell(categtotalStr, true, false, true, wxT("GRAY"));
        }
        if (categs>0)
        {
        grandtotalseparator = false;
        hb.addRowSeparator(2);
        }

        st.Reset();
    }
        
    q1.Finalize();
    st.Finalize();

    if (grandtotalseparator)
    hb.addRowSeparator(2);
    wxString grandtotalStr;
    mmex::formatDoubleToCurrency(grandtotal, grandtotalStr);
    hb.startTableRow();
    hb.addTableCell(_("Grand Total: "),false, true, true);
    hb.addTableCell(grandtotalStr, true, false, true);
    hb.endTableRow();
        
    hb.endTable();
    hb.endCenter();

    hb.end();

    gg.init(valueList);
    gg.Generate(wxEmptyString);

    return hb.getHTMLText();
}

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

#include "categexp.h"
#include "budget.h"

#include "../htmlbuilder.h"
#include "../util.h"
#include "../mmOption.h"
#include "../mmgraphpie.h"

mmReportCategoryExpenses::mmReportCategoryExpenses(
    mmCoreDB* core,
    mmDateRange* date_range,
    const wxString& title,
    int type
) :
    mmPrintableBase(core),
    date_range_(date_range),
    title_(title),
    type_(type),
    ignoreFutureDate_(mmIniOptions::instance().ignoreFutureTransactions_)
{
}

wxString mmReportCategoryExpenses::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, title_);

    hb.DisplayDateHeading(date_range_->start_date(), date_range_->end_date(), date_range_->is_with_date());

    hb.startCenter();

    // Add the graph
    mmGraphPie gg;
    hb.addImage(gg.getOutputFileName());

    hb.startTable("60%");
    hb.startTableRow();
    hb.addTableHeaderCell(_("Category"));
    hb.addTableHeaderCell(_("Amount"), true);
    hb.endTableRow();

    core_->currencyList_.LoadBaseCurrencySettings();

    std::vector<ValuePair> valueList;

    double grandtotal = 0.0;
    bool ignore_date = !date_range_->is_with_date();

    for (const auto& category: core_->categoryList_.entries_)
    {
        int categs = 0;
        bool grandtotalseparator = true;
        double categtotal = 0.0;
        int categID = category->categID_;
        const wxString sCategName = category->categName_;
        double amt = core_->bTransactionList_.getAmountForCategory(categID, -1, ignore_date
            , date_range_->start_date(), date_range_->end_date(), false, false, ignoreFutureDate_);
        if (type_ == GOES && amt < 0.0) amt = 0;
        if (type_ == COME && amt > 0.0) amt = 0;

        categtotal += amt;
        grandtotal += amt;

        if (amt != 0)
        {
            ValuePair vp;
            vp.label = sCategName;
            vp.amount = amt;
            valueList.push_back(vp);

            hb.startTableRow();
            hb.addTableCell(sCategName, false, true);
            hb.addMoneyCell(amt, false);
            hb.endTableRow();
        }

        for (const auto & sub_category: category->children_)
        {
            int subcategID = sub_category->categID_;

            wxString sFullCategName = core_->categoryList_.GetFullCategoryString(categID, subcategID);
            amt = core_->bTransactionList_.getAmountForCategory(categID, subcategID, ignore_date
                , date_range_->start_date(), date_range_->end_date(), false, false, ignoreFutureDate_);

            if (type_ == GOES && amt < 0.0) amt = 0;
            if (type_ == COME && amt > 0.0) amt = 0;

            categtotal += amt;
            grandtotal += amt;

            if (amt != 0)
            {
                categs++;
                ValuePair vp;
                vp.label = sFullCategName;
                vp.amount = amt;
                valueList.push_back(vp);

                hb.startTableRow();
                hb.addTableCell(sFullCategName, false, true);
                hb.addMoneyCell(amt, false);
                hb.endTableRow();
            }
        }

        if (categs>1)
        {
            hb.addRowSeparator(0);
            hb.startTableRow();
            hb.addTableCell(_("Category Total: "),false, true, true, "GRAY");
			hb.addMoneyCell(categtotal, "GRAY");
            hb.endTableRow();
        }

        if (categs>0)
        {
            grandtotalseparator = false;
            hb.addRowSeparator(2);
        }
    }

    hb.startTableRow();
    hb.addTableCell(_("Grand Total: "),false, true, true);
	hb.addMoneyCell(grandtotal);
    hb.endTableRow();

    hb.endTable();
    hb.endCenter();
    hb.end();

    gg.init(valueList);
    gg.Generate(wxEmptyString);

    return hb.getHTMLText();
}

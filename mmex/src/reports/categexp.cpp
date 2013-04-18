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
#include "../mmgraphpie.h"
#include "../mmex.h"

mmReportCategoryExpenses::mmReportCategoryExpenses(
    mmCoreDB* core,
    bool ignoreDate,
    const wxDateTime& dtBegin,
    const wxDateTime& dtEnd,
    const wxString& title,
    int type
) :
    mmPrintableBase(core),
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
    hb.addHeader(2, title_);

    mmCommonReportDetails dateDisplay(NULL);
    dateDisplay.DisplayDateHeading(hb, dtBegin_, dtEnd_, !ignoreDate_);

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

    wxString sBalance = "?";
    std::pair<mmCategoryList::const_iterator, mmCategoryList::const_iterator> range = core_->categoryList_.Range();
    for (mmCategoryList::const_iterator it = range.first; it != range.second; ++ it)
    {
        int categs = 0;
        bool grandtotalseparator = true;
        double categtotal = 0.0;
        const wxSharedPtr<mmCategory> category = *it;
        int categID = category->categID_;
        const wxString sCategName = category->categName_;
        double amt = core_->bTransactionList_.getAmountForCategory(categID, -1, ignoreDate_,
            dtBegin_, dtEnd_, false, false, mmIniOptions::instance().ignoreFutureTransactions_);
        if (type_ == 1 && amt < 0.0) amt = 0;
        if (type_ == 2 && amt > 0.0) amt = 0;

        categtotal += amt;
        grandtotal += amt;

        if (amt != 0)
        {
            ValuePair vp;
            vp.label = sCategName;
            vp.amount = amt;
            valueList.push_back(vp);

            mmex::formatDoubleToCurrency(amt, sBalance);
            hb.startTableRow();
            hb.addTableCell(sCategName, false, true);
            hb.addTableCell(sBalance, true, false, true);
            hb.endTableRow();
        }

        for (std::vector<wxSharedPtr<mmCategory> >::const_iterator cit =  category->children_.begin()
            ; cit != category->children_.end()
            ; ++ cit)
        {
            const wxSharedPtr<mmCategory> sub_category = *cit;
            int subcategID = sub_category->categID_;

            wxString sFullCategName = core_->categoryList_.GetFullCategoryString(categID, subcategID);
            amt = core_->bTransactionList_.getAmountForCategory(categID, subcategID, ignoreDate_,
                dtBegin_, dtEnd_, false, false, mmIniOptions::instance().ignoreFutureTransactions_);

            if (type_ == 1 && amt < 0.0) amt = 0;
            if (type_ == 2 && amt > 0.0) amt = 0;

            categtotal += amt;
            grandtotal += amt;

            if (amt != 0)
            {
                categs++;
                ValuePair vp;
                vp.label = sFullCategName;
                vp.amount = amt;
                valueList.push_back(vp);

                mmex::formatDoubleToCurrency(amt, sBalance);
                hb.startTableRow();
                hb.addTableCell(sFullCategName, false, true);
                hb.addTableCell(sBalance, true, false, true);
                hb.endTableRow();
            }
        }

        if (categs>1)
        {
            wxString categtotalStr;
            mmex::formatDoubleToCurrency(categtotal, categtotalStr);
            hb.addRowSeparator(0);
            hb.startTableRow();
            hb.addTableCell(_("Category Total: "),false, true, true, "GRAY");
            hb.addTableCell(categtotalStr, true, false, true, "GRAY");
            hb.endTableRow();
        }

        if (categs>0)
        {
            grandtotalseparator = false;
            hb.addRowSeparator(2);
        }
    }

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

mmReportCategoryExpensesGoesCurrentMonth::mmReportCategoryExpensesGoesCurrentMonth(mmCoreDB* core) : 
    mmReportCategoryExpensesGoes(core)
    {
        this->dtBegin_ = wxDateTime::Now().SetDay(1).GetDateOnly();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            this->title_ = _("Where the Money Goes - Current Month to Date");
            this->dtEnd_ = wxDateTime::Now().GetDateOnly();
        }
        else
        {
            this->title_ = _("Where the Money Goes - Current Month");
            this->dtEnd_ = wxDateTime(dtBegin_).GetLastMonthDay();
        }
    }

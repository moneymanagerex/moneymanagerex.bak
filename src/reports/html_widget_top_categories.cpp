/*******************************************************
 Copyright (C) 2013 Nikolay

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

#include "html_widget_top_categories.h"

#include "util.h"
#include "model/Model_Checking.h"
#include "model/Model_Account.h"
#include "model/Model_Category.h"
#include "model/Model_Subcategory.h"
#include "model/Model_Splittransaction.h"

#include <algorithm>

htmlWidgetTop7Categories::htmlWidgetTop7Categories(mmDateRange* date_range)
    : date_range_(date_range)
{
    title_ = wxString::Format(_("Top Withdrawals: %s"), date_range_->title());
}

htmlWidgetTop7Categories::~htmlWidgetTop7Categories()
{
    if (date_range_) delete date_range_;
}

wxString htmlWidgetTop7Categories::getHTMLText()
{
    static const wxString FUNCTION =
        "<script> function toggleCategories()\n"
        "{\n"
        "    var elem = document.getElementById(\"%s\");\n"
        "    var label = document.getElementById(\"categ_label\");\n"
        "    var hide = elem.style.display == \"none\";\n"
        "    if (hide) {\n"
        "        elem.style.display = \"\";\n"
        "        label.innerHTML = \"[-]\";\n"
        "    }\n"
        "    else {\n"
        "        elem.style.display = \"none\";\n"
        "        label.innerHTML = \"[+]\";\n"
        "    }\n"
        "}\n"
        "</script>\n";
    const wxString id = "TOP_CATEGORIES";

    wxString output = "<table class = \"table\"><thead><tr class='active'><th>\n";
    output += title_ + "</th><th class='text-right'><a id=\"bils_label\" onclick=\"toggleCategories(); \" href=\"#\">[-]</a></th></tr></thead>\n";
    output += wxString::Format("<tbody id='%s'>", id);
    output += "<tr style='background-color: #d8ebf0'><td>";
    output += _("Category") + "</td><td class='text-right'>" + _("Summary") + "</td></tr>";

    std::vector<std::pair<wxString, double> > topCategoryStats;
    getTopCategoryStats(topCategoryStats, date_range_);

    for (const auto& i : topCategoryStats)
    {
        output += "<tr>";
        output += wxString::Format("<td>%s</td>", (i.first.IsEmpty() ? "..." : i.first));
        output += wxString::Format("<td class='text-right'>%s</td>", Model_Currency::toCurrency(i.second));
        output += "</tr>";
    }
    output += "</tbody></table>\n";
    output += wxString::Format(FUNCTION, id);

    return output;
}

void htmlWidgetTop7Categories::getTopCategoryStats(
    std::vector<std::pair<wxString, double> > &categoryStats
    , const mmDateRange* date_range) const
{
    //Get base currency rates for all accounts
    std::map<int, double> acc_conv_rates;
    for (const auto& account: Model_Account::instance().all())
    {
        Model_Currency::Data* currency = Model_Account::currency(account);
        acc_conv_rates[account.ACCOUNTID] = currency->BASECONVRATE;
    }
    //Temporary map
    std::map<std::pair<int /*category*/, int /*sub category*/>, double> stat;

    const auto &transactions = Model_Checking::instance().find(
            Model_Checking::TRANSDATE(date_range->start_date(), GREATER_OR_EQUAL)
            , Model_Checking::TRANSDATE(date_range->end_date(), LESS_OR_EQUAL)
            , Model_Checking::STATUS(Model_Checking::VOID_, NOT_EQUAL)
            , Model_Checking::TRANSCODE(Model_Checking::TRANSFER, NOT_EQUAL));

    for (const auto &trx : transactions)
    {
        bool withdrawal = Model_Checking::type(trx) == Model_Checking::WITHDRAWAL;
        if (Model_Checking::splittransaction(trx).empty())
        {
            std::pair<int, int> category = std::make_pair(trx.CATEGID, trx.SUBCATEGID);
            if (withdrawal)
                stat[category] -= trx.TRANSAMOUNT * (acc_conv_rates[trx.ACCOUNTID]);
            else
                stat[category] += trx.TRANSAMOUNT * (acc_conv_rates[trx.ACCOUNTID]);
        }
        else
        {
            const auto &splits = Model_Splittransaction::instance().find(Model_Splittransaction::TRANSID(trx.TRANSID));
            for (const auto& entry : splits)
            {
                std::pair<int, int> category = std::make_pair(entry.CATEGID, entry.SUBCATEGID);
                double val = entry.SPLITTRANSAMOUNT
                    * (acc_conv_rates[trx.ACCOUNTID])
                    * (withdrawal ? -1 : 1);
                stat[category] += val;
            }
        }
    }

    categoryStats.clear();
    for (const auto& i : stat)
    {
        if (i.second < 0)
        {
            std::pair <wxString, double> stat_pair;
            stat_pair.first = Model_Category::full_name(i.first.first, i.first.second);
            stat_pair.second = i.second;
            categoryStats.push_back(stat_pair);
        }
    }

    std::stable_sort(categoryStats.begin(), categoryStats.end()
        , [] (const std::pair<wxString, double> x, const std::pair<wxString, double> y)
        { return x.second < y.second; }
    );

    int counter = 0;
    std::vector<std::pair<wxString, double> >::iterator iter;
    for (iter = categoryStats.begin(); iter != categoryStats.end(); )
    {
        counter++;
        if (counter > 7)
            iter = categoryStats.erase(iter);
        else
            ++iter;
    }
}


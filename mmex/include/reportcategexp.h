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
 /*******************************************************/

#ifndef _MM_EX_REPORTCATEGEXP_H_
#define _MM_EX_REPORTCATEGEXP_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmgraphpie.h"

class mmReportCategoryExpenses : public mmPrintableBase 
{
public:
    mmReportCategoryExpenses(mmCoreDB* core, bool ignoreDate, 
        wxDateTime dtBegin, 
        wxDateTime dtEnd) 
        : core_(core),
          db_(core_->db_.get()),
          ignoreDate_(ignoreDate),
          dtBegin_(dtBegin),
          dtEnd_(dtEnd)
    {
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Where the Money Goes"));

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
                + _(" To: ") 
                + mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        // Add the graph
        mmGraphPie gg;
        hb.addHTML(gg.getHTML());

        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(_("Category  "));
        headerR.push_back(_("Amount   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        core_->currencyList_.loadBaseCurrencySettings();

        std::vector<ValuePair> valueList;
        mmBEGINSQL_LITE_EXCEPTION;
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from CATEGORY_V1 order by CATEGNAME;");
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
        while (q1.NextRow())
        {
            int categID          = q1.GetInt(wxT("CATEGID"));
            wxString categString = q1.GetString(wxT("CATEGNAME"));
            wxString balance;
            double amt = core_->bTransactionList_.getAmountForCategory(categID, -1, ignoreDate_, 
                dtBegin_, dtEnd_);
            mmCurrencyFormatter::formatDoubleToCurrency(amt, balance);

            if (amt != 0.0)
            {
                ValuePair vp;
                vp.label = categString;
                vp.amount = amt;
                valueList.push_back(vp);

                hb.addHTML(wxT("<tr><td>")); 
                hb.addHTML(categString);
                hb.addHTML(wxT("</td><td align=\"right\">"));
                hb.addHTML(balance);
                hb.addHTML(wxT("</td></tr>"));
            }

            wxSQLite3StatementBuffer bufSQL1;
            bufSQL1.Format("select * from SUBCATEGORY_V1 where CATEGID=%d;", categID);
            wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL1); 
            while(q2.NextRow())
            {
                int subcategID          = q2.GetInt(wxT("SUBCATEGID"));
                wxString subcategString    = q2.GetString(wxT("SUBCATEGNAME"));

                amt = core_->bTransactionList_.getAmountForCategory(categID, subcategID, 
                    ignoreDate_,  dtBegin_, dtEnd_);
                mmCurrencyFormatter::formatDoubleToCurrency(amt, balance);

                if (amt != 0.0)
                {
                   ValuePair vp;
                   vp.label = categString + wxT(" : ") + subcategString;
                   vp.amount = amt;
                   valueList.push_back(vp);

                    hb.addHTML(wxT("<tr><td>")); 
                    hb.addHTML(categString + wxT(" : ") + subcategString);
                    hb.addHTML(wxT("</td><td align=\"right\">"));
                    hb.addHTML(balance);
                    hb.addHTML(wxT("</td></tr>"));

                }
            }
            q2.Finalize();
           
        }
        q1.Finalize();
        mmENDSQL_LITE_EXCEPTION;
        hb.endTable();

        hb.end();

        gg.init(valueList);
        gg.generate();

        return hb.getHTMLText();

    }

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
};

#endif

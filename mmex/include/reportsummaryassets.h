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
#ifndef _MM_EX_REPORTSUMMARYASSETS_H_
#define _MM_EX_REPORTSUMMARYASSETS_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "assets.h"

class mmReportSummaryAssets : public mmPrintableBase 
{
public:
    mmReportSummaryAssets(wxSQLite3Database* db) : db_(db) {}

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Summary of Assets"));

        wxDateTime now = wxDateTime::Now();
        wxString dt(wxDateTime::GetWeekDayName(now.GetWeekDay()) + wxString(wxT(", ")));
        dt += wxDateTime::GetMonthName(now.GetMonth()) + wxString(wxT(" "));
        dt += wxString::Format(wxT("%d"), now.GetDay()) + wxT(", ") 
            + wxString::Format(wxT("%d"), now.GetYear());
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        hb.addHTML(wxT("<font size=\"-2\">"));

        mmDBWrapper::loadBaseCurrencySettings(db_);
        int ct = 0;  
        double tincome = 0.0;
        double texpenses = 0.0;
        double tBalance = 0.0;
        mmBEGINSQL_LITE_EXCEPTION;
        
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from ASSETS_V1;");
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);

        hb.beginTable();

        std::vector<wxString> headerR;
        headerR.push_back(_("Name "));
        headerR.push_back(_("Type   "));
        headerR.push_back(_("Value  "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        int ct = 0;
        while (q1.NextRow())
        {
            mmAssetHolder th;

            th.assetID_           = q1.GetInt(wxT("ASSETID"));
            th.value_             = mmDBWrapper::getAssetValue(db_, th.assetID_);
            th.assetName_         = q1.GetString(wxT("ASSETNAME"));
            th.assetType_         = q1.GetString(wxT("ASSETTYPE"));
            wxString tempString;
            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.value_, tempString))
                th.valueStr_   = tempString;

            std::vector<wxString> data;
            data.push_back(th.assetName_);
            data.push_back(th.assetType_);
            data.push_back(th.valueStr_);
          
            hb.addRow(data);
        }
        q1.Finalize();
        
        hb.endTable();
        mmENDSQL_LITE_EXCEPTION

        /* Assets */
        double assetBalance = mmDBWrapper::getAssetBalance(db_);
        wxString assetBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(assetBalance, assetBalanceStr);
        hb.endTable();
        hb.addHTML(wxT("</font>"));
        hb.addLineBreak();
        hb.addLineBreak();
        
        wxString dispStr = _("Total Assets :") + assetBalanceStr; 
        hb.addHeader(7, dispStr);
        

        hb.end();
        return hb.getHTMLText();
    }

private:
    wxSQLite3Database* db_;


};

#endif

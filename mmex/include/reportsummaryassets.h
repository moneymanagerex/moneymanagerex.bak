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
#include "assetspanel.h"

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
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        mmDBWrapper::loadBaseCurrencySettings(db_);
        int ct = 0;  
        double tincome = 0.0;
        double texpenses = 0.0;
        double tBalance = 0.0;
        mmBEGINSQL_LITE_EXCEPTION;
        
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from ASSETS_V1;");
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);

        hb.startCenter();

		hb.startTable(wxT("50%"));
		hb.startTableRow();
		hb.addTableHeaderCell(_("Name"));
		hb.addTableHeaderCell(_("Type"));
		hb.addTableHeaderCell(_("Value"));
		hb.endTableRow();

        int ct = 0;
        while (q1.NextRow())
        {
            mmAssetHolder th;

            th.assetID_           = q1.GetInt(wxT("ASSETID"));
            th.value_             = mmDBWrapper::getAssetValue(db_, th.assetID_);
            th.assetName_         = q1.GetString(wxT("ASSETNAME"));
            wxString itemAssetTypeStrings[] =  
            {
               _("Property"),
               _("Automobile"),
               _("Household Object"),
               _("Art"),
               _("Jewellery"),
               _("Cash"),
               _("Other"),
            };

            wxString assetTypeStr = q1.GetString(wxT("ASSETTYPE"));
            if (assetTypeStr == wxT("Property"))
               th.assetType_ =  _("Property");
            else if (assetTypeStr == wxT("Automobile"))
               th.assetType_ =  _("Automobile");
            else if (assetTypeStr == wxT("Household Object"))
               th.assetType_ =  _("Household Object");
            else if (assetTypeStr == wxT("Art"))
               th.assetType_ =  _("Art");
            else if (assetTypeStr == wxT("Jewellery"))
               th.assetType_ =  _("Jewellery");
            else if (assetTypeStr == wxT("Cash"))
               th.assetType_ =  _("Cash");
            else if (assetTypeStr == wxT("Other"))
               th.assetType_ =  _("Other");

            wxString tempString;
            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.value_, tempString))
                th.valueStr_   = tempString;

			hb.startTableRow();
			hb.addTableCell(th.assetName_, false, true);
			hb.addTableCell(th.assetType_);
			hb.addTableCell(th.valueStr_, true);
			hb.endTableRow();
        }
        q1.Finalize();
        
        mmENDSQL_LITE_EXCEPTION

        /* Assets */
        double assetBalance = mmDBWrapper::getAssetBalance(db_);
        wxString assetBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(assetBalance, assetBalanceStr);

		hb.addRowSeparator(3);
		hb.addTotalRow(_("Total Assets: "), 3, assetBalanceStr);
        hb.endTable();

		hb.endCenter();

        hb.end();
        return hb.getHTMLText();
    }

private:
    wxSQLite3Database* db_;


};

#endif

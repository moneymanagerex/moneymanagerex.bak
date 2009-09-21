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

        hb.startCenter();

        hb.startTable(wxT("50%"));
        hb.startTableRow();
        hb.addTableHeaderCell(_("Name"));
        hb.addTableHeaderCell(_("Type"));
        hb.addTableHeaderCell(_("Value"));
        hb.endTableRow();

        mmDBWrapper::loadBaseCurrencySettings(db_);
        
        static const char sql[] =
        "select ASSETID, "
               "ASSETNAME, "
               "ASSETTYPE " 
        "from ASSETS_V1";

        mmBEGINSQL_LITE_EXCEPTION;
        
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

        while (q1.NextRow())
        {
            mmAssetHolder th;

            th.assetID_           = q1.GetInt(wxT("ASSETID"));
            th.value_             = mmDBWrapper::getAssetValue(db_, th.assetID_);
            th.assetName_         = q1.GetString(wxT("ASSETNAME"));

            wxString assetTypeStr = q1.GetString(wxT("ASSETTYPE"));
            th.assetType_ = wxGetTranslation(assetTypeStr);

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

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

#include "reportsummaryassets.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "assetspanel.h"

wxString mmReportSummaryAssets::getHTMLText()
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

        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

        while (q1.NextRow())
        {
            mmAssetHolder th;

            th.assetID_           = q1.GetInt(wxT("ASSETID"));
            th.value_             = mmDBWrapper::getAssetValue(db_, th.assetID_);
            th.assetName_         = q1.GetString(wxT("ASSETNAME"));

            wxString assetTypeStr = q1.GetString(wxT("ASSETTYPE"));
            th.assetType_ = wxGetTranslation(assetTypeStr);

            mmex::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);

			hb.startTableRow();
			hb.addTableCell(th.assetName_, false, true);
			hb.addTableCell(th.assetType_);
			hb.addTableCell(th.valueStr_, true);
			hb.endTableRow();
        }
        q1.Finalize();
        
        /* Assets */
        double assetBalance = mmDBWrapper::getAssetBalance(db_);
        wxString assetBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmex::formatDoubleToCurrency(assetBalance, assetBalanceStr);

	hb.addRowSeparator(3);
	hb.addTotalRow(_("Total Assets: "), 3, assetBalanceStr);
        hb.endTable();

	hb.endCenter();
        hb.end();

        return hb.getHTMLText();
}

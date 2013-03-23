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

#include "summaryassets.h"

#include "../defs.h"
#include "../htmlbuilder.h"
#include "../util.h"
#include "../dbwrapper.h"
#include "../assetspanel.h"

wxString mmReportSummaryAssets::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, _("Summary of Assets"));
    hb.addDateNow();

    hb.startCenter();

    hb.startTable(wxT("95%"));
    hb.startTableRow();
    hb.addTableHeaderCell(_("Name"));
    hb.addTableHeaderCell(_("Type"));
    hb.addTableHeaderCell(_("Value"), true);
    hb.addTableHeaderCell(_("Notes"));
    hb.endTableRow();

    core_->currencyList_.LoadBaseCurrencySettings();
    
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_FROM_ASSETS_V1);

    while (q1.NextRow())
    {
        mmAssetHolder th;

        th.id_         = q1.GetInt(wxT("ASSETID"));
        th.value_      = mmDBWrapper::getAssetValue(db_, th.id_);
        th.assetName_  = q1.GetString(wxT("ASSETNAME"));
        th.asset_notes_ = q1.GetString(wxT("NOTES"));

        wxString assetTypeStr = q1.GetString(wxT("ASSETTYPE"));
        th.assetType_ = wxGetTranslation(assetTypeStr);

        mmex::formatDoubleToCurrencyEdit(th.value_, th.valueStr_);

        hb.startTableRow();
        hb.addTableCell(th.assetName_, false, true);
        hb.addTableCell(th.assetType_);
        hb.addTableCell(th.valueStr_, true);
        hb.addTableCell(th.asset_notes_);
        hb.endTableRow();
    }
    q1.Finalize();
    
    /* Assets */
    double assetBalance = mmDBWrapper::getAssetBalance(db_);
    wxString assetBalanceStr;
    core_->currencyList_.LoadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(assetBalance, assetBalanceStr);

    hb.addRowSeparator(4);
    hb.addTotalRow(_("Total Assets: "), 3, assetBalanceStr);
    hb.addTableCell(wxT(""));
    hb.endTableRow();
    hb.endTable();

    hb.endCenter();
    hb.end();

    return hb.getHTMLText();
}

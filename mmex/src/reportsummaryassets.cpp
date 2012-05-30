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
#include "util.h"
#include "dbwrapper.h"
#include "assetspanel.h"
#include "mmex_db_view.h"
#include <boost/foreach.hpp>

#define _MM_EX_REPORTSUMMARYASSETS_CPP_REVISION_ID    "$Revision$"
wxString mmReportSummaryAssets::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, _("Summary of Assets"));
    
    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeaderItalic(0, dt);
    hb.addLineBreak();
    hb.addLineBreak();

    hb.startCenter();

    hb.startTable(wxT("95%"));
    hb.startTableRow();
    hb.addTableHeaderCell(_("Name"));
    hb.addTableHeaderCell(_("Type"));
    hb.addTableHeaderCell(_("Value"), true);
    hb.addTableHeaderCell(_("Notes"));
    hb.endTableRow();

    mmDBWrapper::loadBaseCurrencySettings(db_);

    double assetBalance = 0.0;
    wxString assetBalanceStr;
    DB_View_ASSETS_V1::Data_Set all_assets = ASSETS_V1.all(db_);
    BOOST_FOREACH(const DB_View_ASSETS_V1::Data &asset, all_assets)
    {
        double value = mmDBWrapper::getAssetValue(asset);
        assetBalance += value;
        wxString asset_type = wxGetTranslation(asset.ASSETTYPE);
        wxString value_str;
        mmex::formatDoubleToCurrencyEdit(value, value_str);
        
        hb.startTableRow();
        hb.addTableCell(asset.ASSETNAME, false, true);
        hb.addTableCell(asset_type);
        hb.addTableCell(value_str, true);
        hb.addTableCell(asset.NOTES);
        hb.endTableRow();
    }
    
    /* Assets */
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

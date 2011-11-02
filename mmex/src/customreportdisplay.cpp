/*************************************************************************
 Copyright (C) 2011 Stefano Giorgio      

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
 *************************************************************************/

#include "customreportdisplay.h"
#include "util.h"

mmCustomSQLReport::mmCustomSQLReport(mmCoreDB* core, const wxString& reportTitle, const wxString& sqlQuery):
    core_(core), reportTitle_(reportTitle), sqlQuery_(sqlQuery)
{
}

void mmCustomSQLReport::displayReportHeader(mmHTMLBuilder& hb, wxString reportTitle)
{
    hb.addHeader(3, reportTitle);

    wxDateTime now = wxDateTime::Now();
    wxString dt = wxT("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(7, dt);
    hb.addLineBreak();
    hb.addLineBreak();

	hb.startCenter();
    hb.startTable(wxT("90%"));
}

wxString mmCustomSQLReport::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    displayReportHeader(hb, wxString() << _("Custom SQL Report: ") << reportTitle_ );

	wxSQLite3ResultSet sqlQueryResult;
	try
	{
		sqlQueryResult = core_->db_->ExecuteQuery(sqlQuery_);
	}
	catch(wxSQLite3Exception e)
	{
		wxSafeShowMessage(wxT("Error"),e.GetMessage().c_str());
		return wxString::Format(_("Error: %s"), e.GetMessage().c_str());
	}

    // Use column info to determine if data is to be right justified.    
    int columnCount = sqlQueryResult.GetColumnCount();
    bool *alignRight = new bool[columnCount];

    hb.startTableRow();
    for (int index = 0; index < columnCount; index ++)
    {
    	hb.addTableHeaderCell(sqlQueryResult.GetColumnName(index));
    	alignRight[index] = (sqlQueryResult.GetColumnType(index)==WXSQLITE_INTEGER || sqlQueryResult.GetColumnType(index)==WXSQLITE_FLOAT);
    }
    hb.endTableRow();

    while (sqlQueryResult.NextRow())
    {
        hb.startTableRow();
        for (int index = 0; index < columnCount; index ++)
        {
            wxString displayData = sqlQueryResult.GetAsString(index); 
            if ( sqlQueryResult.GetColumnType(index) == WXSQLITE_FLOAT )
            {
                mmex::formatDoubleToCurrencyEdit(sqlQueryResult.GetDouble(index), displayData);
            }

            //Right justify numeric data. 
            hb.addTableCell( displayData, alignRight[index]);
        }
        hb.endTableRow();
    }
    delete [] alignRight;
    sqlQueryResult.Finalize();

    hb.endTable();
	hb.endCenter();

    hb.end();
    return hb.getHTMLText();
}


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

mmCustomSQLReport::mmCustomSQLReport(mmCoreDB* core, const wxString& reportTitle,
    const wxString& sqlQuery, wxHtmlWindow* htmlWindow):
    mmPrintableBase(core), reportTitle_(reportTitle), sqlQuery_(sqlQuery)
{
}

void mmCustomSQLReport::displayReportHeader(mmHTMLBuilder& hb, wxString reportTitle)
{
    hb.addHeader(2, reportTitle);
    hb.addHeaderItalic(0, wxT("Today's Date: ") + mmGetNiceDateString(wxDateTime::Now()));
}

wxString mmCustomSQLReport::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    displayReportHeader(hb, wxString() << _("Custom SQL Report: ") << reportTitle_ );
    hb.startCenter();
    hb.startTable(wxT("90%"));

    wxSQLite3ResultSet sqlQueryResult;

    int lines_number;
    wxString error_string;

    try
    {
        lines_number = core_->db_->ExecuteScalar(wxT("select count (*) from (\n") + sqlQuery_ + wxT("\n)"));
    }
    catch(const wxSQLite3Exception& e)
    {
        error_string = e.GetMessage();
    }

    wxString lower = error_string.Lower();
    if (lower.Contains(wxT("update")) ||
        lower.Contains(wxT("delete")) ||
        lower.Contains(wxT("insert")))
    {
        wxMessageDialog msgDlg(0, _("SQL Query will modify your Data. Proceed??"), _("Warning"), wxYES_NO);
        if (msgDlg.ShowModal() != wxID_YES) {
            hb.addHeader(2, _("SQL query discardeded by user"));
            hb.end();
            return hb.getHTMLText();
        }
    }

    try
    {
        sqlQueryResult = core_->db_->ExecuteQuery(sqlQuery_);
    }
    catch(const wxSQLite3Exception& e)
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
        alignRight[index] = (sqlQueryResult.GetColumnType(index) == WXSQLITE_INTEGER
                          || sqlQueryResult.GetColumnType(index) == WXSQLITE_FLOAT);
    }
    hb.endTableRow();

    int progress = 0;
    wxString progressMsg;
    wxProgressDialog dlg(_("Report printing in progress"), progressMsg, lines_number,
        NULL, wxPD_AUTO_HIDE |  wxPD_CAN_ABORT);


    while (sqlQueryResult.NextRow())
    {
        progress++;
        progressMsg = wxString() << _("Lines prepared: ") << progress;
        if(!dlg.Update(progress, progressMsg))
        {
            hb.clear();
            hb.addHeader(2, _("SQL query discardeded by user"));
            hb.end();
            return hb.getHTMLText(); // abort processing
        }

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

    if (lower.Contains(wxT("update")))
         hb.addHeader(2, _("Dababase updated succesfully"));
    if (lower.Contains(wxT("delete")))
         hb.addHeader(2, _("Deletion completed"));
    if (lower.Contains(wxT("insert")))
         hb.addHeader(2, _("Data Insertion completed"));

    return hb.getHTMLText();
}


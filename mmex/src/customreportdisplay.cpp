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
    const wxString& sqlQuery, wxProgressDialog* progressBar): mmPrintableBase(core)
    , reportTitle_(reportTitle), sqlQuery_(sqlQuery), progressBar_(progressBar)
{
}

void mmCustomSQLReport::displayReportHeader(mmHTMLBuilder& hb, wxString reportTitle)
{
    hb.addHeader(2, reportTitle);
    hb.addHeaderItalic(0, ("Today's Date: ") + mmGetNiceDateString(wxDateTime::Now()));
}

wxString mmCustomSQLReport::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    displayReportHeader(hb, _("Custom SQL Report: ") + reportTitle_ );
    hb.startCenter();
    hb.startTable(("90%"));

    wxSQLite3ResultSet sqlQueryResult;

    int lines_number = 0;
    wxString error_string;

    try
    {
        lines_number = core_->db_->ExecuteScalar(("select count (*) from (\n") + sqlQuery_ + ("\n)"));
    }
    catch(const wxSQLite3Exception& e)
    {
        error_string = e.GetMessage();
    }

    wxString lower = error_string.Lower();
    if (lower.Contains(("update")) ||
        lower.Contains(("delete")) ||
        lower.Contains(("insert")))
    {
        wxMessageDialog msgDlg(0, _("SQL Query will modify your Data. Proceed??"), _("Warning"), wxYES_NO|wxICON_WARNING);
        if (msgDlg.ShowModal() != wxID_YES) {
            hb.addHeader(2, _("SQL query discarded by user"));
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
        wxSafeShowMessage("Error",e.GetMessage().c_str());
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
//    wxProgressDialog dlg(_("Report printing in progress"), progressMsg, lines_number,
//        NULL, wxPD_AUTO_HIDE |  wxPD_CAN_ABORT);

    while (sqlQueryResult.NextRow())
    {
        if (progressBar_)
        {
            progress++;
            if (progress > 99) progress = 1;
            progressMsg = wxString() << _("Lines prepared: ") << progress;
            if(!progressBar_->Update(progress, progressMsg))
            {
                hb.clear();
                hb.addHeader(2, _("SQL query discarded by user"));
                hb.end();
                return hb.getHTMLText(); // abort processing
            }
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

    if (lower.Contains(("update")))
         hb.addHeader(2, _("Dababase updated succesfully"));
    if (lower.Contains(("delete")))
         hb.addHeader(2, _("Deletion completed"));
    if (lower.Contains(("insert")))
         hb.addHeader(2, _("Data Insertion completed"));

    hb.end();

    progressMsg = wxString() << _("Lines prepared: ") << progress << _(" Completed");
    if (progressBar_) progressBar_->Update(90, progressMsg);

    return hb.getHTMLText();
}


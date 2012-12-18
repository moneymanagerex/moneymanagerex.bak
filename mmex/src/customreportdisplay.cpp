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

mmCustomSQLReport::mmCustomSQLReport(wxWindow* parent, mmCoreDB* core
    , const wxString& reportTitle, const wxString& sScript, const wxString& sSctiptType):
    mmPrintableBase(core), parent_(parent), reportTitle_(reportTitle), sScript_(sScript), sSctiptType_(sSctiptType)
{
}

void mmCustomSQLReport::displayReportHeader(mmHTMLBuilder& hb, const wxString reportTitle)
{
    hb.addHeader(2, reportTitle);
    hb.addDateNow();
}

wxString mmCustomSQLReport::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    displayReportHeader(hb, _("Custom Report: ") + reportTitle_ );
    hb.startCenter();
    hb.startTable(wxT("90%"));

    wxString lower;
    if (sSctiptType_ == wxT("SQL"))
    {
        int rows = 0;
        wxString error_string;

        try
        {
            rows = core_->db_->ExecuteScalar(wxT("select count (*) from (\n") + sScript_ + wxT("\n)"));
        }
        catch(const wxSQLite3Exception& e)
        {
            error_string = e.GetMessage();
        }

        lower = error_string.Lower();
        if (lower.Contains(wxT("update")) ||
            lower.Contains(wxT("delete")) ||
            lower.Contains(wxT("insert")))
        {
            wxMessageDialog msgDlg(parent_, _("SQL Query will modify your Data. Proceed??"), _("Warning"), wxYES_NO|wxICON_WARNING);
            if (msgDlg.ShowModal() != wxID_YES) {
                hb.endCenter();
                hb.addHeader(2, _("SQL query discarded by user"));
                hb.end();
                return hb.getHTMLText();
            }
        }

        wxSQLite3ResultSet sqlQueryResult;
        try
        {
            sqlQueryResult = core_->db_->ExecuteQuery(sScript_);
        }
        catch(const wxSQLite3Exception& e)
        {
            wxSafeShowMessage(wxT("Error"),e.GetMessage().c_str());
            return wxString::Format(_("Error: %s"), e.GetMessage().c_str());
        }

        // Use column info to determine if data is to be right justified.
        int columnCount = sqlQueryResult.GetColumnCount();
        bool *alignRight = new bool[columnCount];

        hb.startTable();
        hb.startTableRow();
        for (int index = 0; index < columnCount; index ++)
        {
            hb.addTableHeaderCell(sqlQueryResult.GetColumnName(index));
            alignRight[index] = (sqlQueryResult.GetColumnType(index) == WXSQLITE_INTEGER
                              || sqlQueryResult.GetColumnType(index) == WXSQLITE_FLOAT);
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
    }
    else if (sSctiptType_ == wxT("Lua"))
    {
        wxString sOutput, sError;
        int e = lua2cppGetString(sScript_, sOutput, sError);
        if (e == 0)
            hb.addParaText(sOutput);
        else
            hb.addParaText(sError);
    }
    else
        wxASSERT(true);

    hb.endTable();
    hb.endCenter();

    if (sSctiptType_ == wxT("SQL"))
    {
        if (lower.Contains(wxT("update")))
             hb.addHeader(2, _("Dababase updated succesfully"));
        if (lower.Contains(wxT("delete")))
             hb.addHeader(2, _("Deletion completed"));
        if (lower.Contains(wxT("insert")))
             hb.addHeader(2, _("Data Insertion completed"));
    }

    hb.end();
    return hb.getHTMLText();
}

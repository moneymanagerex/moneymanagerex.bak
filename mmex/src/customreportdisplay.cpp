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
#include "lua_interface.h"
#include "util.h"

mmCustomSQLReport::mmCustomSQLReport(wxWindow* parent, mmCoreDB* core
, const wxString& reportTitle, const wxString& sScript, const wxString& sScriptType)
: mmPrintableBase(core)
, parent_(parent)
, reportTitle_(reportTitle)
, sScript_(sScript)
, sScriptType_(sScriptType)
{}

void mmCustomSQLReport::DisplayReportHeader(mmHTMLBuilder& hb, const wxString reportTitle)
{
    hb.addHeader(2, reportTitle);
    hb.addDateNow();
}

bool mmCustomSQLReport::DisplaySQL_Results(mmHTMLBuilder& hb)
{
    hb.startCenter();
    hb.startTable();
//  hb.startTable(wxT("90%"));

    int rows = 0;
    wxString sql_script_exception;
    wxString sql_modify;
    try
    {
        rows = core_->db_->ExecuteScalar(wxT("select count (*) from (\n") + sScript_ + wxT("\n)"));
    }
    catch(const wxSQLite3Exception& e)
    {
        sql_script_exception = e.GetMessage();
    }
    
    sql_modify = sql_script_exception.Lower();
    if (sql_modify.Contains(wxT("update")) ||
        sql_modify.Contains(wxT("delete")) ||
        sql_modify.Contains(wxT("insert")))
    {
        wxMessageDialog msgDlg(parent_, _("SQL Query will modify your Data. Proceed??"), _("Warning"), wxYES_NO|wxICON_WARNING);
        if (msgDlg.ShowModal() != wxID_YES)
        {
            hb.endCenter();
            hb.addParaText(_("SQL query discarded by user"));
            hb.endTable();
            return false;
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

        hb.endCenter();
        hb.addParaText(wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        hb.endTable();
        return false;
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

    if (sql_modify.Contains(wxT("update")))
         hb.addHeader(2, _("Dababase updated succesfully"));

    if (sql_modify.Contains(wxT("delete")))
         hb.addHeader(2, _("Deletion completed"));
    
    if (sql_modify.Contains(wxT("insert")))
             hb.addHeader(2, _("Data Insertion completed"));

    hb.endCenter();
    hb.endTable();
    return true;
}

void mmCustomSQLReport::DisplayLua_Results(mmHTMLBuilder& hb)
{
    TLuaInterface lua(core_);
    wxString lua_result = lua.RunLuaCode(sScript_);
    hb.addParaText(lua_result);
}

wxString mmCustomSQLReport::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    DisplayReportHeader(hb, _("Custom Report: ") + reportTitle_ );

    if (sScriptType_ == wxT("SQL"))
    {
        DisplaySQL_Results(hb);
    }
    else if (sScriptType_ == wxT("LUA"))
    {
        DisplayLua_Results(hb);
    }
    else
    {
        hb.addParaText(_("Cannot execute unknown Script type"));
    }

    hb.end();
    return hb.getHTMLText();
}

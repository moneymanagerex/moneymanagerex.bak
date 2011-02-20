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

#include "customsqlreports.h"
#include "htmlbuilder.h"
#include "util.h"
#include "constants.h"
#include <wx/tokenzr.h>

mmCustomSQLReport::mmCustomSQLReport(mmCoreDB* core, const wxString& reportTitle, const wxString& sqlQuery):
    core_(core), reportTitle_(reportTitle), sqlQuery_(sqlQuery)
{
}

wxString mmCustomSQLReport::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    wxString title = wxString() << _("Custom SQL Report: ") << reportTitle_; 
    hb.addHeader(3, title);

    wxDateTime now = wxDateTime::Now();
    wxString dt = wxT("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(7, dt);
    hb.addLineBreak();
    hb.addLineBreak();

	hb.startCenter();
    hb.startTable(wxT("90%"));

    wxSQLite3ResultSet sqlQueryResult = core_->db_->ExecuteQuery(sqlQuery_);

    int columnCount = sqlQueryResult.GetColumnCount();
    hb.startTableRow();
    for (int index = 0; index < columnCount; index ++)
    {
    	hb.addTableHeaderCell(sqlQueryResult.GetColumnName(index));
    }
    hb.endTableRow();

    while (sqlQueryResult.NextRow())
    {
        hb.startTableRow();
        for (int index = 0; index < columnCount; index ++)
        {
            hb.addTableCell(sqlQueryResult.GetAsString(index));
        }
        hb.endTableRow();
    }

    sqlQueryResult.Finalize();

    hb.endTable();
	hb.endCenter();

    hb.end();
    return hb.getHTMLText();
}

//===============================================================
// Methods for Class: customSQLReportIndex
//
//===============================================================
customSQLReportIndex::customSQLReportIndex()
{
    validTitle_ = false;      
    activeSqlReports_ = false;
    indexFile_  = new wxTextFile(mmex::getCustomReportIndexFilename());  
    if (indexFile_->Exists())
    {
        indexFile_ ->Open();
        if (indexFile_->GetLineCount() > 4 )
            setSQLReportsActive();
    }
}

bool customSQLReportIndex::hasActiveSQLReports()
{
    return activeSqlReports_;
}

void customSQLReportIndex::setSQLReportsActive()
{
    activeSqlReports_ = true;
}

void customSQLReportIndex::resetReportsIndex()
{
    indexFile_->GetFirstLine(); // File Heading line
    indexFile_->GetNextLine();  // Blank Line
    indexFile_->GetNextLine();  // Headings Line
    indexFile_->GetNextLine();  // Separator line
}

bool customSQLReportIndex::validTitle()
{
    return validTitle_;
}

wxString customSQLReportIndex::nextReportTitle()
{
    validTitle_ = false;
    if (! indexFile_->Eof() )
    {
        wxString line = indexFile_->GetNextLine();
        if (line != wxT(""))
        {
            wxStringTokenizer tk(wxStringTokenizer(line, wxT(":")));
            currentReportName_ = tk.GetNextToken();
            currentFileName_ = tk.GetNextToken();
            validTitle_ = true;
        }
    }
    return currentReportName_;
}

wxString customSQLReportIndex::reportTitle()
{
    return currentReportName_;
}

wxString customSQLReportIndex::reportFileName(int index)
{
    resetReportsIndex();

    int currentLine = 0;
    while (currentLine <= index)
    {
        nextReportTitle();
        currentLine ++;
    }
    return currentFileName_;
}

bool customSQLReportIndex::initIndexFileHeader()
{
    bool result = false;
    if (!indexFile_->Exists())
    {
        result = indexFile_->Create();
        if (result)
        {
            indexFile_->AddLine(_("Custom SQL Reports - Current limit: 10"));
            indexFile_->AddLine(wxT(""));
            indexFile_->AddLine(_("Report Name:Report Filename.sql"));
            indexFile_->AddLine(wxT("========================================"));
            indexFile_->AddLine(wxT("My First Report:My First Report.sql"));
            indexFile_->Write();
            resetReportsIndex();
            setSQLReportsActive();
        }
    }
    return result;
}

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

#include "customreportindex.h"
#include "paths.h"
#include <wx/tokenzr.h>

//===============================================================
// Methods for Class: customSQLReportIndex
//
//===============================================================
customSQLReportIndex::customSQLReportIndex()
{
    validTitle_ = false;      
    activeSqlReports_ = false;
    userSelectedFileIndex_ = 0;
    indexFile_  = new wxTextFile(mmex::getPathUser(mmex::CUSTOM_REPORTS));  
    if (indexFile_->Exists())
    {
        indexFile_ ->Open();
        if (indexFile_->GetLineCount() >= 4 )
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
    if (hasActiveSQLReports())
    {
        indexFile_->GetFirstLine(); // File Heading line
        indexFile_->GetNextLine();  // Blank Line
        indexFile_->GetNextLine();  // Headings Line
        indexFile_->GetNextLine();  // Separator line
    }
}

bool customSQLReportIndex::validTitle()
{
    return validTitle_;
}

bool customSQLReportIndex::reportIsSubReport()
{
    return reportIsSubReport_;
}

wxString customSQLReportIndex::nextReportTitle()
{
    validTitle_         = false;
    reportIsSubReport_  = false;

    if (! indexFile_->Eof() )
    {
        wxString line = indexFile_->GetNextLine();
        if (line != wxT(""))
        {
            wxStringTokenizer tk(wxStringTokenizer(line, wxT(":")));
            currentReportTitle_ = tk.GetNextToken();
            currentReportFileName_ = tk.GetNextToken();
            if (tk.HasMoreTokens())
            {
                reportIsSubReport_ = true;
            }
            validTitle_ = true;
        }
    }
    return currentReportTitle_;
}

wxString customSQLReportIndex::currentReportTitle()
{
    return currentReportTitle_;
}

wxString customSQLReportIndex::currentReportFileName()
{
    return wxString() << mmex::getPathUser(mmex::DIRECTORY) << currentReportFileName_;
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
    return currentReportFileName();
}

bool customSQLReportIndex::initIndexFileHeader()
{
    bool result = false;
    if (!indexFile_->Exists())
    {
        result = indexFile_->Create();
        if (result)
        {
            indexFile_->AddLine(_("Custom SQL Reports"));
            indexFile_->AddLine(wxT(""));
            indexFile_->AddLine(_("Report Name:Report Filename.sql[:SUB]"));
            indexFile_->AddLine(wxT("========================================"));
            indexFile_->AddLine(wxT("My First Report:My First Report.sql"));
            indexFile_->Write();
            resetReportsIndex();
            setSQLReportsActive();
        }
    }
    return result;
}

void customSQLReportIndex::LoadArrays(wxArrayString& titleArray, wxArrayString& fileNameArray)
{
    resetReportsIndex();
    nextReportTitle();
    while (validTitle())
    {
        titleArray.Add(currentReportTitle_);
        fileNameArray.Add(currentReportFileName_);
        nextReportTitle();
    }
}

wxString customSQLReportIndex::UserDialogHeading()
{
    return _("Custom SQL Reports");
}

void customSQLReportIndex::getUserTitleSelection(wxString description)
{
    wxArrayString reportTitles;
    wxArrayString reportFileNames;
    LoadArrays(reportTitles, reportFileNames);

    validTitle_ = false;
    wxString msgStr = wxString() << _("Select the Custom SQL Report")<< description;
    wxSingleChoiceDialog reportTitleSelectionDlg(0,msgStr, UserDialogHeading(), reportTitles);
    if (reportTitleSelectionDlg.ShowModal() == wxID_OK)
    {
        int selectionIndex_    = reportTitleSelectionDlg.GetSelection();
        if (selectionIndex_ > -1 )
        {
            currentReportTitle_    = reportTitles.Item(selectionIndex_);
            currentReportFileName_ = reportFileNames.Item(selectionIndex_);
            userSelectedFileIndex_ = selectionIndex_ + 4;  // Add number of header lines in file.
            validTitle_ = true;
        }
    }
}

void customSQLReportIndex::setUserTitleSelection(wxString titleIndex)
{
    long index; 
    wxString indexStr = titleIndex.Right(1);
    indexStr.ToLong(&index);

    wxArrayString reportTitles;
    wxArrayString reportFileNames;
    LoadArrays(reportTitles, reportFileNames);

    currentReportTitle_    = reportTitles.Item(index);
    currentReportFileName_ = reportFileNames.Item(index);
    userSelectedFileIndex_ = index + 4;  // Add number of header lines in file.
}

void customSQLReportIndex::deleteSelectedReportTitle()
{
    indexFile_->RemoveLine(userSelectedFileIndex_);
    indexFile_->Write();
}

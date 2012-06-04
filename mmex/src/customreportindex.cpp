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
//===============================================================
customSQLReportIndex::customSQLReportIndex()
:activeSqlReports_ (false), validTitle_ (false), reportIsSubReport_(false), currentReportFileIndex_(0)      
{
    indexFile_  = new wxTextFile(mmex::getPathUser(mmex::CUSTOM_REPORTS));  
    if (indexFile_->Exists())
    {
        indexFile_ ->Open();
        if (indexFile_->GetLineCount() >= 4 )
            activeSqlReports_ = true;
    }
}

void customSQLReportIndex::initIndexFileHeader()
{
    if (!indexFile_->Exists())
    {
        bool result = indexFile_->Create();
        if (result)
        {
            indexFile_->AddLine(_("Custom SQL Reports"));
            indexFile_->AddLine((""));
            indexFile_->AddLine(_("Report Name[|Report Filename.sql[|SUB]]"));
            indexFile_->AddLine(("========================================"));
//            indexFile_->AddLine(("My First Report|My First Report.sql"));
//            indexFile_->AddLine(("MY REPORT HEADING"));
//            indexFile_->AddLine(("My Second Report|My Second Report.sql|SUB"));
            indexFile_->Write();
            activeSqlReports_ = true;
        }
    }
}

bool customSQLReportIndex::hasActiveSQLReports()
{
    return activeSqlReports_;
}

void customSQLReportIndex::resetReportsIndex()
{
    if (activeSqlReports_)
    {
        indexFile_->GetFirstLine(); // File Heading line
        indexFile_->GetNextLine();  // Blank Line
        indexFile_->GetNextLine();  // Headings Line
        indexFile_->GetNextLine();  // Separator line
        currentReportFileIndex_ = 4;
        validTitle_         = false;
        reportIsSubReport_  = false;
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
        wxString line = indexFile_->GetNextLine().Trim(false);
        currentReportFileIndex_ ++;
        if (!line.IsEmpty())
        {
            wxStringTokenizer tk(line, ("|"));
            currentReportTitle_     = tk.GetNextToken();
            currentReportFileName_  = tk.GetNextToken();
            if (tk.HasMoreTokens())
                reportIsSubReport_ = true;
            validTitle_ = true;
        }
    }
    return currentReportTitle_;
}

wxString customSQLReportIndex::currentReportTitle()
{
    return currentReportTitle_;
}

wxString customSQLReportIndex::currentReportFileName(bool withfilePath)
{
    wxString returnStr = currentReportFileName_;
    if ( ! currentReportFileName_.IsEmpty() )
    {
        if (withfilePath)
            returnStr = wxString() << mmex::getPathUser(mmex::DIRECTORY) << currentReportFileName_; 
    }
    return returnStr;
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

void customSQLReportIndex::LoadArrays(wxArrayString& titleArray, wxArrayString& fileNameArray, wxArrayString& subArray)
{
    resetReportsIndex();
    nextReportTitle();
    while (validTitle())
    {
        titleArray.Add(currentReportTitle_);
        fileNameArray.Add(currentReportFileName_);
        if (reportIsSubReport_)
            subArray.Add(("SUB"));
        else
            subArray.Add(("NS"));
        nextReportTitle();
    }
}

wxString customSQLReportIndex::UserDialogHeading()
{
    return _("Custom SQL Reports");
}

wxString customSQLReportIndex::getUserTitleSelection(wxString description)
{
    wxArrayString reportTitles;
    wxArrayString reportFileNames;
    wxArrayString reportIsSub;

    LoadArrays(reportTitles, reportFileNames, reportIsSub);

    validTitle_ = false;
    wxString msgStr = wxString() << _("Select the Custom SQL Report") << description;
    wxSingleChoiceDialog reportTitleSelectionDlg(0,msgStr, UserDialogHeading(), reportTitles);

    int selectionIndex = -1;
    if (reportTitleSelectionDlg.ShowModal() == wxID_OK)
    {
        selectionIndex = reportTitleSelectionDlg.GetSelection();
        if (selectionIndex > -1 )
        {
            currentReportTitle_     = reportTitles.Item(selectionIndex);
            currentReportFileName_  = reportFileNames.Item(selectionIndex);
            validTitle_ = true;
            if ( reportIsSub.Item(selectionIndex) == ("SUB"))
                reportIsSubReport_ = true;
            else
                reportIsSubReport_ = false;
        }
    }
    else
    {
        selectionIndex = reportTitles.GetCount();
    }

    currentReportFileIndex_ = selectionIndex + 4;  // Add File header line Count.

    return wxString() << ("Custom_Report_") << selectionIndex;
}

void customSQLReportIndex::getSelectedTitleSelection(wxString titleIndex)
{
    long index; 
    wxStringTokenizer tk(titleIndex, ("_")); // get the 3rd token 'Custom_Report_xx'   
    tk.GetNextToken();
    tk.GetNextToken();
    wxString indexStr = tk.GetNextToken();
    indexStr.ToLong(&index);

    wxArrayString reportTitles;
    wxArrayString reportFileNames;
    wxArrayString reportIsSub;
    LoadArrays(reportTitles, reportFileNames, reportIsSub);

    currentReportTitle_     = reportTitles.Item(index);
    currentReportFileName_  = reportFileNames.Item(index);
    currentReportFileIndex_ = index + 4;  // Add number of header lines in file.
    validTitle_ = true;
    if ( reportIsSub.Item(index) == ("SUB"))
        reportIsSubReport_ = true;
    else
        reportIsSubReport_ = false;
}

void customSQLReportIndex::addReportTitle(wxString reportTitle, bool updateIndex, wxString reportFileName, bool isSub)
{
    if (!reportTitle.IsEmpty())
    {
        wxString indexLine = reportTitle;
        if (!reportFileName.IsEmpty())
        {
            indexLine = indexLine << ("|") << reportFileName;
            if (isSub)
                indexLine = indexLine << ("|SUB");
        }

        if (updateIndex)
        {
            indexFile_->RemoveLine(currentReportFileIndex_);
        }

        indexFile_->InsertLine(indexLine, currentReportFileIndex_);
        indexFile_->Write();
        indexFile_->Close();
        indexFile_->Open();
    }
}

bool customSQLReportIndex::ReportListHasItems()
{
    bool result = false;
    int lineCount = indexFile_->GetLineCount();
    if ( lineCount > 4 )
        result = true;

    return result;
}

void customSQLReportIndex::deleteSelectedReportTitle()
{
    indexFile_->RemoveLine(currentReportFileIndex_);
    indexFile_->Write();
}

bool customSQLReportIndex::getSqlFileData(wxString& sqlText)
{
    bool result = false;
    wxTextFile sqlFile(currentReportFileName());  
    if ( sqlFile.Exists() )
    {
        sqlFile.Open();
        sqlText << sqlFile.GetFirstLine(); 
        while (! sqlFile.Eof())
        {
            wxString nextLine = sqlFile.GetNextLine();
            if (! nextLine.IsEmpty())
                sqlText << ("\n"); 
            sqlText << nextLine;
        }
        result = true;
    }
    else
    {
        wxString msg = wxString() << _("Cannot locate file: ") << currentReportFileName() << ("\n\n");
        wxMessageBox(msg,UserDialogHeading(),wxOK|wxICON_ERROR);
    }

    return result;
}

//customSQLReportIndex::~customSQLReportIndex()
//{
//    wxMessageBox(("Testing that customSQLReportIndex objects are being destroyed.\nGoodby.."),("customSQLReportIndex Dialog Destructor..."));
//}

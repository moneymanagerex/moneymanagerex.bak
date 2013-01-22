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
CustomReportIndex::CustomReportIndex()
: activeReports_ (false)
, validTitle_ (false)
, reportIsSubReport_(false)
, currentReportFileIndex_(0)
{
    indexFile_  = new wxTextFile(mmex::getPathUser(mmex::CUSTOM_REPORTS));
    if (indexFile_->Exists())
    {
        indexFile_ ->Open();
		activeReports_ = ReportListHasItems();
    }
    else
    {
        if (indexFile_->Create())
        {
            indexFile_->AddLine(_("Custom Reports"));
            indexFile_->AddLine(wxT(""));
            indexFile_->AddLine(_("Report Name[|Report Filename.sql[|SUB]]"));
            indexFile_->AddLine(wxT("========================================"));
            indexFile_->Write();
            indexFile_->Close();    // need to close to actually save the file contents   
            indexFile_->Open();
        }
    }
}

bool CustomReportIndex::HasActiveReports()
{
    return activeReports_;
}

void CustomReportIndex::ResetReportsIndex()
{
    if (activeReports_)
    {
        indexFile_->GetFirstLine(); // File Heading line
        indexFile_->GetNextLine();  // Blank Line
        indexFile_->GetNextLine();  // Headings Line
        indexFile_->GetNextLine();  // Separator line
        currentReportFileIndex_ = 0;
        validTitle_         = false;
        reportIsSubReport_  = false;
    }
}

bool CustomReportIndex::ValidTitle()
{
    return validTitle_;
}

bool CustomReportIndex::ReportIsSubReport()
{
    return reportIsSubReport_;
}

wxString CustomReportIndex::NextReportTitle()
{
    validTitle_         = false;
    reportIsSubReport_  = false;

    if (! indexFile_->Eof() )
    {
        wxString line = indexFile_->GetNextLine().Trim(false);
        if (!line.IsEmpty())
        {
            currentReportFileIndex_ ++;
            wxStringTokenizer tk(line, wxT("|"));
            currentReportTitle_    = tk.GetNextToken();
            currentReportFileName_ = tk.GetNextToken();
            SetNewCurrentFileValues();

            if (tk.HasMoreTokens())
            {
                reportIsSubReport_ = true;
            }
            validTitle_ = true;
        }
    }

    return currentReportTitle_;
}

wxString CustomReportIndex::CurrentReportTitle()
{
    return currentReportTitle_;
}

wxString CustomReportIndex::CurrentReportFileName(bool withfilePath)
{
    wxString returnStr = currentReportFileName_;
    if ( ! currentReportFileName_.IsEmpty() )
    {
        if (withfilePath)
        {
            returnStr = wxString() << mmex::getPathUser(mmex::DIRECTORY) << currentReportFileName_;
        }
    }
    return returnStr;
}

wxString CustomReportIndex::CurrentReportFileExt()
{
    return currentReportFileExt_;
}

wxString CustomReportIndex::CurrentReportFileType()
{
    return currentReportFileType_;
}

wxString CustomReportIndex::ReportFileName(int index)
{
    ResetReportsIndex();

    int currentLine = 0;

// TODO: Find a solution to allow deleting last file in report manager and prevent crash
//       without rendering report 0 inoperative in main navigation.
//       
//    if (index > 0)
//    {
        while (currentLine <= index)
        {
            NextReportTitle();
            currentLine ++;
        }
//    }
//    else
//    {
//        currentReportFileName_ = wxEmptyString;
//    }

    return CurrentReportFileName();
}

void CustomReportIndex::LoadArrays(wxArrayString& titleArray, wxArrayString& fileNameArray, wxArrayString& subArray)
{
    ResetReportsIndex();
    NextReportTitle();
    while (ValidTitle())
    {
        titleArray.Add(currentReportTitle_);
        fileNameArray.Add(currentReportFileName_);
        if (reportIsSubReport_)
            subArray.Add(wxT("SUB"));
        else
            subArray.Add(wxT("NS"));
        NextReportTitle();
    }
}

wxString CustomReportIndex::UserDialogHeading()
{
    return _("Custom Reports");
}

void CustomReportIndex::SetNewCurrentFileValues()
{
    wxFileName fn(currentReportFileName_);
    currentReportFileExt_ = fn.GetExt();
    currentReportFileExt_.MakeUpper();
    if (currentReportFileExt_ == wxT("SQL"))
        currentReportFileType_ = wxT("SQL");
    else if (currentReportFileExt_ == wxT("LUA"))
        currentReportFileType_ = wxT("LUA");
    else 
        currentReportFileType_ = wxT("");
}

wxString CustomReportIndex::GetUserTitleSelection(wxString description)
{
    wxArrayString reportTitles;
    wxArrayString reportFileNames;
    wxArrayString reportIsSub;

    LoadArrays(reportTitles, reportFileNames, reportIsSub);

    validTitle_ = false;
    wxString msgStr = wxString::Format(_("Select the Custom Report %s"), description.c_str());
    wxSingleChoiceDialog reportTitleSelectionDlg(0,msgStr, UserDialogHeading(), reportTitles);

    int selectionIndex = -1;
    if (reportTitleSelectionDlg.ShowModal() == wxID_OK)
    {
        selectionIndex = reportTitleSelectionDlg.GetSelection();
        if (selectionIndex > -1 )
        {
            currentReportTitle_     = reportTitles.Item(selectionIndex);
            currentReportFileName_  = reportFileNames.Item(selectionIndex);
            SetNewCurrentFileValues();
            validTitle_ = true;
            if ( reportIsSub.Item(selectionIndex) == wxT("SUB"))
                reportIsSubReport_ = true;
            else
                reportIsSubReport_ = false;
        }
    }
    else
    {
        selectionIndex = reportTitles.GetCount();
    }

    currentReportFileIndex_ = selectionIndex;  // Add File header line Count.

    return wxString() << wxT("Custom_Report_") << selectionIndex;
}

bool CustomReportIndex::GetSelectedTitleSelection(wxString titleIndex)
{
    long index;
    wxStringTokenizer tk(titleIndex, wxT("_")); // get the 3rd token 'Custom_Report_xx'
    if (tk.HasMoreTokens()) tk.GetNextToken(); else return false;
    if (tk.HasMoreTokens()) tk.GetNextToken(); else return false;
    wxString indexStr = tk.GetNextToken();
    indexStr.ToLong(&index);

    wxArrayString reportTitles;
    wxArrayString reportFileNames;
    wxArrayString reportIsSub;
    LoadArrays(reportTitles, reportFileNames, reportIsSub);

    currentReportTitle_     = reportTitles.Item(index);
    currentReportFileName_  = reportFileNames.Item(index);
    SetNewCurrentFileValues();
    currentReportFileIndex_ = index;  // Add number of header lines in file.
    validTitle_ = true;
    if ( reportIsSub.Item(index) == wxT("SUB"))
        reportIsSubReport_ = true;
    else
        reportIsSubReport_ = false;
    return true;
}

void CustomReportIndex::AddReportTitle(wxString reportTitle, bool updateIndex, wxString ReportFileName, bool isSub)
{
    if (!reportTitle.IsEmpty())
    {
        wxString indexLine = reportTitle;
        if (!ReportFileName.IsEmpty())
        {
            indexLine = indexLine << wxT("|") << ReportFileName;
            reportIsSubReport_ = isSub;
            if (reportIsSubReport_)
            {
                indexLine = indexLine << wxT("|SUB");
            }
        }

        if (updateIndex)
        {
            indexFile_->RemoveLine(currentReportFileIndex_ + 4);
        }

        indexFile_->InsertLine(indexLine, currentReportFileIndex_ + 4);
        indexFile_->Write();
        indexFile_->Close();
        indexFile_->Open();
        activeReports_ = true;
    }
}

bool CustomReportIndex::ReportListHasItems()
{
    size_t lineCount = indexFile_->GetLineCount();
    return ( lineCount > 4 );
}

void CustomReportIndex::DeleteSelectedReportTitle()
{
    indexFile_->RemoveLine(currentReportFileIndex_ + 4);
    indexFile_->Write();
}

bool CustomReportIndex::GetReportFileData(wxString& reportText)
{
    bool result = false;
    wxTextFile reportFile(CurrentReportFileName());
    if ( reportFile.Exists() )
    {
        reportFile.Open();
        reportText << reportFile.GetFirstLine() << wxT("\n");

        size_t currentline = 1;
        while (! reportFile.Eof())
        {
            reportText << reportFile.GetNextLine();
            currentline ++;
            if (currentline < reportFile.GetLineCount())
            {
                reportText << wxT("\n");
            }
        }

        result = true;
    }
    else
    {
        wxString msg = wxString() << _("Cannot locate file: ") << CurrentReportFileName() << wxT("\n\n");
        wxMessageBox(msg,UserDialogHeading(),wxOK|wxICON_ERROR);
    }

    return result;
}

//CustomReportIndex::~CustomReportIndex()
//{
//    wxMessageBox(wxT("Testing that CustomReportIndex objects are being destroyed.\nGoodby.."),wxT("CustomReportIndex Dialog Destructor..."));
//}

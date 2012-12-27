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
#ifndef _MM_EX_CUSTOMREPORTINDEX_H_
#define _MM_EX_CUSTOMREPORTINDEX_H_

#include "mmcoredb.h"

//===============================================================
// Class: customSQLReportIndex
//        Handles the text index file for custom reports  
//===============================================================
class customSQLReportIndex
{
public:
    customSQLReportIndex();
    
//    virtual ~customSQLReportIndex();

    /** Create the index file if the file does not exist.
        sets hasActiveSQLReports() to true if successful. */
    void initIndexFileHeader();
    
    /** Returns true if file is initialised and open */
    bool hasActiveSQLReports();

    /**  Sets the index file pointer at beginning of file */ 
    void resetReportsIndex();

    /** Sets and Returns the report title. Reads one line of the index file.
        Sets: validTitle_ currentReportTitle_ currentReportFileName_ reportIsSubReport_ */
    wxString nextReportTitle();
    
    /** Sets and Returns the filename for the given index.  */
    wxString reportFileName(int index);

    /** Valid after calling: nextReportTitle() */
    bool validTitle();

    /** Returns the report title.
        Valid after calling: : nextReportTitle() or reportFileName() */
    wxString currentReportTitle();

    /** Returns true if report is a subreport title.
        Valid after calling: : nextReportTitle() or reportFileName() */
    bool reportIsSubReport();

    /** Returns the report filename.
        Valid after calling: : nextReportTitle() or reportFileName() */
    wxString currentReportFileName(bool withfilePath = true);
    wxString currentReportFileExt();
    wxString currentReportFileType();

    /** Return index of user selected report title as ID: Custom_Report_xxx.
        sets: validTitle_ currentReportTitle_ currentReportFileName_ currentReportFileIndex_ */
    wxString getUserTitleSelection(wxString description = wxT(":"));

    /** Gets the report title from the given ID: Custom_Report_xxx.
        sets: currentReportTitle_ currentReportFileName_ currentReportFileIndex_  */
    void getSelectedTitleSelection(wxString titleIndex);

    /** Adds a new listing to the index file at the currentReportFileIndex_
        call getUserTitleSelection() or getSelectedTitleSelection() first. */
    void addReportTitle(wxString reportTitle, bool updateIndex, wxString reportFileName = wxT(""), bool isSub = false);

    /** Returns true if the report list is not empty */
    bool ReportListHasItems();

    /** Removes the listing from the index file at location: currentReportFileIndex_  */
    void deleteSelectedReportTitle();

    /** Returns the title header string for User Dialog */
    wxString UserDialogHeading(); 

    /** Returns the file contents if the .SQL file */ 
    bool getSqlFileData(wxString& sqlText);

private:
    bool activeSqlReports_;
    bool validTitle_;
    bool reportIsSubReport_;
    int  currentReportFileIndex_;

    wxTextFile* indexFile_; 
    wxString currentReportTitle_;
    wxString currentReportFileName_;
    wxString currentReportFileExt_;
    wxString currentReportFileType_;

    void SetNewCurrentFileValues();
    void LoadArrays(wxArrayString& titleArray, wxArrayString& fileNameArray, wxArrayString& subArray);
};

#endif


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

#include "reportbase.h"
#include "mmcoredb.h"

//===============================================================
// Class: customSQLReportIndex
//        Handles the text index file for custom reports  
//===============================================================
class customSQLReportIndex
{
public:
    customSQLReportIndex();

    bool hasActiveSQLReports();
    void setSQLReportsActive();
    bool reportIsSubReport();

    bool initIndexFileHeader();
    void resetReportsIndex();
    bool validTitle();
    wxString nextReportTitle();
    wxString reportFileName(int index);
    wxString currentReportTitle();
    wxString currentReportFileName();

    wxString UserDialogHeading(); 
    void getUserTitleSelection(wxString description = wxT(":"));
    void setUserTitleSelection(wxString titleIndex);
    void deleteSelectedReportTitle();

private:
    bool activeSqlReports_;
    wxTextFile* indexFile_; 
    bool validTitle_;
    bool reportIsSubReport_;
    wxString currentReportTitle_;
    wxString currentReportFileName_;
    int userSelectedFileIndex_;

    void LoadArrays(wxArrayString& titleArray, wxArrayString& fileNameArray);

};


#endif


/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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
 /*******************************************************/

#ifndef _MM_EX_APPSTARTDIALOG_H_
#define _MM_EX_APPSTARTDIALOG_H_

#include "defs.h"
#include "guiid.h"
#include "util.h"

#define SYMBOL_MMAPPSTARTDIALOG_STYLE    wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MMAPPSTARTDIALOG_TITLE    mmIniOptions::appName_ + _(" Start Page")
#define SYMBOL_MMAPPSTARTDIALOG_IDNAME   ID_DIALOG_APPSTART
#define SYMBOL_MMAPPSTARTDIALOG_SIZE     wxSize(400, 300)
#define SYMBOL_MMAPPSTARTDIALOG_POSITION wxDefaultPosition

class mmAppStartDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmAppStartDialog )
    DECLARE_EVENT_TABLE()

public:
    mmAppStartDialog( );
    mmAppStartDialog( wxSQLite3Database* inidb, wxWindow* parent, 
        wxWindowID id = SYMBOL_MMAPPSTARTDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_MMAPPSTARTDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_MMAPPSTARTDIALOG_POSITION, 
        const wxSize& size = SYMBOL_MMAPPSTARTDIALOG_SIZE, 
        long style = SYMBOL_MMAPPSTARTDIALOG_STYLE );
    ~mmAppStartDialog();

    bool Create( wxWindow* parent, 
        wxWindowID id = SYMBOL_MMAPPSTARTDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_MMAPPSTARTDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_MMAPPSTARTDIALOG_POSITION, 
        const wxSize& size = SYMBOL_MMAPPSTARTDIALOG_SIZE, 
        long style = SYMBOL_MMAPPSTARTDIALOG_STYLE );

    void CreateControls();

    int getReturnCode() { return retCode_; }
    void OnButtonAppstartOpenDatabaseClick( wxCommandEvent& event );
    void OnButtonAppstartNewDatabaseClick( wxCommandEvent& event );
    void OnButtonAppstartHelpClick( wxCommandEvent& event );
    void OnButtonAppstartWebsiteClick( wxCommandEvent& event );
    void OnButtonAppstartLastDatabaseClick( wxCommandEvent& event );

private:
    int retCode_;
    wxSQLite3Database* inidb_;
    wxCheckBox* itemCheckBox;
};

#endif

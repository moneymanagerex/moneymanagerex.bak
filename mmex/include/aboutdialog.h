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
#ifndef _MM_EX_ABOUTDIALOG_H_
#define _MM_EX_ABOUTDIALOG_H_

#include "guiid.h"

#define SYMBOL_ABOUTDIALOG_STYLE    wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX
#define SYMBOL_ABOUTDIALOG_TITLE    _("About Money Manager Ex")
#define SYMBOL_ABOUTDIALOG_IDNAME   ID_DIALOG_ABOUT
#define SYMBOL_ABOUTDIALOG_SIZE     wxSize(500, 220)
#define SYMBOL_ABOUTDIALOG_POSITION wxDefaultPosition

#define ID_DIALOG_BUTTON_ABOUT_CONTRIBUTERS 5000

class mmAboutDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmTransDialog )
    DECLARE_EVENT_TABLE()

public:
    mmAboutDialog();
    mmAboutDialog(wxSQLite3Database* inidb, wxWindow* parent, 
        wxWindowID id = SYMBOL_ABOUTDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_ABOUTDIALOG_TITLE, 
        const wxPoint& pos      = SYMBOL_ABOUTDIALOG_POSITION, 
        const wxSize& size      = SYMBOL_ABOUTDIALOG_SIZE, 
        long style              = SYMBOL_ABOUTDIALOG_STYLE );

    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_ABOUTDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_ABOUTDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_ABOUTDIALOG_POSITION, 
        const wxSize& size = SYMBOL_ABOUTDIALOG_SIZE, 
        long style = SYMBOL_ABOUTDIALOG_STYLE );

    void CreateControls();
    void OnVersionHistory(wxCommandEvent& event);
    void OnContributerList(wxCommandEvent& event);

private:
    wxSQLite3Database* inidb_; 
};

#endif

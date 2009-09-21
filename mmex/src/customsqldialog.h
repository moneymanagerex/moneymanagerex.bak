/*******************************************************
 Copyright (C) 2007 Madhan Kanagavel

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

#ifndef _MM_EX_CUSTOMSQLDIALOG_H_
#define _MM_EX_CUSTOMSQLDIALOG_H_

#include "guiid.h"
#include "defs.h"
#include "dbwrapper.h"
#include "mmcoredb.h"

#define ID_MYDIALOG34 10042
#define SYMBOL_CUSTOMSQLDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_CUSTOMSQLDIALOG_TITLE _("Custom SQL Dialog")
#define SYMBOL_CUSTOMSQLDIALOG_IDNAME ID_MYDIALOG34
#define SYMBOL_CUSTOMSQLDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_CUSTOMSQLDIALOG_POSITION wxDefaultPosition
#define ID_TEXTBOX 10090
#define ID_PANEL10 10091
#define ID_CUSTOMSQLBUTTON_QUERY 10094
#define ID_CUSTOMSQLBUTTON_LOAD 10095
#define ID_CUSTOMSQLBUTTON_SAVE 10096
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

class mmCustomSQLDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmCustomSQLDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    mmCustomSQLDialog( );
    mmCustomSQLDialog( mmCoreDB* core, wxWindow* parent, 
        wxWindowID id = SYMBOL_CUSTOMSQLDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_CUSTOMSQLDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_CUSTOMSQLDIALOG_POSITION, 
        const wxSize& size = SYMBOL_CUSTOMSQLDIALOG_SIZE, 
        long style = SYMBOL_CUSTOMSQLDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, 
        wxWindowID id = SYMBOL_CUSTOMSQLDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_CUSTOMSQLDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_CUSTOMSQLDIALOG_POSITION, 
        const wxSize& size = SYMBOL_CUSTOMSQLDIALOG_SIZE, 
        long style = SYMBOL_CUSTOMSQLDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    
    void OnLoad(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
	void OnQuery(wxCommandEvent& event);

    wxBitmap GetBitmapResource( const wxString& name );
    wxIcon GetIconResource( const wxString& name );
    static bool ShowToolTips();

    wxString sqlQuery_;

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;

    wxTextCtrl* queryBox_;
};

#endif

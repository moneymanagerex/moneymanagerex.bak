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

#ifndef _MM_EX_OPTIONSDIALOG_H_
#define _MM_EX_OPTIONSDIALOG_H_

#include "guiid.h"
#include "mmcoredb.h"

#define SYMBOL_MMOPTIONSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MMOPTIONSDIALOG_TITLE _("Options Dialog")
#define SYMBOL_MMOPTIONSDIALOG_IDNAME ID_DIALOG_OPTIONS
#define SYMBOL_MMOPTIONSDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_MMOPTIONSDIALOG_POSITION wxDefaultPosition

class mmOptionsDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmOptionsDialog )
    DECLARE_EVENT_TABLE()

public:
    mmOptionsDialog( );
    mmOptionsDialog( mmCoreDB* db, wxSQLite3Database* inidb,
        wxWindow* parent, wxWindowID id = SYMBOL_MMOPTIONSDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_MMOPTIONSDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_MMOPTIONSDIALOG_POSITION, 
        const wxSize& size = SYMBOL_MMOPTIONSDIALOG_SIZE, 
        long style = SYMBOL_MMOPTIONSDIALOG_STYLE );
    ~mmOptionsDialog( );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MMOPTIONSDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_MMOPTIONSDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_MMOPTIONSDIALOG_POSITION, 
        const wxSize& size = SYMBOL_MMOPTIONSDIALOG_SIZE, 
        long style = SYMBOL_MMOPTIONSDIALOG_STYLE );

    void CreateControls();

    void OnCurrency(wxCommandEvent& event);
    void OnDateFormatChanged(wxCommandEvent& event);
    void OnViewAccountsChanged(wxCommandEvent& event);
	void OnLanguageChanged(wxCommandEvent& event);
    void OnNavTreeColorChanged(wxCommandEvent& event);

    void OnAlt0Changed(wxCommandEvent& event);
    void OnAlt1Changed(wxCommandEvent& event);
    void OnListBackgroundChanged(wxCommandEvent& event);
    void OnListBorderChanged(wxCommandEvent& event);
    void OnRestoreDefaultColors(wxCommandEvent& event);
    void OnListDetailsColors(wxCommandEvent& event);
    void OnBackupDBChecked(wxCommandEvent& event);

private:
    wxImageList* m_imageList;
    wxChoice* choiceDateFormat_;
    wxChoice* choiceVisible_;
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    wxSQLite3Database* inidb_;
};

#endif

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

#ifndef _MM_EX_CATEGDIALOG_H_
#define _MM_EX_CATEGDIALOG_H_

#define SYMBOL_CATEGDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxRESIZE_BORDER
#define SYMBOL_CATEGDIALOG_TITLE _("Organize Categories")
#define SYMBOL_CATEGDIALOG_IDNAME ID_DIALOG_CATEGORY
#define SYMBOL_CATEGDIALOG_SIZE wxSize(500, 300)
#define SYMBOL_CATEGDIALOG_POSITION wxDefaultPosition

#include "guiid.h"
#include "defs.h"
#include "dbwrapper.h"
#include "mmcoredb.h"

class mmTreeItemCateg : public wxTreeItemData
{
public:
    mmTreeItemCateg(int categID, int subcategID) 
        : categID_(categID),
          subcategID_(subcategID){ }
    int getCategID() { return categID_; }
    int getSubCategID() { return subcategID_; }

private:
    int categID_;
    int subcategID_;
};

class mmCategDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmCategDialog )
    DECLARE_EVENT_TABLE()

public:
    mmCategDialog();
    mmCategDialog(mmCoreDB* core, wxWindow* parent, 
        wxWindowID id = SYMBOL_CATEGDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_CATEGDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_CATEGDIALOG_POSITION, 
        const wxSize& size = SYMBOL_CATEGDIALOG_SIZE, 
        long style = SYMBOL_CATEGDIALOG_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CATEGDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_CATEGDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_CATEGDIALOG_POSITION, 
        const wxSize& size = SYMBOL_CATEGDIALOG_SIZE, 
        long style = SYMBOL_CATEGDIALOG_STYLE );

    void CreateControls();
    
    // utility functions
    void OnOk(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnBSelect(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    
    void fillControls();
    void OnSelChanged(wxTreeEvent& event);
    void OnDoubleClicked(wxTreeEvent& event);

private:
    wxSQLite3Database* db_;
    mmCoreDB* core_;
    wxTreeCtrl* treeCtrl_; 
    wxTreeItemId selectedItemId_;
    wxTreeItemId root_;

public:
    int categID_;
    int subcategID_;

};

#endif

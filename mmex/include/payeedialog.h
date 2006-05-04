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
#ifndef _MM_EX_PAYEEDIALOG_H_
#define _MM_EX_PAYEEDIALOG_H_

#define SYMBOL_PAYEEDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PAYEEDIALOG_TITLE _("Organize Payees")
#define SYMBOL_PAYEEDIALOG_IDNAME ID_DIALOG_PAYEE
#define SYMBOL_PAYEEDIALOG_SIZE wxSize(500, 300)
#define SYMBOL_PAYEEDIALOG_POSITION wxDefaultPosition

#include "guiid.h"
#include "dbwrapper.h"

class mmPayeeListBoxItem: public wxClientData
{
public:
    mmPayeeListBoxItem(int payeeID) 
        : payeeID_(payeeID){}
          
    int getPayeeID() { return payeeID_; }

private:
    int payeeID_;
};

class mmPayeeDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmPayeeDialog )
    DECLARE_EVENT_TABLE()

public:
    mmPayeeDialog();
    mmPayeeDialog(wxSQLite3Database* db, bool selectPayees, 
        wxWindow* parent, wxWindowID id = SYMBOL_PAYEEDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_PAYEEDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_PAYEEDIALOG_POSITION, 
        const wxSize& size = SYMBOL_PAYEEDIALOG_SIZE, 
        long style = SYMBOL_PAYEEDIALOG_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PAYEEDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_PAYEEDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_PAYEEDIALOG_POSITION, 
        const wxSize& size = SYMBOL_PAYEEDIALOG_SIZE, 
        long style = SYMBOL_PAYEEDIALOG_STYLE );

    void CreateControls();
    
    // utility functions
    void OnOk(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnBSelect(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    
    void fillControls();
    void OnSelChanged(wxCommandEvent& event);
    void OnDoubleClicked(wxCommandEvent& event);

private:
    wxSQLite3Database* db_;
    wxListBox* listBox_;
  
public:
    int payeeID_;
    bool selectPayees_;
};

#endif


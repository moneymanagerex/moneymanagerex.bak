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
 ********************************************************/

#ifndef _MM_EX_CURRENCYDIALOG_H_
#define _MM_EX_CURRENCYDIALOG_H_

#define SYMBOL_CURRENCYDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_CURRENCYDIALOG_TITLE _("Currency Manager")
#define SYMBOL_CURRENCYDIALOG_IDNAME ID_DIALOG_CURRENCY
#define SYMBOL_CURRENCYDIALOG_SIZE wxSize(500, 300)
#define SYMBOL_CURRENCYDIALOG_POSITION wxDefaultPosition

#include "guiid.h"
#include "defs.h"
#include "mmcoredb.h"
#include "dbwrapper.h"

class mmCurrencyDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmCurrencyDialog )
    DECLARE_EVENT_TABLE()

public:
    mmCurrencyDialog();
    ~mmCurrencyDialog();
    mmCurrencyDialog(mmCoreDB* core,
       int currencyID,
        wxWindow* parent, 
        wxWindowID id = SYMBOL_CURRENCYDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_CURRENCYDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_CURRENCYDIALOG_POSITION, 
        const wxSize& size = SYMBOL_CURRENCYDIALOG_SIZE, 
        long style = SYMBOL_CURRENCYDIALOG_STYLE );
    mmCurrencyDialog(mmCoreDB* core, wxWindow* parent, 
        wxWindowID id = SYMBOL_CURRENCYDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_CURRENCYDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_CURRENCYDIALOG_POSITION, 
        const wxSize& size = SYMBOL_CURRENCYDIALOG_SIZE, 
        long style = SYMBOL_CURRENCYDIALOG_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CURRENCYDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_CURRENCYDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_CURRENCYDIALOG_POSITION, 
        const wxSize& size = SYMBOL_CURRENCYDIALOG_SIZE, 
        long style = SYMBOL_CURRENCYDIALOG_STYLE );

    void CreateControls();
    
    // utility functions
    void OnCancel(wxCommandEvent& event);
    void OnBSelect(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnCurrencyTypeChanged(wxCommandEvent& event);
    void updateControls();
    
    void fillControls();
    void OnSelChanged(wxCommandEvent& event);

private:
    mmCoreDB* core_;
    wxChoice* currencyChoice_;
  
public:
    int currencyID_;
};

#endif

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

#ifndef _MM_EX_TRANSDIALOG_H_
#define _MM_EX_TRANSDIALOG_H_

#define SYMBOL_TRANSDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_TRANSDIALOG_TITLE _("New/Edit Transaction")
#define SYMBOL_TRANSDIALOG_IDNAME ID_DIALOG_TRANS
#define SYMBOL_TRANSDIALOG_SIZE wxSize(500, 300)
#define SYMBOL_TRANSDIALOG_POSITION wxDefaultPosition

#include "guiid.h"
#include "defs.h"
#include "mmcoredb.h"

class mmData;
class wxDatePickerCtrl;
class mmCategory;
class mmAdvancedPopup;

class mmTransDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmTransDialog )
    DECLARE_EVENT_TABLE()

public:
    mmTransDialog();
    mmTransDialog(wxSQLite3Database* db, 
        mmCoreDB* core,
        int accountID, int transID, 
        bool edit, 
        wxSQLite3Database* iniDB, 
        wxWindow* parent, 
        wxWindowID id = SYMBOL_TRANSDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_TRANSDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_TRANSDIALOG_POSITION, 
        const wxSize& size = SYMBOL_TRANSDIALOG_SIZE, 
        long style = SYMBOL_TRANSDIALOG_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TRANSDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_TRANSDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_TRANSDIALOG_POSITION, 
        const wxSize& size = SYMBOL_TRANSDIALOG_SIZE, 
        long style = SYMBOL_TRANSDIALOG_STYLE );

    void CreateControls();
    
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnCategs(wxCommandEvent& event);
    void OnPayee(wxCommandEvent& event);
    void OnTo(wxCommandEvent& event);
    void OnTransTypeChanged(wxCommandEvent& event);
    void OnDateChanged(wxDateEvent& event);
    void fillControls();
    void dataToControls();
    void updateControlsForTransType();
    bool getPayeeID(wxString payee, int& payeeID, int& categID, int& subcategID );
    void addPayee(wxString payee, int categID, int subcategID );
    void OnAdvanced(wxCommandEvent& event);
    void OnSplitChecked(wxCommandEvent& event);

private:
    wxSQLite3Database* db_;
    wxSQLite3Database* inidb_;
    mmCoreDB* core_;
    int transID_;
    int accountID_;
    wxTextCtrl *textNumber_;
    wxTextCtrl *textAmount_;
    wxTextCtrl *textNotes_;
    wxTextCtrl* textCategory_;
    wxButton* bCategory_;
    wxButton* bPayee_;
    wxButton* bTo_;
    mmAdvancedPopup* m_simplePopup_;
    wxButton* bAdvanced_;
    wxCheckBox* cSplit_;

    wxChoice* choiceStatus_;
    wxChoice* choiceTrans_;

    bool edit_;

    wxDatePickerCtrl* dpc_;
    int categID_;
    int subcategID_;
    int payeeID_;
    int toID_;
    double toTransAmount_;
    bool advancedToTransAmountSet_;
    boost::shared_ptr<mmSplitTransactionEntries> split_;
};

#endif

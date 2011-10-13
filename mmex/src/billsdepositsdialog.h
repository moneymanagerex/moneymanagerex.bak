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

#ifndef _MM_EX_BDDIALOG_H_
#define _MM_EX_BDDIALOG_H_

#define SYMBOL_BDDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_BDDIALOG_TITLE _("New/Edit Repeating Transaction")
#define SYMBOL_BDDIALOG_IDNAME ID_DIALOG_BD
#define SYMBOL_BDDIALOG_SIZE wxSize(500, 300)
#define SYMBOL_BDDIALOG_POSITION wxDefaultPosition

#include "guiid.h"
#include "defs.h"
#include "mmcoredb.h"

class mmData;
class wxDatePickerCtrl;
class mmCategory;
class mmAdvancedPopup;

class mmBDDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmBDDialog )
    DECLARE_EVENT_TABLE()

public:
    mmBDDialog();
    mmBDDialog(wxSQLite3Database* db, 
        mmCoreDB* core,
        int bdD, bool edit, bool enterOccur, wxWindow* parent, 
        wxWindowID id = SYMBOL_BDDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_BDDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_BDDIALOG_POSITION, 
        const wxSize& size = SYMBOL_BDDIALOG_SIZE, 
        long style = SYMBOL_BDDIALOG_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BDDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_BDDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_BDDIALOG_POSITION, 
        const wxSize& size = SYMBOL_BDDIALOG_SIZE, 
        long style = SYMBOL_BDDIALOG_STYLE );

    void CreateControls();
    
    // utility functions
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
    void OnAccountName(wxCommandEvent& event);
    void OnSplitChecked(wxCommandEvent& event);
    void OnAutoExecutionUserAckChecked(wxCommandEvent& event);
    void OnAutoExecutionSilentChecked(wxCommandEvent& event);
    void OnCalendarSelChanged(wxCalendarEvent& event);

private:
    wxSQLite3Database* db_;
    mmCoreDB* core_;
    
    wxTextCtrl *textNumber_;
    wxTextCtrl *textAmount_;
    wxTextCtrl *textNotes_;
    wxTextCtrl* textCategory_;
    wxTextCtrl* textNumRepeats_;
    wxButton* bCategory_;
    wxButton* bPayee_;
    wxButton* bTo_;
    mmAdvancedPopup* m_simplePopup_;
    wxButton* bAdvanced_;
    wxCheckBox* cSplit_;
    wxStaticText* staticTextAdvancedActive_;
    bool payeeUnknown_;

    wxChoice* choiceStatus_;
    wxChoice* choiceTrans_;
    wxButton* itemAccountName_;
    
    bool edit_;
    bool enterOccur_;

    wxDatePickerCtrl* dpc_;
    wxDatePickerCtrl* dpcbd_;
    wxChoice* itemRepeats_;
    wxCheckBox* itemCheckBoxAutoExeUserAck_;
    wxCheckBox* itemCheckBoxAutoExeSilent_;
    bool autoExecuteUserAck_;
    bool autoExecuteSilent_;

    int categID_;
    int subcategID_;
    int payeeID_;
    int accountID_;
    int toID_;
    int bdID_;
    double toTransAmount_;
    bool advancedToTransAmountSet_;
    boost::shared_ptr<mmSplitTransactionEntries> split_;
};

#endif

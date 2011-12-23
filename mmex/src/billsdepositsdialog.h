/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011 Nikolay & Stefano Giorgio

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
#include <wx/spinbutt.h>

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
    void dataToControls();
    void updateControlsForTransType();
    bool getPayeeID(wxString payee, int& payeeID, int& categID, int& subcategID );
    void addPayee(wxString payee, int categID, int subcategID );
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
    wxTextCtrl *toTextAmount_;
    wxTextCtrl *textNotes_;
    wxTextCtrl* textCategory_;
    wxTextCtrl* textNumRepeats_;
    wxButton* bCategory_;
    wxButton* bPayee_;
    wxButton* bTo_;
    mmAdvancedPopup* m_simplePopup_;
    wxCheckBox* cSplit_;
    wxCheckBox* cAdvanced_;
    bool payeeUnknown_;

    wxChoice* choiceStatus_;
    wxChoice* choiceTrans_;
    wxButton* itemAccountName_;
    
    bool edit_;
    bool enterOccur_;

    wxDatePickerCtrl* dpc_;         // dpcTransDate_
    wxDatePickerCtrl* dpcbd_;       // dpcNextOccDate_
    wxCalendarCtrl* calendarCtrl_;
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

    wxString payeeDepositTip_;
    wxString payeeWithdrawalTip_;
    wxString amountNormalTip_;
    wxString amountTransferTip_;
    wxSpinButton *spinNextOccDate_;
    wxSpinButton *spinTransDate_;

    void displayControlsForType( int transType, bool enableAdvanced = false);
    void resetPayeeString(bool normal = true);
    void OnAdvanceChecked(wxCommandEvent& event);
    void SetTransferControls(bool transfers = false);
    void SetAdvancedTransferControls(bool advanced = false);
    void OnTransDateForward(wxSpinEvent& event);
    void OnTransDateBack(wxSpinEvent& event);
    void OnNextOccurDateForward(wxSpinEvent& event);
    void OnNextOccurDateBack(wxSpinEvent& event);
    void SetNewDate(wxDatePickerCtrl* dpc, bool forward = true);

    wxStaticText* staticTimesRepeat_;
    wxStaticText* staticTextRepeats_;
    wxButton* bSetNextOccurDate_;
    void OnRepeatTypeChanged(wxCommandEvent& event);
    void OnsetNextRepeatDate(wxCommandEvent& event);
    void setRepeatDetails();

    void activateSplitTransactionsDlg();
};

#endif

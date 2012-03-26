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
#include<wx/spinbutt.h>

class wxDatePickerCtrl;

class mmTransDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmTransDialog )
    DECLARE_EVENT_TABLE()

public:
    mmTransDialog() {}

    mmTransDialog(boost::shared_ptr<wxSQLite3Database> db, 
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
    void OnAutoTransNum(wxCommandEvent& event);
    void OnFrequentUsedNotes(wxCommandEvent& event);  
    void OnTransTypeChanged(wxCommandEvent& event);
    void OnSpinUp(wxSpinEvent&);
    void OnSpinDown(wxSpinEvent&);
    void OnDateChanged(wxDateEvent& event);
    void dataToControls();
    void updateControlsForTransType();
    void displayControlsToolTips(int transType, bool enableAdvanced = false);
    bool getPayeeID(wxString payee, int& payeeID, int& categID, int& subcategID );
    void addPayee(wxString payee, int categID, int subcategID );
    void OnSplitChecked(wxCommandEvent& event);
    void onNoteSelected(wxCommandEvent& event);
    void changeFocus(wxChildFocusEvent& event);
    void onTextEntered(wxCommandEvent& event);
    void onChoiceTransChar(wxKeyEvent& event);
    void onChoiceStatusChar(wxKeyEvent& event);
    void OnButtonPayeeChar(wxKeyEvent& event);
    void OnButtonToAccountChar(wxKeyEvent& event);
    void OnButtonPayeeMouse(wxMouseEvent& event); 
    void OnButtonToAccountMouse(wxMouseEvent& event); 
    void OnButtonDateChar(wxKeyEvent& event);

    void SetDialogToDuplicateTransaction();

private:
    boost::shared_ptr<wxSQLite3Database> db_;
    wxSQLite3Database* inidb_;
    mmCoreDB* core_;
    int transID_;
    int accountID_;
    int referenceAccountID_;    // used for transfer transactions 
    wxTextCtrl *textNumber_;
    wxTextCtrl *textAmount_;
    wxTextCtrl *toTextAmount_;
    wxTextCtrl *textNotes_;
    wxTextCtrl* textCategory_;
    wxButton* bCategory_;
    wxButton* bPayee_;
    wxButton* bTo_;
    wxCheckBox* cSplit_;
    wxCheckBox* cAdvanced_;
    wxButton* bAuto_;
    wxButton* bFrequentUsedNotes_;
    bool payeeUnknown_;
    bool categUpdated_;

    wxString categoryName_;
    wxString subCategoryName_;

    wxChoice* choiceStatus_;
    wxChoice* choiceTrans_;
    bool edit_;

    wxDatePickerCtrl* dpc_;
    wxSpinButton *spinCtrl_;
    wxStaticText* itemStaticTextWeek;
    int categID_;
    int subcategID_;
    int payeeID_;
    int toID_;
    double toTransAmount_;
    bool advancedToTransAmountSet_;
    boost::shared_ptr<mmSplitTransactionEntries> split_;

    // store the original currency rate for transaction editing
    double  edit_currency_rate;

    wxString payeeWithdrawalTip_;
    wxString payeeDepositTip_;
    wxString amountNormalTip_;
    wxString amountTransferTip_;

    wxArrayString freqnotes_;
    bool richText_;
    int object_id;

    wxString getMostFrequentlyUsedPayee(wxString& categString);
    wxString resetPayeeString(bool normal = true);
    wxString getMostFrequentlyUsedCategory();
    wxString getLastTrxDate();
    wxString resetCategoryString();
    void OnAdvanceChecked(wxCommandEvent& event);
    void SetTransferControls(bool transfers = false);
    void SetAdvancedTransferControls(bool advanced = false);

    void activateSplitTransactionsDlg();
};

#endif

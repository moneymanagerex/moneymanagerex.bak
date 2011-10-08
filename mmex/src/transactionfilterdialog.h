/*************************************************************************
 Copyright (C) 2011 Stefano Giorgio

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
 ************************************************************************/

#ifndef _MM_EX_TRANSACTIONFILTERDIALOG_H_
#define _MM_EX_TRANSACTIONFILTERDIALOG_H_

#include "guiid.h"
#include "defs.h"
#include "mmcoredb.h"

#define SYMBOL_TRANSACTIONFILTERDIALOG_TITLE _("MMEX Account View: Transaction Filter")
#define SYMBOL_TRANSACTIONFILTERDIALOG_POSITION wxDefaultPosition
#define SYMBOL_TRANSACTIONFILTERDIALOG_SIZE wxSize(355, 300)
#define SYMBOL_TRANSACTIONFILTERDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX

class TransFilterDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS( TransFilterDialog )
    DECLARE_EVENT_TABLE()

public:
    // Constructors
    TransFilterDialog( );
    TransFilterDialog( mmCoreDB* core, wxWindow* parent,
              wxWindowID id     = wxID_ANY,
        const wxString& caption = SYMBOL_TRANSACTIONFILTERDIALOG_TITLE,
        const wxPoint& pos      = SYMBOL_TRANSACTIONFILTERDIALOG_POSITION,
        const wxSize& size      = SYMBOL_TRANSACTIONFILTERDIALOG_SIZE,
                long style      = SYMBOL_TRANSACTIONFILTERDIALOG_STYLE );

    // Destructor - Currently used for testing only
    ~TransFilterDialog( );

    bool byDateRange(wxDateTime transDate);
    bool byPayee(wxString payee);
    bool byCategory( wxString category, wxString subCategory );
    bool byStatus( wxString status );
    bool byType(wxString type);
    bool byTransNumber(wxString trNum);
    bool byNotes(wxString notes); 

private:
    // Define our variables
    mmCoreDB* core_;
    wxString  catName_;
    wxString  subCatName_;

    wxCheckBox* cbDateRange_;
    wxCheckBox* cbPayee_;
    wxCheckBox* cbCategory_;
    wxCheckBox* cbStatus_;
    wxCheckBox* cbType_;
    wxCheckBox* cbTransNumber_;
    wxCheckBox* cbNotes_;

    wxDatePickerCtrl* dpDateStart_;
    wxDatePickerCtrl* dpDateEnd_;
    wxButton*   btnPayee_;
    wxButton*   btnCategory_;
    wxChoice*   choiceStatus_;
    wxChoice*   choiceType_;
    wxTextCtrl* txtTransNumber_;
    wxTextCtrl* txtNotes_;

    // Initialize our variables
    void Init();
    
    // Creation
    bool Create( wxWindow* parent, 
        wxWindowID id           = wxID_ANY,
        const wxString& caption = SYMBOL_TRANSACTIONFILTERDIALOG_TITLE,
        const wxPoint& pos      = SYMBOL_TRANSACTIONFILTERDIALOG_POSITION,
        const wxSize& size      = SYMBOL_TRANSACTIONFILTERDIALOG_SIZE,
        long  style             = SYMBOL_TRANSACTIONFILTERDIALOG_STYLE );

    // Creates the controls and sizers
    void CreateControls();

    void OnCBDateRange( wxCommandEvent& event );
    void OnCBPayee( wxCommandEvent& event );
    void OnCBCategory( wxCommandEvent& event );
    void OnCBStatus( wxCommandEvent& event );
    void OnCBType( wxCommandEvent& event );
    void OnCBTransNum( wxCommandEvent& event );
    void OnCBNotes( wxCommandEvent& event );

    void OnPayeeSelect(wxCommandEvent& event);
    void OnCategorySelect(wxCommandEvent& event);

    void OnButtonOK( wxCommandEvent& event );
    void OnButtonCancel( wxCommandEvent& event );
    bool searchResult( wxCheckBox* chkBox, wxTextCtrl* txtCtrl, wxString sourceStr);
};


#endif

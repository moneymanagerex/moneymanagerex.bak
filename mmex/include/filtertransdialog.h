/////////////////////////////////////////////////////////////////////////////
// Name:        filtertransdialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     08/30/06 07:23:20
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (Personal Edition), 08/30/06 07:23:20

#ifndef _FILTERTRANSDIALOG_H_
#define _FILTERTRANSDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filtertransdialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/datectrl.h"
#include "guiid.h"
#include "mmcheckingpanel.h"
#include "mmcoredb.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_MYDIALOG9 10095
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_TITLE _("Filter Transactions")
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_IDNAME ID_MYDIALOG9
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_POSITION wxDefaultPosition
#define ID_PANEL11 10096
#define ID_CHECKBOXACCOUNT 10097
#define ID_CHOICE4 10098
#define ID_CHECKBOXDATERANGE 10101
#define ID_CHOICE5 10102
#define ID_CHOICE6 10103
#define ID_CHECKBOXPAYEE 10104
#define ID_BUTTONPAYEE 10105
#define ID_CHECKBOXCATEGORY 10106
#define ID_BUTTONCATEGORY 10107
#define ID_CHECKBOXSTATUS 10108
#define ID_CHOICE7 10109
#define ID_CHECKBOXTYPE 10110
#define ID_CHOICE8 10111
#define ID_CHECKBOXAMOUNTRANGE 10112
#define ID_TEXTCTRL13 10113
#define ID_TEXTCTRL14 10114
#define ID_CHECKBOXNOTES 10115
#define ID_TEXTCTRL15 10116
#define ID_PANEL12 10000
#define ID_BUTTONOK 10099
#define ID_BUTTONCANCEL 10100
#define ID_CHECKBOXTRANSNUM 10301
#define ID_TEXTTRANSNUM 10302
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

/*!
 * mmFilterTransactionsDialog class declaration
 */

class mmFilterTransactionsDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmFilterTransactionsDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    mmFilterTransactionsDialog( );
    mmFilterTransactionsDialog(  std::vector< boost::shared_ptr<mmBankTransaction> >* trans, 
        mmCoreDB* core,
        wxWindow* parent, 
        wxWindowID id = SYMBOL_MMFILTERTRANSACTIONSDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_MMFILTERTRANSACTIONSDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_MMFILTERTRANSACTIONSDIALOG_POSITION, 
        const wxSize& size = SYMBOL_MMFILTERTRANSACTIONSDIALOG_SIZE, 
        long style = SYMBOL_MMFILTERTRANSACTIONSDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, 
       wxWindowID id = SYMBOL_MMFILTERTRANSACTIONSDIALOG_IDNAME, 
       const wxString& caption = SYMBOL_MMFILTERTRANSACTIONSDIALOG_TITLE, 
       const wxPoint& pos = SYMBOL_MMFILTERTRANSACTIONSDIALOG_POSITION, 
       const wxSize& size = SYMBOL_MMFILTERTRANSACTIONSDIALOG_SIZE, 
       long style = SYMBOL_MMFILTERTRANSACTIONSDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

   ////@begin mmFilterTransactionsDialog event handler declarations

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXACCOUNT
    void OnCheckboxaccountClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXDATERANGE
    void OnCheckboxDateRangeClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXPAYEE
    void OnCheckboxpayeeClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXCATEGORY
    void OnCheckboxcategoryClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXSTATUS
    void OnCheckboxstatusClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXTYPE
    void OnCheckboxtypeClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXAMOUNTRANGE
    void OnCheckboxamountrangeClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXNOTES
    void OnCheckboxnotesClick( wxCommandEvent& event );

     /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXNOTES
    void OnCheckboxTransNumberClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONOK
    void OnButtonokClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONCANCEL
    void OnButtoncancelClick( wxCommandEvent& event );

    void OnCategs(wxCommandEvent& event);
    void OnPayee(wxCommandEvent& event);


    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin mmFilterTransactionsDialog member variables
    wxCheckBox* accountCheckBox;
    wxChoice* accountDropDown;
    wxCheckBox* dateRangeCheckBox;
    wxDatePickerCtrl* fromDateCtrl;
    wxDatePickerCtrl* toDateControl;
    wxCheckBox* payeeCheckBox;
    wxButton* btnPayee;
    wxCheckBox* categoryCheckBox;
    wxButton* btnCategory;
    wxCheckBox* statusCheckBox;
    wxChoice* choiceStatus;
    wxCheckBox* typeCheckBox;
    wxChoice* choiceType;
    wxCheckBox* amountRangeCheckBox;
    wxTextCtrl* amountMinEdit;
    wxTextCtrl* amountMaxEdit;
    wxCheckBox* notesCheckBox;
    wxTextCtrl* notesEdit;
    wxCheckBox* transNumberCheckBox;
    wxTextCtrl* transNumberEdit;
    std::vector< boost::shared_ptr<mmBankTransaction> >* trans_;
    wxSQLite3Database* db_;
    mmCoreDB* core_;

    int categID_;
    int subcategID_;
    int payeeID_;
////@end mmFilterTransactionsDialog member variables
};

#endif
    // _FILTERTRANSDIALOG_H_

// Generated by DialogBlocks (Personal Edition), 08/30/06 07:23:20

#ifndef _FILTERTRANSDIALOG_H_
#define _FILTERTRANSDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filtertransdialog.cpp"
#endif

#include "mmcoredb.h"

#define ID_MYDIALOG9 10095
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_TITLE _("Transaction Filter")
#define SYMBOL_MMFILTERTRANSACTIONSDIALOG_IDNAME ID_MYDIALOG9

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

class mmFilterTransactionsDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( mmFilterTransactionsDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    mmFilterTransactionsDialog( );
    mmFilterTransactionsDialog
    (
        mmCoreDB* core,
        wxWindow* parent,
        wxWindowID id = SYMBOL_MMFILTERTRANSACTIONSDIALOG_IDNAME,
        const wxString& caption = SYMBOL_MMFILTERTRANSACTIONSDIALOG_TITLE,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(400, 300),
        long style = SYMBOL_MMFILTERTRANSACTIONSDIALOG_STYLE
    );

    bool somethingSelected();
    bool getAccountCheckBox() {return accountCheckBox_->GetValue();}
    int getAccountID() {return refAccountID_;}
    wxString getAccountName();

    bool getDateRangeCheckBox() {return dateRangeCheckBox_->GetValue();}
    wxDateTime getFromDateCtrl() {return fromDateCtrl_->GetValue();}
    wxDateTime getToDateControl() {return toDateControl_->GetValue();}

    bool getAmountRangeCheckBox() {return amountRangeCheckBox_->GetValue();}
    double getAmountMax();
    double getAmountMin();

    bool getPayeeCheckBox() {return payeeCheckBox_->GetValue();}
    int getPayeeID() const;

    bool getStatusCheckBox() {return statusCheckBox_->GetValue();}
    wxString getStatus() const;

    bool getTypeCheckBox() {return typeCheckBox_->GetValue();}
    wxString getType() const;

    bool getNumberCheckBox() {return transNumberCheckBox_->GetValue();}
    wxString getNumber() {return transNumberEdit_->GetValue();}

    bool getNotesCheckBox() {return notesCheckBox_->GetValue();}
    wxString getNotes() {return notesEdit_->GetValue();}

    bool getCategoryCheckBox() {return categoryCheckBox_->GetValue();}
    int getCategoryID() {return categID_;}
    int getSubCategoryID() {return subcategID_;}
    bool getExpandStatus() {return bExpandStaus_;}

    /// Returns the payee string, when Payee is selected.
    wxString userPayeeStr() const;
    /// Returns the Category/subcategory string, when Category is selected.
    wxString userCategoryStr() const;
    /// Returns the date range string, when Date Range is selected.
    wxString userDateRangeStr() const;
    /// Returns the Status string, when Status is selected.
    wxString userStatusStr() const;
    /// Returns the amount range string, when Amount Range is selected.
    wxString userAmountRangeStr() const;
    wxString userTypeStr() const;

    /// Returns true with valid dates, when the dialog date range is selected.
    bool getDateRange(wxDateTime& startDate, wxDateTime& endDate) const;

    void setAccountToolTip(wxString tip) const;

private:
    /// Creation
    bool Create( wxWindow* parent,
       wxWindowID id = SYMBOL_MMFILTERTRANSACTIONSDIALOG_IDNAME,
       const wxString& caption = SYMBOL_MMFILTERTRANSACTIONSDIALOG_TITLE,
       const wxPoint& pos = wxDefaultPosition,
       const wxSize& size = wxSize(400, 300),
       long style = SYMBOL_MMFILTERTRANSACTIONSDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    void dataToControls();
    wxString GetStoredSettings(int id);
    wxString GetCurrentSettings();

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXACCOUNT
    void OnCheckboxClick( wxCommandEvent& event );

    void OnButtonokClick( wxCommandEvent& event );
    void OnButtoncancelClick( wxCommandEvent& event );
    void OnButtonSaveClick( wxCommandEvent& event );
    void OnSettingsSelected( wxCommandEvent& event );
    void OnPayeeUpdated(wxCommandEvent& event);
    bool get_next_value(wxStringTokenizer& tkz, wxString& value);

    void OnCategs(wxCommandEvent& event);

    /// Should we show tooltips?
    static bool ShowToolTips();

    wxString settings_string_;
    wxString prev_value_;
    wxCheckBox* accountCheckBox_;
    wxChoice* accountDropDown_;
    wxCheckBox* dateRangeCheckBox_;
    wxDatePickerCtrl* fromDateCtrl_;
    wxDatePickerCtrl* toDateControl_;
    wxCheckBox* payeeCheckBox_;
    wxComboBox* cbPayee_;
    wxCheckBox* categoryCheckBox_;
    wxButton* btnCategory_;
    wxCheckBox* statusCheckBox_;
    wxChoice* choiceStatus_;
    wxCheckBox* typeCheckBox_;
    wxCheckBox* cbTypeWithdrawal_;
    wxCheckBox* cbTypeDeposit_;
    wxCheckBox* cbTypeTransfer_;
    wxCheckBox* amountRangeCheckBox_;
    wxTextCtrl* amountMinEdit_;
    wxTextCtrl* amountMaxEdit_;
    wxCheckBox* notesCheckBox_;
    wxTextCtrl* notesEdit_;
    wxRadioBox* m_radio_box_;
    wxCheckBox* transNumberCheckBox_;
    wxTextCtrl* transNumberEdit_;
    std::vector< wxSharedPtr<mmBankTransaction> >* trans_;
    mmCoreDB* core_;

    int categID_;
    int subcategID_;
    bool bExpandStaus_;
	int payeeID_;
    int refAccountID_;
    wxString refAccountStr_;
};

#endif
    // _FILTERTRANSDIALOG_H_

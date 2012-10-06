/*******************************************************
  Copyright (C) 2012 Nikolay
 ********************************************************/


#define SYMBOL_QIFDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_QIFDIALOG_TITLE _("QIF Export")
#define SYMBOL_QIFDIALOG_IDNAME wxID_HIGHEST+1
#define SYMBOL_QIFDIALOG_SIZE wxSize(500, 300)
#define SYMBOL_QIFDIALOG_POSITION wxDefaultPosition

#include "guiid.h"
#include "defs.h"
#include "mmcoredb.h"

class wxDatePickerCtrl;

class mmQIFDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS( mQIFDialog )
    DECLARE_EVENT_TABLE()

public:
    mmQIFDialog() {}

    mmQIFDialog(
        mmCoreDB* core,
        wxWindow* parent,
        wxWindowID id = SYMBOL_QIFDIALOG_IDNAME,
        const wxString& caption = SYMBOL_QIFDIALOG_TITLE,
        const wxPoint& pos = SYMBOL_QIFDIALOG_POSITION,
        const wxSize& size = SYMBOL_QIFDIALOG_SIZE,
        long style = SYMBOL_QIFDIALOG_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_QIFDIALOG_IDNAME,
        const wxString& caption = SYMBOL_QIFDIALOG_TITLE,
        const wxPoint& pos = SYMBOL_QIFDIALOG_POSITION,
        const wxSize& size = SYMBOL_QIFDIALOG_SIZE,
        long style = SYMBOL_QIFDIALOG_STYLE );

    void CreateControls();

private:
    void OnQuit(wxCloseEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnOk(wxCommandEvent& /*event*/);
    void mmExportQIF();
    void OnAccountsButton(wxCommandEvent& /*event*/);
    void OnCheckboxClick(wxCommandEvent& /*event*/);
    void OnFileNameEntered(wxCommandEvent& event);
    void OnFileNameChanged(wxCommandEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& /*event*/);
    mmCoreDB* core_;
    wxWindow* parent_;
    wxArrayInt accounts_id_;
    wxArrayInt items_index_;

    wxCheckBox* cCategs_;
    wxCheckBox* accountsCheckBox_;
    wxButton* bSelectedAccounts_;
    wxCheckBox* dateFromCheckBox_;
    wxCheckBox* dateToCheckBox_;
    wxDatePickerCtrl* fromDateCtrl_;
    wxDatePickerCtrl* toDateCtrl_;
    wxCheckBox* toFileCheckBox_;
    wxStaticText* file_name_label_;
    wxButton* button_search_;
    wxTextCtrl* m_text_ctrl_;
    wxTextCtrl* log_field_;
    wxRadioBox* m_radio_box_;
    wxString delimit_;
};

void mmExportQIF(wxWindow* parent_, mmCoreDB* core);


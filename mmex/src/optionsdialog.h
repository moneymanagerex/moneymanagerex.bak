/*************************************************************************
 Copyright (C) 2006 Madhan Kanagavel
 copyright (C) 2011, 2012 Nikolay & Stefano Giorgio.

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
 *************************************************************************/

#ifndef _MM_EX_OPTIONSDIALOG_H_
#define _MM_EX_OPTIONSDIALOG_H_

#include "guiid.h"
#include "defs.h"

#define SYMBOL_MMOPTIONSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MMOPTIONSDIALOG_TITLE _("New MMEX Options")
#define SYMBOL_MMOPTIONSDIALOG_IDNAME ID_DIALOG_OPTIONS
#define SYMBOL_MMOPTIONSDIALOG_SIZE wxSize(500, 400)
#define SYMBOL_MMOPTIONSDIALOG_POSITION wxDefaultPosition

enum
{
    //buttons
    ID_DIALOG_OPTIONS_BUTTON_LANGUAGE = wxID_HIGHEST+1,
    ID_DIALOG_OPTIONS_BUTTON_CURRENCY,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES,
    //checkboxes
    ID_DIALOG_OPTIONS_CHK_BACKUP,
    ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE,
    ID_DIALOG_OPTIONS_EXPAND_BANK_TREE,
    ID_DIALOG_OPTIONS_EXPAND_TERM_TREE,
    ID_DIALOG_OPTIONS_EXPAND_BANK_HOME,
    ID_DIALOG_OPTIONS_EXPAND_TERM_HOME,
    ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME,
    ID_DIALOG_OPTIONS_VIEW_BUDGET_AS,
    ID_DIALOG_OPTIONS_VIEW_BUDGET_WITH_TRANSFER,
    ID_DIALOG_OPTIONS_VIEW_BUDGET_WITHOUT_SUM,
    ID_DIALOG_OPTIONS_VIEW_BUDGET_WITHOUT_CATEG,
    ID_DIALOG_OPTIONS_VIEW_BUDGET_WITHOUT_FUTURE,
    //choices
    ID_DIALOG_OPTIONS_DATE_FORMAT,
    ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_MONTH,
    ID_DIALOG_OPTIONS_VIEW_ACCOUNTS,
    ID_DIALOG_OPTIONS_VIEW_TRANS,
    ID_DIALOG_OPTIONS_FONT_SIZE,
    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE,
    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE,
    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY,
    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS,
    //textctrl
    ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER,
    ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL,
    ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME,
    //spin
    ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY,
    //others
    ID_DIALOG_OPTIONS_RESTART_REQUIRED,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT,
    ID_BOOK_PANELCOLORS,
    ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE,
    ID_BOOK_PANELMISC,
    ID_BOOK_PANEL_EXP_IMP,
    ID_BOOK_PANELGENERAL,
    ID_BOOK_PANELVIEWS,
    ID_DIALOG_OPTIONS,
    ID_DIALOG_OPTIONS_LISTBOOK,
    ID_DIALOG_OPTIONS_PANEL1,
    ID_DIALOG_OPTIONS_PANEL2,
    ID_RADIO_BOX,
};

class mmCoreDB;
class wxSQLite3Database;

class mmOptionsDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( mmOptionsDialog )
    DECLARE_EVENT_TABLE()

public:
    mmOptionsDialog( );
    mmOptionsDialog( mmCoreDB* db,
        wxWindow* parent, wxWindowID id = SYMBOL_MMOPTIONSDIALOG_IDNAME,
        const wxString& caption = SYMBOL_MMOPTIONSDIALOG_TITLE,
        const wxPoint& pos = SYMBOL_MMOPTIONSDIALOG_POSITION,
        const wxSize& size = SYMBOL_MMOPTIONSDIALOG_SIZE,
        long style = SYMBOL_MMOPTIONSDIALOG_STYLE );
    ~mmOptionsDialog( );

    /// Saves the updated System Options to the appropriate databases.
    void SaveNewSystemSettings();

    /// Returns the value of the dialog setting;
    bool GetUpdateCurrencyRateSetting();

    bool RequiresRestart() { return restartRequired_; }
    bool AppliedChanges() { return changesApplied_; }

private:
    /// System database access variables
    mmCoreDB* core_;
    wxSQLite3Database* db_;

    /// Dialog specific controls
    wxImageList* m_imageList;
    wxChoice* choiceDateFormat_;
    wxChoice* choiceVisible_;
    wxChoice* choiceTransVisible_;
    wxChoice* choiceFontSize_;
    wxChoice* monthSelection_;

    /// set colour variables.
    wxColour navTreeBkColor_;
    wxColour listAlternativeColor0_;
    wxColour listAlternativeColor1_;
    wxColour listBackColor_;
    wxColour listBorderColor_;
    wxColour listDetailsPanelColor_;
    wxColour listFutureDateColor_;

    wxCheckBox* cbUseOrgDateCopyPaste_;
    wxCheckBox* cbUseSound_;
    wxCheckBox* cbEnableCurrencyUpd_;

    wxCheckBox* cbBudgetFinancialYears_;
    wxCheckBox* cbBudgetIncludeTransfers_;
    wxCheckBox* cbBudgetSetupWithoutSummary_;
    wxCheckBox* cbBudgetSummaryWithoutCateg_;
    wxCheckBox* cbIgnoreFutureTransactions_;

    wxRadioBox* m_radio_box_;
    wxTextCtrl* textDelimiter_;

    bool restartRequired_;
    bool changesApplied_;
    int currencyId_;
    wxString dateFormat_;
    wxString currentLanguage_;
    wxString delimit_;

    wxArrayString viewAccountStrings(bool translated, wxString get_string_id);
    wxArrayString viewAccountStrings(bool translated, wxString input_string, int& row_id);

    /// Dialog Creation - Used by constructor
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MMOPTIONSDIALOG_IDNAME,
        const wxString& caption = SYMBOL_MMOPTIONSDIALOG_TITLE,
        const wxPoint& pos = SYMBOL_MMOPTIONSDIALOG_POSITION,
        const wxSize& size = SYMBOL_MMOPTIONSDIALOG_SIZE,
        long style = SYMBOL_MMOPTIONSDIALOG_STYLE );

    void CreateControls();

    void OnCurrency(wxCommandEvent& event);
    void OnDateFormatChanged(wxCommandEvent& event);
    void OnLanguageChanged(wxCommandEvent& event);

    /// Colour Changing events
    void OnColorChanged(wxCommandEvent& event);
    void OnRestoreDefaultColors(wxCommandEvent& event);

    bool GetIniDatabaseCheckboxValue(wxString dbField, bool defaultState);

    void OnDelimiterSelected(wxCommandEvent& event);

    void SaveViewAccountOptions();
    void SaveViewTransactionOptions();
    void SaveFinancialYearStart();

    void SaveGeneralPanelSettings();
    void SaveViewPanelSettings();
    void SaveColourPanelSettings();
    void SaveOthersPanelSettings();
    void SaveImportExportPanelSettings();
    void OnOk(wxCommandEvent& /*event*/);
};

#endif

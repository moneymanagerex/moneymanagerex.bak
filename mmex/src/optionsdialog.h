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
#include "mmcoredb.h"

#define SYMBOL_MMOPTIONSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MMOPTIONSDIALOG_TITLE _("New MMEX Options")
#define SYMBOL_MMOPTIONSDIALOG_IDNAME ID_DIALOG_OPTIONS
#define SYMBOL_MMOPTIONSDIALOG_SIZE wxSize(500, 400)
#define SYMBOL_MMOPTIONSDIALOG_POSITION wxDefaultPosition

class mmOptionsDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmOptionsDialog )
    DECLARE_EVENT_TABLE()

public:
    mmOptionsDialog( );
    mmOptionsDialog( mmCoreDB* db, wxSQLite3Database* inidb,
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

private:
    /// System database access variables
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    wxSQLite3Database* inidb_;

    /// Dialog specific controls
    wxImageList* m_imageList;
    wxComboBox* choiceDateFormat_;
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

    bool restartRequired_;
    int currencyId_;
    wxString dateFormat_;
    wxString currentLanguage_;
    wxArrayString itemChoiceStrings();
    wxArrayString DateFormats();

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
    void OnNavTreeColorChanged(wxCommandEvent& event);
    void OnAlt0Changed(wxCommandEvent& event);
    void OnAlt1Changed(wxCommandEvent& event);
    void OnListBackgroundChanged(wxCommandEvent& event);
    void OnListBorderChanged(wxCommandEvent& event);
    void OnListDetailsColors(wxCommandEvent& event);
    void OnListFutureDates(wxCommandEvent& event);
    void OnRestoreDefaultColors(wxCommandEvent& event);
   
	void SetIniDatabaseCheckboxValue(wxString dbField, bool dbState);
    bool GetIniDatabaseCheckboxValue(wxString dbField, bool defaultState);

    void OnDelimiterSelectedU(wxCommandEvent& event);
    void OnDelimiterSelectedC(wxCommandEvent& event);
    void OnDelimiterSelectedS(wxCommandEvent& event);
    void OnDelimiterSelectedT(wxCommandEvent& event);

    wxString DisplayDate2FormatDate(wxString strDate);
    wxString FormatDate2DisplayDate(wxString strDate);

    void SaveViewAccountOptions();
    void SaveViewTransactionOptions();
    void SaveFinancialYearStart();
    void SaveStocksUrl();
	
    void SaveGeneralPanelSettings();
    void SaveViewPanelSettings();
    void SaveColourPanelSettings();
    void SaveOthersPanelSettings();
    void SaveImportExportPanelSettings();
};

#endif

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

#include "optionsdialog.h"
#include "defs.h"
#include "maincurrencydialog.h"
#include "util.h"
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include "mmgraphgenerator.h"
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <limits>

enum 
{   
    ID_BOOK_PANEL_EXP_IMP = wxID_HIGHEST + 1,
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_USER4,
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_COMMA4,
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_SEMICOLON4, 
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_TAB4,
    ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4,
};

IMPLEMENT_DYNAMIC_CLASS( mmOptionsDialog, wxDialog )

BEGIN_EVENT_TABLE( mmOptionsDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, mmOptionsDialog::OnCurrency)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT, mmOptionsDialog::OnDateFormatChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE, mmOptionsDialog::OnLanguageChanged)
    
    /// Colour Changing events
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE, mmOptionsDialog::OnNavTreeColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0, mmOptionsDialog::OnAlt0Changed)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1, mmOptionsDialog::OnAlt1Changed)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK, mmOptionsDialog::OnListBackgroundChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER, mmOptionsDialog::OnListBorderChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS, mmOptionsDialog::OnListDetailsColors)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES, mmOptionsDialog::OnListFutureDates)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT, mmOptionsDialog::OnRestoreDefaultColors)

    EVT_RADIOBUTTON(ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_COMMA4, mmOptionsDialog::OnDelimiterSelectedC)
    EVT_RADIOBUTTON(ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_SEMICOLON4, mmOptionsDialog::OnDelimiterSelectedS)
    EVT_RADIOBUTTON(ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_TAB4, mmOptionsDialog::OnDelimiterSelectedT)
    EVT_RADIOBUTTON(ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_USER4, mmOptionsDialog::OnDelimiterSelectedU)
END_EVENT_TABLE()

#include "../resources/main-setup.xpm"
#include "../resources/preferences-color.xpm"
#include "../resources/view.xpm"
#include "../resources/preferences-other.xpm"
#include "../resources/export-import.xpm"

mmOptionsDialog::mmOptionsDialog( )
{
}

mmOptionsDialog::~mmOptionsDialog( )
{
    delete m_imageList;
}

mmOptionsDialog::mmOptionsDialog( mmCoreDB* core, wxSQLite3Database* inidb,
                                 wxWindow* parent, wxWindowID id, 
                                 const wxString& caption, 
                                 const wxPoint& pos, const wxSize& size, long style )
                                 : core_(core), inidb_(inidb), db_(core->db_.get()), restartRequired_(false)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmOptionsDialog::Create( wxWindow* parent, wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );
    
    /// initialize the colour variables
    navTreeBkColor_        = mmColors::navTreeBkColor;
    listAlternativeColor0_ = mmColors::listAlternativeColor0;
    listAlternativeColor1_ = mmColors::listAlternativeColor1;
    listBackColor_         = mmColors::listBackColor;
    listBorderColor_       = mmColors::listBorderColor;
    listDetailsPanelColor_ = mmColors::listDetailsPanelColor;
    listFutureDateColor_   = mmColors::listFutureDateColor;

    currencyId_ = mmDBWrapper::getBaseCurrencySettings(db_);
    dateFormat_ = mmDBWrapper::getInfoSettingValue(db_, wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);

    CreateControls();
    Centre();
    Fit();

    return TRUE;
}

wxString mmOptionsDialog::DisplayDate2FormatDate(wxString strDate)
{
    wxArrayString DateFormat = DateFormats();
    wxArrayString itemChoice7Strings = itemChoiceStrings();

    for(size_t i=0; i<DateFormat.Count(); i++)
    {
        if(strDate == itemChoice7Strings[i])
            return DateFormat[i];
    }

    return DateFormat[0];
}

wxString mmOptionsDialog::FormatDate2DisplayDate(wxString strDate)
{
    wxArrayString DateFormat = DateFormats();
    wxArrayString itemChoice7Strings = itemChoiceStrings();

    for(size_t i=0; i<DateFormat.Count(); i++)
    {
        if(strDate == DateFormat[i])
            return itemChoice7Strings[i];
    }

    return itemChoice7Strings[0];
}

wxArrayString mmOptionsDialog::DateFormats() {

    wxArrayString DateFormat;
    
    DateFormat.Add(wxT("%d/%m/%y"));
    DateFormat.Add(wxT("%d/%m/%Y"));
    DateFormat.Add(wxT("%d-%m-%y"));
    DateFormat.Add(wxT("%d-%m-%Y"));
    DateFormat.Add(wxT("%d.%m.%y"));
    DateFormat.Add(wxT("%d.%m.%Y"));
    DateFormat.Add(wxT("%d,%m,%y"));
    DateFormat.Add(wxT("%d/%m'%Y"));
    DateFormat.Add(wxT("%d/%m %Y"));
    DateFormat.Add(wxT("%m/%d/%y"));
    DateFormat.Add(wxT("%m/%d/%Y"));
    DateFormat.Add(wxT("%m-%d-%y"));
    DateFormat.Add(wxT("%m-%d-%Y"));
    DateFormat.Add(wxT("%m/%d'%Y"));
    DateFormat.Add(wxT("%y/%m/%d"));
    DateFormat.Add(wxT("%y-%m-%d"));
    DateFormat.Add(wxT("%Y/%m/%d"));
    DateFormat.Add(wxT("%Y-%m-%d"));
    DateFormat.Add(wxT("%Y.%m.%d"));
    DateFormat.Add(wxT("%Y%m%d"));

    return DateFormat;
}

wxArrayString mmOptionsDialog::itemChoiceStrings() {

    wxArrayString itemChoice7Strings;
    
    itemChoice7Strings.Add(wxT("DD/MM/YY"));
    itemChoice7Strings.Add(wxT("DD/MM/YYYY"));
    itemChoice7Strings.Add(wxT("DD-MM-YY"));
    itemChoice7Strings.Add(wxT("DD-MM-YYYY"));
    itemChoice7Strings.Add(wxT("DD.MM.YY"));
    itemChoice7Strings.Add(wxT("DD.MM.YYYY"));
    itemChoice7Strings.Add(wxT("DD,MM,YY"));
    itemChoice7Strings.Add(wxT("DD/MM'YYYY"));
    itemChoice7Strings.Add(wxT("DD/MM YYYY"));
    itemChoice7Strings.Add(wxT("MM/DD/YY"));
    itemChoice7Strings.Add(wxT("MM/DD/YYYY"));
    itemChoice7Strings.Add(wxT("MM-DD-YY"));
    itemChoice7Strings.Add(wxT("MM-DD-YYYY"));
    itemChoice7Strings.Add(wxT("MM/DD'YYYY"));
    itemChoice7Strings.Add(wxT("YY/MM/DD"));
    itemChoice7Strings.Add(wxT("YY-MM-DD"));
    itemChoice7Strings.Add(wxT("YYYY/MM/DD"));
    itemChoice7Strings.Add(wxT("YYYY-MM-DD"));
    itemChoice7Strings.Add(wxT("YYYY.MM.DD"));
    itemChoice7Strings.Add(wxT("YYYYMMDD"));
    
    return itemChoice7Strings;
}

wxArrayString mmOptionsDialog::viewAccountStrings(bool translated, wxString input_string, int& row_id_) {

    wxArrayString itemChoiceViewAccountStrings;
    
    if (translated) {
        itemChoiceViewAccountStrings.Add(_("All"));
        itemChoiceViewAccountStrings.Add(_("Open"));
        itemChoiceViewAccountStrings.Add(_("Favorites"));
    } else {
        itemChoiceViewAccountStrings.Add(VIEW_ACCOUNTS_ALL_STR);
        itemChoiceViewAccountStrings.Add(VIEW_ACCOUNTS_OPEN_STR);
        itemChoiceViewAccountStrings.Add(VIEW_ACCOUNTS_FAVORITES_STR);
    }
    if (!input_string.IsEmpty()) 
    {
	    for(size_t i = 0; i < itemChoiceViewAccountStrings.Count(); i++)
	    {
	        if(input_string == itemChoiceViewAccountStrings[i]) {
	            row_id_ = i;
	            break;
	        }
	    }
    }
    
    return itemChoiceViewAccountStrings;
}

void mmOptionsDialog::CreateControls()
{
    wxSize imageSize(48, 48);
    m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
    m_imageList->Add(wxBitmap(view_xpm));
    m_imageList->Add(wxBitmap(preferences_color_xpm));
    m_imageList->Add(wxBitmap(main_setup_xpm));
    m_imageList->Add(wxBitmap(preferences_other_xpm));
    m_imageList->Add(wxBitmap(export_import_xpm));
 
    wxBoxSizer* mainDialogSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainDialogSizer);

    wxPanel* mainDialogPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    mainDialogSizer->Add(mainDialogPanel, 1, wxGROW|wxALL, 5);

    wxBoxSizer* mainDialogPanelSizer = new wxBoxSizer(wxVERTICAL);
    mainDialogPanel->SetSizer(mainDialogPanelSizer);

    wxListbook* newBook = new wxListbook(mainDialogPanel, ID_DIALOG_OPTIONS_LISTBOOK, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);

    /*********************************************************************************************
     General Panel
    **********************************************************************************************/
    wxPanel* generalPanel = new wxPanel(newBook, ID_BOOK_PANELGENERAL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    SetMinSize(wxSize(500,600));
    wxBoxSizer* generalPanelSizer = new wxBoxSizer(wxVERTICAL);
    generalPanel->SetSizer(generalPanelSizer);

    // Display Header Settings
    wxStaticBox* headerStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Display Heading"));

    // Define the staticBox font and set it as wxFONTWEIGHT_BOLD
    wxFont staticBoxFontSetting = headerStaticBox->GetFont();
    staticBoxFontSetting.SetWeight(wxFONTWEIGHT_BOLD);

    headerStaticBox->SetFont(staticBoxFontSetting);

    wxStaticBoxSizer* headerStaticBoxSizer = new wxStaticBoxSizer(headerStaticBox, wxHORIZONTAL);
    generalPanelSizer->Add(headerStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);

    wxStaticText* userNameText = new wxStaticText(generalPanel, wxID_STATIC,
        _("User Name"), wxDefaultPosition, wxDefaultSize, 0);
    headerStaticBoxSizer->Add(userNameText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString userName = mmDBWrapper::getInfoSettingValue(db_, wxT("USERNAME"), wxT(""));
    wxTextCtrl* userNameTextCtr = new wxTextCtrl(generalPanel, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME,
        userName, wxDefaultPosition, wxDefaultSize, 0);
    userNameTextCtr->SetToolTip(_("The User Name is used as a title for the database."));
    headerStaticBoxSizer->Add(userNameTextCtr, 1, wxALIGN_LEFT|wxGROW|wxALL, 5);

    // Language Settings
    wxStaticBox* languageStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Language"));
    languageStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* languageStaticBoxSizer = new wxStaticBoxSizer(languageStaticBox, wxHORIZONTAL);
    generalPanelSizer->Add(languageStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    currentLanguage_ = mmDBWrapper::getINISettingValue(inidb_, LANGUAGE_PARAMETER);
    wxButton* languageButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_LANGUAGE,
        currentLanguage_.Left(1).Upper() + currentLanguage_.SubString(1,currentLanguage_.Len()),
        wxDefaultPosition, wxSize(150, -1), 0);
    languageButton->SetToolTip(_("Specify the language to use"));
    languageStaticBoxSizer->Add(languageButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Currency Settings
    wxStaticBox* currencyStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Currency"));
    currencyStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* currencyStaticBoxSizer = new wxStaticBoxSizer(currencyStaticBox, wxHORIZONTAL);
    currencyStaticBox->SetFont(staticBoxFontSetting);
    generalPanelSizer->Add(currencyStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxStaticText* baseCurrencyText = new wxStaticText(generalPanel, wxID_STATIC,
        _("Base Currency"), wxDefaultPosition, wxDefaultSize, 0);
    currencyStaticBoxSizer->Add(baseCurrencyText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString currName = _("Set Currency");
    if (currencyId_ != -1)
        currName = core_->getCurrencyName(currencyId_);
    wxButton* baseCurrencyButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_CURRENCY,
        currName, wxDefaultPosition, wxSize(150, -1), 0);
    baseCurrencyButton->SetToolTip(_("Sets the default currency for the database."));
    currencyStaticBoxSizer->Add(baseCurrencyButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Date Format Settings
    wxStaticBox* dateFormatStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Date Format"));
    dateFormatStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* dateFormatStaticBoxSizer = new wxStaticBoxSizer(dateFormatStaticBox, wxVERTICAL);
    wxFlexGridSizer* dateFormatSettingStaticBoxSizerGrid = new wxFlexGridSizer(2,2,0,5);
    generalPanelSizer->Add(dateFormatStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    dateFormatStaticBoxSizer->Add(dateFormatSettingStaticBoxSizerGrid);
    
    wxArrayString itemChoice7Strings = itemChoiceStrings(); 
    
    choiceDateFormat_ = new wxComboBox(generalPanel, ID_DIALOG_OPTIONS_DATE_FORMAT, wxT(""),
        wxDefaultPosition, wxSize(140, -1), itemChoice7Strings);
    dateFormatSettingStaticBoxSizerGrid->Add(choiceDateFormat_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceDateFormat_->SetToolTip(_("Specify the date format for display"));
    choiceDateFormat_->SetValue(FormatDate2DisplayDate(dateFormat_));

    wxButton* setFormatButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT,
        _("Set"), wxDefaultPosition, wxDefaultSize, 0);
    dateFormatSettingStaticBoxSizerGrid->Add(setFormatButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* sampleDateExampleText = new wxStaticText( generalPanel, wxID_ANY,
        _("New date format sample:"), wxDefaultPosition, wxDefaultSize, 0);
    wxStaticText* sampleDateText = new wxStaticText(generalPanel, ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE,
        wxT("redefined elsewhere"), wxDefaultPosition, wxDefaultSize, 0);
    dateFormatSettingStaticBoxSizerGrid->Add(sampleDateExampleText, 0, wxALIGN_LEFT|wxALL, 5);
    dateFormatSettingStaticBoxSizerGrid->Add(sampleDateText, 0, wxALIGN_LEFT|wxALL, 5);
    sampleDateText->SetLabel(wxDateTime::Now().Format(dateFormat_));

    // Financial Year Settings
    wxStaticBox* financialYearStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Financial Year"));
    financialYearStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* financialYearStaticBoxSizer = new wxStaticBoxSizer(financialYearStaticBox, wxVERTICAL);
    wxFlexGridSizer* financialYearStaticBoxSizerGrid = new wxFlexGridSizer(3,2,0,0);
    generalPanelSizer->Add(financialYearStaticBoxSizer, 0, wxGROW|wxALL, 5);
    financialYearStaticBoxSizer->Add(financialYearStaticBoxSizerGrid);
        
    wxStaticText* itemStaticTextFYSDay = new wxStaticText(generalPanel, wxID_STATIC, _("Start Day"),
        wxDefaultPosition, wxDefaultSize, 0 );
    financialYearStaticBoxSizerGrid->Add(itemStaticTextFYSDay, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString financialPeriodStartDay = mmDBWrapper::getInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_DAY"), wxT("1"));
    int day = wxAtoi(financialPeriodStartDay); 
    wxSpinCtrl *textFPSDay = new wxSpinCtrl(generalPanel, ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY,
        wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 31, day);
    textFPSDay->SetToolTip(_("Specify Day for start of financial year"));
    
    financialYearStaticBoxSizerGrid->Add(textFPSDay, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticTextSmonth = new wxStaticText(generalPanel, wxID_STATIC, _("Start Month"),
        wxDefaultPosition, wxDefaultSize, 0);
    financialYearStaticBoxSizerGrid->Add(itemStaticTextSmonth, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString financialMonthsSelection;
    for(int i=0; i<12; i++) {
        financialMonthsSelection.Add(mmGetNiceShortMonthName(i));
    };
    monthSelection_ = new wxChoice(generalPanel, ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_MONTH,
        wxDefaultPosition, wxSize(100, -1), financialMonthsSelection);
    financialYearStaticBoxSizerGrid->Add(monthSelection_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxString financialPeriodStartMonth = mmDBWrapper::getInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_MONTH"), wxT("7"));
 
    int monthItem = wxAtoi(financialPeriodStartMonth);
    monthSelection_->SetSelection(monthItem - 1);
    monthSelection_->SetToolTip(_("Specify month for start of financial year"));
 
    // Backup Settings
    wxStaticBox* backupStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Database Backup"));
    backupStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* backupStaticBoxSizer = new wxStaticBoxSizer(backupStaticBox, wxVERTICAL);
    generalPanelSizer->Add(backupStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxCheckBox* backupCheckBox = new wxCheckBox(generalPanel, ID_DIALOG_OPTIONS_CHK_BACKUP,
        _("Create a new backup when MMEX Start"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    backupCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("BACKUPDB"),false));
    backupCheckBox->SetToolTip(_("When MMEX Starts,\ncreates the backup database: dbFile_start_YYYY-MM-DD.ext."));
    backupStaticBoxSizer->Add(backupCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* backupUpdateCheckBox = new wxCheckBox(generalPanel, ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE,
        _("Update database changes to database backup on exit."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    backupUpdateCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("BACKUPDB_UPDATE"),false));
    backupUpdateCheckBox->SetToolTip(_("When MMEX shuts down and changes made to database,\ncreates or updates the backup database: dbFile_update_YYYY-MM-DD.ext."));
    backupStaticBoxSizer->Add(backupUpdateCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    /*********************************************************************************************
     Views Panel
    **********************************************************************************************/
    wxPanel* viewsPanel = new wxPanel(newBook, ID_BOOK_PANELVIEWS, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* viewsPanelSizer = new wxBoxSizer(wxVERTICAL);
    viewsPanel->SetSizer(viewsPanelSizer);

    // Account View Options
    wxStaticBox* accountStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Account View Options"));
    accountStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* accountStaticBoxSizer = new wxStaticBoxSizer(accountStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(accountStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxStaticText* accountStaticText = new wxStaticText( viewsPanel, wxID_STATIC,
        _("Accounts Visible"), wxDefaultPosition, wxDefaultSize, 0);
    accountStaticBoxSizer->Add(accountStaticText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    int row_id_ = 0;
    wxArrayString itemChoiceViewAccountTranslatedStrings = viewAccountStrings(true, wxEmptyString, row_id_);
    
    choiceVisible_ = new wxChoice(viewsPanel, ID_DIALOG_OPTIONS_VIEW_ACCOUNTS,
        wxDefaultPosition, wxDefaultSize, itemChoiceViewAccountTranslatedStrings);
    accountStaticBoxSizer->Add(choiceVisible_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), VIEW_ACCOUNTS_ALL_STR);
    row_id_ = 0;
    wxArrayString itemChoiceViewAccountStrings = viewAccountStrings(false, vAccts, row_id_);
    choiceVisible_->SetSelection(row_id_);
         
    choiceVisible_->SetToolTip(_("Specify which accounts are visible"));

    // Transaction View options
    wxStaticBox* transOptionStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Transaction View Options"));
    transOptionStaticBox->SetFont(staticBoxFontSetting);

    wxBoxSizer* transOptionStaticBoxSizer = new wxStaticBoxSizer(transOptionStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(transOptionStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* dateFormatSettingStaticBoxSizerGrid2 = new wxFlexGridSizer(1,2,0,5);
    transOptionStaticBoxSizer->Add(dateFormatSettingStaticBoxSizerGrid2);

    wxStaticText* transVisibleStaticText = new wxStaticText(viewsPanel, wxID_STATIC,
        _("Transactions Visible"), wxDefaultPosition, wxDefaultSize, 0);
    dateFormatSettingStaticBoxSizerGrid2->Add(transVisibleStaticText, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString itemChoiceViewTransStrings; 
    itemChoiceViewTransStrings.Add(_("View All Transactions"));
    itemChoiceViewTransStrings.Add(_("View Today"));
    itemChoiceViewTransStrings.Add(_("View Current Month"));
    itemChoiceViewTransStrings.Add(_("View Last 30 days"));
    itemChoiceViewTransStrings.Add(_("View Last 90 days"));
    itemChoiceViewTransStrings.Add(_("View Last Month"));
    itemChoiceViewTransStrings.Add(_("View Last 3 Months"));

    choiceTransVisible_ = new wxChoice(viewsPanel, ID_DIALOG_OPTIONS_VIEW_TRANS, wxDefaultPosition, wxSize(220,-1),
        itemChoiceViewTransStrings);
    dateFormatSettingStaticBoxSizerGrid2->Add(choiceTransVisible_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString vTrans = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWTRANSACTIONS"), VIEW_TRANS_ALL_STR);
    wxArrayString itemChoiceViewTransactionsString = viewTransactionsStrings(false, vTrans, row_id_);
    choiceTransVisible_->SetSelection(row_id_);
    choiceTransVisible_->SetToolTip(_("Specify which transactions are visible by default"));

    wxStaticBox* fontSizeOptionStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Font Size Options"));
    fontSizeOptionStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* fontSizeOptionStaticBoxSizer = new wxStaticBoxSizer(fontSizeOptionStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(fontSizeOptionStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxStaticText* reportFontSizeStaticText = new wxStaticText(viewsPanel, wxID_STATIC,
        _("Report Font Size"), wxDefaultPosition, wxDefaultSize, 0);
    fontSizeOptionStaticBoxSizer->Add(reportFontSizeStaticText, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString itemChoiceHTMLFontSize; 

    itemChoiceHTMLFontSize.Add(wxT("XSmall"));
    itemChoiceHTMLFontSize.Add(wxT("Small"));
    itemChoiceHTMLFontSize.Add(wxT("Normal"));
    itemChoiceHTMLFontSize.Add(wxT("Large"));
    itemChoiceHTMLFontSize.Add(wxT("XLarge"));
    itemChoiceHTMLFontSize.Add(wxT("XXLarge"));
    itemChoiceHTMLFontSize.Add(wxT("Huge"));
    
    choiceFontSize_ = new wxChoice(viewsPanel, ID_DIALOG_OPTIONS_FONT_SIZE,
        wxDefaultPosition, wxSize(85, -1), itemChoiceHTMLFontSize);

    int vFontSize = -1 + wxAtoi(mmDBWrapper::getINISettingValue(inidb_, wxT("HTMLFONTSIZE"), wxT("3")));
    choiceFontSize_->SetSelection(vFontSize);

    choiceFontSize_->SetToolTip(_("Specify which font size is used on the report tables"));
    fontSizeOptionStaticBoxSizer->Add(choiceFontSize_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Navigation Tree Expansion Options
    wxStaticBox* navTreeOptionsStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Navigation Tree Expansion Options"));
    navTreeOptionsStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* navTreeOptionsStaticBoxSizer = new wxStaticBoxSizer(navTreeOptionsStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(navTreeOptionsStaticBoxSizer, 0, wxGROW|wxALL, 5);

    // Expand Bank Tree
    wxCheckBox* expandBankCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_BANK_TREE,
        _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandBankCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_TREE"),true));
    expandBankCheckBox->SetToolTip(_("Expand Bank Accounts in Trew View when tree is refreshed"));
    navTreeOptionsStaticBoxSizer->Add(expandBankCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Expand Term Tree
    wxCheckBox* expandTermCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_TERM_TREE,
        _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandTermCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_TREE"),false));
    expandTermCheckBox->SetToolTip(_("Expand Term Accounts in Trew View when tree is refreshed"));
    navTreeOptionsStaticBoxSizer->Add(expandTermCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // Home Page Expansion Options
    wxStaticBox* homePageStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Home Page Expansion Options"));
    homePageStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* homePageStaticBoxSizer = new wxStaticBoxSizer(homePageStaticBox, wxVERTICAL);
    viewsPanelSizer->Add(homePageStaticBoxSizer, 0, wxGROW|wxALL, 5);

    // Expand Bank Home
    wxCheckBox* expandBankHomeCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_BANK_HOME,
        _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandBankHomeCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_HOME"),true));
    expandBankHomeCheckBox->SetToolTip(_("Expand Bank Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(expandBankHomeCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Expand Term Home
    wxCheckBox* itemCheckBoxExpandTermHome = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_TERM_HOME,
        _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandTermHome->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_HOME"),false));
    itemCheckBoxExpandTermHome->SetToolTip(_("Expand Term Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(itemCheckBoxExpandTermHome, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Expand Stock Home
    wxCheckBox* itemCheckBoxExpandStockHome = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME,
        _("Stock Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandStockHome->SetValue(GetIniDatabaseCheckboxValue(wxT("ENABLESTOCKS"),true));
    itemCheckBoxExpandStockHome->SetToolTip(_("Expand Stock Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(itemCheckBoxExpandStockHome, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);


    cbBudgetFinancialYears_ = new wxCheckBox(viewsPanel, wxID_ANY, _("View Budgets as Financial Years"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetFinancialYears_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_FINANCIAL_YEARS, false));
    viewsPanelSizer->Add(cbBudgetFinancialYears_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cbBudgetIncludeTransfers_ = new wxCheckBox(viewsPanel, wxID_ANY, _("View Budgets with 'transfer' transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetIncludeTransfers_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_INCLUDE_TRANSFERS, false));
    viewsPanelSizer->Add(cbBudgetIncludeTransfers_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cbBudgetSetupWithoutSummary_ = new wxCheckBox(viewsPanel, wxID_ANY, _("View Budgets Setup Without Budget Summaries"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetSetupWithoutSummary_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, false));
    viewsPanelSizer->Add(cbBudgetSetupWithoutSummary_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cbBudgetSummaryWithoutCateg_ = new wxCheckBox(viewsPanel, wxID_ANY, _("View Budget Summary Report without Categories"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetSummaryWithoutCateg_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, true));
    viewsPanelSizer->Add(cbBudgetSummaryWithoutCateg_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cbIgnoreFutureTransactions_ = new wxCheckBox(viewsPanel, wxID_ANY, _("View Reports without Future Transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbIgnoreFutureTransactions_->SetValue(GetIniDatabaseCheckboxValue(INIDB_IGNORE_FUTURE_TRANSACTIONS, false));
    viewsPanelSizer->Add(cbIgnoreFutureTransactions_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    /*********************************************************************************************
     Colours Panel
    **********************************************************************************************/
    wxPanel* colourPanel = new wxPanel(newBook, ID_BOOK_PANELCOLORS, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* colourPanelSizer = new wxBoxSizer(wxVERTICAL);
    colourPanel->SetSizer(colourPanelSizer);

    wxStaticBox* colourSettingStaticBox = new wxStaticBox(colourPanel, wxID_ANY, _("Colour Settings"));
    colourSettingStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* colourSettingStaticBoxSizer = new wxStaticBoxSizer(colourSettingStaticBox, wxVERTICAL);
    colourPanelSizer->Add(colourSettingStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* colourPanelSizerGrid = new wxFlexGridSizer(2, 2, 10, 10);
    colourSettingStaticBoxSizer->Add(colourPanelSizerGrid, 0, wxALL, 5);

    wxStaticText* navTreeStaticText = new wxStaticText(colourPanel, wxID_STATIC, _("Nav Tree"));
    wxButton* navTreeButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE,
        _("Nav Tree"), wxDefaultPosition, wxSize(150,-1), 0);
    navTreeButton->SetToolTip(_("Specify the color for the nav tree"));
    navTreeButton->SetBackgroundColour(mmColors::navTreeBkColor);
    colourPanelSizerGrid->Add(navTreeStaticText, 0, wxALIGN_LEFT, 5);
    colourPanelSizerGrid->Add(navTreeButton, 0, wxALIGN_LEFT, 5);

    wxStaticText* listBackgroundStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Background"));
    wxButton* listBackgroundButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK,
        _("List Background"), wxDefaultPosition, navTreeButton->GetSize(), 0 );
    listBackgroundButton->SetToolTip(_("Specify the color for the list background"));
    listBackgroundButton->SetBackgroundColour(mmColors::listBackColor);
    colourPanelSizerGrid->Add(listBackgroundStaticText, 0, wxALIGN_LEFT, 5);
    colourPanelSizerGrid->Add(listBackgroundButton, 0, wxALIGN_LEFT, 5);

    wxStaticText* listRowZeroStaticText = new wxStaticText(colourPanel, wxID_STATIC, _("List Row 0"));
    wxButton* listRowZeroButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0,
        _("List Row 0"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listRowZeroButton->SetToolTip(_("Specify the color for the list row 0"));
    listRowZeroButton->SetBackgroundColour(mmColors::listAlternativeColor0);
    colourPanelSizerGrid->Add(listRowZeroStaticText, 0, wxALIGN_LEFT, 5);
    colourPanelSizerGrid->Add(listRowZeroButton, 0, wxALIGN_LEFT, 5);

    wxStaticText* listRowOneStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Row 1"));
    wxButton* listRowOneButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1,
        _("List Row 1"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listRowOneButton->SetToolTip(_("Specify the color for the list row 1"));
    listRowOneButton->SetBackgroundColour(mmColors::listAlternativeColor1);
    colourPanelSizerGrid->Add(listRowOneStaticText, 0, wxALIGN_LEFT, 5);
    colourPanelSizerGrid->Add(listRowOneButton, 0, wxALIGN_LEFT, 5);

    wxStaticText* listBorderSaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Border"));
    wxButton* listBorderButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER,
        _("List Border"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listBorderButton->SetToolTip(_("Specify the color for the list Border"));
    listBorderButton->SetBackgroundColour(mmColors::listBorderColor);
    colourPanelSizerGrid->Add(listBorderSaticText, 0, wxALIGN_LEFT, 5);
    colourPanelSizerGrid->Add(listBorderButton, 0, wxALIGN_LEFT, 5);
    
    wxStaticText* listDetailsStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Details"));
    wxButton* listDetailsButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS,
        _("List Details"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listDetailsButton->SetToolTip(_("Specify the color for the list details"));
    listDetailsButton->SetBackgroundColour(mmColors::listDetailsPanelColor);
    colourPanelSizerGrid->Add(listDetailsStaticText, 0, wxALIGN_LEFT, 5);
    colourPanelSizerGrid->Add(listDetailsButton, 0, wxALIGN_LEFT, 5);
    
    wxStaticText* futureTransStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("Future Transactions"));
    wxButton* futureTransButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES,
        _("Future Transactions"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    futureTransButton->SetToolTip(_("Specify the color for future transactions"));
    futureTransButton->SetBackgroundColour(mmColors::listFutureDateColor);
    colourPanelSizerGrid->Add(futureTransStaticText, 0, wxALIGN_LEFT, 5);
    colourPanelSizerGrid->Add(futureTransButton, 0, wxALIGN_LEFT, 5);

    wxButton* restoreDefaultButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT,
        _("Restore Defaults"), wxDefaultPosition, wxDefaultSize, 0);
    restoreDefaultButton->SetToolTip(_("Restore Default Colors"));
    colourPanelSizer->Add(restoreDefaultButton, 0, wxALIGN_LEFT|wxALL, 5);
   
    /*********************************************************************************************
     Others Panel
    **********************************************************************************************/
    wxPanel* othersPanel = new wxPanel(newBook, ID_BOOK_PANELMISC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* othersPanelSizer = new wxBoxSizer(wxVERTICAL);
    othersPanel->SetSizer(othersPanelSizer);
    
    // New transaction dialog settings
    wxStaticBox* transSettingsStaticBox = new wxStaticBox(othersPanel, wxID_ANY, _("New Transaction Dialog Settings"));
    transSettingsStaticBox->SetFont(staticBoxFontSetting);

    wxStaticBoxSizer* transSettingsStaticBoxSizer = new wxStaticBoxSizer(transSettingsStaticBox, wxVERTICAL);
    othersPanelSizer->Add(transSettingsStaticBoxSizer, 0, wxGROW|wxALL, 5);

    //  Default Date
    wxStaticText* dateStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Date:"), wxDefaultPosition, wxDefaultSize);

    wxArrayString defaultValues_;
    defaultValues_.Add(_("None"));
    defaultValues_.Add(_("Last Used"));
    
    wxChoice* defaultDateChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE,
        wxDefaultPosition, wxSize(140, -1), defaultValues_);
    defaultDateChoice->SetSelection(mmIniOptions::instance().transDateDefault_);

    //  Default Payee
    wxStaticText* payeeStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Payee:"), wxDefaultPosition, wxDefaultSize);

    wxChoice* defaultPayeeChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE,
        wxDefaultPosition, wxSize(140, -1), defaultValues_);
    defaultPayeeChoice->SetSelection(mmIniOptions::instance().transPayeeSelectionNone_);
    
    //  Default Category
    wxStaticText* categoryStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Category:"), wxDefaultPosition, wxDefaultSize);


    defaultValues_[1]=(_("Last used for payee"));
    
    wxChoice* defaultCategoryChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY,
        wxDefaultPosition, defaultPayeeChoice->GetSize(), defaultValues_);
    defaultCategoryChoice->SetSelection(mmIniOptions::instance().transCategorySelectionNone_);

    //  Default Status
    wxStaticText* statusStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Status:"), wxDefaultPosition, defaultPayeeChoice->GetSize());

    wxArrayString choiceStatusStrings;
    choiceStatusStrings.Add(_("None"));
    choiceStatusStrings.Add(_("Reconciled"));
    choiceStatusStrings.Add(_("Void"));
    choiceStatusStrings.Add(_("Follow up"));
    choiceStatusStrings.Add(_("Duplicate"));

    wxChoice* defaultStatusChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS,
        wxDefaultPosition, defaultPayeeChoice->GetSize(), choiceStatusStrings);
    defaultStatusChoice->SetSelection(mmIniOptions::instance().transStatusReconciled_);

    wxFlexGridSizer* newTransflexGridSizer = new wxFlexGridSizer(3,2,0,0);
    transSettingsStaticBoxSizer->Add(newTransflexGridSizer);
    newTransflexGridSizer->Add(dateStaticText,       0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(defaultDateChoice,    0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(payeeStaticText,      0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(defaultPayeeChoice,   0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(categoryStaticText,   0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(defaultCategoryChoice,0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(statusStaticText,     0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(defaultStatusChoice,  0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //----------------------------------------------
    //a bit more space visual appearance
    othersPanelSizer->AddSpacer(10);
  
    wxBoxSizer* itemBoxSizerStockURL = new wxBoxSizer(wxVERTICAL);
    othersPanelSizer->Add(itemBoxSizerStockURL, 0, wxGROW|wxALIGN_LEFT|wxALL, 0);

    wxStaticText* itemStaticTextURL = new wxStaticText(othersPanel, wxID_STATIC, _("Stock Quote Web Page"), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticTextURL->SetFont(staticBoxFontSetting);
    itemBoxSizerStockURL->Add(itemStaticTextURL, 0, wxALIGN_LEFT|wxALL, 5);

    wxString stockURL = mmDBWrapper::getInfoSettingValue(db_, wxT("STOCKURL"), mmex::DEFSTOCKURL);
    wxTextCtrl* itemTextCtrURL = new wxTextCtrl(othersPanel, ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL, stockURL, wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizerStockURL->Add(itemTextCtrURL, 1, wxGROW|wxALIGN_LEFT|wxALL, 5);
    itemTextCtrURL->SetToolTip(_("Clear the field to Reset the value to system default."));

    //a bit more space visual appearance
    othersPanelSizer->AddSpacer(15);

    cbUseOrgDateCopyPaste_ = new wxCheckBox(othersPanel, wxID_ANY, _("Use Original Date when Pasting Transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbUseOrgDateCopyPaste_->SetValue(GetIniDatabaseCheckboxValue(INIDB_USE_ORG_DATE_COPYPASTE, false));
    cbUseOrgDateCopyPaste_->SetToolTip(_("Select whether to use the original transaction date or current date when copying/pasting transactions"));
    othersPanelSizer->Add(cbUseOrgDateCopyPaste_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    cbUseSound_ = new wxCheckBox(othersPanel, wxID_ANY, _("Use Transaction Sound"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbUseSound_->SetValue(GetIniDatabaseCheckboxValue(INIDB_USE_TRANSACTION_SOUND,true));
    cbUseSound_->SetToolTip(_("Select whether to use sounds when entering transactions"));
    othersPanelSizer->Add(cbUseSound_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    cbEnableCurrencyUpd_ = new wxCheckBox(othersPanel, wxID_ANY, _("Enable online currency update \n(Get data from European Central Bank)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbEnableCurrencyUpd_->SetValue(GetIniDatabaseCheckboxValue(INIDB_UPDATE_CURRENCY_RATE, false));
    cbEnableCurrencyUpd_->SetToolTip(_("Enable or disable get data from European Central Bank to update currency rate"));
    othersPanelSizer->Add(cbEnableCurrencyUpd_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    /*********************************************************************************************
     Import/Export Panel
    **********************************************************************************************/
    wxPanel* importExportPanel = new wxPanel(newBook, ID_BOOK_PANEL_EXP_IMP, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    wxBoxSizer* importExportPanelSizer = new wxBoxSizer(wxVERTICAL);
    importExportPanel->SetSizer(importExportPanelSizer);

    wxStaticBox* importExportStaticBox = new wxStaticBox(importExportPanel, wxID_ANY, _("Import/Export Settings"));
    importExportStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* importExportStaticBoxSizer = new wxStaticBoxSizer(importExportStaticBox, wxVERTICAL);
    
    importExportPanelSizer->Add(importExportStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxStaticText* csvDelimiterStaticText = new wxStaticText(importExportPanel, wxID_STATIC, _("CSV Delimiter"), wxDefaultPosition, wxDefaultSize, 0);
    importExportStaticBoxSizer->Add(csvDelimiterStaticText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* radioButtonSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* userDefinedSizer = new wxBoxSizer(wxHORIZONTAL);
    importExportStaticBoxSizer->Add(radioButtonSizer, 0, wxALL);
    importExportStaticBoxSizer->Add(userDefinedSizer, 0, wxALL);
    importExportStaticBoxSizer->AddSpacer(5);

    wxString delimiter = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), mmex::DEFDELIMTER);
    
    wxRadioButton* delimiterRadioButtonU4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_USER4, _("User Defined"), wxDefaultPosition, wxDefaultSize, 0);
    wxRadioButton* delimiterRadioButtonC4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_COMMA4, _("Comma"), wxDefaultPosition, wxDefaultSize, 0);
    if (delimiter == wxT(",")) delimiterRadioButtonC4 ->SetValue(true);

    wxRadioButton* delimiterRadioButtonS4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_SEMICOLON4, _("Semicolon"), wxDefaultPosition, wxDefaultSize, 0);
    if (delimiter == wxT(";")) delimiterRadioButtonS4 ->SetValue(true);

    wxRadioButton* delimiterRadioButtonT4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_TAB4, _("TAB"), wxDefaultPosition, wxDefaultSize, 0);
    if (delimiter == wxT("\t")) delimiterRadioButtonT4 ->SetValue(true);

    wxTextCtrl* textDelimiter4 = new wxTextCtrl( importExportPanel, ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4, delimiter, wxDefaultPosition, wxDefaultSize, 0);
    textDelimiter4->SetToolTip(_("Specify the delimiter to use when importing/exporting CSV files"));
    textDelimiter4->SetMaxLength(2);
    if (delimiter == wxT("\t") || delimiter == wxT(",") || delimiter == wxT(";"))
        textDelimiter4->Enable(false);

    radioButtonSizer->Add(delimiterRadioButtonC4, 0, wxALIGN_LEFT|wxALL, 5);
    radioButtonSizer->Add(delimiterRadioButtonS4, 0, wxALIGN_LEFT|wxALL, 5);
    radioButtonSizer->Add(delimiterRadioButtonT4, 0, wxALIGN_LEFT|wxALL, 5);

    userDefinedSizer->Add(delimiterRadioButtonU4, 0, wxALIGN_LEFT|wxALL, 5);
    userDefinedSizer->Add(textDelimiter4, 0, wxALIGN_LEFT|wxLEFT, 5);

// temporarily hide from interface because the user defined CSV delimiter as this is not supported by the universal CSV dialog.
    delimiterRadioButtonU4->Hide();
    textDelimiter4->Hide();

   /**********************************************************************************************
    Setting up the notebook with the 5 pages
    **********************************************************************************************/
    newBook->SetImageList(m_imageList);

    newBook->InsertPage(0, generalPanel, _("General"), true, 2);
    newBook->InsertPage(1, viewsPanel, _("View Options"), false, 0);
    newBook->InsertPage(2, colourPanel, _("Colors"), false, 1);
    newBook->InsertPage(3, importExportPanel, _("Import/Export"), false, 4);
    newBook->InsertPage(4, othersPanel, _("Others"), false, 3);

    mainDialogPanelSizer->Add(newBook, 1, wxGROW|wxALL, 5);
    mainDialogPanelSizer->Layout();

   /**********************************************************************************************
    Button Panel with OK and Cancel Buttons
    **********************************************************************************************/
    wxStaticLine* panelSeparatorLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    mainDialogSizer->Add(panelSeparatorLine,0,wxGROW|wxLEFT|wxRIGHT, 10);
    
    wxPanel* buttonPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainDialogSizer->Add(buttonPanel, 0, wxALIGN_RIGHT|wxALL, 5);
    
    wxButton* itemButtonOK = new wxButton(buttonPanel, wxID_OK, _("&OK"));
    wxButton* itemButtonCancel = new wxButton(buttonPanel, wxID_CANCEL, _("&Cancel"));
    buttonPanelSizer->Add(itemButtonOK, 0, wxALIGN_RIGHT|wxRIGHT, 5);
    buttonPanelSizer->Add(itemButtonCancel, 0, wxALIGN_RIGHT|wxRIGHT, 5);
    itemButtonOK->SetFocus();
}

void mmOptionsDialog::OnLanguageChanged(wxCommandEvent& /*event*/)
{
    wxString lang = mmSelectLanguage(this, inidb_, true, false);
    if (lang.empty()) return;

    // Advisable to restart GUI when user acknowledges the change.
    restartRequired_ = true;

    wxButton *btn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
    wxASSERT(btn);
    btn->SetLabel(lang.Left(1).Upper() + lang.SubString(1,lang.Len()));
}

void mmOptionsDialog::OnCurrency(wxCommandEvent& /*event*/)
{
    int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
     
    if (mmMainCurrencyDialog::Execute(core_, this, currencyID) && currencyID != -1)
    {
        wxString currName = core_->getCurrencyName(currencyID);
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_CURRENCY);
        bn->SetLabel(currName);
        currencyId_ = currencyID;

        wxMessageDialog msgDlg(this, _("Remember to update currency rate"), _("Important note"));
        msgDlg.ShowModal();            
    }
}

void mmOptionsDialog::OnDateFormatChanged(wxCommandEvent& /*event*/)
{
    wxString newFormat = choiceDateFormat_->GetValue();
    if (newFormat == DisplayDate2FormatDate(newFormat)) // Not a predefined format
    {
        choiceDateFormat_->SetValue(FormatDate2DisplayDate(mmex::DEFDATEFORMAT));
        return;
    }

    try // setting the date to the new format to ensure it works.
    {
        wxDateTime::Now().Format(DisplayDate2FormatDate(newFormat));
    }
    catch(...)
    {
        choiceDateFormat_->SetValue(FormatDate2DisplayDate(mmex::DEFDATEFORMAT));
        return;
    }

    dateFormat_ = DisplayDate2FormatDate(newFormat);
    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE);
    st->SetLabel(wxDateTime::Now().Format(dateFormat_) + _(" : Requires MMEX Restart"));
    restartRequired_ = true;
}

void mmOptionsDialog::OnNavTreeColorChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(navTreeBkColor_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColour col = dialog.GetColourData().GetColour();
        navTreeBkColor_ = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnAlt0Changed(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(listAlternativeColor0_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColour col = dialog.GetColourData().GetColour();
        listAlternativeColor0_ = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnAlt1Changed(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(listAlternativeColor1_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColour col = dialog.GetColourData().GetColour();
        listAlternativeColor1_ = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnListBackgroundChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(listBackColor_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColour col = dialog.GetColourData().GetColour();
        listBackColor_ = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnListBorderChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(listBorderColor_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColour col = dialog.GetColourData().GetColour();
        listBorderColor_ = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER);
        bn->SetBackgroundColour(col);
    }
}

void  mmOptionsDialog::OnListDetailsColors(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(listDetailsPanelColor_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColour col = dialog.GetColourData().GetColour();
        listDetailsPanelColor_ = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS);
        bn->SetBackgroundColour(col);
    }
}

void  mmOptionsDialog::OnListFutureDates(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(listFutureDateColor_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColour col = dialog.GetColourData().GetColour();
        listFutureDateColor_ = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnRestoreDefaultColors(wxCommandEvent& /*event*/)
{
    // set the new colours to the default colours
    navTreeBkColor_ = wxColour(255, 255, 255);
    listAlternativeColor0_ = wxColour(225, 237, 251);
    listAlternativeColor1_ = wxColour(255, 255, 255);
    listBackColor_ = wxColour(255, 255, 255);
    listBorderColor_ = wxColour(0, 0, 0);
    listDetailsPanelColor_ = wxColour(244, 247, 251);
    listFutureDateColor_ = wxColour(116, 134, 168);

    // Display the original colours to the user
    wxButton* bn1 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE);
    bn1->SetBackgroundColour(navTreeBkColor_);

    wxButton* bn2 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0);
    bn2->SetBackgroundColour(listAlternativeColor0_);

    wxButton* bn3 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1);
    bn3->SetBackgroundColour(listAlternativeColor1_);

    wxButton* bn4 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK);
    bn4->SetBackgroundColour(listBackColor_);

    wxButton* bn5 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER);
    bn5->SetBackgroundColour(listBorderColor_);

    wxButton* bn6 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS);
    bn6->SetBackgroundColour(listDetailsPanelColor_);

    wxButton* bn7 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES);
    bn7->SetBackgroundColour(listFutureDateColor_);
}

void mmOptionsDialog::SetIniDatabaseCheckboxValue(wxString dbField, bool dbState)
{
    if (dbState)
        mmDBWrapper::setINISettingValue(inidb_, dbField, wxT("TRUE"));
    else
        mmDBWrapper::setINISettingValue(inidb_, dbField, wxT("FALSE"));
}

bool mmOptionsDialog::GetIniDatabaseCheckboxValue(wxString dbField, bool defaultState)
{
    wxString dbState = wxT("FALSE");
    bool result = false;
    
    if (defaultState)
    {
        dbState = wxT("TRUE");
        result = true;
    }
    if (mmDBWrapper::getINISettingValue(inidb_, dbField, dbState) != dbState)
    {
        result = !result;
    }
    return result;
}

void mmOptionsDialog::OnDelimiterSelectedU(wxCommandEvent& /*event*/)
{
    wxStaticText* d = (wxStaticText*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4);
    d ->Enable(true);
}

void mmOptionsDialog::OnDelimiterSelectedC(wxCommandEvent& /*event*/)
{
    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4);
    st ->Enable(false);
    st ->SetValue(wxT(","));
}

void mmOptionsDialog::OnDelimiterSelectedS(wxCommandEvent& /*event*/)
{
    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4);
    st ->Enable(false);
    st ->SetValue(wxT(";"));
}

void mmOptionsDialog::OnDelimiterSelectedT(wxCommandEvent& /*event*/)
{
    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4);
    st ->Enable(false);
    st ->SetValue(wxT("\t"));
}

void mmOptionsDialog::SaveViewAccountOptions()
{
    int selection = choiceVisible_->GetSelection();
    int row_id_ = 0;
    wxArrayString viewAcct = viewAccountStrings(false, wxEmptyString, row_id_);
    mmDBWrapper::setINISettingValue(inidb_, wxT("VIEWACCOUNTS"), viewAcct[selection]);
}

void mmOptionsDialog::SaveViewTransactionOptions()
{
    int selection = choiceTransVisible_->GetSelection();
    int row_id_ = 0;
    wxArrayString ViewTransaction = viewTransactionsStrings(false, wxEmptyString, row_id_);
    mmDBWrapper::setINISettingValue(inidb_, wxT("VIEWTRANSACTIONS"), ViewTransaction[selection]);
}

void mmOptionsDialog::SaveFinancialYearStart()
{
    //Save Financial Year Start Day
    wxSpinCtrl* fysDay = (wxSpinCtrl*)FindWindow(ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY);
    wxString fysDayVal = wxString::Format(wxT("%d"),fysDay->GetValue());
    mmOptions::instance().financialYearStartDayString_ = fysDayVal;
    mmDBWrapper::setInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_DAY"), fysDayVal); 

    //Save Financial Year Start Month
    wxString fysMonthVal = wxString() << monthSelection_->GetSelection() + 1;
    mmOptions::instance().financialYearStartMonthString_ = fysMonthVal;
    mmDBWrapper::setInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_MONTH"), fysMonthVal); 
}

void mmOptionsDialog::SaveStocksUrl()
{
    wxTextCtrl* url = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL);
    wxString stockURL = url->GetValue();
    if (!stockURL.IsEmpty())
    {
        mmDBWrapper::setInfoSettingValue(db_, wxT("STOCKURL"), stockURL); 
    } 
    else
    {
        // Clear database record: Allows value to reset to system default.
        db_->ExecuteUpdate("delete from INFOTABLE_V1 where INFONAME = \"STOCKURL\";");
    }
}

/// Saves the updated System Options to the appropriate databases.
void mmOptionsDialog::SaveNewSystemSettings()
{
    // initialize database saves -------------------------------------------------------------
    db_->Begin();
    inidb_->Begin();

    // Save all the details for all the panels
    SaveGeneralPanelSettings();
    SaveViewPanelSettings();
    SaveColourPanelSettings();
    SaveOthersPanelSettings();
    SaveImportExportPanelSettings();

    // finalise database saves ---------------------------------------------------------------
    db_->Commit();
    inidb_->Commit();
}

void mmOptionsDialog::SaveGeneralPanelSettings()
{
    wxTextCtrl* stun = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME);
    mmIniOptions::instance().userNameString_ = stun->GetValue();
    mmDBWrapper::setInfoSettingValue(db_, wxT("USERNAME"), mmIniOptions::instance().userNameString_); 

    wxButton *languageButton = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
    mmDBWrapper::setINISettingValue(inidb_, LANGUAGE_PARAMETER, languageButton->GetLabel().Lower());
    mmSelectLanguage(0, inidb_, false);

    mmDBWrapper::setBaseCurrencySettings(db_, currencyId_);
    mmDBWrapper::setInfoSettingValue(db_, wxT("DATEFORMAT"), dateFormat_);
    SaveFinancialYearStart();
    
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP);
    SetIniDatabaseCheckboxValue(wxT("BACKUPDB"), itemCheckBox->GetValue() );

    wxCheckBox* itemCheckBoxUpdate = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE);
    SetIniDatabaseCheckboxValue(wxT("BACKUPDB_UPDATE"), itemCheckBoxUpdate->GetValue() );
}

void mmOptionsDialog::SaveViewPanelSettings()
{
    SaveViewAccountOptions();
    SaveViewTransactionOptions();

    int size = choiceFontSize_->GetCurrentSelection() + 1;
    mmIniOptions::instance().fontSize_ = wxString::Format(wxT("%d"), size);
    mmDBWrapper::setINISettingValue(inidb_, wxT("HTMLFONTSIZE"), mmIniOptions::instance().fontSize_);

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_TREE);
    mmIniOptions::instance().expandBankTree_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_TREE"),mmIniOptions::instance().expandBankTree_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_TREE);
    mmIniOptions::instance().expandTermTree_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_TREE"),mmIniOptions::instance().expandTermTree_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_HOME);
    mmIniOptions::instance().expandBankHome_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_HOME"),mmIniOptions::instance().expandBankHome_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_HOME);
    mmIniOptions::instance().expandTermHome_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_HOME"),mmIniOptions::instance().expandTermHome_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME);
    mmIniOptions::instance().expandStocksHome_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("ENABLESTOCKS"),mmIniOptions::instance().expandStocksHome_);

    mmIniOptions::instance().budgetFinancialYears_ = cbBudgetFinancialYears_->GetValue();
    SetIniDatabaseCheckboxValue(INIDB_BUDGET_FINANCIAL_YEARS, mmIniOptions::instance().budgetFinancialYears_);

    mmIniOptions::instance().budgetIncludeTransfers_ = cbBudgetIncludeTransfers_->GetValue();
    SetIniDatabaseCheckboxValue(INIDB_BUDGET_INCLUDE_TRANSFERS, mmIniOptions::instance().budgetIncludeTransfers_);

    mmIniOptions::instance().budgetSetupWithoutSummaries_ = cbBudgetSetupWithoutSummary_->GetValue();
    SetIniDatabaseCheckboxValue(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, mmIniOptions::instance().budgetSetupWithoutSummaries_);

    mmIniOptions::instance().budgetSummaryWithoutCategories_ = cbBudgetSummaryWithoutCateg_->GetValue();
    SetIniDatabaseCheckboxValue(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, mmIniOptions::instance().budgetSummaryWithoutCategories_);

    mmIniOptions::instance().ignoreFutureTransactions_ = cbIgnoreFutureTransactions_->GetValue();
    SetIniDatabaseCheckboxValue(INIDB_IGNORE_FUTURE_TRANSACTIONS, mmIniOptions::instance().ignoreFutureTransactions_);
}

void mmOptionsDialog::SaveColourPanelSettings()
{
    mmColors::navTreeBkColor = navTreeBkColor_;
    mmColors::listAlternativeColor0 = listAlternativeColor0_;
    mmColors::listAlternativeColor1 = listAlternativeColor1_;
    mmColors::listBackColor = listBackColor_;
    mmColors::listBorderColor = listBorderColor_;
    mmColors::listDetailsPanelColor = listDetailsPanelColor_;
    mmColors::listFutureDateColor = listFutureDateColor_;

    mmDBWrapper::setINISettingValue(inidb_, wxT("LISTALT0"), mmGetStringFromColour(mmColors::listAlternativeColor0));
    mmDBWrapper::setINISettingValue(inidb_, wxT("LISTALT1"), mmGetStringFromColour(mmColors::listAlternativeColor1));
    mmDBWrapper::setINISettingValue(inidb_, wxT("LISTBACK"), mmGetStringFromColour(mmColors::listBackColor));
    mmDBWrapper::setINISettingValue(inidb_, wxT("NAVTREE"),  mmGetStringFromColour(mmColors::navTreeBkColor));
    mmDBWrapper::setINISettingValue(inidb_, wxT("LISTBORDER"), mmGetStringFromColour(mmColors::listBorderColor));
    mmDBWrapper::setINISettingValue(inidb_, wxT("LISTDETAILSPANEL"), mmGetStringFromColour(mmColors::listDetailsPanelColor));
    mmDBWrapper::setINISettingValue(inidb_, wxT("LISTFUTUREDATES"), mmGetStringFromColour(mmColors::listFutureDateColor));
}

void mmOptionsDialog::SaveOthersPanelSettings()
{
    wxChoice* itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE);
    mmIniOptions::instance().transPayeeSelectionNone_ = itemChoice->GetSelection();
    mmDBWrapper::setINISettingValue(inidb_, wxT("TRANSACTION_PAYEE_NONE"),
        wxString::Format(wxT("%d"), (int)mmIniOptions::instance().transPayeeSelectionNone_)); 
    
    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY);
    mmIniOptions::instance().transCategorySelectionNone_ = itemChoice->GetSelection();
    mmDBWrapper::setINISettingValue(inidb_, wxT("TRANSACTION_CATEGORY_NONE"),
        wxString::Format(wxT("%d"), (int)mmIniOptions::instance().transCategorySelectionNone_)); 
    
    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS);
    mmIniOptions::instance().transStatusReconciled_ = itemChoice->GetSelection();
    mmDBWrapper::setINISettingValue(inidb_, wxT("TRANSACTION_STATUS_RECONCILED"),
        wxString::Format(wxT("%d"), (int)mmIniOptions::instance().transStatusReconciled_)); 

    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE);
    mmIniOptions::instance().transDateDefault_ = itemChoice->GetSelection();
    mmDBWrapper::setINISettingValue(inidb_, wxT("TRANSACTION_DATE_DEFAULT"),
        wxString::Format(wxT("%d"), (int)mmIniOptions::instance().transDateDefault_)); 
    
    SaveStocksUrl();
    
    SetIniDatabaseCheckboxValue(INIDB_USE_ORG_DATE_COPYPASTE, cbUseOrgDateCopyPaste_->GetValue());
    
    SetIniDatabaseCheckboxValue(INIDB_USE_TRANSACTION_SOUND, cbUseSound_->GetValue());

    SetIniDatabaseCheckboxValue(INIDB_UPDATE_CURRENCY_RATE, cbEnableCurrencyUpd_->GetValue());
}

void mmOptionsDialog::SaveImportExportPanelSettings()
{
    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4);
    wxString delim = st->GetValue();
    if (!delim.IsEmpty()) mmDBWrapper::setInfoSettingValue(db_, wxT("DELIMITER"), delim); 
}

bool mmOptionsDialog::GetUpdateCurrencyRateSetting()
{
    return cbEnableCurrencyUpd_->GetValue();
}

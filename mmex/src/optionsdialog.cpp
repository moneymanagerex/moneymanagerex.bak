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
#include "maincurrencydialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include "mmgraphgenerator.h"
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <limits>

IMPLEMENT_DYNAMIC_CLASS( mmOptionsDialog, wxDialog )

BEGIN_EVENT_TABLE( mmOptionsDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmOptionsDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, mmOptionsDialog::OnCurrency)

    /// Colour Changing events
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE, mmOptionsDialog::OnColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0, mmOptionsDialog::OnColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1, mmOptionsDialog::OnColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK, mmOptionsDialog::OnColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER, mmOptionsDialog::OnColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS, mmOptionsDialog::OnColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES, mmOptionsDialog::OnColorChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT, mmOptionsDialog::OnRestoreDefaultColors)
    EVT_RADIOBOX(ID_RADIO_BOX, mmOptionsDialog::OnDelimiterSelected)
END_EVENT_TABLE()

#include "../resources/main-setup.xpm"
#include "../resources/preferences-color.xpm"
#include "../resources/view.xpm"
#include "../resources/preferences-other.xpm"
#include "../resources/export-import.xpm"

#define _MM_EX_OPTIONSDIALOG_CPP_REVISION_ID    "$Revision$"

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
                                 : core_(core), inidb_(inidb), db_(core->db_.get()),
                                 restartRequired_(false), changesApplied_(false)
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
    dateFormat_ = mmDBWrapper::getInfoSettingValue(db_, ("DATEFORMAT"), mmex::DEFDATEFORMAT);

    CreateControls();
    Centre();
    Fit();

    return TRUE;
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
    wxConfigBase *config = wxConfigBase::Get();
    
    wxSizerFlags flags(1);
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);

    wxSize imageSize(48, 48);
    m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
    m_imageList->Add(wxBitmap(view_xpm));
    m_imageList->Add(wxBitmap(preferences_color_xpm));
    m_imageList->Add(wxBitmap(main_setup_xpm));
    m_imageList->Add(wxBitmap(preferences_other_xpm));
    m_imageList->Add(wxBitmap(export_import_xpm));

    wxBoxSizer* mainDialogSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainDialogSizer);

    wxPanel* mainDialogPanel = new wxPanel(this,
        wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    mainDialogSizer->Add(mainDialogPanel, 0, wxGROW|wxALL, 5);

    wxBoxSizer* mainDialogPanelSizer = new wxBoxSizer(wxVERTICAL);
    mainDialogPanel->SetSizer(mainDialogPanelSizer);

    wxListbook* newBook = new wxListbook(mainDialogPanel,
        ID_DIALOG_OPTIONS_LISTBOOK, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);

    /*********************************************************************************************
     General Panel
    **********************************************************************************************/
    wxPanel* generalPanel = new wxPanel(newBook,
        ID_BOOK_PANELGENERAL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

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
    headerStaticBoxSizer->Add(userNameText, flags);

    wxString userName = mmDBWrapper::getInfoSettingValue(db_, ("USERNAME"), (""));
    wxTextCtrl* userNameTextCtr = new wxTextCtrl(generalPanel, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME,
        userName, wxDefaultPosition, wxDefaultSize, 0);
    userNameTextCtr->SetToolTip(_("The User Name is used as a title for the database."));
    headerStaticBoxSizer->Add(userNameTextCtr, 1, wxALIGN_LEFT|wxGROW|wxALL, 5);

    // Language Settings
    wxStaticBox* languageStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Language"));
    languageStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* languageStaticBoxSizer = new wxStaticBoxSizer(languageStaticBox, wxHORIZONTAL);
    generalPanelSizer->Add(languageStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);

    currentLanguage_ = config->Read(LANGUAGE_PARAMETER, "English");
    wxButton* languageButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_LANGUAGE,
        currentLanguage_.Capitalize(), wxDefaultPosition, wxSize(150, -1));
    languageButton->SetToolTip(_("Specify the language to use"));
    languageButton->Connect(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE, wxEVT_COMMAND_BUTTON_CLICKED,
                            wxCommandEventHandler(mmOptionsDialog::OnLanguageChanged), NULL, this);

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
    wxFlexGridSizer* dateFormatSettingStaticBoxSizerGrid = new wxFlexGridSizer(0,2,0,5);
    generalPanelSizer->Add(dateFormatStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    dateFormatStaticBoxSizer->Add(dateFormatSettingStaticBoxSizerGrid);

    wxArrayString itemChoice7Strings = itemChoiceStrings();
    wxArrayString DateFormat = DateFormats();
    wxString default_date_format = mmDBWrapper::getInfoSettingValue(db_, ("DATEFORMAT"), mmex::DEFDATEFORMAT);
    size_t i=0;
    for(i; i<DateFormat.Count(); i++)
    {
        if(default_date_format == DateFormat[i])
            break;
    }

    choiceDateFormat_ = new wxChoice(generalPanel, ID_DIALOG_OPTIONS_DATE_FORMAT,
        wxDefaultPosition, wxSize(140, -1), itemChoice7Strings, 0);
    choiceDateFormat_->SetSelection(i);
    dateFormatSettingStaticBoxSizerGrid->Add(choiceDateFormat_, flags);
    choiceDateFormat_->SetToolTip(_("Specify the date format for display"));
    choiceDateFormat_->Connect(ID_DIALOG_OPTIONS_DATE_FORMAT, wxEVT_COMMAND_CHOICE_SELECTED,
                               wxCommandEventHandler(mmOptionsDialog::OnDateFormatChanged), NULL, this);

    wxStaticText* restartText = new wxStaticText( generalPanel, ID_DIALOG_OPTIONS_RESTART_REQUIRED,
        (""), wxDefaultPosition, wxDefaultSize, 0);
    wxStaticText* sampleDateExampleText = new wxStaticText( generalPanel, wxID_ANY,
        _("New date format sample:"), wxDefaultPosition, wxDefaultSize, 0);
    wxStaticText* sampleDateText = new wxStaticText(generalPanel, ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE,
        ("redefined elsewhere"), wxDefaultPosition, wxDefaultSize, 0);
    dateFormatSettingStaticBoxSizerGrid->Add(restartText, flags);
    dateFormatSettingStaticBoxSizerGrid->Add(sampleDateExampleText, flags);
    dateFormatSettingStaticBoxSizerGrid->Add(sampleDateText, flags);
    sampleDateText->SetLabel(wxDateTime::Now().Format(dateFormat_));

    // Financial Year Settings
    wxStaticBox* financialYearStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Financial Year"));
    financialYearStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* financialYearStaticBoxSizer = new wxStaticBoxSizer(financialYearStaticBox, wxVERTICAL);
    wxFlexGridSizer* financialYearStaticBoxSizerGrid = new wxFlexGridSizer(0,2,0,0);
    generalPanelSizer->Add(financialYearStaticBoxSizer, 0, wxGROW|wxALL, 5);
    financialYearStaticBoxSizer->Add(financialYearStaticBoxSizerGrid);

    wxStaticText* itemStaticTextFYSDay = new wxStaticText(generalPanel, wxID_STATIC, _("Start Day"),
        wxDefaultPosition, wxDefaultSize, 0 );
    financialYearStaticBoxSizerGrid->Add(itemStaticTextFYSDay, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString financialPeriodStartDay = mmDBWrapper::getInfoSettingValue(db_, ("FINANCIAL_YEAR_START_DAY"), ("1"));
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
    financialYearStaticBoxSizerGrid->Add(monthSelection_, flags);
    wxString financialPeriodStartMonth = mmDBWrapper::getInfoSettingValue(db_, ("FINANCIAL_YEAR_START_MONTH"), ("7"));

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
    backupCheckBox->SetValue(GetIniDatabaseCheckboxValue(("BACKUPDB"),false));
    backupCheckBox->SetToolTip(_("When MMEX Starts,\ncreates the backup database: dbFile_start_YYYY-MM-DD.ext."));
    backupStaticBoxSizer->Add(backupCheckBox, flags);

    wxCheckBox* backupUpdateCheckBox = new wxCheckBox(generalPanel, ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE,
        _("Update database changes to database backup on exit."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    backupUpdateCheckBox->SetValue(GetIniDatabaseCheckboxValue(("BACKUPDB_UPDATE"),false));
    backupUpdateCheckBox->SetToolTip(_("When MMEX shuts down and changes made to database,\ncreates or updates the backup database: dbFile_update_YYYY-MM-DD.ext."));
    backupStaticBoxSizer->Add(backupUpdateCheckBox, flags);

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

    wxStaticText* accountStaticText = new wxStaticText( viewsPanel,
        wxID_STATIC, _("Accounts Visible"));
    accountStaticBoxSizer->Add(accountStaticText, flags);

    int row_id_ = 0;
    wxArrayString itemChoiceViewAccountTranslatedStrings = viewAccountStrings(true, wxEmptyString, row_id_);

    choiceVisible_ = new wxChoice(viewsPanel, ID_DIALOG_OPTIONS_VIEW_ACCOUNTS,
        wxDefaultPosition, wxDefaultSize, itemChoiceViewAccountTranslatedStrings);
    accountStaticBoxSizer->Add(choiceVisible_, 1, wxGROW|wxALL, 5);

    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, ("VIEWACCOUNTS"), VIEW_ACCOUNTS_ALL_STR);
    row_id_ = 0;
    wxArrayString itemChoiceViewAccountStrings = viewAccountStrings(false, vAccts, row_id_);
    choiceVisible_->SetSelection(row_id_);

    choiceVisible_->SetToolTip(_("Specify which accounts are visible"));

    // Transaction View options
    wxStaticBox* transOptionStaticBox = new wxStaticBox(viewsPanel,
        wxID_ANY, _("Transaction View Options"));
    transOptionStaticBox->SetFont(staticBoxFontSetting);

    wxBoxSizer* transOptionStaticBoxSizer = new wxStaticBoxSizer(transOptionStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(transOptionStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* dateFormatSettingStaticBoxSizerGrid2 = new wxFlexGridSizer(0,2,0,5);
    transOptionStaticBoxSizer->Add(dateFormatSettingStaticBoxSizerGrid2);

    wxStaticText* transVisibleStaticText = new wxStaticText(viewsPanel,
        wxID_STATIC, _("Transactions Visible"));
    dateFormatSettingStaticBoxSizerGrid2->Add(transVisibleStaticText, flags);

    wxArrayString itemChoiceViewTransStrings;
    itemChoiceViewTransStrings.Add(_("View All Transactions"));
    itemChoiceViewTransStrings.Add(_("View Today"));
    itemChoiceViewTransStrings.Add(_("View Current Month"));
    itemChoiceViewTransStrings.Add(_("View Last 30 days"));
    itemChoiceViewTransStrings.Add(_("View Last 90 days"));
    itemChoiceViewTransStrings.Add(_("View Last Month"));
    itemChoiceViewTransStrings.Add(_("View Last 3 Months"));

    choiceTransVisible_ = new wxChoice(viewsPanel,
        ID_DIALOG_OPTIONS_VIEW_TRANS, wxDefaultPosition, wxSize(220,-1), itemChoiceViewTransStrings);
    dateFormatSettingStaticBoxSizerGrid2->Add(choiceTransVisible_, 1, wxGROW|wxALL, 5);

    wxString vTrans = mmDBWrapper::getINISettingValue(inidb_, ("VIEWTRANSACTIONS"), VIEW_TRANS_ALL_STR);
    wxArrayString itemChoiceViewTransactionsString = viewTransactionsStrings(false, vTrans, row_id_);
    choiceTransVisible_->SetSelection(row_id_);
    choiceTransVisible_->SetToolTip(_("Specify which transactions are visible by default"));

    wxStaticBox* fontSizeOptionStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Font Size Options"));
    fontSizeOptionStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* fontSizeOptionStaticBoxSizer = new wxStaticBoxSizer(fontSizeOptionStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(fontSizeOptionStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxStaticText* reportFontSizeStaticText = new wxStaticText(viewsPanel,
        wxID_STATIC, _("Report Font Size"));
    fontSizeOptionStaticBoxSizer->Add(reportFontSizeStaticText, flags);

    wxArrayString itemChoiceHTMLFontSize;

    itemChoiceHTMLFontSize.Add("XSmall");
    itemChoiceHTMLFontSize.Add("Small");
    itemChoiceHTMLFontSize.Add("Normal");
    itemChoiceHTMLFontSize.Add("Large");
    itemChoiceHTMLFontSize.Add("XLarge");
    itemChoiceHTMLFontSize.Add("XXLarge");
    itemChoiceHTMLFontSize.Add("Huge");

    choiceFontSize_ = new wxChoice(viewsPanel, ID_DIALOG_OPTIONS_FONT_SIZE,
        wxDefaultPosition, wxSize(85, -1), itemChoiceHTMLFontSize);

    choiceFontSize_->SetSelection((int)config->ReadLong("HTMLFONTSIZE", 3) -1);

    choiceFontSize_->SetToolTip(_("Specify which font size is used on the report tables"));
    fontSizeOptionStaticBoxSizer->Add(choiceFontSize_, 1, wxGROW|wxALL, 5);

    // Navigation Tree Expansion Options
    wxStaticBox* navTreeOptionsStaticBox = new wxStaticBox(viewsPanel,
        wxID_ANY, _("Navigation Tree Expansion Options"));
    navTreeOptionsStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* navTreeOptionsStaticBoxSizer = new wxStaticBoxSizer(navTreeOptionsStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(navTreeOptionsStaticBoxSizer, 0, wxGROW|wxALL, 5);

    // Expand Bank Tree
    wxCheckBox* expandBankCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_BANK_TREE,
        _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandBankCheckBox->SetValue(GetIniDatabaseCheckboxValue(("EXPAND_BANK_TREE"),true));
    expandBankCheckBox->SetToolTip(_("Expand Bank Accounts in Trew View when tree is refreshed"));
    navTreeOptionsStaticBoxSizer->Add(expandBankCheckBox, flags);

    // Expand Term Tree
    wxCheckBox* expandTermCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_TERM_TREE,
        _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandTermCheckBox->SetValue(GetIniDatabaseCheckboxValue(("EXPAND_TERM_TREE"),false));
    expandTermCheckBox->SetToolTip(_("Expand Term Accounts in Trew View when tree is refreshed"));
    navTreeOptionsStaticBoxSizer->Add(expandTermCheckBox, flags);

    // Home Page Expansion Options
    wxStaticBox* homePageStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Home Page Expansion Options"));
    homePageStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* homePageStaticBoxSizer = new wxStaticBoxSizer(homePageStaticBox, wxVERTICAL);
    viewsPanelSizer->Add(homePageStaticBoxSizer, 0, wxGROW|wxALL, 5);

    // Expand Bank Home
    wxCheckBox* expandBankHomeCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_BANK_HOME,
        _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandBankHomeCheckBox->SetValue(GetIniDatabaseCheckboxValue(("EXPAND_BANK_HOME"),true));
    expandBankHomeCheckBox->SetToolTip(_("Expand Bank Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(expandBankHomeCheckBox, flags);

    // Expand Term Home
    wxCheckBox* itemCheckBoxExpandTermHome = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_TERM_HOME,
        _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandTermHome->SetValue(GetIniDatabaseCheckboxValue(("EXPAND_TERM_HOME"),false));
    itemCheckBoxExpandTermHome->SetToolTip(_("Expand Term Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(itemCheckBoxExpandTermHome, flags);

    // Expand Stock Home
    wxCheckBox* itemCheckBoxExpandStockHome = new wxCheckBox(viewsPanel,
        ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME,
        _("Stock Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandStockHome->SetValue(GetIniDatabaseCheckboxValue(("ENABLESTOCKS"),true));
    itemCheckBoxExpandStockHome->SetToolTip(_("Expand Stock Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(itemCheckBoxExpandStockHome, flags);


    cbBudgetFinancialYears_ = new wxCheckBox(viewsPanel, wxID_ANY,
        _("View Budgets as Financial Years"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetFinancialYears_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_FINANCIAL_YEARS, false));
    viewsPanelSizer->Add(cbBudgetFinancialYears_, flags);

    cbBudgetIncludeTransfers_ = new wxCheckBox(viewsPanel, wxID_ANY,
        _("View Budgets with 'transfer' transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetIncludeTransfers_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_INCLUDE_TRANSFERS, false));
    viewsPanelSizer->Add(cbBudgetIncludeTransfers_, flags);

    cbBudgetSetupWithoutSummary_ = new wxCheckBox(viewsPanel, wxID_ANY,
        _("View Budgets Setup Without Budget Summaries"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetSetupWithoutSummary_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, false));
    viewsPanelSizer->Add(cbBudgetSetupWithoutSummary_, flags);

    cbBudgetSummaryWithoutCateg_ = new wxCheckBox(viewsPanel, wxID_ANY,
        _("View Budget Summary Report without Categories"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetSummaryWithoutCateg_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, true));
    viewsPanelSizer->Add(cbBudgetSummaryWithoutCateg_, flags);

    cbIgnoreFutureTransactions_ = new wxCheckBox(viewsPanel, wxID_ANY,
        _("View Reports without Future Transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbIgnoreFutureTransactions_->SetValue(GetIniDatabaseCheckboxValue(INIDB_IGNORE_FUTURE_TRANSACTIONS, false));
    viewsPanelSizer->Add(cbIgnoreFutureTransactions_, flags);

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

    wxFlexGridSizer* colourPanelSizerGrid = new wxFlexGridSizer(0, 2, 10, 10);
    colourSettingStaticBoxSizer->Add(colourPanelSizerGrid);

    wxStaticText* navTreeStaticText = new wxStaticText(colourPanel, wxID_STATIC, _("Nav Tree"));
    wxButton* navTreeButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE,
        _("Nav Tree"), wxDefaultPosition, wxSize(150,-1), 0);
    navTreeButton->SetToolTip(_("Specify the color for the nav tree"));
    navTreeButton->SetBackgroundColour(mmColors::navTreeBkColor);
    colourPanelSizerGrid->Add(navTreeStaticText, flags);
    colourPanelSizerGrid->Add(navTreeButton, flags);

    wxStaticText* listBackgroundStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Background"));
    wxButton* listBackgroundButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK,
        _("List Background"), wxDefaultPosition, navTreeButton->GetSize(), 0 );
    listBackgroundButton->SetToolTip(_("Specify the color for the list background"));
    listBackgroundButton->SetBackgroundColour(mmColors::listBackColor);
    colourPanelSizerGrid->Add(listBackgroundStaticText, flags);
    colourPanelSizerGrid->Add(listBackgroundButton, flags);

    wxStaticText* listRowZeroStaticText = new wxStaticText(colourPanel, wxID_STATIC, _("List Row 0"));
    wxButton* listRowZeroButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0,
        _("List Row 0"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listRowZeroButton->SetToolTip(_("Specify the color for the list row 0"));
    listRowZeroButton->SetBackgroundColour(mmColors::listAlternativeColor0);
    colourPanelSizerGrid->Add(listRowZeroStaticText, flags);
    colourPanelSizerGrid->Add(listRowZeroButton, flags);

    wxStaticText* listRowOneStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Row 1"));
    wxButton* listRowOneButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1,
        _("List Row 1"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listRowOneButton->SetToolTip(_("Specify the color for the list row 1"));
    listRowOneButton->SetBackgroundColour(mmColors::listAlternativeColor1);
    colourPanelSizerGrid->Add(listRowOneStaticText, flags);
    colourPanelSizerGrid->Add(listRowOneButton, flags);

    wxStaticText* listBorderSaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Border"));
    wxButton* listBorderButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER,
        _("List Border"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listBorderButton->SetToolTip(_("Specify the color for the list Border"));
    listBorderButton->SetBackgroundColour(mmColors::listBorderColor);
    colourPanelSizerGrid->Add(listBorderSaticText, flags);
    colourPanelSizerGrid->Add(listBorderButton, flags);

    wxStaticText* listDetailsStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("List Details"));
    wxButton* listDetailsButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS,
        _("List Details"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listDetailsButton->SetToolTip(_("Specify the color for the list details"));
    listDetailsButton->SetBackgroundColour(mmColors::listDetailsPanelColor);
    colourPanelSizerGrid->Add(listDetailsStaticText, flags);
    colourPanelSizerGrid->Add(listDetailsButton, flags);

    wxStaticText* futureTransStaticText = new wxStaticText( colourPanel, wxID_STATIC, _("Future Transactions"));
    wxButton* futureTransButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES,
        _("Future Transactions"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    futureTransButton->SetToolTip(_("Specify the color for future transactions"));
    futureTransButton->SetBackgroundColour(mmColors::listFutureDateColor);
    colourPanelSizerGrid->Add(futureTransStaticText, flags);
    colourPanelSizerGrid->Add(futureTransButton, flags);

    wxButton* restoreDefaultButton = new wxButton(colourPanel,
        ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT, _("Restore Defaults"));
    restoreDefaultButton->SetToolTip(_("Restore Default Colors"));
    colourPanelSizer->Add(restoreDefaultButton, 0);

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

    wxFlexGridSizer* newTransflexGridSizer = new wxFlexGridSizer(0,2,5,5);
    transSettingsStaticBoxSizer->Add(newTransflexGridSizer);

    //  Default Date
    wxStaticText* dateStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Date:"), wxDefaultPosition, wxDefaultSize);

    wxArrayString defaultValues;
    defaultValues.Add(_("None"));
    defaultValues.Add(_("Last Used"));

    wxChoice* defaultDateChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE,
        wxDefaultPosition, wxSize(140, -1), defaultValues);
    defaultDateChoice->SetSelection(mmIniOptions::instance().transDateDefault_);
    newTransflexGridSizer->Add(dateStaticText,flags);
    newTransflexGridSizer->Add(defaultDateChoice, flags);

    //  Default Payee
    wxStaticText* payeeStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Payee:"), wxDefaultPosition, wxDefaultSize);

    wxChoice* defaultPayeeChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE,
        wxDefaultPosition, defaultDateChoice->GetSize(), defaultValues);
    defaultPayeeChoice->SetSelection(mmIniOptions::instance().transPayeeSelectionNone_);
    newTransflexGridSizer->Add(payeeStaticText, flags);
    newTransflexGridSizer->Add(defaultPayeeChoice, flags);

    //  Default Category
    wxStaticText* categoryStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Category:"), wxDefaultPosition, wxDefaultSize);

    defaultValues[1]=(_("Last used for payee"));

    wxChoice* defaultCategoryChoice = new wxChoice(othersPanel,
        ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY,
        wxDefaultPosition, defaultPayeeChoice->GetSize(), defaultValues);
    defaultCategoryChoice->SetSelection(mmIniOptions::instance().transCategorySelectionNone_);
    newTransflexGridSizer->Add(categoryStaticText, flags);
    newTransflexGridSizer->Add(defaultCategoryChoice, flags);

    //  Default Status
    wxStaticText* statusStaticText = new wxStaticText(othersPanel, wxID_STATIC,
        _("Default Status:"), wxDefaultPosition, defaultPayeeChoice->GetSize());

    wxChoice* default_status = new wxChoice(othersPanel,
        ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS,
        wxDefaultPosition, defaultDateChoice->GetSize());
    wxString transaction_status[] = 
    {
        wxTRANSLATE("None"),
        wxTRANSLATE("Reconciled"),
        wxTRANSLATE("Void"),
        wxTRANSLATE("Follow up"),
        wxTRANSLATE("Duplicate")
    };
    for(size_t i = 0; i < sizeof(transaction_status)/sizeof(wxString); ++i)
        default_status->Append(wxGetTranslation(transaction_status[i]),
        new wxStringClientData(transaction_status[i]));

    default_status->SetSelection(mmIniOptions::instance().transStatusReconciled_);

    newTransflexGridSizer->Add(statusStaticText, flags);
    newTransflexGridSizer->Add(default_status, flags);
    newTransflexGridSizer->AddSpacer(10);
    newTransflexGridSizer->AddSpacer(10);

    //----------------------------------------------
    //a bit more space visual appearance
    othersPanelSizer->AddSpacer(10);

    wxBoxSizer* itemBoxSizerStockURL = new wxBoxSizer(wxVERTICAL);
    othersPanelSizer->Add(itemBoxSizerStockURL, 0, wxGROW);

    wxStaticText* itemStaticTextURL = new wxStaticText(othersPanel, wxID_STATIC,
        _("Stock Quote Web Page"), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticTextURL->SetFont(staticBoxFontSetting);
    itemBoxSizerStockURL->Add(itemStaticTextURL, 0, wxALIGN_LEFT|wxALL, 5);

    wxTextCtrl* itemTextCtrURL = new wxTextCtrl(othersPanel, ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL,
        config->Read("STOCKURL", mmex::DEFSTOCKURL));
    itemBoxSizerStockURL->Add(itemTextCtrURL, 1, wxGROW|wxALIGN_LEFT|wxALL, 5);
    itemTextCtrURL->SetToolTip(_("Clear the field to Reset the value to system default."));

    //a bit more space visual appearance
    othersPanelSizer->AddSpacer(15);

    cbUseOrgDateCopyPaste_ = new wxCheckBox(othersPanel, wxID_ANY,
        _("Use Original Date when Pasting Transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbUseOrgDateCopyPaste_->SetValue(GetIniDatabaseCheckboxValue(INIDB_USE_ORG_DATE_COPYPASTE, false));
    cbUseOrgDateCopyPaste_->SetToolTip(_("Select whether to use the original transaction date or current date when copying/pasting transactions"));
    othersPanelSizer->Add(cbUseOrgDateCopyPaste_, flags);

    cbUseSound_ = new wxCheckBox(othersPanel, wxID_ANY, _("Use Transaction Sound"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbUseSound_->SetValue(config->ReadBool(INIDB_USE_TRANSACTION_SOUND, true));
    cbUseSound_->SetToolTip(_("Select whether to use sounds when entering transactions"));
    othersPanelSizer->Add(cbUseSound_, flags);

    cbEnableCurrencyUpd_ = new wxCheckBox(othersPanel, wxID_ANY, _("Enable online currency update \n(Get data from yahoo.com)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbEnableCurrencyUpd_->SetValue(GetIniDatabaseCheckboxValue(INIDB_UPDATE_CURRENCY_RATE, false));
    cbEnableCurrencyUpd_->SetToolTip(_("Enable or disable get data from yahoo.com to update currency rate"));
    othersPanelSizer->Add(cbEnableCurrencyUpd_, flags);

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

    wxStaticBox* importExportStaticBox2 = new wxStaticBox(importExportPanel, wxID_ANY, _("CSV Delimiter"));
    importExportStaticBox2->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* importExportStaticBoxSizer2 = new wxStaticBoxSizer(importExportStaticBox2, wxVERTICAL);

    delimit_ = mmDBWrapper::getInfoSettingValue(db_, ("DELIMITER"), mmex::DEFDELIMTER);

    // CSV Delimiter
    wxString choices[] = { _("Comma"), _("Semicolon"), _("TAB"), _("User Defined")};
    int num = sizeof(choices) / sizeof(wxString);
    m_radio_box_ = new wxRadioBox(importExportPanel, ID_RADIO_BOX, (""),
        wxDefaultPosition, wxDefaultSize, num, choices, 4, wxRA_SPECIFY_ROWS);

    textDelimiter_ = new wxTextCtrl(importExportPanel,
        ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER, delimit_, wxDefaultPosition, wxDefaultSize);
    textDelimiter_->SetToolTip(_("Specify the delimiter to use when importing/exporting CSV files"));
    textDelimiter_->SetMaxLength(1);
    textDelimiter_->Disable();

    if (delimit_ == (","))
        m_radio_box_->SetSelection(0);
    else if (delimit_ == (";"))
        m_radio_box_->SetSelection(1);
    else if (delimit_ == ("\t"))
        m_radio_box_->SetSelection(2);
    else {
        m_radio_box_->SetSelection(3);
        textDelimiter_->Enable();
     }

    importExportStaticBoxSizer->AddSpacer(10);
    importExportStaticBoxSizer->Add(importExportStaticBoxSizer2);
    importExportStaticBoxSizer->AddSpacer(10);
    importExportStaticBoxSizer2->Add(m_radio_box_);
    importExportStaticBoxSizer2->Add(textDelimiter_, 0, wxTOP, 10);

   /**********************************************************************************************
    Setting up the notebook with the 5 pages
    **********************************************************************************************/
    newBook->SetImageList(m_imageList);

    newBook->InsertPage(0, generalPanel, _("General"), true, 2);
    newBook->InsertPage(1, viewsPanel, _("View Options"), false, 0);
    newBook->InsertPage(2, colourPanel, _("Colors"), false, 1);
    newBook->InsertPage(3, importExportPanel, _("Import/Export"), false, 4);
    newBook->InsertPage(4, othersPanel, _("Others"), false, 3);

    mainDialogPanelSizer->Add(newBook, 0, wxGROW|wxALL, 5);
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
    wxButton *btn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
    wxASSERT(btn);

    wxString lang = mmSelectLanguage(this, true, false);
    if (!lang.empty()) {

        // Advisable to restart GUI when user acknowledges the change.
        restartRequired_ = true;

        btn->SetLabel(lang.Capitalize());
    }
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
    dateFormat_ = DisplayDate2FormatDate(choiceDateFormat_->GetStringSelection());

    wxStaticText* restart_required = (wxStaticText*)FindWindow(ID_DIALOG_OPTIONS_RESTART_REQUIRED);
    restart_required->SetLabel(_("Requires MMEX Restart"));

    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE);
    st->SetLabel(wxDateTime::Now().Format(dateFormat_));
    restartRequired_ = true;
    changesApplied_ = true;
}

void mmOptionsDialog::OnColorChanged(wxCommandEvent& event)
{
    int id = event.GetId();

    wxString label;
    wxColour col;
    wxColourData data;
    data.SetChooseFull(true);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxButton* bn = (wxButton*)FindWindow(id);

        switch(id)
        {
            case ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE :
            {
                data.SetColour(navTreeBkColor_);
                col = dialog.GetColourData().GetColour();
                navTreeBkColor_ = col;
                break;
            }
            case ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0 :
            {
                data.SetColour(listAlternativeColor0_);
                col = dialog.GetColourData().GetColour();
                listAlternativeColor0_ = col;
                break;
            }
            case ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1 :
            {
                data.SetColour(listAlternativeColor1_);
                col = dialog.GetColourData().GetColour();
                listAlternativeColor1_ = col;
                break;
            }
            case ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK :
            {
                data.SetColour(listBackColor_);
                col = dialog.GetColourData().GetColour();
                listBackColor_ = col;
                break;
            }
            case ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER :
            {
                data.SetColour(listBorderColor_);
                col = dialog.GetColourData().GetColour();
                listBorderColor_ = col;
                break;
            }
            case ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS :
            {
                data.SetColour(listDetailsPanelColor_);
                col = dialog.GetColourData().GetColour();
                listDetailsPanelColor_ = col;
                break;
            }
            case ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES :
            {
                data.SetColour(listFutureDateColor_);
                col = dialog.GetColourData().GetColour();
                listFutureDateColor_ = col;
                break;
            }
            default:
            wxASSERT(false);
        }
        bn->SetBackgroundColour(col);
        label = bn->GetLabel()+("*");
        label.Replace(("****"), ("***"));
        bn->SetLabel(label);
    }
}

void mmOptionsDialog::OnRestoreDefaultColors(wxCommandEvent& /*event*/)
{
    // set the new colours to the default colours
    navTreeBkColor_ = wxColour(("WHITE"));
    listAlternativeColor0_ = wxColour(225, 237, 251);
    listAlternativeColor1_ = wxColour(("WHITE"));
    listBackColor_ = wxColour(("WHITE"));
    listBorderColor_ = wxColour(("BLACK"));
    listDetailsPanelColor_ = wxColour(244, 247, 251);
    listFutureDateColor_ = wxColour(116, 134, 168);

    // Display the original colours to the user
    wxButton* bn1 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE);
    bn1->SetBackgroundColour(navTreeBkColor_);
    bn1->SetLabel(_("Nav Tree"));

    wxButton* bn2 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0);
    bn2->SetBackgroundColour(listAlternativeColor0_);
    bn2->SetLabel(_("List Row 0"));

    wxButton* bn3 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1);
    bn3->SetBackgroundColour(listAlternativeColor1_);
    bn3->SetLabel(_("List Row 1"));

    wxButton* bn4 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK);
    bn4->SetBackgroundColour(listBackColor_);
    bn4->SetLabel(_("List Background"));

    wxButton* bn5 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER);
    bn5->SetBackgroundColour(listBorderColor_);
    bn5->SetLabel(_("List Border"));

    wxButton* bn6 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS);
    bn6->SetBackgroundColour(listDetailsPanelColor_);
    bn6->SetLabel(_("List Details"));

    wxButton* bn7 = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES);
    bn7->SetBackgroundColour(listFutureDateColor_);
    bn7->SetLabel(_("Future Transactions"));
}

bool mmOptionsDialog::GetIniDatabaseCheckboxValue(wxString dbField, bool defaultState)
{
    wxConfigBase *config = wxConfigBase::Get();
    return config->ReadBool(dbField, defaultState);
}

void mmOptionsDialog::OnDelimiterSelected(wxCommandEvent& event)
{
    wxString ud_delimit = textDelimiter_->GetValue();

    switch(m_radio_box_->GetSelection())
    {
        case 0:
            delimit_ = (",");
            textDelimiter_->Disable();
            break;
        case 1:
            delimit_ = (";");
            textDelimiter_->Disable();
            break;
        case 2:
            delimit_ = ("\t");
            textDelimiter_->Disable();
            break;
        case 3:
            delimit_ = ud_delimit;
            textDelimiter_->Enable();
            break;
        default:
            break;
    }

    textDelimiter_->SetLabel(delimit_);
    event.Skip();
}

void mmOptionsDialog::SaveViewAccountOptions()
{
    int selection = choiceVisible_->GetSelection();
    int row_id_ = 0;
    wxArrayString viewAcct = viewAccountStrings(false, wxEmptyString, row_id_);
    mmDBWrapper::setINISettingValue(inidb_, ("VIEWACCOUNTS"), viewAcct[selection]);
}

void mmOptionsDialog::SaveViewTransactionOptions()
{
    int selection = choiceTransVisible_->GetSelection();
    int row_id_ = 0;
    wxArrayString ViewTransaction = viewTransactionsStrings(false, wxEmptyString, row_id_);
    mmDBWrapper::setINISettingValue(inidb_, ("VIEWTRANSACTIONS"), ViewTransaction[selection]);
}

void mmOptionsDialog::SaveFinancialYearStart()
{
    //Save Financial Year Start Day
    wxSpinCtrl* fysDay = (wxSpinCtrl*)FindWindow(ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY);
    wxString fysDayVal = wxString::Format(("%d"),fysDay->GetValue());
    mmOptions::instance().financialYearStartDayString_ = fysDayVal;
    mmDBWrapper::setInfoSettingValue(db_, ("FINANCIAL_YEAR_START_DAY"), fysDayVal);

    //Save Financial Year Start Month
    wxString fysMonthVal = wxString() << monthSelection_->GetSelection() + 1;
    mmOptions::instance().financialYearStartMonthString_ = fysMonthVal;
    mmDBWrapper::setInfoSettingValue(db_, ("FINANCIAL_YEAR_START_MONTH"), fysMonthVal);
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
    wxConfigBase *config = wxConfigBase::Get();
    wxTextCtrl* stun = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME);
    mmIniOptions::instance().userNameString_ = stun->GetValue();
    mmDBWrapper::setInfoSettingValue(db_, ("USERNAME"), mmIniOptions::instance().userNameString_);

    wxButton *languageButton = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
    config->Write(LANGUAGE_PARAMETER, languageButton->GetLabel().Lower());
    mmSelectLanguage(0, false);

    mmDBWrapper::setBaseCurrencySettings(db_, currencyId_);
    mmDBWrapper::setInfoSettingValue(db_, ("DATEFORMAT"), dateFormat_);
    SaveFinancialYearStart();

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP);
    config->Write("BACKUPDB", itemCheckBox->GetValue());

    wxCheckBox* itemCheckBoxUpdate = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE);
    config->Write("BACKUPDB_UPDATE", itemCheckBoxUpdate->GetValue() );
}

void mmOptionsDialog::SaveViewPanelSettings()
{
    SaveViewAccountOptions();
    SaveViewTransactionOptions();
    wxConfigBase *config = wxConfigBase::Get();

    mmIniOptions::instance().font_size_ = (long)choiceFontSize_->GetCurrentSelection() + 1;
    config->Write("HTMLFONTSIZE", mmIniOptions::instance().font_size_);

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_TREE);
    mmIniOptions::instance().expandBankTree_ = itemCheckBox->GetValue();
    config->Write("EXPAND_BANK_TREE", mmIniOptions::instance().expandBankTree_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_TREE);
    mmIniOptions::instance().expandTermTree_ = itemCheckBox->GetValue();
    config->Write("EXPAND_TERM_TREE", mmIniOptions::instance().expandTermTree_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_HOME);
    mmIniOptions::instance().expandBankHome_ = itemCheckBox->GetValue();
    config->Write("EXPAND_BANK_HOME", mmIniOptions::instance().expandBankHome_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_HOME);
    mmIniOptions::instance().expandTermHome_ = itemCheckBox->GetValue();
    config->Write("EXPAND_TERM_HOME", mmIniOptions::instance().expandTermHome_);

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME);
    mmIniOptions::instance().expandStocksHome_ = itemCheckBox->GetValue();
    config->Write("ENABLESTOCKS", mmIniOptions::instance().expandStocksHome_);

    mmIniOptions::instance().budgetFinancialYears_ = cbBudgetFinancialYears_->GetValue();
    config->Write(INIDB_BUDGET_FINANCIAL_YEARS, mmIniOptions::instance().budgetFinancialYears_);

    mmIniOptions::instance().budgetIncludeTransfers_ = cbBudgetIncludeTransfers_->GetValue();
    config->Write(INIDB_BUDGET_INCLUDE_TRANSFERS, mmIniOptions::instance().budgetIncludeTransfers_);

    mmIniOptions::instance().budgetSetupWithoutSummaries_ = cbBudgetSetupWithoutSummary_->GetValue();
    config->Write(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, mmIniOptions::instance().budgetSetupWithoutSummaries_);

    mmIniOptions::instance().budgetSummaryWithoutCategories_ = cbBudgetSummaryWithoutCateg_->GetValue();
    config->Write(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, mmIniOptions::instance().budgetSummaryWithoutCategories_);

    mmIniOptions::instance().ignoreFutureTransactions_ = cbIgnoreFutureTransactions_->GetValue();
    config->Write(INIDB_IGNORE_FUTURE_TRANSACTIONS, mmIniOptions::instance().ignoreFutureTransactions_);
}

void mmOptionsDialog::SaveColourPanelSettings()
{
    wxConfigBase *config = wxConfigBase::Get();
    mmColors::navTreeBkColor = navTreeBkColor_;
    mmColors::listAlternativeColor0 = listAlternativeColor0_;
    mmColors::listAlternativeColor1 = listAlternativeColor1_;
    mmColors::listBackColor = listBackColor_;
    mmColors::listBorderColor = listBorderColor_;
    mmColors::listDetailsPanelColor = listDetailsPanelColor_;
    mmColors::listFutureDateColor = listFutureDateColor_;

    config->Write(("LISTALT0"), (navTreeBkColor_).GetAsString(wxC2S_CSS_SYNTAX));
    config->Write(("LISTALT1"), (listAlternativeColor0_).GetAsString(wxC2S_CSS_SYNTAX));
    config->Write(("LISTBACK"), (listAlternativeColor1_).GetAsString(wxC2S_CSS_SYNTAX));
    config->Write(("NAVTREE"), (listBackColor_).GetAsString(wxC2S_CSS_SYNTAX));
    config->Write(("LISTBORDER"), (listBorderColor_).GetAsString(wxC2S_CSS_SYNTAX));
    config->Write(("LISTDETAILSPANEL"), (listDetailsPanelColor_).GetAsString(wxC2S_CSS_SYNTAX));
    config->Write(("LISTFUTUREDATES"), (listFutureDateColor_).GetAsString(wxC2S_CSS_SYNTAX));
}

void mmOptionsDialog::SaveOthersPanelSettings()
{
    wxConfigBase *config = wxConfigBase::Get();
    wxChoice* itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE);
    mmIniOptions::instance().transPayeeSelectionNone_ = itemChoice->GetSelection();
    config->Write("TRANSACTION_PAYEE_NONE",
        wxString::Format(("%d"), (int)mmIniOptions::instance().transPayeeSelectionNone_));

    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY);
    mmIniOptions::instance().transCategorySelectionNone_ = itemChoice->GetSelection();
    config->Write("TRANSACTION_CATEGORY_NONE",
        wxString::Format(("%d"), (int)mmIniOptions::instance().transCategorySelectionNone_));

    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS);
    mmIniOptions::instance().transStatusReconciled_ = itemChoice->GetSelection();
    config->Write("TRANSACTION_STATUS_RECONCILED",
        wxString::Format(("%d"), (int)mmIniOptions::instance().transStatusReconciled_));

    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE);
    mmIniOptions::instance().transDateDefault_ = itemChoice->GetSelection();
    config->Write("TRANSACTION_DATE_DEFAULT",
        wxString::Format(("%d"), (int)mmIniOptions::instance().transDateDefault_));
    
    wxTextCtrl* url = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL);
    wxString stockURL = url->GetValue();
    config->Write("STOCKURL", stockURL);

    config->Write(INIDB_USE_ORG_DATE_COPYPASTE, cbUseOrgDateCopyPaste_->GetValue());
    config->Write(INIDB_UPDATE_CURRENCY_RATE, cbEnableCurrencyUpd_->GetValue());

    config->Write(INIDB_USE_TRANSACTION_SOUND, cbUseSound_->GetValue());
}

void mmOptionsDialog::SaveImportExportPanelSettings()
{
    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER);
    wxString delim = st->GetValue();
    if (!delim.IsEmpty()) mmDBWrapper::setInfoSettingValue(db_, ("DELIMITER"), delim);
}

bool mmOptionsDialog::GetUpdateCurrencyRateSetting()
{
    return cbEnableCurrencyUpd_->GetValue();
}

void mmOptionsDialog::OnOk(wxCommandEvent& /*event*/)
{
    changesApplied_ = true;
    EndModal(wxID_OK);
}

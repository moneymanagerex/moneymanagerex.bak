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
#include <wx/statline.h>
#include <limits>
#include "constants.h"

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
    EVT_BUTTON(wxID_OK, mmOptionsDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, mmOptionsDialog::OnCurrency)
    EVT_BUTTON(wxID_APPLY, mmOptionsDialog::OnDateFormatChanged)
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

mmOptionsDialog::mmOptionsDialog( )
{
}

mmOptionsDialog::~mmOptionsDialog( )
{
    delete m_imageList;
}

mmOptionsDialog::mmOptionsDialog(
    mmCoreDB* core,
    wxWindow* parent, wxWindowID id,
    const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
: core_(core)
, db_(core->db_.get())
, restartRequired_(false)
, changesApplied_(false)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmOptionsDialog::Create(
    wxWindow* parent, wxWindowID id,
    const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
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

    currencyId_ = core_->currencyList_.getBaseCurrencySettings(core_->dbInfoSettings_.get());
    dateFormat_ = core_->dbInfoSettings_->GetStringSetting(wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);

    CreateControls();
    Centre();
    Fit();

    return TRUE;
}

wxArrayString mmOptionsDialog::viewAccountStrings(bool translated, wxString input_string, int& row_id_)
{
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
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Expand();

    wxSize imageSize(48, 48);
    m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
    m_imageList->Add(wxBitmap(view_xpm));
    m_imageList->Add(wxBitmap(preferences_color_xpm));
    m_imageList->Add(wxBitmap(main_setup_xpm));
    m_imageList->Add(wxBitmap(preferences_other_xpm));
    m_imageList->Add(wxBitmap(export_import_xpm));

    wxBoxSizer* mainDialogSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainDialogSizer);

    wxPanel* mainDialogPanel = new wxPanel(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    mainDialogSizer->Add(mainDialogPanel, 1, wxGROW|wxALL, 5);

    wxBoxSizer* mainDialogPanelSizer = new wxBoxSizer(wxVERTICAL);
    mainDialogPanel->SetSizer(mainDialogPanelSizer);

    wxListbook* newBook = new wxListbook(mainDialogPanel, ID_DIALOG_OPTIONS_LISTBOOK, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);

    /*********************************************************************************************
     General Panel
    **********************************************************************************************/
    wxPanel* generalPanel = new wxPanel(newBook, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    SetMinSize(wxSize(500,600));
    wxBoxSizer* generalPanelSizer = new wxBoxSizer(wxVERTICAL);
    generalPanel->SetSizer(generalPanelSizer);

    // Display Header Settings
    wxStaticBox* headerStaticBox = new wxStaticBox(generalPanel, wxID_STATIC, _("Display Heading"));
    // Define the staticBox font and set it as wxFONTWEIGHT_BOLD
    wxFont staticBoxFontSetting = headerStaticBox->GetFont();
    staticBoxFontSetting.SetWeight(wxFONTWEIGHT_BOLD);
    headerStaticBox->SetFont(staticBoxFontSetting);

    wxStaticBoxSizer* headerStaticBoxSizer = new wxStaticBoxSizer(headerStaticBox, wxHORIZONTAL);

    headerStaticBoxSizer->Add(new wxStaticText(generalPanel, wxID_STATIC,
        _("User Name")), flags);

    wxString userName = core_->dbInfoSettings_->GetStringSetting(wxT("USERNAME"), wxT(""));
    wxTextCtrl* userNameTextCtr = new wxTextCtrl(generalPanel, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME,
        userName, wxDefaultPosition, wxSize(200, -1));
    userNameTextCtr->SetToolTip(_("The User Name is used as a title for the database."));
    headerStaticBoxSizer->Add(userNameTextCtr, 1, wxEXPAND|wxALL, 5);
    generalPanelSizer->Add(headerStaticBoxSizer, flagsExpand);

    // Language Settings
    wxStaticBox* languageStaticBox = new wxStaticBox(generalPanel, wxID_STATIC, _("Language"));
    languageStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* languageStaticBoxSizer = new wxStaticBoxSizer(languageStaticBox, wxHORIZONTAL);
    generalPanelSizer->Add(languageStaticBoxSizer, flagsExpand);

    currentLanguage_ = core_->iniSettings_->GetStringSetting(LANGUAGE_PARAMETER, wxT("english"));
    wxButton* languageButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_LANGUAGE,
        currentLanguage_.Left(1).Upper() + currentLanguage_.SubString(1,currentLanguage_.Len()),
        wxDefaultPosition, wxSize(150, -1), 0);
    languageButton->SetToolTip(_("Specify the language to use"));
    languageStaticBoxSizer->Add(languageButton, flags);

    // Currency Settings
    wxStaticBox* currencyStaticBox = new wxStaticBox(generalPanel, wxID_STATIC, _("Currency"));
    currencyStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* currencyStaticBoxSizer = new wxStaticBoxSizer(currencyStaticBox, wxHORIZONTAL);
    currencyStaticBox->SetFont(staticBoxFontSetting);
    generalPanelSizer->Add(currencyStaticBoxSizer, flagsExpand);

    currencyStaticBoxSizer->Add(new wxStaticText(generalPanel, wxID_STATIC, _("Base Currency")), flags);

    wxString currName = _("Set Currency");
    if (currencyId_ != -1)
        currName = core_->currencyList_.getCurrencyName(currencyId_);
    wxButton* baseCurrencyButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_CURRENCY,
        currName, wxDefaultPosition, wxDefaultSize);
    baseCurrencyButton->SetToolTip(_("Sets the default currency for the database."));
    currencyStaticBoxSizer->Add(baseCurrencyButton, flags);

    // Date Format Settings
    wxStaticBox* dateFormatStaticBox = new wxStaticBox(generalPanel, wxID_STATIC, _("Date Format"));
    dateFormatStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* dateFormatStaticBoxSizer = new wxStaticBoxSizer(dateFormatStaticBox, wxVERTICAL);
    wxFlexGridSizer* flex_sizer = new wxFlexGridSizer(0,2,0,5);
    generalPanelSizer->Add(dateFormatStaticBoxSizer, flagsExpand);
    dateFormatStaticBoxSizer->Add(flex_sizer);

    choiceDateFormat_ = new wxComboBox(generalPanel, wxID_STATIC, wxT(""),
        wxDefaultPosition, wxDefaultSize, date_format());
    flex_sizer->Add(choiceDateFormat_, flags);
    choiceDateFormat_->SetToolTip(_("Specify the date format for display"));
    choiceDateFormat_->SetValue(FormatDate2DisplayDate(dateFormat_));
    //choiceDateFormat_->AutoComplete(date_format());

    wxButton* setFormatButton = new wxButton(generalPanel, wxID_APPLY, _("Set"));
    flex_sizer->Add(setFormatButton, flags);

    sampleDateText_ = new wxStaticText(generalPanel, wxID_STATIC,
        wxT("redefined elsewhere"));
    flex_sizer->Add(new wxStaticText(generalPanel, wxID_STATIC,
        _("New date format sample:")), flags);
    flex_sizer->Add(sampleDateText_, flags);
    sampleDateText_->SetLabel(wxDateTime::Now().Format(dateFormat_));

    // Financial Year Settings
    wxStaticBox* financialYearStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Financial Year"));
    financialYearStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* financialYearStaticBoxSizer = new wxStaticBoxSizer(financialYearStaticBox, wxVERTICAL);
    wxFlexGridSizer* financialYearStaticBoxSizerGrid = new wxFlexGridSizer(0,2,0,0);
    generalPanelSizer->Add(financialYearStaticBoxSizer, 0, wxGROW|wxALL, 5);
    financialYearStaticBoxSizer->Add(financialYearStaticBoxSizerGrid);

    financialYearStaticBoxSizerGrid->Add(new wxStaticText(generalPanel, wxID_STATIC, _("Start Day")), flags);
    int day = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_DAY"), 1);

    wxSpinCtrl *textFPSDay = new wxSpinCtrl(generalPanel, ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY,
        wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 31, day);
    textFPSDay->SetToolTip(_("Specify Day for start of financial year"));

    financialYearStaticBoxSizerGrid->Add(textFPSDay, flags);

    financialYearStaticBoxSizerGrid->Add(new wxStaticText(generalPanel, wxID_STATIC, _("Start Month")), flags);

    wxArrayString financialMonthsSelection;
    for(int i=0; i<12; i++) {
        financialMonthsSelection.Add(mmGetNiceShortMonthName(i));
    };
    monthSelection_ = new wxChoice(generalPanel, ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_MONTH,
        wxDefaultPosition, wxSize(100, -1), financialMonthsSelection);
    financialYearStaticBoxSizerGrid->Add(monthSelection_, flags);

    int monthItem = core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_MONTH"), 7);
    monthSelection_->SetSelection(monthItem - 1);
    monthSelection_->SetToolTip(_("Specify month for start of financial year"));

    /*********************************************************************************************
     Views Panel
    **********************************************************************************************/
    wxPanel* viewsPanel = new wxPanel(newBook, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* viewsPanelSizer = new wxBoxSizer(wxVERTICAL);
    viewsPanel->SetSizer(viewsPanelSizer);

    // Account View Options
    wxStaticBox* accountStaticBox = new wxStaticBox(viewsPanel, wxID_STATIC, _("View Options"));
    accountStaticBox->SetFont(staticBoxFontSetting);

    wxStaticBoxSizer* accountStaticBoxSizer = new wxStaticBoxSizer(accountStaticBox, wxVERTICAL);
    viewsPanelSizer->Add(accountStaticBoxSizer, flagsExpand);
    wxFlexGridSizer* view_sizer1 = new wxFlexGridSizer(0,2,0,5);
    accountStaticBoxSizer->Add(view_sizer1);

    view_sizer1->Add(new wxStaticText(viewsPanel, wxID_STATIC, _("Accounts Visible")), flags);

    int row_id_ = 0;
    wxArrayString itemChoiceViewAccountTranslatedStrings = viewAccountStrings(true, wxEmptyString, row_id_);

    choiceVisible_ = new wxChoice(viewsPanel, ID_DIALOG_OPTIONS_VIEW_ACCOUNTS,
        wxDefaultPosition, wxDefaultSize, itemChoiceViewAccountTranslatedStrings);
    view_sizer1->Add(choiceVisible_, flags);

    wxString vAccts = core_->iniSettings_->GetStringSetting(wxT("VIEWACCOUNTS"), VIEW_ACCOUNTS_ALL_STR);
    row_id_ = 0;
    wxArrayString itemChoiceViewAccountStrings = viewAccountStrings(false, vAccts, row_id_);
    choiceVisible_->SetSelection(row_id_);

    choiceVisible_->SetToolTip(_("Specify which accounts are visible"));

    view_sizer1->Add(new wxStaticText(viewsPanel, wxID_STATIC,
        _("Transactions Visible")), flags);

    wxArrayString view_strings;
    view_strings.Add(VIEW_TRANS_ALL_STR);
    view_strings.Add(VIEW_TRANS_TODAY_STR);
    view_strings.Add(VIEW_TRANS_CURRENT_MONTH_STR);
    view_strings.Add(VIEW_TRANS_LAST_30_DAYS_STR);
    view_strings.Add(VIEW_TRANS_LAST_90_DAYS_STR);
    view_strings.Add(VIEW_TRANS_LAST_MONTH_STR);
    view_strings.Add(VIEW_TRANS_LAST_3MONTHS_STR);

    choiceTransVisible_ = new wxChoice(viewsPanel, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize);
    for(size_t i = 0; i < view_strings.GetCount(); ++i)
        choiceTransVisible_->Append(wxGetTranslation(view_strings[i]),
        new wxStringClientData(view_strings[i]));

    view_sizer1->Add(choiceTransVisible_,flags);

    wxString vTrans = core_->iniSettings_->GetStringSetting(wxT("VIEWTRANSACTIONS"), VIEW_TRANS_ALL_STR);
    choiceTransVisible_->SetStringSelection(wxGetTranslation(vTrans));
    choiceTransVisible_->SetToolTip(_("Specify which transactions are visible by default"));

    view_sizer1->Add(new wxStaticText(viewsPanel, wxID_STATIC, _("Report Font Size")), flags);

    wxString itemChoiceFontSize[] = {
        wxTRANSLATE("XSmall"),
        wxTRANSLATE("Small"),
        wxTRANSLATE("Normal"),
        wxTRANSLATE("Large"),
        wxTRANSLATE("XLarge"),
        wxTRANSLATE("XXLarge"),
        wxTRANSLATE("Huge")};

    choiceFontSize_ = new wxChoice(viewsPanel, wxID_STATIC);

    for(size_t i = 0; i < sizeof(itemChoiceFontSize)/sizeof(wxString); ++i)
        choiceFontSize_->Append(wxGetTranslation(itemChoiceFontSize[i]));

    int vFontSize = -1 + core_->iniSettings_->GetIntSetting(wxT("HTMLFONTSIZE"), 3);
    choiceFontSize_->SetSelection(vFontSize);

    choiceFontSize_->SetToolTip(_("Specify which font size is used on the report tables"));
    view_sizer1->Add(choiceFontSize_, flags);

    // Navigation Tree Expansion Options
    wxStaticBox* navTreeOptionsStaticBox = new wxStaticBox(viewsPanel, wxID_ANY, _("Navigation Tree Expansion Options"));
    navTreeOptionsStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* navTreeOptionsStaticBoxSizer = new wxStaticBoxSizer(navTreeOptionsStaticBox, wxHORIZONTAL);
    viewsPanelSizer->Add(navTreeOptionsStaticBoxSizer, flagsExpand);

    // Expand Bank Tree
    wxCheckBox* expandBankCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_BANK_TREE,
        _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandBankCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_TREE"),true));
    expandBankCheckBox->SetToolTip(_("Expand Bank Accounts in Trew View when tree is refreshed"));
    navTreeOptionsStaticBoxSizer->Add(expandBankCheckBox, flags);

    // Expand Term Tree
    wxCheckBox* expandTermCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_TERM_TREE,
        _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandTermCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_TREE"),false));
    expandTermCheckBox->SetToolTip(_("Expand Term Accounts in Trew View when tree is refreshed"));
    navTreeOptionsStaticBoxSizer->Add(expandTermCheckBox, flags);

    // Home Page Expansion Options
    wxStaticBox* homePageStaticBox = new wxStaticBox(viewsPanel, wxID_STATIC, _("Home Page Expansion Options"));
    homePageStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* homePageStaticBoxSizer = new wxStaticBoxSizer(homePageStaticBox, wxVERTICAL);
    viewsPanelSizer->Add(homePageStaticBoxSizer, flagsExpand);

    // Expand Bank Home
    wxCheckBox* expandBankHomeCheckBox = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_BANK_HOME,
        _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    expandBankHomeCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_HOME"),true));
    expandBankHomeCheckBox->SetToolTip(_("Expand Bank Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(expandBankHomeCheckBox, flags);

    // Expand Term Home
    wxCheckBox* itemCheckBoxExpandTermHome = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_TERM_HOME,
        _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandTermHome->SetValue(GetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_HOME"),false));
    itemCheckBoxExpandTermHome->SetToolTip(_("Expand Term Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(itemCheckBoxExpandTermHome, flags);

    // Expand Stock Home
    wxCheckBox* itemCheckBoxExpandStockHome = new wxCheckBox(viewsPanel, ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME,
        _("Stock Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandStockHome->SetValue(GetIniDatabaseCheckboxValue(wxT("ENABLESTOCKS"),true));
    itemCheckBoxExpandStockHome->SetToolTip(_("Expand Stock Accounts on home page when page is refreshed"));
    homePageStaticBoxSizer->Add(itemCheckBoxExpandStockHome, flags);

    cbBudgetFinancialYears_ = new wxCheckBox(viewsPanel, wxID_STATIC, _("View Budgets as Financial Years"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetFinancialYears_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_FINANCIAL_YEARS, false));
    viewsPanelSizer->Add(cbBudgetFinancialYears_, flags);

    cbBudgetIncludeTransfers_ = new wxCheckBox(viewsPanel, wxID_STATIC, _("View Budgets with 'transfer' transactions"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetIncludeTransfers_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_INCLUDE_TRANSFERS, false));
    viewsPanelSizer->Add(cbBudgetIncludeTransfers_, flags);

    cbBudgetSetupWithoutSummary_ = new wxCheckBox(viewsPanel, wxID_STATIC, _("View Budgets Setup Without Budget Summaries"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetSetupWithoutSummary_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, false));
    viewsPanelSizer->Add(cbBudgetSetupWithoutSummary_, flags);

    cbBudgetSummaryWithoutCateg_ = new wxCheckBox(viewsPanel, wxID_STATIC, _("View Budget Summary Report without Categories"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbBudgetSummaryWithoutCateg_->SetValue(GetIniDatabaseCheckboxValue(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, true));
    viewsPanelSizer->Add(cbBudgetSummaryWithoutCateg_, flags);

    cbIgnoreFutureTransactions_ = new wxCheckBox(viewsPanel, wxID_STATIC, _("View Reports without Future Transactions"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbIgnoreFutureTransactions_->SetValue(GetIniDatabaseCheckboxValue(INIDB_IGNORE_FUTURE_TRANSACTIONS, false));
    viewsPanelSizer->Add(cbIgnoreFutureTransactions_, flags);

    /*********************************************************************************************
     Colours Panel
    **********************************************************************************************/
    wxPanel* colourPanel = new wxPanel(newBook, ID_BOOK_PANELCOLORS, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* colourPanelSizer = new wxBoxSizer(wxVERTICAL);
    colourPanel->SetSizer(colourPanelSizer);

    wxStaticBox* colourSettingStaticBox = new wxStaticBox(colourPanel, wxID_STATIC, _("Colour Settings"));
    colourSettingStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* colourSettingStaticBoxSizer = new wxStaticBoxSizer(colourSettingStaticBox, wxVERTICAL);
    colourPanelSizer->Add(colourSettingStaticBoxSizer, flagsExpand);

    wxFlexGridSizer* colourPanelSizerGrid = new wxFlexGridSizer(0, 2, 10, 10);
    colourSettingStaticBoxSizer->Add(colourPanelSizerGrid, flags);

    wxButton* navTreeButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE,
        _("Nav Tree"), wxDefaultPosition, wxSize(150,-1), 0);
    navTreeButton->SetToolTip(_("Specify the color for the nav tree"));
    navTreeButton->SetBackgroundColour(mmColors::navTreeBkColor);
    colourPanelSizerGrid->Add(new wxStaticText(colourPanel, wxID_STATIC, _("Nav Tree")), flags);
    colourPanelSizerGrid->Add(navTreeButton, flags);

    wxButton* listBackgroundButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK,
        _("List Background"), wxDefaultPosition, navTreeButton->GetSize(), 0 );
    listBackgroundButton->SetToolTip(_("Specify the color for the list background"));
    listBackgroundButton->SetBackgroundColour(mmColors::listBackColor);
    colourPanelSizerGrid->Add(new wxStaticText( colourPanel, wxID_STATIC, _("List Background")), flags);
    colourPanelSizerGrid->Add(listBackgroundButton, flags);

    wxButton* listRowZeroButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0,
        _("List Row 0"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listRowZeroButton->SetToolTip(_("Specify the color for the list row 0"));
    listRowZeroButton->SetBackgroundColour(mmColors::listAlternativeColor0);
    colourPanelSizerGrid->Add(new wxStaticText(colourPanel, wxID_STATIC, _("List Row 0")), flags);
    colourPanelSizerGrid->Add(listRowZeroButton, flags);

    wxButton* listRowOneButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1,
        _("List Row 1"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listRowOneButton->SetToolTip(_("Specify the color for the list row 1"));
    listRowOneButton->SetBackgroundColour(mmColors::listAlternativeColor1);
    colourPanelSizerGrid->Add(new wxStaticText( colourPanel, wxID_STATIC, _("List Row 1")), flags);
    colourPanelSizerGrid->Add(listRowOneButton, flags);

    wxButton* listBorderButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER,
        _("List Border"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listBorderButton->SetToolTip(_("Specify the color for the list Border"));
    listBorderButton->SetBackgroundColour(mmColors::listBorderColor);
    colourPanelSizerGrid->Add(new wxStaticText( colourPanel, wxID_STATIC, _("List Border")), flags);
    colourPanelSizerGrid->Add(listBorderButton, flags);

    wxButton* listDetailsButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS,
        _("List Details"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    listDetailsButton->SetToolTip(_("Specify the color for the list details"));
    listDetailsButton->SetBackgroundColour(mmColors::listDetailsPanelColor);
    colourPanelSizerGrid->Add(new wxStaticText( colourPanel, wxID_STATIC, _("List Details")), flags);
    colourPanelSizerGrid->Add(listDetailsButton, flags);

    wxButton* futureTransButton = new wxButton( colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES,
        _("Future Transactions"), wxDefaultPosition, navTreeButton->GetSize(), 0);
    futureTransButton->SetToolTip(_("Specify the color for future transactions"));
    futureTransButton->SetBackgroundColour(mmColors::listFutureDateColor);
    colourPanelSizerGrid->Add(new wxStaticText(colourPanel, wxID_STATIC,
        _("Future Transactions")), flags);
    colourPanelSizerGrid->Add(futureTransButton, flags);

    wxButton* restoreDefaultButton = new wxButton(colourPanel, ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT,
        _("Restore Defaults"));
    restoreDefaultButton->SetToolTip(_("Restore Default Colors"));
    colourPanelSizer->Add(restoreDefaultButton, flags);

    /*********************************************************************************************
     Others Panel
    **********************************************************************************************/
    wxPanel* othersPanel = new wxPanel(newBook, ID_BOOK_PANELMISC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* othersPanelSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* itemStaticTextURL = new wxStaticText(othersPanel, wxID_STATIC, _("Stock Quote Web Page"));
    itemStaticTextURL->SetFont(staticBoxFontSetting);
    othersPanelSizer->Add(itemStaticTextURL, flags);

    wxString stockURL = core_->dbInfoSettings_->GetStringSetting(wxT("STOCKURL"), mmex::DEFSTOCKURL);
    wxTextCtrl* itemTextCtrURL = new wxTextCtrl(othersPanel, ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL, stockURL);
    othersPanelSizer->Add(itemTextCtrURL, flagsExpand);
    itemTextCtrURL->SetToolTip(_("Clear the field to Reset the value to system default."));
    othersPanelSizer->AddSpacer(15);

    // New transaction dialog settings
    wxStaticBox* transSettingsStaticBox = new wxStaticBox(othersPanel, wxID_STATIC,
        _("New Transaction Dialog Settings"));
    transSettingsStaticBox->SetFont(staticBoxFontSetting);

    wxStaticBoxSizer* transSettingsStaticBoxSizer = new wxStaticBoxSizer(transSettingsStaticBox, wxVERTICAL);
    othersPanelSizer->Add(transSettingsStaticBoxSizer, flagsExpand);

    wxArrayString defaultValues_;
    defaultValues_.Add(_("None"));
    defaultValues_.Add(_("Last Used"));

    wxChoice* defaultDateChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE,
        wxDefaultPosition, wxSize(140, -1), defaultValues_);
    defaultDateChoice->SetSelection(mmIniOptions::instance().transDateDefault_);

    wxChoice* defaultPayeeChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE,
        wxDefaultPosition, wxSize(140, -1), defaultValues_);
    defaultPayeeChoice->SetSelection(mmIniOptions::instance().transPayeeSelectionNone_);

    defaultValues_[1]=(_("Last used for payee"));

    wxChoice* defaultCategoryChoice = new wxChoice(othersPanel, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY,
        wxDefaultPosition, defaultPayeeChoice->GetSize(), defaultValues_);
    defaultCategoryChoice->SetSelection(mmIniOptions::instance().transCategorySelectionNone_);

    wxChoice* default_status = new wxChoice(othersPanel,
        ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS,
        wxDefaultPosition, defaultDateChoice->GetSize());

    for(size_t i = 0; i < sizeof(TRANSACTION_STATUS)/sizeof(wxString); ++i)
        default_status->Append(wxGetTranslation(TRANSACTION_STATUS[i]),
        new wxStringClientData(TRANSACTION_STATUS[i]));

    default_status->SetSelection(mmIniOptions::instance().transStatusReconciled_);

    wxFlexGridSizer* newTransflexGridSizer = new wxFlexGridSizer(0,2,0,0);
    transSettingsStaticBoxSizer->Add(newTransflexGridSizer);
    newTransflexGridSizer->Add(new wxStaticText(othersPanel, wxID_STATIC, _("Default Date:")), flags);
    newTransflexGridSizer->Add(defaultDateChoice,    flags);
    newTransflexGridSizer->Add(new wxStaticText(othersPanel, wxID_STATIC, _("Default Payee:")), flags);
    newTransflexGridSizer->Add(defaultPayeeChoice,   flags);
    newTransflexGridSizer->Add(new wxStaticText(othersPanel, wxID_STATIC, _("Default Category:")), flags);
    newTransflexGridSizer->Add(defaultCategoryChoice,flags);
    newTransflexGridSizer->Add(new wxStaticText(othersPanel, wxID_STATIC, _("Default Status:")), flags);
    newTransflexGridSizer->Add(default_status,  flags);

    //----------------------------------------------
    //a bit more space visual appearance
    othersPanelSizer->AddSpacer(10);

    wxBoxSizer* itemBoxSizerStockURL = new wxBoxSizer(wxVERTICAL);
    othersPanelSizer->Add(itemBoxSizerStockURL);


    //a bit more space visual appearance
    othersPanelSizer->AddSpacer(15);

    cbUseOrgDateCopyPaste_ = new wxCheckBox(othersPanel, wxID_STATIC, _("Use Original Date when Pasting Transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbUseOrgDateCopyPaste_->SetValue(GetIniDatabaseCheckboxValue(INIDB_USE_ORG_DATE_COPYPASTE, false));
    cbUseOrgDateCopyPaste_->SetToolTip(_("Select whether to use the original transaction date or current date when copying/pasting transactions"));
    othersPanelSizer->Add(cbUseOrgDateCopyPaste_, flags);

    cbUseSound_ = new wxCheckBox(othersPanel, wxID_STATIC, _("Use Transaction Sound"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbUseSound_->SetValue(GetIniDatabaseCheckboxValue(INIDB_USE_TRANSACTION_SOUND,true));
    cbUseSound_->SetToolTip(_("Select whether to use sounds when entering transactions"));
    othersPanelSizer->Add(cbUseSound_, flags);

    cbEnableCurrencyUpd_ = new wxCheckBox(othersPanel, wxID_STATIC, _("Enable online currency update"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbEnableCurrencyUpd_->SetValue(GetIniDatabaseCheckboxValue(INIDB_UPDATE_CURRENCY_RATE, false));
    cbEnableCurrencyUpd_->SetToolTip(_("Enable or disable get data from Yahoo.com to update currency rate"));
    othersPanelSizer->Add(cbEnableCurrencyUpd_, flags);

    // Backup Settings
    wxStaticBox* backupStaticBox = new wxStaticBox(othersPanel, wxID_STATIC, _("Database Backup"));
    backupStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* backupStaticBoxSizer = new wxStaticBoxSizer(backupStaticBox, wxVERTICAL);
    othersPanelSizer->Add(backupStaticBoxSizer, flagsExpand);

    wxCheckBox* backupCheckBox = new wxCheckBox(othersPanel, ID_DIALOG_OPTIONS_CHK_BACKUP,
        _("Create a new backup when MMEX Start"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    backupCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("BACKUPDB"),false));
    backupCheckBox->SetToolTip(_("When MMEX Starts,\ncreates the backup database: dbFile_start_YYYY-MM-DD.ext."));
    backupStaticBoxSizer->Add(backupCheckBox, flags);

    wxCheckBox* backupUpdateCheckBox = new wxCheckBox(othersPanel, ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE,
        _("Backup database on exit."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    backupUpdateCheckBox->SetValue(GetIniDatabaseCheckboxValue(wxT("BACKUPDB_UPDATE"),false));
    backupUpdateCheckBox->SetToolTip(_("When MMEX shuts down and changes made to database,\ncreates or updates the backup database: dbFile_update_YYYY-MM-DD.ext."));
    backupStaticBoxSizer->Add(backupUpdateCheckBox, flags);

    int max =  core_->iniSettings_->GetIntSetting(wxT("MAX_BACKUP_FILES"), 4);
    scMax_files_ = new wxSpinCtrl(othersPanel, wxID_ANY,
        wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 999, max);
    scMax_files_->SetToolTip(_("Specify max number of backup files"));

    wxFlexGridSizer* flex_sizer2 = new wxFlexGridSizer(0,2,0,0);
    flex_sizer2->Add(new wxStaticText(othersPanel, wxID_STATIC, _("Max Files")), flags);
    flex_sizer2->Add(scMax_files_, flags);
    backupStaticBoxSizer->Add(flex_sizer2);

    othersPanel->SetSizer(othersPanelSizer);

    /*********************************************************************************************
     Import/Export Panel
    **********************************************************************************************/
    wxPanel* importExportPanel = new wxPanel(newBook, ID_BOOK_PANEL_EXP_IMP, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    wxBoxSizer* importExportPanelSizer = new wxBoxSizer(wxVERTICAL);
    importExportPanel->SetSizer(importExportPanelSizer);

    wxStaticBox* importExportStaticBox = new wxStaticBox(importExportPanel, wxID_ANY, _("Import/Export Settings"));
    importExportStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* importExportStaticBoxSizer = new wxStaticBoxSizer(importExportStaticBox, wxVERTICAL);

    importExportPanelSizer->Add(importExportStaticBoxSizer, flagsExpand);

    wxStaticText* csvDelimiterStaticText = new wxStaticText(importExportPanel, wxID_STATIC, _("CSV Delimiter"));
    importExportStaticBoxSizer->Add(csvDelimiterStaticText, flags);

    wxBoxSizer* radioButtonSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* userDefinedSizer = new wxBoxSizer(wxHORIZONTAL);
    importExportStaticBoxSizer->Add(radioButtonSizer);
    importExportStaticBoxSizer->Add(userDefinedSizer);
    importExportStaticBoxSizer->AddSpacer(5);

    wxString delimiter = core_->dbInfoSettings_->GetStringSetting(wxT("DELIMITER"), mmex::DEFDELIMTER);

    wxRadioButton* delimiterRadioButtonU4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_USER4, _("User Defined"));
    wxRadioButton* delimiterRadioButtonC4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_COMMA4, _("Comma"));
    if (delimiter == wxT(",")) delimiterRadioButtonC4 ->SetValue(true);

    wxRadioButton* delimiterRadioButtonS4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_SEMICOLON4, _("Semicolon"));
    if (delimiter == wxT(";")) delimiterRadioButtonS4 ->SetValue(true);

    wxRadioButton* delimiterRadioButtonT4 = new wxRadioButton(importExportPanel, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_TAB4, _("TAB"));
    if (delimiter == wxT("\t")) delimiterRadioButtonT4 ->SetValue(true);

    wxTextCtrl* textDelimiter4 = new wxTextCtrl( importExportPanel, ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4, delimiter);
    textDelimiter4->SetToolTip(_("Specify the delimiter to use when importing/exporting CSV files"));
    textDelimiter4->SetMaxLength(2);
    if (delimiter == wxT("\t") || delimiter == wxT(",") || delimiter == wxT(";"))
    {
        textDelimiter4->Enable(false);
    }
    else
    {
        delimiterRadioButtonU4->SetValue(true);
    }
    radioButtonSizer->Add(delimiterRadioButtonC4, flags);
    radioButtonSizer->Add(delimiterRadioButtonS4, flags);
    radioButtonSizer->Add(delimiterRadioButtonT4, flags);

    userDefinedSizer->Add(delimiterRadioButtonU4, flags);
    userDefinedSizer->Add(textDelimiter4, flags);

// temporarily hide from interface because the user defined CSV delimiter as this is not supported by the universal CSV dialog.
//    delimiterRadioButtonU4->Hide();
//    textDelimiter4->Hide();

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

    wxButton* itemButtonOK = new wxButton(buttonPanel, wxID_OK);
    wxButton* itemButtonCancel = new wxButton(buttonPanel, wxID_CANCEL);
    buttonPanelSizer->Add(itemButtonOK, 0, wxALIGN_RIGHT|wxRIGHT, 5);
    buttonPanelSizer->Add(itemButtonCancel, 0, wxALIGN_RIGHT|wxRIGHT, 5);
    itemButtonOK->SetFocus();
}

void mmOptionsDialog::OnLanguageChanged(wxCommandEvent& /*event*/)
{
    wxString lang = mmSelectLanguage(this, core_->iniSettings_, true, false);
    if (lang.empty()) return;

    // Advisable to restart GUI when user acknowledges the change.
    restartRequired_ = true;
    changesApplied_ = true;

    wxButton *btn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
    wxASSERT(btn);
    btn->SetLabel(lang.Left(1).Upper() + lang.SubString(1,lang.Len()));
}

void mmOptionsDialog::OnCurrency(wxCommandEvent& /*event*/)
{
    int currencyID = core_->currencyList_.getBaseCurrencySettings(core_->dbInfoSettings_.get());

    if (mmMainCurrencyDialog::Execute(core_, this, currencyID) && currencyID != -1)
    {
        wxString currName = core_->currencyList_.getCurrencyName(currencyID);
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
    mmOptions::instance().dateFormat_ = dateFormat_;

    sampleDateText_->SetLabel(wxDateTime::Now().Format(dateFormat_));
    core_->bTransactionList_.ChangeDateFormat();
}

void mmOptionsDialog::OnNavTreeColorChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(navTreeBkColor_);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxOK)
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
    if (dialog.ShowModal() == wxOK)
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
    if (dialog.ShowModal() == wxOK)
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
    if (dialog.ShowModal() == wxOK)
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
    if (dialog.ShowModal() == wxOK)
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
    if (dialog.ShowModal() == wxOK)
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
    if (dialog.ShowModal() == wxOK)
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

bool mmOptionsDialog::GetIniDatabaseCheckboxValue(wxString dbField, bool defaultState)
{
    bool result = core_->iniSettings_->GetBoolSetting(dbField, defaultState);

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
    core_->iniSettings_->SetStringSetting(wxT("VIEWACCOUNTS"), viewAcct[selection]);
}

void mmOptionsDialog::SaveViewTransactionOptions()
{
    wxString visible = VIEW_TRANS_ALL_STR;
    wxStringClientData* visible_obj = (wxStringClientData *)choiceTransVisible_->GetClientObject(choiceTransVisible_->GetSelection());
    if (visible_obj)
    {
        visible = visible_obj->GetData();
    }
    core_->iniSettings_->SetStringSetting(wxT("VIEWTRANSACTIONS"), visible);
}

void mmOptionsDialog::SaveFinancialYearStart()
{
    //Save Financial Year Start Day
    wxSpinCtrl* fysDay = (wxSpinCtrl*)FindWindow(ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY);
    wxString fysDayVal = wxString::Format(wxT("%d"),fysDay->GetValue());
    mmOptions::instance().financialYearStartDayString_ = fysDayVal;
    core_->dbInfoSettings_->SetStringSetting(wxT("FINANCIAL_YEAR_START_DAY"), fysDayVal);

    //Save Financial Year Start Month
    wxString fysMonthVal = wxString() << monthSelection_->GetSelection() + 1;
    mmOptions::instance().financialYearStartMonthString_ = fysMonthVal;
    core_->dbInfoSettings_->SetStringSetting(wxT("FINANCIAL_YEAR_START_MONTH"), fysMonthVal);
}

void mmOptionsDialog::SaveStocksUrl()
{
    wxTextCtrl* url = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL);
    wxString stockURL = url->GetValue();
    if (!stockURL.IsEmpty())
    {
        core_->dbInfoSettings_->SetStringSetting(wxT("STOCKURL"), stockURL);
    }
    else
    {
        // Clear database record: Allows value to reset to system default.
        db_->ExecuteUpdate("DELETE FROM INFOTABLE_V1 where INFONAME = \"STOCKURL\";");
    }
}

/// Saves the updated System Options to the appropriate databases.
void mmOptionsDialog::SaveNewSystemSettings()
{
    // initialize database saves -------------------------------------------------------------
    db_->Begin();

    // Save all the details for all the panels
    SaveGeneralPanelSettings();
    SaveViewPanelSettings();
    SaveColourPanelSettings();
    SaveOthersPanelSettings();
    SaveImportExportPanelSettings();

    // finalise database saves ---------------------------------------------------------------
    db_->Commit();
}

void mmOptionsDialog::SaveGeneralPanelSettings()
{
    wxTextCtrl* stun = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME);
    mmOptions::instance().userNameString_ = stun->GetValue();
    core_->dbInfoSettings_->SetStringSetting(wxT("USERNAME"), mmOptions::instance().userNameString_);

    wxButton *languageButton = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
    core_->iniSettings_->SetStringSetting(LANGUAGE_PARAMETER, languageButton->GetLabel().Lower());
    mmSelectLanguage(this, core_->iniSettings_, false);

    core_->currencyList_.setBaseCurrencySettings(core_->dbInfoSettings_.get(), currencyId_);
    core_->dbInfoSettings_->SetStringSetting(wxT("DATEFORMAT"), dateFormat_);
    SaveFinancialYearStart();
}

void mmOptionsDialog::SaveViewPanelSettings()
{
    SaveViewAccountOptions();
    SaveViewTransactionOptions();

    int size = choiceFontSize_->GetCurrentSelection() + 1;
    mmIniOptions::instance().html_font_size_ = size;
    core_->iniSettings_->SetIntSetting(wxT("HTMLFONTSIZE"), size);

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_TREE);
    mmIniOptions::instance().expandBankTree_ = itemCheckBox->GetValue();
    core_->iniSettings_->SetBoolSetting(wxT("EXPAND_BANK_TREE"), itemCheckBox->GetValue() );

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_TREE);
    mmIniOptions::instance().expandTermTree_ = itemCheckBox->GetValue();
    core_->iniSettings_->SetBoolSetting(wxT("EXPAND_TERM_TREE"), itemCheckBox->GetValue() );

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_HOME);
    mmIniOptions::instance().expandBankHome_ = itemCheckBox->GetValue();
    core_->iniSettings_->SetBoolSetting(wxT("EXPAND_BANK_HOME"), itemCheckBox->GetValue() );

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_HOME);
    mmIniOptions::instance().expandTermHome_ = itemCheckBox->GetValue();
    core_->iniSettings_->SetBoolSetting(wxT("EXPAND_TERM_HOME"), itemCheckBox->GetValue() );

    itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME);
    mmIniOptions::instance().expandStocksHome_ = itemCheckBox->GetValue();
    core_->iniSettings_->SetBoolSetting(wxT("ENABLESTOCKS"), itemCheckBox->GetValue() );

    mmIniOptions::instance().budgetFinancialYears_ = cbBudgetFinancialYears_->GetValue();
    core_->iniSettings_->SetBoolSetting(INIDB_BUDGET_FINANCIAL_YEARS, mmIniOptions::instance().budgetFinancialYears_);

    mmIniOptions::instance().budgetIncludeTransfers_ = cbBudgetIncludeTransfers_->GetValue();
    core_->iniSettings_->SetBoolSetting(INIDB_BUDGET_INCLUDE_TRANSFERS, mmIniOptions::instance().budgetIncludeTransfers_);

    mmIniOptions::instance().budgetSetupWithoutSummaries_ = cbBudgetSetupWithoutSummary_->GetValue();
    core_->iniSettings_->SetBoolSetting(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, mmIniOptions::instance().budgetSetupWithoutSummaries_);

    mmIniOptions::instance().budgetSummaryWithoutCategories_ = cbBudgetSummaryWithoutCateg_->GetValue();
    core_->iniSettings_->SetBoolSetting(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, mmIniOptions::instance().budgetSummaryWithoutCategories_);

    mmIniOptions::instance().ignoreFutureTransactions_ = cbIgnoreFutureTransactions_->GetValue();
    core_->iniSettings_->SetBoolSetting(INIDB_IGNORE_FUTURE_TRANSACTIONS, mmIniOptions::instance().ignoreFutureTransactions_);
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

    core_->iniSettings_->SetStringSetting(wxT("LISTALT0"), mmGetStringFromColour(mmColors::listAlternativeColor0));
    core_->iniSettings_->SetStringSetting(wxT("LISTALT1"), mmGetStringFromColour(mmColors::listAlternativeColor1));
    core_->iniSettings_->SetStringSetting(wxT("LISTBACK"), mmGetStringFromColour(mmColors::listBackColor));
    core_->iniSettings_->SetStringSetting(wxT("NAVTREE"),  mmGetStringFromColour(mmColors::navTreeBkColor));
    core_->iniSettings_->SetStringSetting(wxT("LISTBORDER"), mmGetStringFromColour(mmColors::listBorderColor));
    core_->iniSettings_->SetStringSetting(wxT("LISTDETAILSPANEL"), mmGetStringFromColour(mmColors::listDetailsPanelColor));
    core_->iniSettings_->SetStringSetting(wxT("LISTFUTUREDATES"), mmGetStringFromColour(mmColors::listFutureDateColor));
}

void mmOptionsDialog::SaveOthersPanelSettings()
{
    wxChoice* itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE);
    mmIniOptions::instance().transPayeeSelectionNone_ = itemChoice->GetSelection();
    core_->iniSettings_->SetIntSetting(wxT("TRANSACTION_PAYEE_NONE"), itemChoice->GetSelection());

    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY);
    mmIniOptions::instance().transCategorySelectionNone_ = itemChoice->GetSelection();
    core_->iniSettings_->SetIntSetting(wxT("TRANSACTION_CATEGORY_NONE"), itemChoice->GetSelection());

    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS);
    mmIniOptions::instance().transStatusReconciled_ = itemChoice->GetSelection();
    core_->iniSettings_->SetIntSetting(wxT("TRANSACTION_STATUS_RECONCILED"), itemChoice->GetSelection());

    itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE);
    mmIniOptions::instance().transDateDefault_ = itemChoice->GetSelection();
    core_->iniSettings_->SetIntSetting(wxT("TRANSACTION_DATE_DEFAULT"), itemChoice->GetSelection());

    SaveStocksUrl();

    core_->iniSettings_->SetBoolSetting(INIDB_USE_ORG_DATE_COPYPASTE, cbUseOrgDateCopyPaste_->GetValue());
    core_->iniSettings_->SetBoolSetting(INIDB_USE_TRANSACTION_SOUND, cbUseSound_->GetValue());
    core_->iniSettings_->SetBoolSetting(INIDB_UPDATE_CURRENCY_RATE, cbEnableCurrencyUpd_->GetValue());

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP);
    core_->iniSettings_->SetBoolSetting(wxT("BACKUPDB"), itemCheckBox->GetValue() );

    wxCheckBox* itemCheckBoxUpdate = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE);
    core_->iniSettings_->SetBoolSetting(wxT("BACKUPDB_UPDATE"), itemCheckBoxUpdate->GetValue() );

    core_->iniSettings_->SetIntSetting(wxT("MAX_BACKUP_FILES"), scMax_files_->GetValue());
}

void mmOptionsDialog::SaveImportExportPanelSettings()
{
    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4);
    wxString delim = st->GetValue();
    if (!delim.IsEmpty()) core_->dbInfoSettings_->SetStringSetting(wxT("DELIMITER"), delim);
}

bool mmOptionsDialog::GetUpdateCurrencyRateSetting()
{
    return cbEnableCurrencyUpd_->GetValue();
}

void mmOptionsDialog::OnOk(wxCommandEvent& /*event*/)
{
    changesApplied_ = true;
    EndModal(wxOK);
}


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
{   ID_BOOK_PANEL_EXP_IMP = wxID_HIGHEST + 1,
	ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_USER4,
	ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_COMMA4,
	ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_SEMICOLON4, 
	ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_TAB4,
	ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4,
};

enum ViewEnum
{
    VIEW_ALL,
    VIEW_OPEN,
    VIEW_FAVORITES,
    VIEW_MAX // number of elements, must be last
};

enum ViewTransEnum
{
    VIEW_TRANS_ALL,
    VIEW_TRANS_REC,
    VIEW_TRANS_UNREC,
    VIEW_TRANS_TODAY,
    VIEW_TRANS_30,
    VIEW_TRANS_90,
    VIEW_TRANS_LASTMONTH,
    VIEW_TRANS_CURRENTMONTH,
    VIEW_TRANS_MAX // number of elements, must be last
};

enum HtmlFontEnum
{
    HTML_FONT_XSMALL,
    HTML_FONT_SMALL,
    HTML_FONT_NORMAL,
    HTML_FONT_LARGE,
    HTML_FONT_XLARGE,
    HTML_FONT_XXLARGE,
    HTML_FONT_HUGE,
    HTML_FONT_MAX // number of elements, must be last
};

IMPLEMENT_DYNAMIC_CLASS( mmOptionsDialog, wxDialog )

BEGIN_EVENT_TABLE( mmOptionsDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, mmOptionsDialog::OnCurrency)

    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT, mmOptionsDialog::OnDateFormatChanged)

    EVT_CHOICE(ID_DIALOG_OPTIONS_VIEW_ACCOUNTS, mmOptionsDialog::OnViewAccountsChanged)  
    EVT_CHOICE(ID_DIALOG_OPTIONS_VIEW_TRANS, mmOptionsDialog::OnViewTransChanged)  
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE, mmOptionsDialog::OnLanguageChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE, mmOptionsDialog::OnNavTreeColorChanged)

    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0, mmOptionsDialog::OnAlt0Changed)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1, mmOptionsDialog::OnAlt1Changed)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK, mmOptionsDialog::OnListBackgroundChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER, mmOptionsDialog::OnListBorderChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT, mmOptionsDialog::OnRestoreDefaultColors)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS, mmOptionsDialog::OnListDetailsColors)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES, mmOptionsDialog::OnListFutureDates)

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_CHK_BACKUP, mmOptionsDialog::OnBackupDBChecked)
    EVT_CHECKBOX(ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE, mmOptionsDialog::OnBackupDBUpdateChecked)
    EVT_CHECKBOX(ID_DIALOG_OPTIONS_CHK_ORIG_DATE, mmOptionsDialog::OnOriginalDateChecked)

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_EXPAND_BANK_HOME, mmOptionsDialog::OnExpandBankHome)
    EVT_CHECKBOX(ID_DIALOG_OPTIONS_EXPAND_TERM_HOME, mmOptionsDialog::OnExpandTermHome)
    EVT_CHECKBOX(ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME, mmOptionsDialog::OnExpandStockHome)

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_EXPAND_BANK_TREE, mmOptionsDialog::OnExpandBankTree)
    EVT_CHECKBOX(ID_DIALOG_OPTIONS_EXPAND_TERM_TREE, mmOptionsDialog::OnExpandTermTree)

    EVT_CHOICE(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE, mmOptionsDialog::OnDefaultTransactionPayeeChanged)
    EVT_CHOICE(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY, mmOptionsDialog::OnTransactionCategoryChanged)
    EVT_CHOICE(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS, mmOptionsDialog::OnDefaultTransactionStatusChanged)

    EVT_CHOICE(ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_MONTH, mmOptionsDialog::OnFYSMonthChange)

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_CHK_USE_SOUND, mmOptionsDialog::OnUseSoundChecked)
    EVT_CHOICE(ID_DIALOG_OPTIONS_FONT_SIZE, mmOptionsDialog::OnFontSizeChanged)  

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_UPD_CURRENCY, mmOptionsDialog::OnUpdCurrencyChecked)
    
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

    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4);
    wxString delim = st->GetValue();
    if (!delim.IsEmpty())
        mmDBWrapper::setInfoSettingValue(db_, wxT("DELIMITER"), delim); 

    wxTextCtrl* stun = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME);
    wxString username = stun->GetValue();
    mmDBWrapper::setInfoSettingValue(db_, wxT("USERNAME"), username); 
    mmIniOptions::userNameString_ = username;

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

    //Find Financial Year Start Day field
    wxSpinCtrl* fysDay = (wxSpinCtrl*)FindWindow(ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY);
    //Get integer value then convert it to string value
    wxString fysDayVal = wxString::Format(wxT("%d"),fysDay->GetValue());
    //Put it ot DB
    mmDBWrapper::setInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_DAY"), fysDayVal); 
    mmOptions::financialYearStartDayString_ = fysDayVal;
}

mmOptionsDialog::mmOptionsDialog( mmCoreDB* core, wxSQLite3Database* inidb,
                                 wxWindow* parent, wxWindowID id, 
                                 const wxString& caption, 
                                 const wxPoint& pos, const wxSize& size, long style )
                                 : core_(core), inidb_(inidb), db_(core_->db_.get())
{
    Create(parent, id, caption, pos, size, style);
}

bool mmOptionsDialog::Create( wxWindow* parent, wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
    Fit();
    return TRUE;
}

wxString mmOptionsDialog::DisplayDate2FormatDate(wxString strDate)
{
    wxString DateFormat[TOTAL_DATEFORMAT] = 
    {
        wxT("%d/%m/%y"),
        wxT("%d/%m/%Y"),
        wxT("%d-%m-%y"),
        wxT("%d-%m-%Y"),
        wxT("%d.%m.%y"),
        wxT("%d.%m.%Y"),
        wxT("%d,%m,%y"),
        wxT("%d/%m %Y"),
        wxT("%m/%d/%y"),
        wxT("%m/%d/%Y"),
        wxT("%m-%d-%y"),
        wxT("%m-%d-%Y"),
        wxT("%m/%d'%Y"),
        wxT("%y/%m/%d"),
        wxT("%y-%m-%d"),
        wxT("%Y/%m/%d"),
        wxT("%Y-%m-%d"),
        wxT("%Y.%m.%d"),
        wxT("%Y%m%d"),
    };

    wxString itemChoice7Strings[TOTAL_DATEFORMAT] = 
    {
        wxT("DD/MM/YY"),
        wxT("DD/MM/YYYY"),
        wxT("DD-MM-YY"),
        wxT("DD-MM-YYYY"),
        wxT("DD.MM.YY"),
        wxT("DD.MM.YYYY"),
        wxT("DD,MM,YY"),
        wxT("DD/MM YYYY"),
        wxT("MM/DD/YY"),
        wxT("MM/DD/YYYY"),
        wxT("MM-DD-YY"),
        wxT("MM-DD-YYYY"),
        wxT("MM/DD'YYYY"),
        wxT("YY/MM/DD"),
        wxT("YY-MM-DD"),
        wxT("YYYY/MM/DD"),
        wxT("YYYY-MM-DD"),
        wxT("YYYY.MM.DD"),
        wxT("YYYYMMDD"),
    };

    for(int i=0; i<TOTAL_DATEFORMAT; i++)
    {
        if(strDate == itemChoice7Strings[i])
        {
            return DateFormat[i];
        }
    }

    return strDate;
}

wxString mmOptionsDialog::FormatDate2DisplayDate(wxString strDate)
{
    wxString DateFormat[TOTAL_DATEFORMAT] = 
    {
        wxT("%d/%m/%y"),
        wxT("%d/%m/%Y"),
        wxT("%d-%m-%y"),
        wxT("%d-%m-%Y"),
        wxT("%d.%m.%y"),
        wxT("%d.%m.%Y"),
        wxT("%d,%m,%y"),
        wxT("%d/%m %Y"),
        wxT("%m/%d/%y"),
        wxT("%m/%d/%Y"),
        wxT("%m-%d-%y"),
        wxT("%m-%d-%Y"),
        wxT("%m/%d'%Y"),
        wxT("%y/%m/%d"),
        wxT("%y-%m-%d"),
        wxT("%Y/%m/%d"),
        wxT("%Y-%m-%d"),
        wxT("%Y.%m.%d"),
        wxT("%Y%m%d"),
    };

    wxString itemChoice7Strings[TOTAL_DATEFORMAT] = 
    {
        wxT("DD/MM/YY"),
        wxT("DD/MM/YYYY"),
        wxT("DD-MM-YY"),
        wxT("DD-MM-YYYY"),
        wxT("DD.MM.YY"),
        wxT("DD.MM.YYYY"),
        wxT("DD,MM,YY"),
        wxT("DD/MM YYYY"),
        wxT("MM/DD/YY"),
        wxT("MM/DD/YYYY"),
        wxT("MM-DD-YY"),
        wxT("MM-DD-YYYY"),
        wxT("MM/DD'YYYY"),
        wxT("YY/MM/DD"),
        wxT("YY-MM-DD"),
        wxT("YYYY/MM/DD"),
        wxT("YYYY-MM-DD"),
        wxT("YYYY.MM.DD"),
        wxT("YYYYMMDD"),
    };

    for(int i=0; i<TOTAL_DATEFORMAT; i++)
    {
        if(strDate == DateFormat[i])
        {
            return itemChoice7Strings[i];
        }
    }

    return wxT("");
}

void mmOptionsDialog::OnDateFormatChanged(wxCommandEvent& /*event*/)
{
    wxString format = choiceDateFormat_->GetValue();
    if (format.Trim().IsEmpty())
    {
        return;
    }
    try
    {
        mmGetDateForDisplay(db_, wxDateTime::Now());
    }
    catch(...)
    {
        choiceDateFormat_->SetValue(FormatDate2DisplayDate(wxT("%m/%d/%y")));
        return;
    }

    mmOptions::dateFormat = DisplayDate2FormatDate(format);
    mmOptions::saveOptions(db_);
    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE);
    st->SetLabel(mmGetDateForDisplay(db_, wxDateTime::Now()) + _(" : Restart"));
}

void mmOptionsDialog::OnViewAccountsChanged(wxCommandEvent& /*event*/)
{
    int selection = choiceVisible_->GetSelection();

    wxString viewAcct = wxT("ALL");
    if (selection == VIEW_OPEN)
    {
        viewAcct = wxT("Open");
    }
    else if (selection == VIEW_FAVORITES)
    {
        viewAcct = wxT("Favorites");
    }
    mmDBWrapper::setINISettingValue(inidb_, wxT("VIEWACCOUNTS"), viewAcct);
}

void mmOptionsDialog::OnViewTransChanged(wxCommandEvent& /*event*/)
{
    int selection = choiceTransVisible_->GetSelection();

    wxString viewTrans = wxT("View All Transactions");
    if (selection == VIEW_TRANS_REC)
        viewTrans = wxT("View Reconciled");
    else if (selection == VIEW_TRANS_UNREC)
        viewTrans = wxT("View UnReconciled");
    else if (selection == VIEW_TRANS_TODAY)
        viewTrans = wxT("View Today");
    else if (selection == VIEW_TRANS_30)
        viewTrans = wxT("View 30 days");
    else if (selection == VIEW_TRANS_90)
        viewTrans = wxT("View 90 days");
    else if (selection == VIEW_TRANS_LASTMONTH)
        viewTrans = wxT("View Last Month");
    else if (selection == VIEW_TRANS_CURRENTMONTH)
        viewTrans = wxT("View Current Month");

    mmDBWrapper::setINISettingValue(inidb_, wxT("VIEWTRANSACTIONS"), viewTrans);
}

void mmOptionsDialog::OnFYSMonthChange(wxCommandEvent& /*event*/)
{
    wxString fysMonthVal = wxString() << monthSelection_->GetSelection() + 1;
    mmDBWrapper::setInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_MONTH"), fysMonthVal); 
    mmOptions::financialYearStartMonthString_ = fysMonthVal;
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
 
    mmOptionsDialog* mainDialog = this;
    wxBoxSizer* mainDialogSizer = new wxBoxSizer(wxVERTICAL);
    mainDialog->SetSizer(mainDialogSizer);

    wxPanel* mainDialogPanel = new wxPanel(mainDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    mainDialogSizer->Add(mainDialogPanel, 1, wxGROW|wxALL, 5);

    wxBoxSizer* mainDialogPanelSizer = new wxBoxSizer(wxVERTICAL);
    mainDialogPanel->SetSizer(mainDialogPanelSizer);

    wxListbook* newBook = new wxListbook(mainDialogPanel, ID_DIALOG_OPTIONS_LISTBOOK, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);

   /**********************************************************************************************
    General Panel
    **********************************************************************************************/
    wxPanel* generalPanel = new wxPanel(newBook, ID_BOOK_PANELGENERAL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    wxBoxSizer* generalPanelSizer = new wxBoxSizer(wxVERTICAL);
    generalPanel->SetSizer(generalPanelSizer);

    // ------------------------------------------
    // HomePage Header Settings
    wxStaticBox* headerStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Home Page Header"));

    // Define the staticBox font and set it as wxFONTWEIGHT_BOLD
    wxFont staticBoxFontSetting = headerStaticBox->GetFont();
    staticBoxFontSetting.SetWeight(wxFONTWEIGHT_BOLD);

    headerStaticBox->SetFont(staticBoxFontSetting);

    wxStaticBoxSizer* headerStaticBoxSizer = new wxStaticBoxSizer(headerStaticBox, wxHORIZONTAL);
    generalPanelSizer->Add(headerStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);

    wxStaticText* userNameText = new wxStaticText(generalPanel, wxID_STATIC, _("User Name"), wxDefaultPosition, wxDefaultSize, 0);
    headerStaticBoxSizer->Add(userNameText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString userName = mmDBWrapper::getInfoSettingValue(db_, wxT("USERNAME"), wxT(""));
    
    wxTextCtrl* userNameTextCtr = new wxTextCtrl(generalPanel, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME, userName, wxDefaultPosition, wxDefaultSize, 0);
    userNameTextCtr->SetToolTip(_("The User Name is used as a title for the database."));
    headerStaticBoxSizer->Add(userNameTextCtr, 1, wxALIGN_LEFT|wxGROW|wxALL, 5);

    // ------------------------------------------
    // Language Settings
    wxStaticBox* languageStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Language"));
    languageStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* languageStaticBoxSizer = new wxStaticBoxSizer(languageStaticBox, wxHORIZONTAL);
    generalPanelSizer->Add(languageStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxString lang = mmDBWrapper::getINISettingValue(inidb_, wxT("LANGUAGE"));
    wxButton* languageButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_LANGUAGE, lang, wxDefaultPosition, wxDefaultSize, 0);
    
    languageStaticBoxSizer->Add(languageButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    languageButton->SetToolTip(_("Specify the language to use"));

    // ------------------------------------------
    // Currency Settings
    wxStaticBox* currencyStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Currency"));
    currencyStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* currencyStaticBoxSizer = new wxStaticBoxSizer(currencyStaticBox, wxHORIZONTAL);
    generalPanelSizer->Add(currencyStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    currencyStaticBox->SetFont(staticBoxFontSetting);
    
    wxStaticText* baseCurrencyText = new wxStaticText( generalPanel, wxID_STATIC, _("Base Currency"), wxDefaultPosition, wxDefaultSize, 0);
    currencyStaticBoxSizer->Add(baseCurrencyText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
    wxString currName = _("Set Currency");
    if (currencyID != -1)
    {
        currName = mmDBWrapper::getCurrencyName(db_, currencyID);
    }
    wxButton* baseCurrencyButton = new wxButton(generalPanel, ID_DIALOG_OPTIONS_BUTTON_CURRENCY, currName, wxDefaultPosition, wxDefaultSize, 0);
    baseCurrencyButton->SetToolTip(_("Sets the default currency for the database."));
    currencyStaticBoxSizer->Add(baseCurrencyButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // ------------------------------------------
    // Date Format Settings
    wxStaticBox* dateFormatStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Date Format"));
    dateFormatStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* dateFormatStaticBoxSizer = new wxStaticBoxSizer(dateFormatStaticBox, wxVERTICAL);
    wxFlexGridSizer* dateFormatSettingStaticBoxSizerGrid = new wxFlexGridSizer(2,2,5,5);
    generalPanelSizer->Add(dateFormatStaticBoxSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    dateFormatStaticBoxSizer->Add(dateFormatSettingStaticBoxSizerGrid);
    
    wxStaticText* dateFormatText = new wxStaticText(generalPanel, wxID_STATIC, _("Date Format"), wxDefaultPosition, wxDefaultSize, 0);
    dateFormatSettingStaticBoxSizerGrid->Add(dateFormatText, 0, wxALIGN_CENTER|wxALL, 5);

    wxString itemChoice7Strings[TOTAL_DATEFORMAT] = 
    {
        wxT("DD/MM/YY"),
        wxT("DD/MM/YYYY"),
        wxT("DD-MM-YY"),
        wxT("DD-MM-YYYY"),
        wxT("DD.MM.YY"),
        wxT("DD.MM.YYYY"),
        wxT("DD,MM,YY"),
        wxT("DD/MM YYYY"),
        wxT("MM/DD/YY"),
        wxT("MM/DD/YYYY"),
        wxT("MM-DD-YY"),
        wxT("MM-DD-YYYY"),
        wxT("MM/DD'YYYY"),
        wxT("YY/MM/DD"),
        wxT("YY-MM-DD"),
        wxT("YYYY/MM/DD"),
        wxT("YYYY-MM-DD"),
        wxT("YYYY.MM.DD"),
        wxT("YYYYMMDD"),
    };

    wxString selectDateFormat = mmDBWrapper::getInfoSettingValue(db_, wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);
    choiceDateFormat_ = new wxComboBox(generalPanel, ID_DIALOG_OPTIONS_DATE_FORMAT, wxT(""), wxDefaultPosition, wxSize(140, -1), TOTAL_DATEFORMAT, itemChoice7Strings, 0);
    dateFormatSettingStaticBoxSizerGrid->Add(choiceDateFormat_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceDateFormat_->SetToolTip(_("Specify the date format for display"));
    choiceDateFormat_->SetValue(FormatDate2DisplayDate(selectDateFormat));

    wxButton* setFormatButton = new wxButton( generalPanel, ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT, _("Set"), wxDefaultPosition, wxDefaultSize, 0);
    dateFormatSettingStaticBoxSizerGrid->Add(setFormatButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* sampleDateText = new wxStaticText( generalPanel, ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE, _("Sample Date"), wxDefaultPosition, wxDefaultSize, 0);
    dateFormatSettingStaticBoxSizerGrid->Add(sampleDateText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sampleDateText->SetLabel(mmGetDateForDisplay(db_, wxDateTime::Now()));

    // ------------------------------------------
    // Financial Year Settings
    wxStaticBox* financialYearStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Financial Year"));
    financialYearStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* financialYearStaticBoxSizer = new wxStaticBoxSizer(financialYearStaticBox, wxVERTICAL);
    wxFlexGridSizer* financialYearStaticBoxSizerGrid = new wxFlexGridSizer(3,2,0,0);
    generalPanelSizer->Add(financialYearStaticBoxSizer, 0, wxGROW|wxALL, 5);
    financialYearStaticBoxSizer->Add(financialYearStaticBoxSizerGrid);
        
    wxStaticText* itemStaticTextFYSDay = new wxStaticText(generalPanel, wxID_STATIC, _("Start Day"), wxDefaultPosition, wxDefaultSize, 0 );
    financialYearStaticBoxSizerGrid->Add(itemStaticTextFYSDay, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString financialPeriodStartDay = mmDBWrapper::getInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_DAY"), wxT("1"));
    int day = wxAtoi(financialPeriodStartDay); 
    wxSpinCtrl *textFPSDay = new wxSpinCtrl(generalPanel, ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 31, day);
	textFPSDay->SetToolTip(_("Specify Day for start of financial year"));
    
    financialYearStaticBoxSizerGrid->Add(textFPSDay, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticTextSmonth = new wxStaticText(generalPanel, wxID_STATIC, _("Start Month"), wxDefaultPosition, wxDefaultSize, 0);
    financialYearStaticBoxSizerGrid->Add(itemStaticTextSmonth, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString financialMonthsSelection[12] = {  _("Jan"), _("Feb"), _("Mar"), _("Apr"), _("May"), _("Jun"),
                                                     _("Jul"), _("Aug"), _("Sep"), _("Oct"), _("Nov"), _("Dec") };
    monthSelection_ = new wxChoice(generalPanel, ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_MONTH, wxDefaultPosition, wxSize(100, -1), 12, financialMonthsSelection, 0);
    financialYearStaticBoxSizerGrid->Add(monthSelection_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxString financialPeriodStartMonth = mmDBWrapper::getInfoSettingValue(db_, wxT("FINANCIAL_YEAR_START_MONTH"), wxT("7"));
 
    int monthItem = wxAtoi(financialPeriodStartMonth);
    monthSelection_->SetSelection(monthItem - 1);
    monthSelection_->SetToolTip(_("Specify month for start of financial year"));
 
    //----------------------------------------------
    // Backup Settings
    wxStaticBox* backupStaticBox = new wxStaticBox(generalPanel, wxID_ANY, _("Database Backup"));
    backupStaticBox->SetFont(staticBoxFontSetting);
    wxStaticBoxSizer* backupStaticBoxSizer = new wxStaticBoxSizer(backupStaticBox, wxVERTICAL);
    generalPanelSizer->Add(backupStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxString backupDBState =  mmDBWrapper::getINISettingValue(inidb_, wxT("BACKUPDB"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxBackup = new wxCheckBox(generalPanel, ID_DIALOG_OPTIONS_CHK_BACKUP, _("Backup before opening"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxBackup->SetValue(FALSE);
    if (backupDBState == wxT("TRUE"))
    {
        itemCheckBoxBackup->SetValue(TRUE);
    }
    backupStaticBoxSizer->Add(itemCheckBoxBackup, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxBackup->SetToolTip(_("Select whether to create a _YYYY-MM-DD.bak file before opening the database"));

    wxString backupDBUpdate =  mmDBWrapper::getINISettingValue(inidb_, wxT("BACKUPDB_UPDATE"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxBackupUpdate = new wxCheckBox(generalPanel,ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE,_("Daily Update, before opening"),wxDefaultPosition,wxDefaultSize,wxCHK_2STATE);
    itemCheckBoxBackupUpdate->SetValue(FALSE);
    if (backupDBUpdate == wxT("TRUE"))
    {
        itemCheckBoxBackupUpdate->SetValue(TRUE);
    }
    itemCheckBoxBackupUpdate->Enable(itemCheckBoxBackup->GetValue());
    backupStaticBoxSizer->Add(itemCheckBoxBackupUpdate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxBackupUpdate->SetToolTip(_("For same day, select to update the backup before opening the database."));

   /**********************************************************************************************
    Views Panel
    **********************************************************************************************/
    wxPanel* itemPanelViews = new wxPanel(newBook, ID_BOOK_PANELVIEWS, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemPanelViews->SetSizer(itemBoxSizer7);

    wxStaticBox* itemStaticBoxSizer10Static = new wxStaticBox(itemPanelViews, wxID_ANY, _("Account View Options"));
    wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(itemStaticBoxSizer10Static, wxHORIZONTAL);
    itemBoxSizer7->Add(itemStaticBoxSizer10, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText45 = new wxStaticText( itemPanelViews, wxID_STATIC, _("Accounts Visible"), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticBoxSizer10->Add(itemStaticText45, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemChoiceViewAccountStrings[VIEW_MAX] = 
    {
        _("All"),
        _("Open"),
        _("Favorites")
    };  
    
    choiceVisible_ = new wxChoice(itemPanelViews, ID_DIALOG_OPTIONS_VIEW_ACCOUNTS, wxDefaultPosition, wxDefaultSize, 3, itemChoiceViewAccountStrings, 0);
    itemStaticBoxSizer10->Add(choiceVisible_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));
    choiceVisible_->SetSelection(VIEW_ALL);

    if (vAccts == wxT("Open"))
        choiceVisible_->SetSelection(VIEW_OPEN);
    else if (vAccts == wxT("Favorites"))
        choiceVisible_->SetSelection(VIEW_FAVORITES);
           
    choiceVisible_->SetToolTip(_("Specify which accounts are visible"));

    // -----------------------------------------

    wxStaticBox* itemStaticBoxSizerTransViewStatic = new wxStaticBox(itemPanelViews, wxID_ANY, _("Transaction View Options"));
    wxStaticBoxSizer* itemStaticBoxSizerTransView = new wxStaticBoxSizer(itemStaticBoxSizerTransViewStatic, wxVERTICAL);
    itemBoxSizer7->Add(itemStaticBoxSizerTransView, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticTextTransText = new wxStaticText(itemPanelViews, wxID_STATIC, _("Transactions Visible"), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticBoxSizerTransView->Add(itemStaticTextTransText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemChoiceViewTransStrings[VIEW_TRANS_MAX] = 
    {
        _("View All Transactions"),
        _("View Reconciled"),
        _("View UnReconciled"),
        _("View Today"),
        _("View 30 days"),
        _("View 90 days"),
        _("View Last Month"),
        _("View Current Month")
    };  
    
    choiceTransVisible_ = new wxChoice( itemPanelViews, ID_DIALOG_OPTIONS_VIEW_TRANS, wxDefaultPosition, wxDefaultSize, sizeof(itemChoiceViewTransStrings)/sizeof(*itemChoiceViewTransStrings), itemChoiceViewTransStrings, 0);
    itemStaticBoxSizerTransView->Add(choiceTransVisible_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString vTrans = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWTRANSACTIONS"), wxT("View All Transactions"));
    choiceTransVisible_->SetSelection(VIEW_TRANS_ALL);

    if (vTrans == wxT("View Reconciled"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_REC);
    else if (vTrans == wxT("View UnReconciled"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_UNREC);
    else if (vTrans == wxT("View Today"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_TODAY);
    else if (vTrans == wxT("View 30 days"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_30);
    else if (vTrans == wxT("View 90 days"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_90);
    else if (vTrans == wxT("View Last Month"))
         choiceTransVisible_->SetSelection(VIEW_TRANS_LASTMONTH);
    else if (vTrans == wxT("View Current Month"))
         choiceTransVisible_->SetSelection(VIEW_TRANS_CURRENTMONTH);
           
    choiceTransVisible_->SetToolTip(_("Specify which transactions are visible by default"));

    wxStaticBox* itemStaticBoxSizerFontSizeStatic = new wxStaticBox(itemPanelViews, wxID_ANY, _("Font Size Options"));
    wxStaticBoxSizer* itemStaticBoxSizerFontSize = new wxStaticBoxSizer(itemStaticBoxSizerFontSizeStatic, wxHORIZONTAL);
    itemBoxSizer7->Add(itemStaticBoxSizerFontSize, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticTextHTMLFontSizeText = new wxStaticText(itemPanelViews, wxID_STATIC, _("Report Font Size"), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticBoxSizerFontSize->Add(itemStaticTextHTMLFontSizeText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemChoiceHTMLFontSize[HTML_FONT_MAX] = 
    {
        wxT("XSmall"),
        wxT("Small"),
        wxT("Normal"),
        wxT("Large"),
        wxT("XLarge"),
        wxT("XXLarge"),
        wxT("Huge")
    };  
    
    choiceFontSize_ = new wxChoice(itemPanelViews, ID_DIALOG_OPTIONS_FONT_SIZE, wxDefaultPosition, wxSize(85, -1), 7, itemChoiceHTMLFontSize, 0);

    wxString vFontSize = mmDBWrapper::getINISettingValue(inidb_, wxT("HTMLFONTSIZE"), wxT("Font Size on the reports"));

    choiceFontSize_->SetSelection(HTML_FONT_NORMAL);

    if (vFontSize == wxT("1"))
        choiceFontSize_->SetSelection(HTML_FONT_XSMALL);
    else if (vFontSize == wxT("2"))
        choiceFontSize_->SetSelection(HTML_FONT_SMALL);
    else if (vFontSize == wxT("3"))
        choiceFontSize_->SetSelection(HTML_FONT_NORMAL);
    else if (vFontSize == wxT("4"))
        choiceFontSize_->SetSelection(HTML_FONT_LARGE);
    else if (vFontSize == wxT("5"))
         choiceFontSize_->SetSelection(HTML_FONT_XLARGE);
    else if (vFontSize == wxT("6"))
         choiceFontSize_->SetSelection(HTML_FONT_XXLARGE);
    else if (vFontSize == wxT("7"))
         choiceFontSize_->SetSelection(HTML_FONT_HUGE);
    
    choiceFontSize_->SetToolTip(_("Specify which font size is used on the report tables"));
        
    itemStaticBoxSizerFontSize->Add(choiceFontSize_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

//  Tree View Options
    wxStaticBox* itemStaticBoxSizerTreeViewOption = new wxStaticBox(itemPanelViews, wxID_ANY, _("Tree View Options: Expand Section"));
    wxStaticBoxSizer* itemStaticBoxSizerTreeView = new wxStaticBoxSizer(itemStaticBoxSizerTreeViewOption, wxHORIZONTAL);
    itemBoxSizer7->Add(itemStaticBoxSizerTreeView, 0, wxGROW|wxALL, 5);

// Check boxes for Tree View Options
    wxString expandBankTree =  mmDBWrapper::getINISettingValue(inidb_, wxT("EXPAND_BANK_TREE"), wxT("TRUE"));
    wxCheckBox* itemCheckBoxExpandBankTree = new wxCheckBox( itemPanelViews, ID_DIALOG_OPTIONS_EXPAND_BANK_TREE, _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandBankTree->SetValue(FALSE);
    if (expandBankTree == wxT("TRUE"))
    {
        itemCheckBoxExpandBankTree->SetValue(TRUE);
    }
    itemStaticBoxSizerTreeView->Add(itemCheckBoxExpandBankTree, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxExpandBankTree->SetToolTip(_("Expand Bank Accounts in Trew View when tree is refreshed"));

    wxString expandTermTree =  mmDBWrapper::getINISettingValue(inidb_, wxT("EXPAND_TERM_TREE"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxExpandTermTree = new wxCheckBox( itemPanelViews, ID_DIALOG_OPTIONS_EXPAND_TERM_TREE, _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandTermTree->SetValue(FALSE);
    if (expandTermTree == wxT("TRUE"))
    {
        itemCheckBoxExpandTermTree->SetValue(TRUE);
    }
    itemStaticBoxSizerTreeView->Add(itemCheckBoxExpandTermTree, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxExpandTermTree->SetToolTip(_("Expand Term Accounts in Trew View when tree is refreshed"));
    
//  Home Page Options
    wxStaticBox* itemStaticBoxSizerHomePageOption = new wxStaticBox(itemPanelViews, wxID_ANY, _("Home Page Options: Expand Section"));
    wxStaticBoxSizer* itemStaticBoxSizerHomePage = new wxStaticBoxSizer(itemStaticBoxSizerHomePageOption, wxVERTICAL);
    itemBoxSizer7->Add(itemStaticBoxSizerHomePage, 0, wxGROW|wxALL, 5);

//  Check boxes for Home Page Options
    wxString expandBankHome =  mmDBWrapper::getINISettingValue(inidb_, wxT("EXPAND_BANK_HOME"), wxT("TRUE"));
    wxCheckBox* itemCheckBoxExpandBankHome = new wxCheckBox( itemPanelViews, ID_DIALOG_OPTIONS_EXPAND_BANK_HOME, _("Bank Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandBankHome->SetValue(FALSE);
    if (expandBankHome == wxT("TRUE"))
    {
        itemCheckBoxExpandBankHome->SetValue(TRUE);
    }
    itemStaticBoxSizerHomePage->Add(itemCheckBoxExpandBankHome, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxExpandBankHome->SetToolTip(_("Expand Bank Accounts on home page when page is refreshed"));

    wxString expandTermHome =  mmDBWrapper::getINISettingValue(inidb_, wxT("EXPAND_TERM_HOME"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxExpandTermHome = new wxCheckBox( itemPanelViews, ID_DIALOG_OPTIONS_EXPAND_TERM_HOME, _("Term Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandTermHome->SetValue(FALSE);
    if (expandTermHome == wxT("TRUE"))
    {
        itemCheckBoxExpandTermHome->SetValue(TRUE);
    }
    itemStaticBoxSizerHomePage->Add(itemCheckBoxExpandTermHome, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxExpandTermHome->SetToolTip(_("Expand Term Accounts on home page when page is refreshed"));

    wxString expandStockHome =  mmDBWrapper::getINISettingValue(inidb_, wxT("ENABLESTOCKS"), wxT("TRUE"));
    wxCheckBox* itemCheckBoxExpandStockHome = new wxCheckBox(itemPanelViews, ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME, _("Stock Accounts"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxExpandStockHome->SetValue(FALSE);
    if (expandStockHome == wxT("TRUE"))
    {
        itemCheckBoxExpandStockHome->SetValue(TRUE);
    }
    itemStaticBoxSizerHomePage->Add(itemCheckBoxExpandStockHome, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxExpandStockHome->SetToolTip(_("Expand Stock Accounts on home page when page is refreshed"));
    // ------------------------------------------------------------------------
    // Colours Panel
    wxPanel* itemPanelColors = new wxPanel(newBook, ID_BOOK_PANELCOLORS, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemPanelColors->SetSizer(itemBoxSizer8);

    wxButton* itemButtonColorDefault = new wxButton(itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT, _("Restore Defaults"), wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer8->Add(itemButtonColorDefault, 0, wxALIGN_LEFT|wxALL, 5);
    itemButtonColorDefault->SetToolTip(_("Restore Default Colors"));

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(2, 2, 10, 10);
    itemBoxSizer8->Add(itemGridSizer2, 0, wxALL);

    wxStaticText* itemButtonColorLabel = new wxStaticText(itemPanelColors, wxID_STATIC, _("Nav Tree"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton* itemButtonColorNavTreeCtrl = new wxButton( itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE, _("Nav Tree"), wxDefaultPosition, wxDefaultSize, 0);
    itemGridSizer2->Add(itemButtonColorLabel, 0, wxALIGN_CENTER, 0);
    itemGridSizer2->Add(itemButtonColorNavTreeCtrl, 0, wxALIGN_CENTER, 0);
    itemButtonColorNavTreeCtrl->SetToolTip(_("Specify the color for the nav tree"));
    itemButtonColorNavTreeCtrl->SetBackgroundColour(mmColors::navTreeBkColor);

    wxStaticText* itemButtonColorListBackgroundLabel = new wxStaticText( itemPanelColors, wxID_STATIC, _("List Background"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton* itemButtonColorListBackground = new wxButton(itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK, _("List Background"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorListBackgroundLabel, 0, wxALIGN_CENTER, 0);
    itemGridSizer2->Add(itemButtonColorListBackground, 0, wxALIGN_CENTER, 0);
    itemButtonColorListBackground->SetToolTip(_("Specify the color for the list background"));
    itemButtonColorListBackground->SetBackgroundColour(mmColors::listBackColor);

    wxStaticText* itemButtonColorListAlt0Label = new wxStaticText(itemPanelColors, wxID_STATIC, _("List Row 0"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton* itemButtonColorListAlt0 = new wxButton(itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0, _("List Row 0"), wxDefaultPosition, wxDefaultSize, 0);
    itemGridSizer2->Add(itemButtonColorListAlt0Label, 0, wxALIGN_CENTER, 0);
    itemGridSizer2->Add(itemButtonColorListAlt0, 0, wxALIGN_CENTER, 0);
    itemButtonColorListAlt0->SetToolTip(_("Specify the color for the list row 0"));
    itemButtonColorListAlt0->SetBackgroundColour(mmColors::listAlternativeColor0);

    wxStaticText* itemButtonColorListAlt1Label = new wxStaticText( itemPanelColors, wxID_STATIC, _("List Row 1"), wxDefaultPosition, wxDefaultSize, 0 );
    wxButton* itemButtonColorListAlt1 = new wxButton( itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1, _("List Row 1"), wxDefaultPosition, wxDefaultSize, 0);
    itemGridSizer2->Add(itemButtonColorListAlt1Label, 0, wxALIGN_CENTER, 0);
    itemGridSizer2->Add(itemButtonColorListAlt1, 0, wxALIGN_CENTER, 0);
    itemButtonColorListAlt1->SetToolTip(_("Specify the color for the list row 1"));
    itemButtonColorListAlt1->SetBackgroundColour(mmColors::listAlternativeColor1);

    wxStaticText* itemButtonColorListBorderLabel = new wxStaticText( itemPanelColors, wxID_STATIC, _("List Border"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton* itemButtonColorListBorder = new wxButton( itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER, _("List Border"), wxDefaultPosition, wxDefaultSize, 0);
    itemGridSizer2->Add(itemButtonColorListBorderLabel, 0, wxALIGN_CENTER, 0);
    itemGridSizer2->Add(itemButtonColorListBorder, 0, wxALIGN_CENTER, 0);
    itemButtonColorListBorder->SetToolTip(_("Specify the color for the list Border"));
    itemButtonColorListBorder->SetBackgroundColour(mmColors::listBorderColor);

    wxStaticText* itemButtonColorListDetailsLabel = new wxStaticText( itemPanelColors, wxID_STATIC, _("List Details"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton* itemButtonColorListDetails = new wxButton( itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS, _("List Details"), wxDefaultPosition, wxDefaultSize, 0);
    itemGridSizer2->Add(itemButtonColorListDetailsLabel, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    itemGridSizer2->Add(itemButtonColorListDetails, 0, wxALIGN_CENTER, 0);
    itemButtonColorListDetails->SetToolTip(_("Specify the color for the list details"));
    itemButtonColorListDetails->SetBackgroundColour(mmColors::listDetailsPanelColor);

    wxStaticText* itemButtonColorFutureDatesLabel = new wxStaticText( itemPanelColors, wxID_STATIC, _("Future Transactions"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton* itemButtonColorFutureDates = new wxButton( itemPanelColors, ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES, _("Future Transactions"), wxDefaultPosition, wxDefaultSize, 0);
    itemGridSizer2->Add(itemButtonColorFutureDatesLabel, 0, wxALIGN_CENTER, 0);
    itemGridSizer2->Add(itemButtonColorFutureDates, 0, wxALIGN_CENTER, 0);
    itemButtonColorFutureDates->SetToolTip(_("Specify the color for future transactions"));
    itemButtonColorFutureDates->SetBackgroundColour(mmColors::listFutureDateColor);
   
    // ------------------------------------------------------------------------
    // Miscellaneous Panel
    // ------------------------------------------------------------------------
    wxPanel* itemPanelMisc = new wxPanel(newBook, ID_BOOK_PANELMISC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* itemBoxSizerMisc = new wxBoxSizer(wxVERTICAL);
    itemPanelMisc->SetSizer(itemBoxSizerMisc);


    //----------------------------------------------
    
    // New transaction dialog settings
    wxStaticBox* itemStaticBoxNewTransactionSettings = new wxStaticBox(itemPanelMisc, wxID_ANY, _("New Transaction Dialog Settings"));
    wxStaticBoxSizer* itemStaticBoxSizerNewTransactionSettings = new wxStaticBoxSizer(itemStaticBoxNewTransactionSettings, wxVERTICAL);
    itemBoxSizerMisc->Add(itemStaticBoxSizerNewTransactionSettings, 0, wxGROW|wxALL, 5);

    //  Default Payee
    wxStaticText* payeeStaticText = new wxStaticText(itemPanelMisc, wxID_STATIC, _("Default Payee:"),wxDefaultPosition, wxDefaultSize);

    wxString itemChoiceDefaultTransPayeeStrings[] = 
    {
        _("None"),
        _("Last Used"),
    };
    
    wxChoice* itemChoiceDefaultTransPayee = new wxChoice(itemPanelMisc, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE, wxDefaultPosition, wxDefaultSize, 2, itemChoiceDefaultTransPayeeStrings, 0);
    itemChoiceDefaultTransPayee->SetSelection(mmIniOptions::transPayeeSelectionNone_);
    
    //  Default Category
    wxStaticText* categoryStaticText = new wxStaticText(itemPanelMisc, wxID_STATIC, _("Default Category:"),wxDefaultPosition, wxDefaultSize);

    wxString itemChoiceDefaultTransCategoryStrings[] =
    {
        _("None"),
        _("Last used for payee"),
    };
    
    wxChoice* itemChoiceDefaultTransCategory = new wxChoice( itemPanelMisc, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY, wxDefaultPosition, wxDefaultSize, 2, itemChoiceDefaultTransCategoryStrings);
    itemChoiceDefaultTransCategory->SetSelection(mmIniOptions::transCategorySelectionNone_);

    //  Default Status
    wxStaticText* statusStaticText = new wxStaticText( itemPanelMisc, wxID_STATIC, _("Default Status:"), wxDefaultPosition, wxDefaultSize);

    wxChoice* itemChoiceDefaultTransStatus = new wxChoice( itemPanelMisc, ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS, wxDefaultPosition, wxDefaultSize, 5, trxStatuses4Choice);
    itemChoiceDefaultTransStatus->SetSelection(mmIniOptions::transStatusReconciled_);

    wxFlexGridSizer* newTransflexGridSizer = new wxFlexGridSizer(3,2,0,0);
    itemStaticBoxSizerNewTransactionSettings->Add(newTransflexGridSizer);
    newTransflexGridSizer->Add(payeeStaticText,0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
    newTransflexGridSizer->Add(itemChoiceDefaultTransPayee,0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(categoryStaticText,0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
    newTransflexGridSizer->Add(itemChoiceDefaultTransCategory,0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    newTransflexGridSizer->Add(statusStaticText,0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5);
    newTransflexGridSizer->Add(itemChoiceDefaultTransStatus,0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //----------------------------------------------
  
    wxBoxSizer* itemBoxSizerStockURL = new wxBoxSizer(wxVERTICAL);
    itemBoxSizerMisc->Add(itemBoxSizerStockURL, 0, wxGROW|wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticTextURL = new wxStaticText(itemPanelMisc, wxID_STATIC, _("Stock Quote Web Page"), wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizerStockURL->Add(itemStaticTextURL, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxString stockURL = mmDBWrapper::getInfoSettingValue(db_, wxT("STOCKURL"), mmex::DEFSTOCKURL);
    wxTextCtrl* itemTextCtrURL = new wxTextCtrl(itemPanelMisc, ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL, stockURL, wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizerStockURL->Add(itemTextCtrURL, 1, wxGROW|wxALIGN_LEFT|wxALL, 5);
    itemTextCtrURL->SetToolTip(_("Clear the field to Reset the value to system default."));

    wxString useOriginalDate =  mmDBWrapper::getINISettingValue(inidb_, wxT("USEORIGDATEONCOPYPASTE"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxOrigDate = new wxCheckBox(itemPanelMisc, ID_DIALOG_OPTIONS_CHK_ORIG_DATE, _("Use Original Date when Pasting Transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxOrigDate->SetValue(FALSE);
    if (useOriginalDate == wxT("TRUE"))
    {
        itemCheckBoxOrigDate->SetValue(TRUE);
    }
    itemBoxSizerMisc->Add(itemCheckBoxOrigDate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxOrigDate->SetToolTip(_("Select whether to use the original transaction date or current date when copying/pasting transactions"));

    wxString useSound =  mmDBWrapper::getINISettingValue(inidb_, wxT("USETRANSSOUND"), wxT("TRUE"));
    wxCheckBox* itemCheckBoxUseSound = new wxCheckBox(itemPanelMisc, ID_DIALOG_OPTIONS_CHK_USE_SOUND, _("Use Transaction Sound"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxUseSound->SetValue(FALSE);
    if (useSound == wxT("TRUE"))
    {
        itemCheckBoxUseSound->SetValue(TRUE);
    }
    itemBoxSizerMisc->Add(itemCheckBoxUseSound, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxUseSound->SetToolTip(_("Select whether to use sounds when entering transactions"));

    wxString enableCurrencyUpd = mmDBWrapper::getINISettingValue(inidb_, wxT("UPDATECURRENCYRATE"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxOnlineCurrencyUpd = new wxCheckBox( itemPanelMisc, ID_DIALOG_OPTIONS_UPD_CURRENCY, _("Enable online currency update \n(Get data from European Central Bank)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBoxOnlineCurrencyUpd->SetValue(FALSE);
    if(enableCurrencyUpd == wxT("TRUE"))
    {
        itemCheckBoxOnlineCurrencyUpd->SetValue(TRUE);
    }
    itemBoxSizerMisc->Add(itemCheckBoxOnlineCurrencyUpd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxOnlineCurrencyUpd->SetToolTip(_("Enable or disable get data from European Central Bank to update currency rate"));
    
    //a bit more space needed for proper alignment
    itemBoxSizerMisc->AddSpacer(40);

    // ---------------------------------------------------------------------------------------------------
    // Export Import Panel
    wxPanel* itemPanelExpImp = new wxPanel(newBook, ID_BOOK_PANEL_EXP_IMP, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    wxBoxSizer* itemBoxSizer204 = new wxBoxSizer(wxVERTICAL);
    itemPanelExpImp->SetSizer(itemBoxSizer204);


    // ------------------------------------------    
    wxStaticBox* itemStaticBoxSizer184Static = new wxStaticBox(itemPanelExpImp, wxID_ANY, _("Import/Export Settings"));
    wxStaticBoxSizer* itemStaticBoxSizer184 = new wxStaticBoxSizer(itemStaticBoxSizer184Static, wxVERTICAL);
    
    itemBoxSizer204->Add(itemStaticBoxSizer184, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxStaticText* itemStaticText54 = new wxStaticText(itemPanelExpImp, wxID_STATIC, _("CSV Delimiter"), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticBoxSizer184->Add(itemStaticText54, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    wxFlexGridSizer* itemFlexGridSizer184 = new wxFlexGridSizer(2, 3, 0, 0);
    itemStaticBoxSizer184->Add(itemFlexGridSizer184, 0, wxALL);

    wxString delimiter = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), mmex::DEFDELIMTER);
    
    wxRadioButton* delimiterRadioButtonU4 = new wxRadioButton(itemPanelExpImp, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_USER4, _("User Defind"), wxDefaultPosition, wxDefaultSize, 0);
    wxRadioButton* delimiterRadioButtonC4 = new wxRadioButton(itemPanelExpImp, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_COMMA4, _("Comma"), wxDefaultPosition, wxDefaultSize, 0);
    if (delimiter == wxT(","))
    {
        delimiterRadioButtonC4 ->SetValue(true);
    }
    wxRadioButton* delimiterRadioButtonS4 = new wxRadioButton(itemPanelExpImp, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_SEMICOLON4, _("Semicolon"), wxDefaultPosition, wxDefaultSize, 0);
    if (delimiter == wxT(";"))
    {
        delimiterRadioButtonS4 ->SetValue(true);
    }
    wxRadioButton* delimiterRadioButtonT4 = new wxRadioButton(itemPanelExpImp, ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_TAB4, _("TAB"), wxDefaultPosition, wxDefaultSize, 0);
    if (delimiter == wxT("\t"))
    {
        delimiterRadioButtonT4 ->SetValue(true);
    }
    wxTextCtrl* textDelimiter4 = new wxTextCtrl( itemPanelExpImp, ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER4, delimiter, wxDefaultPosition, wxDefaultSize, 0);
    textDelimiter4->SetToolTip(_("Specify the delimiter to use when importing/exporting CSV files"));
    textDelimiter4->SetMaxLength(2);
    if (delimiter == wxT("\t") || delimiter == wxT(",") || delimiter == wxT(";"))
    {
        textDelimiter4->Enable(false);
    }

    itemFlexGridSizer184->Add(delimiterRadioButtonC4, 0, wxALIGN_LEFT|wxALL, 0);
    itemFlexGridSizer184->Add(delimiterRadioButtonS4, 0, wxALIGN_LEFT|wxALL, 0);
    itemFlexGridSizer184->Add(delimiterRadioButtonT4, 0, wxALIGN_LEFT|wxALL, 0);

    itemFlexGridSizer184->Add(delimiterRadioButtonU4, 0, wxALIGN_LEFT|wxALL, 0);
    itemFlexGridSizer184->Add(textDelimiter4, 0, wxALIGN_LEFT|wxLEFT, 20);



   /**********************************************************************************************
    Setting up the notebook with the 5 pages
    **********************************************************************************************/
    newBook->SetImageList(m_imageList);

    newBook->InsertPage(0, generalPanel, _("General"), true, 2);
    newBook->InsertPage(1, itemPanelViews, _("View Options"), false, 0);
    newBook->InsertPage(2, itemPanelColors, _("Colors"), false, 1);
    newBook->InsertPage(3, itemPanelMisc, _("Others"), false, 3);
    newBook->InsertPage(4, itemPanelExpImp, _("Export/Import"), false, 4);

    mainDialogPanelSizer->Add(newBook, 1, wxGROW|wxALL, 5);
    mainDialogPanelSizer->Layout();

   /**********************************************************************************************
    Button Panel with OK and Cancel Buttons
    **********************************************************************************************/
    wxStaticLine* panelSeparatorLine = new wxStaticLine(mainDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    mainDialogSizer->Add(panelSeparatorLine,0,wxGROW|wxLEFT|wxRIGHT, 10);
    
    wxPanel* buttonPanel = new wxPanel(mainDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainDialogSizer->Add(buttonPanel, 0, wxALIGN_RIGHT|wxALL, 5);
    
    wxButton* itemButtonOK = new wxButton(buttonPanel, wxID_OK, _("&OK"));
    wxButton* itemButtonCancel = new wxButton(buttonPanel, wxID_CANCEL, _("&Cancel"));
    buttonPanelSizer->Add(itemButtonOK, 0, wxALIGN_RIGHT|wxRIGHT, 5);
    buttonPanelSizer->Add(itemButtonCancel, 0, wxALIGN_RIGHT|wxRIGHT, 5);
    itemButtonOK->SetFocus();
}

void mmOptionsDialog::OnCurrency(wxCommandEvent& /*event*/)
{
    int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
     
    if (mmMainCurrencyDialog::Execute(core_, this, currencyID) && currencyID != -1)
    {
        wxString currName = mmDBWrapper::getCurrencyName(db_, currencyID);
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_CURRENCY);
        bn->SetLabel(currName);
        mmDBWrapper::setBaseCurrencySettings(db_, currencyID);

        wxMessageDialog msgDlg(this, _("Remember to update currency rate"), _("Important note"));
        msgDlg.ShowModal();            
    }
}

void mmOptionsDialog::OnLanguageChanged(wxCommandEvent& /*event*/)
{
    wxString lang = mmSelectLanguage(this, inidb_, true);

    if (lang.empty()) {
        return;
    }

    wxButton *btn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
    wxASSERT(btn);
        
    if (btn && btn->GetLabel() != lang) {

        int x = 0;
        int y = 0;
        btn->GetTextExtent(lang, &x, &y);

        wxSize sz = btn->GetSize();
        sz.SetWidth(x << 1);
        btn->SetSize(sz);

        btn->SetLabel(lang);

        Fit(); // resize dialog window
    }
}

void mmOptionsDialog::OnNavTreeColorChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::navTreeBkColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::navTreeBkColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnAlt0Changed(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listAlternativeColor0);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listAlternativeColor0 = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnAlt1Changed(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listAlternativeColor1);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listAlternativeColor1 = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnListBackgroundChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listBackColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listBackColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnListBorderChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listBorderColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listBorderColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER);
        bn->SetBackgroundColour(col);
    }
}

void  mmOptionsDialog::OnListDetailsColors(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listDetailsPanelColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listDetailsPanelColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS);
        bn->SetBackgroundColour(col);
    }
}

void  mmOptionsDialog::OnListFutureDates(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listFutureDateColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listFutureDateColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnRestoreDefaultColors(wxCommandEvent& /*event*/)
{
    mmRestoreDefaultColors();
}

void mmOptionsDialog::SetIniDatabaseCheckboxValue(wxString dbField, bool dbState)
{
    if (dbState)
        mmDBWrapper::setINISettingValue(inidb_, dbField, wxT("TRUE"));
    else
        mmDBWrapper::setINISettingValue(inidb_, dbField, wxT("FALSE"));
}

void mmOptionsDialog::OnExpandBankHome(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_HOME);
    mmIniOptions::expandBankHome_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_HOME"),mmIniOptions::expandBankHome_);
}

void mmOptionsDialog::OnExpandTermHome(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_HOME);
    mmIniOptions::expandTermHome_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_HOME"),mmIniOptions::expandTermHome_);
}

void mmOptionsDialog::OnExpandStockHome(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME);
    mmIniOptions::expandStocksHome_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("ENABLESTOCKS"),mmIniOptions::expandStocksHome_);
}

void mmOptionsDialog::OnExpandBankTree(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_BANK_TREE);
    mmIniOptions::expandBankTree_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_BANK_TREE"),mmIniOptions::expandBankTree_);
}

void mmOptionsDialog::OnExpandTermTree(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_EXPAND_TERM_TREE);
    mmIniOptions::expandTermTree_ = itemCheckBox->GetValue();
    SetIniDatabaseCheckboxValue(wxT("EXPAND_TERM_TREE"),mmIniOptions::expandTermTree_);
}

void mmOptionsDialog::OnBackupDBChecked(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP);
    SetIniDatabaseCheckboxValue(wxT("BACKUPDB"), itemCheckBox->GetValue() );

    wxCheckBox* itemCheckBox_u = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE);
    itemCheckBox_u->Enable(itemCheckBox->GetValue());
}

void mmOptionsDialog::OnBackupDBUpdateChecked(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE);
    SetIniDatabaseCheckboxValue(wxT("BACKUPDB_UPDATE"), itemCheckBox->GetValue() );
}

void mmOptionsDialog::OnOriginalDateChecked(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_ORIG_DATE);
    SetIniDatabaseCheckboxValue(wxT("USEORIGDATEONCOPYPASTE"), itemCheckBox->GetValue() );
}

void mmOptionsDialog::OnUseSoundChecked(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_USE_SOUND);
    SetIniDatabaseCheckboxValue(wxT("USETRANSSOUND"), itemCheckBox->GetValue() );
}

void mmOptionsDialog::OnUpdCurrencyChecked(wxCommandEvent& /*event*/)
{
    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_UPD_CURRENCY);
    SetIniDatabaseCheckboxValue(wxT("UPDATECURRENCYRATE"), itemCheckBox->GetValue() );
}

void mmOptionsDialog::OnFontSizeChanged(wxCommandEvent& /*event*/)
{
    int size = choiceFontSize_->GetCurrentSelection() + 1;
    mmIniOptions::fontSize_ = wxString::Format(wxT("%d"), size);
    mmDBWrapper::setINISettingValue(inidb_, wxT("HTMLFONTSIZE"), mmIniOptions::fontSize_);
    // resize dialog window
    Fit();
}

void mmOptionsDialog::OnDefaultTransactionPayeeChanged(wxCommandEvent& /*event*/)
{
    wxChoice* itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE);
    mmIniOptions::transPayeeSelectionNone_ = itemChoice->GetSelection();
    mmDBWrapper::setINISettingValue(inidb_, wxT("TRANSACTION_PAYEE_NONE"), wxString::Format(wxT("%d"), (int)mmIniOptions::transPayeeSelectionNone_)); 
}

void mmOptionsDialog::OnTransactionCategoryChanged(wxCommandEvent& /*event*/)
{
    wxChoice* itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY);
    mmIniOptions::transCategorySelectionNone_ = itemChoice->GetSelection();
    mmDBWrapper::setINISettingValue(inidb_, wxT("TRANSACTION_CATEGORY_NONE"), wxString::Format(wxT("%d"), (int)mmIniOptions::transCategorySelectionNone_)); 
}

void mmOptionsDialog::OnDefaultTransactionStatusChanged(wxCommandEvent& /*event*/)
{
    wxChoice* itemChoice = (wxChoice*)FindWindow(ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS);
    mmIniOptions::transStatusReconciled_ = itemChoice->GetSelection();
    mmDBWrapper::setINISettingValue(inidb_, wxT("TRANSACTION_STATUS_RECONCILED"), wxString::Format(wxT("%d"), (int)mmIniOptions::transStatusReconciled_)); 
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

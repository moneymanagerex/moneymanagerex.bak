/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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
 ********************************************************/
#include "mmex.h"
#include "aboutdialog.h"
#include "appstartdialog.h"
#include "assetspanel.h"
#include "billsdepositsdialog.h"
#include "billsdepositspanel.h"
#include "budgetingpanel.h"
#include "budgetyeardialog.h"
#include "categdialog.h"
#include "constants.h"
#include "currencydialog.h"
#include "customreportdialog.h"
#include "customreportdisplay.h"
#include "customreportindex.h"
#include "dbwrapper.h"
#include "filtertransdialog.h"
#include "htmlbuilder.h"
#include "maincurrencydialog.h"
#include "mmcheckingpanel.h"
#include "mmgraphtopcategories.h"
#include "mmhelppanel.h"
#include "mmhomepagepanel.h"
#include "mmreportspanel.h"
#include "newacctdialog.h"
#include "optionsdialog.h"
#include "paths.h"
#include "payeedialog.h"
#include "platfdep.h"
#include "qif_export.h"
#include "qif_import.h"
#include "relocatecategorydialog.h"
#include "relocatepayeedialog.h"
#include "reportbase.h"
#include "reports/budgetcategorysummary.h"
#include "reports/budgetingperf.h"
#include "reports/cashflow.h"
#include "reports/categexp.h"
#include "reports/categovertimeperf.h"
#include "reports/incexpenses.h"
#include "reports/incexpensesfinancialperiod.h"
#include "reports/incexpesestime.h"
#include "reports/payee.h"
#include "reports/summary.h"
#include "reports/summaryassets.h"
#include "reports/summarystocks.h"
#include "reports/transactions.h"
#include "reports/trx_stats.h"
#include "recentfiles.h"
#include "stockspanel.h"
#include "transdialog.h"
#include "univcsvdialog.h"
#include "util.h"

//----------------------------------------------------------------------------
#include <string>
#include <wx/debugrpt.h>
#include <wx/sysopt.h>
//----------------------------------------------------------------------------

const int REPEAT_TRANS_DELAY_TIME = 7000; // 7 seconds
//----------------------------------------------------------------------------

/*
    See also: wxStackWalker, wxDebugReportUpload.
*/
void reportFatalException(wxDebugReport::Context ctx)
{
    wxDebugReportCompress rep;

    if (!rep.IsOk())
    {
        wxSafeShowMessage(mmex::getProgramName(), _("Fatal error occured.\nApplication will be terminated."));
        return;
    }

    rep.AddAll(ctx);

    wxDebugReportPreviewStd preview;

    if (preview.Show(rep) && rep.Process()) {
        rep.Reset();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
IMPLEMENT_APP(mmGUIApp)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

/*
    wxHandleFatalExceptions implemented for some compilers\platforms only.
    MinGW could't find this function, wxWidgets 2.8.10.

    P.S. Try for next versions of wxWidgets.
*/

mmGUIApp::mmGUIApp()
{
#ifndef __MINGW32__
    wxHandleFatalExceptions(); // tell the library to call OnFatalException()
#endif
}
//----------------------------------------------------------------------------

mmGUIApp::SQLiteInit::SQLiteInit()
{
    wxSQLite3Database::InitializeSQLite();
}
//----------------------------------------------------------------------------

mmGUIApp::SQLiteInit::~SQLiteInit()
{
    wxSQLite3Database::ShutdownSQLite();
}
//----------------------------------------------------------------------------

/*
    This method allows catching the exceptions thrown by any event handler.
*/
void mmGUIApp::HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const
{
    try
    {
        wxApp::HandleEvent(handler, func, event);
    }
    catch (const wxSQLite3Exception &e)
    {
        wxLogError(e.GetMessage());
    }
    catch (const std::exception &e)
    {
        wxLogError(wxString::Format(wxT("%s"), e.what()));
    }
}
//----------------------------------------------------------------------------

void mmGUIApp::OnFatalException()
{
    reportFatalException(wxDebugReport::Context_Exception);
}
//----------------------------------------------------------------------------

bool OnInitImpl(mmGUIApp &app)
{
    app.SetAppName(mmex::GetAppName());

    /* Setting Locale causes unexpected problems, so default to English Locale */
    app.getLocale().Init(wxLANGUAGE_ENGLISH);

    /* Initialize Image Handlers */
    wxImage::AddHandler(new wxICOHandler());
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());

    wxSharedPtr<wxSQLite3Database> pIniSettingsDb(new wxSQLite3Database);
    pIniSettingsDb.get()->Open(mmex::getPathUser(mmex::SETTINGS));
    wxSharedPtr<MMEX_IniSettings> pIniSettings(new MMEX_IniSettings(pIniSettingsDb));

    /* Load Colors from Database */
    mmLoadColorsFromDatabase(pIniSettings);

    /* Load MMEX Custom Settings */
    mmIniOptions::instance().loadOptions(pIniSettings);

    /* Was App Maximized? */
    bool isMaxStr = pIniSettings->GetBoolSetting(wxT("ISMAXIMIZED"), false);

    /* Load Dimensions of Window */
    int valx = pIniSettings->GetIntSetting(wxT("ORIGINX"),50);
    int valy = pIniSettings->GetIntSetting(wxT("ORIGINY"),50);
    int valw = pIniSettings->GetIntSetting(wxT("SIZEW"),800);
    int valh = pIniSettings->GetIntSetting(wxT("SIZEH"),600);

    mmSelectLanguage(0, pIniSettings, false);


//removed by omalleypat - can't compile in wxWidgets 2.9
//#if defined (__WXMAC__) || defined (__WXOSX__)
//wxSystemOptions::SetOption(wxMAC_ALWAYS_USE_GENERIC_LISTCTRL,1);
//#endif

    mmGUIFrame *frame = new mmGUIFrame(mmex::getProgramName(), wxPoint(valx, valy), wxSize(valw, valh), pIniSettings);
    bool ok = frame->Show();
    wxASSERT(ok);

    if (isMaxStr)
    {
        frame->Maximize(true);
    }

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return ok;
}

//----------------------------------------------------------------------------
bool mmGUIApp::OnInit()
{
    bool ok = false;

    try
    {
        ok = wxApp::OnInit() && OnInitImpl(*this);
    }
    catch (const wxSQLite3Exception &e)
    {
        wxLogError(e.GetMessage());
    }
    catch (const std::exception &e)
    {
        wxLogError(wxString::Format(wxT("%s"), e.what()));
    }

    return ok;
}
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(mmNewDatabaseWizard, wxWizard)
    EVT_WIZARD_CANCEL(wxID_ANY,   mmGUIFrame::OnWizardCancel)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

mmNewDatabaseWizard::mmNewDatabaseWizard(wxFrame *frame, mmCoreDB* core)
         :wxWizard(frame,wxID_ANY,_("New Database Wizard"),
                   wxBitmap(addacctwiz_xpm),wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE), m_core(core)
{
/****************** Message to be displayed******************

    The next pages will help you create a new database.

    Your database file is stored with an extension of .mmb.

    As this file contains important financial information,
    we recommended creating daily backups with the Options
    setting: 'Backup before opening', and store your backups
    in a separate location.

    The database can later be encrypted if required, by
    using the option: 'Save database as' and changing the
    file type before saving.
*/
    page1 = new wxWizardPageSimple(this);
    wxString displayMsg;
    displayMsg << _("The next pages will help you create a new database.") << wxT("\n\n")
               << _("Your database file is stored with an extension of .mmb.")<< wxT("\n\n")
               << _("As this file contains important financial information,\nwe recommended creating daily backups with the Options\nsetting: 'Backup before opening', and store your backups\nin a separate location.")<< wxT("\n\n")
               << _("The database can later be encrypted if required, by\nusing the option: 'Save database as' and changing the\nfile type before saving.");
    new wxStaticText(page1, wxID_ANY,displayMsg);

    mmNewDatabaseWizardPage1* page2 = new mmNewDatabaseWizardPage1(this);

    // set the page order using a convenience function - could also use
    // SetNext/Prev directly as below
    wxWizardPageSimple::Chain(page1, page2);

    // allow the wizard to size itself around the pages
    GetPageAreaSizer()->Add(page1);
}
//----------------------------------------------------------------------------

void mmNewDatabaseWizard::RunIt(bool modal)
{
    if ( modal )
    {
        if ( RunWizard(page1) )
        {
            // Success
        }

        Destroy();
    }
    else
    {
        FinishLayout();
        ShowPage(page1);
        Show(true);
    }
}
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(mmNewDatabaseWizardPage1, wxWizardPageSimple)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, mmNewDatabaseWizardPage1::OnCurrency)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

mmNewDatabaseWizardPage1::mmNewDatabaseWizardPage1(mmNewDatabaseWizard* parent) :
    wxWizardPageSimple(parent),
    parent_(parent),
    currencyID_(-1)
{
    currencyID_ = parent_->m_core->currencyList_.GetBaseCurrencySettings();
    wxString currName = _("Set Currency");
    if (currencyID_ != -1)
        currName = parent_->m_core->currencyList_.getCurrencySharedPtr(currencyID_)->currencyName_;

    itemButtonCurrency_ = new wxButton( this, ID_DIALOG_OPTIONS_BUTTON_CURRENCY, currName, wxDefaultPosition, wxSize(130,-1), 0 );

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add( new wxStaticText(this, wxID_ANY, _("Base Currency for account")), 0, wxALL, 5 );
    mainSizer->Add( itemButtonCurrency_, 0 /* No stretching */, wxALL, 5 /* Border size */ );

    wxString helpMsg;
/**************************Message to be displayed *************
    Specify the base (or default) currency to be used for the
    database. The base currency can later be changed by using
    the options dialog. New accounts, will use this currency by
    default, and can be changed when editing account details.
***************************************************************/
    helpMsg << _("Specify the base (or default) currency to be used for the\ndatabase. The base currency can later be changed by using\nthe options dialog. New accounts, will use this currency by\ndefault, and can be changed when editing account details.")
            << wxT("\n");
    mainSizer->Add( new wxStaticText(this, wxID_ANY, helpMsg), 0, wxALL, 5);


    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( this, wxID_STATIC, _("User Name"));
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    itemUserName_ = new wxTextCtrl( this, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME, _T(""), wxDefaultPosition, wxSize(130,-1), 0 );
    itemBoxSizer5->Add(itemUserName_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    helpMsg.Empty();
    helpMsg << _("(Optional) Specify a title or your name.") << wxT("\n")
            << _("Used as a database title for displayed and printed reports.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY, helpMsg), 0, wxALL, 5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool mmNewDatabaseWizardPage1::TransferDataFromWindow()
{
    if ( currencyID_ == -1)
    {
        wxMessageBox(_("Base Currency Not Set"), _("New Database"), wxOK|wxICON_WARNING, this);

        return false;
    }
    userName = itemUserName_->GetValue().Trim();
    parent_->m_core->dbInfoSettings_->SetStringSetting(wxT("USERNAME"), userName);

    return true;
}
//----------------------------------------------------------------------------

void mmNewDatabaseWizardPage1::OnCurrency(wxCommandEvent& /*event*/)
{
    currencyID_ = parent_->m_core->currencyList_.GetBaseCurrencySettings();

    if (mmMainCurrencyDialog::Execute(parent_->m_core, this, currencyID_) && currencyID_ != -1)
    {
        wxString currName = parent_->m_core->currencyList_.getCurrencySharedPtr(currencyID_)->currencyName_;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_CURRENCY);
        bn->SetLabel(currName);
        parent_->m_core->currencyList_.SetBaseCurrencySettings(currencyID_);
    }
}
//----------------------------------------------------------------------------

mmAddAccountWizard::mmAddAccountWizard(wxFrame *frame, mmCoreDB* core) :
    wxWizard(frame,wxID_ANY,_("Add Account Wizard"),
    wxBitmap(addacctwiz_xpm),wxDefaultPosition,
    wxDEFAULT_DIALOG_STYLE), m_core(core), acctID_(-1)
{
    // a wizard page may be either an object of predefined class
    page1 = new wxWizardPageSimple(this);

    wxString noteString = mmex::getProgramName() +
    _(" models all transactions as belonging to accounts.\n\nThe next pages will help you create a new account.\n\nTo help you get started, begin by making a list of all\nfinancial institutions where you hold an account.");

    new wxStaticText(page1, wxID_ANY, noteString);

    mmAddAccountPage1* page2 = new mmAddAccountPage1(this);
    mmAddAccountPage2* page3 = new mmAddAccountPage2(this);

    // set the page order using a convenience function - could also use
    // SetNext/Prev directly as below
    wxWizardPageSimple::Chain(page1, page2);
    wxWizardPageSimple::Chain(page2, page3);

    // allow the wizard to size itself around the pages
    GetPageAreaSizer()->Add(page1);
    this->CentreOnParent();
}
//----------------------------------------------------------------------------

void mmAddAccountWizard::RunIt(bool modal)
{
    if (modal) {
        if (RunWizard(page1)) {
            // Success
        }
        Destroy();
    } else {
        FinishLayout();
        ShowPage(page1);
        Show(true);
    }
}
//----------------------------------------------------------------------------

bool mmAddAccountPage1::TransferDataFromWindow()
{
    if ( textAccountName_->GetValue().empty())
    {
        wxMessageBox(_("Account Name Invalid"), _("New Account"), wxOK|wxICON_WARNING, this);
        return false;
    }
    parent_->accountName_ = textAccountName_->GetValue().Trim();
    return true;
}
//----------------------------------------------------------------------------

mmAddAccountPage1::mmAddAccountPage1(mmAddAccountWizard* parent) :
    wxWizardPageSimple(parent), parent_(parent)
{
    textAccountName_ = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(130,-1), 0 );

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Name of the Account")), 0, wxALL, 5 );
    mainSizer->Add( textAccountName_, 0 /* No stretching */, wxALL, 5 /* Border Size */);

    wxString helpMsg;
    helpMsg  << wxT("\n") << _("Specify a descriptive name for the account.") << wxT("\n")
            << _("This is generally the name of a financial institution\nwhere the account is held. For example: 'ABC Bank'.");
    mainSizer->Add(new wxStaticText(this, wxID_ANY, helpMsg ), 0, wxALL, 5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

mmAddAccountPage2::mmAddAccountPage2(mmAddAccountWizard *parent) :
    wxWizardPageSimple(parent),
    parent_(parent)
{
    wxArrayString itemAcctTypeStrings;
    itemAcctTypeStrings.Add(_("Checking/Savings"));      // ACCOUNT_TYPE_BANK
    itemAcctTypeStrings.Add(_("Investment"));            // ACCOUNT_TYPE_STOCK
    itemAcctTypeStrings.Add(_("Term"));                  // ACCOUNT_TYPE_TERM

    itemChoiceType_ = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, itemAcctTypeStrings);
    itemChoiceType_->SetToolTip(_("Specify the type of account to be created."));

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add( new wxStaticText(this, wxID_ANY, _("Type of Account")), 0, wxALL, 5 );
    mainSizer->Add( itemChoiceType_, 0 /* No stretching*/, wxALL, 5 /* Border Size */);

    wxString textMsg;
    textMsg << wxT("\n")
            << _("Select the type of account you want to create:") << wxT("\n\n")
            << _("General bank accounts cover a wide variety of account\ntypes like Checking, Savings and Credit card type accounts.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY,textMsg), 0, wxALL, 5);

    textMsg = wxT("\n");
    textMsg << _("Investment accounts are specialized accounts that only\nhave stock/mutual fund investments associated with them.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY,textMsg), 0, wxALL, 5);

    textMsg = wxT("\n");
    textMsg << _("Term accounts are specialized bank accounts. Intended for asset\ntype accounts such as Term Deposits and Bonds. These accounts\ncan have regular money coming in and out, being outside the\ngeneral income stream.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY,textMsg), 0, wxALL, 5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool mmAddAccountPage2::TransferDataFromWindow()
{
    int acctType = itemChoiceType_->GetSelection();
    wxString acctTypeStr = ACCOUNT_TYPE_BANK;
    if (acctType == 1)
        acctTypeStr = ACCOUNT_TYPE_STOCK;
    else if (acctType == 2)
        acctTypeStr = ACCOUNT_TYPE_TERM;

    int currencyID = parent_->m_core->currencyList_.GetBaseCurrencySettings();
    if (currencyID == -1)
    {
        wxString errorMsg;
        errorMsg << _("Base Account Currency Not set.") << wxT("\n")
                 << _("Set that first using Tools->Options menu and then add a new account.");
        wxMessageBox( errorMsg, _("New Account"), wxOK|wxICON_WARNING, this);
        return false;
    }

    mmAccount* ptrBase = new mmAccount();

    wxSharedPtr<mmAccount> pAccount(ptrBase);

    pAccount->favoriteAcct_ = true;
    pAccount->status_ = mmAccount::MMEX_Open;
    pAccount->acctType_ = acctTypeStr;
    pAccount->name_ = parent_->accountName_;
    pAccount->initialBalance_ = 0;
    pAccount->currency_ = parent_->m_core->currencyList_.getCurrencySharedPtr(currencyID);
    // prevent same account being added multiple times in case of using 'Back' and 'Next' in wizard.
    if ( ! parent_->m_core->accountList_.AccountExists(pAccount->name_))
        parent_->acctID_ = parent_->m_core->accountList_.AddAccount(pAccount);

    return true;
}
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(mmGUIFrame, wxFrame)
    EVT_MENU(MENU_NEW, mmGUIFrame::OnNew)
    EVT_MENU(MENU_OPEN, mmGUIFrame::OnOpen)
    EVT_MENU(MENU_SAVE_AS, mmGUIFrame::OnSaveAs)
    EVT_MENU(MENU_CONVERT_ENC_DB, mmGUIFrame::OnConvertEncryptedDB)
    EVT_MENU(MENU_EXPORT_CSV, mmGUIFrame::OnExportToCSV)
    EVT_MENU(MENU_EXPORT_QIF, mmGUIFrame::OnExportToQIF)
    EVT_MENU(MENU_IMPORT_QIF, mmGUIFrame::OnImportQIF)
    EVT_MENU(MENU_IMPORT_UNIVCSV, mmGUIFrame::OnImportUniversalCSV)
    EVT_MENU(wxID_EXIT,  mmGUIFrame::OnQuit)
    EVT_MENU(MENU_NEWACCT,  mmGUIFrame::OnNewAccount)
    EVT_MENU(MENU_ACCTLIST,  mmGUIFrame::OnAccountList)
    EVT_MENU(MENU_ACCTEDIT,  mmGUIFrame::OnEditAccount)
    EVT_MENU(MENU_ACCTDELETE,  mmGUIFrame::OnDeleteAccount)
    EVT_MENU(MENU_ORGCATEGS,  mmGUIFrame::OnOrgCategories)
    EVT_MENU(MENU_ORGPAYEE,  mmGUIFrame::OnOrgPayees)
    EVT_MENU(wxID_PREFERENCES,  mmGUIFrame::OnOptions)
    EVT_MENU(wxID_NEW,  mmGUIFrame::OnNewTransaction)
    EVT_MENU(MENU_BUDGETSETUPDIALOG, mmGUIFrame::OnBudgetSetupDialog)
    EVT_MENU(wxID_HELP,  mmGUIFrame::OnHelp)
    EVT_MENU(MENU_CHECKUPDATE,  mmGUIFrame::OnCheckUpdate)
    EVT_MENU(MENU_REPORTISSUES,  mmGUIFrame::OnReportIssues)
    EVT_MENU(MENU_ANNOUNCEMENTMAILING,  mmGUIFrame::OnBeNotified)
    EVT_MENU(MENU_FACEBOOK,  mmGUIFrame::OnFacebook)
    EVT_MENU(wxID_ABOUT, mmGUIFrame::OnAbout)
    EVT_MENU(MENU_PRINT_PAGE_SETUP, mmGUIFrame::OnPrintPageSetup)
    EVT_MENU(MENU_PRINT_REPORT, mmGUIFrame::OnPrintPageReport)
    EVT_MENU(MENU_PRINT_PREVIEW_REPORT, mmGUIFrame::OnPrintPagePreview)
    EVT_MENU(MENU_SHOW_APPSTART, mmGUIFrame::OnShowAppStartDialog)
    EVT_MENU(MENU_EXPORT_HTML, mmGUIFrame::OnExportToHtml)
    EVT_MENU(MENU_BILLSDEPOSITS, mmGUIFrame::OnBillsDeposits)

    EVT_MENU(MENU_ASSETS, mmGUIFrame::OnAssets)
    EVT_MENU(MENU_CURRENCY, mmGUIFrame::OnCurrency)
    EVT_MENU(MENU_TRANSACTIONREPORT, mmGUIFrame::OnTransactionReport)
    EVT_MENU(MENU_TREEPOPUP_LAUNCHWEBSITE, mmGUIFrame::OnLaunchAccountWebsite)
    EVT_MENU(MENU_VIEW_TOOLBAR, mmGUIFrame::OnViewToolbar)
    EVT_MENU(MENU_VIEW_LINKS, mmGUIFrame::OnViewLinks)
    EVT_MENU(MENU_VIEW_BANKACCOUNTS, mmGUIFrame::OnViewBankAccounts)
    EVT_MENU(MENU_VIEW_TERMACCOUNTS, mmGUIFrame::OnViewTermAccounts)
    EVT_MENU(MENU_VIEW_STOCKACCOUNTS, mmGUIFrame::OnViewStockAccounts)
    EVT_MENU(MENU_CATEGORY_RELOCATION, mmGUIFrame::OnCategoryRelocation)
    EVT_MENU(MENU_PAYEE_RELOCATION, mmGUIFrame::OnPayeeRelocation)

    // Added for easier ability to test new feature.
    // May be taken out in future after being added to Options Dialog.
    EVT_MENU(MENU_IGNORE_FUTURE_TRANSACTIONS, mmGUIFrame::OnIgnoreFutureTransactions)

    EVT_MENU(MENU_ONLINE_UPD_CURRENCY_RATE, mmGUIFrame::OnOnlineUpdateCurRate)
    EVT_UPDATE_UI(MENU_VIEW_TOOLBAR, mmGUIFrame::OnViewToolbarUpdateUI)
    EVT_UPDATE_UI(MENU_VIEW_LINKS, mmGUIFrame::OnViewLinksUpdateUI)
    EVT_MENU(MENU_TREEPOPUP_EDIT, mmGUIFrame::OnPopupEditAccount)
    EVT_MENU(MENU_TREEPOPUP_DELETE, mmGUIFrame::OnPopupDeleteAccount)

    EVT_TREE_ITEM_MENU(wxID_ANY, mmGUIFrame::OnItemMenu)
    //EVT_TREE_ITEM_RIGHT_CLICK(ID_NAVTREECTRL, mmGUIFrame::OnItemRightClick)
    EVT_TREE_ITEM_ACTIVATED(ID_NAVTREECTRL, mmGUIFrame::OnItemRightClick)
    EVT_TREE_SEL_CHANGED(ID_NAVTREECTRL, mmGUIFrame::OnSelChanged)
    EVT_TREE_ITEM_EXPANDED(ID_NAVTREECTRL, mmGUIFrame::OnTreeItemExpanded)
    EVT_TREE_ITEM_COLLAPSED(ID_NAVTREECTRL,mmGUIFrame::OnTreeItemCollapsed)

    EVT_MENU(MENU_GOTOACCOUNT, mmGUIFrame::OnGotoAccount)
    EVT_MENU(MENU_STOCKS, mmGUIFrame::OnGotoStocksAccount)

    /* Navigation Panel */
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_NEW, mmGUIFrame::OnNewAccount)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_DELETE, mmGUIFrame::OnDeleteAccount)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_EDIT, mmGUIFrame::OnEditAccount)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_LIST, mmGUIFrame::OnAccountList)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_EXPORT2CSV, mmGUIFrame::OnExportToCSV)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_EXPORT2QIF, mmGUIFrame::OnExportToQIF)
    //EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTQIF, mmGUIFrame::OnImportQIF)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTUNIVCSV, mmGUIFrame::OnImportUniversalCSV)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWALL, mmGUIFrame::OnViewAllAccounts)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE, mmGUIFrame::OnViewFavoriteAccounts)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWOPEN, mmGUIFrame::OnViewOpenAccounts)

    /* Custom Reports */
    EVT_MENU(wxID_EDIT, mmGUIFrame::OnEditCustomSqlReport)

    /*Automatic processing of repeat transactions*/
    EVT_TIMER(AUTO_REPEAT_TRANSACTIONS_TIMER_ID, mmGUIFrame::OnAutoRepeatTransactionsTimer)

    /* Recent Files */
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE5, mmGUIFrame::OnRecentFiles)
    EVT_MENU(MENU_RECENT_FILES_CLEAR, mmGUIFrame::OnClearRecentFiles)

END_EVENT_TABLE()
//----------------------------------------------------------------------------

mmGUIFrame::mmGUIFrame(const wxString& title,
                       const wxPoint& pos,
                       const wxSize& size,
                       wxSharedPtr<MMEX_IniSettings> pIniSettings)
: wxFrame(0, -1, title, pos, size)
, m_inisettings(pIniSettings)
, gotoAccountID_(-1)
, gotoTransID_(-1)
, homePageAccountSelect_(false)
, checkingAccountPage_(0)
, activeCheckingAccountPage_(false)
, budgetingPage_(0)
, activeBudgetingPage_(false)
, refreshRequested_()
, autoRepeatTransactionsTimer_(this, AUTO_REPEAT_TRANSACTIONS_TIMER_ID)
, activeHomePage_(false)
, panelCurrent_()
, homePanel_()
, navTreeCtrl_()
, menuBar_()
, toolBar_()
, selectedItemData_()
, menuItemOnlineUpdateCurRate_()
, helpFileIndex_(mmex::HTML_INDEX)
, activeTermAccounts_(false)
, expandedReportNavTree_(true)
, expandedCustomSqlReportNavTree_(false)
, expandedBudgetingNavTree_(true)
{
    // tell wxAuiManager to manage this frame
    m_mgr.SetManagedWindow(this);
    CreateStatusBar();
    SetIcon(mmex::getProgramIcon());
    SetMinSize(wxSize(480,275));

    /* Setup Printer */
    printer_.reset(new wxHtmlEasyPrinting(mmex::getProgramName(), this));
    wxString printHeaderBase = mmex::getProgramName();
    printer_-> SetHeader( printHeaderBase + wxT("(@PAGENUM@/@PAGESCNT@)<hr>"), wxPAGE_ALL);

    restorePrinterValues();

    custRepIndex_ = new CustomReportIndex();

    // decide if we need to show app start dialog
    bool from_scratch = m_inisettings->GetBoolSetting(wxT("SHOWBEGINAPP"), true);

    wxFileName dbpath = mmDBWrapper::getLastDbPath(m_inisettings);
    if (from_scratch && !dbpath.IsOk()) mmSelectLanguage(this, m_inisettings, true);

    /* Create the Controls for the frame */
    createMenu();
    createToolBar();
    createControls();
    recentFiles_ = new RecentDatabaseFiles(m_inisettings, menuRecentFiles_);

    // Load perspective
    wxString auiPerspective = m_inisettings->GetStringSetting(wxT("AUIPERSPECTIVE"), m_perspective);
    m_mgr.LoadPerspective(auiPerspective);

    // add the toolbars to the manager
    m_mgr.AddPane(toolBar_, wxAuiPaneInfo().
        Name(wxT("toolbar")).Caption(_("Toolbar")).ToolbarPane().Top()
        .LeftDockable(false).RightDockable(false).MinSize(1000,-1)
        .Show(m_inisettings->GetBoolSetting(wxT("SHOWTOOLBAR"), true)));

    // change look and feel of wxAuiManager
    m_mgr.GetArtProvider()->SetMetric(16, 0);
    m_mgr.GetArtProvider()->SetMetric(3, 1);

    // Save default perspective
    m_perspective = m_mgr.SavePerspective();
    m_inisettings->SetStringSetting(wxT("AUIPERSPECTIVE"), m_perspective);

    // "commit" all changes made to wxAuiManager
    m_mgr.Update();

    // enable or disable online update currency rate
    if (m_inisettings->GetBoolSetting(INIDB_UPDATE_CURRENCY_RATE, false))
    {
        if (menuItemOnlineUpdateCurRate_)
            menuItemOnlineUpdateCurRate_->Enable(true);
    }
    else
    {
        if (menuItemOnlineUpdateCurRate_)
            menuItemOnlineUpdateCurRate_->Enable(false);
    }

    if (from_scratch) dbpath  = wxGetEmptyString();
    if (from_scratch || !dbpath.IsOk())
    {
        menuEnableItems(false);
        showBeginAppDialog(dbpath.GetFullName().IsEmpty());
    }
    else
    {
        if (!openFile(dbpath.GetFullPath(), false))
            showBeginAppDialog(true);
    }
}
//----------------------------------------------------------------------------

mmGUIFrame::~mmGUIFrame()
{
    try {
        cleanup();
    } catch (...) {
        wxASSERT(false);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::cleanup()
{
    printer_.reset();
    delete recentFiles_;
    delete custRepIndex_;
    saveSettings();

    m_mgr.UnInit();

    /* Delete the GUI */
    cleanupHomePanel(false);

    if (m_core)  m_core.reset();
    if (m_db)    m_db->Close();

    /// Update the database according to user requirements
    if (mmOptions::instance().databaseUpdated_ && m_inisettings->GetBoolSetting(wxT("BACKUPDB_UPDATE"), false))
    {
        BackupDatabase(fileName_, true);
    }
}
//----------------------------------------------------------------------------
// process all events waiting in the event queue if any.
void mmGUIFrame::processPendingEvents()
{
    while (wxGetApp().Pending())
    {
        wxGetApp().Dispatch();
    }
}

//----------------------------------------------------------------------------
// returns a wxTreeItemID for the accountName in the navtree section.
wxTreeItemId mmGUIFrame::getTreeItemfor(wxTreeItemId itemID, const wxString& accountName) const
{
    wxTreeItemIdValue treeDummyValue;
    wxTreeItemId navTreeID = navTreeCtrl_->GetFirstChild(itemID, treeDummyValue);

    bool searching = true;
    while (navTreeID.IsOk() && searching)
    {
        if ( accountName == navTreeCtrl_->GetItemText(navTreeID))
            searching = false;
        else
            navTreeID = navTreeCtrl_->GetNextChild(itemID, treeDummyValue);
    }
    return navTreeID;
}

//----------------------------------------------------------------------------
bool mmGUIFrame::setAccountInSection(wxString sectionName, wxString accountName)
{
    bool accountNotFound = true;
    wxTreeItemId rootItem = getTreeItemfor(navTreeCtrl_->GetRootItem(), sectionName );
    if (rootItem.IsOk() && navTreeCtrl_->ItemHasChildren(rootItem))
    {
        navTreeCtrl_->ExpandAllChildren(rootItem);
        wxTreeItemId accountItem = getTreeItemfor(rootItem, accountName);
        if (accountItem.IsOk())
        {
            // Set the NavTreeCtrl and prevent any event code being executed for now.
            homePageAccountSelect_ = true;
            navTreeCtrl_->SelectItem(accountItem);
            processPendingEvents();
            homePageAccountSelect_ = false;
            accountNotFound = false;
        }
    }
    return accountNotFound;
}

//----------------------------------------------------------------------------
bool mmGUIFrame::setNavTreeSection( wxString sectionName)
{
    bool accountNotFound = true;
    wxTreeItemId rootItem = getTreeItemfor(navTreeCtrl_->GetRootItem(), sectionName );
    if (rootItem.IsOk())
    {
        // Set the NavTreeCtrl and prevent any event code being executed for now.
        homePageAccountSelect_ = true;
        navTreeCtrl_->SelectItem(rootItem);
        processPendingEvents();
        homePageAccountSelect_ = false;
        accountNotFound = false;
    }
    return accountNotFound;
}

//----------------------------------------------------------------------------
void mmGUIFrame::setAccountNavTreeSection(wxString accountName)
{
    if ( setAccountInSection(_("Bank Accounts"), accountName))
        if (setAccountInSection(_("Term Accounts"), accountName))
            setAccountInSection(_("Stocks"), accountName);
}

//----------------------------------------------------------------------------
void mmGUIFrame::setHomePageActive(bool active)
{
    activeHomePage_ = active;
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnAutoRepeatTransactionsTimer(wxTimerEvent& /*event*/)
{
    bool autoExecuteManual = false; // Used when decoding: REPEATS
    bool autoExecuteSilent = false;
    bool requireExecution  = false;
    bool continueExecution = false;

    m_core->currencyList_.LoadBaseCurrencySettings();
    m_db.get()->Begin();
    wxSQLite3ResultSet q1 = m_db.get()->ExecuteQuery(SELECT_ALL_FROM_BILLSDEPOSITS_V1);
    while (q1.NextRow())
    {
        mmBDTransactionHolder th;
        th.id_             = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));

        th.nextOccurStr_   = mmGetDateForDisplay(th.nextOccurDate_);
        th.payeeID_        = q1.GetInt(wxT("PAYEEID"));
        th.transType_      = q1.GetString(wxT("TRANSCODE"));
        th.accountID_      = q1.GetInt(wxT("ACCOUNTID"));
        th.toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));

        th.accountName_    = m_core.get()->accountList_.GetAccountName(th.accountID_);
        th.amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
        th.toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));
        th.notes_          = q1.GetString(wxT("NOTES"));
        th.categID_        = q1.GetInt(wxT("CATEGID"));
        th.categoryStr_    = m_core.get()->categoryList_.GetCategoryName(th.categID_);
        th.subcategID_     = q1.GetInt(wxT("SUBCATEGID"));
        th.subcategoryStr_ = m_core.get()->categoryList_.GetSubCategoryName(th.categID_, th.subcategID_);

        // DeMultiplex the Auto Executable fields from the db entry: REPEATS
        int repeats        = q1.GetInt(wxT("REPEATS"));
        int numRepeats     = q1.GetInt(wxT("NUMOCCURRENCES"));

        autoExecuteManual = false;
        autoExecuteSilent = false;
        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute User Acknowlegement required
        {
            autoExecuteManual = true;
            repeats -= BD_REPEATS_MULTIPLEX_BASE;
        }

        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute Silent mode
        {
            autoExecuteManual = false;               // Can only be manual or auto. Not both
            autoExecuteSilent = true;
            repeats -= BD_REPEATS_MULTIPLEX_BASE;
        }

        wxDateTime today = wxDateTime::Now();
        wxTimeSpan ts = th.nextOccurDate_.Subtract(today);
        th.daysRemaining_ = ts.GetDays();
        int minutesRemaining_ = ts.GetMinutes();

        requireExecution = false;
        if (minutesRemaining_ > 0)
            th.daysRemaining_ += 1;

        if (th.daysRemaining_ < 1)
        {
            requireExecution = true;
        }

        mmex::formatDoubleToCurrencyEdit(th.amt_, th.transAmtString_);
        mmex::formatDoubleToCurrencyEdit(th.toAmt_, th.transToAmtString_);

        th.payeeStr_ = m_core.get()->payeeList_.GetPayeeName(th.payeeID_);

        if (th.transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            wxString fromAccount = m_core.get()->accountList_.GetAccountName(th.accountID_);
            wxString toAccount = m_core.get()->accountList_.GetAccountName(th.toAccountID_ );

            th.payeeStr_ = toAccount;
        }

        if (autoExecuteManual && requireExecution)
        {
            if ( (repeats < 11) || (numRepeats > 0) || (repeats > 14) )
            {
                continueExecution = true;
                mmBDDialog repeatTransactionsDlg(m_core.get(), th.id_ ,false ,true , this, SYMBOL_BDDIALOG_IDNAME , _(" Auto Repeat Transactions"));
                if ( repeatTransactionsDlg.ShowModal() == wxID_OK )
                {
                    if (activeHomePage_)
                    {
                        createHomePage(); // Update home page details only if it is being displayed
                    }
                }
                else // stop repeat executions from occuring
                {
                    continueExecution = false;
                }
            }
        }

        if (autoExecuteSilent && requireExecution)
        {
            if ( (repeats < 11) || (numRepeats > 0) || (repeats > 14))
            {
                continueExecution = true;
                wxSharedPtr<mmBankTransaction> pTransaction;
                wxSharedPtr<mmBankTransaction> pTemp(new mmBankTransaction(m_core.get()->db_));
                pTransaction = pTemp;

                wxSharedPtr<mmCurrency> pCurrencyPtr = m_core.get()->accountList_.getCurrencySharedPtr(th.accountID_);
                wxASSERT(pCurrencyPtr);

                pTransaction->accountID_ = th.accountID_;
                pTransaction->toAccountID_ = th.toAccountID_;
                pTransaction->payee_ = m_core.get()->payeeList_.GetPayeeSharedPtr(th.payeeID_);
                pTransaction->transType_ = th.transType_;
                pTransaction->amt_ = th.amt_;
                pTransaction->status_ = q1.GetString(wxT("STATUS"));
                pTransaction->transNum_ = q1.GetString(wxT("TRANSACTIONNUMBER"));
                pTransaction->notes_ = th.notes_;
                pTransaction->category_ = m_core.get()->categoryList_.GetCategorySharedPtr(th.categID_, th.subcategID_);
                pTransaction->date_ = th.nextOccurDate_;
                pTransaction->toAmt_ = th.toAmt_;

                wxSharedPtr<mmSplitTransactionEntries> split(new mmSplitTransactionEntries());
                split->loadFromBDDB(m_core.get(),th.id_);
                *pTransaction->splitEntries_.get() = *split.get();

                pTransaction->updateAllData(m_core.get(), th.accountID_, pCurrencyPtr);
                m_core.get()->bTransactionList_.addTransaction(pTransaction);
            }
            mmDBWrapper::completeBDInSeries(m_db.get(), th.id_);

            if (activeHomePage_)
            {
                createHomePage(); // Update home page details only if it is being displayed
            }
        }
    }
    q1.Finalize();
    m_db.get()->Commit();

    if (continueExecution)
    {
        autoRepeatTransactionsTimer_.Start(5, wxTIMER_ONE_SHOT);
    }
}
//----------------------------------------------------------------------------

/*
        Save our settings to ini db.
*/
void mmGUIFrame::saveSettings()
{
    if (! fileName_.IsEmpty())
    {
        wxFileName fname(fileName_);
        m_inisettings->SetStringSetting(wxT("LASTFILENAME"), fname.GetFullPath());
    }

    /* Aui Settings */
    m_inisettings->SetStringSetting(wxT("AUIPERSPECTIVE"), m_mgr.SavePerspective());

    // prevent values being saved while window is in an iconised state.
    if (this->IsIconized()) this->Restore();

    int value_x = 0, value_y = 0;
    this->GetPosition(&value_x, &value_y);
    m_inisettings->SetIntSetting(wxT("ORIGINX"), value_x);
    m_inisettings->SetIntSetting(wxT("ORIGINY"), value_y);

    int value_w = 0, value_h = 0;
    this->GetSize(&value_w, &value_h);
    m_inisettings->SetIntSetting(wxT("SIZEW"), value_w);
    m_inisettings->SetIntSetting(wxT("SIZEH"), value_h);
    m_inisettings->SetIntSetting(wxT("ISMAXIMIZED"), this->IsMaximized());
}
//----------------------------------------------------------------------------

void mmGUIFrame::menuEnableItems(bool enable)
{
    menuBar_->FindItem(MENU_SAVE_AS)->Enable(enable);
    menuBar_->FindItem(MENU_EXPORT)->Enable(enable);
    if (mmIniOptions::instance().enableAddAccount_)
        menuBar_->FindItem(MENU_NEWACCT)->Enable(enable);
    menuBar_->FindItem(MENU_ACCTLIST)->Enable(enable);
    menuBar_->FindItem(MENU_ACCTEDIT)->Enable(enable);
    if (mmIniOptions::instance().enableDeleteAccount_)
        menuBar_->FindItem(MENU_ACCTDELETE)->Enable(enable);

    menuBar_->FindItem(MENU_ORGCATEGS)->Enable(enable);
    menuBar_->FindItem(MENU_ORGPAYEE)->Enable(enable);
    menuBar_->FindItem(MENU_CATEGORY_RELOCATION)->Enable(enable);
    menuBar_->FindItem(MENU_PAYEE_RELOCATION)->Enable(enable);
    menuBar_->FindItem(MENU_CONVERT_ENC_DB)->Enable(enable);

    menuBar_->FindItem(MENU_IMPORT)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT_PREVIEW)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT)->Enable(enable);
    menuBar_->FindItem(wxID_PREFERENCES)->Enable(enable);
    if (mmIniOptions::instance().enableRepeatingTransactions_)
        menuBar_->FindItem(MENU_BILLSDEPOSITS)->Enable(enable);
    menuBar_->FindItem(MENU_CURRENCY)->Enable(enable);
    if (mmIniOptions::instance().enableAssets_)
        menuBar_->FindItem(MENU_ASSETS)->Enable(enable);
    if (mmIniOptions::instance().enableBudget_)
        menuBar_->FindItem(MENU_BUDGETSETUPDIALOG)->Enable(enable);
    menuBar_->FindItem(MENU_TRANSACTIONREPORT)->Enable(enable);

    if (mmIniOptions::instance().enableAddAccount_)
        toolBar_->EnableTool(MENU_NEWACCT, enable);
    toolBar_->EnableTool(MENU_ACCTLIST, enable);
    toolBar_->EnableTool(MENU_ORGPAYEE, enable);
    toolBar_->EnableTool(MENU_ORGCATEGS, enable);
    toolBar_->EnableTool(MENU_CURRENCY, enable);
}
//----------------------------------------------------------------------------

void mmGUIFrame::menuPrintingEnable(bool enable)
{
    menuBar_->FindItem(MENU_PRINT_PREVIEW)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT)->Enable(enable);
    menuBar_->FindItem(MENU_EXPORT_HTML)->Enable(enable);
}
//----------------------------------------------------------------------------

void mmGUIFrame::createControls()
{
#if defined (__WXGTK__) || defined (__WXMAC__)
    // Under GTK, row lines look ugly
    navTreeCtrl_ = new wxTreeCtrl( this, ID_NAVTREECTRL,
        wxDefaultPosition, wxSize(100, 100));
#else
    navTreeCtrl_ = new wxTreeCtrl( this, ID_NAVTREECTRL,
        wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE | wxTR_HAS_BUTTONS | wxTR_ROW_LINES );
#endif

    navTreeCtrl_->AssignImageList(navtree_images_list_());

    homePanel_ = new wxPanel( this, ID_PANEL,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxTR_SINGLE | wxNO_BORDER);

    m_mgr.AddPane(navTreeCtrl_, wxAuiPaneInfo().
        Name(wxT("Navigation")).Caption(_("Navigation")).
        BestSize(wxSize(200,100)).MinSize(wxSize(100,100)).
        Left());

    m_mgr.AddPane(homePanel_, wxAuiPaneInfo().
        Name(wxT("Home")).Caption(wxT("Home")).
        CenterPane().PaneBorder(false));
}
//----------------------------------------------------------------------------

void mmGUIFrame::updateNavTreeControl(bool expandTermAccounts)
{
    activeTermAccounts_ = false;
    // if no database is present yet, ignore testing for Term Accounts
    if (m_db && m_core->accountList_.has_term_account())
    {
        activeTermAccounts_ = true;
    }

    navTreeCtrl_->SetEvtHandlerEnabled(false);
    navTreeCtrl_->DeleteAllItems();
    //navTreeCtrl_->SetBackgroundColour(mmColors::navTreeBkColor);

    wxTreeItemId root = navTreeCtrl_->AddRoot(_("Home Page"), 0, 0);
    navTreeCtrl_->SetItemData(root, new mmTreeItemData(wxT("Home Page")));
    navTreeCtrl_->SetItemBold(root, true);
    navTreeCtrl_->SetFocus();


    wxTreeItemId accounts = navTreeCtrl_->AppendItem(root, _("Bank Accounts"), 9, 9);
    navTreeCtrl_->SetItemData(accounts, new mmTreeItemData(wxT("Bank Accounts")));
    navTreeCtrl_->SetItemBold(accounts, true);

    wxTreeItemId termAccount;
    if ( hasActiveTermAccounts() )
    {
    //  Positioning for new type of accounts: Term Accounts
        termAccount = navTreeCtrl_->AppendItem(root, _("Term Accounts"), 12, 12);
        navTreeCtrl_->SetItemData(termAccount, new mmTreeItemData(wxT("Term Accounts")));
        navTreeCtrl_->SetItemBold(termAccount, true);
    }

    wxTreeItemId stocks;
    stocks = navTreeCtrl_->AppendItem(root, _("Stocks"), 15, 15);
    navTreeCtrl_->SetItemData(stocks, new mmTreeItemData(wxT("Stocks")));
    navTreeCtrl_->SetItemBold(stocks, true);

    if (mmIniOptions::instance().enableAssets_)
    {
        wxTreeItemId assets = navTreeCtrl_->AppendItem(root, _("Assets"), 7, 7);
        navTreeCtrl_->SetItemData(assets, new mmTreeItemData(wxT("Assets")));
        navTreeCtrl_->SetItemBold(assets, true);
    }

    if (mmIniOptions::instance().enableRepeatingTransactions_)
    {
       wxTreeItemId bills = navTreeCtrl_->AppendItem(root, _("Repeating Transactions"), 2, 2);
       navTreeCtrl_->SetItemData(bills, new mmTreeItemData(wxT("Bills & Deposits")));
       navTreeCtrl_->SetItemBold(bills, true);
    }

    wxTreeItemId budgeting;
    if (mmIniOptions::instance().enableBudget_)
    {
        budgeting = navTreeCtrl_->AppendItem(root, _("Budget Setup"), 3, 3);
        navTreeCtrl_->SetItemData(budgeting, new mmTreeItemData(wxT("Budgeting")));
        navTreeCtrl_->SetItemBold(budgeting, true);
    }

    wxTreeItemId reports = navTreeCtrl_->AppendItem(root, _("Reports"), 4, 4);
    navTreeCtrl_->SetItemBold(reports, true);
    navTreeCtrl_->SetItemData(reports, new mmTreeItemData(NAVTREECTRL_REPORTS));

    /* ================================================================================================= */
    if (custRepIndex_->HasActiveReports())
    {
        wxTreeItemId customSqlReports = navTreeCtrl_->AppendItem(root, _("Custom Reports"), 8, 8);
        navTreeCtrl_->SetItemBold(customSqlReports, true);
        navTreeCtrl_->SetItemData(customSqlReports, new mmTreeItemData(NAVTREECTRL_CUSTOM_REPORTS));

        int reportNumber = -1;
        wxString reportNumberStr;
        wxTreeItemId customSqlReportRootItem;
        custRepIndex_->ResetReportsIndex();

        wxString reportTitle = custRepIndex_->NextReportTitle();
        while (custRepIndex_->ValidTitle())
        {
            wxTreeItemId customSqlReportItem;
            if (custRepIndex_->ReportIsSubReport() && reportNumber >= 0 )
            {
                customSqlReportItem = navTreeCtrl_->AppendItem(customSqlReportRootItem,reportTitle, 8, 8);
            }
            else
            {
                customSqlReportItem = navTreeCtrl_->AppendItem(customSqlReports,reportTitle, 8, 8);
                customSqlReportRootItem = customSqlReportItem;
            }
            reportNumberStr.Printf(wxT("Custom_Report_%d"), ++reportNumber);
            navTreeCtrl_->SetItemData(customSqlReportItem, new mmTreeItemData(reportNumberStr));
            reportTitle = custRepIndex_->NextReportTitle();
        }

        if (expandedCustomSqlReportNavTree_)
            navTreeCtrl_->Expand(customSqlReports);
    }

    /* ================================================================================================= */

    wxTreeItemId reportsSummary = navTreeCtrl_->AppendItem(reports, _("Summary of Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(reportsSummary, new mmTreeItemData(wxT("Summary of Accounts"), new mmReportSummary(m_core.get(), this)));

    wxTreeItemId reportsStocks = navTreeCtrl_->AppendItem(reportsSummary, _("Stocks"), 4, 4);
    navTreeCtrl_->SetItemData(reportsStocks, new mmTreeItemData(wxT("Summary of Stocks"), new mmReportSummaryStocks(m_core.get())));

    if (mmIniOptions::instance().enableAssets_)
    {
        wxTreeItemId reportsAssets = navTreeCtrl_->AppendItem(reportsSummary, _("Assets"), 4, 4);
        navTreeCtrl_->SetItemData(reportsAssets, new mmTreeItemData(wxT("Summary of Assets"), new mmReportSummaryAssets(m_core.get())));
    }

    wxTreeItemId categsOverTime = navTreeCtrl_->AppendItem(reports, _("Where the Money Goes"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTime, new mmTreeItemData(wxT("Where the Money Goes"), new mmReportCategoryExpensesGoes(m_core.get())));

    wxTreeItemId categsOverTimeCalMonth = navTreeCtrl_->AppendItem(categsOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCalMonth, new mmTreeItemData(wxTRANSLATE("Where the Money Goes - Last Calendar Month"), new mmReportCategoryExpensesGoesLastMonth(m_core.get())));

    wxString currentMonthMsg = _("Current Month");
    if (mmIniOptions::instance().ignoreFutureTransactions_) currentMonthMsg = _("Current Month to Date");

    wxTreeItemId categsOverTimeCurrentMonth = navTreeCtrl_->AppendItem(categsOverTime, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentMonth, new mmTreeItemData(wxTRANSLATE("Where the Money Goes - Current Month"), new mmReportCategoryExpensesGoesCurrentMonth(m_core.get())));

    wxTreeItemId categsOverTimeLast30 = navTreeCtrl_->AppendItem(categsOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLast30, new mmTreeItemData(wxTRANSLATE("Where the Money Goes - 30 Days"), new mmReportCategoryExpensesGoesLast30Days(m_core.get())));

    wxTreeItemId categsOverTimeLastYear = navTreeCtrl_->AppendItem(categsOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLastYear, new mmTreeItemData(wxTRANSLATE("Where the Money Goes - Last Year"), new mmReportCategoryExpensesGoesLastYear(m_core.get())));

    wxTreeItemId categsOverTimeCurrentYear = navTreeCtrl_->AppendItem(categsOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentYear, new mmTreeItemData(wxTRANSLATE("Where the Money Goes - Current Year"), new mmReportCategoryExpensesGoesCurrentYear(m_core.get())));

    if (financialYearIsDifferent())
    {
        wxTreeItemId categsOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(categsOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsOverTimeLastFinancialYear, new mmTreeItemData(wxTRANSLATE("Where the Money Goes - Last Financial Year")
                    , new mmReportCategoryExpensesGoesLastFinancialYear(m_core.get())));

        wxTreeItemId categsOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(categsOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsOverTimeCurrentFinancialYear, new mmTreeItemData(wxTRANSLATE("Where the Money Goes - Current Financial Year")
                    , new mmReportCategoryExpensesGoesCurrentFinancialYear(m_core.get())));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId posCategs = navTreeCtrl_->AppendItem(reports, _("Where the Money Comes From"), 4, 4);
    navTreeCtrl_->SetItemData(posCategs, new mmTreeItemData(wxT("Where the Money Comes From"), new mmReportCategoryExpensesComesCurrentMonth(m_core.get())));

    wxTreeItemId posCategsCalMonth = navTreeCtrl_->AppendItem(posCategs, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsCalMonth, new mmTreeItemData(wxTRANSLATE("Where the Money Comes From - Last Calendar Month"), new mmReportCategoryExpensesComesLastMonth(m_core.get())));

    wxTreeItemId posCategsCurrentMonth = navTreeCtrl_->AppendItem(posCategs, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(posCategsCurrentMonth, new mmTreeItemData(wxTRANSLATE("Where the Money Comes From - Current Month"), new mmReportCategoryExpensesComesCurrentMonth(m_core.get())));

    wxTreeItemId posCategsTimeLast30 = navTreeCtrl_->AppendItem(posCategs, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLast30, new mmTreeItemData(wxTRANSLATE("Where the Money Comes From - 30 Days"), new mmReportCategoryExpensesComesLast30Days(m_core.get())));

    wxTreeItemId posCategsTimeLastYear = navTreeCtrl_->AppendItem(posCategs, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLastYear, new mmTreeItemData(wxTRANSLATE("Where the Money Comes From - Last Year"), new mmReportCategoryExpensesComesLastYear(m_core.get())));

    wxTreeItemId posCategsTimeCurrentYear = navTreeCtrl_->AppendItem(posCategs, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeCurrentYear, new mmTreeItemData(wxTRANSLATE("Where the Money Comes From - Current Year"), new mmReportCategoryExpensesComesCurrentYear(m_core.get())));

    if (financialYearIsDifferent())
    {
        wxTreeItemId posCategsTimeLastFinancialYear = navTreeCtrl_->AppendItem(posCategs, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(posCategsTimeLastFinancialYear, new mmTreeItemData(wxTRANSLATE("Where the Money Comes From - Last Financial Year")
                    , new mmReportCategoryExpensesComesLastFinancialYear(m_core.get())));

        wxTreeItemId posCategsTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(posCategs, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(posCategsTimeCurrentFinancialYear, new mmTreeItemData(wxTRANSLATE("Where the Money Comes From - Current Financial Year")
                    , new mmReportCategoryExpensesComesCurrentFinancialYear(m_core.get())));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId categs = navTreeCtrl_->AppendItem(reports, _("Categories"), 4, 4);
    navTreeCtrl_->SetItemData(categs, new mmTreeItemData(wxT("Categories - Over Time")));

    wxTreeItemId categsCalMonth = navTreeCtrl_->AppendItem(categs, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsCalMonth, new mmTreeItemData(wxTRANSLATE("Categories - Last Calendar Month")));

    wxTreeItemId categsCurrentMonth = navTreeCtrl_->AppendItem(categs, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(categsCurrentMonth, new mmTreeItemData(wxTRANSLATE("Categories - Current Month")));

    wxTreeItemId categsTimeLast30 = navTreeCtrl_->AppendItem(categs, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLast30, new mmTreeItemData(wxTRANSLATE("Categories - 30 Days")));

    wxTreeItemId categsTimeLastYear = navTreeCtrl_->AppendItem(categs, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLastYear, new mmTreeItemData(wxTRANSLATE("Categories - Last Year")));

    wxTreeItemId categsTimeCurrentYear = navTreeCtrl_->AppendItem(categs, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeCurrentYear, new mmTreeItemData(wxTRANSLATE("Categories - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId categsTimeLastFinancialYear = navTreeCtrl_->AppendItem(categs, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsTimeLastFinancialYear, new mmTreeItemData(wxTRANSLATE("Categories - Last Financial Year")));

        wxTreeItemId categsTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(categs, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsTimeCurrentFinancialYear, new mmTreeItemData(wxTRANSLATE("Categories - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId payeesOverTime = navTreeCtrl_->AppendItem(reports, _("Payees"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTime, new mmTreeItemData(wxT("Payee Report")));

    wxTreeItemId payeesOverTimeCalMonth = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCalMonth, new mmTreeItemData(wxTRANSLATE("Payees - Last Calendar Month")));

    wxTreeItemId payeesOverTimeCurrentMonth = navTreeCtrl_->AppendItem(payeesOverTime, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentMonth, new mmTreeItemData(wxTRANSLATE("Payees - Current Month")));

    wxTreeItemId payeesOverTimeLast30 = navTreeCtrl_->AppendItem(payeesOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLast30, new mmTreeItemData(wxTRANSLATE("Payees - Last 30 Days")));

    wxTreeItemId payeesOverTimeLastYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLastYear, new mmTreeItemData(wxTRANSLATE("Payees - Last Year")));

    wxTreeItemId payeesOverTimeCurrentYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentYear, new mmTreeItemData(wxTRANSLATE("Payees - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId payeesOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(payeesOverTimeLastFinancialYear, new mmTreeItemData(wxTRANSLATE("Payees - Last Financial Year")));

        wxTreeItemId payeesOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(payeesOverTimeCurrentFinancialYear, new mmTreeItemData(wxTRANSLATE("Payees - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////////////

    wxTreeItemId incexpOverTime = navTreeCtrl_->AppendItem(reports, _("Income vs Expenses"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTime, new mmTreeItemData(wxT("Income vs Expenses")));

    wxTreeItemId incexpOverTimeCalMonth = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCalMonth, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - Last Calendar Month")));

    wxTreeItemId incexpOverTimeCurrentMonth = navTreeCtrl_->AppendItem(incexpOverTime, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentMonth, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - Current Month")));

    wxTreeItemId incexpOverTimeLast30 = navTreeCtrl_->AppendItem(incexpOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLast30, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - 30 Days")));

    wxTreeItemId incexpOverTimeLastYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLastYear, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - Last Year")));

    wxTreeItemId incexpOverTimeCurrentYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentYear, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId incexpOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(incexpOverTimeLastFinancialYear, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - Last Financial Year")));

        wxTreeItemId incexpOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(incexpOverTimeCurrentFinancialYear, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - Current Financial Year")));
    }

    wxTreeItemId incexpmonthly = navTreeCtrl_->AppendItem(incexpOverTime, _("All Time"), 4, 4);
    navTreeCtrl_->SetItemData(incexpmonthly, new mmTreeItemData(wxTRANSLATE("Income vs Expenses - All Time")));

    //////////////////////////////////////////////////////////////////
    wxTreeItemId transactionList = navTreeCtrl_->AppendItem(reports, _("Transaction Report"), 4, 4);
    navTreeCtrl_->SetItemData(transactionList, new mmTreeItemData(wxT("Transaction Report")));

    ///////////////////////////////////////////////////////////////////

    if (m_db && mmIniOptions::instance().enableBudget_)
    {

        wxTreeItemId budgetPerformance;
        wxTreeItemId budgetSetupPerformance;

        wxSQLite3ResultSet q1 = m_db->ExecuteQuery(SELECT_ALL_FROM_BUDGETYEAR_V1);

        for (size_t i = 0; q1.NextRow(); ++i)
        {
            if (!i) { // first loop only
                budgetPerformance = navTreeCtrl_->AppendItem(reports, _("Budget Performance"), 4, 4);
                navTreeCtrl_->SetItemData(budgetPerformance, new mmTreeItemData(wxT("Budget Performance")));

                budgetSetupPerformance = navTreeCtrl_->AppendItem(reports, _("Budget Category Summary"), 4, 4);
                navTreeCtrl_->SetItemData(budgetSetupPerformance, new mmTreeItemData(wxT("Budget Setup Performance")));
            }

            int id = q1.GetInt(wxT("BUDGETYEARID"));
            const wxString name = q1.GetString(wxT("BUDGETYEARNAME"));

            wxTreeItemId bYear = navTreeCtrl_->AppendItem(budgeting, name, 3, 3);
            navTreeCtrl_->SetItemData(bYear, new mmTreeItemData(id, true));

            // Only add YEARS for Budget Performance
            if (name.length() < 5)
            {
                wxTreeItemId bYearData = navTreeCtrl_->AppendItem(budgetPerformance, name, 4, 4);
                navTreeCtrl_->SetItemData(bYearData, new mmTreeItemData(id, true));
            }
            wxTreeItemId bYearSetupData = navTreeCtrl_->AppendItem(budgetSetupPerformance, name, 4, 4);
            navTreeCtrl_->SetItemData(bYearSetupData, new mmTreeItemData(id, true));
        }

        q1.Finalize();

        //TODO: Set up as a permanent user option
        if (expandedBudgetingNavTree_)
            navTreeCtrl_->Expand(budgeting);
    }

    ///////////////////////////////////////////////////////////////////
    wxTreeItemId cashFlow = navTreeCtrl_->AppendItem(reports, _("Cash Flow"), 4, 4);
    navTreeCtrl_->SetItemData(cashFlow, new mmTreeItemData(wxT("Cash Flow")));

    wxTreeItemId cashflowWithBankAccounts = navTreeCtrl_->AppendItem(cashFlow, _("Cash Flow - With Bank Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(cashflowWithBankAccounts, new mmTreeItemData(wxT("Cash Flow - With Bank Accounts")));

    if ( hasActiveTermAccounts() )
    {
        wxTreeItemId cashflowWithTermAccounts = navTreeCtrl_->AppendItem(cashFlow, _("Cash Flow - With Term Accounts"), 4, 4);
        navTreeCtrl_->SetItemData(cashflowWithTermAccounts, new mmTreeItemData(wxT("Cash Flow - With Term Accounts")));
    }

    wxTreeItemId cashflowSpecificAccounts = navTreeCtrl_->AppendItem(cashFlow, _("Cash Flow - Specific Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(cashflowSpecificAccounts, new mmTreeItemData(wxT("Cash Flow - Specific Accounts")));


    wxTreeItemId cashflowSpecificAccountsDaily = navTreeCtrl_->AppendItem(cashFlow, _("Daily Cash Flow - Specific Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(cashflowSpecificAccountsDaily, new mmTreeItemData(wxT("Daily Cash Flow - Specific Accounts")));

    ///////////////////////////////////////////////////////
    wxTreeItemId transactionStats = navTreeCtrl_->AppendItem(reports, _("Transaction Statistics"), 4, 4);
    navTreeCtrl_->SetItemData(transactionStats, new mmTreeItemData(wxT("Transaction Statistics")));

     ///////////////////////////////////////////////////////////////////

    wxTreeItemId help = navTreeCtrl_->AppendItem(root, _("Help"), 5, 5);
    navTreeCtrl_->SetItemData(help, new mmTreeItemData(NAVTREECTRL_HELP));
    navTreeCtrl_->SetItemBold(help, true);

     /* Start Populating the dynamic data */
    navTreeCtrl_->Expand(root);
    if (expandedReportNavTree_)
        navTreeCtrl_->Expand(reports);

    if (!m_db)
       return;

    /* Load Nav Tree Control */

    wxString vAccts = m_inisettings->GetStringSetting(wxT("VIEWACCOUNTS"), wxT("ALL"));

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->accountList_.range();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* account = it->get();
        // Checking/Bank Accounts
        if (account->acctType_ == ACCOUNT_TYPE_BANK)
        {
            const mmAccount* pCA = account;
            if ((vAccts == wxT("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pCA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
            {
                int selectedImage = mmIniOptions::instance().account_image_id(m_core.get(), pCA->id_);
                wxTreeItemId tacct = navTreeCtrl_->AppendItem(accounts, pCA->name_, selectedImage, selectedImage);
                navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(pCA->id_, false));
            }
        }
        // Term Accounts
        else if (account->acctType_ == ACCOUNT_TYPE_TERM)
        {
           const mmAccount* pTA = account;
            if ((vAccts == wxT("Open") && pTA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pTA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
            {
                int selectedImage = mmIniOptions::instance().account_image_id(m_core.get(), pTA->id_);
                wxTreeItemId tacct = navTreeCtrl_->AppendItem(termAccount, pTA->name_, selectedImage, selectedImage);
                navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(pTA->id_, false));
            }
        }
        // Stock Accounts
        else //if (account->acctType_ == ACCOUNT_TYPE_STOCK)
        {
            const mmAccount* pIA = account;
            if ((vAccts == wxT("Open") && pIA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pIA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
            {
                int selectedImage = mmIniOptions::instance().account_image_id(m_core.get(), pIA->id_);
                wxTreeItemId tacct = navTreeCtrl_->AppendItem(stocks, pIA->name_, selectedImage, selectedImage);
                navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(pIA->id_, false));
            }
        }
    }

    if (mmIniOptions::instance().expandBankTree_)
        navTreeCtrl_->Expand(accounts);

    if ( hasActiveTermAccounts() )
    {
        menuBar_->FindItem(MENU_VIEW_TERMACCOUNTS)->Enable(true);
        if (mmIniOptions::instance().expandTermTree_ || expandTermAccounts)
            navTreeCtrl_->Expand(termAccount);
    } else
        menuBar_->FindItem(MENU_VIEW_TERMACCOUNTS)->Enable(false);

    navTreeCtrl_->SetEvtHandlerEnabled(true);
}
//----------------------------------------------------------------------------

void mmGUIFrame::CreateCustomReport(int index)
{
    this->SetEvtHandlerEnabled(false);
    homePageAccountSelect_ = true; // prevent Navigation tree code execution.
    wxBeginBusyCursor(wxHOURGLASS_CURSOR);

    if (custRepIndex_->ReportFileName(index) != wxT(""))
    {
        wxString sScript;
        if (custRepIndex_->GetReportFileData(sScript) )
        {
            mmCustomReport* csr = new mmCustomReport(this, m_core.get()
                , custRepIndex_->CurrentReportTitle()
                , sScript
                , custRepIndex_->CurrentReportFileType());
            createReportsPage(csr);
        }
    }
    processPendingEvents();         // clear out pending events
    this->SetEvtHandlerEnabled(true);
    homePageAccountSelect_ = false; // restore Navigation tree code execution.
    wxEndBusyCursor();
}
//----------------------------------------------------------------------------

bool mmGUIFrame::IsCustomReportSelected( int& customSqlReportID, mmTreeItemData* iData )
{
    customSqlReportID = 0;
    bool result = false;
    wxString sItemName = iData->getString();
    if (wxNOT_FOUND != sItemName.Index(wxT("Custom_Report_")))
    {
        sItemName.Replace(wxT("Custom_Report_"), wxT(""));
        long index;
        result = sItemName.ToLong(&index);
        if (result) customSqlReportID = index;
    }
    return result;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnTreeItemExpanded(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(id));

    if (iData->getString() == NAVTREECTRL_REPORTS)
        expandedReportNavTree_ = true;
    else if (iData->getString() == NAVTREECTRL_CUSTOM_REPORTS)
        expandedCustomSqlReportNavTree_ = true;
    else if (iData->getString() == NAVTREECTRL_BUDGET)
        expandedBudgetingNavTree_ = true;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnTreeItemCollapsed(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(id));

    if (iData->getString() == NAVTREECTRL_REPORTS)
        expandedReportNavTree_ = false;
    else if (iData->getString() == NAVTREECTRL_CUSTOM_REPORTS)
        expandedCustomSqlReportNavTree_ = false;
    else if (iData->getString() == NAVTREECTRL_BUDGET)
        expandedBudgetingNavTree_ = false;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnSelChanged(wxTreeEvent& event)
{
    /* Because Windows generates 2 events when selecting the navTree, and Linux
       does not, we need to be able to control when the event is actually executed.
       This ensures that only one event activates the account for all systems. */
    if (homePageAccountSelect_) return;

    menuPrintingEnable(false);
    wxTreeItemId id = event.GetItem();
    if (!id) return;

    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(id));
    selectedItemData_ = iData;

    if (!iData) return;

    if (!iData->isStringData())
    {
        int data = iData->getData();
        if (iData->isBudgetingNode())
        {
            wxString reportWaitingMsg = _("Budget report being generated... Please wait.");
            int year = data;

            wxTreeItemId idparent = navTreeCtrl_->GetItemParent(id);
            mmTreeItemData* iParentData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(idparent));
            if (iParentData->getString() == wxT("Budget Performance"))
            {
                int year = data;
                mmPrintableBase* rs = new mmReportBudgetingPerformance(m_core.get(), this, year);
                createReportsPage(rs);
            }
            else if (iParentData->getString() == wxT("Budget Setup Performance"))
            {
                mmPrintableBase* rs = new mmReportBudgetCategorySummary(m_core.get(), this, year);
                createReportsPage(rs);
            }
            else
            {
                createBudgetingPage(year);
            }
        }
        else
        {
           wxSharedPtr<mmAccount> pAccount = m_core->accountList_.GetAccountSharedPtr(data);
           if (pAccount)
           {
                wxString acctType = pAccount->acctType_;

                if ((acctType == ACCOUNT_TYPE_BANK) || acctType == ACCOUNT_TYPE_TERM)
                {
                    gotoAccountID_ = data;
                    if (gotoAccountID_ != -1) createCheckingAccountPage(gotoAccountID_);
                    navTreeCtrl_->SetFocus();
                }
                else
                {
                    wxSizer *sizer = cleanupHomePanel();

                    panelCurrent_ = new mmStocksPanel(m_core.get(), data, homePanel_, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
                    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

                    homePanel_->Layout();
                }
            }
            else
            {
                /* cannot find accountid */
                wxASSERT(true);
            }
        }
    }
    else
    {
        if (iData->getString() == wxT("Home Page"))
        {
            createHomePage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_HELP)
        {
            helpFileIndex_ = mmex::HTML_INDEX;
            createHelpPage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_CUSTOM_REPORTS)
        {
            helpFileIndex_ = mmex::HTML_CUSTOM_SQL;
            createHelpPage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_INVESTMENT)
        {
            helpFileIndex_ = mmex::HTML_INVESTMENT;
            createHelpPage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_BUDGET)
        {
            helpFileIndex_ = mmex::HTML_BUDGET;
            createHelpPage();
            return;
        }

        if (!m_core || !m_db)
            return;

        //========================================================================
        int customReportID;      // Define before all the if...else statements
        //========================================================================

        wxDateTime dtBegin, dtEnd;
        wxString sData = iData->getString();
        wxString title = wxGetTranslation(sData);
        bool bIgnoreFuture = mmIniOptions::instance().ignoreFutureTransactions_;

        if ( IsCustomReportSelected(customReportID, iData) )
        {
            CreateCustomReport(customReportID);
        }
        else if (sData == wxT("Summary of Accounts"))
        {
            createReportsPage(iData->get_report());
        }
        else if (sData == wxT("Summary of Stocks"))
        {
            createReportsPage(iData->get_report());
        }
        else if (sData == wxT("Summary of Assets"))
        {
            createReportsPage(iData->get_report());
        }
        else if (sData == wxT("Where the Money Goes"))
        {
            createReportsPage(iData->get_report());
        }
        else if (sData.StartsWith(wxT("Where the Money Goes -")))
        {
            createReportsPage(iData->get_report());
        }
        else if (sData == wxT("Where the Money Comes From"))
        {
            createReportsPage(iData->get_report());
        }
        else if (sData.StartsWith(wxT("Where the Money Comes From - ")))
        {
            createReportsPage(iData->get_report());
        }
        else if (sData == wxT("Categories - Over Time"))
        {
            wxProgressDialog proDlg(_("Category Report"), _("Category Report being generated... Please wait."), 100, this);
            mmPrintableBase* rs = new mmReportCategoryOverTimePerformance(m_core.get());
            proDlg.Update(70);
            createReportsPage(rs);
            proDlg.Update(95);
        }
        else if (sData.StartsWith(wxT("Categories - ")))
        {
            GetDateRange(dtBegin, dtEnd, sData);

            if (bIgnoreFuture && sData == wxT("Categories - Current Month"))
                title = _("Current Month to Date");

            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get()
                , false, dtBegin, dtEnd, title, 0);
            createReportsPage(rs);
        }
        else if (sData == wxT("Payee Report"))
        {
            wxString title = _("Payee Report");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get()
                , true, wxDateTime::Now(), wxDateTime::Now(), title);
            createReportsPage(rs);
        }
        else if (sData.StartsWith(wxT("Payees - ")))
        {
            GetDateRange(dtBegin, dtEnd, sData);

            if (bIgnoreFuture && wxT("Payees - Current Month"))
                title = _("Payees - Current Month to Date");

            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get()
                , false, dtBegin, dtEnd, title);
            createReportsPage(rs);

        }
        else if (sData == wxT("Income vs Expenses - All Time"))
        {
            mmPrintableBase* rs = new mmReportIncomeExpensesAllTime(m_core.get());
            createReportsPage(rs);
        }
        else if (sData == wxT("Income vs Expenses"))
        {
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(m_core.get()
                , wxDateTime::Now().GetYear(), 10);

            createReportsPage(rs);
        }
        else if (sData.StartsWith(wxT("Income vs Expenses - ")) && sData.Contains(wxT("Year")))
        {
            mmPrintableBase* rs;
            GetDateRange(dtBegin, dtEnd, sData);

            int year = wxDateTime::Now().GetYear();
            if (sData.Contains(wxT("Last"))) year --;

            if (sData.Contains(wxT("Financial")))
            {
                if (wxDateTime::Now().GetMonth() < dtBegin.GetMonth()) year -- ;
                rs = new mmReportIncExpensesOverFinancialPeriod(m_core.get(), year);
            }
            else
            {
                rs = new mmReportIncExpensesOverTime(m_core.get(), year);
            }
            createReportsPage(rs);
        }
        else if (sData.StartsWith(wxT("Income vs Expenses - ")))
        {
            mmPrintableBase* rs = new mmReportIncomeExpensesCurrentMonth(m_core.get());
            createReportsPage(rs);
        }
        else if (sData == wxT("Cash Flow"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this, 0);

            report->activateBankAccounts();
            if (hasActiveTermAccounts())  report->activateTermAccounts();

            createReportsPage(report);
        }
        else if (sData == wxT("Cash Flow - With Bank Accounts"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this, 0);
            report->activateBankAccounts();
            createReportsPage(report);
        }

        else if (sData == wxT("Cash Flow - With Term Accounts"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this, 0);
            report->activateTermAccounts();
            createReportsPage(report);
        }
        else if (sData == wxT("Cash Flow - Specific Accounts"))
        {
            OnCashFlowSpecificAccounts(0);
        }
        else if (sData == wxT("Daily Cash Flow - Specific Accounts"))
        {
            OnCashFlowSpecificAccounts(1);
        }
        else if (sData == wxT("Transaction Report"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TRANSACTIONREPORT);
            AddPendingEvent(evt);           // Events will be processed in due course.
        }
        else if (sData == wxT("Bills & Deposits"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BILLSDEPOSITS);
            AddPendingEvent(evt);
        }
        else if (sData == wxT("Assets"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ASSETS);
            AddPendingEvent(evt);
        }
        else if (sData == wxT("Transaction Statistics"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportTransactionStats(m_core.get(), year);
            createReportsPage(rs);
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnLaunchAccountWebsite(wxCommandEvent& /*event*/)
{
   if (selectedItemData_)
   {
      int data = selectedItemData_->getData();
      wxSharedPtr<mmAccount> pAccount = m_core->accountList_.GetAccountSharedPtr(data);
      if (pAccount)
      {
         wxString website = pAccount->website_;
         if (!website.IsEmpty()) wxLaunchDefaultBrowser(website);
         return;
      }
   }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPopupEditAccount(wxCommandEvent& /*event*/)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        wxSharedPtr<mmAccount> pAccount = m_core->accountList_.GetAccountSharedPtr(data);
        if (pAccount)
        {
           wxString acctType = pAccount->acctType_;
           if (acctType == ACCOUNT_TYPE_BANK || acctType == ACCOUNT_TYPE_STOCK || acctType == ACCOUNT_TYPE_TERM)
           {
              mmNewAcctDialog dlg(m_core.get(), true, data, this);
              if ( dlg.ShowModal() == wxID_OK )
              {
                 createHomePage();
                 updateNavTreeControl();
              }
           }
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPopupDeleteAccount(wxCommandEvent& /*event*/)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        wxSharedPtr<mmAccount> pAccount = m_core->accountList_.GetAccountSharedPtr(data);
        if (pAccount)
        {
            wxMessageDialog msgDlg(this,
                _("Do you really want to delete the account?"),
                _("Confirm Account Deletion"),
                wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
            if (msgDlg.ShowModal() == wxID_YES)
            {
                m_core->bTransactionList_.deleteTransactions(pAccount->id_);
                m_core->accountList_.RemoveAccount(pAccount->id_);
                updateNavTreeControl();
                createHomePage();
            }
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnItemMenu(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    if (menuBar_->FindItem(MENU_ORGCATEGS)->IsEnabled() )
        showTreePopupMenu(id, event.GetPoint());
    else
        wxMessageBox(_("MMEX has been opened without an active database."),_("MMEX: Menu Popup Error"), wxOK|wxICON_EXCLAMATION);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnItemRightClick(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    navTreeCtrl_ ->SelectItem(id);
}
//----------------------------------------------------------------------------

void mmGUIFrame::showTreePopupMenu(wxTreeItemId id, const wxPoint& pt)
{
    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(id));
    selectedItemData_ = iData;

    if (!iData->isStringData())
    {
        int data = iData->getData();
        if (!iData->isBudgetingNode())
        {
            wxSharedPtr<mmAccount> pAccount = m_core->accountList_.GetAccountSharedPtr(data);
            if (pAccount)
            {
                wxString acctType = pAccount->acctType_;
                if (acctType == ACCOUNT_TYPE_BANK || acctType == ACCOUNT_TYPE_TERM || acctType == ACCOUNT_TYPE_STOCK)
                {
                    wxMenu menu;
//                  menu.Append(MENU_TREEPOPUP_GOTO, _("&Go To.."));
                    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Account"));
                    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Account"));
                    menu.AppendSeparator();
                    menu.Append(MENU_TREEPOPUP_LAUNCHWEBSITE, _("&Launch Account Website"));
                    // Enable menu item only if a website exists for the account.
                    bool webStatus = !pAccount->website_.IsEmpty();
                    menu.Enable(MENU_TREEPOPUP_LAUNCHWEBSITE, webStatus);

                    PopupMenu(&menu, pt);
                }
            }
        }
    }
    else
    {
        if (iData->getString() == wxT("Bank Accounts") ||
            iData->getString() == wxT("Term Accounts") ||
            iData->getString() == wxT("Stocks"))
        { // Create for Bank Term & Stock Accounts

         //wxMenu menu;
            /*Popup Menu for Bank Accounts*/
        //New Account    //
        //Delete Account //
        //Edit Account   //
        //Export >       //
            //CSV Files //
            //QIF Files //
        //Import >       //
            //CSV Files //
            //QIF Files           //
        //Accounts Visible//
            //All      //
            //Favorite //
            //Open     //

            wxMenu *menu = new wxMenu;
            menu->Append(MENU_TREEPOPUP_ACCOUNT_NEW, _("New &Account"));
            menu->Append(MENU_TREEPOPUP_ACCOUNT_DELETE, _("&Delete Account"));
            menu->Append(MENU_TREEPOPUP_ACCOUNT_EDIT, _("&Edit Account"));
            menu->Append(MENU_TREEPOPUP_ACCOUNT_LIST, _("Account &List (Home)"));
            menu->AppendSeparator();
            // menu->Append(menuItemOnlineUpdateCurRate_);
            // menu->AppendSeparator();

            // Create only for Bank Accounts
            if ( (iData->getString() != wxT("Term Accounts")) && (iData->getString() != wxT("Stocks")) )
            {
                wxMenu *exportTo = new wxMenu;
                exportTo->Append(MENU_TREEPOPUP_ACCOUNT_EXPORT2CSV, _("&CSV Files..."));
                exportTo->Append(MENU_TREEPOPUP_ACCOUNT_EXPORT2QIF, _("&QIF Files..."));
                menu->AppendSubMenu(exportTo,  _("&Export"));
                wxMenu *importFrom = new wxMenu;
                importFrom->Append(MENU_TREEPOPUP_ACCOUNT_IMPORTUNIVCSV, _("&CSV Files..."));
                importFrom->Append(MENU_TREEPOPUP_ACCOUNT_IMPORTQIF, _("&QIF Files..."));
                menu->AppendSubMenu(importFrom,  _("&Import"));
                menu->AppendSeparator();
            }

            wxMenu *viewAccounts = new wxMenu;
            viewAccounts->Append(MENU_TREEPOPUP_ACCOUNT_VIEWALL, _("All"));
            viewAccounts->Append(MENU_TREEPOPUP_ACCOUNT_VIEWOPEN, _("Open"));
            viewAccounts->Append(MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE, _("Favorites"));
            menu->AppendSubMenu(viewAccounts, _("Accounts Visible"));
            PopupMenu(&*menu, pt);
        }
        else
        {
            // Bring up popup menu to edit or delete the correct Custom Report
            customSqlReportSelectedItem_ = iData->getString();
            wxString field = customSqlReportSelectedItem_.Mid(6,8);
            if (field == wxT("_Report_"))
            {
                wxMenu* customReportMenu = new wxMenu;
                customReportMenu->Append(wxID_EDIT, _("Edit Custom Report"));
                PopupMenu(&*customReportMenu, pt);
            }
            else if (iData->getString() == wxT("Budgeting"))
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BUDGETSETUPDIALOG);
                AddPendingEvent(evt);
            }
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewAllAccounts(wxCommandEvent&)
{
    //Get current settings for view accounts
    wxString vAccts = m_inisettings->GetStringSetting(wxT("VIEWACCOUNTS"), wxT("ALL"));

    //Set view ALL
    m_inisettings->SetStringSetting(wxT("VIEWACCOUNTS"), wxT("ALL"));
    //Refresh Navigation Panel
    mmGUIFrame::updateNavTreeControl();

    //Restore settings
    m_inisettings->SetStringSetting(wxT("VIEWACCOUNTS"), vAccts);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewFavoriteAccounts(wxCommandEvent&)
{
    //Get current settings for view accounts
    wxString vAccts = m_inisettings->GetStringSetting(wxT("VIEWACCOUNTS"), wxT("ALL"));

    //Set view ALL
    m_inisettings->SetStringSetting(wxT("VIEWACCOUNTS"), wxT("Favorites"));

    //Refresh Navigation Panel
    mmGUIFrame::updateNavTreeControl();

    //Restore settings
    m_inisettings->SetStringSetting(wxT("VIEWACCOUNTS"), vAccts);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewOpenAccounts(wxCommandEvent&)
{
    //Get current settings for view accounts
    wxString vAccts = m_inisettings->GetStringSetting(wxT("VIEWACCOUNTS"), wxT("ALL"));

    //Set view ALL
    m_inisettings->SetStringSetting(wxT("VIEWACCOUNTS"), wxT("Open"));

    //Refresh Navigation Panel
    mmGUIFrame::updateNavTreeControl();

    //Restore settings
    m_inisettings->SetStringSetting(wxT("VIEWACCOUNTS"), vAccts);
}
//----------------------------------------------------------------------------

void mmGUIFrame::SetBudgetingPageInactive()
{
    activeBudgetingPage_ = false;
}
//----------------------------------------------------------------------------

void mmGUIFrame::createBudgetingPage(int budgetYearID)
{
    if (activeBudgetingPage_)
    {
        budgetingPage_->DisplayBudgetingDetails(budgetYearID);
    }
    else
    {
        wxSizer *sizer = cleanupHomePanel();

        budgetingPage_ = new mmBudgetingPanel(m_core.get(), this, budgetYearID,
            homePanel_, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        panelCurrent_ = budgetingPage_;
        activeBudgetingPage_ = true;

        sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
        homePanel_->Layout();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::createHomePage()
{
    wxSizer *sizer = cleanupHomePanel();

    if (panelCurrent_)
    {
        //panelCurrent_->DestroyChildren();
        //panelCurrent_->SetSizer(NULL);
        panelCurrent_  = 0;
    }
    panelCurrent_ = new mmHomePagePanel(this,
        m_core.get(),
        m_topCategories,
        homePanel_,
        wxID_STATIC,
        wxDefaultPosition,
        wxDefaultSize,
        wxNO_BORDER|wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel_->Layout();
    refreshRequested_ = false;
}
//----------------------------------------------------------------------------

void mmGUIFrame::createReportsPage(mmPrintableBase* rs)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmReportsPanel(this, m_core.get(), rs, homePanel_, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel_->Layout();
    menuPrintingEnable(true);
}
//----------------------------------------------------------------------------

void mmGUIFrame::createHelpPage()
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmHelpPanel(this, m_db.get(), homePanel_, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel_->Layout();
    menuPrintingEnable(true);
}
//----------------------------------------------------------------------------

void mmGUIFrame::createMenu()
{
    wxBitmap toolBarBitmaps[11];
    toolBarBitmaps[0] = wxBitmap(new_xpm);
    toolBarBitmaps[1] = wxBitmap(open_xpm);
    toolBarBitmaps[2] = wxBitmap(save_xpm);
    toolBarBitmaps[3] = wxBitmap(newacct_xpm);
    toolBarBitmaps[4] = wxBitmap(house_xpm);
    toolBarBitmaps[5] = wxBitmap(print_xpm);
    toolBarBitmaps[6] = wxBitmap(printpreview_xpm);
    toolBarBitmaps[7] = wxBitmap(printsetup_xpm);
    toolBarBitmaps[8] = wxBitmap(edit_account_xpm);
    toolBarBitmaps[9] = wxBitmap(delete_account_xpm);

    wxMenu *menu_file = new wxMenu;

    wxMenuItem* menuItemNew = new wxMenuItem(menu_file, MENU_NEW,_("&New Database\tCtrl-N"),_("New Database"));
    menuItemNew->SetBitmap(toolBarBitmaps[0]);
    wxMenuItem* menuItemOpen = new wxMenuItem(menu_file, MENU_OPEN,_("&Open Database\tCtrl-O"),_("Open Database"));
    menuItemOpen->SetBitmap(toolBarBitmaps[1]);
    wxMenuItem* menuItemSaveAs = new wxMenuItem(menu_file, MENU_SAVE_AS,_("Save Database &As"),_("Save Database As"));
    menuItemSaveAs->SetBitmap(wxBitmap(saveas_xpm));
    menu_file->Append(menuItemNew);
    menu_file->Append(menuItemOpen);
    menu_file->Append(menuItemSaveAs);
    menu_file->AppendSeparator();

    menuRecentFiles_ = new wxMenu;
    menu_file->Append(MENU_RECENT_FILES, _("&Recent Files..."), menuRecentFiles_);
    // Note: menuRecentFiles_ will be constructed by the class: RecentDatabaseFiles::setMenuFileItems()
    wxMenuItem* menuClearRecentFiles = new wxMenuItem(menu_file, MENU_RECENT_FILES_CLEAR,_("&Clear Recent Files"));
    menuClearRecentFiles->SetBitmap(wxBitmap(clearlist_xpm));
    menu_file->Append(menuClearRecentFiles);
    menu_file->AppendSeparator();

    wxMenu* exportMenu = new wxMenu;
    exportMenu->Append(MENU_EXPORT_CSV, _("&CSV Files..."), _("Export to CSV"));
    exportMenu->Append(MENU_EXPORT_QIF, _("&QIF Files..."), _("Export to QIF"));
    exportMenu->Append(MENU_EXPORT_HTML, _("&Report to HTML"), _("Export to HTML"));
    menu_file->Append(MENU_EXPORT, _("&Export"), exportMenu);

    wxMenu* importMenu = new wxMenu;
    importMenu->Append(MENU_IMPORT_UNIVCSV, _("&CSV Files...")
        , _("Import from any CSV file"));
    importMenu->Append(MENU_IMPORT_QIF, _("&QIF Files..."), _("Import from QIF"));
    menu_file->Append(MENU_IMPORT, _("&Import"), importMenu);

    menu_file->AppendSeparator();

    wxMenuItem* menuItemPrintSetup = new wxMenuItem(menu_file, MENU_PRINT_PAGE_SETUP,
        _("Page Set&up..."), _("Setup page printing options"));
    menuItemPrintSetup->SetBitmap(toolBarBitmaps[7]);
    menu_file->Append(menuItemPrintSetup);

    wxMenu* printPreviewMenu = new wxMenu;
    printPreviewMenu->Append(MENU_PRINT_PREVIEW_REPORT,
        _("Current &View"), _("Preview current report"));

    menu_file->Append(MENU_PRINT_PREVIEW, _("Print Pre&view..."), printPreviewMenu);

    wxMenu* printMenu = new wxMenu;
    wxMenuItem* menuItemPrintView = new wxMenuItem(printMenu, MENU_PRINT_REPORT,
        _("Current &View"), _("Print current report"));
    printMenu->Append(menuItemPrintView);

    menu_file->Append( MENU_PRINT, _("&Print..."),  printMenu);

    menu_file->AppendSeparator();

    wxMenuItem* menuItemQuit = new wxMenuItem(menu_file, wxID_EXIT,
        _("E&xit\tAlt-X"), _("Quit this program"));
    menuItemQuit->SetBitmap(wxBitmap(exit_xpm));
    menu_file->Append(menuItemQuit);

    // Create the required menu items
    wxMenu *menuView = new wxMenu;
    wxMenuItem* menuItemToolbar = new wxMenuItem(menuView, MENU_VIEW_TOOLBAR,
        _("&Toolbar"), _("Show/Hide the toolbar"), wxITEM_CHECK);
    wxMenuItem* menuItemLinks = new wxMenuItem(menuView, MENU_VIEW_LINKS,
        _("&Navigation"), _("Show/Hide the Navigation tree control"), wxITEM_CHECK);
    wxMenuItem* menuItemBankAccount = new wxMenuItem(menuView, MENU_VIEW_BANKACCOUNTS,
        _("&Bank Accounts"), _("Show/Hide Bank Accounts on Summary page"), wxITEM_CHECK);
    wxMenuItem* menuItemTermAccount = new wxMenuItem(menuView, MENU_VIEW_TERMACCOUNTS,
        _("Term &Accounts"), _("Show/Hide Term Accounts on Summary page"), wxITEM_CHECK);
    wxMenuItem* menuItemStockAccount = new wxMenuItem(menuView, MENU_VIEW_STOCKACCOUNTS,
        _("&Stock Accounts"), _("Show/Hide Stock Accounts on Summary page"), wxITEM_CHECK);
    wxMenuItem* menuItemBudgetFinancialYears = new wxMenuItem(menuView, MENU_VIEW_BUDGET_FINANCIAL_YEARS,
        _("Budgets: As &Financial Years"), _("Display Budgets in Financial Year Format"), wxITEM_CHECK);
    wxMenuItem* menuItemBudgetTransferTotal = new wxMenuItem(menuView, MENU_VIEW_BUDGET_TRANSFER_TOTAL,
        _("Budgets: &Include Transfers in Totals"), _("Include the transfer transactions in the Budget Totals"), wxITEM_CHECK);
    wxMenuItem* menuItemBudgetSetupWithoutSummary = new wxMenuItem(menuView, MENU_VIEW_BUDGET_SETUP_SUMMARY,
        _("Budget Setup: &Without Summaries"), _("Display the Budget Setup without category summaries"), wxITEM_CHECK);
    wxMenuItem* menuItemBudgetCategorySummary = new wxMenuItem(menuView, MENU_VIEW_BUDGET_CATEGORY_SUMMARY,
        _("Budget Summary: Include &Categories"), _("Include the categories in the Budget Category Summary"), wxITEM_CHECK);
    wxMenuItem* menuItemIgnoreFutureTransactions = new wxMenuItem(menuView, MENU_IGNORE_FUTURE_TRANSACTIONS,
        _("Ignore F&uture Transactions"), _("Ignore Future transactions"), wxITEM_CHECK);

    //Add the menu items to the menu bar
    menuView->Append(menuItemToolbar);
    menuView->Append(menuItemLinks);
    menuView->AppendSeparator();
    menuView->Append(menuItemBankAccount);
    menuView->Append(menuItemTermAccount);
    menuView->Append(menuItemStockAccount);
    menuView->AppendSeparator();

//    wxMenu* budgetingMenu = new wxMenu;
//    budgetingMenu->Append(menuItemBudgetFinancialYears);
//    budgetingMenu->AppendSeparator();
//    budgetingMenu->Append(menuItemBudgetSetupWithoutSummary);
//    budgetingMenu->Append(menuItemBudgetCategorySummary);
//    menuView->AppendSubMenu(budgetingMenu,_("Budget..."));

    menuView->Append(menuItemBudgetFinancialYears);
    menuView->Append(menuItemBudgetTransferTotal);
    menuView->AppendSeparator();
    menuView->Append(menuItemBudgetSetupWithoutSummary);
    menuView->Append(menuItemBudgetCategorySummary);
    menuView->AppendSeparator();
    menuView->Append(menuItemIgnoreFutureTransactions);

    wxMenu *menuAccounts = new wxMenu;

    if (mmIniOptions::instance().enableAddAccount_)
    {
        wxMenuItem* menuItemNewAcct = new wxMenuItem(menuAccounts, MENU_NEWACCT,
            _("New &Account"), _("New Account"));
        menuItemNewAcct->SetBitmap(toolBarBitmaps[3]);
        menuAccounts->Append(menuItemNewAcct);
    }

    wxMenuItem* menuItemAcctList = new wxMenuItem(menuAccounts, MENU_ACCTLIST,
        _("Account &List"), _("Show Account List"));
    menuItemAcctList->SetBitmap(toolBarBitmaps[4]);

    wxMenuItem* menuItemAcctEdit = new wxMenuItem(menuAccounts, MENU_ACCTEDIT,
        _("&Edit Account"), _("Edit Account"));
    menuItemAcctEdit->SetBitmap(toolBarBitmaps[8]);

    if (mmIniOptions::instance().enableDeleteAccount_)
    {
        wxMenuItem* menuItemAcctDelete = new wxMenuItem(menuAccounts, MENU_ACCTDELETE,
            _("&Delete Account"), _("Delete Account from database"));
        menuItemAcctDelete->SetBitmap(toolBarBitmaps[9]);
        menuAccounts->Append(menuItemAcctDelete);
    }

    menuAccounts->Append(menuItemAcctList);
    menuAccounts->Append(menuItemAcctEdit);

    // Tools Menu
    wxMenu *menuTools = new wxMenu;

    wxMenuItem* menuItemCateg = new wxMenuItem(menuTools,
        MENU_ORGCATEGS, _("Organize &Categories..."), _("Organize Categories"));
    menuItemCateg->SetBitmap(wxBitmap(categoryedit_xpm));
    menuTools->Append(menuItemCateg);

    wxMenuItem* menuItemPayee = new wxMenuItem(menuTools,
        MENU_ORGPAYEE, _("Organize &Payees..."), _("Organize Payees"));
    menuItemPayee->SetBitmap(wxBitmap(user_edit_xpm));
    menuTools->Append(menuItemPayee);

    wxMenuItem* menuItemCurrency = new wxMenuItem(menuTools, MENU_CURRENCY,
        _("Organize Currency..."), _("Organize Currency"));
    menuItemCurrency->SetBitmap(wxBitmap(money_dollar_xpm));
    menuTools->Append(menuItemCurrency);

    wxMenu *menuRelocation = new wxMenu;
    wxMenuItem* menuItemCategoryRelocation = new wxMenuItem(menuRelocation
        , MENU_CATEGORY_RELOCATION, _("&Categories...")
        , _("Reassign all categories to another category"));
       menuItemCategoryRelocation->SetBitmap(wxBitmap(relocate_categories_xpm));
    wxMenuItem* menuItemPayeeRelocation = new wxMenuItem(menuRelocation
        , MENU_PAYEE_RELOCATION, _("&Payees...")
        , _("Reassign all payees to another payee"));
    menuItemPayeeRelocation->SetBitmap(wxBitmap(relocate_payees_xpm));
    menuRelocation->Append(menuItemCategoryRelocation);
    menuRelocation->Append(menuItemPayeeRelocation);
    menuTools->AppendSubMenu(menuRelocation, _("Relocation of...")
        , _("Relocate Categories && Payees"));

    menuTools->AppendSeparator();

    if (mmIniOptions::instance().enableBudget_)
    {
        wxMenuItem* menuItemBudgeting = new wxMenuItem(menuTools, MENU_BUDGETSETUPDIALOG,
            _("&Budget Setup"), _("Budget Setup"));
        menuItemBudgeting->SetBitmap(wxBitmap(calendar_xpm));
        menuTools->Append(menuItemBudgeting);
    }

    if (mmIniOptions::instance().enableRepeatingTransactions_)
    {
        wxMenuItem* menuItemBillsDeposits = new wxMenuItem(menuTools, MENU_BILLSDEPOSITS,
            _("&Repeating Transactions"), _("Bills && Deposits"));
        menuItemBillsDeposits->SetBitmap(wxBitmap(clock_xpm));
        menuTools->Append(menuItemBillsDeposits);
    }

    if (mmIniOptions::instance().enableAssets_)
    {
        wxMenuItem* menuItemAssets = new wxMenuItem(menuTools, MENU_ASSETS
            , _("&Assets"), _("Assets"));
        menuItemAssets->SetBitmap(wxBitmap(car_xpm));
        menuTools->Append(menuItemAssets);
    }

    menuTools->AppendSeparator();

    wxMenuItem* menuItemTransactions = new wxMenuItem(menuTools, MENU_TRANSACTIONREPORT,
        _("&Transaction Report Filter..."), _("Transaction Report Filter"));
    menuItemTransactions->SetBitmap(wxBitmap(filter_xpm));
    menuTools->Append(menuItemTransactions);

    menuTools->AppendSeparator();

    // Create Item
    wxMenuItem* menuItemCustomReportEdit = new wxMenuItem(menuTools
        , wxID_EDIT, _("Custom Reports...")
        , _("Create or modify reports for the Reports section"));
    menuItemCustomReportEdit->SetBitmap(wxBitmap(customsql_xpm));
    // Add menu to Tools menu
    menuTools->Append(menuItemCustomReportEdit);

    menuTools->AppendSeparator();

    wxMenuItem* menuItemOptions = new wxMenuItem(menuTools, wxID_PREFERENCES,
          _("&Options..."), _("Show the Options Dialog"));
    menuItemOptions->SetBitmap(wxBitmap(wrench_xpm));
    menuTools->Append(menuItemOptions);

    menuTools->AppendSeparator();

    wxMenuItem* menuItemConvertDB = new wxMenuItem(menuTools, MENU_CONVERT_ENC_DB,
        _("Convert Encrypted &DB"),
        _("Convert Encrypted DB to Non-Encrypted DB"));
    menuItemConvertDB->SetBitmap(wxBitmap(encrypt_db_xpm));
    menuTools->Append(menuItemConvertDB);

    menuTools->AppendSeparator();

    menuItemOnlineUpdateCurRate_ = new wxMenuItem(menuTools, MENU_ONLINE_UPD_CURRENCY_RATE,
        _("Online &Update Currency Rate"),
        _("Online update currency rate"));
    menuItemOnlineUpdateCurRate_->SetBitmap(wxBitmap(update_currency_xpm));
    menuTools->Append(menuItemOnlineUpdateCurRate_);

    // Help Menu
    wxMenu *menuHelp = new wxMenu;

    wxMenuItem* menuItemHelp = new wxMenuItem(menuTools, wxID_HELP,
         _("&Help\tCtrl-F1"), _("Show the Help file"));
    menuItemHelp->SetBitmap(wxBitmap(help_xpm));
    menuHelp->Append(menuItemHelp);

    wxMenuItem* menuItemAppStart = new wxMenuItem(menuTools, MENU_SHOW_APPSTART,
         _("&Show App Start Dialog"), _("App Start Dialog"));
    menuItemAppStart->SetBitmap(wxBitmap(appstart_xpm));
    menuHelp->Append(menuItemAppStart);

    menuHelp->AppendSeparator();

    if (mmIniOptions::instance().enableCheckForUpdates_)
    {
       wxMenuItem* menuItemCheck = new wxMenuItem(menuTools, MENU_CHECKUPDATE,
          _("Check for &Updates"), _("Check For Updates"));
       menuItemCheck->SetBitmap(wxBitmap(checkupdate_xpm));
       menuHelp->Append(menuItemCheck);
    }

    if (mmIniOptions::instance().enableReportIssues_)
    {
       wxMenuItem* menuItemReportIssues = new wxMenuItem(menuTools, MENU_REPORTISSUES
          , _("Visit MMEX Forum")
          , _("Visit the MMEX forum. See existing user comments, or report new issues with the software."));
       menuItemReportIssues->SetBitmap(wxBitmap(issues_xpm));
       menuHelp->Append(menuItemReportIssues);
    }

    if (mmIniOptions::instance().enableBeNotifiedForNewReleases_)
    {
       wxMenuItem* menuItemNotify = new wxMenuItem(menuTools, MENU_ANNOUNCEMENTMAILING
          ,_("Register/View Release &Notifications.")
          , _("Sign up to Notification Mailing List or View existing announcements."));
       menuItemNotify->SetBitmap(wxBitmap(notify_xpm));
       menuHelp->Append(menuItemNotify);
    }

    wxMenuItem* menuItemFacebook = new wxMenuItem(menuTools, MENU_FACEBOOK,
        _("Visit us on Facebook"), _("Visit us on Facebook"));
    menuItemFacebook->SetBitmap(wxBitmap(facebook_xpm));
    menuHelp->Append(menuItemFacebook);

    wxMenuItem* menuItemAbout = new wxMenuItem(menuTools, wxID_ABOUT,
       _("&About..."), _("Show about dialog"));
    menuItemAbout->SetBitmap(wxBitmap(about_xpm));
    menuHelp->Append(menuItemAbout);

    menuBar_ = new wxMenuBar;
    menuBar_->Append(menu_file, _("&File"));
    menuBar_->Append(menuAccounts, _("&Accounts"));
    menuBar_->Append(menuTools, _("&Tools"));
    menuBar_->Append(menuView, _("&View"));
    menuBar_->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar_);

    menuBar_->Check(MENU_VIEW_BANKACCOUNTS, mmIniOptions::instance().expandBankHome_);
    menuBar_->Check(MENU_VIEW_TERMACCOUNTS, mmIniOptions::instance().expandTermHome_);
    menuBar_->Check(MENU_VIEW_STOCKACCOUNTS, mmIniOptions::instance().expandStocksHome_);
    menuBar_->Check(MENU_VIEW_BUDGET_FINANCIAL_YEARS, mmIniOptions::instance().budgetFinancialYears_);
    menuBar_->Check(MENU_VIEW_BUDGET_TRANSFER_TOTAL, mmIniOptions::instance().budgetIncludeTransfers_);
    menuBar_->Check(MENU_VIEW_BUDGET_SETUP_SUMMARY, mmIniOptions::instance().budgetSetupWithoutSummaries_);
    menuBar_->Check(MENU_VIEW_BUDGET_CATEGORY_SUMMARY, mmIniOptions::instance().budgetSummaryWithoutCategories_);
    menuBar_->Check(MENU_IGNORE_FUTURE_TRANSACTIONS, mmIniOptions::instance().ignoreFutureTransactions_);
}
//----------------------------------------------------------------------------

void mmGUIFrame::createToolBar()
{
    toolBar_ = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
    wxBitmap toolBarBitmaps[12];
    toolBarBitmaps[0] = wxBitmap(new_xpm);
    toolBarBitmaps[1] = wxBitmap(open_xpm);
    toolBarBitmaps[2] = wxBitmap(save_xpm);
    toolBarBitmaps[3] = wxBitmap(newacct_xpm);
    toolBarBitmaps[4] = wxBitmap(house_xpm);
    toolBarBitmaps[5] = wxBitmap(categoryedit_xpm);
    toolBarBitmaps[6] = wxBitmap(user_edit_xpm);
    toolBarBitmaps[7] = wxBitmap(money_dollar_xpm);
    toolBarBitmaps[8] = wxBitmap(filter_xpm);
    toolBarBitmaps[9] = wxBitmap(customsql_xpm);
    toolBarBitmaps[10] = wxBitmap(wrench_xpm);
    toolBarBitmaps[11] = wxBitmap(new_transaction_xpm);

    toolBar_->AddTool(MENU_NEW, _("New"), toolBarBitmaps[0], _("New Database"));
    toolBar_->AddTool(MENU_OPEN, _("Open"), toolBarBitmaps[1], _("Open Database"));
    toolBar_->AddSeparator();
    if (mmIniOptions::instance().enableAddAccount_)
        toolBar_->AddTool(MENU_NEWACCT, _("New Account"), toolBarBitmaps[3], _("New Account"));
    toolBar_->AddTool(MENU_ACCTLIST, _("Account List"), toolBarBitmaps[4], _("Show Account List"));
    toolBar_->AddSeparator();
    toolBar_->AddTool(MENU_ORGCATEGS, _("Organize Categories"), toolBarBitmaps[5], _("Show Organize Categories Dialog"));
    toolBar_->AddTool(MENU_ORGPAYEE, _("Organize Payees"), toolBarBitmaps[6], _("Show Organize Payees Dialog"));
    toolBar_->AddTool(MENU_CURRENCY, _("Organize Currency"), toolBarBitmaps[7], _("Show Organize Currency Dialog"));
    toolBar_->AddSeparator();
    toolBar_->AddTool(MENU_TRANSACTIONREPORT, _("Transaction Report Filter"), toolBarBitmaps[8], _("Transaction Report Filter"));
    toolBar_->AddSeparator();
    toolBar_->AddTool(wxID_EDIT, _("Custom Reports Manager"), toolBarBitmaps[9], _("Create new Custom Reports"));
    toolBar_->AddSeparator();
    toolBar_->AddTool(wxID_PREFERENCES, _("&Options..."), toolBarBitmaps[10], _("Show the Options Dialog"));
    toolBar_->AddSeparator();
    toolBar_->AddTool(wxID_NEW, _("New"), toolBarBitmaps[11], _("New Transaction"));

    // after adding the buttons to the toolbar, must call Realize() to reflect changes
    toolBar_->Realize();
}
//----------------------------------------------------------------------------

bool mmGUIFrame::createDataStore(const wxString& fileName, const wxString& pwd, bool openingNew)
{
    if (m_core) m_core.reset();

    if (m_db)
    {
        m_db->Close();
        m_db.reset();

        /// Update the database according to user requirements
        if (mmOptions::instance().databaseUpdated_ &&
            m_inisettings->GetBoolSetting(wxT("BACKUPDB_UPDATE"), false))
        {
            BackupDatabase(fileName_, true);
            mmOptions::instance().databaseUpdated_ = false;
        }
    }

    wxFileName checkExt(fileName);
    wxString password;
    bool passwordCheckPassed = true;
    if (checkExt.GetExt().Lower() == wxT("emb") && wxFileName::FileExists(fileName))
    {
        password = !pwd.empty() ? pwd : wxGetPasswordFromUser(_("Enter database's password"));
        if (password.IsEmpty())
            passwordCheckPassed = false;
    }

    wxString dialogErrorMessageHeading = _("Opening MMEX Database - Error");

    // Existing Database
    if (!openingNew
        && !fileName.IsEmpty()
        && wxFileName::FileExists(fileName)
        && passwordCheckPassed)
    {
        /* Do a backup before opening */
        if (m_inisettings->GetBoolSetting(wxT("BACKUPDB"), false))
        {
            BackupDatabase(fileName);
        }

        m_db = mmDBWrapper::Open(fileName, password);
        // if the database pointer has been reset, the password is possibly incorrect
        if (!m_db) return false;

        // we need to check the db whether it is the right version
        if (!mmDBWrapper::checkDBVersion(m_db.get()))
        {
            wxString note = mmex::getProgramName() + _(" - No File opened ");
            this->SetTitle(note);
            wxMessageBox(_("Sorry. The Database version is too old or Database password is incorrect"),
                        dialogErrorMessageHeading, wxOK|wxICON_EXCLAMATION);

            m_db->Close();
            m_db.reset();
            return false;
        }

        password_ = password;
        m_core.reset(new mmCoreDB(m_db, m_inisettings));
    }
    else if (openingNew) // New Database
    {
       if (mmIniOptions::instance().enableCustomTemplateDB_
           && wxFileName::FileExists(mmIniOptions::instance().customTemplateDB_))
       {
           wxCopyFile(mmIniOptions::instance().customTemplateDB_, fileName, true);
           m_db = mmDBWrapper::Open(fileName);
           password_ = password;
           m_core.reset(new mmCoreDB(m_db, m_inisettings));
       }
       else
       {
           m_db = mmDBWrapper::Open(fileName, password);
           password_ = password;

           openDataBase(fileName);
           m_core.reset(new mmCoreDB(m_db, m_inisettings));

           mmNewDatabaseWizard* wizard = new mmNewDatabaseWizard(this, m_core.get());
           wizard->CenterOnParent();
           wizard->RunIt(true);

           m_core->currencyList_.LoadBaseCurrencySettings();

           /* Load User Name and Other Settings */
           mmOptions::instance().loadOptions(m_core->dbInfoSettings_.get());

           /* Jump to new account creation screen */
           wxCommandEvent evt;
           OnNewAccount(evt);
           return true;
       }
    }
    else // open of existing database failed
    {
        wxString note = mmex::getProgramName() + _(" - No File opened ");
        this->SetTitle(note);

        wxString msgStr = _("Cannot locate previously opened database.\n");
        if (!passwordCheckPassed)
            msgStr = _("Password not entered for encrypted Database.\n");

        msgStr << fileName;
        wxMessageBox(msgStr, dialogErrorMessageHeading, wxOK|wxICON_ERROR);
        menuEnableItems(false);
        return false;
    }

    openDataBase(fileName);

    return true;
}
//----------------------------------------------------------------------------

void mmGUIFrame::openDataBase(const wxString& fileName)
{
    wxString title = mmex::getProgramName() + wxT(" : ") + fileName;
    if (mmex::isPortableMode())
        title << wxT(" [") << _("portable mode") << wxT(']');

    SetTitle(title);
    m_topCategories.Clear();

    if (m_db)
    {
        fileName_ = fileName;
    }
    else
    {
        fileName_.Clear();
        password_.Clear();
    }
}
//----------------------------------------------------------------------------

bool mmGUIFrame::openFile(const wxString& fileName, bool openingNew, const wxString &password)
{
    if (createDataStore(fileName, password, openingNew))
    {
        menuEnableItems(true);
        menuPrintingEnable(false);
        autoRepeatTransactionsTimer_.Start(REPEAT_TRANS_DELAY_TIME, wxTIMER_ONE_SHOT);

        updateNavTreeControl();

        if (!refreshRequested_)
        {
            refreshRequested_ = true;
            /* Currency Options might have changed so refresh */
            wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
            GetEventHandler()->AddPendingEvent(ev);
        }
    }
    else
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnNew(wxCommandEvent& /*event*/)
{
    autoRepeatTransactionsTimer_.Stop();
    wxFileDialog dlg(this,
                     _("Choose database file to create"),
                     wxEmptyString,
                     wxEmptyString,
                     wxT("MMB Files(*.mmb)|*.mmb"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                    );

    if(dlg.ShowModal() != wxID_OK)
        return;

    wxString fileName = dlg.GetPath();

    if (!fileName.EndsWith(wxT(".mmb")))
        fileName += wxT(".mmb");

    SetDatabaseFile(fileName, true);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOpen(wxCommandEvent& /*event*/)
{
    autoRepeatTransactionsTimer_.Stop();
    wxString fileName = wxFileSelector(_("Choose database file to open"),
                                       wxEmptyString, wxEmptyString, wxEmptyString,
                                       wxT("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb"),
                                       wxFD_FILE_MUST_EXIST,
                                       this
                                      );

    if (!fileName.empty())
    {
        SetDatabaseFile(fileName);
        saveSettings();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnConvertEncryptedDB(wxCommandEvent& /*event*/)
{
    wxString encFileName = wxFileSelector(_("Choose Encrypted database file to open"),
                                          wxEmptyString, wxEmptyString, wxEmptyString,
                                          wxT("Encrypted MMB files (*.emb)|*.emb"),
                                          wxFD_FILE_MUST_EXIST,
                                          this
                                         );

    if (encFileName.empty())
        return;

    wxString password = wxGetPasswordFromUser(_("Enter password for database"));
    if (password.empty())
        return;

    wxFileDialog dlg(this,
                     _("Choose database file to Save As"),
                     wxEmptyString,
                     wxEmptyString,
                     wxT("MMB Files(*.mmb)|*.mmb"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                    );

    if(dlg.ShowModal() != wxID_OK)
        return;

    wxString fileName = dlg.GetPath();

    if (!dlg.GetPath().EndsWith(wxT(".mmb")))
        fileName += wxT(".mmb");

    wxCopyFile(encFileName, fileName);

    wxSQLite3Database db;
    db.Open(fileName, password);
    db.ReKey(wxEmptyString);
    db.Close();

    mmShowErrorMessage(this, _("Converted DB!"), _("MMEX message"));
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnSaveAs(wxCommandEvent& /*event*/)
{
    wxASSERT(m_db);

    if (fileName_.empty())
    {
        wxASSERT(false);
        return;
    }

    wxFileDialog dlg(this,
                     _("Save database file as"),
                     wxEmptyString,
                     wxEmptyString,
                     wxT("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                    );

    if (dlg.ShowModal() != wxID_OK) return;

    // Ensure database is in a steady state first
    if (!activeHomePage_) createHomePage();   // Display Home page when not being displayed.

    bool encrypt = dlg.GetFilterIndex() != 0; // emb -> Encrypted mMB
    wxFileName newFileName(dlg.GetPath());
    fixFileExt(newFileName, encrypt ? wxT("emb") : wxT("mmb"));

    wxFileName oldFileName(fileName_); // opened db's file

    if (newFileName == oldFileName) // on case-sensitive FS uses case-sensitive comparison
    {
        wxMessageDialog dlg(this, _("Can't copy file to itself"), _("Save database file as"), wxOK|wxICON_WARNING);
        dlg.ShowModal();
        return;
    }

    // prepare to copy
    wxString new_password;
    bool rekey = encrypt ^ m_db->IsEncrypted();

    if (encrypt)
    {
        if (rekey)
        {
            new_password = wxGetPasswordFromUser(_("Enter password for new database"));
            if (new_password.empty())
                return;
        }
        else
        {
            new_password = password_;
        }
    }

    // copying db

    if (m_db) // database must be closed before copying its file
    {
        m_core.reset();
        m_db->Close();
        m_db.reset();
    }

    if (!wxCopyFile(oldFileName.GetFullPath(), newFileName.GetFullPath(), true))  // true -> overwrite if file exists
        return;

    if (rekey) // encrypt or reset encryption
    {
        wxSQLite3Database dbx;
        dbx.Open(newFileName.GetFullPath(), password_);
        dbx.ReKey(new_password); // empty password resets encryption
        dbx.Close();
    }

    password_.clear();
    openFile(newFileName.GetFullPath(), false, new_password);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnExportToCSV(wxCommandEvent& /*event*/)
{
    mmUnivCSVDialog(m_core.get(), this, false).ShowModal();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnExportToQIF(wxCommandEvent& /*event*/)
{
    mmQIFExportDialog* dlg = new mmQIFExportDialog(m_core.get(), this);
    dlg->ShowModal();
    dlg->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnImportQIF(wxCommandEvent& /*event*/)
{

    mmQIFImportDialog* dlg = new mmQIFImportDialog(m_core.get(), this);
    dlg->ShowModal();
    int account_id = dlg->get_last_imported_acc();
    refreshRequested_ = true;
    updateNavTreeControl();
    if (account_id > 0)
    {
        setGotoAccountID(account_id, -1);
        setAccountNavTreeSection(m_core->accountList_.GetAccountName(account_id));
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_GOTOACCOUNT);
        this->GetEventHandler()->AddPendingEvent(evt);
    }
    dlg->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnImportUniversalCSV(wxCommandEvent& /*event*/)
{
    if (m_core.get()->accountList_.getNumAccounts() == 0)
    {
        wxMessageBox(_("No account available to import"),_("Universal CSV Import"), wxOK|wxICON_WARNING );
        return;
    }

    mmUnivCSVDialog univCSVDialog(m_core.get(), this);
    univCSVDialog.ShowModal();
    if (univCSVDialog.InportCompletedSuccessfully())
    {
        setAccountNavTreeSection(m_core.get()->accountList_.GetAccountName(univCSVDialog.ImportedAccountID()));
        createCheckingAccountPage(univCSVDialog.ImportedAccountID());
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    autoRepeatTransactionsTimer_.Stop();
//    this->Destroy();
    Close(TRUE);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnNewAccount(wxCommandEvent& /*event*/)
{
    mmAddAccountWizard* wizard = new mmAddAccountWizard(this, m_core.get());
    wizard->CenterOnParent();
    wizard->RunIt(true);

    if (wizard->acctID_ != -1)
    {
        bool firstTermAccount = !hasActiveTermAccounts();
        mmNewAcctDialog dlg(m_core.get(), true, wizard->acctID_, this);
        dlg.ShowModal();
        if (dlg.termAccountActivated() )
        {
            updateNavTreeControl(true);
            menuBar_->FindItem(MENU_VIEW_TERMACCOUNTS)->Check(true);
            if (firstTermAccount)
            {
            /***************Message to display *************************
                Term Account views have been temporarly turned on.
                To maintain this view, change the defaults by using:

                Tools -> Options
                View Options

                This message will not be displayed in future.
            ************************************************************/
                wxString msgStr;
                msgStr << _("Term Account views have been temporarly turned on.") << wxT("\n")
                       << _("To maintain this view, change the defaults by using:\n\nTools -> Options\nView Options")
                       << wxT("\n\n")
                       << _("This message will not be displayed in future.");
                wxMessageBox(msgStr, _("Initial Term Account Activation"), wxOK|wxICON_INFORMATION);
            }
        }
        else
        {
            updateNavTreeControl();
        }
     }

    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        /* Currency Options might have changed so refresh */
        wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
        GetEventHandler()->AddPendingEvent(ev);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnAccountList(wxCommandEvent& /*event*/)
{
    createHomePage();
    homePageAccountSelect_ = true;
    navTreeCtrl_->SelectItem(navTreeCtrl_->GetRootItem());
    homePageAccountSelect_ = false;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOrgCategories(wxCommandEvent& /*event*/)
{
    mmCategDialog(m_core.get(), this, false).ShowModal();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOrgPayees(wxCommandEvent& /*event*/)
{
    mmPayeeDialog(this, m_core.get(), false).ShowModal();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnNewTransaction(wxCommandEvent& /*event*/)
{
    mmTransDialog dlg(m_core.get(), gotoAccountID_, NULL, false, this);

    if ( dlg.ShowModal() == wxID_OK )
    {
        if (activeCheckingAccountPage_)
        {
            if (gotoAccountID_ == dlg.getToAccountID() || gotoAccountID_ == dlg.getAccountID())
            {
                refreshRequested_ = true;
                createCheckingAccountPage(gotoAccountID_);
            }
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnBudgetSetupDialog(wxCommandEvent& /*event*/)
{
    if (m_db)
    {
        mmBudgetYearDialog(m_core.get(), this).ShowModal();
        createHomePage();
        updateNavTreeControl();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnTransactionReport(wxCommandEvent& /*event*/)
{
    if (!m_db) return;

    if (m_core.get()->accountList_.getNumAccounts() == 0) return;

    std::vector< wxSharedPtr<mmBankTransaction> > trans;

    mmFilterTransactionsDialog* dlg= new mmFilterTransactionsDialog(m_core.get(), this);
    if (dlg->ShowModal() == wxID_OK)
    {
        std::vector< wxSharedPtr<mmBankTransaction> >::const_iterator i;
        for (i = m_core.get()->bTransactionList_.transactions_.begin(); i != m_core.get()->bTransactionList_.transactions_.end(); i++ )
        {
            wxSharedPtr<mmBankTransaction> pBankTransaction = *i;
            if (pBankTransaction)
            {

                if (dlg->getAmountRangeCheckBox())
                {
                    double minamt = dlg->getAmountMin();
                    double maxamt = dlg->getAmountMax();

                    if (pBankTransaction->amt_ < minamt)
                        continue; // skip
                    if (pBankTransaction->amt_ > maxamt)
                        continue; // skip
                }

                if (dlg->getAccountCheckBox())
                {
                    int fromAccountID = dlg->getAccountID();

                    if ((pBankTransaction->accountID_ != fromAccountID) && (pBankTransaction->toAccountID_ != fromAccountID))
                        continue; // skip
                }

                if (dlg->getDateRangeCheckBox())
                {
                    wxDateTime dtBegin = dlg->getFromDateCtrl();
                    wxDateTime dtEnd = dlg->getToDateControl();

                    if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                        continue; // skip
                }

                if (dlg->getPayeeCheckBox())
                {
                    if (pBankTransaction->payeeID_ != dlg->getPayeeID())
                        continue; // skip
                }

                if (dlg->getStatusCheckBox())
                {
                    if (dlg->getStatus() != pBankTransaction->status_) continue; //skip
                }

                if (dlg->getTypeCheckBox())
                {
                    if (!dlg->getType().Contains(pBankTransaction->transType_)) continue;
                }

                if (dlg->getNumberCheckBox())
                {
                    const wxString transNumber = dlg->getNumber().Trim().Lower();
                    const wxString orig = pBankTransaction->transNum_.Lower();
                    if (!orig.Matches(transNumber))
                        continue;
                }

                if (dlg->getNotesCheckBox())
                {
                    wxString filter_notes = dlg->getNotes().Trim().Lower();
                    wxString trx_notes = pBankTransaction->notes_.Lower();

                    if (!trx_notes.Matches(filter_notes))
                        continue;
                }

                if (dlg->getCategoryCheckBox())
                {
                    bool ignoreSubCateg = false;
                    int subcategID = dlg->getSubCategoryID();
                    int categID = dlg->getCategoryID();
                    if (subcategID == -1)
                        ignoreSubCateg = dlg->getExpandStatus();
                    if (!pBankTransaction->containsCategory(categID, subcategID, ignoreSubCateg))
                    {
                        pBankTransaction->reportCategAmountStr_ = wxT("");
                        continue;
                    }

                    if (pBankTransaction->splitEntries_->numEntries() > 0)
                    {
                        pBankTransaction->reportCategAmount_ = (pBankTransaction->getAmountForSplit(categID, subcategID));

                        wxSharedPtr<mmCurrency> pCurrencyPtr = m_core.get()->accountList_.getCurrencySharedPtr(pBankTransaction->accountID_);
                        wxASSERT(pCurrencyPtr);
                        mmex::formatDoubleToCurrency(pBankTransaction->reportCategAmount_, pBankTransaction->reportCategAmountStr_);
                    }
                    else
                    {
                        pBankTransaction->reportCategAmount_ = -1;
                        pBankTransaction->reportCategAmountStr_.clear();
                    }
                }

                trans.push_back(pBankTransaction);
            }
        }

        mmReportTransactions* rs = new mmReportTransactions(trans, m_core.get(), dlg->getAccountID(), dlg);
        createReportsPage(rs);
    }
}
//----------------------------------------------------------------------------

wxArrayString mmGUIFrame::getAccountsArray( bool withTermAccounts) const
{
    wxArrayString accountArray;

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->accountList_.range();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* account = it->get();

        if (account->acctType_ == ACCOUNT_TYPE_BANK || (withTermAccounts && account->acctType_ == ACCOUNT_TYPE_TERM))
            accountArray.Add(account->name_);
    }

    return accountArray;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnCashFlowSpecificAccounts(int cashflowreporttype)
{
    if (!m_db.get()) return;
    if (m_core.get()->accountList_.getNumAccounts() == 0) return;

    wxArrayString accountArray = getAccountsArray(true);

    wxMultiChoiceDialog mcd(this, _("Choose Accounts"), _("Cash Flow"), accountArray);
    if (mcd.ShowModal() == wxID_OK)
    {
        wxArrayInt arraySel = mcd.GetSelections();

        wxArrayString* selections = new wxArrayString();
        for (size_t i = 0; i < arraySel.size(); ++i)
        {
            selections->Add(accountArray.Item(arraySel[i]));
        }

        // mmReportCashFlow is a mmPrintableBase
        mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this, cashflowreporttype, selections);

        report->activateBankAccounts();
        if (this->hasActiveTermAccounts()) report->activateTermAccounts();

        createReportsPage(report);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOptions(wxCommandEvent& /*event*/)
{
    if (!m_db.get()) return;

    mmOptionsDialog systemOptions(m_core.get(), this);
    if (systemOptions.ShowModal() == wxOK && systemOptions.AppliedChanges())
    {
        systemOptions.SaveNewSystemSettings();
        // enable or disable online update currency rate
        menuItemOnlineUpdateCurRate_->Enable(systemOptions.GetUpdateCurrencyRateSetting());

        //set the View Menu Option items the same as the options saved.
        menuBar_->FindItem(MENU_VIEW_BANKACCOUNTS)->Check(mmIniOptions::instance().expandBankHome_);
        menuBar_->FindItem(MENU_VIEW_TERMACCOUNTS)->Check(mmIniOptions::instance().expandTermHome_);
        menuBar_->FindItem(MENU_VIEW_STOCKACCOUNTS)->Check(mmIniOptions::instance().expandStocksHome_);
        menuBar_->FindItem(MENU_VIEW_BUDGET_FINANCIAL_YEARS)->Check(mmIniOptions::instance().budgetFinancialYears_);
        menuBar_->FindItem(MENU_VIEW_BUDGET_TRANSFER_TOTAL)->Check(mmIniOptions::instance().budgetIncludeTransfers_);
        menuBar_->FindItem(MENU_VIEW_BUDGET_SETUP_SUMMARY)->Check(mmIniOptions::instance().budgetSetupWithoutSummaries_);
        menuBar_->FindItem(MENU_VIEW_BUDGET_CATEGORY_SUMMARY)->Check(mmIniOptions::instance().budgetSummaryWithoutCategories_);
        menuBar_->FindItem(MENU_IGNORE_FUTURE_TRANSACTIONS)->Check(mmIniOptions::instance().ignoreFutureTransactions_);

        int messageIcon = wxOK|wxICON_INFORMATION;
        wxString sysMsg = wxString() << _("MMEX Options have been updated.") << wxT("\n\n");
        if (systemOptions.RequiresRestart())
        {
            messageIcon = wxOK|wxICON_WARNING;
            sysMsg << _("Recommendation: Shut down and restart MMEX.") << wxT("\n")
                   << _("This will allow all MMEX option updates to take effect.");
        }
        wxMessageBox(sysMsg, _("New MMEX Options"), messageIcon);

        createHomePage();
        updateNavTreeControl();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnHelp(wxCommandEvent& /*event*/)
{
    helpFileIndex_ = mmex::HTML_INDEX;
    createHelpPage();
}
//----------------------------------------------------------------------------

bool mmGUIFrame::IsUpdateAvailable(wxString page)
{
    wxStringTokenizer tkz(page, wxT('.'), wxTOKEN_RET_EMPTY_ALL);
    int numTokens = (int)tkz.CountTokens();
    if (numTokens != 4)
    {
        wxString url = wxT("http://www.codelathe.com/mmex");
        wxLaunchDefaultBrowser(url);
        return false;
    }

    wxString maj = tkz.GetNextToken();
    wxString min = tkz.GetNextToken();
    wxString cust = tkz.GetNextToken();
    wxString build = tkz.GetNextToken();

    // get current version
    wxString currentV = mmex::getProgramVersion();
    currentV = currentV.SubString(0, currentV.Find(wxT("DEV"))-1).Trim();

    wxStringTokenizer tkz1(currentV, wxT('.'), wxTOKEN_RET_EMPTY_ALL);
    numTokens = (int)tkz1.CountTokens();

    wxString majC = tkz1.GetNextToken();
    wxString minC = tkz1.GetNextToken();
    wxString custC = tkz1.GetNextToken();
    wxString buildC = tkz1.GetNextToken();

    bool isUpdateAvailable = false;
    if (maj > majC)
        isUpdateAvailable = true;
    else if (maj == majC)
    {
        if (min > minC)
        {
            isUpdateAvailable = true;
        }
        else if (min == minC)
        {
            if (cust > custC)
            {
                isUpdateAvailable = true;
            }
            else if (cust == custC)
            {
                if (build > buildC)
                    isUpdateAvailable = true;
            }
        }
    }

    return isUpdateAvailable;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnCheckUpdate(wxCommandEvent& /*event*/)
{
    // Set up system information
    wxString versionDetails = wxString()
        << _("Version: ")<< mmex::getProgramVersion() << wxT("\n\n")
        << mmex::getProgramDescription() << wxT("\n")
        << wxT("\n");

    // Access current version details page
    wxString site = wxT("http://www.codelathe.com/mmex/version.html");

    wxString page;
    int err_code = site_content(site, page);
    if (err_code != wxURL_NOERR)
    {
        versionDetails << page;
        wxMessageBox(versionDetails, _("MMEX System Information Check"));
        return;
    }

    /*************************************************************************
        Expected format of the string from the internet. Version: 0.9.8.0
        page = wxT("x.x.x.x - Win: w.w.w.w - Unix: u.u.u.u - Mac: m.m.m.m");
        string length = 53 characters
    **************************************************************************/
    page = page.SubString(page.find(wxPlatformInfo::Get().GetOperatingSystemFamilyName().substr(0, 3)), 53);
    page.Replace(wxT("-"), wxT(":"));
    wxStringTokenizer mySysToken(page, wxT(":"));
    page = mySysToken.GetNextToken();                       // the system
    page = mySysToken.GetNextToken().Trim(false).Trim();    // the version

    // set up display information.
    int style = wxOK|wxCANCEL;
    if (IsUpdateAvailable(page))
    {
        versionDetails << _("New update available!");
        style = wxICON_EXCLAMATION|style;
    }
    else
    {
        versionDetails << _("You have the latest version installed!");
        style = wxICON_INFORMATION|style;
    }

    wxString urlString = wxT("http://www.codelathe.com/mmex");
    versionDetails << wxT("\n\n") << _("Proceed to website: ") << urlString;
    if (wxMessageBox(versionDetails, _("MMEX System Information Check"), style) == wxOK)
        wxLaunchDefaultBrowser(urlString);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOnlineUpdateCurRate(wxCommandEvent& /*event*/)
{
    wxString sMsg = wxT("");
    if (m_core.get()->currencyList_.OnlineUpdateCurRate(sMsg))
    {
        wxMessageDialog msgDlg(this, sMsg, _("Currency rate updated"));
        msgDlg.ShowModal();
    }
    else
    {
        wxMessageDialog msgDlg(this, sMsg, _("Error"), wxICON_ERROR);
        msgDlg.ShowModal();
    }
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_ACCOUNT_LIST);
    //AddPendingEvent(evt);
    OnAccountList(evt);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnReportIssues(wxCommandEvent& /*event*/)
{
   wxString url = mmex::getProgramForum();
   wxLaunchDefaultBrowser(url);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnBeNotified(wxCommandEvent& /*event*/)
{
    // New site location
    //  wxString url = wxT("http://groups.google.com/group/mmlist");
    wxString url = wxT("https://groups.google.com/forum/?fromgroups#!forum/mmlist");

    wxLaunchDefaultBrowser(url);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnFacebook(wxCommandEvent& /*event*/)
{
    wxString url = wxT("http://www.facebook.com/pages/Money-Manager-Ex/242286559144586");
    wxLaunchDefaultBrowser(url);
}

//----------------------------------------------------------------------------

void mmGUIFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    mmAboutDialog(this).ShowModal();
}
//----------------------------------------------------------------------------

void mmGUIFrame::restorePrinterValues()
{
    // Startup Default Settings
    int leftMargin      = m_inisettings->GetIntSetting(wxT("PRINTER_LEFT_MARGIN"), 20);
    int rightMargin     = m_inisettings->GetIntSetting(wxT("PRINTER_RIGHT_MARGIN"), 20);
    int topMargin       = m_inisettings->GetIntSetting(wxT("PRINTER_TOP_MARGIN"), 20);
    int bottomMargin    = m_inisettings->GetIntSetting(wxT("PRINTER_BOTTOM_MARGIN"), 20);
    int pageOrientation = m_inisettings->GetIntSetting(wxT("PRINTER_PAGE_ORIENTATION"), wxPORTRAIT);
    int paperID         = m_inisettings->GetIntSetting(wxT("PRINTER_PAGE_ID"), wxPAPER_A4);

    wxPoint topLeft(leftMargin, topMargin);
    wxPoint bottomRight(rightMargin, bottomMargin);

    wxPageSetupDialogData* pinterData = printer_->GetPageSetupData();
    pinterData->SetMarginTopLeft(topLeft);
    pinterData->SetMarginBottomRight(bottomRight);

    wxPrintData* printerData = printer_->GetPrintData();
    // fix warning for wxWidgets 2.9.3
#if wxCHECK_VERSION(2,9,0)
    printerData->SetOrientation((wxPrintOrientation)pageOrientation);
#else // use code for wxWidgets 2.8.x
    printerData->SetOrientation(pageOrientation);
#endif
    printerData->SetPaperId( (wxPaperSize)paperID );
}

void mmGUIFrame::OnPrintPageSetup(wxCommandEvent& WXUNUSED(event))
{
    if (printer_)
    {
        printer_->PageSetup();

        wxPageSetupDialogData* printerDialogData = printer_->GetPageSetupData();
        wxPoint topLeft = printerDialogData->GetMarginTopLeft();
        wxPoint bottomRight = printerDialogData->GetMarginBottomRight();

        wxPrintData* printerData = printer_->GetPrintData();
        int pageOrientation = printerData->GetOrientation();
        wxPaperSize paperID = printerData->GetPaperId();

        m_inisettings->SetIntSetting(wxT("PRINTER_LEFT_MARGIN"), topLeft.x);
        m_inisettings->SetIntSetting(wxT("PRINTER_RIGHT_MARGIN"), bottomRight.x);
        m_inisettings->SetIntSetting(wxT("PRINTER_TOP_MARGIN"), topLeft.y);
        m_inisettings->SetIntSetting(wxT("PRINTER_BOTTOM_MARGIN"), bottomRight.y);
        m_inisettings->SetIntSetting(wxT("PRINTER_PAGE_ORIENTATION"), pageOrientation);
        m_inisettings->SetIntSetting(wxT("PRINTER_PAGE_ID"), paperID);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPrintPageReport(wxCommandEvent& WXUNUSED(event))
{
    if (!printer_) return;

    mmReportsPanel* report_panel = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    mmHomePagePanel* home_page = dynamic_cast<mmHomePagePanel*>(panelCurrent_);
    mmHelpPanel* help_page = dynamic_cast<mmHelpPanel*>(panelCurrent_);
    if (report_panel)
        printer_ ->PrintText(report_panel->getReportText());
    else if (home_page && activeHomePage_)
        printer_ ->PrintText(home_page->GetHomePageText());
    else if (help_page)
        printer_ ->PrintFile(mmex::getPathDoc((mmex::EDocFile)helpFileIndex_));
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPrintPagePreview(wxCommandEvent& WXUNUSED(event))
{
    if (!printer_) return;

    mmReportsPanel* report_panel = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    mmHomePagePanel* home_page = dynamic_cast<mmHomePagePanel*>(panelCurrent_);
    mmHelpPanel* help_page = dynamic_cast<mmHelpPanel*>(panelCurrent_);
    if (report_panel)
        printer_ ->PreviewText(report_panel->getReportText());
    else if (home_page && activeHomePage_)
        printer_ ->PreviewText(home_page->GetHomePageText());
    else if (help_page)
        printer_ ->PreviewFile(mmex::getPathDoc((mmex::EDocFile)helpFileIndex_));
}
//----------------------------------------------------------------------------

void mmGUIFrame::showBeginAppDialog(bool fromScratch)
{
    mmAppStartDialog dlg(m_inisettings, this);
    if (fromScratch) dlg.SetCloseButtonToExit();

    int end_mod = dlg.ShowModal();
    if (end_mod == wxID_EXIT)
    {
        Close();
    }
    else if (end_mod == wxID_FILE1)
    {
        wxFileName fname(mmDBWrapper::getLastDbPath(m_inisettings));
        if (fname.IsOk()) SetDatabaseFile(fname.GetFullPath());
    }
    else if (end_mod == wxID_OPEN)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_OPEN);
        AddPendingEvent(evt);
    }
    else if (end_mod == wxID_NEW)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_NEW);
        AddPendingEvent(evt);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnShowAppStartDialog(wxCommandEvent& WXUNUSED(event))
{
    showBeginAppDialog();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnExportToHtml(wxCommandEvent& WXUNUSED(event))
{
    mmReportsPanel* rp = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    if (rp)
    {
        wxString fileName = wxFileSelector(wxT("Choose HTML file to Export"),
            wxEmptyString, wxEmptyString, wxEmptyString, wxT("*.html"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if ( !fileName.empty() )
        {
            correctEmptyFileExt(wxT("html"),fileName);
            wxFileOutputStream output( fileName );
            wxTextOutputStream text( output );
            text << rp->getReportText();
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnBillsDeposits(wxCommandEvent& WXUNUSED(event))
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmBillsDepositsPanel(m_core.get(), homePanel_, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel_->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::createStocksAccountPage(int accountID)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmStocksPanel(m_core.get(), accountID, homePanel_, wxID_STATIC,
                                      wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL
                                      );

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
    homePanel_->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnGotoStocksAccount(wxCommandEvent& WXUNUSED(event))
{
    if (gotoAccountID_ != -1)
        createStocksAccountPage(gotoAccountID_);
}
//----------------------------------------------------------------------------

void mmGUIFrame::SetCheckingAccountPageInactive()
{
    activeCheckingAccountPage_ = false;
}
//----------------------------------------------------------------------------

void mmGUIFrame::createCheckingAccountPage(int accountID)
{
    if (activeCheckingAccountPage_)
    {
        checkingAccountPage_->DisplayAccountDetails(accountID);
    }
    else
    {
        wxSizer *sizer = cleanupHomePanel();

        checkingAccountPage_ = new mmCheckingPanel(m_core.get(), this,
                                   accountID, homePanel_, wxID_STATIC,
                                   wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        panelCurrent_ = checkingAccountPage_;
        activeCheckingAccountPage_ = true;
        if (gotoTransID_ > 0)
        {
            checkingAccountPage_->SetSelectedTransaction(gotoTransID_);
        }

        sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
        homePanel_->Layout();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnGotoAccount(wxCommandEvent& WXUNUSED(event))
{
    if (gotoAccountID_ != -1)
    {
        createCheckingAccountPage(gotoAccountID_);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnAssets(wxCommandEvent& /*event*/)
{
    wxSizer *sizer = cleanupHomePanel();
    panelCurrent_ = new mmAssetsPanel(homePanel_, m_core.get());
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
    homePanel_->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnCurrency(wxCommandEvent& /*event*/)
{
    mmMainCurrencyDialog(m_core.get(),this, false).ShowModal();
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TREEPOPUP_ACCOUNT_LIST);
    OnAccountList(evt);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnWizardCancel(wxWizardEvent& event)
{
    event.Veto();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnEditAccount(wxCommandEvent& /*event*/)
{
    if (m_core->accountList_.accounts_.size() == 0)
    {
        wxMessageBox(_("No account available to edit!"), _("Accounts"), wxOK|wxICON_WARNING);
        return;
    }

    wxArrayString as;
    int num = (int)m_core->accountList_.accounts_.size();
    std::vector<int> arrAcctID;

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->accountList_.range();
    int idx = 0;
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* account = it->get();
        as.Add(account->name_);
        arrAcctID[idx ++] = account->id_;
    }

    wxSingleChoiceDialog scd(this, _("Choose Account to Edit"), _("Accounts"), as);
    if (scd.ShowModal() == wxID_OK)
    {
        int choice = scd.GetSelection();
        int acctID = arrAcctID[choice];
        mmNewAcctDialog dlg(m_core.get(), true, acctID, this);
        if ( dlg.ShowModal() == wxID_OK )
        {
            updateNavTreeControl();
            if (!refreshRequested_)
            {
                refreshRequested_ = true;
                wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
                GetEventHandler()->AddPendingEvent(ev);
            }
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnDeleteAccount(wxCommandEvent& /*event*/)
{
    if (m_core->accountList_.accounts_.size() == 0)
    {
        wxMessageBox(_("No account available to delete!"), _("Accounts"), wxOK|wxICON_WARNING);
        return;
    }

    wxArrayString as;
    int num = (int)m_core->accountList_.accounts_.size();
    int* arrAcctID = new int[num];

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->accountList_.range();
    int idx = 0;
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* account = it->get();
        as.Add(account->name_);
        arrAcctID[idx ++] = account->id_;
    }

    wxSingleChoiceDialog scd (this, _("Choose Account to Delete"), _("Accounts"), as);
    if (scd.ShowModal() == wxID_OK)
    {
        int choice = scd.GetSelection();
        int acctID = arrAcctID[choice];

        wxString deletingAccountName = _("Are you sure you want to delete\n") + m_core->accountList_.accounts_[choice]->acctType_ +
                                       _(" account: ") + m_core->accountList_.accounts_[choice]->name_ + wxT(" ?");
        wxMessageDialog msgDlg(this, deletingAccountName, _("Confirm Account Deletion"),
            wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
        if (msgDlg.ShowModal() == wxID_YES)
        {
            m_core->accountList_.RemoveAccount(acctID);
            m_core->bTransactionList_.deleteTransactions(acctID);

            updateNavTreeControl();
        }
    }
    delete[] arrAcctID;
    updateNavTreeControl();
    createHomePage();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewToolbar(wxCommandEvent &event)
{
    m_mgr.GetPane(wxT("toolbar")).Show(event.IsChecked());
    m_mgr.Update();
    m_inisettings->SetBoolSetting(wxT("SHOWTOOLBAR"), event.IsChecked());
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewLinks(wxCommandEvent &event)
{
    m_mgr.GetPane(wxT("Navigation")).Show(event.IsChecked());
    m_mgr.Update();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewToolbarUpdateUI(wxUpdateUIEvent &event)
{
    if(m_mgr.GetPane(wxT("toolbar")).IsShown())
        event.Check(true);
    else
        event.Check(false);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewLinksUpdateUI(wxUpdateUIEvent &event)
{
    if(m_mgr.GetPane(wxT("Navigation")).IsShown())
        event.Check(true);
    else
        event.Check(false);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewBankAccounts(wxCommandEvent &event)
{
    m_mgr.GetPane(wxT("Bank Accounts")).Show(event.IsChecked());
    m_mgr.Update();

    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        createHomePage();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewTermAccounts(wxCommandEvent &event)
{
    m_mgr.GetPane(wxT("Term Accounts")).Show(event.IsChecked());
    m_mgr.Update();

    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        createHomePage();
        updateNavTreeControl(menuBar_->IsChecked(MENU_VIEW_TERMACCOUNTS));
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewStockAccounts(wxCommandEvent &event)
{
    m_mgr.GetPane(wxT("Stock Accounts")).Show(event.IsChecked());
    m_mgr.Update();

    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        createHomePage();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnIgnoreFutureTransactions(wxCommandEvent &event)
{
    m_mgr.GetPane(wxT("Ignore Future Transactions")).Show(event.IsChecked());
    mmIniOptions::instance().ignoreFutureTransactions_ = !mmIniOptions::instance().ignoreFutureTransactions_;
    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        createHomePage();
        updateNavTreeControl(menuBar_->IsChecked(MENU_VIEW_TERMACCOUNTS));
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnCategoryRelocation(wxCommandEvent& /*event*/)
{
    relocateCategoryDialog* dlg = new relocateCategoryDialog(m_core.get(), this);
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString msgStr;
        msgStr << _("Category Relocation Completed.") << wxT("\n\n")
               << wxString::Format( _("Records have been updated in the database: %s"),
                    dlg->updatedCategoriesCount().c_str())
               << wxT("\n\n")
               << _("MMEX must be shutdown and restarted for all the changes to be seen.");
        wxMessageBox(msgStr,_("Category Relocation Result"));
        mmOptions::instance().databaseUpdated_ = true;
    }
    homePanel_->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPayeeRelocation(wxCommandEvent& /*event*/)
{
    relocatePayeeDialog* dlg = new relocatePayeeDialog(m_core.get(), this);
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString msgStr;
        msgStr << _("Payee Relocation Completed.") << wxT("\n\n")
            << wxString::Format(_("Records have been updated in the database: %s"),
                dlg->updatedPayeesCount().c_str())
            << wxT("\n\n");
        wxMessageBox(msgStr, _("Payee Relocation Result"));
        mmOptions::instance().databaseUpdated_ = true;
    }
    homePanel_->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::RunCustomSqlDialog(wxString customReportSelectedItem)
{
    this->SetEvtHandlerEnabled(false);
    //Use Shared pointer to ensure mmCustomSQLDialog object gets destroyed.
    wxSharedPtr<mmCustomSQLDialog> dlg( new mmCustomSQLDialog(custRepIndex_, customReportSelectedItem, this ));

    int dialogStatus = wxID_MORE;
    while (dialogStatus == wxID_MORE)
    {
        if (dlg->sScript() != wxT(""))
        {
            wxBeginBusyCursor(wxHOURGLASS_CURSOR);
            mmCustomReport* csr = new mmCustomReport(this,
                m_core.get(), dlg->sReportTitle(), dlg->sScript(), dlg->sSctiptType());
            createReportsPage(csr);
            wxEndBusyCursor();
        }
        dialogStatus = dlg->ShowModal();
    }
    processPendingEvents();         // clear out pending events
    if (dialogStatus == wxID_OK) updateNavTreeControl();
    this->SetEvtHandlerEnabled(true);
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnEditCustomSqlReport(wxCommandEvent&)
{
    RunCustomSqlDialog(customSqlReportSelectedItem_);
}
//----------------------------------------------------------------------------

wxSizer* mmGUIFrame::cleanupHomePanel(bool new_sizer)
{
    wxASSERT(homePanel_);

    homePanel_->DestroyChildren();
    homePanel_->SetSizer(new_sizer ? new wxBoxSizer(wxHORIZONTAL) : 0);

    return homePanel_->GetSizer();
}
//----------------------------------------------------------------------------

void mmGUIFrame::SetDatabaseFile(wxString dbFileName, bool newDatabase)
{
    autoRepeatTransactionsTimer_.Stop();
    wxProgressDialog *progress = NULL;
    if (! newDatabase)
    {
        progress = new wxProgressDialog(
            _("Setting new Database file"),
            _("Please wait while the new database is being loaded."), 100, this);
        progress->Update(33);
    }

    // Ensure database is in a steady state first
    if (m_db && !activeHomePage_)
    {
        refreshRequested_ = true;
        createHomePage();
    }

    if (progress) progress->Update(65);
    if (openFile(dbFileName, newDatabase))
    {
        if (progress) progress->Update(80);
        recentFiles_->updateRecentList(dbFileName);
    }
    else
    {
        createHomePage();
        if (progress) progress->Update(90);
        updateNavTreeControl();
        showBeginAppDialog(true);
    }

    if (progress)
    {
        progress->Update(100);
        progress->Destroy();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::BackupDatabase(wxString filename, bool updateRequired)
{
    wxFileName fn(filename);
    if (!fn.IsOk()) return;

    wxString backupType = wxT("_start_");
    if (updateRequired) backupType = wxT("_update_");

    wxString backupName = filename + backupType + wxDateTime().Today().FormatISODate() + wxT(".") + fn.GetExt();
    if (updateRequired) // Create or update the backup file.
    {
        wxCopyFile(filename, backupName, true);
    }
    else                // create the backup if it does not exist
    {
        wxFileName fnBak(backupName);
        if (!fnBak.FileExists())
        {
            wxCopyFile(filename, backupName, true);
        }
    }

    // Get the list of created backup files for the given filename.
    wxArrayString backupFileArray;
    wxString fileSearch = filename + backupType + wxT("*.") + fn.GetExt();
    wxString backupFile = wxFindFirstFile(fileSearch);
    while (!backupFile.empty())
    {
        backupFileArray.Add(backupFile);
        backupFile = wxFindNextFile();
    }

    int max =  m_inisettings->GetIntSetting(wxT("MAX_BACKUP_FILES"), 4);
    if (backupFileArray.Count() > (size_t)max)
    {
        backupFileArray.Sort(true);
        // ensure file is not read only before deleting file.
        wxFileName fnLastFile(backupFileArray.Last());
        if (fnLastFile.IsFileWritable()) wxRemoveFile(backupFileArray.Last());
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnRecentFiles(wxCommandEvent& event)
{
    wxString file_name = recentFiles_->getRecentFile(event.GetId() - wxID_FILE1 +1);
    SetDatabaseFile(file_name);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnClearRecentFiles(wxCommandEvent& /*event*/)
{
     recentFiles_->clearRecentList();
}

void mmGUIFrame::setGotoAccountID(int account_id, long transID)
{
    gotoAccountID_ = account_id;
    gotoTransID_   = transID;
}

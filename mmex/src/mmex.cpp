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
#include "mmex.h"
#include "mmgraphtopcategories.h"
#include "mmhelppanel.h"
#include "mmhomepagepanel.h"
#include "mmreportspanel.h"
#include "newacctdialog.h"
#include "optionsdialog.h"
#include "paths.h"
#include "payeedialog.h"
#include "platfdep.h"
#include "relocatecategorydialog.h"
#include "relocatepayeedialog.h"
#include "reportbudgetcategorysummary.h"
#include "reportbudgetingperf.h"
#include "reportcashflow.h"
#include "reportcategexp.h"
#include "reportcategovertimeperf.h"
#include "reportincexpenses.h"
#include "reportincexpensesfinancialperiod.h"
#include "reportincexpesestime.h"
#include "reportpayee.h"
#include "reportsummary.h"
#include "reportsummaryassets.h"
#include "reportsummarystocks.h"
#include "reporttransactions.h"
#include "reporttransstats.h"
#include "stockspanel.h"
#include "univcsvdialog.h"
#include <boost/version.hpp>
#include <wx/aboutdlg.h>

//----------------------------------------------------------------------------

/* Include XPM Support */
#include "../resources/new.xpm"
#include "../resources/open.xpm"
#include "../resources/save.xpm"
#include "../resources/addaccount.xpm"
#include "../resources/accountlist.xpm"
#include "../resources/newacct.xpm"
#include "../resources/listview.xpm"
#include "../resources/homepagetree.xpm"
#include "../resources/budgeting.xpm"
#include "../resources/reporttree.xpm"
#include "../resources/billsdeposits.xpm"
#include "../resources/accounttree.xpm"
#include "../resources/help.xpm"
#include "../resources/stock.xpm"
#include "../resources/print.xpm"
#include "../resources/printpreview.xpm"
#include "../resources/printsetup.xpm"
#include "../resources/saveas.xpm"
#include "../resources/exit.xpm"
#include "../resources/options.xpm"
#include "../resources/about.xpm"
#include "../resources/issues.xpm"
#include "../resources/addacctwiz.xpm"
#include "../resources/notify.xpm"
#include "../resources/checkupdate.xpm"
#include "../resources/categories.xpm"
#include "../resources/payees.xpm"
#include "../resources/currency.xpm"
#include "../resources/appstart.xpm"
#include "../resources/edit_account.xpm"
#include "../resources/delete_account.xpm"
#include "../resources/filter.xpm"
#include "../resources/facebook.xpm"
#include "../resources/money.xpm"
//----------------------------------------------------------------------------

// Icons from Silk Collection
#include "../resources/house.xpm"
#include "../resources/moneyaccount.xpm"
#include "../resources/stock_curve.xpm"
#include "../resources/chartpiereport.xpm"
#include "../resources/car.xpm"
#include "../resources/clock.xpm"
#include "../resources/calendar.xpm"
#include "../resources/categoryedit.xpm"
#include "../resources/money_dollar.xpm"
#include "../resources/user_edit.xpm"
#include "../resources/wrench.xpm"
//
#include "../resources/customsql.xpm"
#include "../resources/termaccount.xpm"
#include "../resources/savings_acc_closed.xpm"
#include "../resources/savings_acc_favorite.xpm"
#include "../resources/term_acc_closed.xpm"
#include "../resources/term_acc_favorite.xpm"

//----------------------------------------------------------------------------
#include <wx/debugrpt.h>
#include <wx/sysopt.h>
#include <wx/wizard.h>

//----------------------------------------------------------------------------
#include <boost/scoped_array.hpp>
//----------------------------------------------------------------------------
#include "mmex_db_view.h"
#include <boost/foreach.hpp>

class mmPrintableBase;

namespace
{

const int REPEAT_TRANS_DELAY_TIME = 7000; // 7 seconds

class mmNewDatabaseWizard : public wxWizard
{
public:
    mmNewDatabaseWizard(wxFrame *frame, mmCoreDB* core);
    void RunIt(bool modal);

    mmCoreDB* m_core;

private:
    wxWizardPageSimple* page1;

    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------

class wxNewDatabaseWizardPage1 : public wxWizardPageSimple
{
public:
    wxNewDatabaseWizardPage1(mmNewDatabaseWizard* parent);

    void OnCurrency(wxCommandEvent& /*event*/);
    virtual bool TransferDataFromWindow();

private:
    mmNewDatabaseWizard* parent_;
    wxButton* itemButtonCurrency_;
    wxTextCtrl* itemUserName_;
    int currencyID_;

    wxString userName;

    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------

class mmAddAccountWizard : public wxWizard
{
public:
    mmAddAccountWizard(wxFrame *frame, mmCoreDB* core);
    void RunIt(bool modal);
    wxString accountName_;

    mmCoreDB* m_core;
    int acctID_;

private:
    wxWizardPageSimple* page1;
};
//----------------------------------------------------------------------------

class wxAddAccountPage1 : public wxWizardPageSimple
{
public:
    wxAddAccountPage1(mmAddAccountWizard* parent);
    virtual bool TransferDataFromWindow();

private:
    mmAddAccountWizard* parent_;
    wxTextCtrl* textAccountName_;
};
//----------------------------------------------------------------------------

class wxAddAccountPage2 : public wxWizardPageSimple
{
public:
    wxAddAccountPage2(mmAddAccountWizard *parent);
    virtual bool TransferDataFromWindow();

private:
    wxChoice* itemChoiceType_;
    mmAddAccountWizard* parent_;
};
//----------------------------------------------------------------------------

/*
    See also: wxStackWalker, wxDebugReportUpload.
*/
void reportFatalException(const wxDebugReport::Context& ctx)
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

bool OnInitImpl(mmGUIApp &app)
{
    app.SetAppName(mmex::GetAppName());

    wxFileConfig *config = new wxFileConfig("", "",
        mmex::GetSharedDir().GetPathWithSep()+ mmex::GetAppName() + ".conf",
        "", wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_SUBDIR );   
    wxConfigBase::Set(config);

    /* Setting Locale causes unexpected problems, so default to English Locale */
    app.getLocale().Init(wxLANGUAGE_ENGLISH);

    /* Initialize Image Handlers */
    wxImage::AddHandler(new wxICOHandler());
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());


    /* Load MMEX Custom Settings */
    mmIniOptions::instance().loadOptions();

    /* Load Colors from Database */
    mmLoadColorsFromDatabase();

    /* Was App Maximized? */
    bool isMax = config->ReadBool("ISMAXIMIZED", false);

    /* Load Dimensions of Window */
    long valx = config->ReadLong("ORIGINX", 50);
    long valy = config->ReadLong("ORIGINY", 50);
    long valw = config->ReadLong("SIZEW", 800);
    long valh = config->ReadLong("SIZEH", 600);

    mmSelectLanguage(0, false);

    mmGUIFrame *frame = new mmGUIFrame(mmex::getProgramName(), wxPoint(valx, valy), wxSize(valw, valh));
    bool ok = frame->Show();
    wxASSERT(ok);

    frame->Maximize(isMax);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return ok;
}
//----------------------------------------------------------------------------
} // namespace

//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(mmNewDatabaseWizard, wxWizard)
    EVT_WIZARD_CANCEL(wxID_ANY,   mmGUIFrame::OnWizardCancel)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxNewDatabaseWizardPage1, wxWizardPageSimple)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, wxNewDatabaseWizardPage1::OnCurrency)
END_EVENT_TABLE()
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
    EVT_MENU(MENU_TREEPOPUP_IMPORT_QIF, mmGUIFrame::OnPopupImportQIFile)

    EVT_TREE_ITEM_RIGHT_CLICK(ID_NAVTREECTRL, mmGUIFrame::OnItemRightClick)
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
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTQIF, mmGUIFrame::OnImportQIF)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTUNIVCSV, mmGUIFrame::OnImportUniversalCSV)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWALL, mmGUIFrame::OnTempViewAccounts)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE, mmGUIFrame::OnTempViewAccounts)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWOPEN, mmGUIFrame::OnTempViewAccounts)

    /* Custom Sql Reports */
    EVT_MENU(MENU_CUSTOM_SQL_REPORT_NEW, mmGUIFrame::OnNewCustomSqlReport)
    EVT_MENU(MENU_CUSTOM_SQL_REPORT_EDIT, mmGUIFrame::OnEditCustomSqlReport)
    EVT_MENU(MENU_CUSTOM_SQL_REPORT_DELETE, mmGUIFrame::OnDeleteCustomSqlReport)
    EVT_MENU(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_EDIT, mmGUIFrame::OnPopupEditCustomSqlReport)
    EVT_MENU(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_DELETE, mmGUIFrame::OnPopupDeleteCustomSqlReport)

    /*Automatic processing of repeat transactions*/
    EVT_TIMER(AUTO_REPEAT_TRANSACTIONS_TIMER_ID, mmGUIFrame::OnAutoRepeatTransactionsTimer)

    /* Recent Files */
    EVT_MENU(MENU_RECENT_FILES_CLEAR, mmGUIFrame::OnClearRecentFiles)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, mmGUIFrame::OnMRUFile)

END_EVENT_TABLE()
//----------------------------------------------------------------------------
IMPLEMENT_APP(mmGUIApp)
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

    wxAddAccountPage1* page2 = new wxAddAccountPage1(this);
    wxAddAccountPage2* page3 = new wxAddAccountPage2(this);

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
    displayMsg << _("The next pages will help you create a new database.") << ("\n\n")
               << _("Your database file is stored with an extension of .mmb.")<< ("\n\n")
               << _("As this file contains important financial information,\nwe recommended creating daily backups with the Options\nsetting: 'Backup before opening', and store your backups\nin a separate location.")<< ("\n\n")
               << _("The database can later be encrypted if required, by\nusing the option: 'Save database as' and changing the\nfile type before saving.");
    new wxStaticText(page1, wxID_ANY,displayMsg);

    wxNewDatabaseWizardPage1* page2 = new wxNewDatabaseWizardPage1(this);

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

mmGUIFrame::mmGUIFrame(const wxString& title,
                       const wxPoint& pos,
                       const wxSize& size
                      ) :
    wxFrame(0, -1, title, pos, size),
    gotoAccountID_(-1),
    homePageAccountSelect_(false),
    refreshRequested_(),
    autoRepeatTransactionsTimer_(this, AUTO_REPEAT_TRANSACTIONS_TIMER_ID),
    activeHomePage_(false),
    panelCurrent_(0),
    homePanel(),
    navTreeCtrl_(),
    menuBar_(),
    toolBar_(),
    selectedItemData_(),
    menuItemOnlineUpdateCurRate_(),
    helpFileIndex_(mmex::HTML_INDEX),
    activeTermAccounts_(false),
    expandedReportNavTree_(true),
    expandedCustomSqlReportNavTree_(false),
    expandedBudgetingNavTree_(true)
{
    // tell wxAuiManager to manage this frame
    m_mgr.SetManagedWindow(this);
    CreateStatusBar();
    wxStatusBar *sb = GetStatusBar();
    wxString mmex_revision = _MM_EX_REVISION_ID;
    mmex_revision.Replace("$", "");
    mmex_revision.Replace("Rev:", _("Revision:"));
    if (sb) sb->SetStatusText(mmex_revision);

    SetIcon(mmex::getProgramIcon());
    SetMinSize(wxSize(800,200));

    /* Setup Printer */
    printer_.reset(new wxHtmlEasyPrinting(mmex::getProgramName(), this));
    wxString printHeaderBase = mmex::getProgramName();
    printer_-> SetHeader( printHeaderBase + ("(@PAGENUM@/@PAGESCNT@)<hr>"), wxPAGE_ALL);

    //loadConfigFile(); // load from Settings DB
    restorePrinterValues();

    custRepIndex_ = new customSQLReportIndex();

    /* Create the Controls for the frame */
    createMenu();
    createToolBar();
    createControls();

    // add the toolbars to the manager
    m_mgr.AddPane(toolBar_, wxAuiPaneInfo().Name(("toolbar")).Caption(("Toolbar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));

    // change look and feel of wxAuiManager
    m_mgr.GetArtProvider()->SetMetric(16, 0);
    m_mgr.GetArtProvider()->SetMetric(3, 1);

    wxConfigBase *config = wxConfigBase::Get();
    m_mgr.LoadPerspective(config->Read("AUIPERSPECTIVE"));

    // "commit" all changes made to wxAuiManager
    m_mgr.Update();

    // enable or disable online update currency rate
    bool enableCurrencyUpd = config->ReadBool(INIDB_UPDATE_CURRENCY_RATE, true);
    if(enableCurrencyUpd)
    {
        if (menuItemOnlineUpdateCurRate_)
            menuItemOnlineUpdateCurRate_->Enable(true);
    }
    else
    {
        if (menuItemOnlineUpdateCurRate_)
            menuItemOnlineUpdateCurRate_->Enable(false);
    }

    // decide if we need to show app start dialog
    bool from_scratch = config->ReadBool("SHOWBEGINAPP", true);

    wxFileName dbpath = from_scratch ? wxGetEmptyString() : mmDBWrapper::getLastDbPath();

    if (from_scratch || !dbpath.IsOk())
    {
        menuEnableItems(false);
        createHomePage();
        updateNavTreeControl();
        showBeginAppDialog(dbpath.GetFullName().IsEmpty());
    }
    else
    {
        if (!openFile(dbpath.GetFullPath(), false)) showBeginAppDialog(true);
    }
}
//----------------------------------------------------------------------------

mmGUIFrame::~mmGUIFrame()
{
    cleanup();
}
//----------------------------------------------------------------------------

void mmGUIFrame::cleanup()
{
    wxConfigBase *config = wxConfigBase::Get();
    printer_.reset();

    //if (!fileName_.IsEmpty())   saveConfigFile();

    m_mgr.UnInit();

    /* Delete the GUI */
    cleanupHomePanel(false);

    if (m_core)  m_core.reset();
    if (m_db) m_db->Close();

    /// Update the database according to user requirements
    if (mmOptions::instance().databaseUpdated_ &&
       (config->ReadBool("BACKUPDB_UPDATE", false)))
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
wxTreeItemId mmGUIFrame::getTreeItemfor(const wxTreeItemId& itemID, const wxString& accountName) const
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
bool mmGUIFrame::setAccountInSection(const wxString& sectionName, const wxString& accountName)
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
bool mmGUIFrame::setNavTreeSection(const wxString& sectionName)
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
void mmGUIFrame::setAccountNavTreeSection(const wxString& accountName)
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
    bool autoExecuteManual; // Used when decoding: REPEATS
    bool autoExecuteSilent;
    bool requireExecution;

    static const char sql[] =
    "select "
        "b.BDID, "
        "b.ACCOUNTID, b.TOACCOUNTID, "
        "b.PAYEEID, b.TRANSCODE, b.TRANSAMOUNT, "
        "b.STATUS, b.TRANSACTIONNUMBER, "
        "b.NOTES, b.CATEGID, b.SUBCATEGID, "
        "b.TOTRANSAMOUNT, "
        "c.categname, sc.subcategname, "
        "b.REPEATS, "
        "b.NUMOCCURRENCES, "
        "b.NEXTOCCURRENCEDATE "
    "from BILLSDEPOSITS_V1 b "
        "left join category_v1 c on c.categid = b.categid "
        "left join subcategory_v1 sc on sc.subcategid = b.subcategid";

    mmDBWrapper::loadBaseCurrencySettings(m_db.get());
    wxSQLite3ResultSet q1 = m_db.get()->ExecuteQuery(sql);
    while (q1.NextRow())
    {
        mmBDTransactionHolder th;
        th.id_           = q1.GetInt(("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(("NEXTOCCURRENCEDATE")));

        th.nextOccurStr_   = mmGetDateForDisplay(m_db.get(), th.nextOccurDate_);
        th.payeeID_        = q1.GetInt(("PAYEEID"));
        th.transType_      = q1.GetString(("TRANSCODE"));
        th.accountID_      = q1.GetInt(("ACCOUNTID"));
        th.toAccountID_    = q1.GetInt(("TOACCOUNTID"));

        th.accountName_     = m_core.get()->getAccountName(th.accountID_);
        th.amt_            = q1.GetDouble(("TRANSAMOUNT"));
        th.toAmt_          = q1.GetDouble(("TOTRANSAMOUNT"));
        th.notes_       = q1.GetString(("NOTES"));
        th.categID_           = q1.GetInt(("CATEGID"));
        th.categoryStr_       = q1.GetString(("CATEGNAME"));
        th.subcategID_       = q1.GetInt(("SUBCATEGID"));
        th.subcategoryStr_ = q1.GetString(("SUBCATEGNAME"));

        // DeMultiplex the Auto Executable fields from the db entry: REPEATS
        int repeats        = q1.GetInt(("REPEATS"));
        int numRepeats     = q1.GetInt(("NUMOCCURRENCES"));

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

        int cid = 0, sid = 0;
        th.payeeStr_ = mmDBWrapper::getPayee(m_db.get(), th.payeeID_, cid, sid);

        if (th.transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            wxString fromAccount = m_core.get()->getAccountName(th.accountID_);
            wxString toAccount = m_core.get()->getAccountName(th.toAccountID_ );

            th.payeeStr_ = toAccount;
        }

        if (autoExecuteManual && requireExecution )
        {
            if ( (repeats < 11) || (numRepeats > 0) )
            {
                mmBDDialog repeatTransactionsDlg(m_db.get(), m_core.get(), th.id_ ,false ,true , this, SYMBOL_BDDIALOG_IDNAME , _(" Auto Repeat Transactions"));
                if ( repeatTransactionsDlg.ShowModal() == wxID_OK )
                {
                    if (activeHomePage_)
                    {
                        createHomePage(); // Update home page details only if it is being displayed
                    }
                }
            }
        }

        if (autoExecuteSilent && requireExecution)
        {
            if ( (repeats < 11) || (numRepeats > 0) )
            {
                boost::shared_ptr<mmBankTransaction> pTransaction;
                boost::shared_ptr<mmBankTransaction> pTemp(new mmBankTransaction(m_core.get()->db_));
                pTransaction = pTemp;

                boost::shared_ptr<mmCurrency> pCurrencyPtr = m_core.get()->getCurrencyWeakPtr(th.accountID_).lock();
                wxASSERT(pCurrencyPtr);

                pTransaction->accountID_ = th.accountID_;
                pTransaction->toAccountID_ = th.toAccountID_;
                pTransaction->payee_ = m_core.get()->getPayeeSharedPtr(th.payeeID_);
                pTransaction->transType_ = th.transType_;
                pTransaction->amt_ = th.amt_;
                pTransaction->status_ = q1.GetString(("STATUS"));
                pTransaction->transNum_ = q1.GetString(("TRANSACTIONNUMBER"));
                pTransaction->notes_ = th.notes_;
                pTransaction->category_ = m_core.get()->getCategorySharedPtr(th.categID_, th.subcategID_);
                pTransaction->date_ = th.nextOccurDate_;
                pTransaction->toAmt_ = th.toAmt_;

                boost::shared_ptr<mmSplitTransactionEntries> split(new mmSplitTransactionEntries());
                split->loadFromBDDB(m_core.get(),th.id_);
                *pTransaction->splitEntries_.get() = *split.get();

                pTransaction->updateAllData(m_core.get(), th.accountID_, pCurrencyPtr);
                m_core.get()->bTransactionList_.addTransaction(m_core.get(), pTransaction);
            }
            mmDBWrapper::completeBDInSeries(m_db.get(), th.id_);

            if (activeHomePage_)
            {
                createHomePage(); // Update home page details only if it is being displayed
            }
        }
    }
    q1.Finalize();
}
//----------------------------------------------------------------------------

/*
        Save our settings to ini db.
*/
void mmGUIFrame::saveConfigFile()
{
    wxConfigBase *config = wxConfigBase::Get();

    wxFileName fname(fileName_);
    config->Write("LASTFILENAME", fname.GetFullPath());

    /* Aui Settings */
    config->Write("AUIPERSPECTIVE", m_mgr.SavePerspective());

    // prevent values being saved while window is in an iconised state.
    if (this->IsIconized()) this->Restore();

    int valx = 0, valy = 0, valw = 0, valh = 0;

    this->GetPosition(&valx, &valy);
    this->GetSize(&valw, &valh);

    config->Write("ORIGINX", (long)valx);
    config->Write("ORIGINY", (long)valx);
    config->Write("SIZEW", (long)valw);
    config->Write("SIZEH", (long)valh);
    config->Write("ISMAXIMIZED", this->IsMaximized());
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
    menuBar_->FindItem(MENU_CUSTOM_SQL_REPORT_NEW)->Enable(enable);
    menuBar_->FindItem(MENU_CUSTOM_SQL_REPORT_EDIT)->Enable(enable);
    menuBar_->FindItem(MENU_CUSTOM_SQL_REPORT_DELETE)->Enable(enable);
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

    //navTreeCtrl_->SetBackgroundColour(mmColors::navTreeBkColor);

    wxImageList* imageList_ = new wxImageList(16, 16);
    imageList_->Add(wxBitmap(wxImage(house_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(moneyaccount_xpm).Scale(16, 16)));  // used for: savings_account
    imageList_->Add(wxBitmap(wxImage(clock_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(calendar_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(chartpiereport_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(help_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(stock_curve_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(car_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(customsql_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(termaccount_xpm).Scale(16, 16)));   // used for: term_account
    imageList_->Add(wxBitmap(wxImage(savings_acc_favorite_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(savings_acc_closed_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(term_acc_favorite_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(term_acc_closed_xpm).Scale(16, 16)));

    navTreeCtrl_->AssignImageList(imageList_);

    homePanel = new wxPanel( this, ID_PANEL,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxTR_SINGLE | wxNO_BORDER);

    m_mgr.AddPane(navTreeCtrl_, wxAuiPaneInfo().
        Name(("Navigation")).Caption(_("Navigation")).
        BestSize(wxSize(200,100)).MinSize(wxSize(100,100)).
        Left());

    m_mgr.AddPane(homePanel, wxAuiPaneInfo().
        Name(("Home")).Caption(("Home")).
        CenterPane().PaneBorder(false));
}
//----------------------------------------------------------------------------

void mmGUIFrame::updateNavTreeControl(bool expandTermAccounts)
{
    activeTermAccounts_ = false;
    // if no database is present yet, ignore testing for Term Accounts
    if (m_db && m_core->has_term_account())
    {
        activeTermAccounts_ = true;
    }

    navTreeCtrl_->DeleteAllItems();
    //navTreeCtrl_->SetBackgroundColour(mmColors::navTreeBkColor);

    wxTreeItemId root = navTreeCtrl_->AddRoot(_("Home Page"), 0, 0);
    navTreeCtrl_->SetItemData(root, new mmTreeItemData(("Home Page")));
    navTreeCtrl_->SetItemBold(root, true);
    navTreeCtrl_->SetFocus();


    wxTreeItemId accounts = navTreeCtrl_->AppendItem(root, _("Bank Accounts"), 1, 1);
    navTreeCtrl_->SetItemData(accounts, new mmTreeItemData(("Bank Accounts")));
    navTreeCtrl_->SetItemBold(accounts, true);

    wxTreeItemId termAccount;
    if ( hasActiveTermAccounts() )
    {
    //  Positioning for new type of accounts: Term Accounts
        termAccount = navTreeCtrl_->AppendItem(root, _("Term Accounts"), 9, 9);
        navTreeCtrl_->SetItemData(termAccount, new mmTreeItemData(("Term Accounts")));
        navTreeCtrl_->SetItemBold(termAccount, true);
    }

    wxTreeItemId stocks;
    stocks = navTreeCtrl_->AppendItem(root, _("Stocks"), 6, 6);
    navTreeCtrl_->SetItemData(stocks, new mmTreeItemData(("Stocks")));
    navTreeCtrl_->SetItemBold(stocks, true);

    if (mmIniOptions::instance().enableAssets_)
    {
        wxTreeItemId assets = navTreeCtrl_->AppendItem(root, _("Assets"), 7, 7);
        navTreeCtrl_->SetItemData(assets, new mmTreeItemData(("Assets")));
        navTreeCtrl_->SetItemBold(assets, true);
    }

    if (mmIniOptions::instance().enableRepeatingTransactions_)
    {
       wxTreeItemId bills = navTreeCtrl_->AppendItem(root, _("Repeating Transactions"), 2, 2);
       navTreeCtrl_->SetItemData(bills, new mmTreeItemData(("Bills & Deposits")));
       navTreeCtrl_->SetItemBold(bills, true);
    }

    wxTreeItemId budgeting;
    if (mmIniOptions::instance().enableBudget_)
    {
        budgeting = navTreeCtrl_->AppendItem(root, _("Budget Setup"), 3, 3);
        navTreeCtrl_->SetItemData(budgeting, new mmTreeItemData(("Budgeting")));
        navTreeCtrl_->SetItemBold(budgeting, true);
    }

    wxTreeItemId reports = navTreeCtrl_->AppendItem(root, _("Reports"), 4, 4);
    navTreeCtrl_->SetItemBold(reports, true);
    navTreeCtrl_->SetItemData(reports, new mmTreeItemData(NAVTREECTRL_REPORTS));

    /* ================================================================================================= */
    if (custRepIndex_->hasActiveSQLReports())
    {
        wxTreeItemId customSqlReports = navTreeCtrl_->AppendItem(root, _("Custom SQL Reports"), 8, 8);
        navTreeCtrl_->SetItemBold(customSqlReports, true);
        navTreeCtrl_->SetItemData(customSqlReports, new mmTreeItemData(NAVTREECTRL_CUSTOM_REPORTS));

        int reportNumber = -1;
        wxString reportNumberStr;
        wxTreeItemId customSqlReportRootItem;
        custRepIndex_->resetReportsIndex();

        wxString reportTitle = custRepIndex_->nextReportTitle();
        while (custRepIndex_->validTitle())
        {
            wxTreeItemId customSqlReportItem;
            if (custRepIndex_->reportIsSubReport() && reportNumber >= 0 )
            {
                customSqlReportItem = navTreeCtrl_->AppendItem(customSqlReportRootItem,reportTitle, 8, 8);
            }
            else
            {
                customSqlReportItem = navTreeCtrl_->AppendItem(customSqlReports,reportTitle, 8, 8);
                customSqlReportRootItem = customSqlReportItem;
            }
            reportNumberStr.Printf(("Custom_Report_%d"), ++reportNumber);
            navTreeCtrl_->SetItemData(customSqlReportItem, new mmTreeItemData(reportNumberStr));
            reportTitle = custRepIndex_->nextReportTitle();
        }

        if (expandedCustomSqlReportNavTree_)
            navTreeCtrl_->Expand(customSqlReports);
    }

    /* ================================================================================================= */

    wxTreeItemId reportsSummary = navTreeCtrl_->AppendItem(reports, _("Summary of Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(reportsSummary, new mmTreeItemData(("Summary of Accounts")));

    wxTreeItemId reportsStocks = navTreeCtrl_->AppendItem(reportsSummary, _("Stocks"), 4, 4);
    navTreeCtrl_->SetItemData(reportsStocks, new mmTreeItemData(("Summary of Stocks")));

    if (mmIniOptions::instance().enableAssets_)
    {
        wxTreeItemId reportsAssets = navTreeCtrl_->AppendItem(reportsSummary, _("Assets"), 4, 4);
        navTreeCtrl_->SetItemData(reportsAssets, new mmTreeItemData(("Summary of Assets")));
    }

    wxTreeItemId categsOverTime = navTreeCtrl_->AppendItem(reports, _("Where the Money Goes"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTime, new mmTreeItemData(("Where the Money Goes")));

    wxTreeItemId categsOverTimeCalMonth = navTreeCtrl_->AppendItem(categsOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCalMonth, new mmTreeItemData(("Where the Money Goes - Month")));

    wxString currentMonthMsg = _("Current Month");
    if (mmIniOptions::instance().ignoreFutureTransactions_) currentMonthMsg = _("Current Month to Date");

    wxTreeItemId categsOverTimeCurrentMonth = navTreeCtrl_->AppendItem(categsOverTime, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentMonth, new mmTreeItemData(("Where the Money Goes - Current Month")));

    wxTreeItemId categsOverTimeLast30 = navTreeCtrl_->AppendItem(categsOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLast30, new mmTreeItemData(("Where the Money Goes - 30 Days")));

    wxTreeItemId categsOverTimeLastYear = navTreeCtrl_->AppendItem(categsOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLastYear, new mmTreeItemData(("Where the Money Goes - Last Year")));

    wxTreeItemId categsOverTimeCurrentYear = navTreeCtrl_->AppendItem(categsOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentYear, new mmTreeItemData(("Where the Money Goes - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId categsOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(categsOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsOverTimeLastFinancialYear, new mmTreeItemData(("Where the Money Goes - Last Financial Year")));

        wxTreeItemId categsOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(categsOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsOverTimeCurrentFinancialYear, new mmTreeItemData(("Where the Money Goes - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId posCategs = navTreeCtrl_->AppendItem(reports, _("Where the Money Comes From"), 4, 4);
    navTreeCtrl_->SetItemData(posCategs, new mmTreeItemData(("Where the Money Comes From")));

    wxTreeItemId posCategsCalMonth = navTreeCtrl_->AppendItem(posCategs, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsCalMonth, new mmTreeItemData(("Where the Money Comes From - Month")));

    wxTreeItemId posCategsCurrentMonth = navTreeCtrl_->AppendItem(posCategs, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(posCategsCurrentMonth, new mmTreeItemData(("Where the Money Comes From - Current Month")));

    wxTreeItemId posCategsTimeLast30 = navTreeCtrl_->AppendItem(posCategs, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLast30, new mmTreeItemData(("Where the Money Comes From - 30 Days")));

    wxTreeItemId posCategsTimeLastYear = navTreeCtrl_->AppendItem(posCategs, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLastYear, new mmTreeItemData(("Where the Money Comes From - Last Year")));

    wxTreeItemId posCategsTimeCurrentYear = navTreeCtrl_->AppendItem(posCategs, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeCurrentYear, new mmTreeItemData(("Where the Money Comes From - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId posCategsTimeLastFinancialYear = navTreeCtrl_->AppendItem(posCategs, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(posCategsTimeLastFinancialYear, new mmTreeItemData(("Where the Money Comes From - Last Financial Year")));

        wxTreeItemId posCategsTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(posCategs, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(posCategsTimeCurrentFinancialYear, new mmTreeItemData(("Where the Money Comes From - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId categs = navTreeCtrl_->AppendItem(reports, _("Categories"), 4, 4);
    navTreeCtrl_->SetItemData(categs, new mmTreeItemData(("Categories - Over Time")));

    wxTreeItemId categsCalMonth = navTreeCtrl_->AppendItem(categs, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsCalMonth, new mmTreeItemData(("Categories - Month")));

    wxTreeItemId categsCurrentMonth = navTreeCtrl_->AppendItem(categs, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(categsCurrentMonth, new mmTreeItemData(("Categories - Current Month")));

    wxTreeItemId categsTimeLast30 = navTreeCtrl_->AppendItem(categs, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLast30, new mmTreeItemData(("Categories - 30 Days")));

    wxTreeItemId categsTimeLastYear = navTreeCtrl_->AppendItem(categs, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLastYear, new mmTreeItemData(("Categories - Last Year")));

    wxTreeItemId categsTimeCurrentYear = navTreeCtrl_->AppendItem(categs, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeCurrentYear, new mmTreeItemData(("Categories - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId categsTimeLastFinancialYear = navTreeCtrl_->AppendItem(categs, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsTimeLastFinancialYear, new mmTreeItemData(("Categories - Last Financial Year")));

        wxTreeItemId categsTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(categs, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsTimeCurrentFinancialYear, new mmTreeItemData(("Categories - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId payeesOverTime = navTreeCtrl_->AppendItem(reports, _("Payees"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTime, new mmTreeItemData(("Payee Report")));

    wxTreeItemId payeesOverTimeCalMonth = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCalMonth, new mmTreeItemData(("Payee Report - Month")));

    wxTreeItemId payeesOverTimeCurrentMonth = navTreeCtrl_->AppendItem(payeesOverTime, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentMonth, new mmTreeItemData(("Payee Report - Current Month")));

    wxTreeItemId payeesOverTimeLast30 = navTreeCtrl_->AppendItem(payeesOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLast30, new mmTreeItemData(("Payee Report - 30 Days")));

    wxTreeItemId payeesOverTimeLastYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLastYear, new mmTreeItemData(("Payee Report - Last Year")));

    wxTreeItemId payeesOverTimeCurrentYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentYear, new mmTreeItemData(("Payee Report - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId payeesOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(payeesOverTimeLastFinancialYear, new mmTreeItemData(("Payee Report - Last Financial Year")));

        wxTreeItemId payeesOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(payeesOverTimeCurrentFinancialYear, new mmTreeItemData(("Payee Report - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////////////

    wxTreeItemId incexpOverTime = navTreeCtrl_->AppendItem(reports, _("Income vs Expenses"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTime, new mmTreeItemData(("Income vs Expenses")));

    wxTreeItemId incexpOverTimeCalMonth = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCalMonth, new mmTreeItemData(("Income vs Expenses - Month")));

    wxTreeItemId incexpOverTimeCurrentMonth = navTreeCtrl_->AppendItem(incexpOverTime, currentMonthMsg, 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentMonth, new mmTreeItemData(("Income vs Expenses - Current Month")));

    wxTreeItemId incexpOverTimeLast30 = navTreeCtrl_->AppendItem(incexpOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLast30, new mmTreeItemData(("Income vs Expenses - 30 Days")));

    wxTreeItemId incexpOverTimeLastYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLastYear, new mmTreeItemData(("Income vs Expenses - Last Year")));

    wxTreeItemId incexpOverTimeCurrentYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentYear, new mmTreeItemData(("Income vs Expenses - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId incexpOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(incexpOverTimeLastFinancialYear, new mmTreeItemData(("Income vs Expenses - Last Financial Year")));

        wxTreeItemId incexpOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(incexpOverTimeCurrentFinancialYear, new mmTreeItemData(("Income vs Expenses - Current Financial Year")));
    }

    wxTreeItemId incexpmonthly = navTreeCtrl_->AppendItem(incexpOverTime, _("Income vs Expenses - All Time"), 4, 4);
    navTreeCtrl_->SetItemData(incexpmonthly, new mmTreeItemData(("Income vs Expenses - All Time")));

    //////////////////////////////////////////////////////////////////
    wxTreeItemId transactionList = navTreeCtrl_->AppendItem(reports, _("Transaction Report"), 4, 4);
    navTreeCtrl_->SetItemData(transactionList, new mmTreeItemData(("Transaction Report")));

    ///////////////////////////////////////////////////////////////////

    if (m_db && mmIniOptions::instance().enableBudget_)
    {
        static const char sql[] =
        "select BUDGETYEARID, BUDGETYEARNAME "
        "from BUDGETYEAR_V1 "
        "order by BUDGETYEARNAME";

        wxTreeItemId budgetPerformance;
        wxTreeItemId budgetSetupPerformance;

        wxSQLite3ResultSet q1 = m_db->ExecuteQuery(sql);

        for (size_t i = 0; q1.NextRow(); ++i)
        {
            if (!i) { // first loop only
                budgetPerformance = navTreeCtrl_->AppendItem(reports, _("Budget Performance"), 4, 4);
                navTreeCtrl_->SetItemData(budgetPerformance, new mmTreeItemData(("Budget Performance")));

                budgetSetupPerformance = navTreeCtrl_->AppendItem(reports, _("Budget Category Summary"), 4, 4);
                navTreeCtrl_->SetItemData(budgetSetupPerformance, new mmTreeItemData(("Budget Setup Performance")));
            }

            int id = q1.GetInt(("BUDGETYEARID"));
            const wxString name = q1.GetString(("BUDGETYEARNAME"));

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
    navTreeCtrl_->SetItemData(cashFlow, new mmTreeItemData(("Cash Flow")));

    wxTreeItemId cashflowWithBankAccounts = navTreeCtrl_->AppendItem(cashFlow, _("Cash Flow - With Bank Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(cashflowWithBankAccounts, new mmTreeItemData(("Cash Flow - With Bank Accounts")));

    if ( hasActiveTermAccounts() )
    {
        wxTreeItemId cashflowWithTermAccounts = navTreeCtrl_->AppendItem(cashFlow, _("Cash Flow - With Term Accounts"), 4, 4);
        navTreeCtrl_->SetItemData(cashflowWithTermAccounts, new mmTreeItemData(("Cash Flow - With Term Accounts")));
    }

    wxTreeItemId cashflowSpecificAccounts = navTreeCtrl_->AppendItem(cashFlow, _("Cash Flow - Specific Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(cashflowSpecificAccounts, new mmTreeItemData(("Cash Flow - Specific Accounts")));

    ///////////////////////////////////////////////////////
    wxTreeItemId transactionStats = navTreeCtrl_->AppendItem(reports, _("Transaction Statistics"), 4, 4);
    navTreeCtrl_->SetItemData(transactionStats, new mmTreeItemData(("Transaction Statistics")));

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

    wxConfigBase *config = wxConfigBase::Get();
    wxString vAccts = config->Read("VIEWACCOUNTS", "ALL");

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->rangeAccount();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* account = it->get();
        // Checking/Bank Accounts
        if (account->acctType_ == ACCOUNT_TYPE_BANK)
        {
            const mmAccount* pCA = account;
            if ((vAccts == ("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == ("Favorites") && pCA->favoriteAcct_) ||
                (vAccts == ("ALL")))
            {
                int selectedImage = 1;
                if (pCA->status_ == mmAccount::MMEX_Closed)
                    selectedImage = 11;
                else if (pCA->favoriteAcct_)
                    selectedImage = 10;

                wxTreeItemId tacct = navTreeCtrl_->AppendItem(accounts, pCA->name_, selectedImage, selectedImage);
                navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(pCA->id_, false));
            }
        }
        // Term Accounts
        else if (account->acctType_ == ACCOUNT_TYPE_TERM)
        {
           const mmAccount* pTA = account;
            if ((vAccts == ("Open") && pTA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == ("Favorites") && pTA->favoriteAcct_) ||
                (vAccts == ("ALL")))
            {
                int selectedImage = 9;
                if (pTA->status_ == mmAccount::MMEX_Closed)
                    selectedImage = 13;
                else if (pTA->favoriteAcct_)
                    selectedImage = 12;

                wxTreeItemId tacct = navTreeCtrl_->AppendItem(termAccount, pTA->name_, selectedImage, selectedImage);
                navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(pTA->id_, false));
            }
        }
        // Stock Accounts
        else //if (account->acctType_ == ACCOUNT_TYPE_STOCK)
        {
            const mmAccount* pIA = account;
            if ((vAccts == ("Open") && pIA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == ("Favorites") && pIA->favoriteAcct_) ||
                (vAccts == ("ALL")))
            {
                wxTreeItemId tacct = navTreeCtrl_->AppendItem(stocks, pIA->name_, 6, 6);
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

}
//----------------------------------------------------------------------------

wxDateTime mmGUIFrame::getUserDefinedFinancialYear(bool prevDayRequired) const
{
    long monthNum;
    mmOptions::instance().financialYearStartMonthString_.ToLong(&monthNum);

    if (monthNum > 0) //Test required for compatability with previous version
        monthNum --;

    wxDateTime today = wxDateTime::Now();
    int year = today.GetYear();
    if (today.GetMonth() < monthNum) year -- ;

    long dayNum;
    wxString dayNumStr = mmOptions::instance().financialYearStartDayString_;
    dayNumStr.ToLong(&dayNum);
    if ((dayNum < 1) || (dayNum > 31 )) {
        dayNum = 1;
    } else if (((monthNum == wxDateTime::Feb) && (dayNum > 28)) ||
        (((monthNum == wxDateTime::Sep) || (monthNum == wxDateTime::Apr) ||
           (monthNum == wxDateTime::Jun) || (monthNum == wxDateTime::Nov)) && (dayNum > 29)))
    {
        dayNum = 1;
    }

    wxDateTime financialYear = wxDateTime(today);
    financialYear.SetDay(dayNum);
    financialYear.SetMonth((wxDateTime::Month)monthNum);
    financialYear.SetYear(year);
    if (prevDayRequired)
        financialYear.Subtract(wxDateSpan::Day());
    return financialYear;
}

void mmGUIFrame::CreateCustomReport(int index)
{
    wxProgressDialog* progressBar = new wxProgressDialog(_("Report printing in progress")
        , wxEmptyString, 100, NULL, wxPD_AUTO_HIDE|wxPD_CAN_ABORT);

    wxString rfn = custRepIndex_->reportFileName(index);
    if (rfn != (""))
    {
        wxString sqlStr;
        if (custRepIndex_->getSqlFileData(sqlStr) )
        {
            mmCustomSQLReport* csr = new mmCustomSQLReport(m_core.get(), custRepIndex_->currentReportTitle(), sqlStr, progressBar);
            menuPrintingEnable(true);
            createReportsPage(csr);
        }
    }
    progressBar->Destroy();
}

bool mmGUIFrame::CustomSQLReportSelected( int& customSqlReportID, mmTreeItemData* iData )
{
    wxStringTokenizer tk(iData->getString(), ("_"));
    wxString tk1 = tk.GetNextToken();
    wxString tk2 = tk.GetNextToken();
    wxString indexStr = tk.GetNextToken();
    bool result = false;
    if (tk1 == ("Custom") && tk2 == ("Report") && indexStr.IsNumber())
    {
        long index;
        indexStr.ToLong(&index);
        customSqlReportID = index;
        result = true;
    }
    return result;
}

void mmGUIFrame::OnTreeItemExpanded(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(id));

    if (iData->getString() == NAVTREECTRL_REPORTS)
        expandedReportNavTree_ = true;
    else if (iData->getString() == NAVTREECTRL_CUSTOM_REPORTS)
        expandedCustomSqlReportNavTree_ = true;
    else if (iData->getString() == ("Budgeting"))
        expandedBudgetingNavTree_ = true;
}

void mmGUIFrame::OnTreeItemCollapsed(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(id));

    if (iData->getString() == NAVTREECTRL_REPORTS)
        expandedReportNavTree_ = false;
    else if (iData->getString() == NAVTREECTRL_CUSTOM_REPORTS)
        expandedCustomSqlReportNavTree_ = false;
    else if (iData->getString() == ("Budgeting"))
        expandedBudgetingNavTree_ = false;
}

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
#ifdef __WXMSW__
            Freeze();
#endif
            wxTreeItemId idparent = navTreeCtrl_->GetItemParent(id);
            mmTreeItemData* iParentData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(idparent));
            if (iParentData->getString() == ("Budget Performance"))
            {
                wxProgressDialog proDlg(_("Budget Performance"), reportWaitingMsg, 100, this);
                mmPrintableBase* rs = new mmReportBudgetingPerformance(m_core.get(), this, data);
                proDlg.Update(30);
                menuPrintingEnable(true);
                createReportsPage(rs);
                proDlg.Update(95);
            }
            else if (iParentData->getString() == ("Budget Setup Performance"))
            {
                wxProgressDialog proDlg(_("Budget Category Summary"), reportWaitingMsg, 100, this);
                mmPrintableBase* rs = new mmReportBudgetCategorySummary(m_core.get(), this, data);
                proDlg.Update(30);
                menuPrintingEnable(true);
                createReportsPage(rs);
                proDlg.Update(95);
            }
            else
            {
                wxProgressDialog proDlg(_("Budget Setup"), reportWaitingMsg, 100, this);
                createBudgetingPage(data);
                proDlg.Update(95);
            }
#ifdef __WXMSW__
            Thaw();
#endif
        }
        else
        {
            boost::shared_ptr<mmAccount> pAccount = m_core->getAccountSharedPtr(data);
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
#ifdef __WXMSW__
                    Freeze();
#endif
                    wxSizer *sizer = cleanupHomePanel();
                
                    panelCurrent_ = new mmStocksPanel(m_db.get(), m_core.get(), data, homePanel, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
                    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 5);
                    
                    homePanel->Layout();
#ifdef __WXMSW__
                    Thaw();
#endif
                }
            }
        }
    }
    else
    {
        if (iData->getString() == ("Home Page"))
        {
            createHomePage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_HELP)
        {
            helpFileIndex_ = mmex::HTML_INDEX;
            menuPrintingEnable(true);
            createHelpPage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_CUSTOM_REPORTS)
        {
            helpFileIndex_ = mmex::HTML_CUSTOM_SQL;
            menuPrintingEnable(true);
            createHelpPage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_INVESTMENT)
        {
            helpFileIndex_ = mmex::HTML_INVESTMENT;
            menuPrintingEnable(true);
            createHelpPage();
            return;
        }
        else if (iData->getString() == NAVTREECTRL_BUDGET)
        {
            helpFileIndex_ = mmex::HTML_BUDGET;
            menuPrintingEnable(true);
            createHelpPage();
            return;
        }

        if (!m_core || !m_db)
            return;

        //========================================================================
        int customSqlReportID;      // Define before all the if...else statements
        //========================================================================

        if (iData->getString() == ("Summary of Accounts"))
        {
            mmPrintableBase* rs = new mmReportSummary(m_core.get(), this);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Summary of Stocks"))
        {
            mmPrintableBase* rs = new mmReportSummaryStocks(m_core.get(), m_db.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if ( CustomSQLReportSelected(customSqlReportID, iData) )
        {
            CreateCustomReport(customSqlReportID);
        }
        else if (iData->getString() == ("Summary of Assets"))
        {
            mmPrintableBase* rs = new mmReportSummaryAssets(m_core.get(), m_db.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Categories - Over Time"))
        {
            wxProgressDialog proDlg(_("Category Report"), _("Category Report being generated... Please wait."), 100, this);
            mmPrintableBase* rs = new mmReportCategoryOverTimePerformance(m_core.get());
            proDlg.Update(70);
            menuPrintingEnable(true);
            createReportsPage(rs);
            proDlg.Update(95);
        }
        else if (iData->getString() == ("Categories - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime::Month cm = today.GetMonth();
            int numDays = 0;
            if (cm == wxDateTime::Jan)
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(wxDateTime::Dec));
            else
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(cm-1));

            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Days(numDays));
            wxString title = _("Categories - Last Calendar Month");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Categories - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            wxString title = _("Categories - Last 30 Days");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Categories - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now().GetLastMonthDay();
            wxString title = _("Categories - Current Month");
            if (mmIniOptions::instance().ignoreFutureTransactions_)
            {
                title = _("Categories - Current Month to Date");
                dtEnd = wxDateTime::Now();
            }
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Categories - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime prevYearEnd = wxDateTime(today);
            prevYearEnd.SetYear(year);
            prevYearEnd.SetMonth(wxDateTime::Dec);
            prevYearEnd.SetDay(31);
            wxDateTime dtEnd = prevYearEnd;
            wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
            wxString title = _("Categories - Last Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Categories - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime yearBegin = wxDateTime(today);
            yearBegin.SetYear(year);
            yearBegin.SetMonth(wxDateTime::Dec);
            yearBegin.SetDay(31);
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = yearBegin;
            wxString title = _("Categories - Current Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd,title,0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Categories - Last Financial Year"))
        {
            wxDateTime refDate = wxDateTime(getUserDefinedFinancialYear(true));
            wxDateTime dtEnd = refDate;
            wxDateTime dtBegin = refDate.Subtract(wxDateSpan::Year());
            wxString title = _("Categories - Last Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Categories - Current Financial Year"))
        {
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear(true));
            wxDateTime dtEnd   = wxDateTime::Now();
            wxString title = _("Categories - Current Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From"))
        {
            wxDateTime dtEnd =  wxDateTime::Now();
            wxDateTime dtBegin =  wxDateTime::Now();
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), true, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime::Month cm = today.GetMonth();
            int numDays = 0;
            if (cm == wxDateTime::Jan)
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(wxDateTime::Dec));
            else
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(cm-1));
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Days(numDays));
            wxString title = _("Where the Money Comes From - Last Calendar Month");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            wxString title = _("Where the Money Comes From - Last 30 days");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now().GetLastMonthDay();
            wxString title = _("Where the Money Comes From - Current Month");
            if (mmIniOptions::instance().ignoreFutureTransactions_)
            {
                title = _("Where the Money Comes From - Current Month to Date");
                dtEnd  = wxDateTime::Now();
            }
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime prevYearEnd = wxDateTime(today);
            prevYearEnd.SetYear(year);
            prevYearEnd.SetMonth(wxDateTime::Dec);
            prevYearEnd.SetDay(31);
            wxDateTime dtEnd = prevYearEnd;
            wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
            wxString title = _("Where the Money Comes From - Last year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime yearBegin = wxDateTime(today);
            yearBegin.SetYear(year);
            yearBegin.SetMonth(wxDateTime::Dec);
            yearBegin.SetDay(31);
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = yearBegin;
            wxString title = _("Where the Money Comes From - Current Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From - Last Financial Year"))
        {
            wxDateTime refDate = wxDateTime(getUserDefinedFinancialYear(true));
            wxDateTime dtEnd = refDate;
            wxDateTime dtBegin = refDate.Subtract(wxDateSpan::Year());
            wxString title = _("Where the Money Comes From - Last Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Comes From - Current Financial Year"))
        {
            wxDateTime dtEnd   = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear(true));

            wxString title = _("Where the Money Comes From - Current Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes"))
        {
            wxDateTime dtEnd = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime::Now();
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), true, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime::Month cm = today.GetMonth();
            int numDays = 0;
            if (cm == wxDateTime::Jan)
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(wxDateTime::Dec));
            else
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(cm-1));
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Days(numDays));
            wxString title = _("Where the Money Goes - Last Calendar Month");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            wxString title = _("Where the Money Goes - Last 30 Days");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now().GetLastMonthDay();
            wxString title = _("Where the Money Goes - Current Month");
            if (mmIniOptions::instance().ignoreFutureTransactions_)
            {
                title = _("Where the Money Goes - Current Month to Date");
                dtEnd  = wxDateTime::Now();
            }
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime prevYearEnd = wxDateTime(today);
            prevYearEnd.SetYear(year);
            prevYearEnd.SetMonth(wxDateTime::Dec);
            prevYearEnd.SetDay(31);
            wxDateTime dtEnd = prevYearEnd;
            wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
            wxString title = _("Where the Money Goes - Last Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime yearBegin = wxDateTime(today);
            yearBegin.SetYear(year);
            yearBegin.SetMonth(wxDateTime::Dec);
            yearBegin.SetDay(31);
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = yearBegin;
            wxString title = _("Where the Money Goes - Current Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes - Last Financial Year"))
        {
            wxDateTime refDate = (getUserDefinedFinancialYear(true));
            wxDateTime dtEnd = refDate;
            wxDateTime dtBegin = refDate.Subtract(wxDateSpan::Year());
            wxString title = _("Where the Money Goes - Last Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Where the Money Goes - Current Financial Year"))
        {
            wxDateTime dtEnd   = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear(true));
            wxString title = _("Where the Money Goes - Current Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Transaction Statistics"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportTransactionStats(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime::Month cm = today.GetMonth();
            int numDays = 0;
            if (cm == wxDateTime::Jan)
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(wxDateTime::Dec));
            else
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(cm-1));
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Days(numDays));
            wxString title = _("Income vs Expenses - Last Calendar Month");
            mmPrintableBase* rs = new mmReportIncomeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            wxString title = _("Income vs Expenses - Last 30 Days");
            mmPrintableBase* rs = new mmReportIncomeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now().GetLastMonthDay();
            wxString title = _("Income vs Expenses - Current Month");
            if (mmIniOptions::instance().ignoreFutureTransactions_)
            {
                title = _("Income vs Expenses - Current Month to Date");
                dtEnd  = wxDateTime::Now();
            }
            mmPrintableBase* rs = new mmReportIncomeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - Last Financial Year"))
        {
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear());

            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            if (today.GetMonth() < dtBegin.GetMonth()) year -- ;
            mmPrintableBase* rs = new mmReportIncExpensesOverFinancialPeriod(this, m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - Current Financial Year"))
        {
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear());

            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            if (today.GetMonth() < dtBegin.GetMonth()) year --;
            mmPrintableBase* rs = new mmReportIncExpensesOverFinancialPeriod(this, m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Income vs Expenses - All Time"))
        {
            wxString title = _("Income vs Expenses - All Time");
            mmPrintableBase* rs = new mmReportIncomeExpenses(m_core.get(), true, wxDateTime::Now(), wxDateTime::Now(), title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report"))
        {
            wxString title = _("Payee Report");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), true, wxDateTime::Now(), wxDateTime::Now(), title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime::Month cm = today.GetMonth();
            int numDays = 0;
            if (cm == wxDateTime::Jan)
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(wxDateTime::Dec));
            else
                numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(cm-1));

            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Days(numDays));
            dtBegin.Add(wxDateSpan::Day());

            wxString title = _("Payees - Last Calendar Month");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Days(30));

            wxString title = _("Payees - Last 30 Days");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now().GetLastMonthDay();
            dtBegin.Add(wxDateSpan::Day());

            wxString title = _("Payees - Current Month");
            if (mmIniOptions::instance().ignoreFutureTransactions_)
            {
                title = _("Payees - Current Month to Date");
                dtEnd  = wxDateTime::Now();
            }
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime prevYearEnd = wxDateTime(today);
            prevYearEnd.SetYear(year);
            prevYearEnd.SetMonth(wxDateTime::Dec);
            prevYearEnd.SetDay(31);
            wxDateTime dtEnd = prevYearEnd;
            wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
            dtBegin.Add(wxDateSpan::Day());

            wxString title = _("Payees - Last Year");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime yearBegin = wxDateTime(today);
            yearBegin.SetYear(year);
            yearBegin.SetMonth(wxDateTime::Dec);
            yearBegin.SetDay(31);
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = yearBegin;
            dtBegin.Add(wxDateSpan::Day());

            wxString title = _("Payees - Current Year");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report - Last Financial Year"))
        {
            wxDateTime refDate = wxDateTime(getUserDefinedFinancialYear());
            refDate.Subtract(wxDateSpan::Day());
            wxDateTime dtEnd = refDate;
            wxDateTime dtBegin = refDate.Subtract(wxDateSpan::Year());
            dtBegin.Add(wxDateSpan::Day());

            wxString title = _("Payees - Last Financial Year");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Payee Report - Current Financial Year"))
        {
            wxDateTime dtEnd   = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear());

            wxString title = _("Payees - Current Financial Year");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == ("Cash Flow"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this);

            report->activateBankAccounts();
            if (hasActiveTermAccounts())  report->activateTermAccounts();

            menuPrintingEnable(true);
            createReportsPage(report);
        }
        else if (iData->getString() == ("Cash Flow - With Bank Accounts"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this);

            report->activateBankAccounts();

            menuPrintingEnable(true);
            createReportsPage(report);
        }

        else if (iData->getString() == ("Cash Flow - With Term Accounts"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this);

            report->activateTermAccounts();

            menuPrintingEnable(true);
            createReportsPage(report);
        }
        else if (iData->getString() == ("Cash Flow - Specific Accounts"))
        {
            navTreeCtrl_->UnselectAll();    // item in navTreeCtrl_ to enable re-selection.
            processPendingEvents();         // Clear event buffer before activating report.
            OnCashFlowSpecificAccounts();
        }
        else if (iData->getString() == ("Transaction Report"))
        {
            navTreeCtrl_->UnselectAll();    // item in navTreeCtrl_ to enable re-selection.
            processPendingEvents();         // Clear event buffer before activating report.
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TRANSACTIONREPORT);
            AddPendingEvent(evt);           // Events will be processed in due course.
        }
        else if (iData->getString() == ("Bills & Deposits"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BILLSDEPOSITS);
            AddPendingEvent(evt);
        }
        else if (iData->getString() == ("Assets"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ASSETS);
            AddPendingEvent(evt);
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnLaunchAccountWebsite(wxCommandEvent& /*event*/)
{
   if (selectedItemData_)
   {
      int data = selectedItemData_->getData();
      boost::shared_ptr<mmAccount> pAccount = m_core->getAccountSharedPtr(data);
      if (pAccount)
      {
         wxString website = pAccount->website_;
         if (!website.IsEmpty()) wxLaunchDefaultBrowser(website);
         return;
      }
   }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPopupImportQIFile(wxCommandEvent& /*event*/)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        boost::shared_ptr<mmAccount> pAccount = m_core->getAccountSharedPtr(data);
        if (pAccount)
        {
           wxString acctType = pAccount->acctType_;
           if (acctType == ACCOUNT_TYPE_BANK || acctType == ACCOUNT_TYPE_TERM)
           {
                int accountID = mmImportQIF(m_core.get(), pAccount->name_);
                if (accountID != -1)
                {
                    setAccountNavTreeSection(m_core.get()->getAccountName(accountID));
                    createCheckingAccountPage(accountID);
                }
           }
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPopupEditAccount(wxCommandEvent& /*event*/)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        boost::shared_ptr<mmAccount> pAccount = m_core->getAccountSharedPtr(data);
        if (pAccount)
        {
           wxString acctType = pAccount->acctType_;
           if (acctType == ACCOUNT_TYPE_BANK || acctType == ACCOUNT_TYPE_STOCK || acctType == ACCOUNT_TYPE_TERM)
           {
              mmNewAcctDialog dlg(m_core.get(), false, data, this);
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
        boost::shared_ptr<mmAccount> pAccount = m_core->getAccountSharedPtr(data);
        if (pAccount)
        {
           wxMessageDialog msgDlg(this,
              _("Do you really want to delete the account?"),
              _("Confirm Account Deletion"),
              wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
           if (msgDlg.ShowModal() == wxID_YES)
           {
              m_core->deleteAccount(pAccount->id_);
              m_core->bTransactionList_.deleteTransactions(pAccount->id_);
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
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnItemRightClick(wxTreeEvent& event)
{
    if (menuBar_->FindItem(MENU_ORGCATEGS)->IsEnabled() )
        showTreePopupMenu(event.GetItem(), event.GetPoint());
    else
        wxMessageBox(_("MMEX has been opened without an active database."),_("MMEX: Menu Popup Error"),wxICON_EXCLAMATION);
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
            boost::shared_ptr<mmAccount> pAccount = m_core->getAccountSharedPtr(data);
            if (pAccount)
            {
                wxString acctType = pAccount->acctType_;
                if (acctType == ACCOUNT_TYPE_BANK || acctType == ACCOUNT_TYPE_TERM || acctType == ACCOUNT_TYPE_STOCK)
                {
                    wxMenu menu;
//                  menu.Append(MENU_TREEPOPUP_GOTO, _("&Go To.."));
                    menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Account"));
                    menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Account"));
                    // Don't allow QIF importing to Investment type accounts
                    if (acctType == ACCOUNT_TYPE_BANK || acctType == ACCOUNT_TYPE_TERM )
                    {
                        menu.AppendSeparator();
                        menu.Append(MENU_TREEPOPUP_IMPORT_QIF, _("Import &QIF File"));
                    }
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
        if (iData->getString() == ("Bank Accounts") ||
            iData->getString() == ("Term Accounts") ||
            iData->getString() == ("Stocks"))
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
            if ( (iData->getString() != ("Term Accounts")) && (iData->getString() != ("Stocks")) )
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
            // Bring up popup menu to edit or delete the correct Custom SQL Report
            customSqlReportSelectedItem_ = iData->getString();
            wxString field = customSqlReportSelectedItem_.Mid(6,8);
            if (field == ("_Report_"))
            {
                wxMenu* customReportMenu = new wxMenu;
                customReportMenu->Append(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_EDIT, _("Edit Custom Report"));
                customReportMenu->Append(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_DELETE, _("Delete Custom Report"));
                PopupMenu(&*customReportMenu, pt);
            }
            else if (iData->getString() == ("Budgeting"))
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BUDGETSETUPDIALOG);
                AddPendingEvent(evt);
            }
        }
    }
}

void mmGUIFrame::OnTempViewAccounts(wxCommandEvent& event)
{
    wxConfigBase *config = wxConfigBase::Get();
    //Get current settings for view accounts
	wxString old_view_type = config->Read("VIEWACCOUNTS", "ALL");
    wxString view_type = "ALL";
    if (event.GetId() == MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE) view_type = "Favorites";
    else if (event.GetId() == MENU_TREEPOPUP_ACCOUNT_VIEWOPEN) view_type = "Open";
    config->Write("VIEWACCOUNTS", view_type);

    //Refresh Navigation Panel
    mmGUIFrame::updateNavTreeControl();

    //Restore settings
	config->Write("VIEWACCOUNTS", old_view_type);
}

void mmGUIFrame::createBudgetingPage(int budgetYearID)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmBudgetingPanel(m_db.get(), m_core.get(), this, budgetYearID,
        homePanel, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::createHomePage()
{
    wxSizer *sizer = cleanupHomePanel();

    if (panelCurrent_)
    {
//        delete panelCurrent_;
        panelCurrent_  = 0;
    }
    panelCurrent_ = new mmHomePagePanel(this
            , m_db.get()
            , m_core.get()
            , m_topCategories
            , homePanel
            , ID_PANEL3
            , wxDefaultPosition
            , wxDefaultSize
            , wxNO_BORDER|wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
    refreshRequested_ = false;
}
//----------------------------------------------------------------------------

void mmGUIFrame::createReportsPage(mmPrintableBase* rs)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmReportsPanel(this, m_db.get(), rs, homePanel, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    wxStatusBar *sb = GetStatusBar();
    if (sb) sb->SetStatusText(rs->get_version());

    homePanel->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::createHelpPage()
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmHelpPanel(this, m_db.get(), homePanel, ID_PANEL3,
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
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

    wxMenuItem* menuItemNew = new wxMenuItem(menu_file,
        MENU_NEW,_("&New Database\tCtrl-N"),_("New Database"));
    menuItemNew->SetBitmap(toolBarBitmaps[0]);
    wxMenuItem* menuItemOpen = new wxMenuItem(menu_file,
        MENU_OPEN,_("&Open Database\tCtrl-O"),_("Open Database"));
    menuItemOpen->SetBitmap(toolBarBitmaps[1]);
    wxMenuItem* menuItemSaveAs = new wxMenuItem(menu_file,
        MENU_SAVE_AS,_("Save Database &As"),_("Save Database As"));
    menuItemSaveAs->SetBitmap(wxBitmap(saveas_xpm));
    menu_file->Append(menuItemNew);
    menu_file->Append(menuItemOpen);
    menu_file->Append(menuItemSaveAs);
    menu_file->AppendSeparator();

    //Files History
    wxConfigBase *config = wxConfigBase::Get();

    menuRecentFiles_ = new wxMenu;
    history_ = new wxFileHistory(9, wxID_FILE1);
    history_->UseMenu(menuRecentFiles_);
    history_->Load(*config);
    menu_file->Append(MENU_RECENT_FILES, _("&Recent Files..."), menuRecentFiles_);
    wxMenuItem* menuClearRecentFiles = new wxMenuItem(menu_file,
        MENU_RECENT_FILES_CLEAR,_("&Clear Recent Files"));
    menuClearRecentFiles->SetBitmap(toolBarBitmaps[9]);
    menu_file->Append(menuClearRecentFiles);
    menu_file->AppendSeparator();

    //Export
    wxMenu* exportMenu = new wxMenu;
    exportMenu->Append(MENU_EXPORT_CSV, _("&CSV Files..."), _("Export to CSV"));
    exportMenu->Append(MENU_EXPORT_QIF, _("&QIF Files..."), _("Export to QIF"));
    exportMenu->Append(MENU_EXPORT_HTML, _("&Report to HTML"), _("Export to HTML"));
    menu_file->Append(MENU_EXPORT, _("&Export"), exportMenu);

    wxMenu* importMenu = new wxMenu;
    importMenu->Append(MENU_IMPORT_UNIVCSV, _("&CSV Files..."), _("Import from any CSV file"));
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
    wxMenuItem* menuItemCategoryRelocation = new wxMenuItem(menuRelocation,
        MENU_CATEGORY_RELOCATION,_("&Categories..."),_("Reassign all categories to another category"));
       menuItemCategoryRelocation->SetBitmap(wxBitmap(wrench_xpm));
    wxMenuItem* menuItemPayeeRelocation = new wxMenuItem(menuRelocation,
        MENU_PAYEE_RELOCATION,_("&Payees..."),_("Reassign all payees to another payee"));
    menuItemPayeeRelocation->SetBitmap(wxBitmap(wrench_xpm));
    menuRelocation->Append(menuItemCategoryRelocation);
    menuRelocation->Append(menuItemPayeeRelocation);
    menuTools->AppendSubMenu(menuRelocation,_("Relocation of..."),_("Relocate Categories && Payees"));

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
        wxMenuItem* menuItemAssets = new wxMenuItem(menuTools, MENU_ASSETS, _("&Assets"), _("Assets"));
        menuItemAssets->SetBitmap(wxBitmap(car_xpm));
        menuTools->Append(menuItemAssets);
    }

    menuTools->AppendSeparator();

    wxMenuItem* menuItemTransactions = new wxMenuItem(menuTools, MENU_TRANSACTIONREPORT,
        _("&Transaction Report Filter..."), _("Transaction Report Filter"));
    menuItemTransactions->SetBitmap(wxBitmap(filter_xpm));
    menuTools->Append(menuItemTransactions);

    menuTools->AppendSeparator();

    // Create the head menu
    wxMenu *menuCustomSqlReports = new wxMenu;
    // create the menu items
    wxMenuItem* menuItemCustomReportNew = new wxMenuItem(menuCustomSqlReports,
        MENU_CUSTOM_SQL_REPORT_NEW, _("New..."),_("Create a new SQL report"));
    wxMenuItem* menuItemCustomReportEdit = new wxMenuItem(menuCustomSqlReports,
        MENU_CUSTOM_SQL_REPORT_EDIT, _("Edit..."),_("Edit an existing SQL report"));
    wxMenuItem* menuItemCustomReportDelete = new wxMenuItem(menuCustomSqlReports,
        MENU_CUSTOM_SQL_REPORT_DELETE, _("Delete..."),_("Remove a file from the Custom SQL Reports menu"));
    // Add menu items to the Custom SQL Reports menu
    menuCustomSqlReports->Append(menuItemCustomReportNew);
    menuCustomSqlReports->Append(menuItemCustomReportEdit);
    menuCustomSqlReports->Append(menuItemCustomReportDelete);
    // Add menu to Tools menu
    menuTools->AppendSubMenu(menuCustomSqlReports,_("Custom SQL Reports..."),_("Create or modify SQL reports for the Reports section"));

    menuTools->AppendSeparator();

    wxMenuItem* menuItemOptions = new wxMenuItem(menuTools, wxID_PREFERENCES,
          _("&Options..."), _("Show the Options Dialog"));
    menuItemOptions->SetBitmap(wxBitmap(wrench_xpm));
    menuTools->Append(menuItemOptions);

    menuTools->AppendSeparator();

    wxMenuItem* menuItemConvertDB = new wxMenuItem(menuTools, MENU_CONVERT_ENC_DB,
        _("Convert Encrypted &DB"),
        _("Convert Encrypted DB to Non-Encrypted DB"));
    menuTools->Append(menuItemConvertDB);

    menuTools->AppendSeparator();

    menuItemOnlineUpdateCurRate_ = new wxMenuItem(menuTools, MENU_ONLINE_UPD_CURRENCY_RATE,
        _("Online &Update Currency Rate"),
        _("Online update currency rate"));
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
       wxMenuItem* menuItemReportIssues = new wxMenuItem(menuTools, MENU_REPORTISSUES,
          _("Visit MMEX Forum or &Report New Issues."), _("Visit the MMEX forum. See existing user comments, or report new issues with the software."));
       menuItemReportIssues->SetBitmap(wxBitmap(issues_xpm));
       menuHelp->Append(menuItemReportIssues);
    }

    if (mmIniOptions::instance().enableBeNotifiedForNewReleases_)
    {
       wxMenuItem* menuItemNotify = new wxMenuItem(menuTools, MENU_ANNOUNCEMENTMAILING,
          _("Register/View Release &Notifications."), _("Sign up to Notification Mailing List or View existing announcements."));
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
    wxBitmap toolBarBitmaps[10];
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
    toolBar_->AddTool(MENU_CUSTOM_SQL_REPORT_NEW, _("Custom SQL Manager"), toolBarBitmaps[9], _("Create new Custom SQL Reports"));

    // after adding the buttons to the toolbar, must call Realize() to reflect changes
    toolBar_->Realize();
}
//----------------------------------------------------------------------------

bool mmGUIFrame::createDataStore(const wxString& fileName, const wxString& pwd, bool openingNew)
{
    wxConfigBase *config = wxConfigBase::Get();
    if (m_core) m_core.reset();

    if (m_db)
    {
        m_db->Close();
        m_db.reset();

        /// Update the database according to user requirements
        if (mmOptions::instance().databaseUpdated_ &&
           (config->ReadBool("BACKUPDB_UPDATE", false)))
        {
            BackupDatabase(fileName_, true);
            mmOptions::instance().databaseUpdated_ = false;
        }
    }

    wxFileName checkExt(fileName);
    wxString password;
    bool passwordCheckPassed = true;
    if (checkExt.GetExt().Lower() == ("emb") && wxFileName::FileExists(fileName))
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
        if (config->ReadBool ("BACKUPDB", false))
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
                        dialogErrorMessageHeading, wxICON_EXCLAMATION);

            m_db->Close();
            m_db.reset();
            return false;
        }

        password_ = password;
        m_core.reset(new mmCoreDB(m_db));
    }
    else if (openingNew) // New Database
    {
       if (mmIniOptions::instance().enableCustomTemplateDB_
           && wxFileName::FileExists(mmIniOptions::instance().customTemplateDB_))
       {
           wxCopyFile(mmIniOptions::instance().customTemplateDB_, fileName, true);
           m_db = mmDBWrapper::Open(fileName);
           password_ = password;
           m_core.reset(new mmCoreDB(m_db));
       }
       else
       {
           m_db = mmDBWrapper::Open(fileName, password);
           password_ = password;

           openDataBase(fileName);
           m_core.reset(new mmCoreDB(m_db));

           mmNewDatabaseWizard* wizard = new mmNewDatabaseWizard(this, m_core.get());
           wizard->CenterOnParent();
           wizard->RunIt(true);

           mmDBWrapper::loadBaseCurrencySettings(m_db.get());

           /* Load User Name and Other Settings */
           mmIniOptions::instance().loadInfoOptions(m_db.get());

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
    //wxProgressDialog dlg(_("Please Wait"),
    //    _("Opening Database File && Verifying Integrity"), 100, this,
    //    wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH );

    //mmDBWrapper::initDB(m_db.get(), &dlg);
    //dlg.Update(100);
    //dlg.Destroy();

    mmDBWrapper::initDB(m_db.get(), 0);

    wxString title = mmex::getProgramName() + (" : ") + fileName;

    if (mmex::isPortableMode())
        title << (" [") << _("portable mode") << (']');

    SetTitle(title);

    m_topCategories.Clear();
    mmIniOptions::instance().loadInfoOptions(m_db.get());

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

wxPanel* mmGUIFrame::createMainFrame(wxPanel* /*parent*/)
{
    return 0;
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
                     ("MMB Files(*.mmb)|*.mmb"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                    );

    if(dlg.ShowModal() != wxID_OK)
        return;

    wxString fileName = dlg.GetPath();

    if (!fileName.EndsWith((".mmb")))
        fileName += (".mmb");

    SetDatabaseFile(fileName, true);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOpen(wxCommandEvent& /*event*/)
{
    autoRepeatTransactionsTimer_.Stop();
    wxString fileName = wxFileSelector(_("Choose database file to open"),
                                       wxEmptyString, wxEmptyString, wxEmptyString,
                                       ("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb"),
                                       wxFD_FILE_MUST_EXIST,
                                       this
                                      );

    if (!fileName.empty()) {
        SetDatabaseFile(fileName);
        mmGUIFrame::saveConfigFile();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnConvertEncryptedDB(wxCommandEvent& /*event*/)
{
    wxString encFileName = wxFileSelector(_("Choose Encrypted database file to open"),
                                          wxEmptyString, wxEmptyString, wxEmptyString,
                                          ("Encrypted MMB files (*.emb)|*.emb"),
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
                     ("MMB Files(*.mmb)|*.mmb"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                    );

    if(dlg.ShowModal() != wxID_OK)
        return;

    wxString fileName = dlg.GetPath();

    if (!dlg.GetPath().EndsWith((".mmb")))
        fileName += (".mmb");

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
                     ("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT
                    );

    if (dlg.ShowModal() != wxID_OK) return;

    // Ensure database is in a steady state first
    if (!activeHomePage_) createHomePage();   // Display Home page when not being displayed.

    bool encrypt = dlg.GetFilterIndex() != 0; // emb -> Encrypted mMB
    wxFileName newFileName(dlg.GetPath());
    fixFileExt(newFileName, encrypt ? ("emb") : ("mmb"));

    wxFileName oldFileName(fileName_); // opened db's file

    if (newFileName == oldFileName) // on case-sensitive FS uses case-sensitive comparison
    {
        wxMessageDialog dlg(this, _("Can't copy file to itself"), _("Save database file as"), wxICON_WARNING);
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
    mmExportQIF(m_core.get(), m_db.get());
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnImportQIF(wxCommandEvent& /*event*/)
{
    int accountID = mmImportQIF(m_core.get());
    if (accountID != -1)
    {
        setAccountNavTreeSection(m_core.get()->getAccountName(accountID));
        createCheckingAccountPage(accountID);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnImportUniversalCSV(wxCommandEvent& /*event*/)
{
    if (m_core.get()->getNumAccounts() == 0)
    {
        wxMessageBox(_("No account available to import"),_("Universal CSV Import"), wxICON_WARNING );
        return;
    }

    mmUnivCSVDialog univCSVDialog(m_core.get(), this);
    univCSVDialog.ShowModal();
    if (univCSVDialog.InportCompletedSuccessfully())
    {
        setAccountNavTreeSection(m_core.get()->getAccountName(univCSVDialog.ImportedAccountID()));
        createCheckingAccountPage(univCSVDialog.ImportedAccountID());
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    autoRepeatTransactionsTimer_.Stop();
    delete wxConfigBase::Set((wxConfigBase *) NULL);
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
        mmNewAcctDialog dlg(m_core.get(), false, wizard->acctID_, this);
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
                msgStr << _("Term Account views have been temporarly turned on.") << ("\n")
                       << _("To maintain this view, change the defaults by using:\n\nTools -> Options\nView Options")
                       << ("\n\n")
                       << _("This message will not be displayed in future.");
                wxMessageBox(msgStr, _("Initial Term Account Activation"),wxICON_INFORMATION);
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

void mmGUIFrame::OnBudgetSetupDialog(wxCommandEvent& /*event*/)
{
    if (m_db)
    {
        mmBudgetYearDialog(m_db.get(), this).ShowModal();
        createHomePage();
        updateNavTreeControl();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnTransactionReport(wxCommandEvent& /*event*/)
{
    if (!m_db) return;

    if (m_core.get()->getNumAccounts() == 0) return;

    std::vector< boost::shared_ptr<mmBankTransaction> >* trans
        = new std::vector< boost::shared_ptr<mmBankTransaction> >;

    mmFilterTransactionsDialog* dlg= new mmFilterTransactionsDialog(trans, m_core.get(), this);
    if (dlg->ShowModal() == wxID_OK)
    {
        mmReportTransactions* rs = new mmReportTransactions(trans, m_core.get(), dlg->refAccountID_, dlg->refAccountStr_, dlg);
        menuPrintingEnable(true);
        createReportsPage(rs);
    }
    else
    {
        delete trans;
    }

//  The transaction filter dialog will be destroyed by the report object when completed.
//  dlg->Destroy();
}
//----------------------------------------------------------------------------

wxArrayString mmGUIFrame::getAccountsArray( bool withTermAccounts) const
{
    wxArrayString accountArray;

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->rangeAccount();
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* account = it->get();

        if (account->acctType_ == ACCOUNT_TYPE_BANK || (withTermAccounts && account->acctType_ == ACCOUNT_TYPE_TERM))
            accountArray.Add(account->name_);
    }

    return accountArray;
}

void mmGUIFrame::OnCashFlowSpecificAccounts()
{
    if (!m_db.get()) return;
    if (m_core.get()->getNumAccounts() == 0) return;

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
        mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), this, selections);

        report->activateBankAccounts();
        if (this->hasActiveTermAccounts()) report->activateTermAccounts();

        menuPrintingEnable(true);
        createReportsPage(report);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOptions(wxCommandEvent& /*event*/)
{
    if (!m_db.get()) return;

    mmOptionsDialog systemOptions(m_core.get(), this);
    if (systemOptions.ShowModal() == wxID_OK && systemOptions.AppliedChanges())
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

        int style = wxICON_INFORMATION|wxOK;
        wxString sysMsg = wxString() << _("MMEX Options have been updated.") << ("\n\n");
        if (systemOptions.RequiresRestart())
        {
            style = wxICON_WARNING|wxOK;
            sysMsg << _("Recommendation: Shut down and restart MMEX.") << ("\n")
                   << _("This will allow all MMEX option updates to take effect.");
        }
        wxMessageBox(sysMsg, _("New MMEX Options"), style);

        createHomePage();
        updateNavTreeControl();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnHelp(wxCommandEvent& /*event*/)
{
    helpFileIndex_ = mmex::HTML_INDEX;
    menuPrintingEnable(true);
    createHelpPage();
}
//----------------------------------------------------------------------------

bool mmGUIFrame::IsUpdateAvailable(const wxString& page)
{
    wxStringTokenizer tkz(page, ('.'), wxTOKEN_RET_EMPTY_ALL);
    int numTokens = (int)tkz.CountTokens();
    if (numTokens != 4)
    {
        return true;
    }

    int maj = wxAtoi(tkz.GetNextToken());
    int min = wxAtoi(tkz.GetNextToken());
    int cust = wxAtoi(tkz.GetNextToken());
    int build = wxAtoi(tkz.GetNextToken());

    // get current version
    wxString currentV = mmex::getProgramVersion();
    wxStringTokenizer tkz1(currentV, ('.'), wxTOKEN_RET_EMPTY_ALL);
    numTokens = (int)tkz1.CountTokens();

    int majC = wxAtoi(tkz1.GetNextToken());
    int minC = wxAtoi(tkz1.GetNextToken());
    int custC = wxAtoi(tkz1.GetNextToken());
    int buildC = wxAtoi(tkz1.GetNextToken());

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

void mmGUIFrame::OnCheckUpdate(wxCommandEvent& /*event*/)
{
    wxString update_check_heading = _("MMEX System Information Check");
    // Set up system information
    wxString versionDetails = wxString()
        << ("MMEX: ")     << mmex::getProgramVersion()       << ("\n")
        << _("System: ")     << wxPlatformInfo::Get().GetOperatingSystemIdName() << ("\n\n")
        << wxVERSION_STRING  << ("\n")
        << ("Boost C++ ") << (BOOST_VERSION/100000)          << ('.')
                             << (BOOST_VERSION / 100 % 1000)    << ('.')
                             << (BOOST_VERSION % 100)           << ("\n")
        << ("SQLite3: ")  << wxSQLite3Database::GetVersion() << ("\n")
        << ("wxSQLite3: 3.0.0 by Ulrich Telle")              << ("\n\n");

    // Access current version details page
    wxString site = ("http://www.codelathe.com/mmex/version.html");

    wxString page;
    int err_code = site_content(site, page);
    if (err_code != wxID_OK) {
        if (err_code == 2)
            wxMessageBox(_("Cannot get data from WWW!"), update_check_heading, wxICON_WARNING);
        else if (err_code == 1)
            wxMessageBox(_("Unable to connect!"), update_check_heading, wxICON_WARNING);
        return;
    }

//  Included for future testing
//  page = ("x.x.x.x - Win: w.w.w.w - Unix: u.u.u.u - Mac: m.m.m.m");
//  page = ("9.9.9.9 - Win: 0.9.9.0 - Unix: 0.9.9.0 - Mac: 0.9.9.0");
//  page = ("9.9.9.9 - Win: 0.9.9.0 - Unix: 0.9.9.0 - Mac: 0.9.9.0 -[ Win: 0.9.9.0 - Unix: 0.9.9.0 - Mac: 0.9.9.0");
    /*************************************************************************
     Note: To allow larger digit counters and maintain backward compatability,
           the leading counters before the character [ is ignored by the
           version checking routines.

     Expected format of the string from the internet. Version: 0.9.8.0
     page = ("x.x.x.x - Win: w.w.w.w - Unix: u.u.u.u - Mac: m.m.m.m");
     string length = 53 characters
    **************************************************************************/
    wxStringTokenizer versionTokens(page,("["));
    page = versionTokens.GetNextToken(); // ignore old counters
    page = versionTokens.GetNextToken();

    wxStringTokenizer sysTokens(page,("-"));
    wxString winSys  = sysTokens.GetNextToken().Trim(false);
    wxString unixSys = sysTokens.GetNextToken().Trim(false);
    wxString macSys  = sysTokens.GetNextToken().Trim(false);

    wxString mySys =  wxPlatformInfo::Get().GetOperatingSystemFamilyName();

    wxStringTokenizer mySysToken;
    if (mySys == ("Windows"))
        mySysToken.SetString(winSys,(":"));
    else if (mySys == ("Unix"))
        mySysToken.SetString(unixSys,(":"));
    else if (mySys == ("Macintosh"))
        mySysToken.SetString(macSys,(":"));

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

    wxString urlString = ("http://www.codelathe.com/mmex");
    versionDetails << ("\n\n") << _("Proceed to website: ") << urlString << ("     ");
    if (wxMessageBox(versionDetails, update_check_heading, style) == wxOK)
        wxLaunchDefaultBrowser(urlString);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOnlineUpdateCurRate(wxCommandEvent& /*event*/)
{
    DB_View_CURRENCYFORMATS_V1::Data_Set all_currency = CURRENCYFORMATS_V1.all(m_core->db_.get());
    if (all_currency.empty())
        return;

    wxString site;
    wxSortedArrayString symbols_array;
    int currencyID = mmDBWrapper::getBaseCurrencySettings(m_core->db_.get());
    wxString base_symbol;
    DB_View_CURRENCYFORMATS_V1::Data* base_currency = CURRENCYFORMATS_V1.get(currencyID, m_core->db_.get());
    if (base_currency) base_symbol = base_currency->CURRENCY_SYMBOL;

    if(base_symbol.IsEmpty())
    {
        wxMessageBox(_("Could not find base currency symbol!"), _("Update Currency Rate"), wxICON_WARNING);
        return;
    }

    BOOST_FOREACH(const DB_View_CURRENCYFORMATS_V1::Data &currency, all_currency)
    {
        wxString currency_symbol = currency.CURRENCY_SYMBOL.Upper();
        if (!currency_symbol.IsEmpty()) {
            if (wxNOT_FOUND == symbols_array.Index(currency_symbol) &&
                    base_symbol != currency_symbol) {
                symbols_array.Add(currency_symbol);
                site << currency.CURRENCY_SYMBOL.Upper() << base_symbol << ("=X+");
            }
        }
    }
    site.RemoveLast(1);

    wxConfigBase *config = wxConfigBase::Get();

    //Sample:
    //http://finance.yahoo.com/d/quotes.csv?s=EURUSD=X+RUBUSD=X&f=sl1n&e=.csv
    site = wxString() << ("http://") 
            << config->Read("HTTP_YAHOO_SERVER","download.finance.yahoo.com")
            << ("/d/quotes.csv?s=") << site;
    site << ("&f=sl1n&e=.csv");

    wxString rates;
    int err_code = site_content(site, rates);
    if (err_code != wxID_OK) {
        if (err_code == 2)
            wxMessageBox(_("Cannot get data from WWW!"), _("Error"), wxICON_WARNING);
        else if (err_code == 1)
            wxMessageBox(_("Unable to connect!"), _("Error"), wxICON_WARNING);
        return;
    }

    wxString CurrencySymbol, dName;
    double dRate = 0;

    std::map<wxString, std::pair<double, wxString> > currency_data;

    // Break it up into lines
    wxStringTokenizer tkz(rates, ("\r\n"));

    while (tkz.HasMoreTokens())
    {
        wxString csvline = tkz.GetNextToken();

        /*** Grab the relevant bits (for now only the symbol and the current price) */
        wxStringTokenizer csvsimple(csvline,("\","),wxTOKEN_STRTOK);
        if (csvsimple.HasMoreTokens())
        {
            CurrencySymbol = csvsimple.GetNextToken();
            if (csvsimple.HasMoreTokens())
            {
                csvsimple.GetNextToken().ToDouble(&dRate);
                if (csvsimple.HasMoreTokens())
                    dName = csvsimple.GetNextToken();
            }
        }
        currency_data.insert(std::make_pair(CurrencySymbol, std::make_pair(dRate, dName)));
    }

    CURRENCYFORMATS_V1.begin(m_core->db_.get());

    wxString msg = ("");
    BOOST_FOREACH(DB_View_CURRENCYFORMATS_V1::Data &currency, all_currency)
    {
        wxString currency_symbol = currency.CURRENCY_SYMBOL.Upper();
        if (!currency_symbol.IsEmpty() && base_symbol != currency_symbol) {
            wxString currency_symbols_pair = currency_symbol + base_symbol + ("=X");

            std::pair<double, wxString> data = currency_data[currency_symbols_pair];

            double rate = data.first;

            msg << currency_symbol << (" : ") << currency.BASECONVRATE << ("\t -> \t") << rate << ("\n");

            if (rate > 0.000001)
                currency.BASECONVRATE = rate;

            // store into db
            if (currency.save(m_core->db_.get())) {
                for (int idx = 0; idx < (int)m_core->currencyList_.currencies_.size(); idx++)
                {
                    wxString currencySymbol  = m_core->currencyList_.currencies_[idx]->currencySymbol_;
                    if (currencySymbol == currency_symbol)
                        m_core->currencyList_.currencies_[idx]->baseConv_ = rate;
                }
            }
        }
    }

    CURRENCYFORMATS_V1.commit(m_core->db_.get());

    wxMessageDialog msgDlg(this, msg, _("Currency rate updated"));
    msgDlg.ShowModal();

    createHomePage();
    updateNavTreeControl();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnReportIssues(wxCommandEvent& /*event*/)
{
   wxString url = ("http://www.codelathe.com/forum");
   wxLaunchDefaultBrowser(url);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnBeNotified(wxCommandEvent& /*event*/)
{
    // New site location
    //  wxString url = ("http://groups.google.com/group/mmlist");
    wxString url = ("https://groups.google.com/forum/?fromgroups#!forum/mmlist");

    wxLaunchDefaultBrowser(url);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnFacebook(wxCommandEvent& /*event*/)
{
    wxString url = mmex::getProgramFacebookSite();
    wxLaunchDefaultBrowser(url);
}

//----------------------------------------------------------------------------

void mmGUIFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    mmAboutDialog(this).ShowModal();
/*    wxIcon logo(money_xpm);
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetIcon(logo);
    aboutInfo.SetName(_("Version: "));
    aboutInfo.SetVersion(mmex::getProgramVersion());
    aboutInfo.SetDescription(mmex::getProgramDescription());
    aboutInfo.SetCopyright(mmex::getProgramCopyright());
    aboutInfo.SetWebSite(mmex::getProgramWebSite());

    wxString filePath = mmex::getPathDoc(mmex::F_CONTRIB);
    wxFileInputStream input(filePath);
    wxTextInputStream text(input);
    wxArrayString data;
    data.Add((""));

    int part = 0;
    while (!input.Eof())
    {
        wxString line = text.ReadLine() << ("\n");
        if (!line.Contains(("-------------\n")))
            data[part] << line;
        else {
            ++part;
            data.Add((""));
        }
    }

    aboutInfo.AddDeveloper(data[0]);
    if (data.GetCount() > 1) aboutInfo.AddTranslator(data[1]);
    if (data.GetCount() > 2) aboutInfo.AddDocWriter(data[2]);
    if (data.GetCount() > 3) aboutInfo.AddArtist(data[3]);

    wxAboutBox(aboutInfo);
*/
}
//----------------------------------------------------------------------------

void mmGUIFrame::restorePrinterValues()
{
    wxConfigBase *config = wxConfigBase::Get();

    wxString paperID_String     = wxString() << wxPAPER_A4;
    wxString pageOrientationStr = wxString() << wxPORTRAIT;
    long leftMargin = config->ReadLong(("PRINTER_LEFT_MARGIN"), 20);
    long rightMargin = config->ReadLong(("PRINTER_RIGHT_MARGIN"), 20);
    long topMargin = config->ReadLong(("PRINTER_TOP_MARGIN"), 20);
    long bottomMargin = config->ReadLong(("PRINTER_BOTTOM_MARGIN"), 20);
    long paperID = config->ReadLong(("PRINTER_PAGE_ID"), wxPAPER_A4);
    long pageOrientation = config->ReadLong(("PRINTER_PAGE_ORIENTATION"), wxPORTRAIT);
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
        wxConfigBase *config = wxConfigBase::Get();

        printer_->PageSetup();

        wxPageSetupDialogData* printerDialogData = printer_->GetPageSetupData();
        wxPoint topLeft = printerDialogData->GetMarginTopLeft();
        wxPoint bottomRight = printerDialogData->GetMarginBottomRight();

        wxPrintData* printerData = printer_->GetPrintData();
        long pageOrientation = printerData->GetOrientation();
        wxPaperSize paperID = printerData->GetPaperId();
        wxString pageID = wxString() << paperID;

        config->Write("PRINTER_LEFT_MARGIN", (long)topLeft.x);
        config->Write("PRINTER_RIGHT_MARGIN", (long)bottomRight.x);
        config->Write("PRINTER_TOP_MARGIN", (long)topLeft.y);
        config->Write("PRINTER_BOTTOM_MARGIN", (long)bottomRight.y);
        config->Write("PRINTER_PAGE_ORIENTATION", pageOrientation);
        config->Write("PRINTER_PAGE_ID", pageID);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPrintPageReport(wxCommandEvent& WXUNUSED(event))
{
    if (!printer_) return;

    mmReportsPanel* rp = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    mmHelpPanel* hp = dynamic_cast<mmHelpPanel*>(panelCurrent_);
    if (rp)
        printer_ ->PrintText(rp->getReportText());
    else if (hp)
        printer_ ->PrintFile(mmex::getPathDoc((mmex::EDocFile)helpFileIndex_));
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPrintPagePreview(wxCommandEvent& WXUNUSED(event))
{
    if (!printer_) return;

    mmReportsPanel* rp = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    mmHelpPanel* hp = dynamic_cast<mmHelpPanel*>(panelCurrent_);
    if (rp)
        printer_ ->PreviewText(rp->getReportText());
    else if (hp)
        printer_ ->PreviewFile(mmex::getPathDoc((mmex::EDocFile)helpFileIndex_));
}
//----------------------------------------------------------------------------

void mmGUIFrame::showBeginAppDialog(bool fromScratch)
{
    mmAppStartDialog dlg(this);
    if (fromScratch) dlg.SetCloseButtonToExit();
    if (dlg.ShowModal() == wxID_OK)
    {
        if (fromScratch) this->Close();
    }

    int rc = dlg.getReturnCode();

    if (rc == appStartDialog(APP_START_NEW_DB))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_NEW);
        AddPendingEvent(evt);
    }
    else if (rc == appStartDialog(APP_START_OPEN))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_OPEN);
        AddPendingEvent(evt);
    }
    else if (rc == appStartDialog(APP_START_HELP))
    {
            /* Do Nothing in this case */
    }
    else if (rc == appStartDialog(APP_START_WEB))
    {
        //wxString url = ("http://www.codelathe.com/mmex/index.php");
        //wxLaunchDefaultBrowser(url);
    }
    else if (rc == appStartDialog(APP_START_LAST_DB))
    {
        wxFileName fname(mmDBWrapper::getLastDbPath());
        if (fname.IsOk()) SetDatabaseFile(fname.GetFullPath());
    }
    else if (rc == -1)
    {
        /* Do Nothing in this case */
    }
    else if (rc == appStartDialog(APP_CLOSE))
    {
        Close();
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
        wxString fileName = wxFileSelector(("Choose HTML file to Export"),
            wxEmptyString, wxEmptyString, wxEmptyString, ("*.html"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if ( !fileName.empty() )
        {
            correctEmptyFileExt(("html"),fileName);
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

    panelCurrent_ = new mmBillsDepositsPanel(m_db.get(), m_core.get(), homePanel, ID_PANEL3,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

//----------------------------------------------------------------------------
void mmGUIFrame::createStocksAccountPage(int accountID)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmStocksPanel(m_db.get(), m_core.get(),
                                        accountID, homePanel, ID_PANEL3,
                                        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL
                                       );

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
    homePanel->Layout();
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnGotoStocksAccount(wxCommandEvent& WXUNUSED(event))
{
    if (gotoAccountID_ != -1)
        createStocksAccountPage(gotoAccountID_);
}

//----------------------------------------------------------------------------
void mmGUIFrame::createCheckingAccountPage(int accountID)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmCheckingPanel(m_core.get(),
                                        accountID, homePanel, ID_PANEL3,
                                        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL
                                       );

    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
    homePanel->Layout();
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnGotoAccount(wxCommandEvent& WXUNUSED(event))
{
    if (gotoAccountID_ != -1)
        createCheckingAccountPage(gotoAccountID_);
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnAssets(wxCommandEvent& /*event*/)
{
    wxSizer *sizer = cleanupHomePanel();
    panelCurrent_ = new mmAssetsPanel(homePanel, m_db.get(), m_core.get());
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
    wxStatusBar *sb = GetStatusBar();
    if (sb) sb->SetStatusText(panelCurrent_->get_version());

    homePanel->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnCurrency(wxCommandEvent& /*event*/)
{
    mmMainCurrencyDialog(m_core.get(),this, false).ShowModal();
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
        wxMessageBox(_("No account available to edit!"), _("Accounts"), wxICON_WARNING);
        return;
    }

    wxArrayString as;
    int num = (int)m_core->accountList_.accounts_.size();
    boost::scoped_array<int> arrAcctID(new int[num]);

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->rangeAccount();
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
        mmNewAcctDialog dlg(m_core.get(), false, acctID, this);
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
        wxMessageBox(_("No account available to delete!"), _("Accounts"), wxICON_WARNING);
        return;
    }

    wxArrayString as;
    int num = (int)m_core->accountList_.accounts_.size();
    int* arrAcctID = new int[num];

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->rangeAccount();
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
                                       _(" account: ") + m_core->accountList_.accounts_[choice]->name_ + (" ?");
        wxMessageDialog msgDlg(this, deletingAccountName, _("Confirm Account Deletion"),
            wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
        if (msgDlg.ShowModal() == wxID_YES)
        {
            m_core->deleteAccount(acctID);
            m_core->bTransactionList_.deleteTransactions(acctID);

            updateNavTreeControl();
            if (!refreshRequested_)
            {
                refreshRequested_ = true;
                wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
                GetEventHandler()->AddPendingEvent(ev);
            }
        }
    }
    delete[] arrAcctID;
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewToolbar(wxCommandEvent &event)
{
    m_mgr.GetPane(("toolbar")).Show(event.IsChecked());
    m_mgr.Update();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewLinks(wxCommandEvent &event)
{
    m_mgr.GetPane(("Navigation")).Show(event.IsChecked());
    m_mgr.Update();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewToolbarUpdateUI(wxUpdateUIEvent &event)
{
    if(m_mgr.GetPane(("toolbar")).IsShown())
        event.Check(true);
    else
        event.Check(false);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewLinksUpdateUI(wxUpdateUIEvent &event)
{
    if(m_mgr.GetPane(("Navigation")).IsShown())
        event.Check(true);
    else
        event.Check(false);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewBankAccounts(wxCommandEvent &event)
{
    m_mgr.GetPane(("Bank Accounts")).Show(event.IsChecked());
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
    m_mgr.GetPane(("Term Accounts")).Show(event.IsChecked());
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
    m_mgr.GetPane(("Stock Accounts")).Show(event.IsChecked());
    m_mgr.Update();

    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        createHomePage();
    }
}

void mmGUIFrame::OnIgnoreFutureTransactions(wxCommandEvent &event)
{
    m_mgr.GetPane(("Ignore Future Transactions")).Show(event.IsChecked());
    mmIniOptions::instance().ignoreFutureTransactions_ = !mmIniOptions::instance().ignoreFutureTransactions_;
    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        createHomePage();
        updateNavTreeControl(menuBar_->IsChecked(MENU_VIEW_TERMACCOUNTS));
    }
}

void mmGUIFrame::OnCategoryRelocation(wxCommandEvent& /*event*/)
{
    relocateCategoryDialog* dlg = new relocateCategoryDialog(m_core.get(), m_db.get(), this);
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString msgStr;
        msgStr << _("Category Relocation Completed.") << ("\n\n")
               << dlg->updatedCategoriesCount() << _(" records have been updated in the database.") << ("\n\n")
               << _("MMEX must be shutdown and restarted for all the changes to be seen.");
        wxMessageBox(msgStr,_("Category Relocation Result"));
        mmOptions::instance().databaseUpdated_ = true;
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPayeeRelocation(wxCommandEvent& /*event*/)
{
    relocatePayeeDialog* dlg = new relocatePayeeDialog(m_core.get(), m_db.get(), this);
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString msgStr;
        msgStr << _("Payee Relocation Completed.") << ("\n\n")
               << dlg->updatedPayeesCount() << _(" records have been updated in the database.") << ("\n\n")
               << _("MMEX must be shutdown and restarted for all the changes to be seen.");
        wxMessageBox(msgStr,_("Payee Relocation Result"));
        mmOptions::instance().databaseUpdated_ = true;
    }
}

void mmGUIFrame::RunCustomSqlDialog(bool forEdit)
{
//    wxProgressDialog* progressBar = new wxProgressDialog(_("Report printing in progress")
//        , wxEmptyString, 100, NULL, wxPD_AUTO_HIDE|wxPD_CAN_ABORT);
    //Use Shared pointer to ensure object gets destroyed if SQL Script errors hijack the object.
    boost::shared_ptr<mmCustomSQLDialog> dlg( new mmCustomSQLDialog(custRepIndex_, this, forEdit ));
    int dialogStatus = dlg->ShowModal();
    wxBeginBusyCursor(wxHOURGLASS_CURSOR);
    while (dialogStatus == wxID_MORE)
    {
        if (dlg->sqlQuery() != (""))
        {
            mmCustomSQLReport* csr = new mmCustomSQLReport(m_core.get(), dlg->sqlReportTitle(), dlg->sqlQuery());
            menuPrintingEnable(true);
            createReportsPage(csr);
        }
        dialogStatus = dlg->ShowModal();
    }
    wxEndBusyCursor();

    if (dialogStatus == wxID_OK) updateNavTreeControl();

    dlg->Destroy();
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnNewCustomSqlReport(wxCommandEvent& /*event*/)
{
    custRepIndex_->initIndexFileHeader();   // create the index file if not exist.
    custRepIndex_->resetReportsIndex();     // Reset the file to start
    RunCustomSqlDialog();
    updateNavTreeControl();
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnEditCustomSqlReport(wxCommandEvent& /*event*/)
{
    custRepIndex_->getUserTitleSelection(_(" to Edit:"));
    EditCustomSqlReport();
}

void mmGUIFrame::OnPopupEditCustomSqlReport(wxCommandEvent&)
{
    custRepIndex_->getSelectedTitleSelection(customSqlReportSelectedItem_);
    EditCustomSqlReport();
}

void mmGUIFrame::EditCustomSqlReport()
{
    if (custRepIndex_->validTitle())
    {
        RunCustomSqlDialog(true);
    }
}

//----------------------------------------------------------------------------
void mmGUIFrame::OnDeleteCustomSqlReport(wxCommandEvent& /*event*/)
{
    custRepIndex_->getUserTitleSelection(_(" to Delete:"));
    if (custRepIndex_->validTitle())
    {
        DeleteCustomSqlReport();
    }
}

void mmGUIFrame::OnPopupDeleteCustomSqlReport(wxCommandEvent& /*event*/)
{
    custRepIndex_->getSelectedTitleSelection(customSqlReportSelectedItem_);
    DeleteCustomSqlReport();
}

void mmGUIFrame::DeleteCustomSqlReport()
{
    wxString msg = wxString() << _("Delete the Custom Report Title:")
                              << ("\n\n")
                              << custRepIndex_->currentReportTitle();
    if ( wxMessageBox(msg ,custRepIndex_->UserDialogHeading(),wxYES_NO|wxICON_QUESTION) == wxYES )
    {
        custRepIndex_->deleteSelectedReportTitle();

        if (! custRepIndex_->currentReportFileName(false).IsEmpty())
        {
            msg = wxString() << _("Do you want to delete the SQL file as well?") << ("\n");
            if ( wxMessageBox(msg, custRepIndex_->UserDialogHeading(), wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION) == wxYES)
            {
                if (wxFileExists(custRepIndex_->currentReportFileName()))
                {
                    wxRemoveFile(custRepIndex_->currentReportFileName());
                }
            }
        }
        updateNavTreeControl();
    }
}

//----------------------------------------------------------------------------

void wxNewDatabaseWizardPage1::OnCurrency(wxCommandEvent& /*event*/)
{
    currencyID_ = parent_->m_core->currencyList_.getBaseCurrencySettings();

    if (mmMainCurrencyDialog::Execute(parent_->m_core, this, currencyID_) && currencyID_ != -1)
    {
        wxString currName = parent_->m_core->currencyList_.getCurrencySharedPtr(currencyID_)->currencyName_;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_CURRENCY);
        bn->SetLabel(currName);
        parent_->m_core->currencyList_.setBaseCurrencySettings(currencyID_);
    }
}
//----------------------------------------------------------------------------

wxNewDatabaseWizardPage1::wxNewDatabaseWizardPage1(mmNewDatabaseWizard* parent) :
    wxWizardPageSimple(parent),
    parent_(parent),
    currencyID_(-1)
{
    currencyID_ = parent_->m_core->currencyList_.getBaseCurrencySettings();
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
            << ("\n");
    mainSizer->Add( new wxStaticText(this, wxID_ANY, helpMsg), 0, wxALL, 5);


    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( this, wxID_STATIC, _("User Name"));
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    itemUserName_ = new wxTextCtrl( this, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME, _T(""), wxDefaultPosition, wxSize(130,-1), 0 );
    itemBoxSizer5->Add(itemUserName_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    helpMsg.Empty();
    helpMsg << _("(Optional) Specify a title or your name.") << ("\n")
            << _("Used as a database title for displayed and printed reports.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY, helpMsg), 0, wxALL, 5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool wxNewDatabaseWizardPage1::TransferDataFromWindow()
{
    if ( currencyID_ == -1)
    {
        wxMessageBox(_("Base Currency Not Set"), _("New Database"), wxICON_WARNING | wxOK, this);

        return false;
    }
    userName = itemUserName_->GetValue().Trim();
    mmDBWrapper::setInfoSettingValue(parent_->m_core->db_.get(), ("USERNAME"), userName);

    return true;
}
//----------------------------------------------------------------------------

wxAddAccountPage1::wxAddAccountPage1(mmAddAccountWizard* parent) :
    wxWizardPageSimple(parent), parent_(parent)
{
    textAccountName_ = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(130,-1), 0 );

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Name of the Account")), 0, wxALL, 5 );
    mainSizer->Add( textAccountName_, 0 /* No stretching */, wxALL, 5 /* Border Size */);

    wxString helpMsg;
    helpMsg  << ("\n") << _("Specify a descriptive name for the account.") << ("\n")
            << _("This is generally the name of a financial institution\nwhere the account is held. For example: 'ABC Bank'.");
    mainSizer->Add(new wxStaticText(this, wxID_ANY, helpMsg ), 0, wxALL, 5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool wxAddAccountPage1::TransferDataFromWindow()
{
    if ( textAccountName_->GetValue().empty())
    {
        wxMessageBox(_("Account Name Invalid"), _("New Account"), wxICON_WARNING | wxOK, this);
        return false;
    }
    parent_->accountName_ = textAccountName_->GetValue().Trim();
    return true;
}
//----------------------------------------------------------------------------

wxAddAccountPage2::wxAddAccountPage2(mmAddAccountWizard *parent) :
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
    textMsg << ("\n")
            << _("Select the type of account you want to create:") << ("\n\n")
            << _("General bank accounts cover a wide variety of account\ntypes like Checking, Savings and Credit card type accounts.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY,textMsg), 0, wxALL, 5);

    textMsg = ("\n");
    textMsg << _("Investment accounts are specialized accounts that only\nhave stock/mutual fund investments associated with them.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY,textMsg), 0, wxALL, 5);

    textMsg = ("\n");
    textMsg << _("Term accounts are specialized bank accounts. Intended for asset\ntype accounts such as Term Deposits and Bonds. These accounts\ncan have regular money coming in and out, being outside the\ngeneral income stream.");
    mainSizer->Add( new wxStaticText(this, wxID_ANY,textMsg), 0, wxALL, 5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool wxAddAccountPage2::TransferDataFromWindow()
{
    int acctType = itemChoiceType_->GetSelection();
    wxString acctTypeStr = ACCOUNT_TYPE_BANK;
    if (acctType == 1)
        acctTypeStr = ACCOUNT_TYPE_STOCK;
    else if (acctType == 2)
        acctTypeStr = ACCOUNT_TYPE_TERM;

    int currencyID = parent_->m_core->currencyList_.getBaseCurrencySettings();
    if (currencyID == -1)
    {
        wxString errorMsg;
        errorMsg << _("Base Account Currency Not set.") << ("\n")
                 << _("Set that first using Tools->Options menu and then add a new account.");
        wxMessageBox( errorMsg, _("New Account"), wxICON_WARNING | wxOK, this);
        return false;
    }

    mmAccount* ptrBase = new mmAccount();

    boost::shared_ptr<mmAccount> pAccount(ptrBase);

    pAccount->favoriteAcct_ = true;
    pAccount->status_ = mmAccount::MMEX_Open;
    pAccount->acctType_ = acctTypeStr;
    pAccount->name_ = parent_->accountName_;
    pAccount->initialBalance_ = 0;
    pAccount->currency_ = parent_->m_core->currencyList_.getCurrencySharedPtr(currencyID);
    // prevent same account being added multiple times in case of using 'Back' and 'Next' in wizard.
    if ( ! parent_->m_core->accountExists(pAccount->name_))
        parent_->acctID_ = parent_->m_core->addAccount(pAccount);

    return true;
}
//----------------------------------------------------------------------------

wxSizer* mmGUIFrame::cleanupHomePanel(bool new_sizer)
{
    wxASSERT(homePanel);

    homePanel->DestroyChildren();
    homePanel->SetSizer(new_sizer ? new wxBoxSizer(wxHORIZONTAL) : 0);

    return homePanel->GetSizer();
}
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
        wxLogError(wxString::Format(("%s"), e.what()));
    }
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
        wxLogError(wxString::Format(("%s"), e.what()));
    }

    return ok;
}
//----------------------------------------------------------------------------

void mmGUIApp::OnFatalException()
{
    reportFatalException(wxDebugReport::Context_Exception);
}
//----------------------------------------------------------------------------

void mmGUIFrame::SetDatabaseFile(const wxString& dbFileName, bool newDatabase)
{
    autoRepeatTransactionsTimer_.Stop();
    wxConfigBase *config = wxConfigBase::Get();
    wxProgressDialog *progress = NULL;
    if (! newDatabase)
    {
        progress = new wxProgressDialog( _("Setting new Database file"),
            _("Please wait while the new database is being loaded."), 100, this);
        progress->Update(33);
    }

    // Ensure database is in a steady state first
    if (!activeHomePage_)
    {
        refreshRequested_ = true;
        createHomePage();
    }

    if (progress) progress->Update(65);
    if (openFile(dbFileName, newDatabase))
    {
        if (progress) progress->Update(80);
        history_->AddFileToHistory(dbFileName);
        config->Write("LASTFILENAME", dbFileName);
        history_->Save(*config);
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

void mmGUIFrame::BackupDatabase(const wxString& filename, bool updateRequired)
{
    wxFileName fn(filename);
    if (!fn.IsOk()) return;

    wxString backupType = ("_start_");
    if (updateRequired) backupType = ("_update_");

    wxString backupName = filename + backupType + wxDateTime().Today().FormatISODate() + (".") + fn.GetExt();
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
    wxString fileSearch = filename + backupType + ("*.") + fn.GetExt();
    wxString backupFile = wxFindFirstFile(fileSearch);
    while (!backupFile.empty())
    {
        backupFileArray.Add(backupFile);
        backupFile = wxFindNextFile();
    }

    // limit the amount of backup files to 4
    if (backupFileArray.Count() > 4)
    {
        backupFileArray.Sort(true);
        // ensure file is not read only before deleting file.
        wxFileName fnLastFile(backupFileArray.Last());
        if (fnLastFile.IsFileWritable()) wxRemoveFile(backupFileArray.Last());
    }
}

void mmGUIFrame::OnMRUFile(wxCommandEvent& event)
{
    wxString file(history_->GetHistoryFile(event.GetId() - wxID_FILE1));
    SetDatabaseFile(file);
}

void mmGUIFrame::OnClearRecentFiles(wxCommandEvent& /*event*/)
{
    wxConfigBase *config = wxConfigBase::Get();
    size_t files_number = history_->GetCount();
    for (size_t i=files_number-1; i>0; i--) {
        history_->RemoveFileFromHistory(i);
    }; 
    history_->Save(*config);
}

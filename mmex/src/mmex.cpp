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
#include "mmcheckingpanel.h"
#include "budgetingpanel.h"
#include "billsdepositspanel.h"
#include "mmhomepagepanel.h"
#include "mmreportspanel.h"
#include "mmhelppanel.h"
#include "stockspanel.h"
#include "assetspanel.h"
#include "univcsvdialog.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "reportsummary.h"
#include "reportcategexp.h"
#include "reportpayee.h"
#include "reportincexpenses.h"
#include "reportbudgetingperf.h"
#include "reportincexpesestime.h"
#include "reportincexpensesfinancialperiod.h"
#include "reportsummarystocks.h"
#include "reportsummaryassets.h"
#include "reporttransactions.h"
#include "reportcashflow.h"
#include "reporttransstats.h"
#include "reportcategovertimeperf.h"
#include "reportbudgetcategorysummary.h"
#include "mmgraphtopcategories.h"
#include "appstartdialog.h"
#include "aboutdialog.h"
#include "newacctdialog.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "relocatecategorydialog.h"
#include "relocatepayeedialog.h"
#include "budgetyeardialog.h"
#include "optionsdialog.h"
#include "currencydialog.h"
#include "maincurrencydialog.h"
#include "filtertransdialog.h"
#include "billsdepositsdialog.h"
#include "dbwrapper.h"
#include "paths.h"
#include "constants.h"
#include "platfdep.h"
#include "customreportdisplay.h"
#include "customreportindex.h"
#include "customreportdialog.h"
#include <boost/version.hpp>
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
#include "../resources/assets.xpm"
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
#include <wx/xml/xml.h>
//----------------------------------------------------------------------------
#include <boost/scoped_array.hpp>
#include <string>
//----------------------------------------------------------------------------

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

bool OnInitImpl(mmGUIApp &app)
{
    app.SetAppName(mmex::GetAppName());

    /* Setting Locale causes unexpected problems, so default to English Locale */
    app.getLocale().Init(wxLANGUAGE_ENGLISH);

    /* Initialize Image Handlers */
    wxImage::AddHandler(new wxICOHandler()); 
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());

    /* Get INI DB for loading settings */
    wxSQLite3Database inidb;

    inidb.Open(mmex::getPathUser(mmex::SETTINGS));
    
    mmDBWrapper::verifyINIDB(&inidb);

    /* Load Colors from Database */
    mmLoadColorsFromDatabase(&inidb);

    /* Load MMEX Custom Settings */
    mmIniOptions::instance().loadOptions(&inidb);

    /* Was App Maximized? */
    wxString isMaxStrDef = wxT("FALSE");
    wxString isMaxStr = mmDBWrapper::getINISettingValue(&inidb, wxT("ISMAXIMIZED"), isMaxStrDef);

    /* Load Dimensions of Window */
    wxString originX = wxT("50");
    wxString originY = wxT("50");
    wxString sizeW = wxT("800");
    wxString sizeH = wxT("600");
    wxString valxStr = mmDBWrapper::getINISettingValue(&inidb, wxT("ORIGINX"), originX); 
    wxString valyStr = mmDBWrapper::getINISettingValue(&inidb, wxT("ORIGINY"), originY);
    wxString valWStr = mmDBWrapper::getINISettingValue(&inidb, wxT("SIZEW"),  sizeW);
    wxString valHStr = mmDBWrapper::getINISettingValue(&inidb, wxT("SIZEH"),  sizeH);

    long valx = 0;
    long valy = 0; 
    long valw = 0;
    long valh = 0;

    valxStr.ToLong(&valx);
    valyStr.ToLong(&valy);
    valWStr.ToLong(&valw);
    valHStr.ToLong(&valh);

    mmSelectLanguage(0, &inidb, false);

    inidb.Close();

#if defined (__WXMAC__) || defined (__WXOSX__)
wxSystemOptions::SetOption(wxMAC_ALWAYS_USE_GENERIC_LISTCTRL,1);
#endif

    mmGUIFrame *frame = new mmGUIFrame(mmex::getProgramName(), wxPoint(valx, valy), wxSize(valw, valh));
    bool ok = frame->Show();
    wxASSERT(ok);

    if (isMaxStr == wxT("TRUE")) {
        frame->Maximize(true);
    }

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
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWALL, mmGUIFrame::OnViewAllAccounts)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE, mmGUIFrame::OnViewFavoriteAccounts)
    EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWOPEN, mmGUIFrame::OnViewOpenAccounts)

    /* Custom Sql Reports */
    EVT_MENU(MENU_CUSTOM_SQL_REPORT_NEW, mmGUIFrame::OnNewCustomSqlReport)
    EVT_MENU(MENU_CUSTOM_SQL_REPORT_EDIT, mmGUIFrame::OnEditCustomSqlReport)
    EVT_MENU(MENU_CUSTOM_SQL_REPORT_DELETE, mmGUIFrame::OnDeleteCustomSqlReport)
    EVT_MENU(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_EDIT, mmGUIFrame::OnPopupEditCustomSqlReport)
    EVT_MENU(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_DELETE, mmGUIFrame::OnPopupDeleteCustomSqlReport)
    
    /*Automatic processing of repeat transactions*/
    EVT_TIMER(AUTO_REPEAT_TRANSACTIONS_TIMER_ID, mmGUIFrame::OnAutoRepeatTransactionsTimer)
    
    /* Recent Files */
    EVT_MENU(MENU_RECENT_FILES_1, mmGUIFrame::OnRecentFiles_1)
    EVT_MENU(MENU_RECENT_FILES_2, mmGUIFrame::OnRecentFiles_2)
    EVT_MENU(MENU_RECENT_FILES_3, mmGUIFrame::OnRecentFiles_3)
    EVT_MENU(MENU_RECENT_FILES_4, mmGUIFrame::OnRecentFiles_4)
    EVT_MENU(MENU_RECENT_FILES_5, mmGUIFrame::OnRecentFiles_5)
    EVT_MENU(MENU_RECENT_FILES_CLEAR, mmGUIFrame::OnClearRecentFiles)

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
    displayMsg << _("The next pages will help you create a new database.") << wxT("\n\n")
               << _("Your database file is stored with an extension of .mmb.")<< wxT("\n\n")
               << _("As this file contains important financial information,\nwe recommended creating daily backups with the Options\nsetting: 'Backup before opening', and store your backups\nin a separate location.")<< wxT("\n\n")
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
    panelCurrent_(),
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

    SetIcon(mmex::getProgramIcon());
    SetMinSize(wxSize(480,275));

    /* Setup Printer */
    printer_.reset(new wxHtmlEasyPrinting(mmex::getProgramName(), this));
    wxString printHeaderBase = mmex::getProgramName();
    printer_-> SetHeader( printHeaderBase + wxT("(@PAGENUM@/@PAGESCNT@)<hr>"), wxPAGE_ALL);

    loadConfigFile(); // load from Settings DB
    restorePrinterValues();

    custRepIndex_ = new customSQLReportIndex();

    /* Create the Controls for the frame */
    createMenu();
    createToolBar();
    createControls();
    recentFiles_ = new RecentDatabaseFiles(m_inidb.get(), menuRecentFiles_);

    // add the toolbars to the manager
    m_mgr.AddPane(toolBar_, wxAuiPaneInfo().
        Name(wxT("toolbar")).Caption(wxT("Toolbar")).ToolbarPane().Top().
        LeftDockable(false).RightDockable(false));

    // change look and feel of wxAuiManager
    m_mgr.GetArtProvider()->SetMetric(16, 0);
    m_mgr.GetArtProvider()->SetMetric(3, 1);

    // Save default perspective
    m_perspective = m_mgr.SavePerspective();
    
    wxString auiPerspective = mmDBWrapper::getINISettingValue(m_inidb.get(), 
        wxT("AUIPERSPECTIVE"), m_perspective);

    m_mgr.LoadPerspective(auiPerspective);

    // "commit" all changes made to wxAuiManager
    m_mgr.Update();

    // enable or disable online update currency rate
    wxString enableCurrencyUpd = mmDBWrapper::getINISettingValue(m_inidb.get(), INIDB_UPDATE_CURRENCY_RATE, wxT("FALSE"));
    if(enableCurrencyUpd == wxT("TRUE")) 
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
    wxString showBeginApp = mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("SHOWBEGINAPP"), wxT("TRUE"));
    bool from_scratch = showBeginApp == wxT("TRUE");

    wxFileName dbpath = from_scratch ? wxGetEmptyString() : mmDBWrapper::getLastDbPath(m_inidb.get());
 
    if (from_scratch || !dbpath.IsOk()) {
        menuEnableItems(false);
        createHomePage();
        updateNavTreeControl();
        showBeginAppDialog();
    } else {
        openFile(dbpath.GetFullPath(), false);
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
    recentFiles_->~RecentDatabaseFiles();
    saveConfigFile();

    m_mgr.UnInit();

    /* Delete the GUI */
    cleanupHomePanel(false);

    if (m_core)  m_core.reset();
    if (m_db) m_db->Close();

    /// Update the database according to user requirements
    if (mmOptions::instance().databaseUpdated_ && 
       (mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("BACKUPDB_UPDATE"), wxT("FALSE")) == wxT("TRUE")))
    {
        BackupDatabase(fileName_, true);
    }

    if (m_inidb) m_inidb->Close();
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
        th.id_           = q1.GetInt(wxT("BDID"));
        th.nextOccurDate_  = mmGetStorageStringAsDate(q1.GetString(wxT("NEXTOCCURRENCEDATE")));
        
        th.nextOccurStr_   = mmGetDateForDisplay(m_db.get(), th.nextOccurDate_);
        th.payeeID_        = q1.GetInt(wxT("PAYEEID"));
        th.transType_      = q1.GetString(wxT("TRANSCODE"));
        th.accountID_      = q1.GetInt(wxT("ACCOUNTID"));
        th.toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));

        th.accountName_     = m_core.get()->getAccountName(th.accountID_);
        th.amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
        th.toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));
        th.notes_       = q1.GetString(wxT("NOTES"));
        th.categID_           = q1.GetInt(wxT("CATEGID"));
        th.categoryStr_       = q1.GetString(wxT("CATEGNAME"));
        th.subcategID_       = q1.GetInt(wxT("SUBCATEGID"));
        th.subcategoryStr_ = q1.GetString(wxT("SUBCATEGNAME"));

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
                pTransaction->status_ = q1.GetString(wxT("STATUS"));
                pTransaction->transNum_ = q1.GetString(wxT("TRANSACTIONNUMBER"));
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
    wxFileName fname(fileName_);
    mmDBWrapper::setLastDbPath(m_inidb.get(), fname.GetFullPath());

    /* Aui Settings */
    m_perspective = m_mgr.SavePerspective();
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("AUIPERSPECTIVE"), m_perspective);

    int valx = 0;
    int valy = 0;
    int valw = 0;
    int valh = 0;
    
    this->GetPosition(&valx, &valy);
    this->GetSize(&valw, &valh);    

    wxString valxs = wxString::Format(wxT("%d"), valx);
    wxString valys = wxString::Format(wxT("%d"), valy);
    wxString valws = wxString::Format(wxT("%d"), valw);
    wxString valhs = wxString::Format(wxT("%d"), valh);

    m_inidb.get()->Begin();
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("ORIGINX"), valxs); 
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("ORIGINY"), valys);
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("SIZEW"), valws);
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("SIZEH"), valhs);
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("ISMAXIMIZED"), this->IsMaximized() ? wxT("TRUE") : wxT("FALSE"));
    m_inidb.get()->Commit();
}
//----------------------------------------------------------------------------

void mmGUIFrame::loadConfigFile()
{
    m_inidb = mmDBWrapper::Open(mmex::getPathUser(mmex::SETTINGS));
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

    navTreeCtrl_->SetBackgroundColour(mmColors::navTreeBkColor);
    
    wxSize imageSize(16, 16);
    wxImageList* imageList_ = new wxImageList( imageSize.GetWidth(), imageSize.GetHeight() );
    imageList_->Add(wxBitmap(house_xpm));
    imageList_->Add(wxBitmap(moneyaccount_xpm));  // used for: savings_account
    imageList_->Add(wxBitmap(clock_xpm));
    imageList_->Add(wxBitmap(calendar_xpm));
    imageList_->Add(wxBitmap(chartpiereport_xpm));
    imageList_->Add(wxBitmap(help_xpm));
    imageList_->Add(wxBitmap(stock_curve_xpm));
    imageList_->Add(wxBitmap(car_xpm));
    imageList_->Add(wxBitmap(customsql_xpm));
    imageList_->Add(wxBitmap(termaccount_xpm));   // used for: term_account
    imageList_->Add(wxBitmap(savings_acc_favorite_xpm));
    imageList_->Add(wxBitmap(savings_acc_closed_xpm));
    imageList_->Add(wxBitmap(term_acc_favorite_xpm));
    imageList_->Add(wxBitmap(term_acc_closed_xpm));

    navTreeCtrl_->AssignImageList(imageList_);

    homePanel = new wxPanel( this, ID_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxTR_SINGLE | wxNO_BORDER);

    m_mgr.AddPane(navTreeCtrl_, wxAuiPaneInfo().
        Name(wxT("Navigation")).Caption(_("Navigation")).
        BestSize(wxSize(200,100)).MinSize(wxSize(100,100)).
        Left());

    m_mgr.AddPane(homePanel, wxAuiPaneInfo().
        Name(wxT("Home")).Caption(wxT("Home")).
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
    navTreeCtrl_->SetBackgroundColour(mmColors::navTreeBkColor);

    wxTreeItemId root = navTreeCtrl_->AddRoot(_("Home Page"), 0, 0);
    navTreeCtrl_->SetItemData(root, new mmTreeItemData(wxT("Home Page")));
    navTreeCtrl_->SetItemBold(root, true);
    navTreeCtrl_->SetFocus();


    wxTreeItemId accounts = navTreeCtrl_->AppendItem(root, _("Bank Accounts"), 1, 1);
    navTreeCtrl_->SetItemData(accounts, new mmTreeItemData(wxT("Bank Accounts")));
    navTreeCtrl_->SetItemBold(accounts, true);

    wxTreeItemId termAccount;
    if ( hasActiveTermAccounts() )
    {
    //  Positioning for new type of accounts: Term Accounts        
        termAccount = navTreeCtrl_->AppendItem(root, _("Term Accounts"), 9, 9);
        navTreeCtrl_->SetItemData(termAccount, new mmTreeItemData(wxT("Term Accounts")));
        navTreeCtrl_->SetItemBold(termAccount, true);
    }

    wxTreeItemId stocks;
    stocks = navTreeCtrl_->AppendItem(root, _("Stocks"), 6, 6);
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
            reportNumberStr.Printf(wxT("Custom_Report_%d"), ++reportNumber);
            navTreeCtrl_->SetItemData(customSqlReportItem, new mmTreeItemData(reportNumberStr));
            reportTitle = custRepIndex_->nextReportTitle();
        }

        if (expandedCustomSqlReportNavTree_)
            navTreeCtrl_->Expand(customSqlReports);
    }

    /* ================================================================================================= */

    wxTreeItemId reportsSummary = navTreeCtrl_->AppendItem(reports, _("Summary of Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(reportsSummary, new mmTreeItemData(wxT("Summary of Accounts")));

    wxTreeItemId reportsStocks = navTreeCtrl_->AppendItem(reportsSummary, _("Stocks"), 4, 4);
    navTreeCtrl_->SetItemData(reportsStocks, new mmTreeItemData(wxT("Summary of Stocks")));

    if (mmIniOptions::instance().enableAssets_)
    {
        wxTreeItemId reportsAssets = navTreeCtrl_->AppendItem(reportsSummary, _("Assets"), 4, 4);
        navTreeCtrl_->SetItemData(reportsAssets, new mmTreeItemData(wxT("Summary of Assets")));
    }

    wxTreeItemId categsOverTime = navTreeCtrl_->AppendItem(reports, _("Where the Money Goes"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTime, new mmTreeItemData(wxT("Where the Money Goes")));
    
    wxTreeItemId categsOverTimeCalMonth = navTreeCtrl_->AppendItem(categsOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCalMonth, new mmTreeItemData(wxT("Where the Money Goes - Month")));

    wxTreeItemId categsOverTimeCurrentMonth = navTreeCtrl_->AppendItem(categsOverTime, _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentMonth, new mmTreeItemData(wxT("Where the Money Goes - Current Month")));

    wxTreeItemId categsOverTimeLast30 = navTreeCtrl_->AppendItem(categsOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLast30, new mmTreeItemData(wxT("Where the Money Goes - 30 Days")));
    
    wxTreeItemId categsOverTimeLastYear = navTreeCtrl_->AppendItem(categsOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLastYear, new mmTreeItemData(wxT("Where the Money Goes - Last Year")));

    wxTreeItemId categsOverTimeCurrentYear = navTreeCtrl_->AppendItem(categsOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentYear, new mmTreeItemData(wxT("Where the Money Goes - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId categsOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(categsOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsOverTimeLastFinancialYear, new mmTreeItemData(wxT("Where the Money Goes - Last Financial Year")));

        wxTreeItemId categsOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(categsOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsOverTimeCurrentFinancialYear, new mmTreeItemData(wxT("Where the Money Goes - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId posCategs = navTreeCtrl_->AppendItem(reports, _("Where the Money Comes From"), 4, 4);
    navTreeCtrl_->SetItemData(posCategs, new mmTreeItemData(wxT("Where the Money Comes From")));

    wxTreeItemId posCategsCalMonth = navTreeCtrl_->AppendItem(posCategs, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsCalMonth, new mmTreeItemData(wxT("Where the Money Comes From - Month")));

    wxTreeItemId posCategsCurrentMonth = navTreeCtrl_->AppendItem(posCategs, _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsCurrentMonth, new mmTreeItemData(wxT("Where the Money Comes From - Current Month")));

    wxTreeItemId posCategsTimeLast30 = navTreeCtrl_->AppendItem(posCategs, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLast30, new mmTreeItemData(wxT("Where the Money Comes From - 30 Days")));
    
    wxTreeItemId posCategsTimeLastYear = navTreeCtrl_->AppendItem(posCategs, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLastYear, new mmTreeItemData(wxT("Where the Money Comes From - Last Year")));

    wxTreeItemId posCategsTimeCurrentYear = navTreeCtrl_->AppendItem(posCategs, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeCurrentYear, new mmTreeItemData(wxT("Where the Money Comes From - Current Year")));
 
    if (financialYearIsDifferent())
    {
        wxTreeItemId posCategsTimeLastFinancialYear = navTreeCtrl_->AppendItem(posCategs, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(posCategsTimeLastFinancialYear, new mmTreeItemData(wxT("Where the Money Comes From - Last Financial Year")));

        wxTreeItemId posCategsTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(posCategs, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(posCategsTimeCurrentFinancialYear, new mmTreeItemData(wxT("Where the Money Comes From - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId categs = navTreeCtrl_->AppendItem(reports, _("Categories"), 4, 4);
    navTreeCtrl_->SetItemData(categs, new mmTreeItemData(wxT("Categories - Over Time")));
    
    wxTreeItemId categsCalMonth = navTreeCtrl_->AppendItem(categs, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsCalMonth, new mmTreeItemData(wxT("Categories - Month")));

    wxTreeItemId categsCurrentMonth = navTreeCtrl_->AppendItem(categs, _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsCurrentMonth, new mmTreeItemData(wxT("Categories - Current Month")));

    wxTreeItemId categsTimeLast30 = navTreeCtrl_->AppendItem(categs, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLast30, new mmTreeItemData(wxT("Categories - 30 Days")));
    
    wxTreeItemId categsTimeLastYear = navTreeCtrl_->AppendItem(categs, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLastYear, new mmTreeItemData(wxT("Categories - Last Year")));

    wxTreeItemId categsTimeCurrentYear = navTreeCtrl_->AppendItem(categs, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeCurrentYear, new mmTreeItemData(wxT("Categories - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId categsTimeLastFinancialYear = navTreeCtrl_->AppendItem(categs, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsTimeLastFinancialYear, new mmTreeItemData(wxT("Categories - Last Financial Year")));

        wxTreeItemId categsTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(categs, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(categsTimeCurrentFinancialYear, new mmTreeItemData(wxT("Categories - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////

    wxTreeItemId payeesOverTime = navTreeCtrl_->AppendItem(reports, _("Payees"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTime, new mmTreeItemData(wxT("Payee Report")));

    wxTreeItemId payeesOverTimeCalMonth = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCalMonth, new mmTreeItemData(wxT("Payee Report - Month")));

    wxTreeItemId payeesOverTimeCurrentMonth = navTreeCtrl_->AppendItem(payeesOverTime, _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentMonth, new mmTreeItemData(wxT("Payee Report - Current Month")));

    wxTreeItemId payeesOverTimeLast30 = navTreeCtrl_->AppendItem(payeesOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLast30, new mmTreeItemData(wxT("Payee Report - 30 Days")));
    
    wxTreeItemId payeesOverTimeLastYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLastYear, new mmTreeItemData(wxT("Payee Report - Last Year")));

    wxTreeItemId payeesOverTimeCurrentYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentYear, new mmTreeItemData(wxT("Payee Report - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId payeesOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(payeesOverTimeLastFinancialYear, new mmTreeItemData(wxT("Payee Report - Last Financial Year")));

        wxTreeItemId payeesOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(payeesOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(payeesOverTimeCurrentFinancialYear, new mmTreeItemData(wxT("Payee Report - Current Financial Year")));
    }
    ///////////////////////////////////////////////////////////////////

    wxTreeItemId incexpOverTime = navTreeCtrl_->AppendItem(reports, _("Income vs Expenses"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTime, new mmTreeItemData(wxT("Income vs Expenses")));

    wxTreeItemId incexpOverTimeCalMonth = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCalMonth, new mmTreeItemData(wxT("Income vs Expenses - Month")));

    wxTreeItemId incexpOverTimeCurrentMonth = navTreeCtrl_->AppendItem(incexpOverTime, _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentMonth, new mmTreeItemData(wxT("Income vs Expenses - Current Month")));

    wxTreeItemId incexpOverTimeLast30 = navTreeCtrl_->AppendItem(incexpOverTime, _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLast30, new mmTreeItemData(wxT("Income vs Expenses - 30 Days")));
    
    wxTreeItemId incexpOverTimeLastYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLastYear, new mmTreeItemData(wxT("Income vs Expenses - Last Year")));

    wxTreeItemId incexpOverTimeCurrentYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentYear, new mmTreeItemData(wxT("Income vs Expenses - Current Year")));

    if (financialYearIsDifferent())
    {
        wxTreeItemId incexpOverTimeLastFinancialYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Last Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(incexpOverTimeLastFinancialYear, new mmTreeItemData(wxT("Income vs Expenses - Last Financial Year")));

        wxTreeItemId incexpOverTimeCurrentFinancialYear = navTreeCtrl_->AppendItem(incexpOverTime, _("Current Financial Year"), 4, 4);
        navTreeCtrl_->SetItemData(incexpOverTimeCurrentFinancialYear, new mmTreeItemData(wxT("Income vs Expenses - Current Financial Year")));
    }

    wxTreeItemId incexpmonthly = navTreeCtrl_->AppendItem(incexpOverTime, _("Income vs Expenses - All Time"), 4, 4);
    navTreeCtrl_->SetItemData(incexpmonthly, new mmTreeItemData(wxT("Income vs Expenses - All Time")));

    //////////////////////////////////////////////////////////////////
    wxTreeItemId transactionList = navTreeCtrl_->AppendItem(reports, _("Transaction Report"), 4, 4);
    navTreeCtrl_->SetItemData(transactionList, new mmTreeItemData(wxT("Transaction Report")));

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

    wxString vAccts = mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = m_core->rangeAccount();
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
            if ((vAccts == wxT("Open") && pTA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pTA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
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
            if ((vAccts == wxT("Open") && pIA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pIA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
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
    if ( (dayNum < 1) || (dayNum > 31 ) ) {
        dayNum = 1;
    } else if ( ( (monthNum == wxDateTime::Feb) && (dayNum > 28) ) || 
        ( ((monthNum == wxDateTime::Sep) || (monthNum == wxDateTime::Apr) || 
           (monthNum == wxDateTime::Jun) || (monthNum == wxDateTime::Nov)) && (dayNum > 29) ) )
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
    wxString rfn = custRepIndex_->reportFileName(index);
    if (rfn != wxT(""))
    {
        wxString sqlStr;
        if (custRepIndex_->getSqlFileData(sqlStr) )
        {
            mmCustomSQLReport* csr = new mmCustomSQLReport(m_core.get(), custRepIndex_->currentReportTitle(), sqlStr);
            menuPrintingEnable(true);
            createReportsPage(csr);
        }
    }
}

bool mmGUIFrame::CustomSQLReportSelected( int& customSqlReportID, mmTreeItemData* iData )
{
    wxStringTokenizer tk(iData->getString(), wxT("_"));
    wxString tk1 = tk.GetNextToken();
    wxString tk2 = tk.GetNextToken();
    wxString indexStr = tk.GetNextToken();
    bool result = false;
    if (tk1 == wxT("Custom") && tk2 == wxT("Report") && indexStr.IsNumber())
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
    else if (iData->getString() == wxT("Budgeting"))
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
    else if (iData->getString() == wxT("Budgeting"))
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
            Freeze();
            wxTreeItemId idparent = navTreeCtrl_->GetItemParent(id);
            mmTreeItemData* iParentData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(idparent));
            if (iParentData->getString() == wxT("Budget Performance")) 
            {
                wxProgressDialog proDlg(_("Budget Performance"), reportWaitingMsg, 100, this);
                mmPrintableBase* rs = new mmReportBudgetingPerformance(m_core.get(), this, data);
                proDlg.Update(70);
                menuPrintingEnable(true);
                createReportsPage(rs);
                proDlg.Update(95);
            } 
            else if (iParentData->getString() == wxT("Budget Setup Performance")) 
            {
                wxProgressDialog proDlg(_("Budget Category Summary"), reportWaitingMsg, 100, this);
                mmPrintableBase* rs = new mmReportBudgetCategorySummary(m_core.get(), this, data);
                proDlg.Update(70);
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
            Thaw();
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
                 Freeze();
                 wxSizer *sizer = cleanupHomePanel();

                 panelCurrent_ = new mmStocksPanel(m_db.get(), m_inidb.get(), m_core.get(), data, homePanel, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
                 sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

                 homePanel->Layout();
                 Thaw();
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
        
        if (iData->getString() == wxT("Summary of Accounts"))
        {
            mmPrintableBase* rs = new mmReportSummary(m_core.get(), this);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Summary of Stocks"))
        {
            mmPrintableBase* rs = new mmReportSummaryStocks(m_core.get(), m_db.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if ( CustomSQLReportSelected(customSqlReportID, iData) )
        {
            CreateCustomReport(customSqlReportID);
        }
        else if (iData->getString() == wxT("Summary of Assets"))
        {
            mmPrintableBase* rs = new mmReportSummaryAssets(m_core.get(), m_db.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Categories - Over Time"))
        {
            wxProgressDialog proDlg(_("Category Report"), _("Category Report being generated... Please wait."), 100, this);
            mmPrintableBase* rs = new mmReportCategoryOverTimePerformance(m_core.get());
            proDlg.Update(70);
            menuPrintingEnable(true);
            createReportsPage(rs);
            proDlg.Update(95);
        }
        else if (iData->getString() == wxT("Categories - Month"))
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
        else if (iData->getString() == wxT("Categories - 30 Days"))
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
        else if (iData->getString() == wxT("Categories - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            wxString title = _("Categories - Current Month");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Categories - Last Year"))
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
        else if (iData->getString() == wxT("Categories - Current Year"))
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
        else if (iData->getString() == wxT("Categories - Last Financial Year"))
        {
            wxDateTime refDate = wxDateTime(getUserDefinedFinancialYear(true));
            wxDateTime dtEnd = refDate;
            wxDateTime dtBegin = refDate.Subtract(wxDateSpan::Year());
            wxString title = _("Categories - Last Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Categories - Current Financial Year"))
        {
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear(true));
            wxDateTime dtEnd   = wxDateTime::Now();
            wxString title = _("Categories - Current Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Comes From"))
        {
            wxDateTime dtEnd =  wxDateTime::Now();
            wxDateTime dtBegin =  wxDateTime::Now();
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), true, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Comes From - Month"))
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
        else if (iData->getString() == wxT("Where the Money Comes From - 30 Days"))
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
        else if (iData->getString() == wxT("Where the Money Comes From - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            wxString title = _("Where the Money Comes From - Current Month");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Comes From - Last Year"))
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
        else if (iData->getString() == wxT("Where the Money Comes From - Current Year"))
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
        else if (iData->getString() == wxT("Where the Money Comes From - Last Financial Year"))
        {
            wxDateTime refDate = wxDateTime(getUserDefinedFinancialYear(true));
            wxDateTime dtEnd = refDate;
            wxDateTime dtBegin = refDate.Subtract(wxDateSpan::Year());
            wxString title = _("Where the Money Comes From - Last Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Comes From - Current Financial Year"))
        {
            wxDateTime dtEnd   = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear(true));

            wxString title = _("Where the Money Comes From - Current Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Goes"))
        {
            wxDateTime dtEnd = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime::Now();
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), true, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Goes - Month"))
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
        else if (iData->getString() == wxT("Where the Money Goes - 30 Days"))
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
        else if (iData->getString() == wxT("Where the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            wxString title = _("Where the Money Goes - Current Month");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Goes - Last Year"))
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
        else if (iData->getString() == wxT("Where the Money Goes - Current Year"))
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
        else if (iData->getString() == wxT("Where the Money Goes - Last Financial Year"))
        {
            wxDateTime refDate = (getUserDefinedFinancialYear(true));
            wxDateTime dtEnd = refDate;
            wxDateTime dtBegin = refDate.Subtract(wxDateSpan::Year());
            wxString title = _("Where the Money Goes - Last Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Where the Money Goes - Current Financial Year"))
        {
            wxDateTime dtEnd   = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear(true));
            wxString title = _("Where the Money Goes - Current Financial Year");
            mmPrintableBase* rs = new mmReportCategoryExpenses(m_core.get(), false, dtBegin, dtEnd, title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Transaction Statistics"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportTransactionStats(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Income vs Expenses"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Income vs Expenses - Month"))
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
        else if (iData->getString() == wxT("Income vs Expenses - 30 Days"))
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
        else if (iData->getString() == wxT("Income vs Expenses - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            wxString title = _("Income vs Expenses - Current Month");
            mmPrintableBase* rs = new mmReportIncomeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Income vs Expenses - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Income vs Expenses - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Income vs Expenses - Last Financial Year"))
        {
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear());

            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            if (today.GetMonth() < dtBegin.GetMonth()) year -- ;
            mmPrintableBase* rs = new mmReportIncExpensesOverFinancialPeriod(this, m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Income vs Expenses - Current Financial Year"))
        {
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear());
            
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            if (today.GetMonth() < dtBegin.GetMonth()) year --;
            mmPrintableBase* rs = new mmReportIncExpensesOverFinancialPeriod(this, m_core.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Income vs Expenses - All Time"))
        {
            wxString title = _("Income vs Expenses - All Time");
            mmPrintableBase* rs = new mmReportIncomeExpenses(m_core.get(), true, wxDateTime::Now(), wxDateTime::Now(), title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Payee Report"))
        {
            wxString title = _("Payee Report");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), true, wxDateTime::Now(), wxDateTime::Now(), title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Payee Report - Month"))
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
        else if (iData->getString() == wxT("Payee Report - 30 Days"))
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
        else if (iData->getString() == wxT("Payee Report - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            dtBegin.Add(wxDateSpan::Day());

            wxString title = _("Payees - Current Month");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Payee Report - Last Year"))
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
        else if (iData->getString() == wxT("Payee Report - Current Year"))
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
        else if (iData->getString() == wxT("Payee Report - Last Financial Year"))
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
        else if (iData->getString() == wxT("Payee Report - Current Financial Year"))
        {
            wxDateTime dtEnd   = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime(getUserDefinedFinancialYear());

            wxString title = _("Payees - Current Financial Year");
            mmPrintableBase* rs = new mmReportPayeeExpenses(m_core.get(), false, dtBegin, dtEnd, title);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        else if (iData->getString() == wxT("Cash Flow"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get());

            report->activateBankAccounts();
            if (hasActiveTermAccounts())  report->activateTermAccounts();

            menuPrintingEnable(true);
            createReportsPage(report);
        }
        else if (iData->getString() == wxT("Cash Flow - With Bank Accounts"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get());

            report->activateBankAccounts();

            menuPrintingEnable(true);
            createReportsPage(report);
        }
 
        else if (iData->getString() == wxT("Cash Flow - With Term Accounts"))
        {
            mmReportCashFlow* report = new mmReportCashFlow(m_core.get());

            report->activateTermAccounts();

            menuPrintingEnable(true);
            createReportsPage(report);
        }
        else if (iData->getString() == wxT("Cash Flow - Specific Accounts"))
        {
            navTreeCtrl_->UnselectAll();    // item in navTreeCtrl_ to enable re-selection.
            processPendingEvents();         // Clear event buffer before activating report.
            OnCashFlowSpecificAccounts();
        }
        else if (iData->getString() == wxT("Transaction Report"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TRANSACTIONREPORT);
            AddPendingEvent(evt);           // Events will be processed in due course.
            navTreeCtrl_->UnselectAll();    // item in navTreeCtrl_ to enable re-selection.
        }
        else if (iData->getString() == wxT("Bills & Deposits"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BILLSDEPOSITS);
            AddPendingEvent(evt);
        }
        else if (iData->getString() == wxT("Assets"))
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
            // Bring up popup menu to edit or delete the correct Custom SQL Report
            customSqlReportSelectedItem_ = iData->getString();
            wxString field = customSqlReportSelectedItem_.Mid(6,8);
            if (field == wxT("_Report_"))
            {
                wxMenu* customReportMenu = new wxMenu;
                customReportMenu->Append(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_EDIT, _("Edit Custom Report"));
                customReportMenu->Append(MENU_TREEPOPUP_CUSTOM_SQL_REPORT_DELETE, _("Delete Custom Report"));
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
    wxString vAccts = mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

    //Set view ALL 
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

    //Refresh Navigation Panel
    mmGUIFrame::updateNavTreeControl();

    //Restore settings
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), vAccts);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewFavoriteAccounts(wxCommandEvent&)
{
    //Get current settings for view accounts
    wxString vAccts = mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

    //Set view ALL 
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), wxT("Favorites"));

    //Refresh Navigation Panel
    mmGUIFrame::updateNavTreeControl();

    //Restore settings
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), vAccts);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewOpenAccounts(wxCommandEvent&)
{
    //Get current settings for view accounts
    wxString vAccts = mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

    //Set view ALL 
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), wxT("Open"));

    //Refresh Navigation Panel
    mmGUIFrame::updateNavTreeControl();

    //Restore settings
    mmDBWrapper::setINISettingValue(m_inidb.get(), wxT("VIEWACCOUNTS"), vAccts);
}

//----------------------------------------------------------------------------
void mmGUIFrame::createBudgetingPage(int budgetYearID)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmBudgetingPanel(m_db.get(), m_inidb.get(), m_core.get(), this, budgetYearID, 
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
        //panelCurrent_->DestroyChildren();
        //panelCurrent_->SetSizer(NULL);
        panelCurrent_  = 0;
    }
    panelCurrent_ = new mmHomePagePanel(this, 
        m_db.get(), 
        m_inidb.get(),
        m_core.get(), 
        m_topCategories,
        homePanel, 
        ID_PANEL3, 
        wxDefaultPosition, 
        wxDefaultSize, 
        wxNO_BORDER|wxTAB_TRAVERSAL);
    
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
     
    homePanel->Layout();
    refreshRequested_ = false;
}
//----------------------------------------------------------------------------

void mmGUIFrame::createReportsPage(mmPrintableBase* rs)
{
    wxSizer *sizer = cleanupHomePanel();
       
    panelCurrent_ = new mmReportsPanel(this, m_db.get(), rs, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
     
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
    menuClearRecentFiles->SetBitmap(toolBarBitmaps[9]);
    menu_file->Append(menuClearRecentFiles);
    menu_file->AppendSeparator();

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

void mmGUIFrame::createDataStore(const wxString& fileName, const wxString& pwd, bool openingNew)
{
    if (m_core) m_core.reset();

    if (m_db)
    {
        m_db->Close();
        m_db.reset();

        /// Update the database according to user requirements
        if (mmOptions::instance().databaseUpdated_ && 
           (mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("BACKUPDB_UPDATE"), wxT("FALSE")) == wxT("TRUE")))
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
        if (mmDBWrapper::getINISettingValue(m_inidb.get(), wxT("BACKUPDB"), wxT("FALSE")) == wxT("TRUE"))
        {
            BackupDatabase(fileName);
        }

        m_db = mmDBWrapper::Open(fileName, password);
        // if the database pointer has been reset, the password is possibly incorrect
        if (!m_db) return;

        // we need to check the db whether it is the right version
        if (!mmDBWrapper::checkDBVersion(m_db.get()))
        {
            wxString note = mmex::getProgramName() + _(" - No File opened ");
            this->SetTitle(note);   
            wxMessageBox(_("Sorry. The Database version is too old or Database password is incorrect"),
                        dialogErrorMessageHeading, wxICON_EXCLAMATION);

            m_db->Close();
            m_db.reset();
            return ;
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
           return;
       }
    }
    else // open of existing database failed
    {
        wxString note = mmex::getProgramName() + _(" - No File opened ");
        this->SetTitle(note);   
        
        wxString msgStr = _("Cannot locate previously opened database.\n");
        if (!passwordCheckPassed)
            msgStr = _("Password not entered for encrypted Database.\n");

        wxMessageDialog msgDlg(this, msgStr + _("Do you want to browse to locate another file?"), 
                                dialogErrorMessageHeading, wxYES_NO | wxYES_DEFAULT);
        if (msgDlg.ShowModal() == wxID_YES)
        {
            wxCommandEvent evt;
            OnOpen(evt);
            return;
        }
        menuEnableItems(false);
        return ;
    }

    openDataBase(fileName);
}
//----------------------------------------------------------------------------

void mmGUIFrame::openDataBase(const wxString& fileName)
{
    wxProgressDialog dlg(_("Please Wait"), 
        _("Opening Database File && Verifying Integrity"), 100, this, 
        wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH );

    mmDBWrapper::initDB(m_db.get(), &dlg);
    dlg.Update(100);
    dlg.Destroy();

    wxString title = mmex::getProgramName() + wxT(" : ") + fileName;

    if (mmex::isPortableMode())
        title << wxT(" [") << _("portable mode") << wxT(']');

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

void mmGUIFrame::openFile(const wxString& fileName, bool openingNew, const wxString &password)
{
    createDataStore(fileName, password, openingNew);
  
    if (m_db) 
    {
        menuEnableItems(true);
        menuPrintingEnable(false);
        autoRepeatTransactionsTimer_.Start(REPEAT_TRANS_DELAY_TIME, wxTIMER_ONE_SHOT); 
    }
    
    updateNavTreeControl();

    if (!refreshRequested_) 
    {
        refreshRequested_ = true;
        /* Currency Options might have changed so refresh */
        wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
        GetEventHandler()->AddPendingEvent(ev); 
    }

    if (!m_db)
        showBeginAppDialog();
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
        SetDatabaseFile(fileName);
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

    mmUnivCSVDialog(m_core.get(), this).ShowModal();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    autoRepeatTransactionsTimer_.Stop();
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
                msgStr << _("Term Account views have been temporarly turned on.") << wxT("\n")
                       << _("To maintain this view, change the defaults by using:\n\nTools -> Options\nView Options")
                       << wxT("\n\n")
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
        mmReportCashFlow* report = new mmReportCashFlow(m_core.get(), selections);

        report->activateBankAccounts();
        if (this->hasActiveTermAccounts()) report->activateTermAccounts();

        menuPrintingEnable(true);
        createReportsPage(report);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOptions(wxCommandEvent& /*event*/)
{
    if (!m_db.get() || !m_inidb) return;

    mmOptionsDialog systemOptions(m_core.get(), m_inidb.get(), this);
    if (systemOptions.ShowModal() == wxID_OK)
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

        int messageIcon = wxICON_INFORMATION;
        wxString sysMsg = wxString() << _("MMEX Options have been updated.") << wxT("\n\n");
        if (systemOptions.RequiresRestart())
        {
            messageIcon = wxICON_WARNING;
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
    menuPrintingEnable(true);
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

void mmGUIFrame::OnCheckUpdate(wxCommandEvent& /*event*/)
{
    // Set up system information
    wxString versionDetails = wxString()
        << wxT("MMEX: ")     << mmex::getProgramVersion()       << wxT("\n")
        << _("System: ")     << wxPlatformInfo::Get().GetOperatingSystemIdName() << wxT("\n\n")
        << wxVERSION_STRING  << wxT("\n")
        << wxT("Boost C++ ") << (BOOST_VERSION/100000)          << wxT('.') 
                             << (BOOST_VERSION / 100 % 1000)    << wxT('.') 
                             << (BOOST_VERSION % 100)           << wxT("\n")
        << wxT("SQLite3: ")  << wxSQLite3Database::GetVersion() << wxT("\n")
        << wxT("wxSQLite3: 3.0.0 by Ulrich Telle")              << wxT("\n\n");

    // Access current version details page
    wxString site = wxT("http://www.codelathe.com/mmex/version.html");
    wxURL url(site);

    unsigned char buf[1024];
    boost::scoped_ptr<wxInputStream> in_stream(url.GetInputStream());
    if (!in_stream)
    {
        versionDetails << _("Unable to connect!");
        wxMessageBox(versionDetails, _("MMEX System Information Check"));
        return;
    }
    in_stream->Read(buf, 1024);
    //size_t bytes_read=in_stream->LastRead();
    in_stream.reset();
    buf[53] = '\0';

    wxString page = wxString::FromAscii((const char *)buf);
    /*************************************************************************
        Expected format of the string from the internet. Version: 0.9.8.0
        page = wxT("x.x.x.x - Win: w.w.w.w - Unix: u.u.u.u - Mac: m.m.m.m");
        string length = 53 characters
    **************************************************************************/
    wxStringTokenizer sysTokens(page,wxT("-"));
    // Ignored the first token. Added for compatibility for pre 0.9.8.0
    wxString oldSys  = sysTokens.GetNextToken(); 
    wxString winSys  = sysTokens.GetNextToken().Trim(false);
    wxString unixSys = sysTokens.GetNextToken().Trim(false);
    wxString macSys  = sysTokens.GetNextToken().Trim(false);

    wxString mySys =  wxPlatformInfo::Get().GetOperatingSystemFamilyName();

    wxStringTokenizer mySysToken;
    if (mySys == wxT("Windows")) 
        mySysToken.SetString(winSys,wxT(":"));
    else if (mySys == wxT("Unix"))
        mySysToken.SetString(unixSys,wxT(":"));
    else if (mySys == wxT("Macintosh"))
        mySysToken.SetString(macSys,wxT(":"));
    
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
    wxArrayString   currency_name, currency_rate;
    double          *rate_ptr, rate_value, base_rate;
    
    // we will get latest currency rate data from European Central Bank
    wxString site = wxT("http://www.ecb.europa.eu/stats/eurofxref/eurofxref-daily.xml");

    if (!m_core) 
    {
       wxMessageBox(_("No database!"), _("Update Currency Rate"), wxICON_WARNING);
       return;
    }

    wxURL url(site);

    wxInputStream* in_stream = url.GetInputStream();

    if (!in_stream) 
    {
        wxMessageBox(_("Unable to connect!"), _("Update Currency Rate"), wxICON_WARNING);
        return;
    }
    
    wxXmlDocument doc;

    if(!doc.Load(*in_stream)) 
    {
        wxMessageBox(_("Cannot get data from WWW!"), _("Update Currency Rate"), wxICON_WARNING);
        return;
    }

    // decode received XML data
    if(doc.GetRoot()->GetName() != wxT("gesmes:Envelope")) 
    {
        wxMessageBox(_("Incorrect XML data (#1)!"), _("Update Currency Rate"), wxICON_WARNING);
        return;
    }

    wxXmlNode *root_child = doc.GetRoot()->GetChildren();

    while(root_child) 
    {
        if(root_child->GetName() == wxT("gesmes:subject")) 
        {
            if(root_child->GetNodeContent() != wxT("Reference rates")) 
            {
                wxMessageBox(_("Incorrect XML data (#2)!"), _("Update Currency Rate"), wxICON_WARNING);
                return;
            }
        } 
        else if (root_child->GetName() == wxT("gesmes:Sender")) 
        {
            wxXmlNode *sender_child = root_child->GetChildren();

            if(!sender_child) 
            {
                wxMessageBox(_("Incorrect XML data (#3)!"), _("Update Currency Rate"), wxICON_WARNING);
                return;
            }

            if(sender_child->GetName() != wxT("gesmes:name")) 
            {
                wxMessageBox(_("Incorrect XML data (#4)!"), _("Update Currency Rate"), wxICON_WARNING);
                return;
            }

            if(sender_child->GetNodeContent() != wxT("European Central Bank")) 
            {
                wxMessageBox(_("Incorrect XML data (#5)!"), _("Update Currency Rate"), wxICON_WARNING);
                return;
            }            
        } 
        else if (root_child->GetName() == wxT("Cube")) 
        {
            wxXmlNode *cube_lv1_child = root_child->GetChildren();

            if(!cube_lv1_child) 
            {
                wxMessageBox(_("Incorrect XML data (#6)!"), _("Update Currency Rate"), wxICON_WARNING);
                return;
            }

            if(cube_lv1_child->GetName() != wxT("Cube")) 
            {
                wxMessageBox(_("Incorrect XML data (#7)!"), _("Update Currency Rate"), wxICON_WARNING);
                return;
            }

            wxXmlNode *cube_lv2_child = cube_lv1_child->GetChildren();

            // set default base currency and its rate            
            currency_name.Add(wxT("EUR"));
            currency_rate.Add(wxT("1.0"));

            while(cube_lv2_child) 
            {
                wxString name = cube_lv2_child->GetPropVal(wxT("currency"), wxGetEmptyString());
                wxString rate = cube_lv2_child->GetPropVal(wxT("rate"), wxT("1"));

                currency_name.Add(name);
                currency_rate.Add(rate);
                
                cube_lv2_child = cube_lv2_child->GetNext();
            }
        }
        root_child = root_child->GetNext();
    }    

    // Get base currency and the adjust currency rate for each currency
    rate_ptr = new double[currency_rate.GetCount()];

    int currencyID = mmDBWrapper::getBaseCurrencySettings(m_core->db_.get());
    wxString base_symbol = mmDBWrapper::getCurrencySymbol(m_core->db_.get(), currencyID);
    base_rate = 0;

    for(int i=0; i<(int)currency_rate.GetCount(); i++) 
    {
        if(currency_rate[i].ToDouble(&rate_value) == false) 
        {
            rate_ptr[i] = 1.0;
        } else 
        {
            rate_ptr[i] = rate_value;
            if(currency_name[i] == base_symbol)
                base_rate = rate_value;
        }
    }

    if(base_rate == 0) 
    {
        wxMessageBox(_("Could not find base currency symbol!"), _("Update Currency Rate"), wxICON_WARNING);
        return;
    }

    // Note:
    // Suppose currency Y / currency X = Y / X and currency Z / currency X = Z / X, then
    // currency Z / currency Y = Z / Y
    // Therefore, if currency X is EUR and currency Y is the base currency, 
    // currency Y : currency Z = base_rate / rate of Z : 1
    for(int i=0; i<(int)currency_rate.GetCount(); i++) 
        rate_ptr[i] = base_rate / rate_ptr[i];

    // update currency rates
    
    wxString msg = _("Currency rate updated");
    msg << wxT("\n\n");
    for (int idx = 0; idx < (int)m_core->currencyList_.currencies_.size(); idx++) 
    {
        wxString currencySymbol  = m_core->currencyList_.currencies_[idx]->currencySymbol_;
        
        for(int i=0; i<(int)currency_name.GetCount(); i++) 
        {
            if(currency_name[i] == currencySymbol) 
            {
                double rate = m_core->currencyList_.currencies_[idx]->baseConv_ ;
                if (rate != rate_ptr[i])
                msg << currencySymbol << wxT(" : ") << rate << wxT(" -> ") << rate_ptr[i] << wxT("\n");

                m_core->currencyList_.currencies_[idx]->baseConv_ = rate_ptr[i];
                m_core->currencyList_.updateCurrency(m_core->currencyList_.currencies_[idx]);
            }
        } // for i loop
    }

    delete[] rate_ptr;

    wxMessageDialog msgDlg(this, msg, _("Currency rate updated"));
    
    //fileviewer(wxT(""), this).ShowModal();
    msgDlg.ShowModal();

    createHomePage();
    updateNavTreeControl();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnReportIssues(wxCommandEvent& /*event*/)
{
   wxString url = wxT("http://www.codelathe.com/forum");
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
    mmAboutDialog(m_inidb.get(), this).ShowModal();
}
//----------------------------------------------------------------------------

void mmGUIFrame::restorePrinterValues()
{
    // Startup Default Settings
    wxString paperID_String     = wxString() << wxPAPER_A4;
    wxString pageOrientationStr = wxString() << wxPORTRAIT;

    long leftMargin;
    long rightMargin;
    long topMargin;
    long bottomMargin;
    long paperID;
    long pageOrientation;

    mmDBWrapper::getINISettingValue( m_inidb.get(), wxT("PRINTER_LEFT_MARGIN"), wxT("20") ).ToLong(&leftMargin);
    mmDBWrapper::getINISettingValue( m_inidb.get(), wxT("PRINTER_RIGHT_MARGIN"), wxT("20")).ToLong(&rightMargin);
    mmDBWrapper::getINISettingValue( m_inidb.get(), wxT("PRINTER_TOP_MARGIN"), wxT("20")).ToLong(&topMargin);
    mmDBWrapper::getINISettingValue( m_inidb.get(), wxT("PRINTER_BORTTOM_MARGIN"), wxT("20")).ToLong(&bottomMargin);
    mmDBWrapper::getINISettingValue( m_inidb.get(), wxT("PRINTER_PAGE_ORIENTATION"), pageOrientationStr).ToLong(&pageOrientation);
    mmDBWrapper::getINISettingValue( m_inidb.get(), wxT("PRINTER_PAGE_ID"), paperID_String).ToLong(&paperID);

    wxPoint topLeft(leftMargin, topMargin);
    wxPoint bottomRight(rightMargin, bottomMargin);

    wxPageSetupDialogData* pinterData = printer_->GetPageSetupData();  
    pinterData->SetMarginTopLeft(topLeft);
    pinterData->SetMarginBottomRight(bottomRight);

    wxPrintData* printerData = printer_->GetPrintData();
    printerData->SetOrientation(pageOrientation);
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

        m_inidb.get()->Begin();

        wxString leftMargin = wxString() << topLeft.x ;
        mmDBWrapper::setINISettingValue( m_inidb.get(), wxT("PRINTER_LEFT_MARGIN"), leftMargin);

        wxString rightMargin = wxString() << bottomRight.x;
        mmDBWrapper::setINISettingValue( m_inidb.get(), wxT("PRINTER_RIGHT_MARGIN"), rightMargin);

        wxString topMargin = wxString() << topLeft.y ;
        mmDBWrapper::setINISettingValue( m_inidb.get(), wxT("PRINTER_TOP_MARGIN"), topMargin);

        wxString bottomMargin = wxString() << bottomRight.y ;
        mmDBWrapper::setINISettingValue( m_inidb.get(), wxT("PRINTER_BORTTOM_MARGIN"), bottomMargin);

        wxString orientation = wxString() << pageOrientation; 
        mmDBWrapper::setINISettingValue( m_inidb.get(), wxT("PRINTER_PAGE_ORIENTATION"), orientation);

        wxString pageID = wxString() << paperID; 
        mmDBWrapper::setINISettingValue( m_inidb.get(), wxT("PRINTER_PAGE_ID"), pageID);

        m_inidb.get()->Commit();
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

void mmGUIFrame::showBeginAppDialog()
{
    mmAppStartDialog dlg(m_inidb.get(), this);
    dlg.ShowModal();
    int rc = dlg.getReturnCode();

    if (rc == 0)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_NEW);
        AddPendingEvent(evt);
    }
    else if (rc == 1)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_OPEN);
        AddPendingEvent(evt);
    }
    else if (rc == 2)
    {
       wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_HELP);
       AddPendingEvent(evt);
    }
    else if (rc == 3)
    {
        wxString url = wxT("http://www.codelathe.com/mmex/index.php");
        wxLaunchDefaultBrowser(url);
    }
    else if (rc == 4)
    {
        wxFileName fname(mmDBWrapper::getLastDbPath(m_inidb.get()));
        if (fname.IsOk()) openFile(fname.GetFullPath(), false);
    }
    else if (rc == -1)
    {
        /* Do Nothing in this case */
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
    
    panelCurrent_ = new mmBillsDepositsPanel(m_db.get(), m_inidb.get(), m_core.get(), homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

//----------------------------------------------------------------------------
void mmGUIFrame::createStocksAccountPage(int accountID)
{
    wxSizer *sizer = cleanupHomePanel();
    
    panelCurrent_ = new mmStocksPanel(m_db.get(), m_inidb.get(), m_core.get(),
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
    
    panelCurrent_ = new mmCheckingPanel(m_core.get(), m_inidb.get(),  
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
    panelCurrent_ = new mmAssetsPanel(homePanel, m_db.get(), m_inidb.get(), m_core.get());
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
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
                                       _(" account: ") + m_core->accountList_.accounts_[choice]->name_ + wxT(" ?");
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
    m_mgr.GetPane(wxT("toolbar")).Show(event.IsChecked());
    m_mgr.Update();
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

void mmGUIFrame::OnCategoryRelocation(wxCommandEvent& /*event*/)
{
    relocateCategoryDialog* dlg = new relocateCategoryDialog(m_core.get(), m_db.get(), this);
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString msgStr;
        msgStr << _("Category Relocation Completed.") << wxT("\n\n")
               << dlg->updatedCategoriesCount() << _(" records have been updated in the database.") << wxT("\n\n")
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
        msgStr << _("Payee Relocation Completed.") << wxT("\n\n")
               << dlg->updatedPayeesCount() << _(" records have been updated in the database.") << wxT("\n\n")
               << _("MMEX must be shutdown and restarted for all the changes to be seen.");
        wxMessageBox(msgStr,_("Payee Relocation Result"));
        mmOptions::instance().databaseUpdated_ = true;
    }
}

void mmGUIFrame::RunCustomSqlDialog(bool forEdit)
{
    //Use Shared pointer to ensure object gets destroyed if SQL Script errors hijack the object. 
    boost::shared_ptr<mmCustomSQLDialog> dlg( new mmCustomSQLDialog(custRepIndex_, this, forEdit ));   
    int dialogStatus = dlg->ShowModal();
    wxBeginBusyCursor(wxHOURGLASS_CURSOR);
    while (dialogStatus == wxID_MORE)
    {
        if (dlg->sqlQuery() != wxT(""))   
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
                              << wxT("\n\n")  
                              << custRepIndex_->currentReportTitle();
    if ( wxMessageBox(msg ,custRepIndex_->UserDialogHeading(),wxYES_NO|wxICON_QUESTION) == wxYES )
    {
        custRepIndex_->deleteSelectedReportTitle();

        if (! custRepIndex_->currentReportFileName(false).IsEmpty())
        {
            msg = wxString() << _("Do you want to delete the SQL file as well?") << wxT("\n"); 
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

bool wxNewDatabaseWizardPage1::TransferDataFromWindow()
{
    if ( currencyID_ == -1)
    {
        wxMessageBox(_("Base Currency Not Set"), _("New Database"), wxICON_WARNING | wxOK, this);

        return false;
    }
    userName = itemUserName_->GetValue().Trim();
    mmDBWrapper::setInfoSettingValue(parent_->m_core->db_.get(), wxT("USERNAME"), userName); 

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
    helpMsg << _("Specify a descriptive name for the account.") << wxT("\n")
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
        errorMsg << _("Base Account Currency Not set.") << wxT("\n")
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
    catch (wxSQLite3Exception &e) 
    {
        wxLogError(e.GetMessage());
    }
    catch (std::exception &e)
    {
        wxString msg(e.what(), wxConvCurrent); // wxConvLibc, wxConvLocal
        wxLogError(msg);
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
    catch (wxSQLite3Exception &e)
    {
        wxLogError(e.GetMessage());
    } 
    catch (std::exception &e)
    {
        wxString msg(e.what(), wxConvCurrent);
        wxLogError(msg);
    }

    return ok;
}
//----------------------------------------------------------------------------
    
void mmGUIApp::OnFatalException()
{
    reportFatalException(wxDebugReport::Context_Exception);
}
//----------------------------------------------------------------------------

void mmGUIFrame::SetDatabaseFile(wxString dbFileName, bool newDatabase)
{
    autoRepeatTransactionsTimer_.Stop();
    wxProgressDialog *progress = NULL;
    if (! newDatabase)
    {
        progress = new wxProgressDialog( _("Setting new Database file"), _("Please wait while the new database is being loaded."), 100, this);
        progress->Update(20);
    }

    // Ensure database is in a steady state first
    if (!activeHomePage_)
    {
        refreshRequested_ = true;
        createHomePage();
    }

    if (progress) progress->Update(40);
    openFile(dbFileName, newDatabase);

    if (progress) progress->Update(95);
    recentFiles_->updateRecentList(dbFileName);

    if (progress)
    {
        progress->Update(100);
        progress->Destroy();
    }
}

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

    // limit the amount of backup files to 4
    if (backupFileArray.Count() > 4)
    {
        backupFileArray.Sort(true);
        // ensure file is not read only before deleting file.
        wxFileName fnLastFile(backupFileArray.Last());
        if (fnLastFile.IsFileWritable()) wxRemoveFile(backupFileArray.Last());
    }

#if 0
//  Start debugging code
    wxLogMessage(wxString() << wxT("Before - File List size:") << backupFileArray.Count());
    if (backupFileArray.Count() > 0)
    {
        backupFileArray.Sort(true);
        for (size_t debugIndex = 0; debugIndex < backupFileArray.Count(); debugIndex++)
        {
            wxLogMessage(wxString() << wxT("File ") <<debugIndex << wxT(": ") << backupFileArray[debugIndex]);
        }
    }

    wxLogMessage(wxT(" "));
    wxLogMessage(wxString() << wxT("After - File List size:") << backupFileArray.Count());
    backupFileArray.Clear();
    backupFile = wxFindFirstFile(fileSearch);
    while (!backupFile.empty())
    {
        backupFileArray.Add(backupFile);
        backupFile = wxFindNextFile();
    }

    if (backupFileArray.Count() > 0)
    {
        backupFileArray.Sort(true);
        for (size_t debugIndex = 0; debugIndex < backupFileArray.Count(); debugIndex++)
        {
            wxLogMessage(wxString() << wxT("File ") <<debugIndex << wxT(": ") << backupFileArray[debugIndex]);
        }
    }
//  End debugging code
#endif
}

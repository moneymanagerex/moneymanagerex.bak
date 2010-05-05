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
#include "guiid.h"
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
#include "reportsummarystocks.h"
#include "reportsummaryassets.h"
#include "reporttransactions.h"
#include "reportcashflow.h"
#include "reporttransstats.h"
#include "reportcategovertimeperf.h"
#include "reportbudgetsetup.h"
#include "mmgraphtopcategories.h"
#include "appstartdialog.h"
#include "aboutdialog.h"
#include "newacctdialog.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "budgetyeardialog.h"
#include "optionsdialog.h"
#include "currencydialog.h"
#include "maincurrencydialog.h"
#include "filtertransdialog.h"
#include "billsdepositsdialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "paths.h"
#include "constants.h"
#include "platfdep.h"
#include "helpers.h"
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

class mmNewDatabaseWizard : public wxWizard
{
public:
    mmNewDatabaseWizard(wxFrame *frame, mmCoreDB* core);
    void RunIt(bool modal);

    mmCoreDB* core_;

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

    mmCoreDB* core_;
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

inline bool sortCategs(const CategInfo &elem1, const CategInfo &elem2)
{
    return elem1.amount < elem2.amount;
}
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
    wxImage::AddHandler( new wxICOHandler ); 
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());

    /* Get INI DB for loading settings */
    wxSQLite3Database inidb;
    inidb.Open(mmex::getPathUser(mmex::SETTINGS));

    mmDBWrapper::verifyINIDB(&inidb);

    /* Load Colors from Database */
    mmLoadColorsFromDatabase(&inidb);

    /* Load MMEX Custom Settings */
    mmIniOptions::loadOptions(&inidb);

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

/*
    Extention expected in lower case.
*/
void fixFileExt(wxFileName &f, const wxString &ext)
{
    if (f.GetExt().Lower() != ext) {
        f.SetExt(ext);
    }
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
    EVT_MENU(MENU_EXPORT_CSV, mmGUIFrame::OnExport)
	EVT_MENU(MENU_EXPORT_QIF, mmGUIFrame::OnExportToQIF)
    EVT_MENU(MENU_IMPORT_CSV, mmGUIFrame::OnImportCSV)
    EVT_MENU(MENU_IMPORT_MMNETCSV, mmGUIFrame::OnImportCSVMMNET)
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
    EVT_MENU(wxID_ABOUT, mmGUIFrame::OnAbout)
    EVT_MENU(MENU_PRINT_PAGE_SETUP, mmGUIFrame::OnPrintPageSetup)
    EVT_MENU(MENU_PRINT_REPORT, mmGUIFrame::OnPrintPageReport)
    EVT_MENU(MENU_PRINT_PREVIEW_REPORT, mmGUIFrame::OnPrintPagePreview)
    EVT_MENU(MENU_SHOW_APPSTART, mmGUIFrame::OnShowAppStartDialog)
    EVT_MENU(MENU_EXPORT_HTML, mmGUIFrame::OnExportToHtml)
    EVT_MENU(MENU_BILLSDEPOSITS, mmGUIFrame::OnBillsDeposits)
    EVT_MENU(MENU_STOCKS, mmGUIFrame::OnStocks)
    EVT_MENU(MENU_ASSETS, mmGUIFrame::OnAssets)
    EVT_MENU(MENU_CURRENCY, mmGUIFrame::OnCurrency)
	EVT_MENU(MENU_TRANSACTIONREPORT, mmGUIFrame::OnTransactionReport)
    EVT_MENU(MENU_TREEPOPUP_LAUNCHWEBSITE, mmGUIFrame::OnLaunchAccountWebsite)
	EVT_MENU(MENU_VIEW_TOOLBAR, mmGUIFrame::OnViewToolbar)
	EVT_MENU(MENU_VIEW_LINKS, mmGUIFrame::OnViewLinks)
    EVT_MENU(MENU_ONLINE_UPD_CURRENCY_RATE, mmGUIFrame::OnOnlineUpdateCurRate)
	EVT_UPDATE_UI(MENU_VIEW_TOOLBAR, mmGUIFrame::OnViewToolbarUpdateUI)
	EVT_UPDATE_UI(MENU_VIEW_LINKS, mmGUIFrame::OnViewLinksUpdateUI)
    
    EVT_MENU(MENU_TREEPOPUP_EDIT, mmGUIFrame::OnPopupEditAccount)
    EVT_MENU(MENU_TREEPOPUP_DELETE, mmGUIFrame::OnPopupDeleteAccount)

    EVT_TREE_ITEM_RIGHT_CLICK(ID_NAVTREECTRL, mmGUIFrame::OnItemRightClick)
    EVT_TREE_SEL_CHANGED(ID_NAVTREECTRL, mmGUIFrame::OnSelChanged)

    EVT_MENU(MENU_GOTOACCOUNT, mmGUIFrame::OnGotoAccount)

    EVT_MENU(MENU_TRANSACTIONREPORT, mmGUIFrame::OnTransactionReport)

    /* Navigation Panel */

	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_NEW, mmGUIFrame::OnNewAccount)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_DELETE, mmGUIFrame::OnDeleteAccount)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_EDIT, mmGUIFrame::OnEditAccount)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_LIST, mmGUIFrame::OnAccountList)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_EXPORT2CSV, mmGUIFrame::OnExport)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_EXPORT2QIF, mmGUIFrame::OnExportToQIF)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTQIF, mmGUIFrame::OnImportQIF)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTCSV, mmGUIFrame::OnImportCSV)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTUNIVCSV, mmGUIFrame::OnImportUniversalCSV)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_IMPORTMMNET, mmGUIFrame::OnImportCSVMMNET)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWALL, mmGUIFrame::OnViewAllAccounts)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE, mmGUIFrame::OnViewFavoriteAccounts)
	EVT_MENU(MENU_TREEPOPUP_ACCOUNT_VIEWOPEN, mmGUIFrame::OnViewOpenAccounts)

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

/*
    Class used to store item specific information in a tree node
*/
class mmTreeItemData : public wxTreeItemData
{
public:
    mmTreeItemData(int id, bool isBudget);
    mmTreeItemData(const wxString& string);

    int getData() const { return id_; }
    wxString getString() const { return stringData_; }
    bool isStringData() const { return isString_; }
    bool isBudgetingNode() const { return isBudgetingNode_; }

private:
    int id_;
    bool isString_;
    bool isBudgetingNode_;
    wxString stringData_;
};
//----------------------------------------------------------------------------

mmTreeItemData::mmTreeItemData(int id, bool isBudget) :
    id_(id), 
    isString_(false), 
    isBudgetingNode_(isBudget) 
{
}
//----------------------------------------------------------------------------

mmTreeItemData::mmTreeItemData(const wxString& string) :
    id_(), 
    isString_(true), 
    isBudgetingNode_(false),
    stringData_(string)
{
}
//----------------------------------------------------------------------------

mmAddAccountWizard::mmAddAccountWizard(wxFrame *frame, mmCoreDB* core) :
    wxWizard(frame,wxID_ANY,_("Add Account Wizard"),
    wxBitmap(addacctwiz_xpm),wxDefaultPosition,
    wxDEFAULT_DIALOG_STYLE), core_(core), acctID_(-1)
{
    // a wizard page may be either an object of predefined class
    page1 = new wxWizardPageSimple(this);

    wxString noteString = mmex::getProgramName() +
    _(" models all transactions as belonging to accounts.\n\n The next pages will help you create a new account.\n\nTo help you get started, begin by making a list of all\nfinancial institutions where you hold an account.");

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
                   wxDEFAULT_DIALOG_STYLE), core_(core)
{
    page1 = new wxWizardPageSimple(this);

    new wxStaticText(page1, wxID_ANY,
             _("The next pages will help you create a new database.\n\nYour database file is stored with an extension of .mmb.\n\nMake sure to make backups of this file \nand to store it carefully as it contains important \nfinancial information."
             )
        );

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
    refreshRequested_(),
    panelCurrent_(),
    homePanel(),
    navTreeCtrl_(),
    menuBar_(),
    toolBar_(),
    selectedItemData_(),
    menuItemOnlineUpdateCurRate_()
{
	// tell wxAuiManager to manage this frame
	m_mgr.SetManagedWindow(this);

    SetIcon(mmex::getProgramIcon());

    /* Setup Printer */
    printer_.reset(new wxHtmlEasyPrinting(mmex::getProgramName(), this));
    wxString printHeaderBase = mmex::getProgramName();
    printer_-> SetHeader( printHeaderBase + wxT("(@PAGENUM@/@PAGESCNT@)<hr>"), wxPAGE_ALL);

	/* Load from Settings DB */
    loadConfigFile();
	
    /* Create the Controls for the frame */
    createMenu();
    createToolBar();
    createControls();


	// add the toolbars to the manager
	m_mgr.AddPane(toolBar_, wxAuiPaneInfo().
		Name(wxT("toolbar")).Caption(wxT("Toolbar")).ToolbarPane().Top().
		LeftDockable(false).RightDockable(false));

    // change look and feel of wxAuiManager
    m_mgr.GetArtProvider()->SetMetric(16, 0);
    m_mgr.GetArtProvider()->SetMetric(3, 1);

	// Save default perspective
	m_perspective = m_mgr.SavePerspective();
	
    wxString auiPerspective = mmDBWrapper::getINISettingValue(inidb_.get(), 
        wxT("AUIPERSPECTIVE"), m_perspective);

    m_mgr.LoadPerspective(auiPerspective);

	// "commit" all changes made to wxAuiManager
	m_mgr.Update();

    // enable or disable online update currency rate
    wxString enableCurrencyUpd = mmDBWrapper::getINISettingValue(inidb_.get(), wxT("UPDATECURRENCYRATE"), wxT("FALSE"));
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
    wxString showBeginApp = mmDBWrapper::getINISettingValue(inidb_.get(), wxT("SHOWBEGINAPP"), wxT("TRUE"));
    bool from_scratch = showBeginApp == wxT("TRUE");

    wxFileName dbpath = from_scratch ? wxGetEmptyString() : mmDBWrapper::getLastDbPath(inidb_.get());
 

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
    saveConfigFile();

    m_mgr.UnInit();

    /* Delete the GUI */
    cleanupHomePanel(false);

    if (core_) {
        core_.reset();
    }

    if (db_) {
        db_->Close();
    }

    if (inidb_) {
        inidb_->Close();
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::unselectNavTree()
{ 
    wxASSERT(navTreeCtrl_);
    navTreeCtrl_->Unselect(); 
}
//----------------------------------------------------------------------------

/* 
        Save our settings to ini db.
*/
void mmGUIFrame::saveConfigFile()
{
    wxFileName fname(fileName_);
    mmDBWrapper::setLastDbPath(inidb_.get(), fname.GetFullPath());

    mmSaveColorsToDatabase(inidb_.get());

    /* Aui Settings */
    m_perspective = m_mgr.SavePerspective();
    mmDBWrapper::setINISettingValue(inidb_.get(), wxT("AUIPERSPECTIVE"), m_perspective);

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

    mmDBWrapper::setINISettingValue(inidb_.get(), wxT("ORIGINX"), valxs); 
    mmDBWrapper::setINISettingValue(inidb_.get(), wxT("ORIGINY"), valys);
    mmDBWrapper::setINISettingValue(inidb_.get(), wxT("SIZEW"), valws);
    mmDBWrapper::setINISettingValue(inidb_.get(), wxT("SIZEH"), valhs);
    mmDBWrapper::setINISettingValue(inidb_.get(), wxT("ISMAXIMIZED"), this->IsMaximized() ? wxT("TRUE") : wxT("FALSE"));
}
//----------------------------------------------------------------------------

void mmGUIFrame::loadConfigFile()
{
    inidb_.reset(new wxSQLite3Database);
    inidb_->Open(mmex::getPathUser(mmex::SETTINGS));
}
//----------------------------------------------------------------------------

void mmGUIFrame::menuEnableItems(bool enable)
{
    menuBar_->FindItem(MENU_SAVE_AS)->Enable(enable);
    menuBar_->FindItem(MENU_EXPORT)->Enable(enable);
    if (mmIniOptions::enableAddAccount_)
      menuBar_->FindItem(MENU_NEWACCT)->Enable(enable);
    menuBar_->FindItem(MENU_ACCTLIST)->Enable(enable);
    menuBar_->FindItem(MENU_ACCTEDIT)->Enable(enable);
    if (mmIniOptions::enableDeleteAccount_)
        menuBar_->FindItem(MENU_ACCTDELETE)->Enable(enable);
    menuBar_->FindItem(MENU_ORGCATEGS)->Enable(enable);
    menuBar_->FindItem(MENU_ORGPAYEE)->Enable(enable);
    menuBar_->FindItem(MENU_IMPORT)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT_PREVIEW)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT)->Enable(enable);
    menuBar_->FindItem(wxID_PREFERENCES)->Enable(enable);
    if (mmIniOptions::enableRepeatingTransactions_)
      menuBar_->FindItem(MENU_BILLSDEPOSITS)->Enable(enable);
    if (mmIniOptions::enableStocks_)
      menuBar_->FindItem(MENU_STOCKS)->Enable(enable);
    menuBar_->FindItem(MENU_CURRENCY)->Enable(enable);
    if (mmIniOptions::enableAssets_)
      menuBar_->FindItem(MENU_ASSETS)->Enable(enable);
    if (mmIniOptions::enableBudget_)
      menuBar_->FindItem(MENU_BUDGETSETUPDIALOG)->Enable(enable);
    menuBar_->FindItem(MENU_TRANSACTIONREPORT)->Enable(enable);
    
    if (mmIniOptions::enableAddAccount_)
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
    wxImageList* imageList_ = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    imageList_->Add(wxBitmap(house_xpm));
    imageList_->Add(wxBitmap(moneyaccount_xpm));
    imageList_->Add(wxBitmap(clock_xpm));
    imageList_->Add(wxBitmap(calendar_xpm));
    imageList_->Add(wxBitmap(chartpiereport_xpm));
    imageList_->Add(wxBitmap(help_xpm));
    imageList_->Add(wxBitmap(stock_curve_xpm));
    imageList_->Add(wxBitmap(car_xpm));

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

void mmGUIFrame::updateNavTreeControl()
{
    navTreeCtrl_->DeleteAllItems();
    navTreeCtrl_->SetBackgroundColour(mmColors::navTreeBkColor);

    wxTreeItemId root = navTreeCtrl_->AddRoot(_("Home Page"), 0, 0);
    navTreeCtrl_->SetItemData(root, new mmTreeItemData(wxT("Home Page")));
    navTreeCtrl_->SetItemBold(root, true);

    wxTreeItemId accounts = navTreeCtrl_->AppendItem(root, _("Bank Accounts"), 1, 1);
    navTreeCtrl_->SetItemData(accounts, new mmTreeItemData(wxT("Bank Accounts")));
    navTreeCtrl_->SetItemBold(accounts, true);

    wxTreeItemId stocks;
    if (mmIniOptions::enableStocks_)
    {
        stocks = navTreeCtrl_->AppendItem(root, _("Stocks"), 6, 6);
        navTreeCtrl_->SetItemData(stocks, new mmTreeItemData(wxT("Stocks")));
        navTreeCtrl_->SetItemBold(stocks, true);
    }

    if (mmIniOptions::enableAssets_)
    {
        wxTreeItemId assets = navTreeCtrl_->AppendItem(root, _("Assets"), 7, 7);
        navTreeCtrl_->SetItemData(assets, new mmTreeItemData(wxT("Assets")));
        navTreeCtrl_->SetItemBold(assets, true);
    }

    if (mmIniOptions::enableRepeatingTransactions_)
    {
       wxTreeItemId bills = navTreeCtrl_->AppendItem(root, _("Repeating Transactions"), 2, 2);
       navTreeCtrl_->SetItemData(bills, new mmTreeItemData(wxT("Bills & Deposits")));
       navTreeCtrl_->SetItemBold(bills, true);
    }

    wxTreeItemId budgeting;
    if (mmIniOptions::enableBudget_)
    {
        budgeting = navTreeCtrl_->AppendItem(root, _("Budgeting"), 3, 3);
        navTreeCtrl_->SetItemData(budgeting, new mmTreeItemData(wxT("Budgeting")));
        navTreeCtrl_->SetItemBold(budgeting, true);
    }

    wxTreeItemId reports = navTreeCtrl_->AppendItem(root, 
        _("Reports"), 4, 4);
    navTreeCtrl_->SetItemBold(reports, true);
    navTreeCtrl_->SetItemData(reports, 
        new mmTreeItemData(wxT("Reports")));

    wxTreeItemId reportsSummary = navTreeCtrl_->AppendItem(reports, 
        _("Summary of Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(reportsSummary, 
        new mmTreeItemData(wxT("Summary of Accounts")));

    if (mmIniOptions::enableStocks_)
    {
        wxTreeItemId reportsStocks = navTreeCtrl_->AppendItem(reportsSummary, 
            _("Stocks"), 4, 4);
        navTreeCtrl_->SetItemData(reportsStocks, 
            new mmTreeItemData(wxT("Summary of Stocks")));
    }

    if (mmIniOptions::enableAssets_)
    {
        wxTreeItemId reportsAssets = navTreeCtrl_->AppendItem(reportsSummary, 
            _("Assets"), 4, 4);
        navTreeCtrl_->SetItemData(reportsAssets, 
            new mmTreeItemData(wxT("Summary of Assets")));
    }

    wxTreeItemId categsOverTime = navTreeCtrl_->AppendItem(reports, 
        _("Where the Money Goes"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTime, 
        new mmTreeItemData(wxT("Where the Money Goes")));
    
    wxTreeItemId categsOverTimeCalMonth = navTreeCtrl_->AppendItem(categsOverTime, 
        _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCalMonth, 
        new mmTreeItemData(wxT("Where the Money Goes - Month")));

    wxTreeItemId categsOverTimeCurrentMonth = navTreeCtrl_->AppendItem(categsOverTime, 
        _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentMonth, 
        new mmTreeItemData(wxT("Where the Money Goes - Current Month")));

    wxTreeItemId categsOverTimeLast30 = navTreeCtrl_->AppendItem(categsOverTime, 
        _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLast30, 
        new mmTreeItemData(wxT("Where the Money Goes - 30 Days")));
    
    wxTreeItemId categsOverTimeLastYear = navTreeCtrl_->AppendItem(categsOverTime, 
        _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeLastYear, 
        new mmTreeItemData(wxT("Where the Money Goes - Last Year")));

    wxTreeItemId categsOverTimeCurrentYear = navTreeCtrl_->AppendItem(categsOverTime,
        _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsOverTimeCurrentYear, 
        new mmTreeItemData(wxT("Where the Money Goes - Current Year")));

    ///////////////////////////////////////////////////////////

    wxTreeItemId posCategs = navTreeCtrl_->AppendItem(reports, 
        _("Where the Money Comes From"), 4, 4);
    navTreeCtrl_->SetItemData(posCategs, 
        new mmTreeItemData(wxT("Where the Money Comes From")));

    wxTreeItemId posCategsCalMonth = navTreeCtrl_->AppendItem(posCategs, 
        _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsCalMonth, 
        new mmTreeItemData(wxT("Where the Money Comes From - Month")));

    wxTreeItemId posCategsCurrentMonth = navTreeCtrl_->AppendItem(posCategs, 
        _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsCurrentMonth, 
        new mmTreeItemData(wxT("Where the Money Comes From - Current Month")));

    wxTreeItemId posCategsTimeLast30 = navTreeCtrl_->AppendItem(posCategs, 
        _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLast30, 
        new mmTreeItemData(wxT("Where the Money Comes From - 30 Days")));
    
    wxTreeItemId posCategsTimeLastYear = navTreeCtrl_->AppendItem(posCategs, 
        _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeLastYear, 
        new mmTreeItemData(wxT("Where the Money Comes From - Last Year")));

    wxTreeItemId posCategsTimeCurrentYear = navTreeCtrl_->AppendItem(posCategs,
        _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(posCategsTimeCurrentYear, 
        new mmTreeItemData(wxT("Where the Money Comes From - Current Year")));
 
    ///////////////////////////////////////////////////////////

    wxTreeItemId categs = navTreeCtrl_->AppendItem(reports, 
        _("Categories"), 4, 4);
    navTreeCtrl_->SetItemData(categs, 
        new mmTreeItemData(wxT("Categories - Over Time")));
    
    wxTreeItemId categsCalMonth = navTreeCtrl_->AppendItem(categs, 
        _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsCalMonth, 
        new mmTreeItemData(wxT("Categories - Month")));

    wxTreeItemId categsCurrentMonth = navTreeCtrl_->AppendItem(categs, 
        _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(categsCurrentMonth, 
        new mmTreeItemData(wxT("Categories - Current Month")));

    wxTreeItemId categsTimeLast30 = navTreeCtrl_->AppendItem(categs, 
        _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLast30, 
        new mmTreeItemData(wxT("Categories - 30 Days")));
    
    wxTreeItemId categsTimeLastYear = navTreeCtrl_->AppendItem(categs, 
        _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeLastYear, 
        new mmTreeItemData(wxT("Categories - Last Year")));

    wxTreeItemId categsTimeCurrentYear = navTreeCtrl_->AppendItem(categs,
        _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(categsTimeCurrentYear, 
        new mmTreeItemData(wxT("Categories - Current Year")));
    ///////////////////////////////////////////////////////////

     wxTreeItemId payeesOverTime = navTreeCtrl_->AppendItem(reports, 
        _("To Whom the Money Goes"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTime, 
        new mmTreeItemData(wxT("To Whom the Money Goes")));

    wxTreeItemId payeesOverTimeCalMonth = navTreeCtrl_->AppendItem(payeesOverTime, 
        _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCalMonth, 
        new mmTreeItemData(wxT("To Whom the Money Goes - Month")));

    wxTreeItemId payeesOverTimeCurrentMonth = navTreeCtrl_->AppendItem(payeesOverTime, 
        _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentMonth, 
        new mmTreeItemData(wxT("To Whom the Money Goes - Current Month")));

    wxTreeItemId payeesOverTimeLast30 = navTreeCtrl_->AppendItem(payeesOverTime, 
        _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLast30, 
        new mmTreeItemData(wxT("To Whom the Money Goes - 30 Days")));
    
    wxTreeItemId payeesOverTimeLastYear = navTreeCtrl_->AppendItem(payeesOverTime, 
        _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeLastYear, 
        new mmTreeItemData(wxT("To Whom the Money Goes - Last Year")));

    wxTreeItemId payeesOverTimeCurrentYear = navTreeCtrl_->AppendItem(payeesOverTime,
        _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(payeesOverTimeCurrentYear, 
        new mmTreeItemData(wxT("To Whom the Money Goes - Current Year")));

    ///////////////////////////////////////////////////////////////////

     wxTreeItemId incexpOverTime = navTreeCtrl_->AppendItem(reports, 
        _("Income vs Expenses"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTime, 
        new mmTreeItemData(wxT("Income vs Expenses")));

    wxTreeItemId incexpOverTimeCalMonth = navTreeCtrl_->AppendItem(incexpOverTime, 
        _("Last Calendar Month"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCalMonth, 
        new mmTreeItemData(wxT("Income vs Expenses - Month")));

    wxTreeItemId incexpOverTimeCurrentMonth = navTreeCtrl_->AppendItem(incexpOverTime, 
        _("Current Month"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentMonth, 
        new mmTreeItemData(wxT("Income vs Expenses - Current Month")));

    wxTreeItemId incexpOverTimeLast30 = navTreeCtrl_->AppendItem(incexpOverTime, 
        _("Last 30 Days"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLast30, 
        new mmTreeItemData(wxT("Income vs Expenses - 30 Days")));
    
    wxTreeItemId incexpOverTimeLastYear = navTreeCtrl_->AppendItem(incexpOverTime, 
        _("Last Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeLastYear, 
        new mmTreeItemData(wxT("Income vs Expenses - Last Year")));

    wxTreeItemId incexpOverTimeCurrentYear = navTreeCtrl_->AppendItem(incexpOverTime,
        _("Current Year"), 4, 4);
    navTreeCtrl_->SetItemData(incexpOverTimeCurrentYear, 
        new mmTreeItemData(wxT("Income vs Expenses - Current Year")));

    wxTreeItemId incexpmonthly = navTreeCtrl_->AppendItem(incexpOverTime, 
        _("Income vs Expenses - All Time"), 4, 4);
    navTreeCtrl_->SetItemData(incexpmonthly, 
        new mmTreeItemData(wxT("Income vs Expenses - All Time")));

    //////////////////////////////////////////////////////////////////
   wxTreeItemId transactionList = navTreeCtrl_->AppendItem(reports, 
        _("Transaction Report"), 4, 4);
    navTreeCtrl_->SetItemData(transactionList, 
        new mmTreeItemData(wxT("Transaction Report")));
    ///////////////////////////////////////////////////////////////////
    
    if (db_ && mmIniOptions::enableBudget_)
    {
        static const char sql[] =
        "select BUDGETYEARID, BUDGETYEARNAME "
        "from BUDGETYEAR_V1 "
        "order by BUDGETYEARNAME";
        
        wxTreeItemId budgetPerformance;
        wxTreeItemId budgetSetupPerformance;

        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

        for (size_t i = 0; q1.NextRow(); ++i)
        {
            if (!i) { // first loop only
                budgetPerformance = navTreeCtrl_->AppendItem(reports, _("Budget Performance"), 4, 4);
                navTreeCtrl_->SetItemData(budgetPerformance, new mmTreeItemData(wxT("Budget Performance")));

                budgetSetupPerformance = navTreeCtrl_->AppendItem(reports, _("Budget Setup and Performance"), 4, 4);
                navTreeCtrl_->SetItemData(budgetSetupPerformance, new mmTreeItemData(wxT("Budget Setup Performance")));
            }

            int id = q1.GetInt(wxT("BUDGETYEARID"));
            const wxString name = q1.GetString(wxT("BUDGETYEARNAME"));
            
            wxTreeItemId bYear = navTreeCtrl_->AppendItem(budgeting, name, 3, 3);
            navTreeCtrl_->SetItemData(bYear, new mmTreeItemData(id, true));
            
            wxTreeItemId bYearData = navTreeCtrl_->AppendItem(budgetPerformance, name, 4, 4);
            navTreeCtrl_->SetItemData(bYearData, new mmTreeItemData(id, true));
            
            wxTreeItemId bYearSetupData = navTreeCtrl_->AppendItem(budgetSetupPerformance, name, 4, 4);
            navTreeCtrl_->SetItemData(bYearSetupData, new mmTreeItemData(id, true));
        }

        q1.Finalize();
        
        navTreeCtrl_->Expand(budgeting);
    }
		
	///////////////////////////////////////////////////////////////////
    wxTreeItemId cashFlow = navTreeCtrl_->AppendItem(reports, _("Cash Flow"), 4, 4);
    navTreeCtrl_->SetItemData(cashFlow, new mmTreeItemData(wxT("Cash Flow")));

    wxTreeItemId cashflowSpecificAccounts = navTreeCtrl_->AppendItem(cashFlow, 
        _("Cash Flow - Specific Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(cashflowSpecificAccounts, 
        new mmTreeItemData(wxT("Cash Flow - Specific Accounts")));

    ///////////////////////////////////////////////////////
    wxTreeItemId transactionStats = navTreeCtrl_->AppendItem(reports, 
        _("Transaction Statistics"), 4, 4);
    navTreeCtrl_->SetItemData(transactionStats, 
        new mmTreeItemData(wxT("Transaction Statistics")));

     ///////////////////////////////////////////////////////////////////

    wxTreeItemId help = navTreeCtrl_->AppendItem(root, _("Help"), 5, 5);
    navTreeCtrl_->SetItemData(help, new mmTreeItemData(wxT("Help")));
    navTreeCtrl_->SetItemBold(help, true);

     /* Start Populating the dynamic data */
    navTreeCtrl_->Expand(root);
    navTreeCtrl_->Expand(reports);

    if (!db_)
       return;

    /* Load Nav Tree Control */

    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

    int numAccounts = (int) core_->accountList_.accounts_.size();
    for (int iAdx = 0; iAdx < numAccounts; iAdx++)
    {
        mmCheckingAccount* pCA = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
        if (pCA)
        {
            if ((vAccts == wxT("Open") && pCA->status_ == mmAccount::MMEX_Open) ||
                (vAccts == wxT("Favorites") && pCA->favoriteAcct_) ||
                (vAccts == wxT("ALL")))
            {
                wxTreeItemId tacct = navTreeCtrl_->AppendItem(accounts, pCA->accountName_, 1, 1);
                navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(pCA->accountID_, false));
            }
        }

        if (mmIniOptions::enableStocks_)
        {
            mmInvestmentAccount* pIA = dynamic_cast<mmInvestmentAccount*>(core_->accountList_.accounts_[iAdx].get());
            if (pIA)
            {
                wxTreeItemId tacct = navTreeCtrl_->AppendItem(stocks, pIA->accountName_, 6, 6);
                navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(pIA->accountID_, false));
            }
        }
    }

    navTreeCtrl_->Expand(accounts);
}
//----------------------------------------------------------------------------

wxString mmGUIFrame::createCategoryList()
{
    if (!db_)
        return wxGetEmptyString();

    mmHTMLBuilder hb;

    std::vector<CategInfo> categList;
	hb.startTable(wxT("95%"));
	hb.addTableHeaderRow(_("Top Categories Last 30 Days"), 2);

    core_->currencyList_.loadBaseCurrencySettings();

    static const char sql[] = 
    "select SUBCATEGID, SUBCATEGNAME "
    "from SUBCATEGORY_V1 "
    "where CATEGID = ?";

    wxSQLite3Statement st = db_->PrepareStatement(sql);

    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select CATEGID, CATEGNAME "
                                              "from CATEGORY_V1 "
                                              "order by CATEGNAME"
                                             );

    while (q1.NextRow())
    {
        wxDateTime today = wxDateTime::Now();
        wxDateTime prevMonthEnd = today;
        wxDateTime dtEnd = today;
        wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());

        int categID          = q1.GetInt(wxT("CATEGID"));
        wxString categString = q1.GetString(wxT("CATEGNAME"));
        wxString balance;
        double amt = core_->bTransactionList_.getAmountForCategory(categID, -1, false, 
            dtBegin, dtEnd);
        mmex::formatDoubleToCurrency(amt, balance);

        if (amt != 0.0)
        {
            CategInfo info;
            info.categ = categString;
            info.amountStr = balance;
            info.amount = amt;
            categList.push_back(info);
        }
        
        // --

        st.Bind(1, categID);
        wxSQLite3ResultSet q2 = st.ExecuteQuery(); 

        while(q2.NextRow())
        {
            int subcategID = q2.GetInt(wxT("SUBCATEGID"));
            wxString subcategString = q2.GetString(wxT("SUBCATEGNAME"));

            amt = core_->bTransactionList_.getAmountForCategory(categID, subcategID, 
                false,  dtBegin, dtEnd);
            mmex::formatDoubleToCurrency(amt, balance);

            if (amt != 0.0)
            {
                CategInfo infoSC;
                infoSC.categ = categString + wxT(" : ") + subcategString;
                infoSC.amountStr = balance;
                infoSC.amount = amt;
                categList.push_back(infoSC);
            }
        }
        
        q2.Finalize();
        st.Reset();

    }
    q1.Finalize();

    std::sort(categList.begin(), categList.end(), sortCategs);

    for (size_t i = 0; i < std::min(categList.size(), size_t(10)); ++i) {

        if (categList[i].amount < 0) {
		hb.startTableRow();
		hb.addTableCell(categList[i].categ, false, true);
		hb.addTableCell(categList[i].amountStr, true);
		hb.endTableRow();
        }
    }

    hb.endTable();

    mmGraphTopCategories gtp;
    gtp.init(categList);
    gtp.Generate(_("Top Categories Last 30 Days"));

    return hb.getHTMLText();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnSelChanged(wxTreeEvent& event)
{
    menuPrintingEnable(false);
    wxTreeItemId id = event.GetItem();
    if (!id)
        return;
    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(id));
    selectedItemData_ = iData;
    
    if (!iData)
        return;

    if (!iData->isStringData())
    {
        int data = iData->getData();
        if (iData->isBudgetingNode())
        {
            Freeze();

            wxTreeItemId idparent = navTreeCtrl_->GetItemParent(id);
            mmTreeItemData* iParentData = dynamic_cast<mmTreeItemData*>(navTreeCtrl_->GetItemData(idparent));
            if (iParentData->getString() == wxT("Budget Performance"))
            {
                mmPrintableBase* rs = new mmReportBudgetingPerformance(core_.get(), data);
                menuPrintingEnable(true);
                createReportsPage(rs);
            }
            else if (iParentData->getString() == wxT("Budget Setup Performance"))
            {
                mmPrintableBase* rs = new mmReportBudgetingSetup(core_.get(), data);
                menuPrintingEnable(true);
                createReportsPage(rs);
            }
            else
                createBudgetingPage(data);

            Thaw();
        }
        else
        {
           boost::shared_ptr<mmAccount> pAccount = core_->accountList_.getAccountSharedPtr(data); 
           if (pAccount)
           {
              wxString acctType = pAccount->acctType_;
              if (acctType == wxT("Checking"))
              {
                 gotoAccountID_ = data;
                 wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_GOTOACCOUNT);
                 GetEventHandler()->AddPendingEvent(evt);
              }
              else
              {
                 Freeze();
                 wxSizer *sizer = cleanupHomePanel();

                 panelCurrent_ = new mmStocksPanel(db_.get(), inidb_.get(), data, homePanel, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
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
            if (!refreshRequested_)
            {
              refreshRequested_ = true;
              createHomePage();
            }
            return;
        }

        if (iData->getString() == wxT("Help"))
        {
            menuPrintingEnable(true);
            createHelpPage();
            return;
        }

        if (!core_ || !db_)
            return;

        //Freeze();

        if (iData->getString() == wxT("Summary of Accounts"))
        {
            mmPrintableBase* rs = new mmReportSummary(core_.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Summary of Stocks"))
        {
            mmPrintableBase* rs = new mmReportSummaryStocks(db_.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Summary of Assets"))
        {
            mmPrintableBase* rs = new mmReportSummaryAssets(db_.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Categories - Over Time"))
        {
            mmPrintableBase* rs = new mmReportCategoryOverTimePerformance(core_.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        /////////////////////////////////////////////////////
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
            wxString title = _("Categories");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
                title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Categories - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            wxString title = _("Categories");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd,
                title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Categories - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            wxString title = _("Categories");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd,
               title, 0);
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
            wxString title = _("Categories");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
               title, 0);
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
            wxString title = _("Categories");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
                title, 0);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        //////////////////////////////////////////////////
        else if (iData->getString() == wxT("Where the Money Comes From"))
        {
            wxDateTime dtEnd =  wxDateTime::Now();
            wxDateTime dtBegin =  wxDateTime::Now();
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), true, dtBegin, dtEnd, 
                title, 1);
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
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
                title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Where the Money Comes From - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd,
                title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Where the Money Comes From - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd,
               title, 1);
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
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
               title, 1);
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
            wxString title = _("Where the Money Comes From");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
                title, 1);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        //////////////////////////////////////////////////
        else if (iData->getString() == wxT("Where the Money Goes"))
        {
            wxDateTime dtEnd = wxDateTime::Now();
            wxDateTime dtBegin = wxDateTime::Now();
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), true, dtBegin, dtEnd, 
                title, 2);
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
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
                title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Where the Money Goes - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd,
                title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Where the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd,
               title, 2);
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
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
               title, 2);
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
            wxString title = _("Where the Money Goes");
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_.get(), false, dtBegin, dtEnd, 
                title, 2);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        /////////////////////////////////////////////////////////////////
        
        else if (iData->getString() == wxT("Transaction Statistics"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportTransactionStats(core_.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        

        /////////////////////////////////////////////////////////////////

        else if (iData->getString() == wxT("Income vs Expenses"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(core_.get(), year);
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

            mmPrintableBase* rs = new mmReportIncomeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Income vs Expenses - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportIncomeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Income vs Expenses - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportIncomeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Income vs Expenses - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(core_.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Income vs Expenses - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(core_.get(), year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Income vs Expenses - All Time"))
        {
            mmPrintableBase* rs = new mmReportIncomeExpenses(core_.get(), true, wxDateTime::Now(),
                wxDateTime::Now());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        /////////////////////////////////////////////////////////////////

        else if (iData->getString() == wxT("To Whom the Money Goes"))
        {
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_.get(), true, wxDateTime::Now(),
                wxDateTime::Now());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("To Whom the Money Goes - Month"))
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

            mmPrintableBase* rs = new mmReportPayeeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("To Whom the Money Goes - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("To Whom the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("To Whom the Money Goes - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime prevYearEnd = wxDateTime(today);
            prevYearEnd.SetYear(year);
            prevYearEnd.SetMonth(wxDateTime::Dec);
            prevYearEnd.SetDay(31);
            wxDateTime dtEnd = prevYearEnd;
            wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("To Whom the Money Goes - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime yearBegin = wxDateTime(today);
            yearBegin.SetYear(year);
            yearBegin.SetMonth(wxDateTime::Dec);
            yearBegin.SetDay(31);
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = yearBegin;
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_.get(), false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        else if (iData->getString() == wxT("Cash Flow"))
        {
            mmPrintableBase* rs = new mmReportCashFlow(core_.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        
       // Thaw();

        else if (iData->getString() == wxT("Cash Flow - Specific Accounts"))
        {
            OnCashFlowSpecificAccounts();
        }

         ///////////////////////////////////////////////
        else if (iData->getString() == wxT("Transaction Report"))
        {
           wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TRANSACTIONREPORT);
           AddPendingEvent(evt);
        }
        
        //////////////////////////////////////////////

        else if (iData->getString() == wxT("Budgeting"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BUDGETSETUPDIALOG);
            AddPendingEvent(evt);
        }
            
        else if (iData->getString() == wxT("Bills & Deposits"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BILLSDEPOSITS);
            AddPendingEvent(evt);
        }

        else if (iData->getString() == wxT("Stocks"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_STOCKS);
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
      boost::shared_ptr<mmAccount> pAccount = core_->accountList_.getAccountSharedPtr(data); 
      if (pAccount)
      {
         wxString website = pAccount->website_;
         if (!website.IsEmpty())
         {
            wxLaunchDefaultBrowser(website);  
         }
         return;
      }
      else
      {
         /* cannot find accountid */
         wxASSERT(true);
      }
   }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPopupEditAccount(wxCommandEvent& /*event*/)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        boost::shared_ptr<mmAccount> pAccount = core_->accountList_.getAccountSharedPtr(data); 
        if (pAccount)
        {
           wxString acctType = pAccount->acctType_;
           if (acctType == wxT("Checking") || acctType == wxT("Investment"))
           {
              mmNewAcctDialog *dlg = new mmNewAcctDialog(core_.get(), false, data, this);
              if ( dlg->ShowModal() == wxID_OK )
              {
                 createHomePage();
                 updateNavTreeControl();      
              }
              dlg->Destroy();
           }
        }
        else
        {
            /* cannot find accountid */
            wxASSERT(true);
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPopupDeleteAccount(wxCommandEvent& /*event*/)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        boost::shared_ptr<mmAccount> pAccount = core_->accountList_.getAccountSharedPtr(data); 
        if (pAccount)
        {
           wxMessageDialog msgDlg(this, 
              _("Do you really want to delete the account?"),
              _("Confirm Account Deletion"),
              wxYES_NO);
           if (msgDlg.ShowModal() == wxID_YES)
           {
              core_->accountList_.deleteAccount(pAccount->accountID_);
              core_->bTransactionList_.deleteTransactions(pAccount->accountID_);  
              updateNavTreeControl();
              if (!refreshRequested_)
              {
                  refreshRequested_ = true;
                  wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
                  GetEventHandler()->AddPendingEvent(ev); 
              }
           } 
        }
        else
        {
            /* cannot find accountid */
            wxASSERT(true);
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnItemRightClick(wxTreeEvent& event)
{
    showTreePopupMenu(event.GetItem(), event.GetPoint());
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
           boost::shared_ptr<mmAccount> pAccount = core_->accountList_.getAccountSharedPtr(data); 
           if (pAccount)
           {
              wxString acctType = pAccount->acctType_;
              if (acctType == wxT("Checking"))
              {
                 wxMenu menu;
                 menu.Append(MENU_TREEPOPUP_GOTO, _("&Go To.."));
                 menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Account"));
                 menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Account"));
                 menu.AppendSeparator();
                 menu.Append(MENU_TREEPOPUP_LAUNCHWEBSITE, _("&Launch Account Website"));
                 PopupMenu(&menu, pt);
              }
              else if (acctType == wxT("Investment"))
              {
                 wxMenu menu;
                 menu.Append(MENU_TREEPOPUP_EDIT, _("&Edit Account"));
                 menu.Append(MENU_TREEPOPUP_DELETE, _("&Delete Account"));
                 menu.AppendSeparator();
                 menu.Append(MENU_TREEPOPUP_LAUNCHWEBSITE, _("&Launch Account Website"));
                 PopupMenu(&menu, pt);
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
       if (iData->getString() == wxT("Bank Accounts"))
		{

         //wxMenu menu;
			/*Popup Menu for Bank Accounts*/
		//New Account    //
		//Delete Account //
		//Edit Account   //
		//Export >       //  
	        //CSV Files //
	        //QIF Files //
		//Import >       //
			//Universal CSV Files //
			//QIF Files           //
			//MMEX CSV Files      //
			//MM.&NET CSV Files   //
		//Accounts Visible//
			//All      //
			//Favorite //
			//Open     //


		 wxMenu *menu = new wxMenu;
         menu->Append(MENU_TREEPOPUP_ACCOUNT_NEW, _("New &Account"));
         menu->Append(MENU_TREEPOPUP_ACCOUNT_DELETE, _("&Delete Account"));
         menu->Append(MENU_TREEPOPUP_ACCOUNT_EDIT, _("&Edit Account"));
         menu->Append(MENU_TREEPOPUP_ACCOUNT_LIST, _("Account &List"));
         
		 menu->AppendSeparator();
		// menu->Append(menuItemOnlineUpdateCurRate_);
		// menu->AppendSeparator();

		 wxMenu *exportTo = new wxMenu;
		 exportTo->Append(MENU_TREEPOPUP_ACCOUNT_EXPORT2CSV, _("&CSV Files"));
		 exportTo->Append(MENU_TREEPOPUP_ACCOUNT_EXPORT2QIF, _("&QIF Files"));
		 menu->AppendSubMenu(exportTo,  _("&Export"));
		 wxMenu *importFrom = new wxMenu;
		 importFrom->Append(MENU_TREEPOPUP_ACCOUNT_IMPORTUNIVCSV, _("&Universal CSV Files"));
		 importFrom->Append(MENU_TREEPOPUP_ACCOUNT_IMPORTQIF, _("&QIF Files"));
		 importFrom->Append(MENU_TREEPOPUP_ACCOUNT_IMPORTCSV, _("&MMEX CSV Files"));
		 importFrom->Append(MENU_TREEPOPUP_ACCOUNT_IMPORTMMNET, _("MM.&NET CSV Files"));
		 menu->AppendSubMenu(importFrom,  _("&Import"));
		 menu->AppendSeparator();
		 wxMenu *viewAccounts = new wxMenu;
		 viewAccounts->Append(MENU_TREEPOPUP_ACCOUNT_VIEWALL, _("All"));
		 viewAccounts->Append(MENU_TREEPOPUP_ACCOUNT_VIEWOPEN, _("Open"));
		 viewAccounts->Append(MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE, _("Favorites"));
		 menu->AppendSubMenu(viewAccounts, _("Accounts Visible"));
         PopupMenu(&*menu, pt);
		}
	}
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewAllAccounts(wxCommandEvent&)
{
	//Get current settings for view accounts
	wxString vAccts = mmDBWrapper::getINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

	//Set view ALL 
	mmDBWrapper::setINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

	//Refresh Navigation Panel
	mmGUIFrame::updateNavTreeControl();

	//Restore settings
	mmDBWrapper::setINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), vAccts);

}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewFavoriteAccounts(wxCommandEvent&)
{
	//Get current settings for view accounts
	wxString vAccts = mmDBWrapper::getINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

	//Set view ALL 
	mmDBWrapper::setINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), wxT("Favorites"));

	//Refresh Navigation Panel
	mmGUIFrame::updateNavTreeControl();

	//Restore settings
	mmDBWrapper::setINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), vAccts);

}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewOpenAccounts(wxCommandEvent&)
{
	//Get current settings for view accounts
	wxString vAccts = mmDBWrapper::getINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), wxT("ALL"));

	//Set view ALL 
	mmDBWrapper::setINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), wxT("Open"));

	//Refresh Navigation Panel
	mmGUIFrame::updateNavTreeControl();

	//Restore settings
	mmDBWrapper::setINISettingValue(inidb_.get(), wxT("VIEWACCOUNTS"), vAccts);

}
//----------------------------------------------------------------------------

void mmGUIFrame::createCheckingAccountPage(int accountID)
{
    wxSizer *sizer = cleanupHomePanel();
    
    panelCurrent_ = new mmCheckingPanel(core_.get(), inidb_.get(),  
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
    {
        createCheckingAccountPage(gotoAccountID_);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::createBudgetingPage(int budgetYearID)
{
    wxSizer *sizer = cleanupHomePanel();

    panelCurrent_ = new mmBudgetingPanel(db_.get(), inidb_.get(), budgetYearID, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::createHomePage()
{
    wxSizer *sizer = cleanupHomePanel();
    
    if (m_topCategories.empty()) {
       m_topCategories = createCategoryList();
    }

    if (panelCurrent_)
    {
        //panelCurrent_->DestroyChildren();
        //panelCurrent_->SetSizer(NULL);
        panelCurrent_  = 0;
    }
    panelCurrent_ = new mmHomePagePanel(this, 
        db_.get(), 
        inidb_.get(),
        core_.get(), 
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
       
    panelCurrent_ = new mmReportsPanel(this, db_.get(), rs, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
     
    homePanel->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::createHelpPage()
{
    wxSizer *sizer = cleanupHomePanel();
       
    panelCurrent_ = new mmHelpPanel(this, db_.get(), homePanel, ID_PANEL3, 
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

   wxMenuItem* menuItemNew = new wxMenuItem(menu_file, MENU_NEW, 
      _("&New Database\tCtrl-N"), 
      _("New Database"));
   menuItemNew->SetBitmap(toolBarBitmaps[0]);

   wxMenuItem* menuItemOpen = new wxMenuItem(menu_file, MENU_OPEN, 
      _("&Open Database\tCtrl-O"), 
      _("Open Database"));
   menuItemOpen->SetBitmap(toolBarBitmaps[1]);

   menu_file->Append(menuItemNew);
   menu_file->Append(menuItemOpen);

   wxMenuItem* menuItemSaveAs = new wxMenuItem(menu_file, MENU_SAVE_AS, 
      _("Save Database &As"), 
      _("Save Database As"));
   menuItemSaveAs->SetBitmap(wxBitmap(saveas_xpm));
   menu_file->Append(menuItemSaveAs);

	menu_file->AppendSeparator();

   wxMenu* exportMenu = new wxMenu;
   exportMenu->Append(MENU_EXPORT_CSV, _("&CSV Files"), _("Export to CSV"));
   exportMenu->Append(MENU_EXPORT_QIF, _("&QIF Files"), _("Export to QIF"));
   exportMenu->Append(MENU_EXPORT_HTML, _("&Report to HTML"), _("Export to HTML"));
   menu_file->Append(MENU_EXPORT, _("&Export"), exportMenu);

   wxMenu* importMenu = new wxMenu;
   importMenu->Append(MENU_IMPORT_QIF, _("&QIF Files"), _("Import from QIF"));
   importMenu->Append(MENU_IMPORT_UNIVCSV, _("&Universal CSV Files"), _("Import from any CSV file"));
   if (mmIniOptions::enableImportMMCSV_)
      importMenu->Append(MENU_IMPORT_CSV, _("&MMEX CSV Files"), _("Import from MMEX CSV"));
   if (mmIniOptions::enableImportMMNETCSV_)
      importMenu->Append(MENU_IMPORT_MMNETCSV, _("MM.&NET CSV Files"), _("Import from MM.NET CSV"));
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
   
	wxMenu *menuView = new wxMenu;

	wxMenuItem* menuItemToolbar = new wxMenuItem(menuView, MENU_VIEW_TOOLBAR, 
		_("&Toolbar"), _("Show/Hide the toolbar"), wxITEM_CHECK);
	wxMenuItem* menuItemLinks = new wxMenuItem(menuView, MENU_VIEW_LINKS, 
		_("&Navigation"), _("Show/Hide the Navigation tree control"), wxITEM_CHECK);

	menuView->Append(menuItemToolbar);
	menuView->Append(menuItemLinks);

	wxMenu *menuAccounts = new wxMenu;

   if (mmIniOptions::enableAddAccount_)
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

    if (mmIniOptions::enableDeleteAccount_)
    {
        wxMenuItem* menuItemAcctDelete = new wxMenuItem(menuAccounts, MENU_ACCTDELETE, 
            _("&Delete Account"), _("Delete Account from database"));
        menuItemAcctDelete->SetBitmap(toolBarBitmaps[9]);
        menuAccounts->Append(menuItemAcctDelete); 
    }

    menuAccounts->Append(menuItemAcctList); 
    menuAccounts->Append(menuItemAcctEdit); 
    

    wxMenu *menuTools = new wxMenu;
    
    wxMenuItem* menuItemCateg = new wxMenuItem(menuTools, MENU_ORGCATEGS, 
		  _("Organize &Categories"), _("Organize Categories"));
	menuItemCateg->SetBitmap(wxBitmap(categoryedit_xpm));
    menuTools->Append(menuItemCateg);

    wxMenuItem* menuItemPayee = new wxMenuItem(menuTools, MENU_ORGPAYEE, 
		  _("Organize &Payees"), _("Organize Payees"));
	menuItemPayee->SetBitmap(wxBitmap(user_edit_xpm));
    menuTools->Append(menuItemPayee); 

    wxMenuItem* menuItemCurrency = new wxMenuItem(menuTools, MENU_CURRENCY, 
		 _("Or&ganize Currency"), _("Organize Currency"));
	menuItemCurrency->SetBitmap(wxBitmap(money_dollar_xpm));
    menuTools->Append(menuItemCurrency);

	if (mmIniOptions::enableBudget_)
    {
        wxMenuItem* menuItemBudgeting = new wxMenuItem(menuTools, MENU_BUDGETSETUPDIALOG, 
            _("&Budget Setup"), _("Budget Setup"));
        menuItemBudgeting->SetBitmap(wxBitmap(calendar_xpm));
        menuTools->Append(menuItemBudgeting); 
    }

    if (mmIniOptions::enableRepeatingTransactions_)
    {
       wxMenuItem* menuItemBillsDeposits = new wxMenuItem(menuTools, MENU_BILLSDEPOSITS, 
          _("&Repeating Transactions"), _("Bills && Deposits"));
       menuItemBillsDeposits->SetBitmap(wxBitmap(clock_xpm));
       menuTools->Append(menuItemBillsDeposits); 
    }

    if (mmIniOptions::enableStocks_)
    {
        wxMenuItem* menuItemStocks = new wxMenuItem(menuTools, MENU_STOCKS, 
            _("&Stock Investments"), _("Stock Investments"));
        menuItemStocks->SetBitmap(wxBitmap(stock_curve_xpm));
        menuTools->Append(menuItemStocks);
    }

    if (mmIniOptions::enableAssets_)
    {
        wxMenuItem* menuItemAssets = new wxMenuItem(menuTools, MENU_ASSETS, 
            _("&Assets"), _("Assets"));
        menuItemAssets->SetBitmap(wxBitmap(car_xpm));
        menuTools->Append(menuItemAssets);
    }

    menuTools->AppendSeparator();

    wxMenuItem* menuItemTransactions = new wxMenuItem(menuTools, MENU_TRANSACTIONREPORT, 
		_("&Transaction Filter"), _("Transaction Filter"));
	menuItemTransactions->SetBitmap(wxBitmap(filter_xpm));
    menuTools->Append(menuItemTransactions);

    menuTools->AppendSeparator();

    wxMenuItem* menuItemOptions = new wxMenuItem(menuTools, wxID_PREFERENCES, 
		  _("&Options"), _("Show the Options Dialog"));
	menuItemOptions->SetBitmap(wxBitmap(wrench_xpm));
    menuTools->Append(menuItemOptions);

    wxMenu *menuHelp = new wxMenu;

    wxMenuItem* menuItemHelp = new wxMenuItem(menuTools, wxID_HELP, 
		 _("&Help\tCtrl-F1"), _("Show the Help file"));
	menuItemHelp->SetBitmap(wxBitmap(help_xpm));
    menuHelp->Append(menuItemHelp);

    wxMenuItem* menuItemAppStart = new wxMenuItem(menuTools, MENU_SHOW_APPSTART, 
		 _("&Show App Start Dialog"), _("App Start Dialog"));
	menuItemAppStart->SetBitmap(wxBitmap(appstart_xpm));
    menuHelp->Append(menuItemAppStart);

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

    menuHelp->AppendSeparator();

    if (mmIniOptions::enableCheckForUpdates_)
    {
       wxMenuItem* menuItemCheck = new wxMenuItem(menuTools, MENU_CHECKUPDATE, 
          _("Check for &Updates"), _("Check For Updates"));
       menuItemCheck->SetBitmap(wxBitmap(checkupdate_xpm));
       menuHelp->Append(menuItemCheck);
    }

    if (mmIniOptions::enableReportIssues_)
    {
       wxMenuItem* menuItemReportIssues = new wxMenuItem(menuTools, MENU_REPORTISSUES, 
          _("&Report Issues or Feedback"), _("Send email through the mailing list to report issues with the software."));
       menuItemReportIssues->SetBitmap(wxBitmap(issues_xpm));
       menuHelp->Append(menuItemReportIssues);
    }

    if (mmIniOptions::enableBeNotifiedForNewReleases_)
    {
       wxMenuItem* menuItemNotify = new wxMenuItem(menuTools, MENU_ANNOUNCEMENTMAILING, 
          _("Be &notified of new releases"), _("Sign up for the announcement mailing list"));
       menuItemNotify->SetBitmap(wxBitmap(notify_xpm));
       menuHelp->Append(menuItemNotify); 
    }
        
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
}
//----------------------------------------------------------------------------

void mmGUIFrame::createToolBar()
{
	toolBar_ = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTB_FLAT | wxTB_NODIVIDER);
    wxBitmap toolBarBitmaps[9];
    toolBarBitmaps[0] = wxBitmap(new_xpm);
    toolBarBitmaps[1] = wxBitmap(open_xpm);
    toolBarBitmaps[2] = wxBitmap(save_xpm);
    toolBarBitmaps[3] = wxBitmap(newacct_xpm);
    toolBarBitmaps[4] = wxBitmap(house_xpm);
    toolBarBitmaps[5] = wxBitmap(categoryedit_xpm);
    toolBarBitmaps[6] = wxBitmap(user_edit_xpm);
    toolBarBitmaps[7] = wxBitmap(money_dollar_xpm);
    toolBarBitmaps[8] = wxBitmap(filter_xpm);

    toolBar_->AddTool(MENU_NEW, _("New"), toolBarBitmaps[0], _("New Database"));
    toolBar_->AddTool(MENU_OPEN, _("Open"), toolBarBitmaps[1], _("Open Database"));
    toolBar_->AddSeparator();
    if (mmIniOptions::enableAddAccount_)
    {
      toolBar_->AddTool(MENU_NEWACCT, _("New Account"), toolBarBitmaps[3], _("New Account"));
    }
    toolBar_->AddTool(MENU_ACCTLIST, _("Account List"), toolBarBitmaps[4], _("Show Account List"));
    toolBar_->AddSeparator();
    toolBar_->AddTool(MENU_ORGCATEGS, _("Organize Categories"), toolBarBitmaps[5], _("Show Organize Categories Dialog"));
    toolBar_->AddTool(MENU_ORGPAYEE, _("Organize Payees"), toolBarBitmaps[6], _("Show Organize Payees Dialog"));
    toolBar_->AddTool(MENU_CURRENCY, _("Organize Currency"), toolBarBitmaps[7], _("Show Organize Currency Dialog"));
	toolBar_->AddSeparator();
    toolBar_->AddTool(MENU_TRANSACTIONREPORT, _("Transaction Filter"), toolBarBitmaps[8], _("Transaction Filter"));
    
    // after adding the buttons to the toolbar, must call Realize() to reflect changes
    toolBar_->Realize();
}
//----------------------------------------------------------------------------

void mmGUIFrame::createDataStore(const wxString& fileName, const wxString& pwd, bool openingNew)
{
    if (core_)
    {
        core_.reset();
    }

    if (db_)
    {
        db_->Close();
        db_.reset();
    }

	wxFileName checkExt(fileName);
	wxString password;
	if (checkExt.GetExt().Lower() == wxT("emb") && wxFileName::FileExists(fileName))
	{
        password = !pwd.empty() ? pwd : wxGetPasswordFromUser(_("Enter database's password"));
	}

    // Existing Database
    if (!openingNew 
        && !fileName.IsEmpty() 
        && wxFileName::FileExists(fileName))
    {    
        /* Do a backup before opening */
        wxString backupDBState =  mmDBWrapper::getINISettingValue(inidb_.get(), wxT("BACKUPDB"), wxT("FALSE"));
        if (backupDBState == wxT("TRUE"))
        {
            wxFileName fn(fileName);
            wxString bkupName = fn.GetPath() + wxT("/") + fn.GetName() + wxT(".bak");
            wxCopyFile(fileName, bkupName, true);
        }

        boost::shared_ptr<wxSQLite3Database> pDB(new wxSQLite3Database);
        db_ = pDB;
        db_->Open(fileName, password);
        // we need to check the db whether it is the right version
        if (!mmDBWrapper::checkDBVersion(db_.get()))
        {
           wxString note = mmex::getProgramName() + _(" - No File opened ");
           this->SetTitle(note);   
           mmShowErrorMessage(this, 
                _("Sorry. The Database version is too old or Database password is incorrect"), 
                _("Error opening database"));

           db_->Close();
           db_.reset();
           return ;
        }

        password_ = password;
        core_.reset(new mmCoreDB(db_));
    }
    else if (openingNew) // New Database
    {
       if (mmIniOptions::enableCustomTemplateDB_
           && wxFileName::FileExists(mmIniOptions::customTemplateDB_))
       {
           wxCopyFile(mmIniOptions::customTemplateDB_, fileName, true);
           boost::shared_ptr<wxSQLite3Database> pDB(new wxSQLite3Database());
           db_ = pDB;
           db_->Open(fileName);
           password_ = password;
           core_.reset(new mmCoreDB(db_));
       }
       else
       {
           db_.reset(new wxSQLite3Database);
           db_->Open(fileName, password);
           password_ = password;

           openDataBase(fileName);
           core_.reset(new mmCoreDB(db_));

           mmNewDatabaseWizard* wizard = new mmNewDatabaseWizard(this, core_.get());
           wizard->CenterOnParent();
           wizard->RunIt(true);

           mmDBWrapper::loadBaseCurrencySettings(db_.get());

           /* Load User Name and Other Settings */
           mmIniOptions::loadInfoOptions(db_.get());

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
        
        wxMessageDialog msgDlg(this, _("Cannot locate previously opened database.\nDo you want to browse to locate the file?"), 
                                _("Error opening database"), wxYES_NO);
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

    mmDBWrapper::initDB(db_.get(), &dlg);
    dlg.Update(100);
    dlg.Destroy();

    wxString title = mmex::getProgramName() + wxT(" : ") + fileName;

    if (mmex::isPortableMode())
        title << wxT(" [") << _("portable mode") << wxT(']');

    SetTitle(title);

    m_topCategories.Clear();
    mmIniOptions::loadInfoOptions(db_.get());

    if (db_) {
        fileName_ = fileName;
    } else {
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
    // Before deleting, go to home page first createHomePage()
    createDataStore(fileName, password, openingNew);
  
    if (db_) {
        menuEnableItems(true);
        menuPrintingEnable(false);
    }
    
    updateNavTreeControl();

    if (!refreshRequested_) {
        refreshRequested_ = true;
        /* Currency Options might have changed so refresh */
        wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
        GetEventHandler()->AddPendingEvent(ev); 
    }

    if (!db_)
        showBeginAppDialog();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnNew(wxCommandEvent& /*event*/)
{
    wxFileDialog dlg(this, 
                     _("Choose database file to create"), 
                     wxGetEmptyString(), 
                     wxGetEmptyString(), 
                     wxT("MMB Files(*.mmb)|*.mmb"), 
                     wxSAVE | wxOVERWRITE_PROMPT
                    );

    if(dlg.ShowModal() != wxID_OK) {
      return;
    }

    wxString fileName = dlg.GetPath();
    
    if (!fileName.EndsWith(wxT(".mmb"))) {
        fileName += wxT(".mmb");
    }
    
    openFile(fileName, true);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOpen(wxCommandEvent& /*event*/)
{
    wxString fileName = wxFileSelector(_("Choose database file to open"), 
                                       0, 0, 0, 
                                       wxT("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb"), 
                                       wxFILE_MUST_EXIST,
                                       this
                                      );
  
    if (!fileName.empty()) {
        openFile(fileName, false);
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnConvertEncryptedDB(wxCommandEvent& /*event*/)
{
    wxString encFileName = wxFileSelector(_("Choose Encrypted database file to open"), 
                                          0, 0, 0, 
                                          wxT("Encrypted MMB files (*.emb)|*.emb"), 
                                          wxFILE_MUST_EXIST, 
                                          this
                                         );

    if (encFileName.empty())
        return;
    
    wxString password = wxGetPasswordFromUser(_("Enter password for database"));
    if (password.empty())
        return;
        
    wxFileDialog dlg(this, 
                     _("Choose database file to Save As"), 
                     wxGetEmptyString(), 
                     wxGetEmptyString(), 
                     wxT("MMB Files(*.mmb)|*.mmb"), 
                     wxSAVE | wxOVERWRITE_PROMPT
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
    bool ok = db_ != 0;
    wxASSERT(ok);

    if (fileName_.empty()) {
        wxASSERT(false);
        return;
    }

    wxFileDialog dlg(this,
                     _("Save database file as"),
                     wxGetEmptyString(),
                     wxGetEmptyString(),
                     wxT("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb"),
                     wxSAVE | wxOVERWRITE_PROMPT
                    );

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    bool encrypt = dlg.GetFilterIndex() != 0; // emb -> Encrypted mMB
    wxFileName newFileName(dlg.GetPath());
    fixFileExt(newFileName, encrypt ? wxT("emb") : wxT("mmb"));

    wxFileName oldFileName(fileName_); // opened db's file

    if (newFileName == oldFileName) { // on case-sensitive FS uses case-sensitive comparison
        wxMessageDialog dlg(this, _("Can't copy file to itself"), _("Error"), wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    // prepare to copy

    wxString new_password;
    bool rekey = encrypt ^ db_->IsEncrypted();

    if (encrypt)
    {
        if (rekey) {
            new_password = wxGetPasswordFromUser(_("Enter password for new database"));
            if (new_password.empty()) {
                return;
            }
        } else {
            new_password = password_;
        }
    }

    // copying db

    if (db_) // database must be closed before copying its file
    {
      db_->Close();
      db_.reset();
    }

    if (!wxCopyFile(oldFileName.GetFullPath(), newFileName.GetFullPath(), true)) { // true -> overwrite if file exists
        return;
    }
    
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

void mmGUIFrame::OnExport(wxCommandEvent& /*event*/)
{
   mmExportCSV(db_.get());
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnExportToQIF(wxCommandEvent& /*event*/)
{
   mmExportQIF(db_.get());
}
//----------------------------------------------------------------------------
 
void mmGUIFrame::OnImportCSV(wxCommandEvent& /*event*/)
{
    int accountID = mmImportCSV(core_.get());
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnImportQIF(wxCommandEvent& /*event*/)
{
    int accountID = mmImportQIF(core_.get());
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnImportUniversalCSV(wxCommandEvent& /*event*/)
{
    if (mmDBWrapper::getNumAccounts(db_.get()) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Import! Create a new account first!"), 
            _("Error"));
        return;
    }

    boost::shared_ptr<mmUnivCSVImportDialog> dlg(new mmUnivCSVImportDialog(core_.get(), this), mmex::Destroy);
    dlg->ShowModal();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnImportCSVMMNET(wxCommandEvent& /*event*/)
{
    int accountID = mmImportCSVMMNET(core_.get());
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}
//----------------------------------------------------------------------------
    
void mmGUIFrame::OnNewAccount(wxCommandEvent& /*event*/)
{
    mmAddAccountWizard* wizard = new mmAddAccountWizard(this, core_.get());
    wizard->CenterOnParent();
    wizard->RunIt(true);

    if (wizard->acctID_ != -1)
    {
        mmNewAcctDialog *dlg = new mmNewAcctDialog(core_.get(), false, wizard->acctID_, this);
        if ( dlg->ShowModal() == wxID_OK )
        {
            
        }
        updateNavTreeControl();    
        dlg->Destroy();
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
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOrgCategories(wxCommandEvent& /*event*/)
{
    mmCategDialog *dlg = new mmCategDialog(core_.get(), this, false);
    dlg->ShowModal();
    dlg->Destroy();
}
//----------------------------------------------------------------------------
 
void mmGUIFrame::OnOrgPayees(wxCommandEvent& /*event*/)
{
    mmPayeeDialog *dlg = new mmPayeeDialog(core_.get(), this, false);
    dlg->ShowModal();
    dlg->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnBudgetSetupDialog(wxCommandEvent& /*event*/)
{
     if (!db_.get())
       return;

    mmBudgetYearDialog *dlg = new mmBudgetYearDialog(db_.get(), this);
    dlg->ShowModal();
    createHomePage();
    updateNavTreeControl();    
    dlg->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnTransactionReport(wxCommandEvent& /*event*/)
{
   if (!db_.get())
      return;

   if (mmDBWrapper::getNumAccounts(db_.get()) == 0)
      return;

   std::vector< boost::shared_ptr<mmBankTransaction> >* trans 
      = new std::vector< boost::shared_ptr<mmBankTransaction> >;
   mmFilterTransactionsDialog* dlg = new mmFilterTransactionsDialog(trans, core_.get(), this);
   if (dlg->ShowModal() == wxID_OK)
   {
      mmPrintableBase* rs = new mmReportTransactions(trans, core_.get());
      menuPrintingEnable(true);
      createReportsPage(rs);
   }
   else
   {
      delete trans;
   }
   dlg->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnCashFlowSpecificAccounts()
{
    if (!db_.get())
       return;

     if (mmDBWrapper::getNumAccounts(db_.get()) == 0)
         return;

     wxArrayString accountArray;
     for (int iAdx = 0; iAdx < (int) core_->accountList_.accounts_.size(); iAdx++)
     {
         mmCheckingAccount* pCA 
             = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
         if (pCA)
         {
             accountArray.Add(pCA->accountName_);
         }
     }
    wxMultiChoiceDialog* mcd = new wxMultiChoiceDialog(this, _("Choose Accounts"), 
        _("Cash Flow"),
        accountArray);
    if (mcd->ShowModal() == wxID_OK)
    {
        wxArrayInt arraySel = mcd->GetSelections();

        wxArrayString selections;
        for (int i = 0; i < (int)arraySel.size(); i++)
        {
            selections.Add(accountArray.Item(arraySel[i]));
        }

        mmPrintableBase* rs = new mmReportCashFlow(core_.get(), &selections);
        menuPrintingEnable(true);
        createReportsPage(rs);
    }
    mcd->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOptions(wxCommandEvent& /*event*/)
{
    if (!db_.get() || !inidb_)
        return;

    mmOptionsDialog *dlg = new mmOptionsDialog(core_.get(), inidb_.get(), this);
    dlg->ShowModal();
    dlg->Destroy();
    createHomePage();
    updateNavTreeControl();

    // enable or disable online update currency rate
    wxString enableCurrencyUpd = mmDBWrapper::getINISettingValue(inidb_.get(), wxT("UPDATECURRENCYRATE"), wxT("FALSE"));
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
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnHelp(wxCommandEvent& /*event*/)
{
    menuPrintingEnable(true);
    createHelpPage();
}
//----------------------------------------------------------------------------
 
void mmGUIFrame::OnCheckUpdate(wxCommandEvent& /*event*/)
{
    wxString site = wxT("http://www.codelathe.com/mmex/version.html");
    wxURL url(site);

    unsigned char buf[1024];
    boost::scoped_ptr<wxInputStream> in_stream(url.GetInputStream());
	if (!in_stream)
	{
		mmShowErrorMessage(this, _("Unable to connect!"), _("Check Update"));
		return;
	}
    in_stream->Read(buf, 1024);
    //size_t bytes_read=in_stream->LastRead();
    in_stream.reset();
    buf[7] = '\0';

    wxString page = wxString::FromAscii((const char *)buf);
    wxStringTokenizer tkz(page, wxT('.'), wxTOKEN_RET_EMPTY_ALL);  
    int numTokens = (int)tkz.CountTokens();
    if (numTokens != 4)
    {
        wxString url = wxT("http://www.codelathe.com/mmex");
        wxLaunchDefaultBrowser(url);
        return;
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

    if (isUpdateAvailable)
    {
        mmShowErrorMessage(this, _("New update available!"), _("Check Update"));
        wxString url = wxT("http://www.codelathe.com/mmex");
        wxLaunchDefaultBrowser(url);
    }
    else
    {
        mmShowErrorMessage(this, _("You have the latest version installed!"), _("Check Update"));
    }

}
//----------------------------------------------------------------------------

void mmGUIFrame::OnOnlineUpdateCurRate(wxCommandEvent& /*event*/)
{
    wxArrayString   currency_name, currency_rate;
    double          *rate_ptr, rate_value, base_rate;
    int             i;
    
    // we will get latest currency rate data from European Central Bank
    wxString site = wxT("http://www.ecb.europa.eu/stats/eurofxref/eurofxref-daily.xml");

    if (!core_) {
       mmShowErrorMessage(this, _("No database!"), _("Update Currency Rate"));
       return;
    }

    wxURL url(site);

    wxInputStream* in_stream = url.GetInputStream();

    if (!in_stream) {
        mmShowErrorMessage(this, _("Unable to connect!"), _("Update Currency Rate"));
        return;
    }
    
    wxXmlDocument doc;

    if(!doc.Load(*in_stream)) {
        mmShowErrorMessage(this, _("Cannot get data from WWW!"), _("Update Currency Rate"));
        return;
    }

    // decode received XML data
    if(doc.GetRoot()->GetName() != wxT("gesmes:Envelope")) {
        mmShowErrorMessage(this, _("Incorrect XML data (#1)!"), _("Update Currency Rate"));
        return;
    }

    wxXmlNode *root_child = doc.GetRoot()->GetChildren();

    while(root_child) {
        if(root_child->GetName() == wxT("gesmes:subject")) {
            if(root_child->GetNodeContent() != wxT("Reference rates")) {
                mmShowErrorMessage(this, _("Incorrect XML data (#2)!"), _("Update Currency Rate"));
                return;
            }
        } else if (root_child->GetName() == wxT("gesmes:Sender")) {
            wxXmlNode *sender_child = root_child->GetChildren();

            if(!sender_child) {
                mmShowErrorMessage(this, _("Incorrect XML data (#3)!"), _("Update Currency Rate"));
                return;
            }

            if(sender_child->GetName() != wxT("gesmes:name")) {
                mmShowErrorMessage(this, _("Incorrect XML data (#4)!"), _("Update Currency Rate"));
                return;
            }

            if(sender_child->GetNodeContent() != wxT("European Central Bank")) {
                mmShowErrorMessage(this, _("Incorrect XML data (#5)!"), _("Update Currency Rate"));
                return;
            }            
        } else if (root_child->GetName() == wxT("Cube")) {
            wxXmlNode *cube_lv1_child = root_child->GetChildren();

            if(!cube_lv1_child) {
                mmShowErrorMessage(this, _("Incorrect XML data (#6)!"), _("Update Currency Rate"));
                return;
            }

            if(cube_lv1_child->GetName() != wxT("Cube")) {
                mmShowErrorMessage(this, _("Incorrect XML data (#7)!"), _("Update Currency Rate"));
                return;
            }

            wxXmlNode *cube_lv2_child = cube_lv1_child->GetChildren();

            // set default base currency and its rate            
            currency_name.Add(wxT("EUR"));
            currency_rate.Add(wxT("1.0"));

            while(cube_lv2_child) {
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

    int currencyID = mmDBWrapper::getBaseCurrencySettings(core_->db_.get());
    wxString base_symbol = mmDBWrapper::getCurrencySymbol(core_->db_.get(), currencyID);
    base_rate = 0;

    for(i=0; i<(int)currency_rate.GetCount(); i++) {
        if(currency_rate[i].ToDouble(&rate_value) == false) {
            rate_ptr[i] = 1.0;
        } else {
            rate_ptr[i] = rate_value;

            if(currency_name[i] == base_symbol) {
                base_rate = rate_value;
            }
        }
    }

    if(base_rate == 0) {
        mmShowErrorMessage(this, _("Could not find base currency symbol!"), _("Update Currency Rate"));
        return;
    }

    // Note:
    // Suppose currency Y / currency X = Y / X and currency Z / currency X = Z / X, then
    // currency Z / currency Y = Z / Y
    // Therefore, if currency X is EUR and currency Y is the base currency, 
    // currency Y : currency Z = base_rate / rate of Z : 1
    for(i=0; i<(int)currency_rate.GetCount(); i++) {
        rate_ptr[i] = base_rate / rate_ptr[i];
    }

    // update currency rates

    for (int idx = 0; idx < (int)core_->currencyList_.currencies_.size(); idx++) {
        wxString currencySymbol  = core_->currencyList_.currencies_[idx]->currencySymbol_;
        for(i=0; i<(int)currency_name.GetCount(); i++) {
            if(currency_name[i] == currencySymbol) {
                core_->currencyList_.currencies_[idx]->baseConv_ = rate_ptr[i];
                core_->currencyList_.updateCurrency(core_->currencyList_.currencies_[idx]);
            }
        } // for i loop
    }

    delete[] rate_ptr;

    wxMessageDialog msgDlg(this, _("Currency rate updated"), _("Update Currency Rate"));
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
    wxString url = wxT("http://groups.google.com/group/mmlist");
    wxLaunchDefaultBrowser(url);
}
//----------------------------------------------------------------------------
    
void mmGUIFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    mmAboutDialog *dlg = new mmAboutDialog(inidb_.get(), this);
    dlg->ShowModal();
    dlg->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPrintPageSetup(wxCommandEvent& WXUNUSED(event))
{
    if (printer_)
        printer_->PageSetup();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPrintPageReport(wxCommandEvent& WXUNUSED(event))
{
    if (!printer_)
        return;

    mmReportsPanel* rp = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    mmHelpPanel* hp = dynamic_cast<mmHelpPanel*>(panelCurrent_);
    if (rp)
    {
        printer_ ->PrintText(rp->getReportText());
    }
     else if (hp)
    {
        printer_ ->PrintFile(mmex::getPathDoc(mmex::HTML_INDEX));
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnPrintPagePreview(wxCommandEvent& WXUNUSED(event))
{ 
    if (!printer_)
        return;

    mmReportsPanel* rp = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    mmHelpPanel* hp = dynamic_cast<mmHelpPanel*>(panelCurrent_);
    if (rp)
    {
        printer_ ->PreviewText(panelCurrent_->getReportText());
    }
    else if (hp)
    {
        printer_ ->PreviewFile(mmex::getPathDoc(mmex::HTML_INDEX));
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::showBeginAppDialog()
{
    mmAppStartDialog *dlg = new mmAppStartDialog(inidb_.get(), this);
    dlg->ShowModal();
    int rc = dlg->getReturnCode();

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
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_REPORTISSUES);
        AddPendingEvent(evt);
    }
    else if (rc == 4)
    {
        wxFileName fname(mmDBWrapper::getLastDbPath(inidb_.get()));
        if (fname.IsOk()) {
                openFile(fname.GetFullPath(), false);
        }
    }
    else if (rc == -1)
    {
        /* Do Nothing in this case */
    }

    dlg->Destroy();
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
            wxGetEmptyString(), wxGetEmptyString(), wxGetEmptyString(), wxT("*.html"), wxSAVE | wxOVERWRITE_PROMPT);
        if ( !fileName.empty() )
        {
            wxFileOutputStream output( fileName );
            wxTextOutputStream text( output );
            text << panelCurrent_->getReportText();
        }
    }
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnBillsDeposits(wxCommandEvent& WXUNUSED(event))
{
    wxSizer *sizer = cleanupHomePanel();
    
    panelCurrent_ = new mmBillsDepositsPanel(db_.get(), inidb_.get(), core_.get(), homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnStocks(wxCommandEvent& /*event*/)
{
	// First, check to see if there is a stock account setup
	static const char sql[] = 
    "SELECT * FROM ACCOUNTLIST_V1 WHERE ACCOUNTTYPE = 'Investment' ";
	
	
    wxSQLite3Statement st = db_->PrepareStatement(sql);
    //st.Bind(1, transID_);
	
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
		wxSizer *sizer = cleanupHomePanel();
		
		panelCurrent_ = new mmStocksPanel(db_.get(), inidb_.get(), -1, homePanel, ID_PANEL3, 
										  wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		
		sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
		
		homePanel->Layout();		
	}
	else 
	{
		wxMessageBox(wxT("This database does not have any investment accounts associated with it.  You can create one using the Accounts menu."),
					 wxT("No Investment Accounts Configured"),wxOK);
	}
	q1.Finalize();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnAssets(wxCommandEvent& /*event*/)
{
    wxSizer *sizer = cleanupHomePanel();
    
    panelCurrent_ = new mmAssetsPanel(db_.get(), inidb_.get(), homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    
    sizer->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnCurrency(wxCommandEvent& /*event*/)
{
    mmMainCurrencyDialog *dlg = new mmMainCurrencyDialog(core_.get(),this, false);
    if ( dlg->ShowModal() == wxID_OK )
    {
    }
    dlg->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnWizardCancel(wxWizardEvent& event)
{
     event.Veto();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnEditAccount(wxCommandEvent& /*event*/)
{
    if (core_->accountList_.accounts_.size() == 0)
    {
        mmShowErrorMessage(0, _("No Account available!"), _("Error"));
        return;
    }
  
    wxArrayString as;
    int num = (int)core_->accountList_.accounts_.size();
    boost::scoped_array<int> arrAcctID(new int[num]);
    for (size_t idx = 0; idx < core_->accountList_.accounts_.size(); ++idx)
    {
        as.Add(core_->accountList_.accounts_[idx]->accountName_);
        arrAcctID[idx] = core_->accountList_.accounts_[idx]->accountID_;
    }
  
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, 
        _("Choose Account to Edit"), 
        _("Accounts"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        int choice = scd->GetSelection();
        int acctID = arrAcctID[choice];
        mmNewAcctDialog *dlg = new mmNewAcctDialog(core_.get(), false, acctID, this);
        if ( dlg->ShowModal() == wxID_OK )
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
    scd->Destroy();
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnDeleteAccount(wxCommandEvent& /*event*/)
{
    if (core_->accountList_.accounts_.size() == 0)
    {
        mmShowErrorMessage(0, _("No Account available!"), _("Error"));
        return;
    }

    wxArrayString as;
    int num = (int)core_->accountList_.accounts_.size();
    int* arrAcctID = new int[num];
    for (int idx = 0; idx < (int)core_->accountList_.accounts_.size(); idx++)
    {
        as.Add(core_->accountList_.accounts_[idx]->accountName_);
        arrAcctID[idx] = core_->accountList_.accounts_[idx]->accountID_;
    }
  
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, 
        _("Choose Account to Delete"), 
        _("Accounts"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        int choice = scd->GetSelection();
        int acctID = arrAcctID[choice];

        wxMessageDialog msgDlg(this, 
            _("Do you really want to delete the account?"),
            _("Confirm Account Deletion"),
            wxYES_NO);
        if (msgDlg.ShowModal() == wxID_YES)
        {
            core_->accountList_.deleteAccount(acctID);
            core_->bTransactionList_.deleteTransactions(acctID);

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
    scd->Destroy();
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
	if(	m_mgr.GetPane(wxT("toolbar")).IsShown())
		event.Check(true);
	else
		event.Check(false);
}
//----------------------------------------------------------------------------

void mmGUIFrame::OnViewLinksUpdateUI(wxUpdateUIEvent &event)
{
	if(	m_mgr.GetPane(wxT("Navigation")).IsShown())
		event.Check(true);
	else
		event.Check(false);
}
//----------------------------------------------------------------------------

void wxNewDatabaseWizardPage1::OnCurrency(wxCommandEvent& /*event*/)
{
    currencyID_ = parent_->core_->currencyList_.getBaseCurrencySettings();


    mmMainCurrencyDialog *dlg = new mmMainCurrencyDialog(parent_->core_, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        currencyID_ = dlg->currencyID_;
        if (currencyID_ != -1)
        {
            wxString currName = parent_->core_->currencyList_.getCurrencySharedPtr(currencyID_)->currencyName_;
            wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_CURRENCY);
            bn->SetLabel(currName);
            parent_->core_->currencyList_.setBaseCurrencySettings(currencyID_);
        }
    }

    dlg->Destroy();
}
//----------------------------------------------------------------------------

wxNewDatabaseWizardPage1::wxNewDatabaseWizardPage1(mmNewDatabaseWizard* parent) : 
    wxWizardPageSimple(parent), 
    parent_(parent), 
    currencyID_(-1)
{
    currencyID_ = parent_->core_->currencyList_.getBaseCurrencySettings();
    wxString currName = _("Set Currency");
    if (currencyID_ != -1)
    {
        currName = parent_->core_->currencyList_.getCurrencySharedPtr(currencyID_)->currencyName_;
    }

    itemButtonCurrency_ = new wxButton( this, 
        ID_DIALOG_OPTIONS_BUTTON_CURRENCY, currName, wxDefaultPosition, wxDefaultSize, 0 );


    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("Base Currency for account")),
        0,
        wxALL,
        5
        );

    mainSizer->Add(
        itemButtonCurrency_,
        0, // No stretching
        wxALL,
        5 // Border
        );

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("\nSpecify the base (or default) currency to be used \nwith the database. You can change the \ncurrency associated with each account if needed.\n")), 0,
        wxALL,
        5);


    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( this, wxID_STATIC, _("User Name"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    itemUserName_ = new wxTextCtrl( this, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME, _T(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemUserName_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("(Optional)Specify your name. Mainly used for reporting and in printing reports.\n")), 0,
        wxALL,
        5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool wxNewDatabaseWizardPage1::TransferDataFromWindow()
{
    if ( currencyID_ == -1)
    {
        wxMessageBox(_("Base Currency Not Set"), 
            _("Error"),
            wxICON_WARNING | wxOK, this);

        return false;
    }
    userName = itemUserName_->GetValue().Trim();
    mmDBWrapper::setInfoSettingValue(parent_->core_->db_.get(), wxT("USERNAME"), userName); 

    return true;
}
//----------------------------------------------------------------------------

wxAddAccountPage1::wxAddAccountPage1(mmAddAccountWizard* parent) : 
    wxWizardPageSimple(parent), parent_(parent)
{
    textAccountName_ = new wxTextCtrl(this, wxID_ANY, 
        wxGetEmptyString(), wxDefaultPosition, wxDefaultSize, 0 );

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("Name of the Account")),
        0,
        wxALL,
        5
        );

    mainSizer->Add(
        textAccountName_,
        0, // No stretching
        wxALL,
        5 // Border
        );

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("\nSpecify a descriptive name of the account. This most commonly \nis the name of the financial institution where the account is held. \nFor example 'ABC Bank'.")), 0,
        wxALL,
        5);


    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool wxAddAccountPage1::TransferDataFromWindow()
{
    if ( textAccountName_->GetValue().empty())
    {
        wxMessageBox(_("Account Name Invalid"), 
            _("Error"),
            wxICON_WARNING | wxOK, this);

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
    wxString itemAcctTypeStrings[] =  
    {
        _("Checking/Savings"),
        _("Investment"),
    };
    itemChoiceType_ = new wxChoice( this, wxID_ANY, 
        wxDefaultPosition, wxDefaultSize, 2, itemAcctTypeStrings, 0 );
    itemChoiceType_->SetSelection(0); // Checking
    itemChoiceType_->SetToolTip(_("Specify the type of account to be created."));

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("Type of Account")),
        0,
        wxALL,
        5
        );

    mainSizer->Add(
        itemChoiceType_,
        0, // No stretching
        wxALL,
        5 // Border
        );

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("\nSelect the type of account you want to create:\n\nGeneral bank accounts cover a wide variety of account\n types like Checking, Savings and Credit card type accounts.")), 0,
        wxALL,
        5);

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY,
        _("\nInvestment accounts are specialized accounts that only \nhave stock/mutual fund investments associated \nwith them.\n")), 0,
        wxALL,
        5);

    SetSizer(mainSizer);
    mainSizer->Fit(this);
}
//----------------------------------------------------------------------------

bool wxAddAccountPage2::TransferDataFromWindow()
{
    int acctType = itemChoiceType_->GetSelection();
    wxString acctTypeStr = wxT("Checking");
    if (acctType == 1)
        acctTypeStr = wxT("Investment");

    int currencyID = parent_->core_->currencyList_.getBaseCurrencySettings();
    if (currencyID == -1)
    {
        mmShowErrorMessage(this, _("Base Account Currency Not set.\nSet that first using tools->options menu and then add a new account"), _("Error"));
        return false;
    }

    mmAccount* ptrBase;
    if (acctTypeStr == wxT("Checking"))
        ptrBase = new mmCheckingAccount(parent_->core_->db_);
    else
        ptrBase = new mmInvestmentAccount(parent_->core_->db_);

    boost::shared_ptr<mmAccount> pAccount(ptrBase);

    pAccount->favoriteAcct_ = true;
    pAccount->status_ = mmAccount::MMEX_Open;
    pAccount->acctType_ = acctTypeStr;
    pAccount->accountName_ = parent_->accountName_;
    pAccount->initialBalance_ = 0.0;
    pAccount->currency_ = parent_->core_->currencyList_.getCurrencySharedPtr(currencyID);
    parent_->acctID_ = parent_->core_->accountList_.addAccount(pAccount);

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

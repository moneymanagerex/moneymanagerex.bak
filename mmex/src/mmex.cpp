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
 /*******************************************************/

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
#include "wxtinyxml.h"
#include "tinyxml.h"

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
/*******************************************************/
#define MMEX_INIDB_FNAME wxT("/mmexini.db3")
#define MMEX_SPLASH_FNAME wxT("/splash.png")
/*******************************************************/

BEGIN_EVENT_TABLE(mmNewDatabaseWizard, wxWizard)
    EVT_WIZARD_CANCEL(wxID_ANY,   mmGUIFrame::OnWizardCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxNewDatabaseWizardPage1, wxWizardPageSimple)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, wxNewDatabaseWizardPage1::OnCurrency)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mmGUIFrame, wxFrame)
    EVT_MENU(MENU_NEW, mmGUIFrame::OnNew)
    EVT_MENU(MENU_OPEN, mmGUIFrame::OnOpen)
    EVT_MENU(MENU_SAVE_AS, mmGUIFrame::OnSaveAs)
    EVT_MENU(MENU_EXPORT_CSV, mmGUIFrame::OnExport)
    EVT_MENU(MENU_IMPORT_CSV, mmGUIFrame::OnImportCSV)
    EVT_MENU(MENU_IMPORT_MMNETCSV, mmGUIFrame::OnImportCSVMMNET)
    EVT_MENU(MENU_IMPORT_QIF, mmGUIFrame::OnImportQIF)
    EVT_MENU(MENU_IMPORT_QFX, mmGUIFrame::OnImportQFX)
    EVT_MENU(MENU_IMPORT_UNIVCSV, mmGUIFrame::OnImportUniversalCSV)
    EVT_MENU(MENU_QUIT,  mmGUIFrame::OnQuit)
    EVT_MENU(MENU_NEWACCT,  mmGUIFrame::OnNewAccount)
    EVT_MENU(MENU_ACCTLIST,  mmGUIFrame::OnAccountList)
    EVT_MENU(MENU_ACCTEDIT,  mmGUIFrame::OnEditAccount)
    EVT_MENU(MENU_ACCTDELETE,  mmGUIFrame::OnDeleteAccount)
    EVT_MENU(MENU_ORGCATEGS,  mmGUIFrame::OnOrgCategories)
    EVT_MENU(MENU_ORGPAYEE,  mmGUIFrame::OnOrgPayees)
    EVT_MENU(MENU_OPTIONS,  mmGUIFrame::OnOptions)
    EVT_MENU(MENU_BUDGETSETUPDIALOG, mmGUIFrame::OnBudgetSetupDialog)
    EVT_MENU(MENU_HELP,  mmGUIFrame::OnHelp)
    EVT_MENU(MENU_CHECKUPDATE,  mmGUIFrame::OnCheckUpdate)
    EVT_MENU(MENU_REPORTISSUES,  mmGUIFrame::OnReportIssues)
    EVT_MENU(MENU_ANNOUNCEMENTMAILING,  mmGUIFrame::OnBeNotified)
    EVT_MENU(MENU_ABOUT, mmGUIFrame::OnAbout)
    EVT_MENU(MENU_PRINT_PAGE_SETUP, mmGUIFrame::OnPrintPageSetup)
    EVT_MENU(MENU_PRINT_REPORT, mmGUIFrame::OnPrintPageReport)
    EVT_MENU(MENU_PRINT_PREVIEW_REPORT, mmGUIFrame::OnPrintPagePreview)
    EVT_MENU(MENU_SHOW_APPSTART, mmGUIFrame::OnShowAppStartDialog)
    EVT_MENU(MENU_EXPORT_HTML, mmGUIFrame::OnExportToHtml)
    EVT_MENU(MENU_BILLSDEPOSITS, mmGUIFrame::OnBillsDeposits)
    EVT_MENU(MENU_STOCKS, mmGUIFrame::OnStocks)
    EVT_MENU(MENU_ASSETS, mmGUIFrame::OnAssets)
    EVT_MENU(MENU_CURRENCY, mmGUIFrame::OnCurrency)
    EVT_MENU(MENU_TREEPOPUP_LAUNCHWEBSITE, mmGUIFrame::OnLaunchAccountWebsite)
	EVT_MENU(MENU_VIEW_TOOLBAR, mmGUIFrame::OnViewToolbar)
	EVT_MENU(MENU_VIEW_LINKS, mmGUIFrame::OnViewLinks)
	EVT_UPDATE_UI(MENU_VIEW_TOOLBAR, mmGUIFrame::OnViewToolbarUpdateUI)
	EVT_UPDATE_UI(MENU_VIEW_LINKS, mmGUIFrame::OnViewLinksUpdateUI)
    
    EVT_MENU(MENU_TREEPOPUP_EDIT, mmGUIFrame::OnPopupEditAccount)
    EVT_MENU(MENU_TREEPOPUP_DELETE, mmGUIFrame::OnPopupDeleteAccount)

    EVT_TREE_ITEM_RIGHT_CLICK(ID_NAVTREECTRL, mmGUIFrame::OnItemRightClick)
    EVT_TREE_SEL_CHANGED(ID_NAVTREECTRL, mmGUIFrame::OnSelChanged)

    EVT_MENU(MENU_GOTOACCOUNT, mmGUIFrame::OnGotoAccount)

    EVT_MENU(MENU_TRANSACTIONREPORT, mmGUIFrame::OnTransactionReport)
END_EVENT_TABLE()
/*******************************************************/
IMPLEMENT_APP(mmGUIApp)
/*******************************************************/
bool mmGUIApp::OnInit()
{
    /* Get INI DB for loading settings */
    wxSQLite3Database* inidb = new wxSQLite3Database();

    wxString cfgPath = mmGetBaseWorkingPath() + MMEX_INIDB_FNAME;
    inidb->Open(cfgPath);
    mmDBWrapper::verifyINIDB(inidb);

    /* Load Colors from Database */
    mmLoadColorsFromDatabase(inidb);

    /* Load MMEX Custom Settings */
    mmIniOptions::loadOptions(inidb);

    /* Was App Maximized? */
    wxString isMaxStrDef = wxT("FALSE");
    wxString isMaxStr = mmDBWrapper::getINISettingValue(inidb, 
        wxT("ISMAXIMIZED"), isMaxStrDef);

    /* Load Dimensions of Window */
    wxString originX = wxT("0");
    wxString originY = wxT("0");
    wxString sizeW = wxT("800");
    wxString sizeH = wxT("600");
    wxString valxStr = mmDBWrapper::getINISettingValue(inidb, 
        wxT("ORIGINX"), originX); 
    wxString valyStr = mmDBWrapper::getINISettingValue(inidb, 
        wxT("ORIGINY"), originY);
    wxString valWStr = mmDBWrapper::getINISettingValue(inidb, 
        wxT("SIZEW"),  sizeW);
    wxString valHStr = mmDBWrapper::getINISettingValue(inidb, 
        wxT("SIZEH"),  sizeH);
    long valx, valy, valw, valh;
    valxStr.ToLong(&valx);
    valyStr.ToLong(&valy);
    valWStr.ToLong(&valw);
    valHStr.ToLong(&valh);

	/* Setting Locale causes unexpected problems, so default to English Locale */
	m_locale.Init(wxLANGUAGE_ENGLISH);

	/* Select language if necessary */
    mmSelectLanguage(inidb);

    inidb->Close();
    delete inidb;

    /* See if we need to load graphs */
    if (!mmGraphGenerator::checkGraphFiles())
        mmIniOptions::enableGraphs_ = false;

    /* Initialize Image Handlers */
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());

    /* Load GUI Frame */
    mmGUIFrame *frame = new mmGUIFrame(mmIniOptions::appName_,
                                 wxPoint(valx, valy), 
                                 wxSize(valw, valh));

    frame->Show(TRUE);
    if (isMaxStr == wxT("TRUE"))
        frame->Maximize(true);
  
    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}
/*******************************************************/
mmAddAccountWizard::mmAddAccountWizard(wxFrame *frame, mmCoreDB* core)
         :wxWizard(frame,wxID_ANY,_("Add Account Wizard"),
                   wxBitmap(addacctwiz_xpm),wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE), core_(core), acctID_(-1)
{
    // a wizard page may be either an object of predefined class
    page1 = new wxWizardPageSimple(this);

    wxString noteString = mmIniOptions::appName_
                          + 
    _(" models all transactions as belonging to accounts.\n\n The next pages will help you create a new account.\n\nTo help you get started, begin by making a list of all\nfinancial institutions where you hold an account.");

    new wxStaticText(page1, 
       wxID_ANY,
       noteString         
        );

    wxAddAccountPage1* page2 = new wxAddAccountPage1(this);
    wxAddAccountPage2* page3 = new wxAddAccountPage2(this);

    // set the page order using a convenience function - could also use
    // SetNext/Prev directly as below
    wxWizardPageSimple::Chain(page1, page2);

     wxWizardPageSimple::Chain(page2, page3);

    // allow the wizard to size itself around the pages
    GetPageAreaSizer()->Add(page1);
}

void mmAddAccountWizard::RunIt(bool modal)
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


mmNewDatabaseWizard::mmNewDatabaseWizard(wxFrame *frame, mmCoreDB* core)
         :wxWizard(frame,wxID_ANY,_("New Database Wizard"),
                   wxBitmap(addacctwiz_xpm),wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE), core_(core)
{
    page1 = new wxWizardPageSimple(this);

    new wxStaticText(page1, wxID_ANY,
             _("The next pages will help you create a new database.\n\nYour database file is stored with an extension \nof .mmb. Make sure to make backups of this \nfile and to store it carefully as it contains important \nfinancial information."
             )
        );

    wxNewDatabaseWizardPage1* page2 = new wxNewDatabaseWizardPage1(this);

    // set the page order using a convenience function - could also use
    // SetNext/Prev directly as below
    wxWizardPageSimple::Chain(page1, page2);

    // allow the wizard to size itself around the pages
    GetPageAreaSizer()->Add(page1);
}

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

/******************************************************
mmGUIFrame 
********************************************************/
bool sortCategs( mmGUIFrame::CategInfo elem1, mmGUIFrame::CategInfo elem2 )
{
   return elem1.amount < elem2.amount;
}

mmGUIFrame::mmGUIFrame(const wxString& title, 
                       const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame*)NULL, -1, title, pos, size), 
       core_(0), 
       inidb_(0), 
       gotoAccountID_(-1), 
       selectedItemData_(0),
       m_topCategories(wxT("")),
       panelCurrent_(0),
       refreshRequested_(false)

{
	// tell wxAuiManager to manage this frame
	m_mgr.SetManagedWindow(this);

	/* Set Icon for Frame */
    wxIcon icon(mainicon_xpm);
    SetIcon(icon);

    /* Setup Printer */
    printer_ = new wxHtmlEasyPrinting(mmIniOptions::appName_, this);
    wxString printHeaderBase = mmIniOptions::appName_;
    printer_-> SetHeader( printHeaderBase + wxT("(@PAGENUM@/@PAGESCNT@)<hr>"), wxPAGE_ALL);

    /* Create the Controls for the frame */
    createMenu();
    createToolBar();
    createControls();

	/* Load from Settings DB */
    loadConfigFile();

	// add the toolbars to the manager
	m_mgr.AddPane(toolBar_, wxAuiPaneInfo().
		Name(wxT("toolbar")).Caption(wxT("Toolbar")).ToolbarPane().Top().
		LeftDockable(false).RightDockable(false));

    // change look and feel of wxAuiManager
    m_mgr.GetArtProvider()->SetMetric(16, 0);
    m_mgr.GetArtProvider()->SetMetric(3, 1);

	// Save default perspective
	m_perspective = m_mgr.SavePerspective();
	
    wxString auiPerspective = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("AUIPERSPECTIVE"), m_perspective);

    m_mgr.LoadPerspective(auiPerspective);

	// "commit" all changes made to wxAuiManager
	m_mgr.Update();

	/* Decide if we need to show app start dialog */
    wxString showBeginApp = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("SHOWBEGINAPP"), wxT("TRUE"));
    if (showBeginApp == wxT("TRUE"))
    {
         /* No Previous File */
        menuEnableItems(false);
        createHomePage();
        updateNavTreeControl();

        /* Show Begin App Dialog */
        showBeginAppDialog();
    }
    else
    {
        /* Try loading last db if it exists */
        wxString val = mmDBWrapper::getINISettingValue(inidb_, 
            wxT("LASTFILENAME"), wxT(""));
        if (!val.IsEmpty())
        {
            /* Try Opening the file */
            wxFileName fName(val);
            wxString absName;

            /* Relative paths don't work well in Linux, so using it only
            for Windows */
#ifdef __WXGTK__
            absName = fName.GetFullPath();		
#else	
            wxFileName appPath(mmGetBaseWorkingPath(true));
            fName.Normalize( wxPATH_NORM_ALL, appPath.GetPath());
            absName = fName.GetFullPath();
#endif

            openFile(absName, false);
        }
        else
        {
            /* No Previous File */
            menuEnableItems(false);
            createHomePage();
            updateNavTreeControl();

            /* Show Begin App Dialog */
            showBeginAppDialog();
        }
    }
}

mmGUIFrame::~mmGUIFrame()
{
	delete printer_;
    saveConfigFile();

    m_mgr.UnInit();
 
    /* Delete the GUI */
    homePanel->DestroyChildren();

    if (core_)
    {
        delete core_;
        core_ = 0;
    }

    if (db_)
    {
        db_->Close();
    }

    if (inidb_)
    {
        inidb_->Close();
        delete inidb_;
    }
}

void mmGUIFrame::saveConfigFile()
{
    /* Save our settings to ini db */
    wxFileName appPath(mmGetBaseWorkingPath(true));
    wxFileName fname(fileName_);

#ifdef __WXGTK__

#else
    bool makeRelative = fname.MakeRelativeTo(appPath.GetPath());
#endif
    mmDBWrapper::setINISettingValue(inidb_, 
        wxT("LASTFILENAME"), mmCleanString(fname.GetFullPath()));

    mmSaveColorsToDatabase(inidb_);

    bool isMax = this->IsMaximized();
    wxString isMaxStr = wxT("FALSE");
    if (isMax)
        isMaxStr = wxT("TRUE");

    /* Aui Settings */
    m_perspective = m_mgr.SavePerspective();
    mmDBWrapper::setINISettingValue(inidb_, 
        wxT("AUIPERSPECTIVE"), m_perspective);

    int valx, valy, valw, valh;
    this->GetPosition(&valx, &valy);
    this->GetSize(&valw, &valh);    
    wxString valxs = wxString::Format(wxT("%d"), valx);
    wxString valys = wxString::Format(wxT("%d"), valy);
    wxString valws = wxString::Format(wxT("%d"), valw);
    wxString valhs = wxString::Format(wxT("%d"), valh);
    mmDBWrapper::setINISettingValue(inidb_, wxT("ORIGINX"), valxs); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("ORIGINY"), valys);
    mmDBWrapper::setINISettingValue(inidb_, wxT("SIZEW"), valws);
    mmDBWrapper::setINISettingValue(inidb_, wxT("SIZEH"), valhs);
    mmDBWrapper::setINISettingValue(inidb_, wxT("ISMAXIMIZED"), isMaxStr);
}

void mmGUIFrame::loadConfigFile()
{
    inidb_ = new wxSQLite3Database();
    wxString iniDBPath = mmGetBaseWorkingPath()
        + MMEX_INIDB_FNAME;
    inidb_->Open(iniDBPath);
}

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
    menuBar_->FindItem(MENU_OPTIONS)->Enable(enable);
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

void mmGUIFrame::menuPrintingEnable(bool enable)
{
    menuBar_->FindItem(MENU_PRINT_PREVIEW)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT)->Enable(enable);
    menuBar_->FindItem(MENU_EXPORT_HTML)->Enable(enable);
}

void mmGUIFrame::createControls()
{
#ifdef __WXGTK__
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
        new mmTreeItemData(wxT("Where the Money Goes - Over Time")));

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
    wxTreeItemId budgetPerformance;
    if (mmIniOptions::enableBudget_)
    {
        budgetPerformance = navTreeCtrl_->AppendItem(reports, 
            _("Budget Performance"), 4, 4);
        navTreeCtrl_->SetItemData(budgetPerformance, 
            new mmTreeItemData(wxT("Budget Performance")));
    }

     ///////////////////////////////////////////////////////////////////
    wxTreeItemId cashFlow = navTreeCtrl_->AppendItem(reports, 
        _("Cash Flow"), 4, 4);
    navTreeCtrl_->SetItemData(cashFlow, 
        new mmTreeItemData(wxT("Cash Flow")));

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

    if (!db_.get())
       return;

    /* Load Nav Tree Control */
    mmBEGINSQL_LITE_EXCEPTION;

    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));

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

    if (mmIniOptions::enableBudget_)
    {
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from BUDGETYEAR_V1 order by BUDGETYEARNAME;");
        wxSQLite3ResultSet q1 = db_.get()->ExecuteQuery(bufSQL);
        int index = 0;
        while (q1.NextRow())
        {
            wxTreeItemId bYear = navTreeCtrl_->AppendItem(budgeting, q1.GetString(wxT("BUDGETYEARNAME")), 3, 3);
            navTreeCtrl_->SetItemData(bYear, new mmTreeItemData(q1.GetInt(wxT("BUDGETYEARID")), true));

            wxTreeItemId bYearData = navTreeCtrl_->AppendItem(budgetPerformance, q1.GetString(wxT("BUDGETYEARNAME")), 4, 4);
            navTreeCtrl_->SetItemData(bYearData, new mmTreeItemData(q1.GetInt(wxT("BUDGETYEARID")), true));
        }
        q1.Finalize();

        navTreeCtrl_->Expand(budgeting);
    }

    mmENDSQL_LITE_EXCEPTION;

    navTreeCtrl_->Expand(accounts);
}

wxString mmGUIFrame::createCategoryList()
{
    if (!db_)
        return wxT("");

    mmHTMLBuilder hb;

    hb.addHTML(wxT("<br>"));
    hb.addHTML(wxT("<br>"));

    std::vector<CategInfo> categList;
    hb.addHTML(wxT("<table cellspacing=\"0\" cellpadding=\"1\" border=\"0\">"));
    std::vector<wxString> headerR;
    headerR.push_back(_("Top Categories Last 30 Days  "));

    hb.addTableHeaderRow(headerR, wxT(" BGCOLOR=\"#80B9E8\" "), wxT(" width=\"130\" COLSPAN=\"2\" "));

    core_->currencyList_.loadBaseCurrencySettings();

    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CATEGORY_V1 order by CATEGNAME;");
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
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
        mmCurrencyFormatter::formatDoubleToCurrency(amt, balance);

        if (amt != 0.0)
        {
            CategInfo info;
            info.categ = categString;
            info.amountStr = balance;
            info.amount = amt;
            categList.push_back(info);
        }

        wxSQLite3StatementBuffer bufSQL1;
        bufSQL1.Format("select * from SUBCATEGORY_V1 where CATEGID=%d;", categID);
        wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL1); 
        while(q2.NextRow())
        {
            int subcategID          = q2.GetInt(wxT("SUBCATEGID"));
            wxString subcategString    = q2.GetString(wxT("SUBCATEGNAME"));

            amt = core_->bTransactionList_.getAmountForCategory(categID, subcategID, 
                false,  dtBegin, dtEnd);
            mmCurrencyFormatter::formatDoubleToCurrency(amt, balance);

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

    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;

    std::sort(categList.begin(), categList.end(), sortCategs);
    for (int idx = 0; idx < (int)categList.size(); idx++)
    {
        if (idx > 5)
            break;

        hb.addHTML(wxT("<tr> <td width=\"130\"> "));
        hb.addHTML(categList[idx].categ);
        hb.addHTML(wxT("</td><td width=\"100\" align=\"right\">"));
        hb.addHTML(categList[idx].amountStr);
        hb.addHTML(wxT("</td>"));
    }

    hb.endTable();

    mmGraphTopCategories gtp;
    gtp.init(categList);
    gtp.generate();

    return hb.getHTMLText();
}

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
                mmPrintableBase* rs = new mmReportBudgetingPerformance(core_, data);
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
                 homePanel->DestroyChildren();
                 homePanel->SetSizer(NULL);

                 wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
                 homePanel->SetSizer(itemBoxSizer1);

                 panelCurrent_ = new mmStocksPanel(db_.get(), inidb_, data, homePanel, ID_PANEL3, 
                    wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
                 itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

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
                wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
                GetEventHandler()->AddPendingEvent(ev); ;
            }
            return;
        }

        if (!core_ || !db_)
            return;

        //Freeze();

        if (iData->getString() == wxT("Summary of Accounts"))
        {
            mmPrintableBase* rs = new mmReportSummary(core_);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Summary of Stocks"))
        {
            mmPrintableBase* rs = new mmReportSummaryStocks(db_.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Summary of Assets"))
        {
            mmPrintableBase* rs = new mmReportSummaryAssets(db_.get());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - Over Time"))
        {
            mmPrintableBase* rs = new mmReportCategoryOverTimePerformance(core_);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - Month"))
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

            mmPrintableBase* rs = new mmReportCategoryExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime prevYearEnd = wxDateTime(today);
            prevYearEnd.SetYear(year);
            prevYearEnd.SetMonth(wxDateTime::Dec);
            prevYearEnd.SetDay(31);
            wxDateTime dtEnd = prevYearEnd;
            wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime yearBegin = wxDateTime(today);
            yearBegin.SetYear(year);
            yearBegin.SetMonth(wxDateTime::Dec);
            yearBegin.SetDay(31);
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = yearBegin;
            mmPrintableBase* rs = new mmReportCategoryExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        /////////////////////////////////////////////////////////////////
        
        if (iData->getString() == wxT("Transaction Statistics"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportTransactionStats(core_, year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        

        /////////////////////////////////////////////////////////////////

        if (iData->getString() == wxT("Income vs Expenses"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(core_, year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Month"))
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

            mmPrintableBase* rs = new mmReportIncomeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportIncomeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportIncomeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(core_, year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(core_, year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - All Time"))
        {
            mmPrintableBase* rs = new mmReportIncomeExpenses(core_, true, wxDateTime::Now(),
                wxDateTime::Now());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        /////////////////////////////////////////////////////////////////

        if (iData->getString() == wxT("To Whom the Money Goes"))
        {
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_, true, wxDateTime::Now(),
                wxDateTime::Now());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - Month"))
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

            mmPrintableBase* rs = new mmReportPayeeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime prevYearEnd = wxDateTime(today);
            prevYearEnd.SetYear(year);
            prevYearEnd.SetMonth(wxDateTime::Dec);
            prevYearEnd.SetDay(31);
            wxDateTime dtEnd = prevYearEnd;
            wxDateTime dtBegin = prevYearEnd.Subtract(wxDateSpan::Year());
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear() - 1;
            wxDateTime yearBegin = wxDateTime(today);
            yearBegin.SetYear(year);
            yearBegin.SetMonth(wxDateTime::Dec);
            yearBegin.SetDay(31);
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = yearBegin;
            mmPrintableBase* rs = new mmReportPayeeExpenses(core_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Help"))
        {
            menuPrintingEnable(true);
            createHelpPage();
        }

        if (iData->getString() == wxT("Cash Flow"))
        {
            mmPrintableBase* rs = new mmReportCashFlow(core_);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }
        
       // Thaw();

        if (iData->getString() == wxT("Cash Flow - Specific Accounts"))
        {
            OnCashFlowSpecificAccounts();
        }

         ///////////////////////////////////////////////
        if (iData->getString() == wxT("Transaction Report"))
        {
           wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_TRANSACTIONREPORT);
           AddPendingEvent(evt);
        }

        //////////////////////////////////////////////

        if (iData->getString() == wxT("Budgeting"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BUDGETSETUPDIALOG);
            AddPendingEvent(evt);
        }
            
        if (iData->getString() == wxT("Bills & Deposits"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_BILLSDEPOSITS);
            AddPendingEvent(evt);
        }

        if (iData->getString() == wxT("Stocks"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_STOCKS);
            AddPendingEvent(evt);
        }

        if (iData->getString() == wxT("Assets"))
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ASSETS);
            AddPendingEvent(evt);
        }

    }
}

void mmGUIFrame::OnLaunchAccountWebsite(wxCommandEvent& event)
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

void mmGUIFrame::OnPopupEditAccount(wxCommandEvent& event)
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
              mmNewAcctDialog *dlg = new mmNewAcctDialog(core_, false, data, this);
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

void mmGUIFrame::OnPopupDeleteAccount(wxCommandEvent& event)
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

void mmGUIFrame::OnItemRightClick(wxTreeEvent& event)
{
    showTreePopupMenu(event.GetItem(), event.GetPoint());
}

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
}


void mmGUIFrame::createCheckingAccountPage(int accountID)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmCheckingPanel(core_, inidb_, 
        accountID, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);


    homePanel->Layout();
}

void mmGUIFrame::OnGotoAccount(wxCommandEvent& WXUNUSED(event))
{
    if (gotoAccountID_ != -1)    
    {
        createCheckingAccountPage(gotoAccountID_);
    }
}

void mmGUIFrame::createBudgetingPage(int budgetYearID)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);

    panelCurrent_ = new mmBudgetingPanel(db_.get(), inidb_, budgetYearID, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::createHomePage()
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    if (m_topCategories == wxT(""))
       m_topCategories = createCategoryList();

    if (panelCurrent_)
    {
        //panelCurrent_->DestroyChildren();
        //panelCurrent_->SetSizer(NULL);
        panelCurrent_  = 0;
    }
    panelCurrent_ = new mmHomePagePanel(this, 
        db_.get(), core_, 
        m_topCategories,
        homePanel, 
        ID_PANEL3, 
        wxDefaultPosition, 
        wxDefaultSize, 
        wxNO_BORDER|wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
     
    homePanel->Layout();
    refreshRequested_ = false;
}

void mmGUIFrame::createReportsPage(mmPrintableBase* rs)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
       
    panelCurrent_ = new mmReportsPanel(this, db_.get(), rs, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
     
    homePanel->Layout();
}

void mmGUIFrame::createHelpPage()
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
       
    panelCurrent_ = new mmHelpPanel(this, db_.get(), homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
     
    homePanel->Layout();
}


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

   wxMenu *menuFile = new wxMenu;
   wxMenuItem* menuItemNew = new wxMenuItem(menuFile, MENU_NEW, 
      _("&New Database\tCtrl-N"), 
      _("New Database"));
   menuItemNew->SetBitmap(toolBarBitmaps[0]);

   wxMenuItem* menuItemOpen = new wxMenuItem(menuFile, MENU_OPEN, 
      _("&Open Database\tCtrl-O"), 
      _("Open Database"));
   menuItemOpen->SetBitmap(toolBarBitmaps[1]);

   menuFile->Append(menuItemNew);
   menuFile->Append(menuItemOpen);

   wxMenuItem* menuItemSaveAs = new wxMenuItem(menuFile, MENU_SAVE_AS, 
      _("Save Database &As"), 
      _("Save Database As"));
   menuItemSaveAs->SetBitmap(wxBitmap(saveas_xpm));
   menuFile->Append(menuItemSaveAs);

   menuFile->AppendSeparator();

   wxMenu* exportMenu = new wxMenu;
   exportMenu->Append(MENU_EXPORT_CSV, _("&CSV Files"), _("Export to CSV"));
   exportMenu->Append(MENU_EXPORT_HTML, _("&Report to HTML"), _("Export to HTML"));
   menuFile->Append(MENU_EXPORT, _("&Export"), exportMenu);

   wxMenu* importMenu = new wxMenu;
   importMenu->Append(MENU_IMPORT_QIF, _("&QIF Files"), _("Import from QIF"));
   importMenu->Append(MENU_IMPORT_UNIVCSV, _("&Universal CSV Files"), _("Import from any CSV file"));
   if (mmIniOptions::enableImportMMCSV_)
      importMenu->Append(MENU_IMPORT_CSV, _("&MMEX CSV Files"), _("Import from MMEX CSV"));
   if (mmIniOptions::enableImportMMNETCSV_)
      importMenu->Append(MENU_IMPORT_MMNETCSV, _("&MM.NET CSV Files"), _("Import from MM.NET CSV"));
   //importMenu->Append(MENU_IMPORT_QFX, _("&QFX Files"), _("Import from QFX"));
   menuFile->Append(MENU_IMPORT, _("Import"), importMenu);

   menuFile->AppendSeparator();

    
	wxMenuItem* menuItemPrintSetup = new wxMenuItem(menuFile, MENU_PRINT_PAGE_SETUP, 
	   _("Page Setup..."), _("Setup page printing options"));
	menuItemPrintSetup->SetBitmap(toolBarBitmaps[7]);
    menuFile->Append(menuItemPrintSetup); 
     
    wxMenu* printPreviewMenu = new wxMenu;
    printPreviewMenu->Append(MENU_PRINT_PREVIEW_REPORT, 
        _("Current View"), _("Preview current report"));

    menuFile->Append(MENU_PRINT_PREVIEW, _("Print Preview..."), printPreviewMenu);

    wxMenu* printMenu = new wxMenu;
    wxMenuItem* menuItemPrintView = new wxMenuItem(printMenu, MENU_PRINT_REPORT, 
		_("Current View"), _("Print current report"));
    printMenu->Append(menuItemPrintView);

    menuFile->Append( MENU_PRINT, _("Print..."),  printMenu);

    menuFile->AppendSeparator();

    wxMenuItem* menuItemQuit = new wxMenuItem(menuFile, MENU_QUIT, 
		_("E&xit\tAlt-X"), _("Quit this program"));
	menuItemQuit->SetBitmap(wxBitmap(exit_xpm));
    menuFile->Append(menuItemQuit);
   
	wxMenu *menuView = new wxMenu;

	wxMenuItem* menuItemToolbar = new wxMenuItem(menuView, MENU_VIEW_TOOLBAR, 
		_("&Toolbar"), _("Show/Hide the toolbar"), wxITEM_CHECK);
	wxMenuItem* menuItemLinks = new wxMenuItem(menuView, MENU_VIEW_LINKS, 
		_("Navigation"), _("Show/Hide the Navigation tree control"), wxITEM_CHECK);

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
		_("Edit Account"), _("Edit Account"));
	menuItemAcctEdit->SetBitmap(toolBarBitmaps[8]);

    if (mmIniOptions::enableDeleteAccount_)
    {
        wxMenuItem* menuItemAcctDelete = new wxMenuItem(menuAccounts, MENU_ACCTDELETE, 
            _("Delete Account"), _("Delete Account from database"));
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
		 _("Organize Currency"), _("Organize Currency"));
	menuItemCurrency->SetBitmap(wxBitmap(money_dollar_xpm));
    menuTools->Append(menuItemCurrency);

    if (mmIniOptions::enableBudget_)
    {
        wxMenuItem* menuItemBudgeting = new wxMenuItem(menuTools, MENU_BUDGETSETUPDIALOG, 
            _("Budget Setup"), _("Budget Setup"));
        menuItemBudgeting->SetBitmap(wxBitmap(calendar_xpm));
        menuTools->Append(menuItemBudgeting); 
    }

    if (mmIniOptions::enableRepeatingTransactions_)
    {
       wxMenuItem* menuItemBillsDeposits = new wxMenuItem(menuTools, MENU_BILLSDEPOSITS, 
          _("Repeating Transactions"), _("Bills && Deposits"));
       menuItemBillsDeposits->SetBitmap(wxBitmap(clock_xpm));
       menuTools->Append(menuItemBillsDeposits); 
    }

    if (mmIniOptions::enableStocks_)
    {
        wxMenuItem* menuItemStocks = new wxMenuItem(menuTools, MENU_STOCKS, 
            _("Stock Investments"), _("Stock Investments"));
        menuItemStocks->SetBitmap(wxBitmap(stock_curve_xpm));
        menuTools->Append(menuItemStocks);
    }

    if (mmIniOptions::enableAssets_)
    {
        wxMenuItem* menuItemAssets = new wxMenuItem(menuTools, MENU_ASSETS, 
            _("Assets"), _("Assets"));
        menuItemAssets->SetBitmap(wxBitmap(car_xpm));
        menuTools->Append(menuItemAssets);
    }

     menuTools->AppendSeparator();

    wxMenuItem* menuItemTransactions = new wxMenuItem(menuTools, MENU_TRANSACTIONREPORT, 
		_("Transaction Filter"), _("Transaction Filter"));
	menuItemTransactions->SetBitmap(wxBitmap(filter_xpm));
    menuTools->Append(menuItemTransactions);

    menuTools->AppendSeparator();

    wxMenuItem* menuItemOptions = new wxMenuItem(menuTools, MENU_OPTIONS, 
		  _("&Options"), _("Show the Options Dialog"));
	menuItemOptions->SetBitmap(wxBitmap(wrench_xpm));
    menuTools->Append(menuItemOptions);

    wxMenu *menuHelp = new wxMenu;

    wxMenuItem* menuItemHelp = new wxMenuItem(menuTools, MENU_HELP, 
		 _("&Help\tCtrl-F1"), _("Show the Help file"));
	menuItemHelp->SetBitmap(wxBitmap(help_xpm));
    menuHelp->Append(menuItemHelp);

    wxMenuItem* menuItemAppStart = new wxMenuItem(menuTools, MENU_SHOW_APPSTART, 
		 _("Show App Start Dialog"), _("App Start Dialog"));
	menuItemAppStart->SetBitmap(wxBitmap(appstart_xpm));
    menuHelp->Append(menuItemAppStart);

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
          _("Report Issues or Feedback"), _("Send email through the mailing list to report issues with the software."));
       menuItemReportIssues->SetBitmap(wxBitmap(issues_xpm));
       menuHelp->Append(menuItemReportIssues);
    }

    if (mmIniOptions::enableBeNotifiedForNewReleases_)
    {
       wxMenuItem* menuItemNotify = new wxMenuItem(menuTools, MENU_ANNOUNCEMENTMAILING, 
          _("Be notified of new releases"), _("Sign up for the announcement mailing list"));
       menuItemNotify->SetBitmap(wxBitmap(notify_xpm));
       menuHelp->Append(menuItemNotify); 
    }
        
    wxMenuItem* menuItemAbout = new wxMenuItem(menuTools, MENU_ABOUT, 
       _("&About..."), _("Show about dialog"));
    menuItemAbout->SetBitmap(wxBitmap(about_xpm));
    menuHelp->Append(menuItemAbout);

    menuBar_ = new wxMenuBar;
    menuBar_->Append(menuFile, _("&File"));
    menuBar_->Append(menuAccounts, _("&Accounts"));
    menuBar_->Append(menuTools, _("&Tools"));
    menuBar_->Append(menuView, _("&View"));
    menuBar_->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar_);
}

void mmGUIFrame::createToolBar()
{
	toolBar_ = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTB_FLAT | wxTB_NODIVIDER);
    wxBitmap toolBarBitmaps[8];
    toolBarBitmaps[0] = wxBitmap(new_xpm);
    toolBarBitmaps[1] = wxBitmap(open_xpm);
    toolBarBitmaps[2] = wxBitmap(save_xpm);
    toolBarBitmaps[3] = wxBitmap(newacct_xpm);
    toolBarBitmaps[4] = wxBitmap(house_xpm);
    toolBarBitmaps[5] = wxBitmap(categoryedit_xpm);
    toolBarBitmaps[6] = wxBitmap(user_edit_xpm);
    toolBarBitmaps[7] = wxBitmap(money_dollar_xpm);

    toolBar_->AddTool(MENU_NEW, _("New"), toolBarBitmaps[0], 
       _("New Database"));
    toolBar_->AddTool(MENU_OPEN, _("Open"), toolBarBitmaps[1], 
       _("Open Database"));
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
    
    // after adding the buttons to the toolbar, must call Realize() to reflect changes
    toolBar_->Realize();
}

void mmGUIFrame::createDataStore(const wxString& fileName, 
                                 bool openingNew)
{
    if (core_)
    {
        delete core_;
        core_ = 0;
    }

    if (db_)
    {
        db_->Close();
        db_.reset();
    }

	wxFileName checkExt(fileName);
	wxString password = wxEmptyString;
	if (checkExt.GetExt() == wxT("emb"))
	{
		password = wxGetPasswordFromUser(wxT("Password For Database.."));
	}

    // Existing Database
    if (!openingNew 
        && !fileName.IsEmpty() 
        && wxFileName::FileExists(fileName))
    {    
        /* Do a backup before opening */
        wxString backupDBState =  mmDBWrapper::getINISettingValue(inidb_, wxT("BACKUPDB"), wxT("FALSE"));
        if (backupDBState == wxT("TRUE"))
        {
            wxFileName fn(fileName);
            wxString bkupName = fn.GetPath() + wxT("/") + fn.GetName() + wxT(".bak");
            wxCopyFile(fileName, bkupName, true);
        }

        boost::shared_ptr<wxSQLite3Database> pDB(new wxSQLite3Database());
        db_ = pDB;
        //db_->Open(fileName, password);
        db_->Open(fileName);

        // we need to check the db whether it is the right version
        if (!mmDBWrapper::checkDBVersion(db_.get()))
        {
           wxString note = mmIniOptions::appName_
              + _(" - No File opened ");
           this->SetTitle(note);   
           mmShowErrorMessage(this, 
              _("Sorry. The Database version is too old or \
                Database password is incorrect"), 
                _("Error opening database"));

           db_->Close();
           db_.reset();
           return ;
        }
        password_ = password;

        core_ = new mmCoreDB(db_);
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
           core_ = new mmCoreDB(db_);
       }
       else
       {
           boost::shared_ptr<wxSQLite3Database> pDB(new wxSQLite3Database());
           db_ = pDB;
           //db_->Open(fileName, password);
           db_->Open(fileName);
           password_ = password;

           openDataBase(fileName);

           core_ = new mmCoreDB(db_);

           mmNewDatabaseWizard* wizard = new mmNewDatabaseWizard(this, core_);
           wizard->RunIt(true);

           mmDBWrapper::loadBaseCurrencySettings(db_.get());

           /* Jump to new account creation screen */
           wxCommandEvent evt;
           OnNewAccount(evt);
           return;
       }
    }
    else // open of existing database failed
    {
       wxString note = mmIniOptions::appName_ + 
               _(" - No File opened ");
        this->SetTitle(note);   
        
        wxMessageDialog msgDlg(this, _("Cannot locate previously opened .mmb database.\nDo you want to browse to locate the file?"), 
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

void mmGUIFrame::openDataBase(const wxString& fileName)
{
    wxString appPath = mmGetBaseWorkingPath();

    wxProgressDialog* pgd = new wxProgressDialog(_("Please Wait"), 
        _("Opening Database File && Verifying Integrity"), 100, this, 
        wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH );
    mmDBWrapper::initDB(db_.get(), pgd, appPath);
    pgd->Update(100);
    pgd->Destroy();

    wxFileName fName(fileName);
    wxString title = mmIniOptions::appName_ + wxT(" : ") 
        + fileName;
    this->SetTitle(title);

    m_topCategories = wxT("");
    mmIniOptions::loadInfoOptions(db_.get());

    if (db_.get())
    {
        fileName_ = fileName;
        
    }
    else
	{
      fileName_ = wxT("");
      password_ = wxEmptyString;
	}
}

wxPanel* mmGUIFrame::createMainFrame(wxPanel* parent)
{
       return 0;
}

void mmGUIFrame::openFile(const wxString& fileName, bool openingNew)
{
    // Before deleting, go to home page first
    // createHomePage();
    createDataStore(fileName, openingNew);
  
    if (db_.get())
    {
        menuEnableItems(true);
        menuPrintingEnable(false);
    }
    
    updateNavTreeControl();
    if (!refreshRequested_)
    {
        refreshRequested_ = true;
        /* Currency Options might have changed so refresh */
        wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MENU_ACCTLIST);
        GetEventHandler()->AddPendingEvent(ev); 
    }

    if (!db_.get())
    {
        mmDBWrapper::setINISettingValue(inidb_, 
            wxT("LASTFILENAME"), wxT(""));
        showBeginAppDialog();
    }
}

void mmGUIFrame::OnNew(wxCommandEvent& event)
{
	//wxString extSupported = wxT("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb");
    wxString extSupported = wxT("MMB Files(*.mmb)|*.mmb");
#ifdef __WXGTK__ 
	// Don't support encrypted databases in Linux yet
	extSupported = wxT("MMB Files(*.mmb)|*.mmb");
#endif
    wxString fileName = wxFileSelector(wxT("Choose database file to create"), 
                wxT(""), wxT(""), wxT(""), extSupported, wxSAVE | wxOVERWRITE_PROMPT);
    if ( !fileName.empty() )
    {
	   openFile(fileName, true);
    }
}

void mmGUIFrame::OnOpen(wxCommandEvent& event)
{
	//wxString extSupported = wxT("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb");
    wxString extSupported = wxT("MMB Files(*.mmb)|*.mmb");
#ifdef __WXGTK__ 
	// Don't support encrypted databases in Linux yet
	extSupported = wxT("MMB Files(*.mmb)|*.mmb");
#endif
    wxString fileName = wxFileSelector(wxT("Choose database file to open"), 
                wxT(""), wxT(""), wxT(""), extSupported, wxFILE_MUST_EXIST);
    if ( !fileName.empty() )
    {
        openFile(fileName, false);
    }
}

void mmGUIFrame::OnSaveAs(wxCommandEvent& event)
{
	wxString wildCardStr = wxT("MMB Files(*.mmb)|*.mmb");
	
#ifdef __WXGTK__ 
	
#else
	//wildCardStr = wxT("MMB Files(*.mmb)|*.mmb|Encrypted MMB files (*.emb)|*.emb");
#endif

    wxString fileName = wxFileSelector(wxT("Choose database file to Save As"), 
                wxT(""), wxT(""), wxT(""), wildCardStr, wxSAVE | wxOVERWRITE_PROMPT);
    if ( !fileName.empty() )
    {
        if (db_)
        {
            db_->Close();
            db_.reset();
        }
        wxCopyFile(fileName_, fileName, false);
		
        wxFileName newFileName(fileName);
        wxFileName oldFileName(fileName_);
        if (newFileName.GetExt() == oldFileName.GetExt())
        {
          // do nothing
        }
        else
        {
#ifdef __WXGTK__ 

#else
#if 0
          wxString password    = wxEmptyString;
          wxString oldpassword = password_;
          if (newFileName.GetExt() == wxT("emb"))
          {
            password = wxGetPasswordFromUser(wxT("Set Password For Database.."));
          }
          
          boost::shared_ptr<wxSQLite3Database> pDB(new wxSQLite3Database());
          db_ = pDB;
          
          if (oldFileName.GetExt() == wxT("emb"))
          {
            //db_->Open(fileName, oldpassword);
            db_->Open(fileName);
            db_->ReKey(wxEmptyString);
          }
          else
          {
            // TODO: This is not working and it crashes
            // because sqlite_rekey is crashing 
            db_->Open(fileName);
            db_->ReKey(password);
          }
          
          db_->Close();
          db_.reset();
#endif
#endif	
        }
        openFile(fileName, false);
    }
}

void mmGUIFrame::OnExport(wxCommandEvent& event)
{
   mmExportCSV(db_.get());
}

void mmGUIFrame::OnImportCSV(wxCommandEvent& event)
{
    int accountID = mmImportCSV(core_);
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}

void mmGUIFrame::OnImportQFX(wxCommandEvent& event)
{
    if (mmDBWrapper::getNumAccounts(db_.get()) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Import! Create a new account first!"), 
            _("Error"));
        return ;
    }

    wxArrayString as;
    int fromAccountID = -1;
    
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3ResultSet q1 
        = db_.get()->ExecuteQuery("select * from ACCOUNTLIST_V1 where ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }

    mmENDSQL_LITE_EXCEPTION;

    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, _("Choose Account to import to:"), 
        _("QFX Import"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        wxString acctName = scd->GetStringSelection();
        fromAccountID = mmDBWrapper::getAccountID(db_.get(), acctName);
     
        wxString fileName = wxFileSelector(wxT("Choose QFX data file to import"), 
                wxT(""), wxT(""), wxT(""), wxT("*.qfx"), wxFILE_MUST_EXIST);
        if ( !fileName.empty() )
        {
            wxTiXmlDocument xmlDoc; 
            TiXmlDocument* basexml = &xmlDoc;

            wxFileInputStream fs(fileName);
            xmlDoc.LoadFile(fs);


            TiXmlNode* node = 0;
            for( node = xmlDoc.IterateChildren( 0 );
                node;
                node = xmlDoc.IterateChildren( node ) )
            {
                // parse XML declaration
                if (node->ToDeclaration())
                {
                    TiXmlDeclaration* declaration = node->ToDeclaration();
                }

                if (node->ToElement())
                {
                    TiXmlElement* ele = node->ToElement();
                    std::string str = ele->ValueStr();
                    wxString eleStr = wxString::FromAscii(str.c_str());
                    if (eleStr != wxT("mmCoreDataStore"))
                    {

                    }
                }
            }
           
        }
    }
    scd->Destroy();
}

void mmGUIFrame::OnImportQIF(wxCommandEvent& event)
{
    int accountID = mmImportQIF(core_);
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}

void mmGUIFrame::OnImportUniversalCSV(wxCommandEvent& event)
{
    if (mmDBWrapper::getNumAccounts(db_.get()) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Import! Create a new account first!"), 
            _("Error"));
        return;
    }

    mmUnivCSVImportDialog *dlg = new mmUnivCSVImportDialog(core_, this);
    dlg->ShowModal();
    dlg->Destroy();
}

void mmGUIFrame::OnImportCSVMMNET(wxCommandEvent& event)
{
    int accountID = mmImportCSVMMNET(core_);
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}

void mmGUIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}
    
void mmGUIFrame::OnNewAccount(wxCommandEvent& event)
{
    mmAddAccountWizard* wizard = new mmAddAccountWizard(this, core_);
    wizard->RunIt(true);

    if (wizard->acctID_ != -1)
    {
        mmNewAcctDialog *dlg = new mmNewAcctDialog(core_, false, wizard->acctID_, this);
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
    
void mmGUIFrame::OnAccountList(wxCommandEvent& event)
{
    createHomePage();
}

void mmGUIFrame::OnOrgCategories(wxCommandEvent& event)
{
    mmCategDialog *dlg = new mmCategDialog(core_, this);
    dlg->ShowModal();
    dlg->Destroy();
}
 
void mmGUIFrame::OnOrgPayees(wxCommandEvent& event)
{
    mmPayeeDialog *dlg = new mmPayeeDialog(core_, true, this, false);
    dlg->ShowModal();
    dlg->Destroy();
}

void mmGUIFrame::OnBudgetSetupDialog(wxCommandEvent& event)
{
     if (!db_.get())
       return;

    mmBudgetYearDialog *dlg = new mmBudgetYearDialog(db_.get(), this);
    dlg->ShowModal();
    createHomePage();
    updateNavTreeControl();    
    dlg->Destroy();
}

void mmGUIFrame::OnTransactionReport(wxCommandEvent& event)
{
   if (!db_.get())
      return;

   if (mmDBWrapper::getNumAccounts(db_.get()) == 0)
      return;

   std::vector< boost::shared_ptr<mmBankTransaction> >* trans 
      = new std::vector< boost::shared_ptr<mmBankTransaction> >;
   mmFilterTransactionsDialog* dlg = new mmFilterTransactionsDialog(trans, core_, this);
   if (dlg->ShowModal() == wxID_OK)
   {
      mmPrintableBase* rs = new mmReportTransactions(trans, core_);
      menuPrintingEnable(true);
      createReportsPage(rs);
   }
   else
   {
      delete trans;
   }
   dlg->Destroy();
}

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

        mmPrintableBase* rs = new mmReportCashFlow(core_, &selections);
        menuPrintingEnable(true);
        createReportsPage(rs);
    }
    mcd->Destroy();
}

void mmGUIFrame::OnOptions(wxCommandEvent& event)
{
    if (!db_.get() || !inidb_)
        return;

    mmOptionsDialog *dlg = new mmOptionsDialog(core_, inidb_, this);
    dlg->ShowModal();
    dlg->Destroy();
    createHomePage();
    updateNavTreeControl();    
}

void mmGUIFrame::OnHelp(wxCommandEvent& event)
{
    menuPrintingEnable(true);
    createHelpPage();
}
 
void mmGUIFrame::OnCheckUpdate(wxCommandEvent& event)
{
    wxString site = wxT("http://www.thezeal.com/software/managerex/version.html");
    wxURL url(site);

    unsigned char buf[1024];
    wxInputStream* in_stream = url.GetInputStream();
	if (!in_stream)
	{
		mmShowErrorMessage(this, _("Unable to connect!"), _("Check Update"));
		return;
	}
    in_stream->Read(buf, 1024);
    size_t bytes_read=in_stream->LastRead();
    delete in_stream;
    buf[7] = '\0';

    wxString page = wxString::FromAscii((const char *)buf);
    wxStringTokenizer tkz(page, wxT('.'), wxTOKEN_RET_EMPTY_ALL);  
    int numTokens = (int)tkz.CountTokens();
    if (numTokens != 4)
    {
        wxString url = wxT("http://sourceforge.net/project/showfiles.php?group_id=163169");
        wxLaunchDefaultBrowser(url);
        return;
    }
    
    wxString maj = tkz.GetNextToken();
    wxString min = tkz.GetNextToken();
    wxString cust = tkz.GetNextToken();
    wxString build = tkz.GetNextToken();

    // get current version
    wxString currentV = MMEXVERSION;
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
        wxString url = wxT("http://sourceforge.net/project/showfiles.php?group_id=163169");
        wxLaunchDefaultBrowser(url);
    }
    else
    {
        mmShowErrorMessage(this, _("You have the latest version installed!"), _("Check Update"));
    }

}

void mmGUIFrame::OnReportIssues(wxCommandEvent& event)
{
   wxString url = wxT("http://groups.google.com/group/zealsupport");
   wxLaunchDefaultBrowser(url);
}

void mmGUIFrame::OnBeNotified(wxCommandEvent& event)
{
    wxString url = wxT("http://groups.google.com/group/mmlist");
    wxLaunchDefaultBrowser(url);
}
    
void mmGUIFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    mmAboutDialog *dlg = new mmAboutDialog(inidb_, this);
    dlg->ShowModal();
    dlg->Destroy();
}

void mmGUIFrame::OnPrintPageSetup(wxCommandEvent& WXUNUSED(event))
{
    if (printer_)
        printer_->PageSetup();
}

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
        printer_ ->PrintFile(wxT("help/index.html"));
    }
}

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
        printer_ ->PreviewFile(wxT("help/index.html"));
    }
}

void mmGUIFrame::showBeginAppDialog()
{
    mmAppStartDialog *dlg = new mmAppStartDialog(inidb_, this);
    dlg->ShowModal();
    
    if (dlg->getReturnCode() == 0)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_NEW);
        AddPendingEvent(evt);
    }
    else if (dlg->getReturnCode() == 1)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_OPEN);
        AddPendingEvent(evt);
    }
    else if (dlg->getReturnCode() == 2)
    {
       wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_HELP);
       AddPendingEvent(evt);
    }
    else if (dlg->getReturnCode() == 3)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_CHECKUPDATE);
        AddPendingEvent(evt);
    }
    else if (dlg->getReturnCode() == 4)
    {
        wxString val = mmDBWrapper::getINISettingValue(inidb_, 
            wxT("LASTFILENAME"), wxT(""));
        if (!val.IsEmpty())
        {
            /* Try Opening the file */
            wxFileName fName(val);
            wxFileName appPath(mmGetBaseWorkingPath(true));
            fName.Normalize( wxPATH_NORM_ALL, appPath.GetPath());
            wxString absName = fName.GetFullPath();
            if (!absName.IsEmpty())
                openFile(absName, false);
        }
    }
    else if (dlg->getReturnCode() == -1)
    {
        /* Do Nothing in this case */
    }

    dlg->Destroy();
}


void mmGUIFrame::OnShowAppStartDialog(wxCommandEvent& WXUNUSED(event))
{
    showBeginAppDialog();
}

void mmGUIFrame::OnExportToHtml(wxCommandEvent& event)
{
    mmReportsPanel* rp = dynamic_cast<mmReportsPanel*>(panelCurrent_);
    if (rp)
    {
        wxString fileName = wxFileSelector(wxT("Choose HTML file to Export"), 
            wxT(""), wxT(""), wxT(""), wxT("*.html"), wxSAVE | wxOVERWRITE_PROMPT);
        if ( !fileName.empty() )
        {
            wxFileOutputStream output( fileName );
            wxTextOutputStream text( output );
            text << panelCurrent_->getReportText();
        }
    }
}

void mmGUIFrame::OnBillsDeposits(wxCommandEvent& event)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmBillsDepositsPanel(db_.get(), inidb_, core_, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::OnStocks(wxCommandEvent& event)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmStocksPanel(db_.get(), inidb_, -1, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::OnAssets(wxCommandEvent& event)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmAssetsPanel(db_.get(), inidb_, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::OnCurrency(wxCommandEvent& event)
{
    mmMainCurrencyDialog *dlg = new mmMainCurrencyDialog(core_,this);
    if ( dlg->ShowModal() == wxID_OK )
    {
    }
    dlg->Destroy();
}

void mmGUIFrame::OnWizardCancel(wxWizardEvent& event)
{
     event.Veto();
}

void mmGUIFrame::OnEditAccount(wxCommandEvent& event)
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
        _("Choose Account to Edit"), 
        _("Accounts"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        int choice = scd->GetSelection();
        int acctID = arrAcctID[choice];
        mmNewAcctDialog *dlg = new mmNewAcctDialog(core_, false, acctID, this);
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
    delete[] arrAcctID;
    scd->Destroy();
}

void mmGUIFrame::OnDeleteAccount(wxCommandEvent& event)
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

void mmGUIFrame::OnViewToolbar(wxCommandEvent &event)
{
	m_mgr.GetPane(wxT("toolbar")).Show(event.IsChecked());
	m_mgr.Update();
}

void mmGUIFrame::OnViewLinks(wxCommandEvent &event)
{
	m_mgr.GetPane(wxT("Navigation")).Show(event.IsChecked());
	m_mgr.Update();
}

void mmGUIFrame::OnViewToolbarUpdateUI(wxUpdateUIEvent &event)
{
	if(	m_mgr.GetPane(wxT("toolbar")).IsShown())
		event.Check(true);
	else
		event.Check(false);
}

void mmGUIFrame::OnViewLinksUpdateUI(wxUpdateUIEvent &event)
{
	if(	m_mgr.GetPane(wxT("Navigation")).IsShown())
		event.Check(true);
	else
		event.Check(false);
}

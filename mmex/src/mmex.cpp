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
#include "billsdeposits.h"
#include "mmhomepagepanel.h"
#include "mmreportspanel.h"
#include "mmhelppanel.h"
#include "stocks.h"
#include "assets.h"

#include "reportbase.h"
#include "reportsummary.h"
#include "reportcategexp.h"
#include "reportpayee.h"
#include "reportincexpenses.h"
#include "reportbudgetingperf.h"
#include "reportincexpesestime.h"

#include "appstartdialog.h"
#include "aboutdialog.h"
#include "newchkacctdialog.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "budgetyeardialog.h"
#include "optionsdialog.h"
#include "currencydialog.h"

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
    EVT_MENU(MENU_SAVE, mmGUIFrame::OnSave)
    EVT_MENU(MENU_SAVE_AS, mmGUIFrame::OnSaveAs)
    EVT_MENU(MENU_EXPORT_CSV, mmGUIFrame::OnExport)
    EVT_MENU(MENU_IMPORT_CSV, mmGUIFrame::OnImportCSV)
    EVT_MENU(MENU_IMPORT_MMNETCSV, mmGUIFrame::OnImportCSVMMNET)
    EVT_MENU(MENU_IMPORT_QIF, mmGUIFrame::OnImportQIF)
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

    EVT_MENU(MENU_TREEPOPUP_EDIT, mmGUIFrame::OnPopupEditAccount)
    EVT_MENU(MENU_TREEPOPUP_DELETE, mmGUIFrame::OnPopupDeleteAccount)

    EVT_TREE_ITEM_RIGHT_CLICK(ID_NAVTREECTRL, mmGUIFrame::OnItemRightClick)
    EVT_TREE_SEL_CHANGED(ID_NAVTREECTRL, mmGUIFrame::OnSelChanged)

    EVT_MENU(MENU_GOTOACCOUNT, mmGUIFrame::OnGotoAccount)

END_EVENT_TABLE()
/*******************************************************/
IMPLEMENT_APP(mmGUIApp)
/*******************************************************/
bool mmGUIApp::OnInit()
{
    /* Get INI DB for loading settings */
    wxSQLite3Database* inidb = new wxSQLite3Database();
    wxFileName fname(wxGetApp().argv[0]);
    wxString cfgPath = fname.GetPath(wxPATH_GET_VOLUME)
                + MMEX_INIDB_FNAME;
    inidb->Open(cfgPath);
    mmDBWrapper::verifyINIDB(inidb);

    /* Load Colors from Database */
    mmLoadColorsFromDatabase(inidb);

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

    /* Load GUI Frame */
    mmGUIFrame *frame = new mmGUIFrame(wxT("Money Manager EX"),
                                 wxPoint(valx, valy), 
                                 wxSize(valw, valh));

    frame->Show(TRUE);
    if (isMaxStr == wxT("TRUE"))
        frame->Maximize(true);

    /* Initialize Image Handlers */
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}
/*******************************************************/
mmAddAccountWizard::mmAddAccountWizard(wxFrame *frame, wxSQLite3Database* db)
         :wxWizard(frame,wxID_ANY,_("Add Account Wizard"),
                   wxBitmap(addacctwiz_xpm),wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE), db_(db), acctID_(-1)
{
    // a wizard page may be either an object of predefined class
    page1 = new wxWizardPageSimple(this);

    /* wxStaticText *text = */ new wxStaticText(page1, wxID_ANY,
             _("Money Manager models all transactions as belonging to accounts.\n\n The next pages will help you create a new account.\n\nTo help you get started, begin by making a list of all\nfinancial institutions where you hold an account.")
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


mmNewDatabaseWizard::mmNewDatabaseWizard(wxFrame *frame, wxSQLite3Database* db)
         :wxWizard(frame,wxID_ANY,_("New Database Wizard"),
                   wxBitmap(addacctwiz_xpm),wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE), db_(db)
{
    // a wizard page may be either an object of predefined class
    page1 = new wxWizardPageSimple(this);

    /* wxStaticText *text = */ new wxStaticText(page1, wxID_ANY,
             _("The next pages will help you create a new database.\n\nYour money manager database file is stored with an extension \nof .mmb. Make sure to make backups of this \nfile and to store it carefully as it contains important \nfinancial information.")
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

/*******************************************************/
mmGUIFrame::mmGUIFrame(const wxString& title, 
                       const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame*)NULL, -1, title, pos, size), 
       db_(0), inidb_(0),gotoAccountID_(-1)
{
    selectedItemData_ = 0;

    /* Set Icon for Frame */
    wxIcon icon(mainicon_xpm);
    SetIcon(icon);

    /* Setup Printer */
    printer_ = new wxHtmlEasyPrinting(wxT("Money Manager Ex"), this);
    wxString printHeaderBase = wxT("Money Manager Ex");
    printer_-> SetHeader( printHeaderBase + wxT("(@PAGENUM@/@PAGESCNT@)<hr>"), wxPAGE_ALL);

    /* Create the Controls for the frame */
    createMenu();
    createToolBar();
    createControls();

    /* Load from Settings DB */
    loadConfigFile();

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
            openFile(val, false);
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
    saveFile();
    saveConfigFile();
 
    /* Delete the GUI */
    homePanel->DestroyChildren();

    if (db_)
    {
        db_->Close();
        delete db_;
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
    mmDBWrapper::setINISettingValue(inidb_, 
        wxT("LASTFILENAME"), fileName_);

    mmSaveColorsToDatabase(inidb_);

    bool isMax = this->IsMaximized();
    wxString isMaxStr = wxT("FALSE");
    if (isMax)
        isMaxStr = wxT("TRUE");

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
    wxFileName fname(wxGetApp().argv[0]);
    wxString iniDBPath = fname.GetPath(wxPATH_GET_VOLUME)
        + MMEX_INIDB_FNAME;
    inidb_->Open(iniDBPath);
}

void mmGUIFrame::menuEnableItems(bool enable)
{
    menuBar_->FindItem(MENU_SAVE)->Enable(enable);
    menuBar_->FindItem(MENU_SAVE_AS)->Enable(enable);
    menuBar_->FindItem(MENU_EXPORT)->Enable(enable);
    menuBar_->FindItem(MENU_NEWACCT)->Enable(enable);
    menuBar_->FindItem(MENU_ACCTLIST)->Enable(enable);
    menuBar_->FindItem(MENU_ORGCATEGS)->Enable(enable);
    menuBar_->FindItem(MENU_ORGPAYEE)->Enable(enable);
    menuBar_->FindItem(MENU_IMPORT)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT_PREVIEW)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT)->Enable(enable);
    menuBar_->FindItem(MENU_OPTIONS)->Enable(enable);
    menuBar_->FindItem(MENU_BILLSDEPOSITS)->Enable(enable);
    menuBar_->FindItem(MENU_STOCKS)->Enable(enable);
    menuBar_->FindItem(MENU_CURRENCY)->Enable(enable);
    menuBar_->FindItem(MENU_BUDGETSETUPDIALOG)->Enable(enable);
    
    toolBar_->EnableTool(MENU_SAVE, enable);
    toolBar_->EnableTool(MENU_NEWACCT, enable);
    toolBar_->EnableTool(MENU_ACCTLIST, enable);
}

void mmGUIFrame::menuPrintingEnable(bool enable)
{
    menuBar_->FindItem(MENU_PRINT_PREVIEW)->Enable(enable);
    menuBar_->FindItem(MENU_PRINT)->Enable(enable);
    menuBar_->FindItem(MENU_EXPORT_HTML)->Enable(enable);
}

void mmGUIFrame::createControls()
{
    wxPanel* framePanel = new wxPanel( this, ID_FRAMEPANEL, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

    wxBoxSizer* itemBoxSizerFrame = new wxBoxSizer(wxVERTICAL);
    framePanel->SetSizer(itemBoxSizerFrame);

    wxSplitterWindow* itemSplitterWindowFrame = new wxSplitterWindow( framePanel, 
        ID_SPLITTERWINDOW1, wxDefaultPosition, wxSize(100, 100), wxNO_BORDER );
    navTreeCtrl_ = new wxTreeCtrl( itemSplitterWindowFrame, ID_NAVTREECTRL, 
        wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE | wxTR_HAS_BUTTONS | wxTR_ROW_LINES );
    navTreeCtrl_->SetBackgroundColour(mmColors::navTreeBkColor);
    
    wxSize imageSize(16, 16);
    wxImageList* imageList_ = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    imageList_->Add(wxBitmap(homepagetree_xpm));
    imageList_->Add(wxBitmap(accounttree_xpm));
    imageList_->Add(wxBitmap(billsdeposits_xpm));
    imageList_->Add(wxBitmap(budgeting_xpm));
    imageList_->Add(wxBitmap(reporttree_xpm));
    imageList_->Add(wxBitmap(help_xpm));
    imageList_->Add(wxBitmap(stock_xpm));
    imageList_->Add(wxBitmap(assets_xpm));

    navTreeCtrl_->AssignImageList(imageList_);

    homePanel = new wxPanel( itemSplitterWindowFrame, ID_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxTR_SINGLE);
    
    itemSplitterWindowFrame->SplitVertically(navTreeCtrl_, homePanel, 100);
    itemSplitterWindowFrame->SetMinimumPaneSize(150);
    itemSplitterWindowFrame->SetSashGravity(0.1);
    itemBoxSizerFrame->Add(itemSplitterWindowFrame, 1, wxGROW|wxALL, 1);
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

    wxTreeItemId stocks = navTreeCtrl_->AppendItem(root, _("Stocks"), 6, 6);
    navTreeCtrl_->SetItemData(stocks, new mmTreeItemData(wxT("Stocks")));
    navTreeCtrl_->SetItemBold(stocks, true);

    wxTreeItemId assets = navTreeCtrl_->AppendItem(root, _("Assets"), 7, 7);
    navTreeCtrl_->SetItemData(assets, new mmTreeItemData(wxT("Assets")));
    navTreeCtrl_->SetItemBold(assets, true);

    wxTreeItemId bills = navTreeCtrl_->AppendItem(root, _("Bills & Deposits"), 2, 2);
    navTreeCtrl_->SetItemData(bills, new mmTreeItemData(wxT("Bills & Deposits")));
    navTreeCtrl_->SetItemBold(bills, true);

    wxTreeItemId budgeting = navTreeCtrl_->AppendItem(root, _("Budgeting"), 3, 3);
    navTreeCtrl_->SetItemData(budgeting, new mmTreeItemData(wxT("Budgeting")));
    navTreeCtrl_->SetItemBold(budgeting, true);


    wxTreeItemId reports = navTreeCtrl_->AppendItem(root, 
        _("Reports"), 4, 4);
    navTreeCtrl_->SetItemBold(reports, true);

    wxTreeItemId reportsSummary = navTreeCtrl_->AppendItem(reports, 
        _("Summary of Accounts"), 4, 4);
    navTreeCtrl_->SetItemData(reportsSummary, 
        new mmTreeItemData(wxT("Summary of Accounts")));

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
        new mmTreeItemData(wxT("To Whom the Money Goes- 30 Days")));
    
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
   
    ///////////////////////////////////////////////////////////////////
    wxTreeItemId budgetPerformance = navTreeCtrl_->AppendItem(reports, 
        _("Budget Performance"), 4, 4);
    navTreeCtrl_->SetItemData(budgetPerformance, 
        new mmTreeItemData(wxT("Budget Performance")));

    ///////////////////////////////////////////////////////
    wxTreeItemId help = navTreeCtrl_->AppendItem(root, _("Help"), 5, 5);
    navTreeCtrl_->SetItemData(help, new mmTreeItemData(wxT("Help")));
    navTreeCtrl_->SetItemBold(help, true);

    navTreeCtrl_->Expand(root);
    navTreeCtrl_->Expand(reports);

    if (!db_)
       return;

    /* Load Nav Tree Control */
    mmBEGINSQL_LITE_EXCEPTION;

    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, wxT("VIEWACCOUNTS"), wxT("ALL"));
    wxString sqlString = wxT("select * from ACCOUNTLIST_V1 WHERE \
        ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
    if (vAccts == wxT("Open"))
        sqlString =  wxT("select * from ACCOUNTLIST_V1 WHERE \
        ACCOUNTTYPE='Checking' AND STATUS='Open' order by ACCOUNTNAME;");
    else if (vAccts == wxT("Favorites"))
        sqlString =  wxT("select * from ACCOUNTLIST_V1 WHERE \
        ACCOUNTTYPE='Checking' AND FAVORITEACCT='TRUE' order by ACCOUNTNAME;");

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sqlString);
    while (q1.NextRow())
    {
        wxTreeItemId tacct = navTreeCtrl_->AppendItem(accounts, q1.GetString(wxT("ACCOUNTNAME")), 1, 1);
        navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(q1.GetInt(wxT("ACCOUNTID")), false));
    }
    q1.Finalize();

     sqlString = wxT("select * from ACCOUNTLIST_V1 WHERE \
        ACCOUNTTYPE='Investment' order by ACCOUNTNAME;");

    wxSQLite3ResultSet q2 = db_->ExecuteQuery(sqlString);
    while (q2.NextRow())
    {
        wxTreeItemId tacct = navTreeCtrl_->AppendItem(stocks, q2.GetString(wxT("ACCOUNTNAME")), 6, 6);
        navTreeCtrl_->SetItemData(tacct, new mmTreeItemData(q2.GetInt(wxT("ACCOUNTID")), false));
    }
    q2.Finalize();


    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from BUDGETYEAR_V1 order by BUDGETYEARNAME;");
    q1 = db_->ExecuteQuery(bufSQL);
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

    mmENDSQL_LITE_EXCEPTION;

    navTreeCtrl_->Expand(accounts);
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
                mmPrintableBase* rs = new mmReportBudgetingPerformance(db_, data);
                menuPrintingEnable(true);
                createReportsPage(rs);
            }
            else
                createBudgetingPage(data);
            Thaw();
        }
        else
        {
            mmBEGINSQL_LITE_EXCEPTION;
            wxSQLite3StatementBuffer bufSQL;
            bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", data);
            wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
            if (q1.NextRow())
            {
                wxString acctType = q1.GetString(wxT("ACCOUNTTYPE"));
                q1.Finalize();
                if (acctType == wxT("Checking"))
                {
                   // Freeze();
                   gotoAccountID_ = data;
                   wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_GOTOACCOUNT);
                   GetEventHandler()->AddPendingEvent(evt);
                    //Thaw();
                }
                else
                {
				    Freeze();
					homePanel->DestroyChildren();
					homePanel->SetSizer(NULL);

					wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
					homePanel->SetSizer(itemBoxSizer1);

					panelCurrent_ = new mmStocksPanel(db_, inidb_, data, homePanel, ID_PANEL3, 
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
            mmENDSQL_LITE_EXCEPTION;
        }
    }
    else 
    {
        if (iData->getString() == wxT("Home Page"))
        {
            createHomePage();
            return;
        }

        if (!db_)
            return;

        Freeze();

        if (iData->getString() == wxT("Summary of Accounts"))
        {
            mmPrintableBase* rs = new mmReportSummary(db_);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes"))
        {
            mmPrintableBase* rs = new mmReportCategoryExpenses(db_, true, wxDateTime::Now(),
                wxDateTime::Now());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportCategoryExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportCategoryExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Where the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportCategoryExpenses(db_, false, dtBegin, dtEnd);
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
            mmPrintableBase* rs = new mmReportCategoryExpenses(db_, false, dtBegin, dtEnd);
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
            mmPrintableBase* rs = new mmReportCategoryExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        /////////////////////////////////////////////////////////////////

        if (iData->getString() == wxT("Income vs Expenses"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(db_, year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportIncomeExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportIncomeExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportIncomeExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Last Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear()-1;
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(db_, year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - Current Year"))
        {
            wxDateTime today = wxDateTime::Now();
            int year = today.GetYear();
            mmPrintableBase* rs = new mmReportIncExpensesOverTime(db_, year);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Income vs Expenses - All Time"))
        {
            mmPrintableBase* rs = new mmReportIncomeExpenses(db_, true, wxDateTime::Now(),
                wxDateTime::Now());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        /////////////////////////////////////////////////////////////////

        if (iData->getString() == wxT("To Whom the Money Goes"))
        {
            mmPrintableBase* rs = new mmReportPayeeExpenses(db_, true, wxDateTime::Now(),
                wxDateTime::Now());
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtEnd = prevMonthEnd;
            wxDateTime dtBegin = prevMonthEnd.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportPayeeExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - 30 Days"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today;
            wxDateTime dtEnd = today;
            wxDateTime dtBegin = today.Subtract(wxDateSpan::Month());
            mmPrintableBase* rs = new mmReportPayeeExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("To Whom the Money Goes - Current Month"))
        {
            wxDateTime today = wxDateTime::Now();
            wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
            wxDateTime dtBegin = prevMonthEnd;
            wxDateTime dtEnd = wxDateTime::Now();
            mmPrintableBase* rs = new mmReportPayeeExpenses(db_, false, dtBegin, dtEnd);
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
            mmPrintableBase* rs = new mmReportPayeeExpenses(db_, false, dtBegin, dtEnd);
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
            mmPrintableBase* rs = new mmReportPayeeExpenses(db_, false, dtBegin, dtEnd);
            menuPrintingEnable(true);
            createReportsPage(rs);
        }

        if (iData->getString() == wxT("Help"))
        {
            menuPrintingEnable(true);
            createHelpPage();
        }
        Thaw();

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
        mmBEGINSQL_LITE_EXCEPTION;
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", data);
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
        if (q1.NextRow())
        {
            wxString website = q1.GetString(wxT("WEBSITE"));
            if (!website.IsEmpty())
            {
                  //wxExecute(_T("explorer ") + website, wxEXEC_ASYNC, NULL ); 
                  wxLaunchDefaultBrowser(website);  
            }
            return;
        }
        else
        {
            /* cannot find accountid */
            wxASSERT(true);
        }
        mmENDSQL_LITE_EXCEPTION;
    }
}

void mmGUIFrame::OnPopupEditAccount(wxCommandEvent& event)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        mmBEGINSQL_LITE_EXCEPTION;
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", data);
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
        if (q1.NextRow())
        {
            wxString acctType = q1.GetString(wxT("ACCOUNTTYPE"));
            if (acctType == wxT("Checking") || acctType == wxT("Investment"))
            {
                q1.Finalize();
                mmNewAcctDialog *dlg = new mmNewAcctDialog(db_, false, data, this);
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
        mmENDSQL_LITE_EXCEPTION;
    }
}

void mmGUIFrame::OnPopupDeleteAccount(wxCommandEvent& event)
{
    if (selectedItemData_)
    {
        int data = selectedItemData_->getData();
        mmBEGINSQL_LITE_EXCEPTION;
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", data);
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
        if (q1.NextRow())
        {
            wxString acctType = q1.GetString(wxT("ACCOUNTTYPE"));
            if (acctType == wxT("Checking"))
            {
                wxMessageDialog msgDlg(this, 
                    _("Do you really want to delete the account?"),
                    _("Confirm Account Deletion"),
                    wxYES_NO);
                if (msgDlg.ShowModal() == wxID_YES)
                {
                    // delete the account
                    q1.Finalize();

                    bufSQL.Format("delete from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", data, data);
                    int nTransDeleted = db_->ExecuteUpdate(bufSQL);

                    bufSQL.Format("delete from ACCOUNTLIST_V1 where ACCOUNTID=%d;", data);
                    int nRows = db_->ExecuteUpdate(bufSQL);
                    wxASSERT(nRows);

                    updateNavTreeControl();
                    createHomePage();
                } 
            }
            else if (acctType == wxT("Investment"))
            {
                wxMessageDialog msgDlg(this, 
                    _("Do you really want to delete the account?"),
                    _("Confirm Account Deletion"),
                    wxYES_NO);
                if (msgDlg.ShowModal() == wxID_YES)
                {
                    // delete the account
                    q1.Finalize();

                    bufSQL.Format("delete from STOCK_V1 where HELDAT=%d;", data);
                    int nTransDeleted = db_->ExecuteUpdate(bufSQL);

                    bufSQL.Format("delete from ACCOUNTLIST_V1 where ACCOUNTID=%d;", data);
                    int nRows = db_->ExecuteUpdate(bufSQL);
                    wxASSERT(nRows);

                    updateNavTreeControl();
                    createHomePage();
                } 
            }
        }
        else
        {
            /* cannot find accountid */
            wxASSERT(true);
        }
        mmENDSQL_LITE_EXCEPTION;
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
            mmBEGINSQL_LITE_EXCEPTION;
            wxSQLite3StatementBuffer bufSQL;
            bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", data);
            wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
            if (q1.NextRow())
            {
                wxString acctType = q1.GetString(wxT("ACCOUNTTYPE"));
                q1.Finalize();
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
            mmENDSQL_LITE_EXCEPTION;
        }
    }
}


void mmGUIFrame::createCheckingAccountPage(int accountID)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmCheckingPanel(db_, inidb_, accountID, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::OnGotoAccount(wxCommandEvent& WXUNUSED(event))
{
    if (gotoAccountID_ != -1)    
    {
        createCheckingAccountPage(gotoAccountID_);
        //navTreeCtrl_->Unselect();
    }
}

void mmGUIFrame::createBudgetingPage(int budgetYearID)
{
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);

    panelCurrent_ = new mmBudgetingPanel(db_, inidb_, budgetYearID, homePanel, ID_PANEL3, 
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
    
    panelCurrent_ = new mmHomePagePanel(this, db_, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);
     
    homePanel->Layout();
}

void mmGUIFrame::createReportsPage(mmPrintableBase* rs)
{
     homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
       
    panelCurrent_ = new mmReportsPanel(this, db_, rs, homePanel, ID_PANEL3, 
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
       
    panelCurrent_ = new mmHelpPanel(this, db_, homePanel, ID_PANEL3, 
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
    toolBarBitmaps[4] = wxBitmap(listview_xpm);
    toolBarBitmaps[5] = wxBitmap(print_xpm);
    toolBarBitmaps[6] = wxBitmap(printpreview_xpm);
    toolBarBitmaps[7] = wxBitmap(printsetup_xpm);
    toolBarBitmaps[8] = wxBitmap(edit_account_xpm);
    toolBarBitmaps[9] = wxBitmap(delete_account_xpm);
    

    wxMenu *menuFile = new wxMenu;
	wxMenuItem* menuItemNew = new wxMenuItem(menuFile, MENU_NEW, 
		_("&New Database\tCtrl-N"), _("New Money Manager Database"));
	menuItemNew->SetBitmaps(toolBarBitmaps[0]);

	wxMenuItem* menuItemOpen = new wxMenuItem(menuFile, MENU_OPEN, 
		_("&Open Database\tCtrl-O"), _("Open Money Manager Database"));
	menuItemOpen->SetBitmaps(toolBarBitmaps[1]);

	wxMenuItem* menuItemSave = new wxMenuItem(menuFile, MENU_SAVE, 
		_("&Save Database\tCtrl-S"), _("Save Money Manager Database"));
	menuItemSave->SetBitmaps(toolBarBitmaps[2]);
	
	menuFile->Append(menuItemNew);
    menuFile->Append(menuItemOpen);
	menuFile->Append(menuItemSave);

    wxMenuItem* menuItemSaveAs = new wxMenuItem(menuFile, MENU_SAVE_AS, 
		 _("Save &As"), _("Save Money Manager Database As"));
	menuItemSaveAs->SetBitmaps(wxBitmap(saveas_xpm));
    menuFile->Append(menuItemSaveAs);

    menuFile->AppendSeparator();
    
    wxMenu* exportMenu = new wxMenu;
    exportMenu->Append(MENU_EXPORT_CSV, _("&CSV Files"), _("Export to CSV"));
    exportMenu->Append(MENU_EXPORT_HTML, _("&Report to HTML"), _("Export to HTML"));
    menuFile->Append(MENU_EXPORT, _("&Export"), exportMenu);

    wxMenu* importMenu = new wxMenu;
    importMenu->Append(MENU_IMPORT_QIF, _("&QIF Files"), _("Import from QIF"));
    importMenu->Append(MENU_IMPORT_CSV, _("&CSV Files"), _("Import from CSV"));
    importMenu->Append(MENU_IMPORT_MMNETCSV, _("&MM.NET CSV Files"), _("Import from CSV"));
    menuFile->Append(MENU_IMPORT, _("Import"), importMenu);

    menuFile->AppendSeparator();

    
	wxMenuItem* menuItemPrintSetup = new wxMenuItem(menuFile, MENU_PRINT_PAGE_SETUP, 
	   _("Page Setup..."), _("Setup page printing options"));
	menuItemPrintSetup->SetBitmaps(toolBarBitmaps[7]);
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
	menuItemQuit->SetBitmaps(wxBitmap(exit_xpm));
    menuFile->Append(menuItemQuit);
   
    wxMenu *menuAccounts = new wxMenu;

	wxMenuItem* menuItemNewAcct = new wxMenuItem(menuAccounts, MENU_NEWACCT, 
		_("New &Account"), _("New Money Manager Account"));
	menuItemNewAcct->SetBitmaps(toolBarBitmaps[3]);

	wxMenuItem* menuItemAcctList = new wxMenuItem(menuAccounts, MENU_ACCTLIST, 
		_("Account &List"), _("Show Account List"));
	menuItemAcctList->SetBitmaps(toolBarBitmaps[4]);

    wxMenuItem* menuItemAcctEdit = new wxMenuItem(menuAccounts, MENU_ACCTEDIT, 
		_("Edit Account"), _("Edit Account"));
	menuItemAcctEdit->SetBitmaps(toolBarBitmaps[8]);

    wxMenuItem* menuItemAcctDelete = new wxMenuItem(menuAccounts, MENU_ACCTDELETE, 
		_("Delete Account"), _("Delete Account from database"));
	menuItemAcctDelete->SetBitmaps(toolBarBitmaps[9]);

    menuAccounts->Append(menuItemNewAcct); 
    menuAccounts->Append(menuItemAcctList); 
    menuAccounts->Append(menuItemAcctEdit); 
    menuAccounts->Append(menuItemAcctDelete); 

    wxMenu *menuTools = new wxMenu;

    
    wxMenuItem* menuItemCateg = new wxMenuItem(menuTools, MENU_ORGCATEGS, 
		  _("Organize &Categories"), _("Organize Categories"));
	menuItemCateg->SetBitmaps(wxBitmap(categories_xpm));
    menuTools->Append(menuItemCateg);

    wxMenuItem* menuItemPayee = new wxMenuItem(menuTools, MENU_ORGPAYEE, 
		  _("Organize &Payees"), _("Organize Payees"));
	menuItemPayee->SetBitmaps(wxBitmap(payees_xpm));
    menuTools->Append(menuItemPayee); 

     wxMenuItem* menuItemCurrency = new wxMenuItem(menuTools, MENU_CURRENCY, 
		 _("Organize Currency"), _("Organize Currency"));
	menuItemCurrency->SetBitmaps(wxBitmap(currency_xpm));
    menuTools->Append(menuItemCurrency);

    wxMenuItem* menuItemBudgeting = new wxMenuItem(menuTools, MENU_BUDGETSETUPDIALOG, 
		  _("Budget Setup"), _("Budget Setup"));
	menuItemBudgeting->SetBitmaps(wxBitmap(budgeting_xpm));
    menuTools->Append(menuItemBudgeting); 

    wxMenuItem* menuItemBillsDeposits = new wxMenuItem(menuTools, MENU_BILLSDEPOSITS, 
		 _("Bills && Deposits"), _("Bills && Deposits"));
	menuItemBillsDeposits->SetBitmaps(wxBitmap(billsdeposits_xpm));
    menuTools->Append(menuItemBillsDeposits); 

    wxMenuItem* menuItemStocks = new wxMenuItem(menuTools, MENU_STOCKS, 
		_("Stock Investments"), _("Stock Investments"));
	menuItemStocks->SetBitmaps(wxBitmap(stock_xpm));
    menuTools->Append(menuItemStocks);

    wxMenuItem* menuItemAssets = new wxMenuItem(menuTools, MENU_ASSETS, 
		_("Assets"), _("Assets"));
	menuItemAssets->SetBitmaps(wxBitmap(assets_xpm));
    menuTools->Append(menuItemAssets);

    menuTools->AppendSeparator();

    wxMenuItem* menuItemOptions = new wxMenuItem(menuTools, MENU_OPTIONS, 
		  _("&Options"), _("Money Manager Options"));
	menuItemOptions->SetBitmaps(wxBitmap(options_xpm));
    menuTools->Append(menuItemOptions);

    wxMenu *menuHelp = new wxMenu;

    wxMenuItem* menuItemHelp = new wxMenuItem(menuTools, MENU_HELP, 
		 _("&Help\tCtrl-F1"), _("Money Manager Help"));
	menuItemHelp->SetBitmaps(wxBitmap(help_xpm));
    menuHelp->Append(menuItemHelp);

    wxMenuItem* menuItemAppStart = new wxMenuItem(menuTools, MENU_SHOW_APPSTART, 
		 _("Show App Start Dialog"), _("App Start Dialog"));
	menuItemAppStart->SetBitmaps(wxBitmap(appstart_xpm));
    menuHelp->Append(menuItemAppStart);

    menuHelp->AppendSeparator();


    wxMenuItem* menuItemCheck = new wxMenuItem(menuTools, MENU_CHECKUPDATE, 
		 _("Check for &Updates"), _("Check For Updates"));
	menuItemCheck->SetBitmaps(wxBitmap(checkupdate_xpm));
    menuHelp->Append(menuItemCheck);

    wxMenuItem* menuItemReportIssues = new wxMenuItem(menuTools, MENU_REPORTISSUES, 
	    _("Report Issues or Feedback"), _("Send email through the mailing list to report issues with the software."));
	menuItemReportIssues->SetBitmaps(wxBitmap(issues_xpm));
    menuHelp->Append(menuItemReportIssues);

    wxMenuItem* menuItemNotify = new wxMenuItem(menuTools, MENU_ANNOUNCEMENTMAILING, 
		_("Be notified of new releases"), _("Sign up for the announcement mailing list"));
	menuItemNotify->SetBitmaps(wxBitmap(notify_xpm));
    menuHelp->Append(menuItemNotify); 
        

    wxMenuItem* menuItemAbout = new wxMenuItem(menuTools, MENU_ABOUT, 
	   _("&About..."), _("Show about dialog"));
	menuItemAbout->SetBitmaps(wxBitmap(about_xpm));
    menuHelp->Append(menuItemAbout);
    
    menuBar_ = new wxMenuBar;
    menuBar_->Append(menuFile, _("&File"));
    menuBar_->Append(menuAccounts, _("&Accounts"));
    menuBar_->Append(menuTools, _("&Tools"));
    menuBar_->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar_);
}

void mmGUIFrame::createToolBar()
{
    toolBar_ = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL, TOOLBAR_MAIN );
    wxBitmap toolBarBitmaps[8];
    toolBarBitmaps[0] = wxBitmap(new_xpm);
    toolBarBitmaps[1] = wxBitmap(open_xpm);
    toolBarBitmaps[2] = wxBitmap(save_xpm);
    toolBarBitmaps[3] = wxBitmap(newacct_xpm);
    toolBarBitmaps[4] = wxBitmap(listview_xpm);

    toolBar_->AddTool(MENU_NEW, _("New"), toolBarBitmaps[0], _("New Money Manager Database"));
    toolBar_->AddTool(MENU_OPEN, _("Open"), toolBarBitmaps[1], _("Open Money Manager Database"));
    toolBar_->AddTool(MENU_SAVE, _("Save"), toolBarBitmaps[2], _("Save Money Manager Database"));
    toolBar_->AddTool(MENU_NEWACCT, _("New Account"), toolBarBitmaps[3], _("New Money Manager Account"));
    toolBar_->AddTool(MENU_ACCTLIST, _("Account List"), toolBarBitmaps[4], _("Show Account List"));
    
    // after adding the buttons to the toolbar, must call Realize() to reflect changes
    toolBar_->Realize();
    SetToolBar(toolBar_);
}

void mmGUIFrame::createDataStore(const wxString& fileName, bool openingNew)
{
    if (db_)
    {
        db_->Close();
        delete db_;
        db_ = 0;
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

        db_ = new wxSQLite3Database();
        db_->Open(fileName);

        // we need to check the db whether it is the right version
        if (!mmDBWrapper::checkDBVersion(db_))
        {
            this->SetTitle(_("Money Manager EX - No File opened "));   
            mmShowErrorMessage(this, 
                    _("Sorry. The Database version is too old. \
                    Cannot open in this version of Money Manager Ex."), 
                    _("Error opening database"));

            db_->Close();
            delete db_;

            db_ = 0;
            return ;
        }
    }
    else if (openingNew) // New Database
    {
        db_ = new wxSQLite3Database();
        db_->Open(fileName);

        openDataBase(fileName);

        mmNewDatabaseWizard* wizard = new mmNewDatabaseWizard(this, db_);
        wizard->RunIt(true);

        mmDBWrapper::loadBaseCurrencySettings(db_);

        /* Jump to new account creation screen */
        wxCommandEvent evt;
        OnNewAccount(evt);
        return;
    }
    else // open of existing database failed
    {
        this->SetTitle(_("Money Manager EX - No File opened "));   
        
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
    wxFileName fname(wxGetApp().argv[0]);
    wxString appPath = fname.GetPath(wxPATH_GET_VOLUME);

    wxProgressDialog* pgd = new wxProgressDialog(_("Please Wait"), 
        _("Opening Database File && Verifying Integrity"), 100, this, 
        wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH );
    mmDBWrapper::initDB(db_, pgd, appPath);
    pgd->Update(100);
    pgd->Destroy();

    wxFileName fName(fileName);
#if 0
    wxString title = wxT("Money Manager EX : ") 
        + fName.GetName() + wxT(".") + fName.GetExt();
#endif
    wxString title = wxT("Money Manager EX : ") 
        + fileName;
    this->SetTitle(title);

    if (db_)
        fileName_ = fileName;
    else
        fileName_ = wxT("");
}

wxPanel* mmGUIFrame::createMainFrame(wxPanel* parent)
{
       return 0;
}

void mmGUIFrame::saveFile(void)
{
    
}

void mmGUIFrame::openFile(const wxString& fileName, bool openingNew)
{
    createDataStore(fileName, openingNew);
  
    if (db_)
    {
        menuEnableItems(true);
        menuPrintingEnable(false);
    }
    createHomePage();
    //panelCurrent_->init(db_);
    updateNavTreeControl();

    if (!db_)
    {
        mmDBWrapper::setINISettingValue(inidb_, 
            wxT("LASTFILENAME"), wxT(""));
        showBeginAppDialog();
    }
}

void mmGUIFrame::OnNew(wxCommandEvent& event)
{
    wxString fileName = wxFileSelector(wxT("Choose Money Manager Ex data file to create"), 
                wxT(""), wxT(""), wxT(""), wxT("*.mmb"), wxSAVE | wxOVERWRITE_PROMPT);
    if ( !fileName.empty() )
    {
       openFile(fileName, true);
    }
}

void mmGUIFrame::OnOpen(wxCommandEvent& event)
{
    wxString fileName = wxFileSelector(wxT("Choose Money Manager Ex data file to open"), 
                wxT(""), wxT(""), wxT(""), wxT("*.mmb"), wxFILE_MUST_EXIST);
    if ( !fileName.empty() )
    {
        openFile(fileName, false);
    }
}

void mmGUIFrame::OnSave(wxCommandEvent& event)
{
  saveFile();
}

void mmGUIFrame::OnSaveAs(wxCommandEvent& event)
{
    wxString fileName = wxFileSelector(wxT("Choose Money Manager Ex data file to Save As"), 
                wxT(""), wxT(""), wxT(""), wxT("*.mmb"), wxSAVE | wxOVERWRITE_PROMPT);
    if ( !fileName.empty() )
    {
        if (db_)
        {
            db_->Close();
            delete db_;
            db_ = 0;
        }
        wxCopyFile(fileName_, fileName, false);
        openFile(fileName, false);
    }
}

void mmGUIFrame::OnExport(wxCommandEvent& event)
{
   mmExportCSV(db_);
}

void mmGUIFrame::OnImportCSV(wxCommandEvent& event)
{
    int accountID = mmImportCSV(db_);
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}

void mmGUIFrame::OnImportQIF(wxCommandEvent& event)
{
    int accountID = mmImportQIF(db_);
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}

void mmGUIFrame::OnImportCSVMMNET(wxCommandEvent& event)
{
    int accountID = mmImportCSVMMNET(db_);
    if (accountID != -1)
        createCheckingAccountPage(accountID);
}

void mmGUIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}
    
void mmGUIFrame::OnNewAccount(wxCommandEvent& event)
{
    mmAddAccountWizard* wizard = new mmAddAccountWizard(this, db_);
    wizard->RunIt(true);

    if (wizard->acctID_ != -1)
    {
        mmNewAcctDialog *dlg = new mmNewAcctDialog(db_, false, wizard->acctID_, this);
        if ( dlg->ShowModal() == wxID_OK )
        {
            
        }
        updateNavTreeControl();    
        dlg->Destroy();
    }
    
  
    /* Currency Options might have changed so refresh */
    createHomePage();
}
    
void mmGUIFrame::OnAccountList(wxCommandEvent& event)
{
    createHomePage();
}

void mmGUIFrame::OnOrgCategories(wxCommandEvent& event)
{
    mmCategDialog *dlg = new mmCategDialog(db_, this);
    dlg->ShowModal();
    dlg->Destroy();
}
 
void mmGUIFrame::OnOrgPayees(wxCommandEvent& event)
{
    mmPayeeDialog *dlg = new mmPayeeDialog(db_, true, this);
    dlg->ShowModal();
    dlg->Destroy();
}

void mmGUIFrame::OnBudgetSetupDialog(wxCommandEvent& event)
{
     if (!db_)
       return;

    mmBudgetYearDialog *dlg = new mmBudgetYearDialog(db_, this);
    dlg->ShowModal();
    createHomePage();
    updateNavTreeControl();    
    dlg->Destroy();
}

void mmGUIFrame::OnOptions(wxCommandEvent& event)
{
    if (!db_ || !inidb_)
        return;

    mmOptionsDialog *dlg = new mmOptionsDialog(db_, inidb_, this);
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
    wxString url = wxT("http://sourceforge.net/project/showfiles.php?group_id=163169");
    //wxExecute(_T("explorer ") + url, wxEXEC_ASYNC, NULL );
    wxLaunchDefaultBrowser(url);
}

void mmGUIFrame::OnReportIssues(wxCommandEvent& event)
{
   wxString url = wxT("http://groups.google.com/group/zealsupport");
   //wxExecute(_T("explorer ") + url, wxEXEC_ASYNC, NULL ); 
   wxLaunchDefaultBrowser(url);
}

void mmGUIFrame::OnBeNotified(wxCommandEvent& event)
{
    wxString url = wxT("http://groups.google.com/group/mmlist");
    //wxExecute(_T("explorer ") + url, wxEXEC_ASYNC, NULL );
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
            openFile(val, false);
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
    //navTreeCtrl_->Unselect();
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmBillsDepositsPanel(db_, inidb_, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::OnStocks(wxCommandEvent& event)
{
    //navTreeCtrl_->Unselect();
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmStocksPanel(db_, inidb_, -1, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::OnAssets(wxCommandEvent& event)
{
    //navTreeCtrl_->Unselect();
    homePanel->DestroyChildren();
    homePanel->SetSizer(NULL);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    homePanel->SetSizer(itemBoxSizer1);
    
    panelCurrent_ = new mmAssetsPanel(db_, inidb_, homePanel, ID_PANEL3, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer1->Add(panelCurrent_, 1, wxGROW|wxALL, 1);

    homePanel->Layout();
}

void mmGUIFrame::OnCurrency(wxCommandEvent& event)
{
    mmCurrencyDialog *dlg = new mmCurrencyDialog(db_,this);
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
    wxUint32 num = mmDBWrapper::getNumAccounts(db_);
    if (num == 0)
    {
        mmShowErrorMessage(0, _("No Account available!"), _("Error"));
        return;
    }
    wxArrayString as;
    int* arrAcctID = new int[num];
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3ResultSet q1 = 
        db_->ExecuteQuery("select * from ACCOUNTLIST_V1 order by ACCOUNTNAME;");
    int i = 0;
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
        arrAcctID[i++] = q1.GetInt(wxT("ACCOUNTID"));
    }

    mmENDSQL_LITE_EXCEPTION;

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), DEFDELIMTER);
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, 
        _("Choose Account to Edit:"), 
        _("Accounts"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        int choice = scd->GetSelection();
        int acctID = arrAcctID[choice];
        mmNewAcctDialog *dlg = new mmNewAcctDialog(db_, false, acctID, this);
        if ( dlg->ShowModal() == wxID_OK )
        {
            createHomePage();
            updateNavTreeControl();      
        }
    }
    delete[] arrAcctID;
}

void mmGUIFrame::OnDeleteAccount(wxCommandEvent& event)
{
    wxUint32 num = mmDBWrapper::getNumAccounts(db_);
    if (num == 0)
    {
        mmShowErrorMessage(0, _("No Account available!"), _("Error"));
        return;
    }
    wxArrayString as;
    int* arrAcctID = new int[num];
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3ResultSet q1 = 
        db_->ExecuteQuery("select * from ACCOUNTLIST_V1 order by ACCOUNTNAME;");
    int i = 0;
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
        arrAcctID[i++] = q1.GetInt(wxT("ACCOUNTID"));
    }
    
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
  

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), DEFDELIMTER);
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, 
        _("Choose Account to Edit:"), 
        _("Accounts"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        int choice = scd->GetSelection();
        int acctID = arrAcctID[choice];
        wxString acctType = mmDBWrapper::getAccountType(db_, acctID);

        wxMessageDialog msgDlg(this, 
            _("Do you really want to delete the account?"),
            _("Confirm Account Deletion"),
            wxYES_NO);
        if (msgDlg.ShowModal() == wxID_YES)
        {
            if (acctType = wxT("Checking"))
            {
                wxSQLite3StatementBuffer bufSQL;
                bufSQL.Format("delete from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", acctID, acctID);
                int nTransDeleted = db_->ExecuteUpdate(bufSQL);

                bufSQL.Format("delete from ACCOUNTLIST_V1 where ACCOUNTID=%d;", acctID);
                int nRows = db_->ExecuteUpdate(bufSQL);
                wxASSERT(nRows);

                updateNavTreeControl();
                createHomePage();
            }
            else if (acctType == wxT("Investment"))
            {
                wxSQLite3StatementBuffer bufSQL;
                bufSQL.Format("delete from STOCK_V1 where HELDAT=%d;", acctID);
                int nTransDeleted = db_->ExecuteUpdate(bufSQL);

                bufSQL.Format("delete from ACCOUNTLIST_V1 where ACCOUNTID=%d;", acctID);
                int nRows = db_->ExecuteUpdate(bufSQL);
                wxASSERT(nRows);

                updateNavTreeControl();
                createHomePage();
            }

        }
         
    }
    delete[] arrAcctID;
  
}
    
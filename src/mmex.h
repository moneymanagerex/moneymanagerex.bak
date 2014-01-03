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

//----------------------------------------------------------------------------
#ifndef _MM_EX_MMEX_H_
#define _MM_EX_MMEX_H_
//----------------------------------------------------------------------------
#include <wx/app.h>
#include <wx/aui/aui.h>
#include <wx/debugrpt.h>
#include <wx/toolbar.h>

enum
{
    MENU_BILLSDEPOSITS = wxID_HIGHEST +1,
    MENU_STOCKS,
    MENU_GOTOACCOUNT,
    MENU_ASSETS,

    MENU_VIEW_BANKACCOUNTS,
    MENU_VIEW_TERMACCOUNTS,
    MENU_VIEW_STOCKACCOUNTS,
    MENU_VIEW_BUDGET_FINANCIAL_YEARS,
    MENU_VIEW_BUDGET_SETUP_SUMMARY,
    MENU_VIEW_BUDGET_CATEGORY_SUMMARY,
    MENU_VIEW_BUDGET_TRANSFER_TOTAL,
    ID_MMEX_MAX,
};
//----------------------------------------------------------------------------

#include "util.h"
#include "mmOption.h"
//----------------------------------------------------------------------------
class wxSQLite3Database;
//----------------------------------------------------------------------------
class mmPrintableBase;
class mmPanelBase;
class mmTreeItemData;
class mmCheckingPanel;
class mmBudgetingPanel;
class CustomReportIndex;
class RecentDatabaseFiles;
//----------------------------------------------------------------------------

class mmGUIFrame;
class mmGUIApp : public wxApp
{
public:
    mmGUIApp();

    wxLocale& getLocale();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
public:
    mmGUIFrame* m_frame;
    wxSQLite3Database* m_setting_db;
    wxString m_optParam;
private:
    void reportFatalException(wxDebugReport::Context);
private:
    wxLocale m_locale; // locale we'll be using

    bool OnInit();
    int OnExit();
    void OnFatalException(); // called when a crash occurs in this application
    void HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const;
};

//----------------------------------------------------------------------------
DECLARE_APP(mmGUIApp)
//----------------------------------------------------------------------------

class mmGUIFrame : public wxFrame
{
public:
    mmGUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~mmGUIFrame();

    void setGotoAccountID(int account_id, long transID = -1);
    void setHomePageActive(bool active = true);
    bool expandedBankAccounts()
    {
        return menuBar_->IsChecked(MENU_VIEW_BANKACCOUNTS);
    }
    bool expandedTermAccounts()
    {
        return menuBar_->IsChecked(MENU_VIEW_TERMACCOUNTS);
    }
    bool expandedStockAccounts()
    {
        return menuBar_->IsChecked(MENU_VIEW_STOCKACCOUNTS);
    }
    bool financialYearIsDifferent()
    {
        return (mmOptions::instance().financialYearStartDayString_   != "1" ||
                mmOptions::instance().financialYearStartMonthString_ != "1");
    }
    bool budgetFinancialYears()
    {
        return menuBar_->IsChecked(MENU_VIEW_BUDGET_FINANCIAL_YEARS);
    }
    bool budgetSetupWithSummary()
    {
        return !menuBar_->IsChecked(MENU_VIEW_BUDGET_SETUP_SUMMARY);
    }
    bool budgetCategoryTotal()
    {
        return menuBar_->IsChecked(MENU_VIEW_BUDGET_CATEGORY_SUMMARY);
    }
    bool budgetTransferTotal()
    {
        return menuBar_->IsChecked(MENU_VIEW_BUDGET_TRANSFER_TOTAL);
    }
    /// return the index (mmex::EDocFile) to return the correct file.
    int getHelpFileIndex() const
    {
        return helpFileIndex_;
    }

    void setAccountNavTreeSection(const wxString& accountName);
    bool setNavTreeSection( wxString sectionName);
    void SetCheckingAccountPageInactive();
    void SetBudgetingPageInactive();
    void menuPrintingEnable(bool enable);

private:
    /* handles to SQLite Database */
    wxSharedPtr<wxSQLite3Database> m_db;

    /* Currently open file name */
    wxString fileName_;
    wxString password_;

    int gotoAccountID_;
    int gotoTransID_;
    bool homePageAccountSelect_;

    /* Cannot process home page recursively */
    bool refreshRequested_;
    /* trigger to avaid double initialization for HomePage on startup*/
    bool initHomePage_;
    /* Update home page details only if it is being displayed */
    bool activeHomePage_;

    /* Repeat Transactions automatic processing delay */
    wxTimer autoRepeatTransactionsTimer_;
    void OnAutoRepeatTransactionsTimer(wxTimerEvent& event);

    /* controls */
    mmPanelBase* panelCurrent_;
    wxPanel* homePanel_;
    wxTreeCtrl* navTreeCtrl_;
    wxMenuBar *menuBar_;
    wxToolBar* toolBar_;

    mmTreeItemData* selectedItemData_;

    wxTreeItemId getTreeItemfor(wxTreeItemId itemID, const wxString& accountName) const;
    bool setAccountInSection(const wxString& sectionName, const wxString& accountName);

    /* printing */
    wxScopedPtr<wxHtmlEasyPrinting> printer_;
    void restorePrinterValues();
    int helpFileIndex_;

    /* wxAUI */
    wxAuiManager m_mgr;
    wxString m_perspective;

    /* Homepage panel logic */

    void cleanup();
    void cleanupNavTreeControl(wxTreeItemId& item);
    wxSizer* cleanupHomePanel(bool new_sizer = true);
    bool openFile(const wxString& fileName, bool openingNew, const wxString &password = "");
    void InitializeModelTables();
    bool createDataStore(const wxString& fileName, const wxString &passwd, bool openingNew);
    void createMenu();
    void CreateToolBar();
    void createHomePage();
    void createReportsPage(mmPrintableBase* rb, bool cleanup);
    void createHelpPage();
    void refreshPanelData(bool catUpdate = true);

    mmCheckingPanel* checkingAccountPage_;
    bool activeCheckingAccountPage_;
    void createCheckingAccountPage(int accountID);

    void createStocksAccountPage(int accountID);

    mmBudgetingPanel* budgetingPage_;
    bool activeBudgetingPage_;
    void createBudgetingPage(int budgetYearID);

    void createControls();
    void saveSettings();
    void menuEnableItems(bool enable);
    void updateNavTreeControl(bool expandTermAccounts = false);
    void showTreePopupMenu(wxTreeItemId id, const wxPoint& pt);
    void showBeginAppDialog(bool fromScratch = false);
    void SetDataBaseParameters(const wxString& fileName);
    void OnLaunchAccountWebsite(wxCommandEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnConvertEncryptedDB(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExportToCSV(wxCommandEvent& event);
    void OnExportToQIF(wxCommandEvent& event);
    void OnExportToHtml(wxCommandEvent& event);
    void OnImportQFX(wxCommandEvent& event);
    void OnImportUniversalCSV(wxCommandEvent& event);
    void OnImportQIF(wxCommandEvent& event);
    void OnPrintPageSetup(wxCommandEvent& WXUNUSED(event));
    void OnPrintPageReport(wxCommandEvent& WXUNUSED(event));
    void OnPrintPagePreview(wxCommandEvent& WXUNUSED(event));
    void OnPrintPage(bool preview);
    void OnQuit(wxCommandEvent& event);
    void OnBillsDeposits(wxCommandEvent& event);
    void OnAssets(wxCommandEvent& event);
    void OnGotoAccount(wxCommandEvent& WXUNUSED(event));
    void OnGotoStocksAccount(wxCommandEvent& WXUNUSED(event));
    void OnViewToolbar(wxCommandEvent &event);
    void OnViewStatusbar(wxCommandEvent &event);
    void OnViewLinks(wxCommandEvent &event);
    void OnViewBankAccounts(wxCommandEvent &event);
    void OnViewTermAccounts(wxCommandEvent &event);
    void OnViewStockAccounts(wxCommandEvent &event);
    void OnViewToolbarUpdateUI(wxUpdateUIEvent &event);
    void OnViewStatusbarUpdateUI(wxUpdateUIEvent &event);
    void OnViewLinksUpdateUI(wxUpdateUIEvent &event);
    void OnNewAccount(wxCommandEvent& event);
    void OnAccountList(wxCommandEvent& event);
    void OnEditAccount(wxCommandEvent& event);
    void OnDeleteAccount(wxCommandEvent& event);

    // new item - may be taken out in future
    void OnIgnoreFutureTransactions(wxCommandEvent &event);

    void OnOrgCategories(wxCommandEvent& event);
    void OnOrgPayees(wxCommandEvent& event);
    void OnCategoryRelocation(wxCommandEvent& event);
    void OnPayeeRelocation(wxCommandEvent& event);
    void OnNewTransaction(wxCommandEvent& event);

    void OnOptions(wxCommandEvent& event);
    void OnBudgetSetupDialog(wxCommandEvent& event);
    void OnCurrency(wxCommandEvent& event);
    void OnTransactionReport(wxCommandEvent& event);
    void OnGeneralReportManager(wxCommandEvent& event);

    void OnHelp(wxCommandEvent& event);
    void OnShowAppStartDialog(wxCommandEvent& WXUNUSED(event));
    void OnCheckUpdate(wxCommandEvent& event);
    void OnReportIssues(wxCommandEvent& event);
    void OnBeNotified(wxCommandEvent& event);
    void OnFacebook(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnItemRightClick(wxTreeEvent& event);
    void OnItemMenu(wxTreeEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnPopupDeleteAccount(wxCommandEvent& event);
    void OnPopupEditAccount(wxCommandEvent& event);

    void OnViewAllAccounts(wxCommandEvent& event);
    void OnViewFavoriteAccounts(wxCommandEvent& event);
    void OnViewOpenAccounts(wxCommandEvent& event);

    bool expandedBudgetingNavTree_;
    bool expandedReportNavTree_;

    void OnTreeItemExpanded(wxTreeEvent& event);
    void OnTreeItemCollapsed(wxTreeEvent& event);

    void OnEditCustomSqlReport(wxCommandEvent& event);

    bool IsUpdateAvailable(const wxString& page);
    void processPendingEvents();

    /* Recent Files */
    RecentDatabaseFiles* recentFiles_;
    wxMenu *menuRecentFiles_;

    void OnRecentFiles(wxCommandEvent& event);
    void OnClearRecentFiles(wxCommandEvent& /*event*/);

    /** Sets the database to the new database selected by the user */
    void SetDatabaseFile(const wxString& dbFileName, bool newDatabase = false);
    /// Make a dated copy of the database. Keep only the last 4 copies.
    void BackupDatabase(const wxString& filename, bool updateRequired = false);
    
    // Required to prevent memory leaks.
    CommitCallbackHook* m_commit_callback_hook;
    void ShutdownDatabase();

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------
#endif // _MM_EX_MMEX_H_
//----------------------------------------------------------------------------

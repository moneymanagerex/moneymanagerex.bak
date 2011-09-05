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
//----------------------------------------------------------------------------
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------
class wxWizardEvent;
class wxSQLite3Database;
class wxTreeCtrl;
class wxToolBar;
class wxHtmlEasyPrinting;
class wxTreeItemId;
class wxTreeEvent;
//----------------------------------------------------------------------------
class mmCoreDB;
class mmPrintableBase;
class mmPanelBase;
class mmTreeItemData;
class customSQLReportIndex;
//----------------------------------------------------------------------------

struct CategInfo
{
    wxString categ;
    wxString amountStr;
    double   amount;
};
//----------------------------------------------------------------------------

class mmGUIApp : public wxApp
{
public:
    mmGUIApp();

    wxLocale& getLocale() { return m_locale; }

private:

    struct SQLiteInit {
        SQLiteInit();
       ~SQLiteInit();
    };

    SQLiteInit m_sqlite_init;

    wxLocale m_locale; // locale we'll be using

    bool OnInit();
    void OnFatalException(); // called when a crash occurs in this application
    void HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const;
};
//----------------------------------------------------------------------------
DECLARE_APP(mmGUIApp)
//----------------------------------------------------------------------------

class mmGUIFrame : public wxFrame
{
public:
    mmGUIFrame(const wxString& title, 
               const wxPoint& pos, 
               const wxSize& size);

    ~mmGUIFrame();

    void OnWizardCancel(wxWizardEvent& event);

    void setGotoAccountID(int account_id) { gotoAccountID_ = account_id; }
    void setHomePageActive(bool active = true);
    bool expandedBankAccounts();
    bool expandedTermAccounts();
    bool expandedStockAccounts();
    bool hasActiveTermAccounts();

    wxDateTime getUserDefinedFinancialYear(bool prevDayRequired = false);

    void setAccountNavTreeSection(wxString accountName); 
    bool setNavTreeSection( wxString sectionName);

private:
    /* handles to the DB Abstraction */
    boost::scoped_ptr<mmCoreDB> m_core;

    /* handles to SQLite Database */
    boost::shared_ptr<wxSQLite3Database> m_db;
    boost::shared_ptr<wxSQLite3Database> m_inidb;

    /* Currently open file name */
    wxString fileName_;
    wxString password_;

    int gotoAccountID_;
    bool homePageAccountSelect_;

    /* Cannot process home page recursively */
    bool refreshRequested_;

    /* Repeat Transactions automatic processing delay */
    wxTimer autoRepeatTransactionsTimer_;
    void OnAutoRepeatTransactionsTimer(wxTimerEvent& event);
    bool activeHomePage_;

    /* controls */
    mmPanelBase* panelCurrent_;
    wxPanel* homePanel;
    wxTreeCtrl* navTreeCtrl_;
    wxMenuBar *menuBar_;
    wxToolBar* toolBar_;
    wxStatusBar* statusBar_;

    mmTreeItemData* selectedItemData_;
    wxMenuItem* menuItemOnlineUpdateCurRate_; // Menu Item for Disabling Item

    wxTreeItemId getTreeItemfor(wxTreeItemId itemID, wxString accountName);
    bool setAccountInSection(wxString sectionName, wxString accountName);

    /* Custom Reports */
    customSQLReportIndex* custRepIndex_;
    wxString customSqlReportSelectedItem_;

    /* printing */
    boost::scoped_ptr<wxHtmlEasyPrinting> printer_;

    /* wxAUI */
    wxAuiManager m_mgr;
    wxString m_perspective;

    /* Homepage panel logic */
    wxString m_topCategories;
    bool activeTermAccounts_;
    wxArrayString getAccountsArray(bool withTermAccounts = false);

    void cleanup();
    wxSizer* cleanupHomePanel(bool new_sizer = true);
    void openFile(const wxString& fileName, bool openingNew, const wxString &password = wxGetEmptyString());
    void createDataStore(const wxString& fileName, const wxString &passwd, bool openingNew);
    void createMenu();
    void createToolBar();
    void createHomePage();
    void createReportsPage(mmPrintableBase* rb);
    void createHelpPage();
    wxPanel* createMainFrame(wxPanel* mainpanel);

    void createCheckingAccountPage(int accountID);
    void createBudgetingPage(int budgetYearID);
    void createControls();
    void loadConfigFile();
    void saveConfigFile();
    void menuEnableItems(bool enable);
    void menuPrintingEnable(bool enable);
    void updateNavTreeControl(bool expandTermAccounts = false);
    void showTreePopupMenu(wxTreeItemId id, const wxPoint& pt);
    void showBeginAppDialog();
    void openDataBase(const wxString& fileName);
    void OnLaunchAccountWebsite(wxCommandEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnConvertEncryptedDB(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExportToCSV(wxCommandEvent& event);
    void OnExportToQIF(wxCommandEvent& event);
    void OnExportToHtml(wxCommandEvent& event);
    void OnImportCSV(wxCommandEvent& event);
    void OnImportQFX(wxCommandEvent& event);
    void OnImportUniversalCSV(wxCommandEvent& event);
    void OnImportCSVMMNET(wxCommandEvent& event);
    void OnImportQIF(wxCommandEvent& event);
    void OnPrintPageSetup(wxCommandEvent& WXUNUSED(event));
    void OnPrintPageReport(wxCommandEvent& WXUNUSED(event));
    void OnPrintPagePreview(wxCommandEvent& WXUNUSED(event));
    void OnQuit(wxCommandEvent& event);
    void OnBillsDeposits(wxCommandEvent& event);
    void OnStocks(wxCommandEvent& event);
    void OnAssets(wxCommandEvent& event);
    void OnGotoAccount(wxCommandEvent& WXUNUSED(event));
    void OnViewToolbar(wxCommandEvent &event);
    void OnViewStatusbar(wxCommandEvent &event);
    void OnViewLinks(wxCommandEvent &event);
    void OnViewBankAccounts(wxCommandEvent &event);
    void OnViewTermAccounts(wxCommandEvent &event);
    void OnViewStockAccounts(wxCommandEvent &event);
    void OnViewToolbarUpdateUI(wxUpdateUIEvent &event);
    void OnViewStatusbarUpdateUI(wxUpdateUIEvent &event);
    void OnViewLinksUpdateUI(wxUpdateUIEvent &event);
    void OnOnlineUpdateCurRate(wxCommandEvent& event);
    void OnNewAccount(wxCommandEvent& event);
    void OnAccountList(wxCommandEvent& event);
    void OnEditAccount(wxCommandEvent& event);
    void OnDeleteAccount(wxCommandEvent& event);

    void OnOrgCategories(wxCommandEvent& event);
    void OnOrgPayees(wxCommandEvent& event);
    void OnCategoryRelocation(wxCommandEvent& event);
    void OnPayeeRelocation(wxCommandEvent& event);

    void OnOptions(wxCommandEvent& event);
    void OnBudgetSetupDialog(wxCommandEvent& event);
    void OnCurrency(wxCommandEvent& event);
    void OnTransactionReport(wxCommandEvent& event);
    void OnCashFlowSpecificAccounts();

    void OnHelp(wxCommandEvent& event);
    void OnShowAppStartDialog(wxCommandEvent& WXUNUSED(event));
    void OnCheckUpdate(wxCommandEvent& event);
    void OnReportIssues(wxCommandEvent& event);
    void OnBeNotified(wxCommandEvent& event);
	void OnFacebook(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnItemRightClick(wxTreeEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnPopupDeleteAccount(wxCommandEvent& event);
    void OnPopupEditAccount(wxCommandEvent& event);
    void OnPopupImportQIFile(wxCommandEvent& event);

    void OnViewAllAccounts(wxCommandEvent& event);
    void OnViewFavoriteAccounts(wxCommandEvent& event);
    void OnViewOpenAccounts(wxCommandEvent& event);

    /* Custom SQL Report*/
    bool CustomSQLReportSelected(int& customSqlReportID, mmTreeItemData* iData );
    void CreateCustomReport(int index);
    void EditCustomSqlReport();
    void DeleteCustomSqlReport();
    void RunCustomSqlDialog(bool forEdit = false);
    bool expandedReportNavTree_;
    bool expandedCustomSqlReportNavTree_;

    void OnTreeItemExpanded(wxTreeEvent& event);
    void OnTreeItemCollapsed(wxTreeEvent& event);

    void OnNewCustomSqlReport(wxCommandEvent& event);
    void OnEditCustomSqlReport(wxCommandEvent& event);
    void OnPopupEditCustomSqlReport(wxCommandEvent& event);
    void OnDeleteCustomSqlReport(wxCommandEvent& event);
    void OnPopupDeleteCustomSqlReport(wxCommandEvent& event);

    bool financialYearIsDifferent();
    bool IsUpdateAvailable(wxString page);
    void processPendingEvents();

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------
#endif // _MM_EX_MMEX_H_
//----------------------------------------------------------------------------

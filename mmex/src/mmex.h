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
    wxString createCategoryList();

    void setGotoAccountID(int account_id) { gotoAccountID_ = account_id; }
    void unselectNavTree();

private:
    /* handles to the DB Abstraction */
    boost::scoped_ptr<mmCoreDB> core_;

    /* handles to SQLite Database */
    boost::shared_ptr<wxSQLite3Database> db_;
    boost::scoped_ptr<wxSQLite3Database> inidb_;

    /* Currently open file name */
    wxString fileName_;
    wxString password_;

    int gotoAccountID_;

    /* Cannot process home page recursively */
    bool refreshRequested_;

    /* controls */
    mmPanelBase* panelCurrent_;
    wxPanel* homePanel;
    wxTreeCtrl* navTreeCtrl_;
    wxMenuBar *menuBar_;
    wxToolBar* toolBar_;
    mmTreeItemData* selectedItemData_;
    wxMenuItem* menuItemOnlineUpdateCurRate_; // Menu Item for Disabling Item

    /* printing */
    boost::scoped_ptr<wxHtmlEasyPrinting> printer_;

    /* wxAUI */
    wxAuiManager m_mgr;
    wxString m_perspective;

    /* Homepage panel logic */
    wxString m_topCategories;

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
    void updateNavTreeControl();
    void showTreePopupMenu(wxTreeItemId id, const wxPoint& pt);
    void showBeginAppDialog();
    void openDataBase(const wxString& fileName);
    void OnLaunchAccountWebsite(wxCommandEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnConvertEncryptedDB(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
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
    void OnAbout(wxCommandEvent& event);

    void OnItemRightClick(wxTreeEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnPopupDeleteAccount(wxCommandEvent& event);
    void OnPopupEditAccount(wxCommandEvent& event);

    void OnViewAllAccounts(wxCommandEvent& event);
    void OnViewFavoriteAccounts(wxCommandEvent& event);
    void OnViewOpenAccounts(wxCommandEvent& event);

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------
#endif // _MM_EX_MMEX_H_
//----------------------------------------------------------------------------

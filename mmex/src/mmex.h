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

#ifndef _MM_EX_MMEX_H_
#define _MM_EX_MMEX_H_
//----------------------------------------------------------------------------
#include "defs.h"
#include "wx/wizard.h"
#include "wx/aui/aui.h"
#include "wx/xml/xml.h"
#include "wx/arrstr.h"
#include "dbwrapper.h"
#include "util.h"
#include "maincurrencydialog.h"
//----------------------------------------------------------------------------
#include "mmcoredb.h"
#include <boost/scoped_ptr.hpp>
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
class wxListCtrl;
class wxPanel;
class wxBoxSizer;
class wxSplitterWindow;
class mmCoreDataStore;
class wxTreeCtrl;
class wxFileConfig;
class mmPanelBase;
class wxTreeItemData;
class mmData;
class wxToolBar;
class mmPrintableBase;
//----------------------------------------------------------------------------

/** 
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

struct CategInfo
{
    wxString categ;
    wxString amountStr;
    double   amount;
};
//----------------------------------------------------------------------------

class mmGUIFrame : public wxFrame
{
public:
    mmGUIFrame(const wxString& title, 
               const wxPoint& pos, 
               const wxSize& size);

    ~mmGUIFrame();

    void createMenu();
    void createToolBar();
    void createHomePage();
    void createReportsPage(mmPrintableBase* rb);
    void createHelpPage();
    wxPanel* createMainFrame(wxPanel* mainpanel);

    wxString createCategoryList();
    void createDataStore(const wxString& fileName,  
						 bool openingNew = false);
    void createCheckingAccountPage(int accountID);
    void createBudgetingPage(int budgetYearID);
    void createControls();
    void loadConfigFile();
    void saveConfigFile();
    void menuEnableItems(bool enable);
    void menuPrintingEnable(bool enable);
    void updateNavTreeControl();
    void showTreePopupMenu(wxTreeItemId id, const wxPoint& pt);
    void openFile(const wxString& fileName, bool openingNew);
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

    void OnWizardCancel(wxWizardEvent& event);

public:
    mmPanelBase* panelCurrent_;
    wxPanel* homePanel;
    wxTreeCtrl* navTreeCtrl_;
    int gotoAccountID_;

private:
    /* handles to the DB Abstraction */
    boost::scoped_ptr<mmCoreDB> core_;

    /* handles to SQLite Database */
    boost::shared_ptr<wxSQLite3Database> db_;
    boost::scoped_ptr<wxSQLite3Database> inidb_;

    /* Currently open file name */
    wxString fileName_;
	wxString password_;

    /* controls */
    wxMenuBar *menuBar_;
    wxToolBar* toolBar_;
    mmTreeItemData* selectedItemData_;
  
    /* printing */
    wxHtmlEasyPrinting* printer_;

	/* wxAUI */
	wxAuiManager m_mgr;
   wxString m_perspective;

   /* Homepage panel logic */
   wxString    m_topCategories;

   /* Cannot process home page recursively */
   bool refreshRequested_;

   /* Menu Item for Disabling Item */
   wxMenuItem* menuItemOnlineUpdateCurRate_;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

    wxSizer* cleanupHomePanel(bool new_sizer = true);
};
//----------------------------------------------------------------------------
#endif // _MM_EX_MMEX_H_
//----------------------------------------------------------------------------

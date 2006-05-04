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

#ifndef _MM_EX_MMEX_H_
#define _MM_EX_MMEX_H_

#include "defs.h"

class mmGUIApp : public wxApp
{
public:
    virtual bool OnInit();

protected:
    wxLocale m_locale; // locale we'll be using
};

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

class mmTreeItemData : public wxTreeItemData
{
public:
    mmTreeItemData(int id, bool isBudget) : id_(id), 
        isString_(false), 
        isBudgetingNode_(isBudget) { }
    mmTreeItemData(const wxString& string) : stringData_(string), 
        isString_(true), isBudgetingNode_(false) { }
    int getData() { return id_; }
    wxString getString() { return stringData_; }
    bool isStringData() { return isString_; }
    bool isBudgetingNode() { return isBudgetingNode_; }

private:
    int id_;
    bool isString_;
    wxString stringData_;
    bool isBudgetingNode_;
};

class mmGUIFrame : public wxFrame
{
public:
    mmGUIFrame(const wxString& title, 
        const wxPoint& pos, const wxSize& size);
    ~mmGUIFrame();

    // utility functions
    void createMenu();
    void createToolBar();
    wxPanel* createMainFrame(wxPanel* mainpanel);

    void createHomePage();
    void createReportsPage(mmPrintableBase* rb);
    void mmGUIFrame::createHelpPage();

    void createDataStore(const wxString& fileName, bool openingNew = false);
    void createCheckingAccountPage(int accountID);
    void createBudgetingPage(int budgetYearID);
    void createControls();
    void loadConfigFile();
    void saveConfigFile();
    void menuEnableItems(bool enable);
    void menuPrintingEnable(bool enable);
    void updateNavTreeControl();
    void showTreePopupMenu(wxTreeItemId id, const wxPoint& pt);
    void saveFile(void);
    void openFile(const wxString& fileName, bool openingNew);
    void showBeginAppDialog();
    void openDataBase(const wxString& fileName);
    void OnLaunchAccountWebsite(wxCommandEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnExportToHtml(wxCommandEvent& event);
    void OnImportCSV(wxCommandEvent& event);
    void OnImportCSVMMNET(wxCommandEvent& event);
    void OnPrintPageSetup(wxCommandEvent& WXUNUSED(event));
    void OnPrintPageReport(wxCommandEvent& WXUNUSED(event));
    void OnPrintPagePreview(wxCommandEvent& WXUNUSED(event));
    void OnQuit(wxCommandEvent& event);
    void OnBillsDeposits(wxCommandEvent& event);
    void OnStocks(wxCommandEvent& event);

    void OnNewAccount(wxCommandEvent& event);
    void OnAccountList(wxCommandEvent& event);

    void OnOrgCategories(wxCommandEvent& event);
    void OnOrgPayees(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnBudgetSetupDialog(wxCommandEvent& event);
    void OnCurrency(wxCommandEvent& event);

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

public:
    mmPanelBase* panelCurrent_;
    //wxPanel* replace;
    wxPanel* homePanel;
    wxTreeCtrl* navTreeCtrl_;

private:
    /* handles to SQLite Database */
    wxSQLite3Database* db_;
    wxSQLite3Database* inidb_;

    /* Currently open file name */
    wxString fileName_;

    /* controls */
    wxMenuBar *menuBar_;
    wxToolBar* toolBar_;
    mmTreeItemData* selectedItemData_;
  
    /* printing */
    wxHtmlEasyPrinting* printer_;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif

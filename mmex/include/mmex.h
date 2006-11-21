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
#include "wx/wizard.h"
#include "dbwrapper.h"
#include "util.h"
#include "currencydialog.h"

#include "mmcoredb.h"

class mmGUIApp : public wxApp
{
public:
    virtual bool OnInit();

public:
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

/** 
Class used to store item specific information in a tree node
*/
class mmTreeItemData : public wxTreeItemData
{
public:
    mmTreeItemData(int id, bool isBudget) 
       :id_(id), 
        isString_(false), 
        isBudgetingNode_(isBudget) {}
    mmTreeItemData(const wxString& string) 
       :stringData_(string), 
        isString_(true), 
        isBudgetingNode_(false) {}
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

class wxNewDatabaseWizardPage1 : public wxWizardPageSimple
{
public:
    void OnCurrency(wxCommandEvent& event)
    {
       currencyID_ = parent_->core_->currencyList_.getBaseCurrencySettings();

        mmCurrencyDialog *dlg = new mmCurrencyDialog(parent_->core_, currencyID_, this);
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

    wxNewDatabaseWizardPage1(mmNewDatabaseWizard* parent) 
        : wxWizardPageSimple(parent), parent_(parent), currencyID_(-1)
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


        SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    virtual bool TransferDataFromWindow()
    {
        if ( currencyID_ == -1)
        {
            wxMessageBox(_("Base Currency Not Set"), _T("Money Manager Ex Error"),
                         wxICON_WARNING | wxOK, this);

            return false;
        }
        return true;
    }

private:
    mmNewDatabaseWizard* parent_;
    wxButton* itemButtonCurrency_;
    int currencyID_;

    DECLARE_EVENT_TABLE()
};

class mmAddAccountWizard : public wxWizard
{
public:
    mmAddAccountWizard(wxFrame *frame, 
       mmCoreDB* core);
    void RunIt(bool modal);
    wxString accountName_;

    mmCoreDB* core_;
    int acctID_;
private:
    wxWizardPageSimple* page1;
};

class wxAddAccountPage1 : public wxWizardPageSimple
{
public:
    wxAddAccountPage1(mmAddAccountWizard* parent) 
        : wxWizardPageSimple(parent), parent_(parent)
    {
        textAccountName_ = new wxTextCtrl(this, wxID_ANY, 
                        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );

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

    virtual bool TransferDataFromWindow()
    {
        if ( textAccountName_->GetValue() == wxT(""))
        {
            wxMessageBox(_("Account Name Invalid"), _T("Money Manager Ex Error"),
                         wxICON_WARNING | wxOK, this);

            return false;
        }
        parent_->accountName_ = mmCleanString(textAccountName_->GetValue().Trim());
        return true;
    }

private:
    mmAddAccountWizard* parent_;
    wxTextCtrl* textAccountName_;
};

class wxAddAccountPage2 : public wxWizardPageSimple
{
public:
    wxAddAccountPage2(mmAddAccountWizard *parent) 
        : wxWizardPageSimple(parent), parent_(parent)
    {
        wxString itemAcctTypeStrings[] =  
        {
            _("Checking/Savings/CreditCard"),
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

    virtual bool TransferDataFromWindow()
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

private:
    wxChoice* itemChoiceType_;
    mmAddAccountWizard* parent_;
};

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
    void openFile(const wxString& fileName, bool openingNew);
    void showBeginAppDialog();
    void openDataBase(const wxString& fileName);
    void OnLaunchAccountWebsite(wxCommandEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
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
    mmCoreDB* core_;

    /* handles to SQLite Database */
    boost::shared_ptr<wxSQLite3Database> db_;
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

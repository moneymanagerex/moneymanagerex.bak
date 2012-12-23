/*******************************************************
Copyright (C) 2009 VaDiM

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

/****************************************************************************
 Revision of last commit: $Revision$
 Author of last commit:   $Author$
 ****************************************************************************/
#include <UnitTest++.h>
#include <boost/scoped_ptr.hpp>
#include <wx/filename.h>
//----------------------------------------------------------------------------
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "mmex_settings.h"
#include <iostream>
//----------------------------------------------------------------------------

const wxString g_BudgetYear   = wxT("2009");
const wxString g_CategName    = wxT("new category");
const wxString g_SubCategName = wxT("new subcategory");
const wxString g_CurrencyName = wxT("US Dollar");
const wxString g_PayeeName    = wxT("Payee #1");

const wxString g_TransType_deposit    = wxT("Deposit");
const wxString g_TransType_withdrawal = wxT("Withdrawal");
const wxString g_status_reconciled    = wxT("R");
const wxString g_status_void          = wxT("V");
//----------------------------------------------------------------------------

/*****************************************************************************************
 The test platform will create and test a new database.

 This class is used to remove the temporary database on completion.
 *****************************************************************************************/
class Cleanup
{
public:
    Cleanup(wxString filename, bool pause = false);
    ~Cleanup();
private:
    wxString dbFileName_;
    bool pause_;
};

Cleanup::Cleanup(wxString filename, bool pause)
: dbFileName_(filename)
, pause_(pause)
{
    printf("\nTest file created at location:\n");
    printf(wxString::Format(wxT("%s \n\n"), dbFileName_.c_str()).char_str());
}

// Cleanup class destructor - Called on test completion.
Cleanup::~Cleanup()
{
    try
    {
        if (pause_)
        {
            printf("\n");
            std::cout << "Test files will be deleted. ";
            std::cout << "Press ENTER to continue... ";
            std::cin.get();
        }
        wxRemoveFile(dbFileName_);
    }
    catch (...)
    {
        wxASSERT(false);
    }
}

/*****************************************************************************************
 Create a single access point for the ini_settings database
 *****************************************************************************************/
// Returns the user's current working directory for the new inidb database.
wxString getIniDbPpath()
{
    wxFileName fn(wxFileName::GetCwd(), wxT("mmexinidb_test.db3"));
    return fn.GetFullPath();
}

boost::shared_ptr<wxSQLite3Database> get_pInidb()
{
    static Cleanup temp_IniDatabase(getIniDbPpath(), true);
    static boost::shared_ptr<wxSQLite3Database> pInidb(new wxSQLite3Database);

    if (!pInidb->IsOpen())
    {
        wxString path = getIniDbPpath();
        wxRemoveFile(path);
        
        pInidb->Open(path);
        CHECK(pInidb->IsOpen());
    }
    return pInidb;
}

// Single point access for the test database, stored in memory.
boost::shared_ptr<MMEX_IniSettings> pSettingsList()
{
    static boost::shared_ptr<MMEX_IniSettings> pIniList(new MMEX_IniSettings(get_pInidb()));

    return pIniList;
}
/*****************************************************************************************/

/*****************************************************************************************
 Create a single access point for the database, Remove database on completion.
 *****************************************************************************************/
// Returns the user's current working directory for the new database.
wxString getDbPath()
{
    wxFileName fn(wxFileName::GetCwd(), wxT("mmex_tests.db3"));
    return fn.GetFullPath();
}

boost::shared_ptr<wxSQLite3Database> get_pDb()
{
    static Cleanup temp_database(getDbPath());
    static boost::shared_ptr<wxSQLite3Database> pDb(new wxSQLite3Database);

    if (!pDb->IsOpen())
    {
        wxString path = getDbPath();
        wxRemoveFile(path);

        pDb->Open(path);
        CHECK(pDb->IsOpen());
    }

    return pDb;
}

// Create a single access point for the main database, stored in memory.
boost::shared_ptr<mmCoreDB> pDb_core()
{
    static boost::shared_ptr<mmCoreDB> pCore(new mmCoreDB(get_pDb(), pSettingsList()));

    return pCore;
}
/*****************************************************************************************/
 
void const displayTimeTaken(const wxString msg, const wxDateTime start_time)
{
    const wxDateTime end_time(wxDateTime::UNow());
    const wxString time_dif = (end_time - start_time).Format(wxT("%S%l"));

    printf(wxString::Format(wxT("Time Taken: %s milliseconds - Test: %s \n"), time_dif.c_str(), msg.c_str()).char_str());
}
/*****************************************************************************************/

SUITE(dbwrapper)
{

TEST(wxSQLite3Exception)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* db = get_pDb().get();
    displayTimeTaken(wxT("wxSQLite3Exception"), start_time);
    CHECK_THROW(db->ExecuteUpdate("update bad_table set bad_col = unknown_value"), wxSQLite3Exception);
}
//----------------------------------------------------------------------------

TEST(HasBackupSupport)
{
    bool ok = get_pDb().get()->HasBackupSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(HasEncryptionSupport)
{
    bool ok = get_pDb().get()->HasEncryptionSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(HasMetaDataSupport)
{
    bool ok = get_pDb().get()->HasMetaDataSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(HasSavepointSupport)
{
    bool ok = get_pDb().get()->HasSavepointSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(ViewExists)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxSQLite3Database* pDb = get_pDb().get();

    bool ok = mmDBWrapper::ViewExists(pDb, "I'm_not_exists");
    CHECK(!ok);

    pDb->ExecuteUpdate(
        "create view master_view as "
        "select * "
        "from sqlite_master"
    );

    ok = mmDBWrapper::ViewExists(pDb, "master_view");
    CHECK(ok);
    displayTimeTaken(wxT("ViewExists"), start_time);

//    pDb->ExecuteUpdate("drop view master_view");
}
//----------------------------------------------------------------------------

TEST(init_DB)
{
    const wxDateTime start_time(wxDateTime::UNow());

    mmCoreDB* pCore = pDb_core().get();
    CHECK(pCore->displayDatabaseError_ == true);

    displayTimeTaken(wxT("init_DB"), start_time);    
}
//----------------------------------------------------------------------------
#if 0
TEST(getCurrencySymbol)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int currency_id = 1;
    wxString s = mmDBWrapper::getCurrencySymbol(pCore->db_.get(), currency_id);
    CHECK(s == L"EUR");

    currency_id = pCore->currencyList_.getCurrencyID(g_CurrencyName);
    s = mmDBWrapper::getCurrencySymbol(pCore->db_.get(), currency_id);
    CHECK(s == L"USD");

    s = mmDBWrapper::getCurrencySymbol(pCore->db_.get(), 0);
    CHECK(s.empty());
    displayTimeTaken(wxT("getCurrencySymbol"), start_time);    
}
#endif
//----------------------------------------------------------------------------

TEST(checkDBVersion)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    bool ok = mmDBWrapper::checkDBVersion(pDb);
    CHECK(ok);
    displayTimeTaken(wxT("checkDBVersion"), start_time);
}
//----------------------------------------------------------------------------

TEST(addBudgetYear)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    
    int year_id = mmDBWrapper::getBudgetYearID(pDb, g_BudgetYear);
    CHECK(year_id == -1);

    mmDBWrapper::addBudgetYear(pDb, g_BudgetYear);

    year_id = mmDBWrapper::getBudgetYearID(pDb, g_BudgetYear);
    CHECK(year_id > 0);
    displayTimeTaken(wxT("addBudgetYear"), start_time);
}
//----------------------------------------------------------------------------

TEST(getBudgetYearID)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();

    int year_id = mmDBWrapper::getBudgetYearID(pDb, wxT("unknown_year"));
    CHECK(year_id == -1);

    year_id = mmDBWrapper::getBudgetYearID(pDb, g_BudgetYear);
    CHECK(year_id > 0);
    displayTimeTaken(wxT("getBudgetYearID"), start_time);
}
//----------------------------------------------------------------------------

TEST(getBudgetYearForID)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();

    int year_id = mmDBWrapper::getBudgetYearID(pDb, g_BudgetYear);
    CHECK(year_id > 0);

    wxString year = mmDBWrapper::getBudgetYearForID(pDb, year_id);
    CHECK(year == g_BudgetYear);
    displayTimeTaken(wxT("getBudgetYearForID"), start_time);
}
//----------------------------------------------------------------------------

TEST(updateYearForID)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();

    int year_id = mmDBWrapper::getBudgetYearID(pDb, g_BudgetYear);
    CHECK(year_id > 0);

    wxString new_year = g_BudgetYear + g_BudgetYear;
    mmDBWrapper::updateYearForID(pDb, new_year, year_id);

    wxString year = mmDBWrapper::getBudgetYearForID(pDb, year_id);
    CHECK(year == new_year);

    // restore original value

    mmDBWrapper::updateYearForID(pDb, g_BudgetYear, year_id);
    year = mmDBWrapper::getBudgetYearForID(pDb, year_id);
    CHECK(year == g_BudgetYear);
    displayTimeTaken(wxT("updateYearForID"), start_time);
}
//----------------------------------------------------------------------------

TEST(copyBudgetYear)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();

    int base_year_id = mmDBWrapper::getBudgetYearID(pDb, g_BudgetYear);
    CHECK(base_year_id > 0);

    // --

    bool ok = mmDBWrapper::copyBudgetYear(pDb, base_year_id, base_year_id);
    CHECK(!ok);

    // --

    wxString new_year = g_BudgetYear + g_BudgetYear;
    mmDBWrapper::addBudgetYear(pDb, new_year);
    
    int new_year_id = mmDBWrapper::getBudgetYearID(pDb, new_year);
    CHECK(new_year_id > 0);

    // --

    ok = mmDBWrapper::copyBudgetYear(pDb, new_year_id, base_year_id);
    CHECK(ok);
    displayTimeTaken(wxT("copyBudgetYear"), start_time);
}
//----------------------------------------------------------------------------

TEST(deleteBudgetYear)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();

    bool deleted = mmDBWrapper::deleteBudgetYear(pDb, wxT("wrong_year"));
    CHECK(!deleted);

    deleted = mmDBWrapper::deleteBudgetYear(pDb, g_BudgetYear);
    CHECK(deleted);
    displayTimeTaken(wxT("deleteBudgetYear"), start_time);
}
//----------------------------------------------------------------------------

TEST(addCategory)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.AddCategory(g_CategName);

    int id = pCore->categoryList_.GetCategoryId(g_CategName);

    CHECK(id == cat_id);
    displayTimeTaken(wxT("addCategory"), start_time);
}
//----------------------------------------------------------------------------

TEST(addSubCategory)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    int subcat_id = pCore->categoryList_.AddSubCategory(cat_id, g_SubCategName);

    int id = pCore->categoryList_.GetSubCategoryID(cat_id, g_SubCategName);
    CHECK(id == subcat_id);
    displayTimeTaken(wxT("addSubCategory"), start_time);
}
//----------------------------------------------------------------------------

TEST(getCategoryID)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    cat_id = pCore->categoryList_.GetCategoryId(wxT("unknown category"));
    CHECK(cat_id == -1);
    displayTimeTaken(wxT("getCategoryID"), start_time);
}
//----------------------------------------------------------------------------

TEST(getSubCategoryID)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    int sc_id = pCore->categoryList_.GetSubCategoryID(cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    sc_id = pCore->categoryList_.GetSubCategoryID(cat_id, wxT("unknown subcategory"));
    CHECK(sc_id == -1);
    displayTimeTaken(wxT("getSubCategoryID"), start_time);
}
//----------------------------------------------------------------------------

TEST(getCategoryName)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    wxString name = pCore->categoryList_.GetCategoryName(cat_id);
    CHECK(name == g_CategName);

    name = pCore->categoryList_.GetCategoryName(0);
    CHECK(name.empty());
    displayTimeTaken(wxT("getCategoryName"), start_time);
}
//----------------------------------------------------------------------------

TEST(getSubCategoryName)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    int sc_id = pCore->categoryList_.GetSubCategoryID(cat_id, g_SubCategName);
    CHECK(sc_id > 0);
    
    wxString name = pCore->categoryList_.GetSubCategoryName(cat_id, sc_id);
    CHECK(name == g_SubCategName);

    name = pCore->categoryList_.GetSubCategoryName(cat_id, 0);
    CHECK(name.empty());
    displayTimeTaken(wxT("getSubCategoryName"), start_time);
}
//----------------------------------------------------------------------------

TEST(updateCategory)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    int sc_id = pCore->categoryList_.GetSubCategoryID(cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    // --

    const wxString new_name = wxT("new name");

    bool ok = pCore->categoryList_.UpdateCategory(cat_id, -1, new_name);
    CHECK(ok);

    wxString name = pCore->categoryList_.GetCategoryName(cat_id);
    CHECK(name == new_name);

    ok = pCore->categoryList_.UpdateCategory(cat_id, -1, g_CategName); // restore name
    CHECK(ok);

    // --

    ok = pCore->categoryList_.UpdateCategory(cat_id, sc_id, new_name);
    CHECK(ok);

    name = pCore->categoryList_.GetSubCategoryString(cat_id, sc_id);
    CHECK(name == new_name);

    ok = pCore->categoryList_.UpdateCategory(cat_id, sc_id, g_SubCategName); // restore
    CHECK(ok);
    displayTimeTaken(wxT("updateCategory"), start_time);
}
//----------------------------------------------------------------------------

TEST(deleteSubCategoryWithConstraints)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    int sc_id = pCore->categoryList_.GetSubCategoryID(cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    bool ok = mmDBWrapper::deleteSubCategoryWithConstraints(pDb, cat_id, sc_id);
    CHECK(ok);
    displayTimeTaken(wxT("deleteSubCategoryWithConstraints"), start_time);
}
//----------------------------------------------------------------------------

TEST(deleteCategoryWithConstraints)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    bool ok = mmDBWrapper::deleteCategoryWithConstraints(pDb, cat_id);
    CHECK(ok);
    displayTimeTaken(wxT("deleteCategoryWithConstraints"), start_time);
}
//----------------------------------------------------------------------------

TEST(getCurrencyID)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->currencyList_.getCurrencyID(g_CurrencyName);
    CHECK(id > 0);

    id = pCore->currencyList_.getCurrencyID(wxT("unknown currency"));
    CHECK(id == -1);
    displayTimeTaken(wxT("getCurrencyID"), start_time);
}
//----------------------------------------------------------------------------

TEST(getCurrencyName)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->currencyList_.getCurrencyID(g_CurrencyName);
    CHECK(id > 0);

    wxString name = pCore->currencyList_.getCurrencyName(id);
    CHECK(name == g_CurrencyName);

    // --

    name = pCore->currencyList_.getCurrencyName(-1);
    CHECK(name.empty());
    displayTimeTaken(wxT("getCurrencyName"), start_time);
}
//----------------------------------------------------------------------------

TEST(setBaseCurrencySettings)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->currencyList_.getCurrencyID(g_CurrencyName);
    CHECK(id > 0);
    pCore->currencyList_.setBaseCurrencySettings(pCore->dbInfoSettings_.get(), id);

    int base_id = pCore->currencyList_.getBaseCurrencySettings(pCore->dbInfoSettings_.get());

    CHECK(base_id == id);
    displayTimeTaken(wxT("setBaseCurrencySettings"), start_time);
}
//----------------------------------------------------------------------------

TEST(getBaseCurrencySettings)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->currencyList_.getCurrencyID(g_CurrencyName);
    CHECK(id > 0);

    int base_id = pCore->currencyList_.getBaseCurrencySettings(pCore->dbInfoSettings_.get());
    CHECK(base_id == id);
    displayTimeTaken(wxT("getBaseCurrencySettings"), start_time);
}
//----------------------------------------------------------------------------

TEST(loadBaseCurrencySettings)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    pCore->currencyList_.LoadBaseCurrencySettings(pCore->dbInfoSettings_.get());

    CHECK(true);
    displayTimeTaken(wxT("loadBaseCurrencySettings"), start_time);
}
//----------------------------------------------------------------------------

#if 0
TEST(getCurrencyBaseConvRate)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->currencyList_.getCurrencyID(g_CurrencyName);
    CHECK(id > 0);

    double rate = mmDBWrapper::getCurrencyBaseConvRateForId(pDb, id);
    CHECK(rate != 0.0);
    displayTimeTaken(wxT("getCurrencyBaseConvRate"), start_time);
}
#endif
//----------------------------------------------------------------------------

TEST(load_Currency_Settings)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->currencyList_.getCurrencyID(g_CurrencyName);
    CHECK(id > 0);

    mmDBWrapper::loadCurrencySettings(pDb, id);
    displayTimeTaken(wxT("load_Currency_Settings"), start_time);
}
//----------------------------------------------------------------------------

TEST(setInfoSettingValue)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    const wxString name = wxT("settings name");
    const wxString val = wxT("settings value");

    pCore->dbInfoSettings_->SetStringSetting(name, val);

    wxString s = pCore->dbInfoSettings_->GetStringSetting(name, wxT(""));
    CHECK(s == val);
    pCore->dbInfoSettings_->Save(); // ensure they end up in the database
    displayTimeTaken(wxT("setInfoSettingValue"), start_time);
}
//----------------------------------------------------------------------------

TEST(getInfoSettingValue)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    const wxString name = wxT("wrong name");
    const wxString defVal = wxT("default value");

    wxString s = pCore->dbInfoSettings_->GetStringSetting(name, defVal);
    CHECK(s == defVal);

    // --

    pCore->dbInfoSettings_->SetStringSetting(name, defVal);
    s = pCore->dbInfoSettings_->GetStringSetting(name, wxT(""));
    CHECK(s == defVal);
    pCore->dbInfoSettings_->Save(); // ensure they end up in the database
    displayTimeTaken(wxT("getInfoSettingValue"), start_time);
}
//----------------------------------------------------------------------------

TEST(addPayee)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    mmCoreDB* pCore = pDb_core().get();

    int added = pCore->categoryList_.AddCategory(g_CategName);
    CHECK(added > 0);

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    added = pCore->categoryList_.AddSubCategory(cat_id, g_SubCategName);
    CHECK(added > 0);

    int sc_id = pCore->categoryList_.GetSubCategoryID(cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    // --

    int payeeID = pCore->payeeList_.AddPayee(g_PayeeName);
    boost::shared_ptr<mmPayee> pPayee = pCore->payeeList_.GetPayeeSharedPtr(payeeID);
    pPayee->categoryId_ = cat_id;
    pPayee->subcategoryId_ = sc_id;
    bool OK = pPayee->UpdateDb(pDb);
    CHECK(OK);

    displayTimeTaken(wxT("addPayee"), start_time);
}
//----------------------------------------------------------------------------

TEST(getPayeeID)
{
    const wxDateTime start_time(wxDateTime::UNow());

    mmCoreDB* pCore = pDb_core().get();

    int cat_id = pCore->categoryList_.GetCategoryId(g_CategName);
    CHECK(cat_id > 0);

    int sc_id = pCore->categoryList_.GetSubCategoryID(cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    int id = pCore->payeeList_.GetPayeeId(g_PayeeName);
    CHECK(id > 0);

    wxString name = pCore->payeeList_.GetPayeeName(id);
    CHECK(name == g_PayeeName);

    id = pCore->payeeList_.GetPayeeId(wxT("bad payee name"));
    CHECK(id < 0);

    displayTimeTaken(wxT("getPayeeID"), start_time);
}
//----------------------------------------------------------------------------

TEST(getPayee)
{
    const wxDateTime start_time(wxDateTime::UNow());

    mmCoreDB* pCore = pDb_core().get();

    int payee_id = pCore->payeeList_.GetPayeeId(g_PayeeName);
    CHECK(payee_id != -1);

    boost::shared_ptr<mmPayee> pPayee = pCore->payeeList_.GetPayeeSharedPtr(payee_id);
    CHECK(pPayee->name_ == g_PayeeName);
    CHECK(pPayee->categoryId_ != 0);
    CHECK(pPayee->subcategoryId_ != 0);

    wxString name = pCore->payeeList_.GetPayeeName(0);
    CHECK(name.empty());

    displayTimeTaken(wxT("getPayee"), start_time);
}
//----------------------------------------------------------------------------

TEST(updatePayee)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxSQLite3Database* pDb = get_pDb().get();
    mmCoreDB* pCore = pDb_core().get();

    int payee_id = pCore->payeeList_.GetPayeeId(g_PayeeName);
    CHECK(payee_id != -1);

    boost::shared_ptr<mmPayee> pPayee = pCore->payeeList_.GetPayeeSharedPtr(payee_id);
    int cat    = pPayee->categoryId_;
    int subcat = pPayee->subcategoryId_;

    const wxString new_name = wxT("new payee name");

    pPayee->name_ = new_name;
    pPayee->categoryId_ = 0;
    pPayee->subcategoryId_ = -1;
    bool ok = pPayee->UpdateDb(pDb);
    CHECK(ok);

    // Reset the payee list to match the database
    pPayee.reset();
    pCore->payeeList_.entries_.clear();
    pCore->payeeList_.LoadPayees();

    pPayee = pCore->payeeList_.GetPayeeSharedPtr(payee_id);
    wxString name2 = pCore->payeeList_.GetPayeeName(payee_id);
    CHECK_EQUAL(name2, new_name);
    CHECK(pPayee->categoryId_ != cat);
    CHECK(pPayee->subcategoryId_ != subcat);

    // restore
    pPayee->name_ = g_PayeeName;
    pPayee->categoryId_ = cat;
    pPayee->subcategoryId_ = subcat;
    ok = pPayee->UpdateDb(pDb);
    CHECK(ok);

    displayTimeTaken(wxT("updatePayee"), start_time);
}
//----------------------------------------------------------------------------

TEST(getAmountForPayee)
{
    const wxDateTime start_time(wxDateTime::UNow());

    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->payeeList_.GetPayeeId(g_PayeeName);
    CHECK(id != -1);

    wxString name = pCore->payeeList_.GetPayeeName(id);
    CHECK(name == g_PayeeName);

    const wxDateTime dt = wxDateTime::Now();
    double amt = pCore->bTransactionList_.getAmountForPayee(pCore, id, true, dt, dt);
    CHECK(amt == 0.0);

    displayTimeTaken(wxT("getAmountForPayee"), start_time);
}
//----------------------------------------------------------------------------

TEST(deletePayeeWithConstraints)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxSQLite3Database* pDb = get_pDb().get();
    mmCoreDB* pCore = pDb_core().get();

    int id = pCore->payeeList_.GetPayeeId(g_PayeeName);
    CHECK(id > 0);

    wxString name = pCore->payeeList_.GetPayeeName(id);
    CHECK(name == g_PayeeName);

    bool ok = mmDBWrapper::deletePayeeWithConstraints(pDb, id);
    CHECK(ok);

    ok = mmDBWrapper::deletePayeeWithConstraints(pDb, 0);
    CHECK(ok); // returns true even for wrong id

    displayTimeTaken(wxT("deletePayeeWithConstraints"), start_time);
}
//----------------------------------------------------------------------------

TEST(getNumAccounts)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int cnt = pCore->accountList_.getNumAccounts();
    CHECK(!cnt);
    displayTimeTaken(wxT("getNumAccounts"), start_time);
}
//----------------------------------------------------------------------------

TEST(add_new_transactions)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int trans_list_size = pCore->bTransactionList_.transactions_.size();
    CHECK(trans_list_size == 0);

    int account_id = 1;

    // Add a deposit transaction Previous month
    boost::shared_ptr<mmBankTransaction> pBankTransaction_1(new mmBankTransaction(pCore->db_));
    pBankTransaction_1->date_      = wxDateTime::Now().Subtract(wxDateSpan::Days(32));
    pBankTransaction_1->accountID_ = account_id;
    pBankTransaction_1->amt_       = 500;
    pBankTransaction_1->toAmt_     = 500;
    pBankTransaction_1->status_    = g_status_reconciled;
    pBankTransaction_1->transType_ = g_TransType_deposit;
    int trans_id_1 = pCore->bTransactionList_.addTransaction(pCore, pBankTransaction_1);
    CHECK(trans_id_1 == 1);

    // Add a withdrawal transaction - Today
    boost::shared_ptr<mmBankTransaction> pBankTransaction_2(new mmBankTransaction(pCore->db_));
    pBankTransaction_2->date_      = wxDateTime::Now();
    pBankTransaction_2->accountID_ = account_id;
    pBankTransaction_2->amt_       = 200;
    pBankTransaction_2->toAmt_     = 200;
    pBankTransaction_2->status_    = g_status_reconciled;
    pBankTransaction_2->transType_ = g_TransType_withdrawal;
    int trans_id_2 = pCore->bTransactionList_.addTransaction(pCore, pBankTransaction_2);
    CHECK(trans_id_2 == 2);

    // Add a void deposit transaction - 2 days before today;
    boost::shared_ptr<mmBankTransaction> pBankTransaction_3(new mmBankTransaction(pCore->db_));
    pBankTransaction_3->date_      = wxDateTime::Now().Subtract(wxDateSpan::Days(2));
    pBankTransaction_3->accountID_ = account_id;
    pBankTransaction_3->amt_       = 1200;
    pBankTransaction_3->toAmt_     = 1200;
    pBankTransaction_3->status_    = g_status_void;
    pBankTransaction_3->transType_ = g_TransType_deposit;
    int trans_id_3 = pCore->bTransactionList_.addTransaction(pCore, pBankTransaction_3);
    CHECK(trans_id_3 == 3);

    // Checks that the transaction is in the database.
    bool trans_exist = pCore->bTransactionList_.checkForExistingTransaction(pBankTransaction_2);
    CHECK(trans_exist);

    double balance = pCore->bTransactionList_.getBalance(account_id);
    CHECK(balance == 300);

    trans_list_size = pCore->bTransactionList_.transactions_.size();
    CHECK(trans_list_size == 3);

    // Delete the void transaction
    bool trans_deleted_ok = pCore->bTransactionList_.deleteTransaction(account_id, trans_id_3);
    CHECK(trans_deleted_ok);

    // checks that the transaction is not in the database.
    trans_exist = pCore->bTransactionList_.checkForExistingTransaction(pBankTransaction_3);
    CHECK(!trans_exist);

    // This method deletes the transactions from memory but not the database
    // Database transactions need to be deleted individually
    // pCore->bTransactionList_.deleteTransactions(account_id);

    pCore->bTransactionList_.deleteTransaction(account_id, trans_id_1);
    pCore->bTransactionList_.deleteTransaction(account_id, trans_id_2);

    balance = pCore->bTransactionList_.getBalance(account_id);
    CHECK(balance == 0);

    // Check that no transactions are left im memory
    trans_list_size = pCore->bTransactionList_.transactions_.size();
    CHECK(trans_list_size == 0);

    // Check that the transactions are not in the database.
    trans_exist = pCore->bTransactionList_.checkForExistingTransaction(pBankTransaction_1);
    CHECK(!trans_exist);  // 
    trans_exist = pCore->bTransactionList_.checkForExistingTransaction(pBankTransaction_2);
    CHECK(!trans_exist);  // 

    displayTimeTaken(wxT("add_new_transactions"), start_time);

    //remove empty spaces from the database
    //pCore->db_.get()->Vacuum();
}

TEST(copy_paste_transactions)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    int account_id = 1;

    boost::shared_ptr<mmBankTransaction> pBankTransaction_1(new mmBankTransaction(pCore->db_));
    pBankTransaction_1->date_      = wxDateTime::Now();
    pBankTransaction_1->accountID_ = account_id;
    pBankTransaction_1->amt_       = 100;
    pBankTransaction_1->toAmt_     = 100;
    pBankTransaction_1->status_    = g_status_reconciled;
    pBankTransaction_1->transType_ = g_TransType_withdrawal;
    int trans_id_1 = pCore->bTransactionList_.addTransaction(pCore, pBankTransaction_1);
    CHECK(trans_id_1 == 1); // The transaction in previous test was not deleted from the database.

    bool new_trans_exist = pCore->bTransactionList_.checkForExistingTransaction(pBankTransaction_1);
    CHECK(new_trans_exist);

    boost::shared_ptr<mmBankTransaction> pCopiedTrans = pCore->bTransactionList_.copyTransaction(pCore, trans_id_1, account_id, true);
    int trans_id_2 = pCopiedTrans->transactionID();
    CHECK(trans_id_2 == 2);

    bool copied_trans_exist = pCore->bTransactionList_.checkForExistingTransaction(pCopiedTrans);
    CHECK(copied_trans_exist);

    pCopiedTrans->amt_   = 200;
    pCopiedTrans->toAmt_ = 200;
    pCore->bTransactionList_.UpdateTransaction(pCopiedTrans);

    double balance = pCore->bTransactionList_.getBalance(account_id);
    CHECK(balance == -300);

    int trans_list_size = pCore->bTransactionList_.transactions_.size();
    CHECK(trans_list_size == 2);
    displayTimeTaken(wxT("copy_paste_transactions"), start_time);
}

} // SUITE

/*****************************************************************************************
 The tests for testing mmex_settings - in a database.
 *****************************************************************************************/
SUITE(Inidb_test)
{

#if 1
TEST(New_INIDB_TEST_1)
{
    const wxDateTime start_time(wxDateTime::UNow());

	const wxString TEST_NAME_BOOL = wxT("Test 1 BOOL");
	const wxString TEST_NAME_INT  = wxT("Test 1 INTEGER");
	const wxString TEST_NAME_STR  = wxT("Test 1 STRING");
	const wxString TEST_VALUE_STR = wxT("String Value Test 1");

    boost::shared_ptr<wxSQLite3Database> pInidb = get_pInidb();
    MMEX_IniSettings* pSettings =  pSettingsList().get();

    bool b = pSettings->GetBoolSetting(TEST_NAME_BOOL, true);
    CHECK_EQUAL(b, true);

    int i = pSettings->GetIntSetting(TEST_NAME_INT, 150);
    CHECK(i == 150);

    wxString s = pSettings->GetStringSetting(TEST_NAME_STR, TEST_VALUE_STR);
    CHECK_EQUAL(s, TEST_VALUE_STR);

    displayTimeTaken(wxT("New_INIDB_TEST_1"), start_time);
}
#endif

#if 1
TEST(New_INIDB_TEST_2)
{
    const wxDateTime start_time(wxDateTime::UNow());

    const wxString TEST_NAME_BOOL = wxT("Test 2 BOOL");
	const wxString TEST_NAME_INT  = wxT("Test 2 INTEGER");
	const wxString TEST_NAME_STR  = wxT("Test 2 STRING");
	const wxString TEST_VALUE_STR = wxT("String Value Test 2");

    boost::shared_ptr<wxSQLite3Database> pInidb = get_pInidb();
    MMEX_IniSettings* pSettings =  pSettingsList().get();

    pSettings->SetBoolSetting(TEST_NAME_BOOL, true);
    bool b = pSettings->GetBoolSetting(TEST_NAME_BOOL, true);
    CHECK_EQUAL(b, true);

    pSettings->SetIntSetting(TEST_NAME_INT, 200);
    int i = pSettings->GetIntSetting(TEST_NAME_INT, 150);
    CHECK_EQUAL(i, 200);

    pSettings->SetStringSetting(TEST_NAME_STR, TEST_VALUE_STR);
    wxString s = pSettings->GetStringSetting(TEST_NAME_STR, TEST_VALUE_STR);
    CHECK_EQUAL(s, TEST_VALUE_STR);

    displayTimeTaken(wxT("New_INIDB_TEST_2"), start_time);
}
#endif

#if 1
TEST(New_INIDB_TEST_3)
{
    const wxDateTime start_time(wxDateTime::UNow());

	const wxString TEST_NAME_BOOL = wxT("Test 3 BOOL");
	const wxString TEST_NAME_INT  = wxT("Test 3 INTEGER");
	const wxString TEST_NAME_STR  = wxT("Test 3 STRING");

    const wxString TEST_VALUE_STR_NEW = wxT("String Value Test 3");
	const wxString TEST_VALUE_STR_ADJ = wxT("String Value Test 3_adjusted");

    boost::shared_ptr<wxSQLite3Database> pInidb = get_pInidb();
    MMEX_IniSettings* pSettings =  pSettingsList().get();

    // New settings being initialised
    pSettings->SetBoolSetting(TEST_NAME_BOOL, true);
    bool b = pSettings->GetBoolSetting(TEST_NAME_BOOL, false);
    CHECK_EQUAL(b, true);

    pSettings->SetIntSetting(TEST_NAME_INT, 300);
    int i = pSettings->GetIntSetting(TEST_NAME_INT, 150);
    CHECK_EQUAL(i, 300);

    pSettings->SetStringSetting(TEST_NAME_STR, TEST_VALUE_STR_NEW);
    wxString s = pSettings->GetStringSetting(TEST_NAME_STR, TEST_VALUE_STR_NEW);
    CHECK_EQUAL(s, TEST_VALUE_STR_NEW);

    // Existing settings being changed
    pSettings->SetBoolSetting(TEST_NAME_BOOL, false);
    b = pSettings->GetBoolSetting(TEST_NAME_BOOL, true);
    CHECK_EQUAL(b, false);

    pSettings->SetIntSetting(TEST_NAME_INT, 600);
    i = pSettings->GetIntSetting(TEST_NAME_INT, 150);
    CHECK_EQUAL(i, 600);

    pSettings->SetStringSetting(TEST_NAME_STR, TEST_VALUE_STR_ADJ);
    s = pSettings->GetStringSetting(TEST_NAME_STR, TEST_VALUE_STR_NEW);
    CHECK_EQUAL(s, TEST_VALUE_STR_ADJ);

    displayTimeTaken(wxT("New_INIDB_TEST_3"), start_time);
}
#endif

#if 1
TEST(New_INIDB_TEST_4)
{
    const wxDateTime start_time(wxDateTime::UNow());

    boost::shared_ptr<wxSQLite3Database> pInidb = get_pInidb();
    MMEX_IniSettings* pSettings =  pSettingsList().get();

    wxString test_name; 
    for(int i = 0; i < 100; ++i)
    {
        test_name = wxT("Inidb Test ");
        test_name << i;
        pSettings->SetIntSetting(test_name, i);
    }
    int value = pSettings->GetIntSetting(test_name, 50);
    CHECK_EQUAL(99, value);
    pSettings->Save();

    displayTimeTaken(wxT("New_INIDB_TEST_4"), start_time);
}
#endif

#if 1
TEST(New_INIDB_TEST_5)
{
    const wxDateTime start_time(wxDateTime::UNow());

    MMEX_IniSettings* pSettings =  pSettingsList().get();
    pSettings->Load();

    wxString test_name; 
    for(int i = 0; i < 100; ++i)
    {
        test_name = wxT("Inidb Test ");
        test_name << i;
        int value = pSettings->GetIntSetting(test_name, 50);
        CHECK_EQUAL(i, value);
    }

    displayTimeTaken(wxT("New_INIDB_TEST_5"), start_time);
}
#endif

#if 1
TEST(New_INFO_DB_TEST_1)
{
    const wxDateTime start_time(wxDateTime::UNow());

    //  Using the details from the master table database
    boost::shared_ptr<wxSQLite3Database> pdb = get_pDb();

    MMEX_IniSettings pdb_settings(pdb, true);

    const wxString name    = wxT("New_info_test_1: setting name");
    const wxString val     = wxT("New_info_test_1: setting value");
    const wxString new_val = wxT("New_info_test_1: new setting value");

    wxString test_name =  pdb_settings.GetStringSetting(name, val);
    CHECK_EQUAL(test_name, val);

    pdb_settings.SetStringSetting(name, new_val);
    test_name =  pdb_settings.GetStringSetting(name, val);
    CHECK_EQUAL(test_name, new_val);
    pdb_settings.Save();

    displayTimeTaken(wxT("New_INFO_DB_TEST_1"), start_time);
}
#endif

#if 1
TEST(New_INFO_DB_TEST_2)
{
    const wxDateTime start_time(wxDateTime::UNow());

    // Using the details from the global ini database
    boost::shared_ptr<wxSQLite3Database> pdb = get_pInidb();

    MMEX_IniSettings pdb_settings(pdb, true);

    const wxString name    = wxT("New_info_test_2: setting name");
    const wxString val     = wxT("New_info_test_2: setting value");
    const wxString new_val = wxT("New_info_test_2: new setting value");

    wxString test_name =  pdb_settings.GetStringSetting(name, val);
    CHECK_EQUAL(test_name, val);

    pdb_settings.SetStringSetting(name, new_val);
    test_name =  pdb_settings.GetStringSetting(name, val);
    CHECK_EQUAL(test_name, new_val);
    pdb_settings.Save();

    displayTimeTaken(wxT("New_INFO_DB_TEST_2"), start_time);
}
#endif

#if 1
TEST(New_INFO_DB_TEST_3)
{
    const wxDateTime start_time(wxDateTime::UNow());

    //  Save the details in the main database
    boost::shared_ptr<wxSQLite3Database> pdb = get_pDb();

    MMEX_IniSettings pdb_settings(pdb, true);

    const wxString name    = wxT("New_info_test_3: setting name");
    const wxString val     = wxT("New_info_test_3: setting value");
    const wxString new_val = wxT("New_info_test_3: new setting value");

    wxString test_name =  pdb_settings.GetStringSetting(name, val);
    CHECK_EQUAL(test_name, val);

    pdb_settings.SetStringSetting(name, new_val);
    test_name =  pdb_settings.GetStringSetting(name, val);
    CHECK_EQUAL(test_name, new_val);

    const wxString prev_name = wxT("settings name");
    const wxString prev_val  = wxT("settings value");

    test_name =  pdb_settings.GetStringSetting(prev_name, wxT(""));
    CHECK_EQUAL(test_name, prev_val);
    pdb_settings.Save();

    displayTimeTaken(wxT("New_INFO_DB_TEST_3"), start_time);
}
#endif

} // SUITE end Inidb_test

//----------------------------------------------------------------------------

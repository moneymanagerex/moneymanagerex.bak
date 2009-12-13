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

//----------------------------------------------------------------------------
#include <UnitTest++.h>
#include <boost/scoped_ptr.hpp>
//----------------------------------------------------------------------------
#include <dbwrapper.h>
#include "utils.h"
//----------------------------------------------------------------------------
#include <wx/filename.h>
//----------------------------------------------------------------------------

namespace
{

const wxString g_BudgetYear = wxT("2009");
const wxString g_CategName = wxT("new category");
const wxString g_SubCategName= wxT("new subcategory");
const wxString g_CurrencyName = wxT("US DOLLAR");
const wxString g_PayeeName = wxT("Payee #1");
//----------------------------------------------------------------------------

struct Cleanup
{
   ~Cleanup();
};
//----------------------------------------------------------------------------

wxString getDbPath()
{
    wxFileName fn(wxFileName::GetTempDir(), wxT("mmex_tests.db3"));
    return fn.GetFullPath();
}
//----------------------------------------------------------------------------

Cleanup::~Cleanup()
{ 
    try {
        wxRemoveFile(getDbPath());
    } catch (...) {
        wxASSERT(false);
    }        
}
//----------------------------------------------------------------------------

wxSQLite3Database& getDb()
{
    static Cleanup dummy;
    static wxSQLite3Database db;

    if (!db.IsOpen())
    {
        wxString path = getDbPath();
        wxRemoveFile(path);

        db.Open(path);
        CHECK(db.IsOpen());
    }

    return db;
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

SUITE(dbwrapper)
{

TEST(wxSQLite3Exception)
{
    wxSQLite3Database &db = getDb();
    CHECK_THROW(db.ExecuteUpdate("update bad_table set bad_col = unknown_value"), wxSQLite3Exception);
}
//----------------------------------------------------------------------------

TEST(HasBackupSupport)
{
    bool ok = wxSQLite3Database::HasBackupSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(HasEncryptionSupport)
{
    bool ok = wxSQLite3Database::HasEncryptionSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(HasMetaDataSupport)
{
    bool ok = wxSQLite3Database::HasMetaDataSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(HasSavepointSupport)
{
    bool ok = wxSQLite3Database::HasSavepointSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(ViewExists)
{
    wxSQLite3Database &db = getDb();

    bool ok = mmDBWrapper::ViewExists(&db, "I'm_not_exists");
    CHECK(!ok);

    db.ExecuteUpdate("create view master_view as "
                     "select * "
                     "from sqlite_master"
                    );

    ok = mmDBWrapper::ViewExists(&db, "master_view");
    CHECK(ok);

    db.ExecuteUpdate("drop view master_view");
}
//----------------------------------------------------------------------------

TEST(initDB)
{
    wxSQLite3Database &db = getDb();

    mmDBWrapper::initDB(&db, 0);
    CHECK(true);
}
//----------------------------------------------------------------------------

TEST(getCurrencySymbol)
{
    wxSQLite3Database &db = getDb();

    wxString s = mmDBWrapper::getCurrencySymbol(&db, 1);
    CHECK(s == L"USD");

    s = mmDBWrapper::getCurrencySymbol(&db, 0);
    CHECK(s.empty());
}
//----------------------------------------------------------------------------

TEST(checkDBVersion)
{
    wxSQLite3Database &db = getDb();
    bool ok = mmDBWrapper::checkDBVersion(&db);
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(addBudgetYear)
{
    wxSQLite3Database &db = getDb();
    
    int year_id = mmDBWrapper::getBudgetYearID(&db, g_BudgetYear);
    CHECK(year_id == -1);

    mmDBWrapper::addBudgetYear(&db, g_BudgetYear);

    year_id = mmDBWrapper::getBudgetYearID(&db, g_BudgetYear);
    CHECK(year_id > 0);
}
//----------------------------------------------------------------------------

TEST(getBudgetYearID)
{
    wxSQLite3Database &db = getDb();

    int year_id = mmDBWrapper::getBudgetYearID(&db, wxT("unknown_year"));
    CHECK(year_id == -1);

    year_id = mmDBWrapper::getBudgetYearID(&db, g_BudgetYear);
    CHECK(year_id > 0);
}
//----------------------------------------------------------------------------

TEST(getBudgetYearForID)
{
    wxSQLite3Database &db = getDb();

    int year_id = mmDBWrapper::getBudgetYearID(&db, g_BudgetYear);
    CHECK(year_id > 0);

    wxString year = mmDBWrapper::getBudgetYearForID(&db, year_id);
    CHECK(year == g_BudgetYear);
}
//----------------------------------------------------------------------------

TEST(updateYearForID)
{
    wxSQLite3Database &db = getDb();

    int year_id = mmDBWrapper::getBudgetYearID(&db, g_BudgetYear);
    CHECK(year_id > 0);

    wxString new_year = g_BudgetYear + g_BudgetYear;
    mmDBWrapper::updateYearForID(&db, new_year, year_id);

    wxString year = mmDBWrapper::getBudgetYearForID(&db, year_id);
    CHECK(year == new_year);

    // restore original value

    mmDBWrapper::updateYearForID(&db, g_BudgetYear, year_id);
    year = mmDBWrapper::getBudgetYearForID(&db, year_id);
    CHECK(year == g_BudgetYear);
}
//----------------------------------------------------------------------------

TEST(copyBudgetYear)
{
    wxSQLite3Database &db = getDb();

    int base_year_id = mmDBWrapper::getBudgetYearID(&db, g_BudgetYear);
    CHECK(base_year_id > 0);

    // --

    bool ok = mmDBWrapper::copyBudgetYear(&db, base_year_id, base_year_id);
    CHECK(!ok);

    // --

    wxString new_year = g_BudgetYear + g_BudgetYear;
    mmDBWrapper::addBudgetYear(&db, new_year);
    
    int new_year_id = mmDBWrapper::getBudgetYearID(&db, new_year);
    CHECK(new_year_id > 0);

    // --

    ok = mmDBWrapper::copyBudgetYear(&db, new_year_id, base_year_id);
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(deleteBudgetYear)
{
    wxSQLite3Database &db = getDb();

    bool deleted = mmDBWrapper::deleteBudgetYear(&db, wxT("wrong_year"));
    CHECK(!deleted);

    deleted = mmDBWrapper::deleteBudgetYear(&db, g_BudgetYear);
    CHECK(deleted);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

TEST(addCategory)
{
    wxSQLite3Database &db = getDb();

    bool added = mmDBWrapper::addCategory(&db, g_CategName);
    CHECK(added);

    int id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(id > 0);
}
//----------------------------------------------------------------------------

TEST(addSubCategory)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    bool added = mmDBWrapper::addSubCategory(&db, cat_id, g_SubCategName);
    CHECK(added);

    int id = mmDBWrapper::getSubCategoryID(&db, cat_id, g_SubCategName);
    CHECK(id > 0);
}
//----------------------------------------------------------------------------

TEST(getCategoryID)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    cat_id = mmDBWrapper::getCategoryID(&db, wxT("unknown category"));
    CHECK(cat_id == -1);
}
//----------------------------------------------------------------------------

TEST(getSubCategoryID)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    int sc_id = mmDBWrapper::getSubCategoryID(&db, cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    sc_id = mmDBWrapper::getSubCategoryID(&db, cat_id, wxT("unknown subcategory"));
    CHECK(sc_id == -1);
}
//----------------------------------------------------------------------------

TEST(getCategoryName)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    wxString name = mmDBWrapper::getCategoryName(&db, cat_id);
    CHECK(name == g_CategName);

    name = mmDBWrapper::getCategoryName(&db, 0);
    CHECK(name.empty());
}
//----------------------------------------------------------------------------

TEST(getSubCategoryName)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    int sc_id = mmDBWrapper::getSubCategoryID(&db, cat_id, g_SubCategName);
    CHECK(sc_id > 0);
    
    wxString name = mmDBWrapper::getSubCategoryName(&db, cat_id, sc_id);
    CHECK(name == g_SubCategName);

    name = mmDBWrapper::getSubCategoryName(&db, cat_id, 0);
    CHECK(name.empty());
}
//----------------------------------------------------------------------------

TEST(updateCategory)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    int sc_id = mmDBWrapper::getSubCategoryID(&db, cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    // --

    const wxString new_name = wxT("new name");

    bool ok = mmDBWrapper::updateCategory(&db, cat_id, -1, new_name);
    CHECK(ok);

    wxString name = mmDBWrapper::getCategoryName(&db, cat_id);
    CHECK(name == new_name);

    ok = mmDBWrapper::updateCategory(&db, cat_id, -1, g_CategName); // restore name
    CHECK(ok);

    // --

    ok = mmDBWrapper::updateCategory(&db, cat_id, sc_id, new_name);
    CHECK(ok);

    name = mmDBWrapper::getSubCategoryName(&db, cat_id, sc_id);
    CHECK(name == new_name);

    ok = mmDBWrapper::updateCategory(&db, cat_id, sc_id, g_SubCategName); // restore
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(deleteSubCategoryWithConstraints)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    int sc_id = mmDBWrapper::getSubCategoryID(&db, cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    bool ok = mmDBWrapper::deleteSubCategoryWithConstraints(&db, cat_id, sc_id);
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(deleteCategoryWithConstraints)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    bool ok = mmDBWrapper::deleteCategoryWithConstraints(&db, cat_id);
    CHECK(ok);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

TEST(getCurrencyID)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    id = mmDBWrapper::getCurrencyID(&db, wxT("unknown currency"));
    CHECK(id == -1);
}
//----------------------------------------------------------------------------

TEST(getCurrencyName)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    wxString name = mmDBWrapper::getCurrencyName(&db, id);
    CHECK(name == g_CurrencyName);

    // --

    name = mmDBWrapper::getCurrencyName(&db, -1);
    CHECK(name.empty());
}
//----------------------------------------------------------------------------

TEST(setBaseCurrencySettings)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    mmDBWrapper::setBaseCurrencySettings(&db, id);
    
    int base_id = mmDBWrapper::getBaseCurrencySettings(&db);
    CHECK(base_id == id);
}
//----------------------------------------------------------------------------

TEST(getBaseCurrencySettings)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    int base_id = mmDBWrapper::getBaseCurrencySettings(&db);
    CHECK(base_id == id);
}
//----------------------------------------------------------------------------

TEST(loadBaseCurrencySettings)
{
    wxSQLite3Database &db = getDb();

    mmDBWrapper::loadBaseCurrencySettings(&db);
    CHECK(true);
}
//----------------------------------------------------------------------------

TEST(getCurrencyBaseConvRate)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    double rate = mmDBWrapper::getCurrencyBaseConvRateForId(&db, id);
    CHECK(rate != 0.0);
}
//----------------------------------------------------------------------------

TEST(loadSettings)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    mmDBWrapper::loadSettings(&db, id);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

TEST(setInfoSettingValue)
{
    wxSQLite3Database &db = getDb();

    const wxString name = wxT("settings name");
    const wxString val = wxT("settings value");

    mmDBWrapper::setInfoSettingValue(&db, name, val);

    wxString s = mmDBWrapper::getInfoSettingValue(&db, name, wxT(""));
    CHECK(s == val);
}
//----------------------------------------------------------------------------

TEST(getInfoSettingValue)
{
    wxSQLite3Database &db = getDb();

    const wxString name = wxT("wrong name");
    const wxString defVal = wxT("default value");

    wxString s = mmDBWrapper::getInfoSettingValue(&db, name, defVal);
    CHECK(s == defVal);

    // --

    mmDBWrapper::setInfoSettingValue(&db, name, defVal);
    s = mmDBWrapper::getInfoSettingValue(&db, name, wxT(""));
    CHECK(s == defVal);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

TEST(addPayee)
{
    wxSQLite3Database &db = getDb();

    bool added = mmDBWrapper::addCategory(&db, g_CategName);
    CHECK(added);

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    added = mmDBWrapper::addSubCategory(&db, cat_id, g_SubCategName);
    CHECK(added);

    int sc_id = mmDBWrapper::getSubCategoryID(&db, cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    // --

    mmDBWrapper::addPayee(&db, g_PayeeName, cat_id, sc_id);
}
//----------------------------------------------------------------------------

TEST(getPayeeID)
{
    wxSQLite3Database &db = getDb();

    int cat_id = mmDBWrapper::getCategoryID(&db, g_CategName);
    CHECK(cat_id > 0);

    int sc_id = mmDBWrapper::getSubCategoryID(&db, cat_id, g_SubCategName);
    CHECK(sc_id > 0);

    // --

    int id = 0;
    int cat = 0;
    int subc = 0;

    bool ok = mmDBWrapper::getPayeeID(&db, g_PayeeName, id, cat, subc);
    CHECK(ok);
    CHECK(id > 0);
    CHECK(cat > 0);
    CHECK(subc > 0);

    // --

    ok = mmDBWrapper::getPayeeID(&db, wxT("bad payee name"), id, cat, subc);
    CHECK(!ok);
}
//----------------------------------------------------------------------------

TEST(getPayee)
{
    wxSQLite3Database &db = getDb();

    int id = 0;
    int cat = 0;
    int subc = 0;

    bool ok = mmDBWrapper::getPayeeID(&db, g_PayeeName, id, cat, subc);
    CHECK(ok);

    // --

    int cat2 = 0;
    int subc2 = 0;
    wxString name = mmDBWrapper::getPayee(&db, id, cat2, subc2);
    CHECK(name == g_PayeeName);
    CHECK(cat2 == cat);
    CHECK(subc2 == subc);

    // --

    name = mmDBWrapper::getPayee(&db, 0, cat, subc);
    CHECK(name.empty());
}
//----------------------------------------------------------------------------

TEST(updatePayee)
{
    wxSQLite3Database &db = getDb();

    int id = 0;
    int cat = 0;
    int subc = 0;

    bool ok = mmDBWrapper::getPayeeID(&db, g_PayeeName, id, cat, subc);
    CHECK(ok);

    // --

    const wxString new_name = wxT("new payee name");
    ok = mmDBWrapper::updatePayee(&db, new_name, id, cat, -1);
    CHECK(ok);

    // --

    int subc2 = 0;
    wxString name = mmDBWrapper::getPayee(&db, id, cat, subc2);
    CHECK(name == new_name);
    CHECK(subc2 == -1);

    // restore

    ok = mmDBWrapper::updatePayee(&db, g_PayeeName, id, cat, subc);
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(getAmountForPayee)
{
    wxSQLite3Database &db = getDb();

    int id = 0;
    int cat = 0;
    int subc = 0;

    bool ok = mmDBWrapper::getPayeeID(&db, g_PayeeName, id, cat, subc);
    CHECK(ok);

    // --

    const wxDateTime dt = wxDateTime::Now();
    double amt = mmDBWrapper::getAmountForPayee(&db, id, true, dt, dt);
    CHECK(amt == 0.0);
}
//----------------------------------------------------------------------------

TEST(deletePayeeWithConstraints)
{
    wxSQLite3Database &db = getDb();

    int id = 0;
    int cat = 0;
    int subc = 0;

    bool ok = mmDBWrapper::getPayeeID(&db, g_PayeeName, id, cat, subc);
    CHECK(ok);

    // --

    ok = mmDBWrapper::deletePayeeWithConstraints(&db, id);
    CHECK(ok);

    // --

    ok = mmDBWrapper::deletePayeeWithConstraints(&db, 0);
    CHECK(ok); // returns true even for wrong id
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

TEST(getNumAccounts)
{
    wxSQLite3Database &db = getDb();

    int cnt = mmDBWrapper::getNumAccounts(&db);
    CHECK(!cnt);
}
//----------------------------------------------------------------------------

} // SUITE

//----------------------------------------------------------------------------

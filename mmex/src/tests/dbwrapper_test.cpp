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
/*******************************************************/

//----------------------------------------------------------------------------
#include <UnitTest++.h>
#include <boost/scoped_ptr.hpp>
//----------------------------------------------------------------------------
#include <dbwrapper.h>
#include "utils.h"
//----------------------------------------------------------------------------

namespace
{

const wxString g_BudgetYear = wxT("2009");
const wxString g_CategName = wxT("new category");
const wxString g_SubCategName= wxT("new subcategory");
//----------------------------------------------------------------------------

struct SQLiteInit
{
    SQLiteInit() { wxSQLite3Database::InitializeSQLite(); }
   ~SQLiteInit() { wxSQLite3Database::ShutdownSQLite();   }
};
//----------------------------------------------------------------------------

std::wstring getDbPath()
{
    std::wstring path = utils::getTempDir();
    path += utils::DirSep;
    path += L"mmex_tests.db3";

    return path;
}
//----------------------------------------------------------------------------

wxSQLite3Database& getDb()
{
    static SQLiteInit db_init;
    static wxSQLite3Database db;

    if (!db.IsOpen())
    {
        std::wstring path = getDbPath();
        
        _unlink(utils::asString(path.c_str()).c_str());

        db.Open(path.c_str());
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

    mmDBWrapper::initDB(&db, 0, wxT(".")); // currency.db3 must be in the current directory
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

} // SUITE

//----------------------------------------------------------------------------

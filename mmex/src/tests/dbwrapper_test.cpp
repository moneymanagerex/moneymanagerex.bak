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
/*************************************************************************
    $Revision$:     Revision of last commit
    $Author$:       Author of last commit
    $Date$:         Date of last commit
**************************************************************************/

//----------------------------------------------------------------------------
#include <UnitTest++.h>
#include <boost/scoped_ptr.hpp>
//----------------------------------------------------------------------------
#include "dbwrapper.h"
#include "utils.h"
//----------------------------------------------------------------------------
#include <wx/filename.h>
//----------------------------------------------------------------------------
#include "mmcategory.h"
#include "mmpayee.h"
#include "mmaccount.h"
#include "mmtransaction.h"

const wxString g_BudgetYear = ("2009");
const wxString g_CategName = ("new category");
const wxString g_SubCategName= ("new subcategory");
const wxString g_CurrencyName = ("US DOLLAR");
const wxString g_PayeeName = ("Payee #1");
//----------------------------------------------------------------------------

wxString getDbPath()
{
    wxFileName fn(wxFileName::GetTempDir(), ("mmex_tests.db3"));
    return fn.GetFullPath();
}
//----------------------------------------------------------------------------

void DumpDates(wxString msg, wxDateTime time_start, wxDateTime time_end)
{
    wxString message = msg;
    msg << " - Test: "
        << "Start: " << time_start.FormatISOTime()
        << " End: " << time_end.FormatISOTime()
        << "\n";

    wxLogDebug(msg.c_str());
}

boost::shared_ptr<wxSQLite3Database> getDb()
{
    static boost::shared_ptr<wxSQLite3Database> db_ptr(new wxSQLite3Database);

    if (!db_ptr->IsOpen())
    {
        wxString path = getDbPath();
        wxRemoveFile(path);

        db_ptr->Open(path);
        CHECK(db_ptr->IsOpen());
    }

    return db_ptr;
}
//----------------------------------------------------------------------------

SUITE(dbwrapper)
{

TEST(wxSQLite3Exception)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();
    CHECK_THROW(db->ExecuteUpdate("update bad_table set bad_col = unknown_value"), wxSQLite3Exception);

    DumpDates("wxSQLite3Exception",time_start, wxDateTime::Now());
}
//----------------------------------------------------------------------------

TEST(HasBackupSupport)
{
    wxDateTime time_start(wxDateTime::Now());
    bool ok = getDb().get()->HasBackupSupport();
    CHECK(ok);

    DumpDates("HasBackupSupport",time_start, wxDateTime::Now());
}
//----------------------------------------------------------------------------

TEST(HasEncryptionSupport)
{
    wxDateTime time_start(wxDateTime::Now());
    bool ok = getDb().get()->HasEncryptionSupport();
    CHECK(ok);

    DumpDates("HasEncryptionSupport",time_start, wxDateTime::Now());
}
//----------------------------------------------------------------------------

TEST(HasMetaDataSupport)
{
    wxDateTime time_start(wxDateTime::Now());
    bool ok = getDb().get()->HasMetaDataSupport();
    CHECK(ok);

    DumpDates("HasMetaDataSupport",time_start, wxDateTime::Now());
}
//----------------------------------------------------------------------------

TEST(HasSavepointSupport)
{
    bool ok = getDb().get()->HasSavepointSupport();
    CHECK(ok);
}
//----------------------------------------------------------------------------

TEST(ViewExists)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();

    bool ok = mmDBWrapper::ViewExists(db, "I'm_not_exists");
    CHECK(!ok);

    db->ExecuteUpdate(
        "create view master_view as "
        "select * "
        "from sqlite_master"
    );

    ok = mmDBWrapper::ViewExists(db, "master_view");
    CHECK(ok);

    db->ExecuteUpdate("drop view master_view");

    DumpDates("ViewExists",time_start, wxDateTime::Now());
}
//----------------------------------------------------------------------------

TEST(initDB)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();

    mmDBWrapper::initDB(db, 0);
    bool ok = false;

    ok = db->TableExists(("ASSETS_V1"));
    CHECK(ok);

    ok = db->TableExists(("ACCOUNTLIST_V1"));
    CHECK(ok);

    ok = db->TableExists(("CATEGORY_V1"));
    CHECK(ok);
    CHECK(!CATEGORY_V1.all(db).empty());

    ok = db->TableExists(("SUBCATEGORY_V1"));
    CHECK(ok);
    CHECK(!SUBCATEGORY_V1.all(db).empty());

    DumpDates("initDB",time_start, wxDateTime::Now());
}
//----------------------------------------------------------------------------

#if 1
TEST(getCurrencySymbol)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();

    wxString s;
    DB_View_CURRENCYFORMATS_V1::Data* currency = CURRENCYFORMATS_V1.get(1, db);
    if (currency)
        s = currency->CURRENCY_SYMBOL;
    CHECK(s == L"EUR");

    s = wxEmptyString;
    currency = CURRENCYFORMATS_V1.get(0, db);
    if (currency)
        s = currency->CURRENCY_SYMBOL;
    CHECK(s.empty());

    DumpDates("getCurrencySymbol",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(checkDBVersion)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();

    bool ok = mmDBWrapper::checkDBVersion(db);
    CHECK(ok);

    DumpDates("checkDBVersion",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(addBudgetYear)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();

    BudgetEntry_Table budget_table(db);

    int year_id = budget_table.GetYearID(g_BudgetYear);
    CHECK(year_id == -1);

    year_id = budget_table.AddYear(g_BudgetYear);
    CHECK(year_id > 0);

    DumpDates("addBudgetYear",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getBudgetYearID)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();

    BudgetYear_Table budget_year_table(db);

    int year_id = budget_year_table.GetYearID("unknown_year");
    CHECK(year_id == -1);

    year_id = budget_year_table.GetYearID(g_BudgetYear);
    CHECK(year_id > 0);

    DumpDates("getBudgetYearID",time_start, wxDateTime::Now());
}

#endif
//----------------------------------------------------------------------------

#if 1
TEST(getBudgetYearForID)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();

    BudgetYear_Table budget_year_table(db);

    int year_id = budget_year_table.GetYearID(g_BudgetYear);
    CHECK(year_id > 0);

    wxString year = budget_year_table.GetYear(year_id);
    CHECK(year == g_BudgetYear);

    DumpDates("getBudgetYearForID",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getBudgetYearForID_2)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    BudgetEntry_Table budget_table(db);

    int year_id = budget_table.GetYearID(g_BudgetYear);
    CHECK(year_id > 0);

    wxString new_year = g_BudgetYear + g_BudgetYear;
    budget_table.UpdateYear(new_year, year_id);

    wxString year = budget_table.GetYear(year_id);
    CHECK(year == new_year);

    // restore original value

    budget_table.UpdateYear(g_BudgetYear, year_id);
    year = budget_table.GetYear(year_id);
    CHECK(year == g_BudgetYear);

    DumpDates("getBudgetYearForID_2",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(copyBudgetYear)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    BudgetEntry_Table budget_table(db);

    int base_year_id = budget_table.GetYearID(g_BudgetYear);
    CHECK(base_year_id > 0);

    int add_id = budget_table.AddEntry(base_year_id,1,1,("None"),100);
    int entry_id = budget_table.GetEntryID(g_BudgetYear);
    CHECK(add_id == entry_id);
    CHECK(entry_id != -1);

    // --

    bool ok = budget_table.CopyYear(base_year_id, base_year_id);
    CHECK(!ok);
    // --

    wxString new_year = g_BudgetYear + g_BudgetYear;
    int new_year_id = budget_table.AddYear(new_year);
    CHECK(new_year_id > 0);
    
    entry_id = budget_table.GetEntryID(new_year);
    CHECK( entry_id == -1);

    // --

    ok = budget_table.CopyYear(base_year_id, new_year_id);
    CHECK(ok);

    DumpDates("copyBudgetYear",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(deleteBudgetYear)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    BudgetEntry_Table budget_table(db);

    bool deleted = budget_table.DeleteYear(("wrong_year"));
    CHECK(!deleted);

    deleted = budget_table.DeleteYear(g_BudgetYear);
    CHECK(deleted);

    DumpDates("deleteBudgetYear",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#if 1
TEST(addCategory)
{
    wxDateTime time_start(wxDateTime::Now());
    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);

    int cat_id = category_table.AddName(g_CategName);
    CHECK(cat_id > 0);

    DumpDates("addCategory",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(addSubCategory)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();

    Category_Table category_table(db);
    SubCategory_Table sub_category_table(db);

    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    int subcat_id = sub_category_table.AddName(g_SubCategName, cat_id);
    CHECK(subcat_id > 0);

    DumpDates("addSubCategory",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getCategoryID)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();

    Category_Table category_table(db);

    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    cat_id = category_table.GetID(("unknown category"));
    CHECK(cat_id == -1);

    DumpDates("getCategoryID",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getSubCategoryID)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();

    Category_Table category_table(db);
    SubCategory_Table subcategory_table(db);

    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    int subcat_id = subcategory_table.GetID(g_SubCategName, cat_id);
    CHECK(subcat_id > 0);

    subcat_id = subcategory_table.GetID(("unknown subcategory"), cat_id);
    CHECK(subcat_id == -1);

    DumpDates("getSubCategoryID",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getCategoryName)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();

    Category_Table category_table(db);
    
    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    wxString name = category_table.GetName(cat_id);
    CHECK(name == g_CategName);

    name = category_table.GetName(0);
    CHECK(name.empty());

    DumpDates("getCategoryName",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getSubCategoryName)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);
    SubCategory_Table subcategory_table(db);

    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    int subcat_id = subcategory_table.GetID(g_SubCategName, cat_id);
    CHECK(subcat_id > 0);

    wxString sct_name = subcategory_table.GetName(cat_id, subcat_id);
    CHECK(sct_name == g_SubCategName);

    sct_name = subcategory_table.GetName(cat_id, 0);
    CHECK(sct_name.empty());

    DumpDates("getSubCategoryName",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(updateCategory)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);
    SubCategory_Table subcategory_table(db);

    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    int sc_id = subcategory_table.GetID(g_SubCategName, cat_id);
    CHECK(sc_id > 0);

    // --

    const wxString new_name = ("new name");
    bool ok = category_table.UpdateName(new_name, cat_id);
    CHECK(ok);

    wxString name = category_table.GetName(cat_id);
    CHECK(name == new_name);

    ok = category_table.UpdateName(g_CategName, cat_id); // restore name
    CHECK(ok);

    // --

    ok = subcategory_table.UpdateName(new_name, cat_id, sc_id);
    CHECK(ok);

    name = subcategory_table.GetName(cat_id, sc_id);
    CHECK(name == new_name);

    ok = subcategory_table.UpdateName(g_SubCategName, cat_id, sc_id); // restore
    CHECK(ok);

    DumpDates("updateCategory",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(DeleteCategoryTest)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);

    int cat_id = category_table.GetID("my_new_category");
    CHECK(cat_id == -1);

    cat_id = category_table.AddName("my_new_category");
    CHECK(cat_id > 0);

    CHECK(category_table.DeleteCategory("my_new_category"));

    cat_id = category_table.GetID("my_new_category");
    CHECK(cat_id == -1);

    DumpDates("DeleteCategoryTest",time_start, wxDateTime::Now());
}
#endif


#if 1
TEST(deleteSubCategoryWithConstraints)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);
    SubCategory_Table subcategory_table(db);

    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    int subcat_id = subcategory_table.GetID(g_SubCategName, cat_id);
    CHECK(subcat_id > 0);

    mmCategoryList category_list(getDb());

    int ok = category_list.deleteSubCategoryWithConstraints(cat_id, subcat_id);
    CHECK(ok == wxID_OK);

    DumpDates("deleteSubCategoryWithConstraints",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(deleteCategoryWithConstraints)
{
    wxDateTime time_start(wxDateTime::Now());

    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);
    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    mmCategoryList category_list(getDb());
    int ok = category_list.deleteCategoryWithConstraints(cat_id);
    CHECK(ok == wxID_OK);

    DumpDates("deleteCategoryWithConstraints",time_start, wxDateTime::Now());
}
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#if 0
TEST(getCurrencyID)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    id = mmDBWrapper::getCurrencyID(&db, ("unknown currency"));
    CHECK(id == -1);
}
#endif
//----------------------------------------------------------------------------

#if 0
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
#endif
//----------------------------------------------------------------------------

#if 0
TEST(setBaseCurrencySettings)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    mmDBWrapper::setBaseCurrencySettings(&db, id);
    
    int base_id = mmDBWrapper::getBaseCurrencySettings(&db);
    CHECK(base_id == id);
}
#endif
//----------------------------------------------------------------------------

#if 0
TEST(getBaseCurrencySettings)
{
    wxSQLite3Database &db = getDb();

    int id = mmDBWrapper::getCurrencyID(&db, g_CurrencyName);
    CHECK(id > 0);

    int base_id = mmDBWrapper::getBaseCurrencySettings(&db);
    CHECK(base_id == id);
}
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#if 0
TEST(loadBaseCurrencySettings)
{
    wxSQLite3Database* db = getDb().get();

    mmDBWrapper::loadBaseCurrencySettings(db);
    CHECK(true);
}
#endif
//----------------------------------------------------------------------------

#if 0
TEST(getCurrencyBaseConvRate)
{
    wxSQLite3Database* db = getDb().get();

    int id = mmDBWrapper::getCurrencyID(db, g_CurrencyName);
    CHECK(id > 0);

    double rate = mmDBWrapper::getCurrencyBaseConvRateForId(db, id);
    CHECK(rate != 0.0);
}
#endif
//----------------------------------------------------------------------------

#if 0
TEST(loadSettings)
{
    wxSQLite3Database* db = getDb().get();

    int id = mmDBWrapper::getCurrencyID(db, g_CurrencyName);
    CHECK(id > 0);

    mmDBWrapper::loadSettings(db, id);
}
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#if 1
TEST(setInfoSettingValue)
{
    wxSQLite3Database* db = getDb().get();

    const wxString name = ("settings name");
    const wxString val = ("settings value");

    mmDBWrapper::setInfoSettingValue(db, name, val);

    wxString s = mmDBWrapper::getInfoSettingValue(db, name, (""));
    CHECK(s == val);
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getInfoSettingValue)
{
    wxSQLite3Database* db = getDb().get();

    const wxString name = ("wrong name");
    const wxString defVal = ("default value");

    wxString s = mmDBWrapper::getInfoSettingValue(db, name, defVal);
    CHECK(s == defVal);

    // --

    mmDBWrapper::setInfoSettingValue(db, name, defVal);
    s = mmDBWrapper::getInfoSettingValue(db, name, (""));
    CHECK(s == defVal);
}
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#if 1
TEST(addPayee)
{
    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);
    SubCategory_Table subcategory_table(db);
    mmPayeeList payee_list(getDb());

    int cat_id = category_table.AddName(g_CategName);
    CHECK(cat_id > 0);

    int subcat_id = subcategory_table.AddName(g_SubCategName, cat_id);
    CHECK(subcat_id > 0);

    // --

    int id = payee_list.add(g_PayeeName);
    CHECK(id > 0);
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getPayeeID)
{
    wxSQLite3Database* db = getDb().get();
    Category_Table category_table(db);
    SubCategory_Table subcategory_table(db);
    mmPayeeList payee_list(getDb());

    int cat_id = category_table.GetID(g_CategName);
    CHECK(cat_id > 0);

    int subcat_id = subcategory_table.GetID(g_SubCategName, cat_id);
    CHECK(subcat_id > 0);

    // --

    int id = payee_list.getID(g_PayeeName);
    CHECK(id > 0);

    id = payee_list.getID(("bad payee name"));
    CHECK(id < 0);
}
#endif
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#if 1
TEST(getPayee)
{
    wxSQLite3Database* db = getDb().get();
    mmPayeeList payee_list(getDb());

    int payee_id = payee_list.getID(g_PayeeName);
    CHECK(payee_id != -1);

    int cat = 0;
    int subc = 0;

    int cat2 = 0;
    int subc2 = 0;
    wxString name = mmDBWrapper::getPayee(db, payee_id, cat2, subc2);
    CHECK(name == g_PayeeName);
    CHECK(cat2 == cat);
    CHECK(subc2 == subc);

    // --

    name = mmDBWrapper::getPayee(db, 0, cat, subc);
    CHECK(name.empty());
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(updatePayee)
{
    wxSQLite3Database* db = getDb().get();
    mmPayeeList payee_list(getDb());

    int id = payee_list.getID(g_PayeeName);
    CHECK(id != -1);

    int cat = 0;
    int subc = 0;

    const wxString new_name = ("new payee name");
    bool ok = mmDBWrapper::updatePayee(db, new_name, id, cat, -1);
    CHECK(ok);

    // --

    int subc2 = 0;
    wxString name = mmDBWrapper::getPayee(db, id, cat, subc2);
    CHECK(name == new_name);
    CHECK(subc2 == -1);

    // restore

    ok = mmDBWrapper::updatePayee(db, g_PayeeName, id, cat, subc);
    CHECK(ok);
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(getAmountForPayee)
{
    mmPayeeList payee_list(getDb());
    mmBankTransactionList transaction_list(getDb());

    int payee_id = payee_list.getID(g_PayeeName);
    CHECK(payee_id != -1);

    // --

    const wxDateTime dt_begin = wxDateTime::Now();
    dt_begin.Subtract(wxDateSpan::Day());

    const wxDateTime dt_end = wxDateTime::Now();

    double amt = transaction_list.getAmountForPayee(payee_id, true,dt_begin, dt_end);
    CHECK(amt == 0.0);
}
#endif
//----------------------------------------------------------------------------

#if 1
TEST(deletePayeeWithConstraints)
{
    wxSQLite3Database* db = getDb().get();

    mmPayeeList payee_list(getDb());

    int payee_id = payee_list.getID(g_PayeeName);
    CHECK(payee_id != -1);
    
    // --

    bool ok = mmDBWrapper::deletePayeeWithConstraints(db, payee_id);
    CHECK(ok);

    // --

    ok = mmDBWrapper::deletePayeeWithConstraints(db, 0);
    CHECK(ok); // returns true even for wrong id
}
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#if 1
TEST(getNumAccounts)
{
    mmAccountList account_list(getDb());
    int count = account_list.getNumAccounts();

    CHECK(!count);
}
#endif
//----------------------------------------------------------------------------

} // SUITE

//----------------------------------------------------------------------------

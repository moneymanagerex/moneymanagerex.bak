/*************************************************************************
 Copyright (C) 2013 Stefano Giorgio

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
 *************************************************************************/

/****************************************************************************
 The tests in this module can be run independantly of other modules.
 This is done by excluding the other modules in testing_util.h
 ****************************************************************************/
#include <UnitTest++.h>
#include "testing_util.h"

#ifdef NEW_CLASSES_TEST_INCLUDED_IN_BUILD
//----------------------------------------------------------------------------
#include "constants.h"

//----------------------------------------------------------------------------
#include "db/settings.h"
#include "db/currency.h"
#include "db/account.h"
#include "db/payee_table.h"
#include "db/category.h"
#include "db/subcategory.h"
#include "db/transaction.h"
#include "db/splittransaction.h"
#include "db/transactionbill.h"
#include "db/stocks.h"
#include "db/assets.h"
#include "db/budget_year.h"
#include "db/budget_table.h"

#if 1
//----------------------------------------------------------------------------
/// Central class holding all major components of the database
class TDatabase
{
public:
    TSettingsList           info_settings_;
    TCurrencyList           currency_list_;
    TAccountList            account_list_;
    TPayeeList              payee_list_;
    TCategoryList           category_list_;
    TSubCategoryList        subcategory_list_;
    TTransactionList        transaction_list_;
    TSplitEntriesList       split_entries_list_;
    TTransactionBillList    transaction_bill_list_;
    TStockList              stock_list_;
    TAssetList              asset_list_;
    TBudgetYearList         budget_year_list_;
    TBudgetTableList        budget_table_list_;

    TDatabase(boost::shared_ptr<wxSQLite3Database> db)
    : info_settings_(db, true)
    , currency_list_(db)
    , account_list_(db, currency_list_)
    , payee_list_(db)
    , category_list_(db)
    , subcategory_list_(db)
    , transaction_list_(db)
    , split_entries_list_(db)
    , transaction_bill_list_(db)
    , stock_list_(db)
    , asset_list_(db)
    , budget_year_list_(db)
    , budget_table_list_(db)
    {}
};

// Create a single access point for the main database, stored in memory.
boost::shared_ptr<TDatabase> main_db()
{
    static boost::shared_ptr<TDatabase> pCore(new TDatabase(get_pDb()));

    return pCore;
}
#endif
/*****************************************************************************************
 The tests for the new classes
 *****************************************************************************************/
SUITE(new_classes_test)
{

#if 1
TEST(Central_Database_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());
    printf("\nCentral_Database_Test: START");
    display_STD_IO_separation_line();

    boost::shared_ptr<wxSQLite3Database> pDB = get_pDb();
    pDB->Begin();
    boost::shared_ptr<TDatabase> pCore = main_db();
    pDB->Commit();

    if (!pCore->info_settings_.Exists(wxT("MMEXVERSION")))
    {
        pCore->info_settings_.SetStringSetting(wxT("MMEXVERSION"), mmex::getProgramVersion());
        pCore->info_settings_.SetStringSetting(wxT("DATAVERSION"), mmex::DATAVERSION);
		pCore->info_settings_.SetStringSetting(wxT("CREATEDATE"), wxDateTime::Now().FormatISODate());
        pCore->info_settings_.SetStringSetting(wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);
        pCore->info_settings_.Save();
    }

    displayTimeTaken(wxT("Central_Database_Test"), start_time);
    display_STD_IO_separation_line();
}
#endif

#if 1
TEST(TCurrencyList_Add)
{
    printf("\nNew_Classes_Test: START");
    display_STD_IO_separation_line();

    const wxDateTime start_time(wxDateTime::UNow());
    TCurrencyList currency_list(get_pDb());

    TCurrencyEntry* pCurrencyEntry = new TCurrencyEntry();
    int id_first = currency_list.AddEntry(pCurrencyEntry);
    CHECK(id_first == 1); // first entry

    TCurrencyEntry* pCurrencyUSD = new TCurrencyEntry();
    int id_USD = currency_list.AddEntry(pCurrencyUSD);
    CHECK(id_USD == id_first); // No duplicates allowed
    delete pCurrencyUSD;

    TCurrencyEntry* pCurrencyAUD = new TCurrencyEntry();
    pCurrencyAUD->currencySymbol_ = CURRENCIES[14];
    pCurrencyAUD->name_           = CURRENCIES[15];
    int id_AUD = currency_list.AddEntry(pCurrencyAUD);
    CHECK(id_first != id_AUD);

    boost::shared_ptr<TCurrencyEntry> pEntry = currency_list.GetEntryPtr(id_first);
    pEntry->baseConv_ = 1.5;
    pEntry->Update(currency_list.ListDatabase());

    displayTimeTaken(wxT("TCurrencyList_Add"), start_time);
}
#endif

#if 1
TEST(TAccountList_Test_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TCurrencyList currency_list(get_pDb());

    TAccountList account_list(get_pDb(), currency_list);
    TAccountEntry* account_entry = new TAccountEntry();
    account_entry->acc_name_    = wxT("Savings");
    account_entry->acc_state_   = ACCOUNT_STATE_DEF[OPEN];
    account_entry->acc_type_    = ACCOUNT_TYPE_DEF[BANK];
    account_entry->currency_id_ = currency_list.GetCurrencyId(wxT("AUD"), true);
    int id_1 = account_list.AddEntry(account_entry);

    account_entry = new TAccountEntry();
    account_entry->acc_name_    = wxT("Cheque");
    account_entry->acc_state_   = ACCOUNT_STATE_DEF[CLOSED];
    account_entry->acc_type_    = ACCOUNT_TYPE_DEF[TERM];
    account_entry->currency_id_ = currency_list.GetCurrencyId(wxT("USD"), true);
    int id_2 = account_list.AddEntry(account_entry);

    CHECK(id_1 != id_2);

    displayTimeTaken(wxT("TAccountList_Test_Add"), start_time);
}
#endif

#if 1
TEST(TCategoryList_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TCategoryList cat_list(get_pDb());

    int cat_id; 
    get_pDb()->Begin();

    cat_id = cat_list.AddEntry(wxT("Food"));
    CHECK(cat_id > 0);
 
    cat_id = cat_list.AddEntry(wxT("Income"));
    CHECK(cat_id > 0);
    
    cat_id = cat_list.AddEntry(wxT("Auto"));
    CHECK(cat_id > 0);

    cat_list.UpdateEntry(cat_id, wxT("Automobile"));
    CHECK_EQUAL(wxT("Automobile"), cat_list.GetCategoryName(cat_id));

    cat_id = cat_list.AddEntry(wxT("Healthcare"));
    CHECK(cat_id > 0);
    
    get_pDb()->Commit();

    boost::shared_ptr<TCategoryEntry> pCatEntry = cat_list.GetEntryPtr(2);
    if (pCatEntry)
    {
        CHECK(true);
        CHECK_EQUAL(wxT("Income"), pCatEntry->name_);
        cat_list.DeleteEntry(pCatEntry->GetId());
    }
    else
    {
        CHECK(false);
    }

    displayTimeTaken(wxT("TCategoryList_Test"), start_time);
}

TEST(TSubCategoryList_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TCategoryList cat_list(get_pDb());
    TSubCategoryList subcat_list(get_pDb());

    cat_list.ListDatabase()->Begin();
    int cat_id = -1;

    cat_id = cat_list.GetCategoryId(wxT("Food"));
    CHECK(cat_id > 0);
    if (cat_id > 0)
    {
        subcat_list.AddEntry(cat_id, wxT("Groceries"));
        subcat_list.AddEntry(cat_id, wxT("Dining Out"));
        subcat_list.AddEntry(cat_id, wxT("Miscellaneous"));
    }

    cat_id = cat_list.GetCategoryId(wxT("Income"));
    CHECK(cat_id <= 0);

    cat_id = cat_list.GetCategoryId(wxT("Automobile"));
    CHECK(cat_id > 0);
    if (cat_id > 0)
    {
        subcat_list.AddEntry(cat_id, wxT("Fuel"));
        subcat_list.AddEntry(cat_id, wxT("Maintenance"));
        subcat_list.AddEntry(cat_id, wxT("Registration"));
        subcat_list.AddEntry(cat_id, wxT("Insurance"));
        subcat_list.AddEntry(cat_id, wxT("Miscellaneous"));
    }

    cat_id = cat_list.GetCategoryId(wxT("Healthcare"));
    CHECK(cat_id > 0);
    if (cat_id > 0)
    {
        subcat_list.AddEntry(cat_id, wxT("Doctor"));
        subcat_list.AddEntry(cat_id, wxT("Dentist"));
        subcat_list.AddEntry(cat_id, wxT("Chemist"));
        subcat_list.AddEntry(cat_id, wxT("Eyecare"));
        subcat_list.AddEntry(cat_id, wxT("Insurance"));
        subcat_list.AddEntry(cat_id, wxT("Miscellaneous"));
    }
    cat_list.ListDatabase()->Commit();

    boost::shared_ptr<TSubCategoryEntry> pSubCatEntry = subcat_list.GetEntryPtr(cat_id, wxT("Insurance"));

    if (pSubCatEntry)
    {
        CHECK(true);
        pSubCatEntry->name_ = wxT("Health Insurance");
        pSubCatEntry->Update(get_pDb().get());                // Test direct update;
    }
    else CHECK(false);

    CHECK(! subcat_list.SubCategoryExists(cat_id, wxT("Insurance")));
    CHECK(subcat_list.SubCategoryExists(cat_id, wxT("Health Insurance")));

    int subcat_id = subcat_list.GetSubCategoryId(cat_id, wxT("Health Insurance"));
    subcat_list.UpdateEntry(cat_id, subcat_id, wxT("Insurance"));
    CHECK(subcat_list.SubCategoryExists(cat_id, wxT("Insurance")));

    subcat_list.DeleteEntry(cat_id, subcat_id);
    CHECK(! subcat_list.SubCategoryExists(cat_id, wxT("Insurance")));

    displayTimeTaken(wxT("TSubCategoryList_Test"), start_time);
}

TEST(TCategoryList_SubList_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TCategoryList cat_list(get_pDb());

    int cat_id = cat_list.GetCategoryId(wxT("Automobile"));
    TSubCategoryList subcat_sublist(get_pDb(), cat_id);

    int subcat_id = subcat_sublist.GetSubCategoryId(cat_id, wxT("Registration"));

    wxString sub_name = subcat_sublist.GetSubCategoryName(cat_id, subcat_id);
    CHECK_EQUAL(wxT("Registration"), sub_name);

    cat_id = cat_list.GetCategoryId(wxT("Food"));
    CHECK(! subcat_sublist.SubCategoryExists(cat_id, wxT("Miscellaneous")));

    displayTimeTaken(wxT("TCategoryList_SubList_Test"), start_time);
}
#endif

#if 1
TEST(TPayeeList_Test_1)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TPayeeList payee_list(get_pDb());
    int payee_id;

    payee_id = payee_list.AddEntry(wxT("Coles"));
    CHECK(payee_id > 0);

    payee_id = payee_list.AddEntry(wxT("Woolworths"));
    CHECK(payee_id > 0);

    payee_id = payee_list.AddEntry(wxT("ACTEW"));
    CHECK(payee_id > 0);

    payee_id = payee_list.GetPayeeId(wxT("Woolworths"));
    CHECK_EQUAL(wxT("Woolworths"), payee_list.GetPayeeName(payee_id));

    payee_list.UpdateEntry(payee_id, wxT("Big W"));
    CHECK_EQUAL(wxT("Big W"), payee_list.GetPayeeName(payee_id));

    payee_list.UpdateEntry(wxT("Coles"), 1, 1);

    boost::shared_ptr<TPayeeEntry> pEntry = payee_list.GetEntryPtr(wxT("Coles"));
    CHECK_EQUAL(wxT("Coles"), pEntry->name_);
    CHECK_EQUAL(1, pEntry->subcat_id_);
    CHECK_EQUAL(1, pEntry->cat_id_);

    payee_list.DeleteEntry(wxT("Big W"));
    CHECK(! payee_list.PayeeExists(wxT("Big W")));

    displayTimeTaken(wxT("TPayeeList_Test_1"), start_time);
}

TEST(TPayeeList_Test_2)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TPayeeList payee_list(get_pDb());
    int payee_id;

    payee_id = payee_list.AddEntry(wxT("Woolworths"));
    CHECK(payee_id > 0);

    payee_id = payee_list.AddEntry(wxT("ACTEW"));
    CHECK(payee_id > 0);

    displayTimeTaken(wxT("TPayeeList_Test_2"), start_time);
}

TEST(TTransactionList_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TTransactionList transactions(get_pDb());
    TTransactionEntry* pTransEntry_1 = new TTransactionEntry();
    pTransEntry_1->amount_from_  = 1000;
    pTransEntry_1->trans_status_ = TRANS_STATE_DEF[TRANS_RECONCILED];
    pTransEntry_1->trans_type_   = TRANS_TYPE_DEF[TRANS_DEPOSIT];
    pTransEntry_1->trans_notes_  = wxT("Transaction Entry");  
    int id_1 = transactions.AddEntry(pTransEntry_1);

    TTransactionEntry* pTransEntry_2 = new TTransactionEntry(pTransEntry_1);

    int id_2 = transactions.AddEntry(pTransEntry_2);
    pTransEntry_1->amount_from_  = 2000;
    pTransEntry_1->Update(transactions.ListDatabase());

    CHECK(id_1 != id_2);
    CHECK(pTransEntry_1->GetId() != pTransEntry_2->GetId());

    displayTimeTaken(wxT("TTransactionList_Add"), start_time);
}

TEST(TTransactionBillList_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TTransactionBillList repeat_transactions(get_pDb());
    TTransactionBillEntry* pBillEntry = new TTransactionBillEntry();
    pBillEntry->amount_from_  = 1000;
    pBillEntry->trans_status_ = TRANS_STATE_DEF[TRANS_RECONCILED];
    pBillEntry->trans_type_   = TRANS_TYPE_DEF[TRANS_DEPOSIT];
    pBillEntry->num_repeats_  = 10;
    pBillEntry->trans_notes_  = wxT("Repeat Transaction Entry");  
    pBillEntry->nextOccurDate_ = start_time.Add(wxDateSpan::Month()).FormatISODate();
    int id_1 = repeat_transactions.AddEntry(pBillEntry);

    TTransactionList transactions(get_pDb());
    int id_2 = transactions.AddEntry(pBillEntry->GetTransaction());

    CHECK(id_1 != id_2);

    displayTimeTaken(wxT("TTransactionBillList_Add"), start_time);
}
#endif

#if 1
TEST(TSplitTransactionList_Test_Create)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TSplitEntriesList global_splits(get_pDb());

    int trans_id = 9;
    TSplitTransactionList split_list_9(trans_id, global_splits);
    
    split_list_9.AddEntry(10,20,100);
    split_list_9.AddEntry(10,21,200);
    split_list_9.AddEntry(10,22,300);
    split_list_9.AddEntry(10,22,400);
    CHECK_EQUAL(1000, split_list_9.TotalAmount());

    trans_id = 10;
    TSplitTransactionList split_list_10(trans_id, global_splits);
    
    split_list_10.AddEntry(10,20,100);
    split_list_10.AddEntry(10,21,200);
    split_list_10.AddEntry(10,22,300);
    split_list_10.AddEntry(10,22,400);
    CHECK_EQUAL(1000, split_list_10.TotalAmount());

    trans_id = 11;
    TSplitTransactionList split_list_11(trans_id, global_splits);
    
    split_list_11.AddEntry(10,20,100);
    split_list_11.AddEntry(10,21,200);
    split_list_11.AddEntry(10,22,300);
    split_list_11.AddEntry(10,22,400);
    CHECK_EQUAL(1000, split_list_11.TotalAmount());

    TSplitTransactionList split_list_2(trans_id, global_splits);
    CHECK_EQUAL(1000, split_list_2.TotalAmount());

    displayTimeTaken(wxT("TSplitTransList_Test_Create"), start_time);
}

TEST(TSplitTransactionList_Test_update)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TSplitEntriesList global_splits(get_pDb());

    int trans_id = 10;
    TSplitTransactionList split_list(trans_id, global_splits);
    CHECK_EQUAL(1000, split_list.TotalAmount());

    int list_size = split_list.GetListSize();
    CHECK_EQUAL(4, list_size);
    // record_id = 3, list_index = 2
    boost::shared_ptr<TSplitEntry> pEntry = split_list.GetIndexedEntryPtr(2);

    pEntry->amount_ = 500;
    split_list.UpdateEntry(pEntry);
    CHECK_EQUAL(1200, split_list.TotalAmount());

    displayTimeTaken(wxT("TSplitTransList_Test_update"), start_time);
}

TEST(TSplitTransactionList_Test_delete)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TSplitEntriesList global_splits(get_pDb());

    int trans_id = 10;
    TSplitTransactionList split_list(trans_id, global_splits);
    CHECK_EQUAL(1200, split_list.TotalAmount());

    int list_size = split_list.GetListSize();
    CHECK_EQUAL(4, list_size);
    boost::shared_ptr<TSplitEntry> pEntry = split_list.GetIndexedEntryPtr(2);
    split_list.DeleteEntry(pEntry);
    CHECK_EQUAL(700, split_list.TotalAmount());
    list_size = split_list.GetListSize();
    CHECK_EQUAL(3, list_size);

    pEntry = split_list.GetIndexedEntryPtr(2);

    int entry_id = pEntry->GetId();

    pEntry = split_list.GetEntryPtr(entry_id);
    if (pEntry)
    {
        split_list.DeleteEntry(pEntry);
        CHECK_EQUAL(300, split_list.TotalAmount());
    }
    displayTimeTaken(wxT("TSplitTransList_Test_delete"), start_time);
}

TEST(TSplitTransactionList_Test_add_after_delete)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TSplitEntriesList global_splits(get_pDb());

    int trans_id = 10;
    TSplitTransactionList split_list(trans_id, global_splits);
    CHECK_EQUAL(300, split_list.TotalAmount());

    int list_size = split_list.GetListSize();
    CHECK_EQUAL(2, list_size);

    double split_entry_id;
    split_entry_id = split_list.AddEntry(10,21,200);
    split_entry_id = split_list.AddEntry(10,22,300);
    CHECK_EQUAL(800, split_list.TotalAmount());

    displayTimeTaken(wxT("TSplitTranList_Test_add_after_delete"), start_time);
}
#endif

#if 1
TEST(TAssetList_Test_entry_with_listed_entry)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxDateTime dt = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
	wxString date = dt.FormatISODate();

    TAssetEntry* asset_entry = new TAssetEntry();
	asset_entry->date_ = date;
    asset_entry->name_ = ASSET_TYPE_DEF[AUTO];
    asset_entry->value_ = 2000;

    TAssetList asset_list(get_pDb());
    int asset_id = asset_list.AddEntry(asset_entry);
    CHECK(asset_id > 0);

    asset_entry->rate_type_ = ASSET_RATE_DEF[DEPRECIATE];
    asset_entry->rate_value_ = 50;
    asset_entry->Update(asset_list.ListDatabase());

    boost::shared_ptr<TAssetEntry> listed_asset_entry = asset_list.GetEntryPtr(asset_id);
    CHECK(listed_asset_entry->name_ == asset_entry->name_);

    displayTimeTaken(wxT("TAssetList_Test_entry_with_listed_entry"), start_time);
}

TEST(TAssetList_Test_Values)
{
    const wxDateTime start_time(wxDateTime::UNow());

	wxDateTime dt = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
	wxString date = dt.FormatISODate();

    TAssetList asset_list(get_pDb());
    CHECK_EQUAL(1, asset_list.CurrentListSize());

    boost::shared_ptr<TAssetEntry> asset_entry = asset_list.GetIndexedEntryPtr(0);
    if (asset_entry)
    {
        CHECK_EQUAL(date, asset_entry->date_);
        CHECK_EQUAL(ASSET_TYPE_DEF[AUTO], asset_entry->name_);
        CHECK_EQUAL(2000, asset_entry->value_);

        double depreciation_value = asset_entry->GetValue();
        CHECK_EQUAL(500, depreciation_value);
 
        asset_entry->rate_type_ = ASSET_RATE_DEF[APPRECIATE];
        double appreciation_value = asset_entry->GetValue();
        CHECK_EQUAL(4500, appreciation_value);

        // wxString str_value;
        // str_value << wxT("\n\nAsset Value: ") << asset_entry->value_;
        // str_value << wxT("     Rate: ") << asset_entry->rate_value_;
        // str_value << wxT("\nDepreciate: ") << depreciation_value;
		// str_value << wxT("     Apreciate: ") << appreciation_value << wxT("\n\n");
        // printf(str_value.char_str());
    }
    else
    {
        CHECK(false);
    }

    int asset_id = asset_entry->GetId();
    asset_list.DeleteEntry(asset_id);

    CHECK_EQUAL(0, asset_list.CurrentListSize());

    displayTimeTaken(wxT("TAssetList_Test_Values"), start_time);
}

TEST(TAssetList_Test_Balance)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxDateTime dt = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
	wxString date = dt.FormatISODate();

    TAssetList asset_list(get_pDb());
    TAssetEntry* asset_entry = new TAssetEntry();
	asset_entry->date_ = date;
    asset_entry->name_ = ASSET_TYPE_DEF[HOUSE];
    asset_entry->value_ = 2000;
    
    int id_1 = asset_list.AddEntry(asset_entry);

    asset_entry = new TAssetEntry();
	asset_entry->date_ = date;
    asset_entry->name_ = ASSET_TYPE_DEF[OTHER];
    asset_entry->value_ = 1000;
    
    int id_2 = asset_list.AddEntry(asset_entry);

    CHECK(id_1 != id_2);

    double value = asset_list.GetAssetBalance();
    CHECK_EQUAL(3000, value);

    displayTimeTaken(wxT("TAssetList_Test_Balance"), start_time);
}

TEST(TAssetList_Test_Delete_entries)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TAssetList asset_list(get_pDb());
    boost::shared_ptr<TAssetEntry> listed_asset_entry;

    while (asset_list.CurrentListSize() > 0)
    {
        listed_asset_entry = asset_list.GetIndexedEntryPtr(0);
        asset_list.DeleteEntry(listed_asset_entry->GetId());
    }

    displayTimeTaken(wxT("TAssetList_Test_Delete_entries"), start_time);
}

TEST(TAssetList_Test_Add_5_years_of_entries)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxDateTime date = wxDateTime::Now();

    TAssetEntry* first_entry = new TAssetEntry();
    first_entry->name_       = ASSET_TYPE_DEF[AUTO];
	first_entry->date_       = date.FormatISODate();
    first_entry->value_      = 20000;
    first_entry->rate_type_  = ASSET_RATE_DEF[DEPRECIATE];
    first_entry->rate_value_ = 20;

    TAssetList asset_list(get_pDb());
    int asset_id = asset_list.AddEntry(first_entry);
    CHECK(asset_id > 0);

    printf("\nAdding 5 years of asset entries...");
    display_STD_IO_separation_line();

    asset_list.ListDatabase()->Begin();
    TAssetEntry* asset_entry;
    for (int i = 1; i < 265; ++i)
    {
        asset_entry = new TAssetEntry(first_entry);
        date = date.Subtract(wxDateSpan::Days(7));
    	first_entry->date_ = date.FormatISODate();
        asset_list.AddEntry(asset_entry);
    }
    asset_list.ListDatabase()->Commit();

    displayTimeTaken(wxT("TAssetList_Test_Add_5_years_of_entries"), start_time);
}

TEST(TAssetList_Test_GetIndexedEntryPtr)
{
    printf("\nTesting speed of Iterators vs indexing...");
    display_STD_IO_separation_line();

    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    boost::shared_ptr<TAssetEntry> pEntry;
    for (unsigned int i = 0; i < asset_list.entrylist_.size(); ++i)
    {
        pEntry = asset_list.GetIndexedEntryPtr(i);
 
        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken(wxT("TAssetList_Test_GetIndexedEntryPtr"), start_time);
}

TEST(TAssetList_Test_const_iterator)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    for (std::vector<boost::shared_ptr<TAssetEntry> >::const_iterator it = asset_list.entrylist_.begin();
        it != asset_list.entrylist_.end(); ++ it)
    {
        const boost::shared_ptr<TAssetEntry> pEntry = *it;

        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken(wxT("TAssetList_Test_const_iterator"), start_time);
    display_STD_IO_separation_line();
}

#endif

#if 1
TEST(TStockList_Test_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxDateTime dt = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
	wxString date = dt.FormatISODate();

    int account_id = 10;

    TStockList stock_list(get_pDb());
    TStockEntry* stock_entry = new TStockEntry();
    stock_entry->heldat_ = account_id;
    stock_entry->name_ = wxT("Stock Name - Should be in Account");
    stock_entry->pur_date_ = date; // date of purchase
    stock_entry->pur_price_ = 1.2275;
    stock_entry->num_shares_ = 1000;
    stock_entry->cur_price_ = 1.575;
    stock_entry->value_ = 2000;
    int id_1 = stock_list.AddEntry(stock_entry);

    stock_entry = new TStockEntry();
    stock_entry->heldat_ = account_id;
    stock_entry->name_ = wxT("Stock Name - Should be in Account");
    stock_entry->pur_date_ = date; // date of purchase
    stock_entry->pur_price_ = 1.7275;
    stock_entry->num_shares_ = 1000;
    stock_entry->cur_price_ = 1.575;
    stock_entry->value_ = 1000;
    int id_2 = stock_list.AddEntry(stock_entry);

    CHECK(id_1 != id_2);

    double value = stock_list.GetStockBalance();
    CHECK_EQUAL(3000, value);

    displayTimeTaken(wxT("TStockList_Test_Add"), start_time);
}

TEST(TStockList_Test_Update)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TStockList stock_list(get_pDb());
    boost::shared_ptr<TStockEntry> stock_entry = stock_list.GetEntryPtr(2); // 2nd entry from test 1
    stock_entry->value_ = 3000;
    stock_entry->Update(stock_list.ListDatabase());

    double value = stock_list.GetStockBalance();
    CHECK_EQUAL(5000, value);

    displayTimeTaken(wxT("TStockList_Test_Update"), start_time);
}

TEST(TStockList_Test_Delete)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TStockList stock_list(get_pDb());
    stock_list.DeleteEntry(1);          // 1st entry from test 1
    double value = stock_list.GetStockBalance();
    CHECK_EQUAL(3000, value);

    displayTimeTaken(wxT("TStockList_Test_Delete"), start_time);
}
#endif

#if 1
TEST(TBudgetYearList_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TBudgetYearList budget_year(get_pDb());

    int year_id = budget_year.AddEntry(wxT("2011"));

    CHECK(year_id > 0);
    displayTimeTaken(wxT("TBudgetYearList_Add"), start_time);
}

#endif

} // End of SUITE: mmex_new_classes_test


//----------------------------------------------------------------------------
#endif // NEW_CLASSES_TEST_INCLUDED_IN_BUILD

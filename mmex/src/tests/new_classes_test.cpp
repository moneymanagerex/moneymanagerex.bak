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


//----------------------------------------------------------------------------
// This is a way to prevent certain tests occuring during development testing
//#define CENTRALL_DB_TESTS
//#define CURRENCY_TESTS
//#define ACCOUNT_TESTS
//#define CATEGORY_TESTS
//#define SUBCATEGORY_TESTS
//#define PAYEE_TESTS
#define TRANSACTION_TESTS
#define REPEAT_TRANSACTION_TESTS
//#define SPLIT_TRANSACTION_TESTS
#define ASSET_TESTS
//#define STOCK_TESTS
//#define BUDGET_TESTS

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

    TDatabase(std::shared_ptr<wxSQLite3Database> db)
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
std::shared_ptr<TDatabase> main_db()
{
    static std::shared_ptr<TDatabase> pCore(new TDatabase(get_pDb()));

    return pCore;
}
/*****************************************************************************************
 The tests for the new classes
 *****************************************************************************************/
SUITE(new_classes_test)
{

#ifdef CENTRALL_DB_TESTS
TEST(Central_Database_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());
    printf("\nCentral_Database_Test: START");
    display_STD_IO_separation_line();

    std::shared_ptr<wxSQLite3Database> pDB = get_pDb();
    pDB->Begin();
    std::shared_ptr<TDatabase> pCore = main_db();
    pDB->Commit();

    if (!pCore->info_settings_.Exists("MMEXVERSION"))
    {
        pCore->info_settings_.SetStringSetting("MMEXVERSION", mmex::getProgramVersion());
        pCore->info_settings_.SetStringSetting("DATAVERSION", mmex::DATAVERSION);
		pCore->info_settings_.SetStringSetting("CREATEDATE", wxDateTime::Now().FormatISODate());
        pCore->info_settings_.SetStringSetting("DATEFORMAT", mmex::DEFDATEFORMAT);
        pCore->info_settings_.Save();
    }

    displayTimeTaken("Central_Database_Test", start_time);
    display_STD_IO_separation_line();
}
#endif

#ifdef CURRENCY_TESTS
/****************************************************************************
 Testing Currency
 ****************************************************************************/
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

    std::shared_ptr<TCurrencyEntry> pEntry = currency_list.GetEntryPtr(id_first);
    pEntry->baseConv_ = 1.5;
    pEntry->Update(currency_list.ListDatabase());

    displayTimeTaken("TCurrencyList_Add", start_time);
}
#endif

#ifdef ACCOUNT_TESTS
/****************************************************************************
 Testing Accounts
 ****************************************************************************/
TEST(TAccountList_Test_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TCurrencyList currency_list(get_pDb());

    TAccountList account_list(get_pDb(), currency_list);
    TAccountEntry* account_entry = new TAccountEntry();
    account_entry->acc_name_    = "Savings";
    account_entry->acc_state_   = ACCOUNT_STATE_DEF[TAccountEntry::STATE_OPEN];
    account_entry->acc_type_    = ACCOUNT_TYPE_DEF[TAccountEntry::TYPE_BANK];
    account_entry->currency_id_ = currency_list.GetCurrencyId("AUD", true);
    int id_1 = account_list.AddEntry(account_entry);

    account_entry = new TAccountEntry();
    account_entry->acc_name_    = "Cheque";
    account_entry->acc_state_   = ACCOUNT_STATE_DEF[TAccountEntry::STATE_CLOSED];
    account_entry->acc_type_    = ACCOUNT_TYPE_DEF[TAccountEntry::TYPE_TERM];
    account_entry->currency_id_ = currency_list.GetCurrencyId("USD", true);
    int id_2 = account_list.AddEntry(account_entry);

    CHECK(id_1 != id_2);

    displayTimeTaken("TAccountList_Test_Add", start_time);
}
#endif

#ifdef CATEGORY_TESTS
/****************************************************************************
 Testing Category
 ****************************************************************************/
TEST(TCategoryList_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TCategoryList cat_list(get_pDb());

    int cat_id; 
    cat_list.ListDatabase()->Begin();

    cat_id = cat_list.AddEntry("Food");
    CHECK(cat_id > 0);
 
    cat_id = cat_list.AddEntry("Income");
    CHECK(cat_id > 0);
    
    cat_id = cat_list.AddEntry("Auto");
    CHECK(cat_id > 0);

    cat_list.UpdateEntry(cat_id, "Automobile");
    CHECK_EQUAL("Automobile", cat_list.GetCategoryName(cat_id));

    cat_id = cat_list.AddEntry("Healthcare");
    CHECK(cat_id > 0);
    
    cat_list.ListDatabase()->Commit();

    std::shared_ptr<TCategoryEntry> pCatEntry = cat_list.GetEntryPtr(2);
    if (pCatEntry)
    {
        CHECK(true);
        CHECK_EQUAL("Income", pCatEntry->name_);
        cat_list.DeleteEntry(pCatEntry->GetId());
    }
    else
    {
        CHECK(false);
    }

    displayTimeTaken("TCategoryList_Test", start_time);
}
#endif

#ifdef SUBCATEGORY_TESTS
/****************************************************************************
 Testing Subcategory
 ****************************************************************************/
TEST(TSubCategoryList_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TCategoryList cat_list(get_pDb());
    TSubCategoryList subcat_list(get_pDb());

    cat_list.ListDatabase()->Begin();
    int cat_id = -1;

    cat_id = cat_list.GetCategoryId("Food");
    CHECK(cat_id > 0);
    if (cat_id > 0)
    {
        subcat_list.AddEntry(cat_id, "Groceries");
        subcat_list.AddEntry(cat_id, "Dining Out");
        subcat_list.AddEntry(cat_id, "Miscellaneous");
    }

    cat_id = cat_list.GetCategoryId("Income");
    CHECK(cat_id <= 0);

    cat_id = cat_list.GetCategoryId("Automobile");
    CHECK(cat_id > 0);
    if (cat_id > 0)
    {
        subcat_list.AddEntry(cat_id, "Fuel");
        subcat_list.AddEntry(cat_id, "Maintenance");
        subcat_list.AddEntry(cat_id, "Registration");
        subcat_list.AddEntry(cat_id, "Insurance");
        subcat_list.AddEntry(cat_id, "Miscellaneous");
    }

    cat_id = cat_list.GetCategoryId("Healthcare");
    CHECK(cat_id > 0);
    if (cat_id > 0)
    {
        subcat_list.AddEntry(cat_id, "Doctor");
        subcat_list.AddEntry(cat_id, "Dentist");
        subcat_list.AddEntry(cat_id, "Chemist");
        subcat_list.AddEntry(cat_id, "Eyecare");
        subcat_list.AddEntry(cat_id, "Insurance");
        subcat_list.AddEntry(cat_id, "Miscellaneous");
    }
    cat_list.ListDatabase()->Commit();

    std::shared_ptr<TSubCategoryEntry> pSubCatEntry = subcat_list.GetEntryPtr(cat_id, "Insurance");

    if (pSubCatEntry)
    {
        CHECK(true);
        pSubCatEntry->name_ = "Health Insurance";
        pSubCatEntry->Update(get_pDb().get());                // Test direct update;
    }
    else CHECK(false);

    CHECK(! subcat_list.SubCategoryExists(cat_id, "Insurance"));
    CHECK(subcat_list.SubCategoryExists(cat_id, "Health Insurance"));

    int subcat_id = subcat_list.GetSubCategoryId(cat_id, "Health Insurance");
    subcat_list.UpdateEntry(cat_id, subcat_id, "Insurance");
    CHECK(subcat_list.SubCategoryExists(cat_id, "Insurance"));

    subcat_list.DeleteEntry(cat_id, subcat_id);
    CHECK(! subcat_list.SubCategoryExists(cat_id, "Insurance"));

    displayTimeTaken("TSubCategoryList_Test", start_time);
}

TEST(TCategoryList_SubList_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TCategoryList cat_list(get_pDb());

    int cat_id = cat_list.GetCategoryId("Automobile");
    TSubCategoryList subcat_sublist(get_pDb(), cat_id);

    int subcat_id = subcat_sublist.GetSubCategoryId(cat_id, "Registration");

    wxString sub_name = subcat_sublist.GetSubCategoryName(cat_id, subcat_id);
    CHECK_EQUAL("Registration", sub_name);

    cat_id = cat_list.GetCategoryId("Food");
    CHECK(! subcat_sublist.SubCategoryExists(cat_id, "Miscellaneous"));

    displayTimeTaken("TCategoryList_SubList_Test", start_time);
}
#endif

#ifdef PAYEE_TESTS
/****************************************************************************
 Testing Payees
 ****************************************************************************/
TEST(TPayeeList_Test_1)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TPayeeList payee_list(get_pDb());
    int payee_id;

    payee_id = payee_list.AddEntry("Coles");
    CHECK(payee_id > 0);

    payee_id = payee_list.AddEntry("Woolworths");
    CHECK(payee_id > 0);

    payee_id = payee_list.AddEntry("ACTEW");
    CHECK(payee_id > 0);

    payee_id = payee_list.GetPayeeId("Woolworths");
    CHECK_EQUAL("Woolworths", payee_list.GetPayeeName(payee_id));

    payee_list.UpdateEntry(payee_id, "Big W");
    CHECK_EQUAL("Big W", payee_list.GetPayeeName(payee_id));

    payee_list.UpdateEntry("Coles", 1, 1);

    std::shared_ptr<TPayeeEntry> pEntry = payee_list.GetEntryPtr("Coles");
    CHECK_EQUAL("Coles", pEntry->name_);
    CHECK_EQUAL(1, pEntry->subcat_id_);
    CHECK_EQUAL(1, pEntry->cat_id_);

    payee_list.DeleteEntry("Big W");
    CHECK(! payee_list.PayeeExists("Big W"));

    displayTimeTaken("TPayeeList_Test_1", start_time);
}

TEST(TPayeeList_Test_2)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TPayeeList payee_list(get_pDb());
    int payee_id;

    payee_id = payee_list.AddEntry("Woolworths");
    CHECK(payee_id > 0);

    payee_id = payee_list.AddEntry("ACTEW");
    CHECK(payee_id > 0);

    displayTimeTaken("TPayeeList_Test_2", start_time);
}
#endif

#ifdef TRANSACTION_TESTS
/****************************************************************************
 Testing Transactions
 ****************************************************************************/
TEST(TTransactionList_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TTransactionList transactions(get_pDb());
    transactions.ListDatabase()->Begin();

    TTransactionEntry* pTransEntry_1 = new TTransactionEntry();
    pTransEntry_1->trans_date_   = start_time.Subtract(wxDateSpan::Month()).FormatISODate();
    pTransEntry_1->amount_from_  = 1000;
    pTransEntry_1->trans_status_ = TRANS_STATE_DEF[TTransactionEntry::STATE_NONE];
    pTransEntry_1->trans_type_   = TRANS_TYPE_DEF[TTransactionEntry::TYPE_DEPOSIT];
    pTransEntry_1->trans_notes_  = "Transaction Entry 1";
    int id_1 = transactions.AddEntry(pTransEntry_1);

    TTransactionEntry* pTransEntry_2 = new TTransactionEntry(pTransEntry_1);
    pTransEntry_2->amount_from_  = 500;
    pTransEntry_2->trans_type_   = TRANS_TYPE_DEF[TTransactionEntry::TYPE_WITHDRAWAL];
    pTransEntry_2->trans_notes_  = "Transaction Entry 2";  
    int id_2 = transactions.AddEntry(pTransEntry_2);

    TTransactionEntry* pTransEntry_3 = new TTransactionEntry(pTransEntry_1);
    pTransEntry_3->amount_from_  = 500;
    pTransEntry_3->amount_to_    = 500;
    pTransEntry_3->trans_type_   = TRANS_TYPE_DEF[TTransactionEntry::TYPE_TRANSFER];
    pTransEntry_3->trans_notes_  = "Transaction Entry 3";
    int id_3 = transactions.AddEntry(pTransEntry_3);
    transactions.ListDatabase()->Commit();

    CHECK_EQUAL(1, id_1);
    CHECK(id_1 == pTransEntry_1->GetId());

    CHECK_EQUAL(2, id_2);
    CHECK(id_2 == pTransEntry_2->GetId());

    CHECK_EQUAL(3, id_3);
    CHECK(id_3 == pTransEntry_3->GetId());

    displayTimeTaken("TTransactionList_Add", start_time);
}

TEST(TTransactionList_Update)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TTransactionList transactions(get_pDb());
    TTransactionEntry* pTransEntry = transactions.GetEntryPtr(2).get();

    CHECK_EQUAL(2, pTransEntry->GetId());
    CHECK_EQUAL(500, pTransEntry->amount_from_);

    pTransEntry->amount_to_ = 500;
    pTransEntry->trans_status_ = TRANS_STATE_DEF[TTransactionEntry::STATE_RECONCILED];
    pTransEntry->Update(transactions.ListDatabase());

    displayTimeTaken("TTransactionList_Update", start_time);
}
#endif

#ifdef REPEAT_TRANSACTION_TESTS
/****************************************************************************
 Testing Repeating Transactions
 ****************************************************************************/
TEST(BillList_Add_first_four_entries)
{
    const wxDateTime start_time(wxDateTime::UNow());
    wxString start_time_str = start_time.FormatISOTime();

    TTransactionBillList repeat_transactions(get_pDb());

    TTransactionBillEntry* pBillEntry = new TTransactionBillEntry();
    pBillEntry->amount_from_   = 1000;
    pBillEntry->trans_status_  = TRANS_STATE_DEF[TTransactionEntry::STATE_RECONCILED];
    pBillEntry->trans_type_    = TRANS_TYPE_DEF[TTransactionEntry::TYPE_DEPOSIT];
    pBillEntry->trans_notes_   = "Repeat - weekly for 10 weeks. Start one month in future";
    pBillEntry->SetNextOccurDate(start_time.Add(wxDateSpan::Month()));
    pBillEntry->repeat_type_   = TTransactionBillEntry::TYPE_WEEKLY;
    pBillEntry->num_repeats_   = 10;
    int bill_id = repeat_transactions.AddEntry(pBillEntry);
    CHECK(bill_id > 0);
    pBillEntry->AdjustNextOccuranceDate();
    CHECK_EQUAL(start_time.Add(wxDateSpan::Month().Add(wxDateSpan::Week())).FormatISODate(),
                pBillEntry->NextOccurDate().FormatISODate());

    int days_remaining = -1;
    bool exec_required = pBillEntry->RequiresExecution(days_remaining);
    CHECK(!exec_required);
    CHECK(days_remaining > 27);

    TTransactionBillEntry* pBillEntry_1 = new TTransactionBillEntry(pBillEntry);
    pBillEntry_1->trans_notes_   = "Repeat - every 10 days. Start one month in past";
    pBillEntry_1->SetNextOccurDate(start_time.Subtract(wxDateSpan::Month()));
    pBillEntry_1->repeat_type_   = TTransactionBillEntry::TYPE_EVERY_X_DAYS;
    pBillEntry_1->autoExecuteManual_ = true;
    bill_id = repeat_transactions.AddEntry(pBillEntry_1);
    CHECK(bill_id > 1);

    exec_required = pBillEntry_1->RequiresExecution(days_remaining);
    CHECK(exec_required);
    CHECK(days_remaining < -27);
    pBillEntry_1->AdjustNextOccuranceDate();

    pBillEntry_1 = new TTransactionBillEntry(pBillEntry);
    pBillEntry_1->autoExecuteManual_ = true;
    pBillEntry_1->autoExecuteSilent_ = true;
    pBillEntry_1->SetNextOccurDate(start_time.Subtract(wxDateSpan::Weeks(2)));
    pBillEntry_1->trans_notes_   = "Repeat in 10 days - auto-silent. Start two weeks in past";
    pBillEntry_1->repeat_type_ = TTransactionBillEntry::TYPE_IN_X_DAYS;
    bill_id = repeat_transactions.AddEntry(pBillEntry_1);
    CHECK(bill_id > 2);

    exec_required = pBillEntry_1->RequiresExecution(days_remaining);
    CHECK(exec_required);
    CHECK(days_remaining > -17);
    pBillEntry_1->AdjustNextOccuranceDate();

    pBillEntry_1 = new TTransactionBillEntry(pBillEntry);
    pBillEntry_1->autoExecuteManual_ = true;
    pBillEntry_1->autoExecuteSilent_ = true;
    pBillEntry_1->SetNextOccurDate(start_time.Subtract(wxDateSpan::Weeks(2)));
    pBillEntry_1->trans_notes_   = "Repeat weekly - auto-silent. Start two weeks in past";
    pBillEntry_1->num_repeats_   = -1;    // repeat indefinately
    bill_id = repeat_transactions.AddEntry(pBillEntry_1);
    CHECK(bill_id > 3);

    exec_required = pBillEntry_1->RequiresExecution(days_remaining);
    CHECK(exec_required);
    CHECK(days_remaining > -17);
    pBillEntry_1->AdjustNextOccuranceDate();

    displayTimeTaken("BillList_Add_first_four_entries", start_time);
}

TEST(BillList_Add_next_two_entries)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TTransactionList transactions(get_pDb());
    TTransactionBillList repeat_transactions(get_pDb());

    TTransactionBillEntry* pBillEntry = new TTransactionBillEntry();
    pBillEntry->amount_from_   = 1000;
    pBillEntry->trans_status_  = TRANS_STATE_DEF[TTransactionBillEntry::STATE_RECONCILED];
    pBillEntry->trans_type_    = TRANS_TYPE_DEF[TTransactionBillEntry::TYPE_DEPOSIT];
    pBillEntry->trans_notes_   = "Repeat Entry: Start- One Month ago, repeat every 10 days";
    pBillEntry->SetNextOccurDate(start_time.Subtract(wxDateSpan::Month()));
    pBillEntry->trans_date_    = pBillEntry->NextOccurDate().FormatISODate();
    pBillEntry->repeat_type_   = TTransactionBillEntry::TYPE_EVERY_X_DAYS;
    pBillEntry->num_repeats_   = 10;
    int id = repeat_transactions.AddEntry(pBillEntry);

    TTransactionBillEntry* pBillEntry_1 = new TTransactionBillEntry(pBillEntry);
    pBillEntry_1->repeat_type_ = TTransactionBillEntry::TYPE_WEEKLY;
    pBillEntry_1->trans_notes_ = "Repeat Entry: Start- One Month ago, repeat weekly";

    int id_1 = repeat_transactions.AddEntry(pBillEntry_1);

    CHECK(id != id_1);

    displayTimeTaken("BillList_Add_next_two_entries", start_time);
}

#ifdef TRANSACTION_TESTS
TEST(BillList_Create_entry_from_transaction)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TTransactionList transactions(get_pDb());
    TTransactionBillList repeat_transactions(get_pDb());

    std::shared_ptr<TTransactionEntry> pTransEntry = transactions.GetEntryPtr(2);

    CHECK_EQUAL(2, pTransEntry->GetId());
    CHECK_EQUAL(500, pTransEntry->amount_from_);

    TTransactionBillEntry* pBillEntry = new TTransactionBillEntry();
    pBillEntry->SetTransaction(pTransEntry);
    pBillEntry->SetNextOccurDate(start_time.Subtract(wxDateSpan::Weeks(2)));
    pBillEntry->repeat_type_   = TTransactionBillEntry::TYPE_BI_WEEKLY;
    repeat_transactions.AddEntry(pBillEntry);

    displayTimeTaken("BillList_Create_entry_from_transaction", start_time);
}
#endif

TEST(BillList_Executing_Entries)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TTransactionList transactions(get_pDb());
    TTransactionBillList repeat_transactions(get_pDb());

    int days_remaining;
    bool continue_Execution = false;

//  TTransactionBillEntry* pBillEntry;
//  for (int i = 0; i < repeat_transactions.CurrentListSize(); ++i)
//  {
//      pBillEntry = repeat_transactions.entrylist_[i].get();

    for (auto& pBillEntry:repeat_transactions.entrylist_)
    {
        if (pBillEntry->RequiresExecution(days_remaining))
        {
            TTransactionEntry* pTransactionEntry = pBillEntry->GetTransaction();
            pTransactionEntry->trans_date_ = pBillEntry->NextOccurDate().FormatISODate();
            transactions.AddEntry(pTransactionEntry);

            wxString date;
            date = pTransactionEntry->DisplayTransactionDate();
            date = pBillEntry->DisplayTransactionDate();
            date = pBillEntry->DisplayNextOccurDate();

            CHECK(pTransactionEntry->trans_date_ != pTransactionEntry->DisplayTransactionDate());
            CHECK(pBillEntry->trans_date_ != pBillEntry->DisplayTransactionDate());
            CHECK(pBillEntry->NextOccurDate().FormatISODate() != pBillEntry->DisplayNextOccurDate());

            pBillEntry->AdjustNextOccuranceDate();
//            CHECK(pTransactionEntry->trans_date_ != pBillEntry->NextOccurDate().FormatISODate());

            pBillEntry->Update(repeat_transactions.ListDatabase());
            continue_Execution = true;
        }
    }

    CHECK(continue_Execution);

#ifdef TRANSACTION_TESTS
    CHECK_EQUAL(9, transactions.CurrentListSize());
#else
    CHECK_EQUAL(5, transactions.CurrentListSize());
#endif

    displayTimeTaken("BillList_Executing_Entries", start_time);
}

#endif

#ifdef SPLIT_TRANSACTION_TESTS
/****************************************************************************
 Testing Split Transactions
 ****************************************************************************/
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

    displayTimeTaken("TSplitTransList_Test_Create", start_time);
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
    std::shared_ptr<TSplitEntry> pEntry = split_list.GetIndexedEntryPtr(2);

    pEntry->amount_ = 500;
    split_list.UpdateEntry(pEntry);
    CHECK_EQUAL(1200, split_list.TotalAmount());

    displayTimeTaken("TSplitTransList_Test_update", start_time);
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
    std::shared_ptr<TSplitEntry> pEntry = split_list.GetIndexedEntryPtr(2);
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
    displayTimeTaken("TSplitTransList_Test_delete", start_time);
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

    displayTimeTaken("TSplitTranList_Test_add_after_delete", start_time);
}
#endif

#ifdef ASSET_TESTS
/****************************************************************************
 Testing Assets
 ****************************************************************************/
TEST(TAssetList_Test_entry_with_listed_entry)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxDateTime date = wxDateTime::Now().Subtract(wxDateSpan::Years(2));

    TAssetEntry* asset_entry = new TAssetEntry();
	asset_entry->date_ = date;
    asset_entry->name_ = ASSET_TYPE_DEF[TAssetEntry::AUTO];
    asset_entry->value_ = 2000;

    TAssetList asset_list(get_pDb());
    int asset_id = asset_list.AddEntry(asset_entry);
    CHECK(asset_id > 0);

    asset_entry->rate_type_ = ASSET_RATE_DEF[TAssetEntry::DEPRECIATE];
    asset_entry->rate_value_ = 50;
    asset_entry->Update(asset_list.ListDatabase());

    std::shared_ptr<TAssetEntry> listed_asset_entry = asset_list.GetEntryPtr(asset_id);
    CHECK(listed_asset_entry->name_ == asset_entry->name_);

    displayTimeTaken("TAssetList_Test_entry_with_listed_entry", start_time);
}

TEST(TAssetList_Test_Values)
{
    const wxDateTime start_time(wxDateTime::UNow());

	wxDateTime date = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
    TAssetList asset_list(get_pDb());
    CHECK_EQUAL(1, asset_list.CurrentListSize());

    std::shared_ptr<TAssetEntry> asset_entry = asset_list.GetIndexedEntryPtr(0);
    if (asset_entry)
    {
        CHECK_EQUAL(date.FormatISODate(), asset_entry->date_.FormatISODate());
        CHECK_EQUAL(ASSET_TYPE_DEF[TAssetEntry::AUTO], asset_entry->name_);
        CHECK_EQUAL(2000, asset_entry->value_);

        double depreciation_value = asset_entry->GetValue();
        CHECK_EQUAL(500, depreciation_value);
 
        asset_entry->rate_type_ = ASSET_RATE_DEF[TAssetEntry::APPRECIATE];
        double appreciation_value = asset_entry->GetValue();
        CHECK_EQUAL(4500, appreciation_value);

        // wxString str_value;
        // str_value << "\n\nAsset Value: " << asset_entry->value_;
        // str_value << "     Rate: " << asset_entry->rate_value_;
        // str_value << "\nDepreciate: " << depreciation_value;
		// str_value << "     Apreciate: " << appreciation_value << "\n\n";
        // printf(str_value.char_str());
    }
    else
    {
        CHECK(false);
    }

    int asset_id = asset_entry->GetId();
    asset_list.DeleteEntry(asset_id);

    CHECK_EQUAL(0, asset_list.CurrentListSize());

    displayTimeTaken("TAssetList_Test_Values", start_time);
}

TEST(TAssetList_Test_Balance)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxDateTime date = wxDateTime::Now().Subtract(wxDateSpan::Years(2));

    TAssetList asset_list(get_pDb());
    TAssetEntry* asset_entry = new TAssetEntry();
	asset_entry->date_ = date;
    asset_entry->name_ = ASSET_TYPE_DEF[TAssetEntry::HOUSE];
    asset_entry->value_ = 2000;
    
    int id_1 = asset_list.AddEntry(asset_entry);

    asset_entry = new TAssetEntry();
	asset_entry->date_ = date;
    asset_entry->name_ = ASSET_TYPE_DEF[TAssetEntry::OTHER];
    asset_entry->value_ = 1000;
    
    int id_2 = asset_list.AddEntry(asset_entry);

    CHECK(id_1 != id_2);

    double value = asset_list.GetAssetBalance();
    CHECK_EQUAL(3000, value);

    displayTimeTaken("TAssetList_Test_Balance", start_time);
}

TEST(TAssetList_Test_Delete_entries)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TAssetList asset_list(get_pDb());
    std::shared_ptr<TAssetEntry> listed_asset_entry;

    while (asset_list.CurrentListSize() > 0)
    {
        listed_asset_entry = asset_list.GetIndexedEntryPtr(0);
        asset_list.DeleteEntry(listed_asset_entry->GetId());
    }

    displayTimeTaken("TAssetList_Test_Delete_entries", start_time);
}

TEST(TAssetList_Test_Add_5_years_of_entries)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    TAssetEntry* new_entry = new TAssetEntry();
    new_entry->name_       = ASSET_TYPE_DEF[TAssetEntry::AUTO];
	new_entry->date_       = start_time;
    new_entry->value_      = 20000;
    new_entry->rate_type_  = ASSET_RATE_DEF[TAssetEntry::DEPRECIATE];
    new_entry->rate_value_ = 20;

    int asset_id = asset_list.AddEntry(new_entry);
    CHECK(asset_id > 0);

    printf("\nAdding 5 years of asset entries...");
    display_STD_IO_separation_line();

    asset_list.ListDatabase()->Begin();
    for (int i = 1; i < 263; ++i)
    {
    	new_entry->date_ = start_time.Subtract(wxDateSpan::Days(7));
        asset_list.AddEntry(new TAssetEntry(new_entry));
    }
    asset_list.ListDatabase()->Commit();

    displayTimeTaken("TAssetList_Test_Add_5_years_of_entries", start_time);
}

#if 1
TEST(TAssetList_Test_Depreciate_Daily)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    const wxString line_feed = "\n";
    const double init_value = 20000;
    double new_value = init_value;
    double dep_rate = (init_value/365) * 0.2; // 20% pa
    int days = 0;
    std::shared_ptr<TAssetEntry> pEntry;
    for (int i = 0; i < asset_list.CurrentListSize(); ++i)
    {
        pEntry = asset_list.GetIndexedEntryPtr(i);
        CHECK_EQUAL(init_value, pEntry->value_);

        new_value = init_value - (dep_rate*days);
        if (new_value < 0) new_value = 0;

//        CHECK_EQUAL(new_value, pEntry->GetValue());

        wxString str_value = line_feed;
        str_value << "Date: " << pEntry->DisplayDate();
        str_value << "   Expected Value: " << wxString::Format("%.2f", new_value);
        str_value << "   Value: " << wxString::Format("%.2f", pEntry->GetValue());
        printf(str_value.char_str());

        days +=7;
    }

    printf(line_feed.char_str());
    displayTimeTaken("TAssetList_Test_Depreciate_Daily", start_time);
    display_STD_IO_separation_line();
}
#endif

#if 0
TEST(TAssetList_Test_Depreciate_Monthly)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    const wxString line_feed = "\n";
    const double init_value = 20000;
    double new_value = init_value;
    double dep_rate = (init_value/12) * 0.2; // 20% pa
    int months = 0;
    std::shared_ptr<TAssetEntry> pEntry;
    for (unsigned int i = 0; i < asset_list.entrylist_.size(); ++i)
    {
        pEntry = asset_list.GetIndexedEntryPtr(i);
        CHECK_EQUAL(init_value, pEntry->value_);

        new_value = init_value - (dep_rate*months);
        if (new_value < 0) new_value = 0;

//        CHECK_EQUAL(new_value, pEntry->GetValue());

        wxString str_value = line_feed;
        str_value << "Date: " << pEntry->date_;
        str_value << "   Expected Value: " << wxString::Format("%.2f", new_value);
        str_value << "   Value: " << wxString::Format("%.2f", pEntry->GetValue());
        printf(str_value.char_str());

        months ++;
    }

    printf(line_feed.char_str());
    displayTimeTaken("TAssetList_Test_Depreciate_Monthly", start_time);
    display_STD_IO_separation_line();
}
#endif

TEST(TAssetList_GetIndexedEntryPtr_Test)
{
    printf("\nTesting speed of Iterators vs indexing...");
    display_STD_IO_separation_line();

    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    std::shared_ptr<TAssetEntry> pEntry;
    for (unsigned int i = 0; i < asset_list.entrylist_.size(); ++i)
    {
        pEntry = asset_list.GetIndexedEntryPtr(i);
 
        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken("TAssetList_GetIndexedEntryPtr_Test", start_time);
}

TEST(TAssetList_const_iterator_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    for (std::vector<std::shared_ptr<TAssetEntry> >::const_iterator it = asset_list.entrylist_.begin();
        it != asset_list.entrylist_.end(); ++ it)
    {
        const std::shared_ptr<TAssetEntry> pEntry = *it;

        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken("TAssetList_const_iterator_Test", start_time);
}

TEST(TAssetList_const_iterator_Retest)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    for (std::vector<std::shared_ptr<TAssetEntry> >::const_iterator it = asset_list.entrylist_.begin();
        it != asset_list.entrylist_.end(); ++ it)
    {
        const std::shared_ptr<TAssetEntry> pEntry = *it;

        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken("TAssetList_const_iterator_Retest", start_time);
}

TEST(TAssetList_GetIndexedEntryPtr_Retest)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    std::shared_ptr<TAssetEntry> pEntry;
    for (unsigned int i = 0; i < asset_list.entrylist_.size(); ++i)
    {
        pEntry = asset_list.GetIndexedEntryPtr(i);
 
        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken("TAssetList_GetIndexedEntryPtr_Retest", start_time);
}

TEST(TAssetList_Smart_const_iterator)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    for (const auto& pEntry:asset_list.entrylist_)
    {
        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken("TAssetList_Smart_const_iterator", start_time);
}


TEST(TAssetList_Test_GetEntryPtr)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TAssetList asset_list(get_pDb());

    std::shared_ptr<TAssetEntry> pEntry;
    for (unsigned int i = 0; i < asset_list.entrylist_.size(); ++i)
    {
        pEntry = asset_list.GetIndexedEntryPtr(i);
        pEntry = asset_list.GetEntryPtr(pEntry->GetId());

        CHECK_EQUAL(20000, pEntry->value_);
    }

    displayTimeTaken("TAssetList_Test_GetEntryPtr", start_time);
    display_STD_IO_separation_line();
}
#endif

#ifdef STOCK_TESTS
/****************************************************************************
 Testing Stocks
 ****************************************************************************/
TEST(TStockList_Test_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());

    wxDateTime dt = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
	wxString date = dt.FormatISODate();

    int account_id = 10;

    TStockList stock_list(get_pDb());
    TStockEntry* stock_entry = new TStockEntry();
    stock_entry->heldat_ = account_id;
    stock_entry->name_ = "Stock Name - Should be in Account";
    stock_entry->pur_date_ = date; // date of purchase
    stock_entry->pur_price_ = 1.2275;
    stock_entry->num_shares_ = 2000;
    stock_entry->cur_price_ = 1.575;
    stock_entry->value_ = 2000;
    int id_1 = stock_list.AddEntry(stock_entry);

    stock_entry = new TStockEntry();
    stock_entry->heldat_ = account_id;
    stock_entry->name_ = "Stock Name - Should be in Account";
    stock_entry->pur_date_ = date; // date of purchase
    stock_entry->pur_price_ = 1.7275;
    stock_entry->num_shares_ = 1000;
    stock_entry->cur_price_ = 1.575;
    stock_entry->value_ = 1000;
    int id_2 = stock_list.AddEntry(stock_entry);

    CHECK(id_1 != id_2);

    double value = stock_list.GetStockBalance();
    CHECK_EQUAL(3000, value);

    displayTimeTaken("TStockList_Test_Add", start_time);
}

TEST(TStockList_Test_Update)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TStockList stock_list(get_pDb());
    int stock_id = 2;        // 2nd entry from test 1
    stock_list.GetEntryPtr(stock_id);   // test setting current index
    std::shared_ptr<TStockEntry> stock_entry = stock_list.GetIndexedEntryPtr(stock_list.GetCurrentIndex());
    stock_entry->value_ = 3000;
    stock_entry->Update(stock_list.ListDatabase());

    double value = stock_list.GetStockBalance();
    CHECK_EQUAL(5000, value);

    wxString check_value = "1.5750";
    wxString return_value = stock_entry->CurrentPrice();
    CHECK_EQUAL(check_value, return_value);

    check_value = "1,575.00";
    return_value = stock_entry->GetValueCurrencyEditFormat();
    CHECK_EQUAL(check_value, return_value);

    check_value = "1000.0000";
    return_value = stock_entry->NumberOfShares(false);
    CHECK_EQUAL(check_value, return_value);

    displayTimeTaken("TStockList_Test_Update", start_time);
}

TEST(TStockList_Test_Delete)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TStockList stock_list(get_pDb());
    stock_list.DeleteEntry(1);          // 1st entry from test 1
    double value = stock_list.GetStockBalance();
    CHECK_EQUAL(3000, value);

    displayTimeTaken("TStockList_Test_Delete", start_time);
}
#endif

#ifdef BUDGET_TESTS
/****************************************************************************
 Testing Budgets
 ****************************************************************************/
TEST(TBudgetYearList_Add)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TBudgetYearList budget_year(get_pDb());

    int year_id = budget_year.AddEntry("2011");

    CHECK(year_id > 0);
    displayTimeTaken("TBudgetYearList_Add", start_time);
}

#endif

} // End of SUITE: mmex_new_classes_test


//----------------------------------------------------------------------------
#endif // NEW_CLASSES_TEST_INCLUDED_IN_BUILD

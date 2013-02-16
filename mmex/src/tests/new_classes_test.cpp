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
 These tests can be run independantly of other tests by excluding
 dbwrapper_test.cpp from the project. Alternatively, this module can be
 prevented from running by excluding mmex_lua_test.cpp from the project.

 Note: Total test time is meaningless with this module included.

 ****************************************************************************/
#include <UnitTest++.h>
#include "testing_util.h"

#ifdef NEW_CLASSES_TEST_INCLUDED_IN_BUILD
//----------------------------------------------------------------------------
#include "db_payee.h"
#include "db_category.h"
#include "db_subcategory.h"
#include "db_transaction.h"
#include "db_splittransaction.h"
#include "db_assets.h"

//----------------------------------------------------------------------------

/*****************************************************************************************
 The tests for the new classes
 *****************************************************************************************/
SUITE(new_classes_test)
{

TEST(TCategoryList_Test)
{
    printf("\nnew_classes_test: START");
    display_STD_IO_separation_line();

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

    get_pDb()->Begin();
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
    get_pDb()->Commit();

    boost::shared_ptr<TSubCategoryEntry> pSubCatEntry = subcat_list.GetEntryPtr(cat_id, wxT("Insurance"));

    if (pSubCatEntry)
    {
        CHECK(true);
        pSubCatEntry->name_ = wxT("Health Insurance");
        pSubCatEntry->UpdateSubcat(get_pDb().get());                // Test direct update;
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

TEST(TPayeeList_test_1)
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

    displayTimeTaken(wxT("TPayeeList_test_1"), start_time);
}

TEST(TPayeeList_test_2)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TPayeeList payee_list(get_pDb());
    int payee_id;

    payee_id = payee_list.AddEntry(wxT("Woolworths"));
    CHECK(payee_id > 0);

    payee_id = payee_list.AddEntry(wxT("ACTEW"));
    CHECK(payee_id > 0);

    displayTimeTaken(wxT("TPayeeList_test_2"), start_time);
}

TEST(TAssetList_test_1)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TAssetList asset_list(get_pDb());
    int asset_id;

    asset_id = asset_list.AddEntry();
    CHECK(asset_id > 0);

    boost::shared_ptr<TAssetEntry> asset_entry = asset_list.GetEntryPtr(asset_id);
	
	wxDateTime dt = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
	wxString date = dt.FormatISODate();

	asset_entry->date_ = date;
    asset_entry->name_ = wxT("Automobile");
    asset_entry->value_ = 2000;
    asset_entry->rate_type_ = ASSET_RATE_DEF[ASSET_RATE_DEPRECIATE];
    asset_entry->rate_value_ = 50;
    asset_list.UpdateEntry(asset_id);

    displayTimeTaken(wxT("TAssetList_test_1"), start_time);
}

TEST(TAssetList_test_2)
{
    const wxDateTime start_time(wxDateTime::UNow());

	wxDateTime dt = wxDateTime::Now().Subtract(wxDateSpan::Years(2));
	wxString date = dt.FormatISODate();

    TAssetList asset_list(get_pDb());

    CHECK_EQUAL(1, asset_list.CurrentListSize());

    boost::shared_ptr<TAssetEntry> asset_entry = asset_list.GetListIndexEntryPtr(0);
    if (asset_entry)
    {
        CHECK(true);
        CHECK_EQUAL(date, asset_entry->date_);
        CHECK_EQUAL(wxT("Automobile"), asset_entry->name_);
        CHECK_EQUAL(2000, asset_entry->value_);

        double depreciation_value = asset_entry->GetAssetValue();
        CHECK_EQUAL(500, depreciation_value);
 
        asset_entry->rate_type_ = ASSET_RATE_DEF[ASSET_RATE_APPRECIATE];
        double appreciation_value = asset_entry->GetAssetValue();
        CHECK_EQUAL(4500, appreciation_value);
        
  //      wxString str_value;
		//str_value << wxT("\n\nAsset Value: ") << asset_entry->value_;
  //      str_value << wxT("     Rate: ") << asset_entry->rate_value_;
  //      str_value << wxT("\nDepreciate: ") << depreciation_value;
		//str_value << wxT("     Apreciate: ") << appreciation_value << wxT("\n\n");
  //      printf(str_value.char_str());
    }
    else
    {
        CHECK(false);
    }

    int asset_id = asset_entry->GetId();
    asset_list.DeleteEntry(asset_id);

    CHECK_EQUAL(0, asset_list.CurrentListSize());

    displayTimeTaken(wxT("TAssetList_test_2"), start_time);
}

TEST(TSplitTransactionList_test_create)
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

    displayTimeTaken(wxT("TSplitTransList_test_create"), start_time);
}

TEST(TSplitTransactionList_test_update)
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

    displayTimeTaken(wxT("TSplitTransList_test_update"), start_time);
}

TEST(TSplitTransactionList_test_delete)
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
    displayTimeTaken(wxT("TSplitTransList_test_delete"), start_time);
}

TEST(TSplitTransactionList_test_add_after_delete)
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

    displayTimeTaken(wxT("TSplitTranList_test_add_after_delete"), start_time);
}


} // End of SUITE: mmex_new_classes_test


//----------------------------------------------------------------------------
#endif // NEW_CLASSES_TEST_INCLUDED_IN_BUILD

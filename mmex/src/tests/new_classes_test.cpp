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
/*****************************************************************************************
 The tests for the new classes
 *****************************************************************************************/
SUITE(new_classes_test)
{

// ----------------------------------------------------------------------------
// The test: init_DB has been duplicated in this suite to ensure that the
// database is initialised when this module is used on its own.
// ----------------------------------------------------------------------------
TEST(init_DB)
{
    printf("\nnew_classes_test: START");
    display_STD_IO_separation_line();
    const wxDateTime start_time(wxDateTime::UNow());

    mmCoreDB* pCore = pDb_core().get();
    CHECK(pCore->displayDatabaseError_ == true);

    displayTimeTaken(wxT("init_DB"), start_time);    
}
//----------------------------------------------------------------------------
#if 1
TEST(Category_ID_Test)
{
    const wxDateTime start_time(wxDateTime::UNow());
    mmCoreDB* pCore = pDb_core().get();

    pCore->categoryList_.entries_.clear();
    pCore->categoryList_.LoadCategories();

    int cat_id_1 = pCore->categoryList_.AddCategory(wxT("Category_ID_Test_1"));
    int cat_id_2 = pCore->categoryList_.AddCategory(wxT("Category_ID_Test_2"));
    int cat_id_3 = pCore->categoryList_.AddCategory(wxT("Category_ID_Test_3"));
    int cat_id_4 = pCore->categoryList_.AddCategory(wxT("Category_ID_Test_4"));
    pCore->categoryList_.AddCategory(wxT("Category_ID_Test_5"));

    pCore->categoryList_.DeleteCategory(cat_id_2);
    pCore->categoryList_.DeleteCategory(cat_id_3);
    pCore->categoryList_.DeleteCategory(cat_id_4);

    pCore->db_.get()->Vacuum();

    cat_id_2 = pCore->categoryList_.AddSubCategory(cat_id_1, wxT("Category_ID_Test_2"));
    cat_id_3 = pCore->categoryList_.AddSubCategory(cat_id_1, wxT("Category_ID_Test_3"));
    cat_id_4 = pCore->categoryList_.AddSubCategory(cat_id_1, wxT("Category_ID_Test_4"));

    int cat_id = pCore->categoryList_.AddCategory(g_CategName);
    int id = pCore->categoryList_.GetCategoryId(g_CategName);
    pCore->categoryList_.DeleteCategory(cat_id);

    CHECK(id == cat_id);

    pCore->categoryList_.entries_.clear();
    pCore->categoryList_.LoadCategories();

    displayTimeTaken(wxT("Category_ID_Test"), start_time);
}

TEST(New_Category_Test_1)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TCategoryList category_list(get_pDb());
    category_list.LoadCategoryList();
    int cat_id = category_list.AddCategory(wxT("New_Cat_Test_1 Category"));
    CHECK(cat_id > -1);

    int subcat_id = category_list.AddSubCategory(cat_id, wxT("New_Cat_Test_1 - SubCategory"));
    CHECK(subcat_id > -1);

    displayTimeTaken(wxT("New_Category_Test"), start_time);
}

TEST(New_Category_Test_2)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TCategoryList category_list(get_pDb());
    category_list.LoadCategoryList();
    
    int cat_id = category_list.GetCategoryId(wxT("Unknown Cat Test Category"));

    CHECK(cat_id == -1);
    cat_id = category_list.GetCategoryId(wxT("New_Cat_Test_1 Category"));
    CHECK(cat_id > -1);

    int subcat_id = category_list.GetSubCategoryId(cat_id, wxT("Unknown Cat Test SubCategory"));
    CHECK(subcat_id == -1);
    subcat_id = category_list.GetSubCategoryId(cat_id, wxT("New_Cat_Test_1 - SubCategory"));
    CHECK(subcat_id > -1);

    wxString fullCatName = category_list.GetFullCategoryName(cat_id, subcat_id);
    CHECK_EQUAL(fullCatName, wxT("New_Cat_Test_1 Category:New_Cat_Test_1 - SubCategory"));

    displayTimeTaken(wxT("New_SubCategory_Test"), start_time);
}
#endif


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

    int entry_id = pEntry->GetSplitEntryId();

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

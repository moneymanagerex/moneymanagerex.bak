==============================================================================
How to generate the file: ./src/mmex_db_view_.h
==============================================================================

Required Files:
./utils/mmex.py                 // Python script file
./sql/tables_v1.sql             // MMEX Table Definitions
./utils/gen_mmex_db_view.bat    // Windows batch file
                                // to create file: ./src/mmex_db_view.h

==============================================================================
Using the Windows command prompt:
Navigate to your local directory: trunk\mmex\utils

Use the following commands:
gen_mmex_db_view

==============================================================================
Creating the file without using the batch file:

C:\..\trunk\mmex>python utils/mmex.py sql/tables_v1.sql > src/mmex_db_view.h
C:\..\trunk\mmex>

==============================================================================
When using python file: cpplint.py
to redirect output, use:  2> outputfile.txt
==============================================================================


==============================================================================
Concepts using ./src/mmex_db_view_.h 
==============================================================================
struct DB_View_ACCOUNTLIST_V1 : public DB_View
struct DB_View_ASSETS_V1 : public DB_View
struct DB_View_BILLSDEPOSITS_V1 : public DB_View
struct DB_View_BUDGETSPLITTRANSACTIONS_V1 : public DB_View
struct DB_View_BUDGETTABLE_V1 : public DB_View
struct DB_View_BUDGETYEAR_V1 : public DB_View
struct DB_View_CATEGORY_V1 : public DB_View
struct DB_View_CHECKINGACCOUNT_V1 : public DB_View
struct DB_View_CURRENCYFORMATS_V1 : public DB_View
struct DB_View_INFOTABLE_V1 : public DB_View
struct DB_View_PAYEE_V1 : public DB_View
struct DB_View_SETTING_V1 : public DB_View
struct DB_View_SPLITTRANSACTIONS_V1 : public DB_View
struct DB_View_STOCK_V1 : public DB_View
struct DB_View_SUBCATEGORY_V1 : public DB_View

==============================================================================
static variables definitions for accessing the database
==============================================================================
static DB_View_ACCOUNTLIST_V1 ACCOUNTLIST_V1;
static DB_View_ASSETS_V1 ASSETS_V1;
static DB_View_BILLSDEPOSITS_V1 BILLSDEPOSITS_V1;
static DB_View_BUDGETSPLITTRANSACTIONS_V1 BUDGETSPLITTRANSACTIONS_V1;
static DB_View_BUDGETTABLE_V1 BUDGETTABLE_V1;
static DB_View_BUDGETYEAR_V1 BUDGETYEAR_V1;
static DB_View_CATEGORY_V1 CATEGORY_V1;
static DB_View_CHECKINGACCOUNT_V1 CHECKINGACCOUNT_V1;
static DB_View_CURRENCYFORMATS_V1 CURRENCYFORMATS_V1;
static DB_View_INFOTABLE_V1 INFOTABLE_V1;
static DB_View_PAYEE_V1 PAYEE_V1;
static DB_View_SETTING_V1 SETTING_V1;
static DB_View_SPLITTRANSACTIONS_V1 SPLITTRANSACTIONS_V1;
static DB_View_STOCK_V1 STOCK_V1;
static DB_View_SUBCATEGORY_V1 SUBCATEGORY_V1;
Matching lines: 15

==============================================================================
static variables definitions to use for accessing the database
==============================================================================
ACCOUNTLIST_V1
ASSETS_V1
BILLSDEPOSITS_V1
BUDGETSPLITTRANSACTIONS_V1
BUDGETTABLE_V1
BUDGETYEAR_V1
CATEGORY_V1
CHECKINGACCOUNT_V1
CURRENCYFORMATS_V1
INFOTABLE_V1
PAYEE_V1
SETTING_V1
SPLITTRANSACTIONS_V1
STOCK_V1
SUBCATEGORY_V1
==============================================================================
Matching lines: 15

use static global variables using dot notation:
example:

ASSETS_V1.all(...);
ASSETS_V1.create();
ASSETS_V1.ensure(...);
ASSETS_V1.remove(...);
ASSETS_V1.save(...);


==============================================================================
example to check the item present or not
==============================================================================
DB_View_CHECKINGACCOUNT_V1::Data* transaction = CHECKINGACCOUNT_V1.get(id, db_);
if (transaction)    // existing
{
    // Add code for existing
}
else                // not present
{
    // Add code for not exist
}

===========================================================================
one more example
===========================================================================
std::vector<DB_View_CHECKINGACCOUNT_V1::Data> all_trans
    = CHECKINGACCOUNT_V1.find<DB_View_CHECKINGACCOUNT_V1::CATEGID, int>(db_, 91);

===========================================================================

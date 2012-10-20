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
#ifndef _MM_EX_CONSTANTS_H_
#define _MM_EX_CONSTANTS_H_

#include "defs.h"
/*************************************************************************
 Revision of last commit: $Revision$
 Author of last commit:   $Author$

 To set a release build, remove the comments for the definition:
 #define _MM_EX_BUILD_TYPE_RELEASE

 then commit the changes to the SVN repository.
 *************************************************************************/
//#define _MM_EX_BUILD_TYPE_RELEASE
#define _MM_EX_REVISION_ID    wxT("$Rev$")

//----------------------------------------------------------------------------
class wxString;
//----------------------------------------------------------------------------

namespace mmex
{

/*
        Uses for titles, reports, etc.
        Do not use mmex::GetAppName() for such things.
*/
wxString getProgramName();
wxString getProgramVersion();
wxString getProgramCopyright();
wxString getProgramWebSite();
wxString getProgramFacebookSite();
wxString getProgramDescription();
} // namespace mmex

const wxString VIEW_TRANS_ALL_STR            = wxTRANSLATE("View All Transactions");
const wxString VIEW_TRANS_RECONCILED_STR     = wxTRANSLATE("View Reconciled");
const wxString VIEW_TRANS_NOT_RECONCILED_STR = wxTRANSLATE("View Not-Reconciled");
const wxString VIEW_TRANS_UNRECONCILED_STR   = wxTRANSLATE("View UnReconciled");
const wxString VIEW_TRANS_VOID               = wxTRANSLATE("View Void");
const wxString VIEW_TRANS_FLAGGED            = wxTRANSLATE("View Flagged");
const wxString VIEW_TRANS_DUPLICATES         = wxTRANSLATE("View Duplicates");
const wxString VIEW_TRANS_TODAY_STR          = wxTRANSLATE("View Today");
const wxString VIEW_TRANS_CURRENT_MONTH_STR  = wxTRANSLATE("View Current Month");
const wxString VIEW_TRANS_LAST_30_DAYS_STR   = wxTRANSLATE("View Last 30 days");
const wxString VIEW_TRANS_LAST_90_DAYS_STR   = wxTRANSLATE("View Last 90 days");
const wxString VIEW_TRANS_LAST_MONTH_STR     = wxTRANSLATE("View Last Month");
const wxString VIEW_TRANS_LAST_3MONTHS_STR   = wxTRANSLATE("View Last 3 Months");

const wxString NAVTREECTRL_REPORTS = wxT("Reports");
const wxString NAVTREECTRL_HELP = wxT("Help");
const wxString NAVTREECTRL_CUSTOM_REPORTS = wxT("Custom_Reports");
const wxString NAVTREECTRL_INVESTMENT = wxT("Stocks");
const wxString NAVTREECTRL_BUDGET = wxT("Budgeting");

const wxString  VIEW_ACCOUNTS_ALL_STR       = wxT("ALL");
const wxString  VIEW_ACCOUNTS_OPEN_STR      = wxT("Open");
const wxString  VIEW_ACCOUNTS_FAVORITES_STR = wxT("Favorites");

const wxString INIDB_UPDATE_CURRENCY_RATE   = wxT("UPDATECURRENCYRATE");
const wxString INIDB_USE_TRANSACTION_SOUND  = wxT("USETRANSSOUND");
const wxString INIDB_USE_ORG_DATE_COPYPASTE = wxT("USEORIGDATEONCOPYPASTE");

const wxString INIDB_BUDGET_FINANCIAL_YEARS       = wxT("BUDGET_FINANCIAL_YEARS");
const wxString INIDB_BUDGET_INCLUDE_TRANSFERS     = wxT("BUDGET_INCLUDE_TRANSFERS");
const wxString INIDB_BUDGET_SETUP_WITHOUT_SUMMARY = wxT("BUDGET_SETUP_WITHOUT_SUMMARY");
const wxString INIDB_BUDGET_SUMMARY_WITHOUT_CATEG = wxT("BUDGET_SUMMARY_WITHOUT_CATEGORIES");
const wxString INIDB_IGNORE_FUTURE_TRANSACTIONS   = wxT("IGNORE_FUTURE_TRANSACTIONS");

const wxString TIPS_BANKS  = wxT("checkingpanel");
const wxString TIPS_BILLS  = wxT("billsdeposits");
const wxString TIPS_STOCK  = wxT("investment");
const wxString TIPS_ASSETS = wxT("assets");

const wxString ASSET_TYPE[] = {
    wxTRANSLATE("Property"),
    wxTRANSLATE("Automobile"),
    wxTRANSLATE("Household Object"),
    wxTRANSLATE("Art"),
    wxTRANSLATE("Jewellery"),
    wxTRANSLATE("Cash"),
    wxTRANSLATE("Other")
};

const wxString BILLSDEPOSITS_REPEATS[] = {
    wxTRANSLATE("None"),
    wxTRANSLATE("Weekly"),
    wxTRANSLATE("Bi-Weekly"),
    wxTRANSLATE("Monthly"),
    wxTRANSLATE("Bi-Monthly"),
    wxTRANSLATE("Quarterly"),
    wxTRANSLATE("Half-Yearly"),
    wxTRANSLATE("Yearly"),
    wxTRANSLATE("Four Months"),
    wxTRANSLATE("Four Weeks"),
    wxTRANSLATE("Daily"),
    wxTRANSLATE("In %s Days"),
    wxTRANSLATE("In %s Months"),
    wxTRANSLATE("Every %s Days"),
    wxTRANSLATE("Every %s Months")
};

static const wxString TRANSACTION_STATUS[] =
{
    wxTRANSLATE("None"),
    wxTRANSLATE("Reconciled"),
    wxTRANSLATE("Void"),
    wxTRANSLATE("Follow up"),
    wxTRANSLATE("Duplicate")
};

static const wxString TRANSACTION_TYPE[] =
{
    wxTRANSLATE("Withdrawal"),
    wxTRANSLATE("Deposit"),
    wxTRANSLATE("Transfer")
};

static const wxString gDaysInWeek[7] =
{
    wxTRANSLATE("Sunday"),
    wxTRANSLATE("Monday"),
    wxTRANSLATE("Tuesday"),
    wxTRANSLATE("Wednesday"),
    wxTRANSLATE("Thursday"),
    wxTRANSLATE("Friday"),
    wxTRANSLATE("Saturday")
};

static const wxString TIPS[] = {
	wxTRANSLATE("Recommendation: Always backup your .mmb database file regularly."),
	wxTRANSLATE("Recommendation: If upgrading to a new version of MMEX, make sure you backup your .mmb database file before doing so."),
	wxTRANSLATE("Recommendation: Use copy (Ctrl+С) and paste (Ctrl+V) for frequently used transactions."),
	wxTRANSLATE("Tip: Remember to make backups of your .mmb."),
	wxTRANSLATE("Tip: The .mmb file is not encrypted. That means anyone else having the proper know how can actually open the file and read the contents. So make sure that if you are storing any sensitive financial information it is properly guarded."),
	wxTRANSLATE("Tip: To mark a transaction as reconciled, just select the transaction and hit the 'r' or 'R' key. To mark a transaction as unreconciled, just select the transaction and hit the 'u' or 'U' key."),
	wxTRANSLATE("Tip: To mark a transaction as requiring followup, just select the transaction and hit the 'f' or 'F' key."),
	wxTRANSLATE("Tip: MMEX supports printing of all reports that can be viewed. The print options are available under the menu, File->Print."),
	wxTRANSLATE("Tip: You can modify some runtime behavior of MMEX by changing the options in the Options Dialog. "),
	wxTRANSLATE("Tip: To print a statement with transactions from any arbitary set of criteria, use the transaction filter to select the transactions you want and then do a print from the menu."),
	wxTRANSLATE("Tip: Set exchange rate for currencies in case if you have accounts with different currencies."),

	wxTRANSLATE("Organize Categories Dialog Tip: Pressing the h key will cycle through all categories starting with the letter h"),
	wxTRANSLATE("Organize Categories Dialog Tip: Pressing 2 key combination will cycle through all categories starting with that key combination. Example: Pressing ho will select Homeneeds, Home, House Tax, etc..."),
	wxTRANSLATE("Organize Payees Dialog Tip: Using the % key as a wildcard when using the filter. Example: %c shows Chemist and Doctor, %c%m shows Chemist only."),

	wxTRANSLATE("Tip to get out of debt: Pay yourself 10% first. Put this into an account that is hard to touch. Make sure it is a chore to get the money out (you have to drive to the bank), so you will only tap it consciously and for major expenses."),
	wxTRANSLATE("Tip to get out of debt: Establish an emergency fund."),
	wxTRANSLATE("Tip to get out of debt: Stop acquiring new debt."),
	wxTRANSLATE("Tip to get out of debt: Create a realistic budget for your expenses."),
	wxTRANSLATE("Tip to get out of debt: Spend less than you earn."),
	wxTRANSLATE("Tip to get out of debt: Pay more than the minimum."),
	wxTRANSLATE("Before going to a shop and buy something: take the time making a list of what you really need. In the shop buy what is in your list.")
};

//----------------------------------------------------------------------------
#endif // _MM_EX_CONSTANTS_H_
//----------------------------------------------------------------------------


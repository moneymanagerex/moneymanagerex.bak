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

//----------------------------------------------------------------------------
#endif // _MM_EX_CONSTANTS_H_
//----------------------------------------------------------------------------

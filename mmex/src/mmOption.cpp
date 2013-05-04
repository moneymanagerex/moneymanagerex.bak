/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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


#include "mmOption.h"
#include "util.h"
#include "mmex.h"
#include "univcsvdialog.h"
#include "paths.h"
#include "constants.h"
#include "singleton.h"

#include <wx/sstream.h>
#include <wx/numformatter.h>

//----------------------------------------------------------------------------
mmOptions::mmOptions()
:   dateFormat_(mmex::DEFDATEFORMAT)
    , language_("english")
    , databaseUpdated_(false)
{}

//----------------------------------------------------------------------------
mmOptions& mmOptions::instance()
{
    return Singleton<mmOptions>::instance();
}

//----------------------------------------------------------------------------
void mmOptions::loadOptions(MMEX_IniSettings* info_table)
{
    dateFormat_     = info_table->GetStringSetting("DATEFORMAT", mmex::DEFDATEFORMAT);
    userNameString_ = info_table->GetStringSetting("USERNAME", "");

    financialYearStartDayString_   = info_table->GetStringSetting("FINANCIAL_YEAR_START_DAY", "1");
    financialYearStartMonthString_ = info_table->GetStringSetting("FINANCIAL_YEAR_START_MONTH", "7");
}

//----------------------------------------------------------------------------
void mmOptions::saveOptions(MMEX_IniSettings* info_table)
{
    info_table->SetStringSetting("DATEFORMAT", dateFormat_);
}

// --------------------------------------------------------------------------
void mmIniOptions::loadOptions(std::shared_ptr<MMEX_IniSettings> pIniSettings)
{
    expandStocksHome_ = pIniSettings->GetBoolSetting("ENABLESTOCKS", true);
    enableAssets_     = pIniSettings->GetBoolSetting("ENABLEASSETS", true);
    enableBudget_     = pIniSettings->GetBoolSetting("ENABLEBUDGET", true);
    enableGraphs_     = pIniSettings->GetBoolSetting("ENABLEGRAPHS", true);

    html_font_size_   = pIniSettings->GetIntSetting("HTMLFONTSIZE", 3);

    expandBankHome_   = pIniSettings->GetBoolSetting("EXPAND_BANK_HOME", true);
    expandTermHome_   = pIniSettings->GetBoolSetting("EXPAND_TERM_HOME", false);
    expandBankTree_   = pIniSettings->GetBoolSetting("EXPAND_BANK_TREE", true);
    expandTermTree_   = pIniSettings->GetBoolSetting("EXPAND_TERM_TREE", false);

    budgetFinancialYears_           = pIniSettings->GetBoolSetting(INIDB_BUDGET_FINANCIAL_YEARS, false);
    budgetIncludeTransfers_         = pIniSettings->GetBoolSetting(INIDB_BUDGET_INCLUDE_TRANSFERS, false);
    budgetSetupWithoutSummaries_    = pIniSettings->GetBoolSetting(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, false);
    budgetSummaryWithoutCategories_ = pIniSettings->GetBoolSetting(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, true);
    ignoreFutureTransactions_       = pIniSettings->GetBoolSetting(INIDB_IGNORE_FUTURE_TRANSACTIONS, false);

    // Read the preference as a string and convert to int
    transPayeeSelectionNone_ = pIniSettings->GetIntSetting("TRANSACTION_PAYEE_NONE", 0);

    // For the category selection, default behavior should remain that the last category used for the payee is selected.
    //  This is item 1 (0-indexed) in the list.
    transCategorySelectionNone_ = pIniSettings->GetIntSetting("TRANSACTION_CATEGORY_NONE", 1);
    transStatusReconciled_      = pIniSettings->GetIntSetting("TRANSACTION_STATUS_RECONCILED", 0);
    transDateDefault_           = pIniSettings->GetIntSetting("TRANSACTION_DATE_DEFAULT", 0);
}


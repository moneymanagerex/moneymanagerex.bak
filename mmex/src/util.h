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

#ifndef _MM_EX_UTIL_H_
#define _MM_EX_UTIL_H_

#include "defs.h"

#include <wx/wxprec.h>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>

enum appStartDialog {APP_START_NEW_DB, APP_START_OPEN, APP_START_HELP, APP_START_WEB, APP_START_LAST_DB};

class mmCoreDB;
class mmCurrency;

struct ValuePair 
{
    wxString label;
    double   amount;
};

class mmListBoxItem: public wxClientData
{
public:
    mmListBoxItem(int index, const wxString& name) 
        : index_(index), name_(name)
    {}
    
    int getIndex() const { return index_; }
    wxString getName() const { return name_; }

private:
    int index_;
    wxString name_;
};

class mmTreeItemData : public wxTreeItemData
{
public:
    mmTreeItemData(int id, bool isBudget): id_(id), isString_(false), isBudgetingNode_(isBudget) {}
    mmTreeItemData(const wxString& string): id_(), isString_(true), isBudgetingNode_(false), stringData_(string) {}

    int getData() const { return id_; }
    wxString getString() const { return stringData_; }
    bool isStringData() const { return isString_; }
    bool isBudgetingNode() const { return isBudgetingNode_; }

private:
    int id_; 
    bool isString_;
    bool isBudgetingNode_;
    wxString stringData_;
};

const wxString NAVTREECTRL_REPORTS = ("Reports");
const wxString NAVTREECTRL_HELP = ("Help");
const wxString NAVTREECTRL_CUSTOM_REPORTS = ("Custom_Reports");
const wxString NAVTREECTRL_INVESTMENT = ("Stocks");
const wxString NAVTREECTRL_BUDGET = ("Budgeting");

const wxString VIEW_TRANS_ALL_STR            = ("View All Transactions");
const wxString VIEW_TRANS_RECONCILED_STR     = ("View Reconciled");
const wxString VIEW_TRANS_NOT_RECONCILED_STR = ("View Not-Reconciled");
const wxString VIEW_TRANS_UNRECONCILED_STR   = ("View UnReconciled");
const wxString VIEW_TRANS_VOID               = ("View Void");
const wxString VIEW_TRANS_FLAGGED            = ("View Flagged");
const wxString VIEW_TRANS_DUPLICATES         = ("View Duplicates");
const wxString VIEW_TRANS_TODAY_STR          = ("View Today"); 
const wxString VIEW_TRANS_CURRENT_MONTH_STR  = ("View Current Month");
const wxString VIEW_TRANS_LAST_30_DAYS_STR   = ("View Last 30 days");
const wxString VIEW_TRANS_LAST_90_DAYS_STR   = ("View Last 90 days");
const wxString VIEW_TRANS_LAST_MONTH_STR     = ("View Last Month");
const wxString VIEW_TRANS_LAST_3MONTHS_STR   = ("View Last 3 Months");

const wxString  VIEW_ACCOUNTS_ALL_STR       = ("ALL");
const wxString  VIEW_ACCOUNTS_OPEN_STR      = ("Open");
const wxString  VIEW_ACCOUNTS_FAVORITES_STR = ("Favorites");

const wxString INIDB_UPDATE_CURRENCY_RATE   = ("UPDATECURRENCYRATE");
const wxString INIDB_USE_TRANSACTION_SOUND  = ("USETRANSSOUND");
const wxString INIDB_USE_ORG_DATE_COPYPASTE = ("USEORIGDATEONCOPYPASTE");

const wxString INIDB_BUDGET_FINANCIAL_YEARS       = ("BUDGET_FINANCIAL_YEARS");
const wxString INIDB_BUDGET_INCLUDE_TRANSFERS     = ("BUDGET_INCLUDE_TRANSFERS");
const wxString INIDB_BUDGET_SETUP_WITHOUT_SUMMARY = ("BUDGET_SETUP_WITHOUT_SUMMARY");
const wxString INIDB_BUDGET_SUMMARY_WITHOUT_CATEG = ("BUDGET_SUMMARY_WITHOUT_CATEGORIES");
const wxString INIDB_IGNORE_FUTURE_TRANSACTIONS   = ("IGNORE_FUTURE_TRANSACTIONS");

const wxString TIPS_BANKS  = ("checkingpanel");
const wxString TIPS_BILLS  = ("billsdeposits");
const wxString TIPS_STOCK  = ("investment");
const wxString TIPS_ASSETS = ("assets");

void mmShowErrorMessageInvalid( wxWindow *parent, const wxString &message );
void mmShowErrorMessage( wxWindow *parent, const wxString &message, const wxString &messageheader );

wxString mmSelectLanguage(wxWindow *parent, wxSQLite3Database *inidb, bool forced_show_dlg, bool save_setting = true);

wxString mmGetDateForStorage( const wxDateTime &dt );
wxDateTime mmGetStorageStringAsDate( const wxString& str );
wxString mmGetDateForDisplay( wxSQLite3Database* db, const wxDateTime &dt );
wxDateTime mmParseDisplayStringToDate( wxSQLite3Database* db, const wxString& dt, const wxString& date_format );
wxString mmGetNiceDateString( const wxDateTime &dt );
wxString mmGetNiceDateSimpleString( const wxDateTime &dt );
wxString mmGetNiceMonthName(const int month );
wxString mmGetNiceShortMonthName(const int month );
wxString mmGetNiceWeekDayName(const int week_day );
wxString mmGetShortWeekDayName(const int week_day);
wxString DisplayDate2FormatDate(wxString strDate);
wxString FormatDate2DisplayDate(wxString strDate);
wxArrayString DateFormats();
wxArrayString itemChoiceStrings();

wxString inQuotes(wxString label, wxString& delimiter);
wxString csv2tab_separated_values(wxString line, wxString& delimit);
void mmExportQIF( mmCoreDB* core, wxSQLite3Database* db_ );
wxString mmReadyDisplayString( const wxString& orig );
wxString adjustedExportAmount(wxString amtSeparator, wxString strValue);
void fixFileExt(wxFileName &f, const wxString &ext);

int mmImportQIF( mmCoreDB* core, wxString destinationAccountName = wxEmptyString );

void correctEmptyFileExt(wxString ext, wxString & fileName );

void mmLoadColorsFromDatabase();

void mmPlayTransactionSound();

/*
   mmOptions caches the options for MMEX
   so that we don't hit the DB that often
   for data.
*/
class mmOptions
{
public:
    mmOptions();
    static mmOptions& instance();
    void loadOptions( wxSQLite3Database* db );
    void saveOptions( wxSQLite3Database* db );

    wxString dateFormat;
    wxString language;
    wxString financialYearStartDayString_;
    wxString financialYearStartMonthString_;
    bool databaseUpdated_;
};

class mmIniOptions
{
public:
    mmIniOptions();
    static mmIniOptions& instance();
    void loadOptions( wxSQLite3Database* db );
    void loadInfoOptions( wxSQLite3Database* db );
    void saveOptions( wxSQLite3Database* db );

    bool enableAssets_;
    bool enableBudget_;
    bool enableGraphs_;
    bool enableAddAccount_;
    bool enableDeleteAccount_;
    bool enableRepeatingTransactions_;
    bool enableCustomLogo_;
    bool enableCheckForUpdates_;
    bool enableReportIssues_;
    bool enableBeNotifiedForNewReleases_;
    bool enableVisitWebsite_;
    wxString logoName_;
    long font_size_;
    bool enableCustomAboutDialog_;
    wxString aboutCompanyName_;
    bool disableCategoryModify_;
    wxString userNameString_;
    bool enableCustomTemplateDB_;
    wxString customTemplateDB_;

    bool expandBankHome_;
    bool expandTermHome_;
    bool expandStocksHome_;
    bool expandBankTree_;
    bool expandTermTree_;

    bool budgetFinancialYears_;
    bool budgetIncludeTransfers_;
    bool budgetSetupWithoutSummaries_;
    bool budgetSummaryWithoutCategories_;
    bool ignoreFutureTransactions_;
   
    int transPayeeSelectionNone_;
    int transCategorySelectionNone_;
    int transStatusReconciled_;
    int transDateDefault_;
};

class mmColors
{
public:
    static wxColour listAlternativeColor0;
    static wxColour listAlternativeColor1;
    static wxColour listBorderColor;
    static wxColour listBackColor;
    static wxColour navTreeBkColor;
    static wxColour listDetailsPanelColor;
    static wxColour listFutureDateColor;
};
//----------------------------------------------------------------------------

namespace mmex
{

template <class T> 
class Singleton 
{ 
public: 
    static inline T& instance() 
    { 
        static T _instance; 
        return _instance; 
    } 

private: 
    Singleton(void); 
    ~Singleton(void); 
    Singleton(const Singleton<T>&); 
    Singleton<T>& operator= (const Singleton<T> &); 
}; 

class CurrencyFormatter
{
public:
    CurrencyFormatter();
	static CurrencyFormatter& instance();

    void loadDefaultSettings();
    void loadSettings(const mmCurrency &cur);

    void loadSettings(const wxString &pfx, const wxString &sfx, 
        	    wxChar dec, wxChar grp, 
        	    const wxString &unit, const wxString &cent, 
        	    int scale 
        	    );

    wxString getPrefix() const { return m_pfx_symbol; }
    wxString getSuffix() const { return m_sfx_symbol; }

    wxChar getDecimalPoint() const { return m_decimal_point; }
    wxChar getGroupSeparator() const { return m_group_separator; }

	int getScale() const { return m_scale; }

private:
    wxString m_pfx_symbol;   // Leading currency symbol
    wxString m_sfx_symbol;   // Trailing currency symbol
    wxChar m_decimal_point;  // Character for 100ths
    wxChar m_group_separator;// Character for 1000nds
    wxString m_unit_name;    // Name of monetary unit
    wxString m_cent_name;    // Name of fraction unit
    int m_scale;
};
//----------------------------------------------------------------------------

void formatDoubleToCurrency( double val, wxString& rdata );
void formatDoubleToCurrencyEdit( double val, wxString& rdata );
bool formatCurrencyToDouble( const wxString& str, double& val );

//----------------------------------------------------------------------------

} // namespace mmex

namespace boost
{
 	inline std::size_t hash_value(const wxChar* string)
	{
		long int_key = 0;

		while (*string) {
			int_key += static_cast<wxUChar>(*string++);
	        }

		return int_key;
	}

   /* problems using wxWidgets 2.9.2
    */
	inline std::size_t hash_value(const wxString& value)
	{
        size_t key = 0;
        for(wxString::const_iterator it = value.begin(); it != value.end(); ++ it)
            key += static_cast<wxUChar>(*it);

        return key;
	}

} // namespace boost

//----------------------------------------------------------------------------

namespace DateTimeProviders {

struct Today
{
	inline static wxDateTime StartRange()
	{
		return wxDateTime::Now().GetDateOnly();
	}
	inline static wxDateTime EndRange()
	{
		return StartRange() + wxTimeSpan(23, 59, 59, 999);
	}
};

template <int year, wxDateTime::Month month, int day>
struct CustomDate
{
	inline static wxDateTime StartRange()
	{
		return wxDateTime(day, month, year, 0, 0, 0, 0);
	}
	inline static wxDateTime EndRange()
	{
		return wxDateTime(day, month, year, 23, 59, 59, 999);
	}
};

template <int Period, typename CurrentData = Today>
struct LastDays
{
	inline static wxDateTime StartRange()
	{
		wxDateTime today = CurrentData::StartRange();
		return today.Subtract((Period - 1) * wxDateSpan::Day());;
	}
	inline static wxDateTime EndRange()
	{
		return CurrentData::EndRange();
	}
};

template <int MonthsAgoStart, int MonthsAgoEnd = 0, typename CurrentData = Today>
struct LastMonths
{
	BOOST_STATIC_ASSERT(MonthsAgoStart >= MonthsAgoEnd);

	inline static wxDateTime StartRange()
	{
		wxDateTime datePast = CurrentData::StartRange().Subtract(MonthsAgoStart * wxDateSpan::Month());
		wxDateTime result(1, datePast.GetMonth(), datePast.GetYear());
        	return result;
	}
	inline static wxDateTime EndRange()
	{
		return StartRange().Add((MonthsAgoStart-MonthsAgoEnd+1) * wxDateSpan::Month()).Subtract(wxTimeSpan(0,0,0,1));
	}
};

template <typename CurrentData = Today>
struct CurrentMonth: public LastMonths<0, 0, CurrentData> { };

}; // namespace DateTimeProviders

int getTransformedTrxStatus(const wxString& in);
wxString getTransformedTrxStatus(int in);
wxString Tips(wxString type);
wxArrayString viewTransactionsStrings(bool translated, wxString input_string, int& row_id);
int site_content(const wxString& site, wxString& output);


#endif // _MM_EX_UTIL_H_
//----------------------------------------------------------------------------

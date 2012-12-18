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

 For development build, comment the definition: _MM_EX_BUILD_TYPE_RELEASE
 For a release build, remove the comment to the definition:

 #define _MM_EX_BUILD_TYPE_RELEASE

 then commit the changes to the SVN repository.
 *************************************************************************/
//#define _MM_EX_BUILD_TYPE_RELEASE

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
    wxTRANSLATE("Organize Payees Dialog Tip: Using the * key as a wildcard when using the filter. Example: *c shows Chemist and Doctor, *c*m shows Chemist only."),

    wxTRANSLATE("Tip to get out of debt: Pay yourself 10% first. Put this into an account that is hard to touch. Make sure it is a chore to get the money out (you have to drive to the bank), so you will only tap it consciously and for major expenses."),
    wxTRANSLATE("Tip to get out of debt: Establish an emergency fund."),
    wxTRANSLATE("Tip to get out of debt: Stop acquiring new debt."),
    wxTRANSLATE("Tip to get out of debt: Create a realistic budget for your expenses."),
    wxTRANSLATE("Tip to get out of debt: Spend less than you earn."),
    wxTRANSLATE("Tip to get out of debt: Pay more than the minimum."),
    wxTRANSLATE("Before going to a shop and buy something: take the time making a list of what you really need. In the shop buy what is in your list.")
};

static const wxString CURRENCIES[] = {
    wxT("AFN"),  wxT("Afghanistan Afghani"),
    wxT("ALL"),  wxT("Albania Lek"),
    wxT("DZD"),  wxT("Algeria Dinar"),
    wxT("AOA"),  wxT("Angola Kwanza"),
    wxT("ARS"),  wxT("Argentina Peso"),
    wxT("AMD"),  wxT("Armenia Dram"),
    wxT("AWG"),  wxT("Aruba Guilder"),
    wxT("AUD"),  wxT("Australia Dollar"),
    wxT("AZN"),  wxT("Azerbaijan New Manat"),
    wxT("BSD"),  wxT("Bahamas Dollar"),
    wxT("BHD"),  wxT("Bahrain Dinar"),
    wxT("BDT"),  wxT("Bangladesh Taka"),
    wxT("BBD"),  wxT("Barbados Dollar"),
    wxT("BYR"),  wxT("Belarus Ruble"),
    wxT("BZD"),  wxT("Belize Dollar"),
    wxT("BMD"),  wxT("Bermuda Dollar"),
    wxT("BTN"),  wxT("Bhutan Ngultrum"),
    wxT("BOB"),  wxT("Bolivia Boliviano"),
    wxT("BAM"),  wxT("Bosnia and Herzegovina Convertible Marka"),
    wxT("BWP"),  wxT("Botswana Pula"),
    wxT("BRL"),  wxT("Brazil Real"),
    wxT("BND"),  wxT("Brunei Darussalam Dollar"),
    wxT("BGN"),  wxT("Bulgaria Lev"),
    wxT("BIF"),  wxT("Burundi Franc"),
    wxT("KHR"),  wxT("Cambodia Riel"),
    wxT("CAD"),  wxT("Canada Dollar"),
    wxT("CVE"),  wxT("Cape Verde Escudo"),
    wxT("KYD"),  wxT("Cayman Islands Dollar"),
    wxT("CLP"),  wxT("Chile Peso"),
    wxT("CNY"),  wxT("China Yuan Renminbi"),
    wxT("COP"),  wxT("Colombia Peso"),
    wxT("XOF"),  wxT("Communauté Financière Africaine (BCEAO) Franc"),
    wxT("KMF"),  wxT("Comoros Franc"),
    wxT("XPF"),  wxT("Comptoirs Français du Pacifique (CFP) Franc"),
    wxT("CDF"),  wxT("Congo/Kinshasa Franc"),
    wxT("CRC"),  wxT("Costa Rica Colon"),
    wxT("HRK"),  wxT("Croatia Kuna"),
    wxT("CUC"),  wxT("Cuba Convertible Peso"),
    wxT("CUP"),  wxT("Cuba Peso"),
    wxT("CZK"),  wxT("Czech Republic Koruna"),
    wxT("DKK"),  wxT("Denmark Krone"),
    wxT("DJF"),  wxT("Djibouti Franc"),
    wxT("DOP"),  wxT("Dominican Republic Peso"),
    wxT("XCD"),  wxT("East Caribbean Dollar"),
    wxT("EGP"),  wxT("Egypt Pound"),
    wxT("SVC"),  wxT("El Salvador Colon"),
    wxT("ERN"),  wxT("Eritrea Nakfa"),
    wxT("ETB"),  wxT("Ethiopia Birr"),
    wxT("EUR"),  wxT("Euro Member Countries"),
    wxT("FKP"),  wxT("Falkland Islands (Malvinas) Pound"),
    wxT("FJD"),  wxT("Fiji Dollar"),
    wxT("GMD"),  wxT("Gambia Dalasi"),
    wxT("GEL"),  wxT("Georgia Lari"),
    wxT("GHS"),  wxT("Ghana Cedi"),
    wxT("GIP"),  wxT("Gibraltar Pound"),
    wxT("GTQ"),  wxT("Guatemala Quetzal"),
    wxT("GGP"),  wxT("Guernsey Pound"),
    wxT("GNF"),  wxT("Guinea Franc"),
    wxT("GYD"),  wxT("Guyana Dollar"),
    wxT("HTG"),  wxT("Haiti Gourde"),
    wxT("HNL"),  wxT("Honduras Lempira"),
    wxT("HKD"),  wxT("Hong Kong Dollar"),
    wxT("HUF"),  wxT("Hungary Forint"),
    wxT("ISK"),  wxT("Iceland Krona"),
    wxT("INR"),  wxT("India Rupee"),
    wxT("IDR"),  wxT("Indonesia Rupiah"),
    wxT("XDR"),  wxT("International Monetary Fund (IMF) Special Drawing Rights"),
    wxT("IRR"),  wxT("Iran Rial"),
    wxT("IQD"),  wxT("Iraq Dinar"),
    wxT("IMP"),  wxT("Isle of Man Pound"),
    wxT("ILS"),  wxT("Israel Shekel"),
    wxT("JMD"),  wxT("Jamaica Dollar"),
    wxT("JPY"),  wxT("Japan Yen"),
    wxT("JEP"),  wxT("Jersey Pound"),
    wxT("JOD"),  wxT("Jordan Dinar"),
    wxT("KZT"),  wxT("Kazakhstan Tenge"),
    wxT("KES"),  wxT("Kenya Shilling"),
    wxT("KPW"),  wxT("Korea (North) Won"),
    wxT("KRW"),  wxT("Korea (South) Won"),
    wxT("KWD"),  wxT("Kuwait Dinar"),
    wxT("KGS"),  wxT("Kyrgyzstan Som"),
    wxT("LAK"),  wxT("Laos Kip"),
    wxT("LVL"),  wxT("Latvia Lat"),
    wxT("LBP"),  wxT("Lebanon Pound"),
    wxT("LSL"),  wxT("Lesotho Loti"),
    wxT("LRD"),  wxT("Liberia Dollar"),
    wxT("LYD"),  wxT("Libya Dinar"),
    wxT("LTL"),  wxT("Lithuania Litas"),
    wxT("MOP"),  wxT("Macau Pataca"),
    wxT("MKD"),  wxT("Macedonia Denar"),
    wxT("MGA"),  wxT("Madagascar Ariary"),
    wxT("MWK"),  wxT("Malawi Kwacha"),
    wxT("MYR"),  wxT("Malaysia Ringgit"),
    wxT("MVR"),  wxT("Maldives (Maldive Islands) Rufiyaa"),
    wxT("MRO"),  wxT("Mauritania Ouguiya"),
    wxT("MUR"),  wxT("Mauritius Rupee"),
    wxT("MXN"),  wxT("Mexico Peso"),
    wxT("MDL"),  wxT("Moldova Leu"),
    wxT("MNT"),  wxT("Mongolia Tughrik"),
    wxT("MAD"),  wxT("Morocco Dirham"),
    wxT("MZN"),  wxT("Mozambique Metical"),
    wxT("MMK"),  wxT("Myanmar (Burma) Kyat"),
    wxT("NAD"),  wxT("Namibia Dollar"),
    wxT("NPR"),  wxT("Nepal Rupee"),
    wxT("ANG"),  wxT("Netherlands Antilles Guilder"),
    wxT("NZD"),  wxT("New Zealand Dollar"),
    wxT("NIO"),  wxT("Nicaragua Cordoba"),
    wxT("NGN"),  wxT("Nigeria Naira"),
    wxT("NOK"),  wxT("Norway Krone"),
    wxT("OMR"),  wxT("Oman Rial"),
    wxT("PKR"),  wxT("Pakistan Rupee"),
    wxT("PAB"),  wxT("Panama Balboa"),
    wxT("PGK"),  wxT("Papua New Guinea Kina"),
    wxT("PYG"),  wxT("Paraguay Guarani"),
    wxT("PEN"),  wxT("Peru Nuevo Sol"),
    wxT("PHP"),  wxT("Philippines Peso"),
    wxT("PLN"),  wxT("Poland Zloty"),
    wxT("QAR"),  wxT("Qatar Riyal"),
    wxT("RON"),  wxT("Romania New Leu"),
    wxT("RUB"),  wxT("Russia Ruble"),
    wxT("RWF"),  wxT("Rwanda Franc"),
    wxT("SHP"),  wxT("Saint Helena Pound"),
    wxT("WST"),  wxT("Samoa Tala"),
    wxT("STD"),  wxT("São Tomé and Príncipe Dobra"),
    wxT("SAR"),  wxT("Saudi Arabia Riyal"),
    wxT("RSD"),  wxT("Serbia Dinar"),
    wxT("SCR"),  wxT("Seychelles Rupee"),
    wxT("SLL"),  wxT("Sierra Leone Leone"),
    wxT("SGD"),  wxT("Singapore Dollar"),
    wxT("SBD"),  wxT("Solomon Islands Dollar"),
    wxT("SOS"),  wxT("Somalia Shilling"),
    wxT("ZAR"),  wxT("South Africa Rand"),
    wxT("LKR"),  wxT("Sri Lanka Rupee"),
    wxT("SDG"),  wxT("Sudan Pound"),
    wxT("SRD"),  wxT("Suriname Dollar"),
    wxT("SZL"),  wxT("Swaziland Lilangeni"),
    wxT("SEK"),  wxT("Sweden Krona"),
    wxT("CHF"),  wxT("Switzerland Franc"),
    wxT("SYP"),  wxT("Syria Pound"),
    wxT("TWD"),  wxT("Taiwan New Dollar"),
    wxT("TJS"),  wxT("Tajikistan Somoni"),
    wxT("TZS"),  wxT("Tanzania Shilling"),
    wxT("THB"),  wxT("Thailand Baht"),
    wxT("TOP"),  wxT("Tonga Pa'anga"),
    wxT("TTD"),  wxT("Trinidad and Tobago Dollar"),
    wxT("TND"),  wxT("Tunisia Dinar"),
    wxT("TRY"),  wxT("Turkey Lira"),
    wxT("TMT"),  wxT("Turkmenistan Manat"),
    wxT("TVD"),  wxT("Tuvalu Dollar"),
    wxT("UGX"),  wxT("Uganda Shilling"),
    wxT("UAH"),  wxT("Ukraine Hryvna"),
    wxT("AED"),  wxT("United Arab Emirates Dirham"),
    wxT("GBP"),  wxT("United Kingdom Pound"),
    wxT("USD"),  wxT("United States Dollar"),
    wxT("UYU"),  wxT("Uruguay Peso"),
    wxT("UZS"),  wxT("Uzbekistan Som"),
    wxT("VUV"),  wxT("Vanuatu Vatu"),
    wxT("VEF"),  wxT("Venezuela Bolivar"),
    wxT("VND"),  wxT("Viet Nam Dong"),
    wxT("YER"),  wxT("Yemen Rial"),
    wxT("ZMK"),  wxT("Zambia Kwacha"),
    wxT("ZWD"),  wxT("Zimbabwe Dollar")

};
//----------------------------------------------------------------------------
#endif // _MM_EX_CONSTANTS_H_
//----------------------------------------------------------------------------

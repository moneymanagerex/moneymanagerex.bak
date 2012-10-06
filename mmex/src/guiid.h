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
//----------------------------------------------------------------------------
#ifndef _MM_EX_GUIID_H_
#define _MM_EX_GUIID_H_
//----------------------------------------------------------------------------
#include <wx/defs.h>
//----------------------------------------------------------------------------
/* Include XPM Support */
#include "../resources/about.xpm"
#include "../resources/accounttree.xpm"
#include "../resources/addacctwiz.xpm"
#include "../resources/appstart.xpm"
#include "../resources/art.xpm"
#include "../resources/assets.xpm"
#include "../resources/calendar.xpm"
#include "../resources/car.xpm"
#include "../resources/cat.xpm"
#include "../resources/categoryedit.xpm"
#include "../resources/chartpiereport.xpm"
#include "../resources/checkupdate.xpm"
#include "../resources/clearlist.xpm"
#include "../resources/clock.xpm"
#include "../resources/coin.xpm"
#include "../resources/currency.xpm"
#include "../resources/customsql.xpm"
#include "../resources/delete_account.xpm"
#include "../resources/delete_custom_sql.xpm"
#include "../resources/dog.xpm"
#include "../resources/downarrow_red.xpm"
#include "../resources/downarrow.xpm"
#include "../resources/duplicate.xpm"
#include "../resources/edit_account.xpm"
#include "../resources/edit_custom_sql.xpm"
#include "../resources/empty.xpm"
#include "../resources/encrypt_db.xpm"
#include "../resources/error.xpm"
#include "../resources/exit.xpm"
#include "../resources/export-import.xpm"
#include "../resources/facebook.xpm"
#include "../resources/filter.xpm"
#include "../resources/flag.xpm"
#include "../resources/help.xpm"
#include "../resources/house.xpm"
#include "../resources/hourglass.xpm"
#include "../resources/issues.xpm"
#include "../resources/leds.xpm"
#include "../resources/main-setup.xpm"
#include "../resources/mmexico.xpm"
#include "../resources/money_dollar.xpm"
#include "../resources/money_euro.xpm"
#include "../resources/money.xpm"
#include "../resources/moneyaccount.xpm"
#include "../resources/new_custom_sql.xpm"
#include "../resources/new.xpm"
#include "../resources/newacct.xpm"
#include "../resources/notify.xpm"
#include "../resources/open.xpm"
#include "../resources/preferences-color.xpm"
#include "../resources/preferences-other.xpm"
#include "../resources/print.xpm"
#include "../resources/printsetup.xpm"
#include "../resources/printpreview.xpm"
#include "../resources/reconciled.xpm"
#include "../resources/relocate_categories.xpm"
#include "../resources/relocate_payees.xpm"
#include "../resources/rightarrow.xpm"
#include "../resources/rt_exec_auto.xpm"
#include "../resources/rt_exec_user.xpm"
#include "../resources/save.xpm"
#include "../resources/saveas.xpm"
#include "../resources/savings_acc_closed.xpm"
#include "../resources/savings_acc_favorite.xpm"
#include "../resources/schedule.xpm"
#include "../resources/stock_acc_closed.xpm"
#include "../resources/stock_acc_favorite.xpm"
#include "../resources/stock_acc.xpm"
#include "../resources/stock_curve.xpm"
#include "../resources/term_acc_closed.xpm"
#include "../resources/term_acc_favorite.xpm"
#include "../resources/termaccount.xpm"
#include "../resources/tipicon.xpm"
#include "../resources/trans_from.xpm"
#include "../resources/trans_into.xpm"
#include "../resources/trees.xpm"
#include "../resources/unreconciled.xpm"
#include "../resources/uparrow.xpm"
#include "../resources/update_currency.xpm"
#include "../resources/user_edit.xpm"
#include "../resources/view.xpm"
#include "../resources/void.xpm"
#include "../resources/wrench.xpm"
#include "../resources/web_money.xpm"
#include "../resources/work.xpm"
#include "../resources/yandex_money.xpm"

namespace mmex 
{

extern const int MIN_DATAVERSION;
extern const wxChar * const DATAVERSION;

extern const wxChar *const DEFDATEFORMAT;
extern const wxChar *const DEFDELIMTER;
extern const wxChar *const DEFSTOCKURL;

} // mmex

//----------------------------------------------------------------------------

enum
{
    /* Assets panel  */
    IDC_PANEL_STOCKS_LISTCTRL = wxID_HIGHEST + 1,
    IDC_PANEL_ASSET_STATIC_DETAILS,
    IDC_PANEL_ASSET_STATIC_DETAILS_MINI,

    /* Main Menu  */
    MENU_NEW,
    MENU_OPEN,
    MENU_SAVE,
    MENU_SAVE_AS,
    MENU_RECENT_FILES,
    MENU_RECENT_FILES_0,
    MENU_RECENT_FILES_CLEAR,
    MENU_EXPORT,
    MENU_ABOUT,
    MENU_NEWACCT,
    MENU_ACCTLIST,
    MENU_ORGCATEGS,
    MENU_ORGPAYEE,
    MENU_BUDGETSETUPDIALOG,
    MENU_BILLSDEPOSITS,
    MENU_OPTIONS,
    MENU_HELP,
    MENU_CHECKUPDATE,
    MENU_IMPORT_CSV,
    MENU_IMPORT_QFX,
    MENU_IMPORT,
    MENU_IMPORT_UNIVCSV,
    MENU_REPORTISSUES,
    MENU_ANNOUNCEMENTMAILING,
    MENU_FACEBOOK,
    MENU_EXPORT_CSV,
    MENU_EXPORT_QIF,
    MENU_PRINT_PAGE_SETUP,
    MENU_PRINT_REPORT,
    MENU_PRINT,
    MENU_PRINT_PREVIEW,
    MENU_PRINT_PREVIEW_REPORT,
    MENU_SHOW_APPSTART,
    MENU_EXPORT_HTML,
    MENU_STOCKS,
    MENU_CURRENCY,
    MENU_TREEPOPUP_LAUNCHWEBSITE,
    MENU_IMPORT_MMNETCSV,
    MENU_ASSETS,
    MENU_GOTOACCOUNT,
    MENU_IMPORT_QIF,
    MENU_ACCTEDIT,
    MENU_ACCTDELETE,
    MENU_TRANSACTIONREPORT,
    MENU_VIEW_TOOLBAR,
    MENU_VIEW_LINKS,
    MENU_VIEW_BANKACCOUNTS,
    MENU_VIEW_TERMACCOUNTS,
    MENU_VIEW_STOCKACCOUNTS,
    MENU_VIEW_BUDGET_FINANCIAL_YEARS,
    MENU_VIEW_BUDGET_SETUP_SUMMARY,
    MENU_VIEW_BUDGET_CATEGORY_SUMMARY,
    MENU_VIEW_BUDGET_TRANSFER_TOTAL,
    MENU_CATEGORY_RELOCATION,
    MENU_PAYEE_RELOCATION,
    MENU_CONVERT_ENC_DB,
    MENU_ONLINE_UPD_CURRENCY_RATE,
    MENU_IGNORE_FUTURE_TRANSACTIONS,

    /* Custom SQL Reports */
    MENU_CUSTOM_SQL_REPORT_NEW,
    MENU_CUSTOM_SQL_REPORT_EDIT,
    MENU_CUSTOM_SQL_REPORT_DELETE,
    MENU_TREEPOPUP_CUSTOM_SQL_REPORT_EDIT,
    MENU_TREEPOPUP_CUSTOM_SQL_REPORT_DELETE,
    DIALOG_CUSTOM_SQL_BTN_QUERY,
    DIALOG_CUSTOM_SQL_TXTCTRL_SOURCE,
    DIALOG_CUSTOM_SQL_TXTCTRL_REPORT_TITLE,
    DIALOG_CUSTOM_SQL_CHKBOX_HEADING_ONLY,
    DIALOG_CUSTOM_SQL_CHKBOX_SUB_REPORT,

    /* Checking Panel */
    MENU_TREEPOPUP_EDIT,
    MENU_TREEPOPUP_MOVE,
    MENU_TREEPOPUP_DELETE,
    MENU_TREEPOPUP_IMPORT_QIF,
    MENU_TREEPOPUP_DELETE_VIEWED,
    MENU_TREEPOPUP_DELETE_FLAGGED,
    MENU_TREEPOPUP_MARKRECONCILED,
    MENU_TREEPOPUP_MARKUNRECONCILED,
    MENU_TREEPOPUP_MARKVOID,
    MENU_TREEPOPUP_NEW,
    MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP,
    MENU_TREEPOPUP_MARKRECONCILED_ALL,
    MENU_TREEPOPUP_MARKUNRECONCILED_ALL,
    MENU_TREEPOPUP_MARKVOID_ALL,
    MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL,
    MENU_SUBMENU_MARK_ALL,
    MENU_TREEPOPUP_MARKDUPLICATE,
    MENU_TREEPOPUP_MARKDUPLICATE_ALL,
    MENU_TREEPOPUP_VIEW_SPLIT_CATEGORIES,

    MENU_VIEW_ALLTRANSACTIONS,
    MENU_VIEW_RECONCILED,
    MENU_VIEW_UNRECONCILED,
    MENU_VIEW_NOTRECONCILED,
    MENU_VIEW_VOID,
    MENU_VIEW_FLAGGED,
    MENU_VIEW_DUPLICATE,
    MENU_VIEW_TODAY,
    MENU_VIEW_LAST30,
    MENU_VIEW_LAST90,
    MENU_VIEW_LAST3MONTHS,
    MENU_VIEW_DELETE_TRANS,
    MENU_VIEW_DELETE_FLAGGED,
    MENU_VIEW_CURRENTMONTH,
    MENU_VIEW_LASTMONTH,

    MENU_ON_COPY_TRANSACTION,
    MENU_ON_PASTE_TRANSACTION,
    MENU_ON_NEW_TRANSACTION,
    MENU_ON_DUPLICATE_TRANSACTION,

    ID_NAVTREECTRL,
    ID_PANEL,
    ID_SPLITTERWINDOW,
    ID_LISTCTRL,
    ID_PANEL1,
    ID_PANEL20,

    ID_DIALOG_ABOUT,
    ID_DIALOG_APPSTART,

    ID_PANEL_CHECKING_STATIC_BALHEADER1,
    ID_PANEL_CHECKING_STATIC_BALHEADER2,
    ID_PANEL_CHECKING_STATIC_BALHEADER3,
    ID_PANEL_CHECKING_STATIC_BALHEADER4,
    ID_PANEL_CHECKING_STATIC_BALHEADER5,
    ID_PANEL_CHECKING_LISTCTRL_ACCT,
    ID_PANEL_CHECKING_STATIC_DETAILS,
    ID_PANEL_CHECKING_STATIC_BITMAP_VIEW,
    ID_PANEL_CHECKING_STATIC_BITMAP_FILTER,
    ID_PANEL_CHECKING_STATIC_PANELVIEW,
    ID_PANEL_CHECKING_STATIC_FILTER,

    /* Transaction Dialog */
    ID_DIALOG_TRANS,
    ID_DIALOG_TRANS_TYPE,
    ID_DIALOG_TRANS_TEXTNUMBER,
    ID_DIALOG_TRANS_BUTTONDATE,
    ID_DIALOG_TRANS_BUTTONCATEGS,
    ID_DIALOG_TRANS_TEXTNOTES,
    ID_DIALOG_TRANS_TEXTAMOUNT,
    ID_DIALOG_TRANS_TOTEXTAMOUNT,
    ID_DIALOG_TRANS_STATIC_FROM,
    ID_DIALOG_TRANS_STATIC_PAYEE,
    ID_DIALOG_TRANS_BUTTONPAYEE,
    ID_DIALOG_TRANS_BUTTONTO,
    ID_DIALOG_TRANS_STATUS,
    ID_DIALOG_TRANS_ADVANCED_CHECKBOX,
    ID_DIALOG_TRANS_ADVANCED_FROM,
    ID_DIALOG_TRANS_ADVANCED_TO,
    ID_DIALOG_TRANS_SPLITCHECKBOX,
    ID_DIALOG_TRANS_BUTTONTRANSNUM,
    ID_DIALOG_TRANS_PAYEECOMBO,
    ID_DIALOG_TRANS_BUTTON_FREQENTNOTES,
    ID_DIALOG_TRANS_DATE_SPINNER,

    /* Budget Entry */
    ID_DIALOG_BUDGETENTRY_COMBO_FREQTYPE,
    ID_DIALOG_BUDGETENTRY_TEXTCTRL_AMOUNT,
    ID_DIALOG_BUDGETENTRY_BUTTON_OK,
    ID_DIALOG_BUDGETENTRY_BUTTON_CANCEL,
    ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_EST,
    ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_ACT,
    ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_DIF,
    ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_EST,
    ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_ACT,
    ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_DIF,

    /* New Account */
    ID_DIALOG_NEWACCOUNT,
    ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNAME,
    ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNUMBER,
    ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT,
    ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE,
    ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT,
    ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO,
    ID_DIALOG_NEWACCT_TEXTCTRL_NOTES,
    ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE,
    ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS,
    ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT,
    ID_DIALOG_NEWACCT_BUTTON_CURRENCY,
    ID_DIALOG_NEWACCT_COMBO_ACCTTYPE,

    ID_PANEL_HOMEPAGE_HTMLWINDOW,

    /* Bills & Deposits */
    MENU_POPUP_BD_ENTER_OCCUR,
    MENU_POPUP_BD_SKIP_OCCUR,
    ID_PANEL_BD_STATIC_HEADER,
    ID_PANEL_BD_STATIC_BALHEADER,
    ID_PANEL_BD_LISTCTRL,
    ID_PANEL_BD_STATIC_DETAILS,
    ID_PANEL_BD_STATIC_MINI,
    ID_PANEL_BD_STATIC_BITMAP_VIEW,
    ID_PANEL_BD_STATIC_PANELVIEW,

    /* Bills & Deposits Dialog */
    ID_DIALOG_BD,
    ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME,
    ID_DIALOG_BD_BUTTON_NEXTOCCUR,
    ID_DIALOG_BD_COMBOBOX_REPEATS,
    ID_DIALOG_BD_TEXTCTRL_NUM_TIMES,
    ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_USERACK,
    ID_DIALOG_BD_CHECKBOX_AUTO_EXECUTE_SILENT,
    ID_DIALOG_BD_CALENDAR,
    ID_DIALOG_BD_REPEAT_DATE_SPINNER,
    
    ID_PANEL_REPORTS_HTMLWINDOW,
    ID_PANEL_REPORTS_HEADER_PANEL,
    ID_PANEL_REPORTS_STATIC_HEADER,

    /* Category */
    ID_DIALOG_CATEGORY,
    ID_DIALOG_CATEG_TEXTCTRL_CATNAME,
    ID_DIALOG_CATEG_TREECTRL_CATS,

    /* Category and Payee Relocation */
    ID_DIALOG_CATEG_SELECT_BUTTON_SOURCE,
    ID_DIALOG_CATEG_SELECT_BUTTON_DEST,
    ID_DIALOG_CATEG_RELOCATE_BUTTON_OK,

    ID_DIALOG_PAYEE_SELECT_BUTTON_SOURCE,
    ID_DIALOG_PAYEE_SELECT_BUTTON_DEST,
    ID_DIALOG_PAYEE_RELOCATE_BUTTON_OK,

    /* Budget Entry */
    ID_DIALOG_BUDGETENTRY,
    ID_DIALOG_BUDGETYEAR_LISTBOX_BUDGETYEARS,
    ID_DIALOG_BUDGETYEAR_BUTTON_ADD,
    ID_DIALOG_BUDGETYEAR_BUTTON_ADD_MONTH,
    ID_DIALOG_BUDGETYEAR_BUTTON_DELETE,

    /* Budget Year Dialog */
    ID_DIALOG_BUDGETYEAR,
    ID_DIALOG_BUDGETYEARENTRY_TEXTCTRL_YEAR,
    ID_DIALOG_BUDGETYEARENTRY_TEXTCTRL_MONTH,
    ID_DIALOG_BUDGETYEARENTRY_COMBO_YEARS,
    ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW,
    ID_DIALOG_BUDGETENTRY_COMBO_TYPE,

    MENU_VIEW_ALLBUDGETENTRIES,
    MENU_VIEW_NONZEROBUDGETENTRIES,
    MENU_VIEW_INCOMEBUDGETENTRIES,
    MENU_VIEW_EXPENSEBUDGETENTRIES,
    MENU_VIEW_SUMMARYBUDGETENTRIES,

    /* Options Dialog */
    ID_BOOK_PANELGENERAL,
    ID_BOOK_PANELVIEWS,
    ID_BOOK_PANELCOLORS,
    ID_DIALOG_OPTIONS,
    ID_DIALOG_OPTIONS_LISTBOOK,
    ID_DIALOG_OPTIONS_PANEL1,
    ID_DIALOG_OPTIONS_PANEL2,
    ID_DIALOG_OPTIONS_BUTTON_CURRENCY,
    ID_DIALOG_OPTIONS_DATE_FORMAT,
    ID_DIALOG_OPTIONS_VIEW_ACCOUNTS,
    ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE,
    ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER,
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_COMMA,
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_SEMICOLON,
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_TAB,
    ID_DIALOG_OPTIONS_RADIOBUTTON_DELIMITER_USER,
    ID_DIALOG_OPTIONS_BUTTON_LANGUAGE,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS,
    ID_BOOK_PANELMISC,
    ID_DIALOG_OPTIONS_CHK_BACKUP,
    ID_DIALOG_OPTIONS_CHK_BACKUP_UPDATE,
    ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL,
    ID_DIALOG_OPTIONS_VIEW_TRANS,
    ID_DIALOG_OPTIONS_FONT_SIZE,
    ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES,
    ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT,
    ID_DIALOG_OPTIONS_EXPAND_BANK_HOME,
    ID_DIALOG_OPTIONS_EXPAND_TERM_HOME,
    ID_DIALOG_OPTIONS_EXPAND_STOCK_HOME,
    ID_DIALOG_OPTIONS_EXPAND_BANK_TREE,
    ID_DIALOG_OPTIONS_EXPAND_TERM_TREE,
    ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_DAY,
    ID_DIALOG_OPTIONS_FINANCIAL_YEAR_START_MONTH,

    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_PAYEE,
    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_CATEGORY,
    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_STATUS,
    ID_DIALOG_OPTIONS_DEFAULT_TRANSACTION_DATE,

    /* APP Start Dialog */
    ID_BUTTON_APPSTART_NEW_DATABASE,
    ID_BUTTON_APPSTART_OPEN_DATABASE,
    ID_BUTTON_APPSTART_HELP,
    ID_BUTTON_APPSTART_WEBSITE,
    ID_CHECKBOX_APPSTART_SHOWAPPSTART,
    ID_BUTTON_APPSTART_LAST_DATABASE,

    /* Stocks Dialog */
    ID_DPC_STOCK_PDATE,
    ID_TEXTCTRL_STOCKNAME,
    ID_TEXTCTRL_STOCK_SYMBOL,
    ID_TEXTCTRL_NUMBER_SHARES,
    ID_TEXTCTRL_STOCK_PP,
    ID_TEXTCTRL_STOCK_CP,
    ID_STATIC_STOCK_VALUE,
    ID_TEXTCTRL_STOCK_COMMISSION,
    ID_WEBPROXY_SETTINGSBUTTON,
    ID_TIMER_REFRESH_STOCK,
    ID_TIMER_SCHEDULE_STOCK,
    ID_TIMER_DOWNLOAD_WATCHDOG,
    ID_DOWNLOAD_THREAD_STOCKS,
    ID_HTTP_DOWNLOAD_COMPLETE_STOCKS,

    ID_DIALOG_STOCKS,
    ID_PANEL_STOCKS_LISTCTRL,

    /* File Viewer */
    ID_DIALOG_FILEVIEWER,
    ID_TEXTCTRL_FILEVIEWER,

    MENU_TREEPOPUP_ACCOUNT_NEW,
    MENU_TREEPOPUP_ACCOUNT_DELETE,
    MENU_TREEPOPUP_ACCOUNT_EDIT,
    MENU_TREEPOPUP_ACCOUNT_LIST,
    MENU_TREEPOPUP_ACCOUNT_EXPORT2CSV,
    MENU_TREEPOPUP_ACCOUNT_EXPORT2QIF,
    MENU_TREEPOPUP_ACCOUNT_IMPORTCSV,
    MENU_TREEPOPUP_ACCOUNT_IMPORTQIF,
    MENU_TREEPOPUP_ACCOUNT_IMPORTUNIVCSV,
    MENU_TREEPOPUP_ACCOUNT_IMPORTMMNET,
    MENU_TREEPOPUP_ACCOUNT_VIEWALL,
    MENU_TREEPOPUP_ACCOUNT_VIEWFAVORITE,
    MENU_TREEPOPUP_ACCOUNT_VIEWOPEN,

    AUTO_REPEAT_TRANSACTIONS_TIMER_ID,

};

//----------------------------------------------------------------------------
#endif // _MM_EX_GUIID_H_
//----------------------------------------------------------------------------


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
#include <wx/defs.h> // wxID_HIGHEST
//----------------------------------------------------------------------------

namespace mmex 
{

extern const int MIN_DATAVERSION;
extern const wxChar * const DATAVERSION;
//----------------------------------------------------------------------------
extern const wxChar *const DEFDATEFORMAT;
extern const wxChar *const DEFDELIMTER;
extern const wxChar *const DEFSTOCKURL;

} // mmex

//----------------------------------------------------------------------------

enum
{
    /* Main Menu  */
    MENU_NEW = wxID_HIGHEST + 1,
    MENU_OPEN,
    MENU_SAVE,
    MENU_SAVE_AS,
    MENU_EXPORT,
    MENU_QUIT,
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
    MENU_CONVERT_ENC_DB,
    MENU_ONLINE_UPD_CURRENCY_RATE,
    /* Checking Panel */
    MENU_TREEPOPUP_ENTEROCCUR,
    MENU_TREEPOPUP_GOTO,
    MENU_TREEPOPUP_EDIT,
    MENU_TREEPOPUP_DELETE,
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

    MENU_VIEW_ALLTRANSACTIONS,
    MENU_VIEW_RECONCILED,
    MENU_VIEW_UNRECONCILED,
    MENU_VIEW_NOTRECONCILED,
    MENU_VIEW_VOID,
    MENU_VIEW_FLAGGED,
	MENU_VIEW_DUPLICATE,
    MENU_VIEW_TODAY,
    MENU_VIEW_LAST30,
    MENU_VIEW_LAST3MONTHS,
    MENU_VIEW_DELETE_TRANS,
    MENU_VIEW_DELETE_FLAGGED,
    MENU_VIEW_CURRENTMONTH,
    MENU_VIEW_LASTMONTH,

    MENU_ON_COPY_TRANSACTION,
    MENU_ON_PASTE_TRANSACTION,
    MENU_ON_NEW_TRANSACTION,

    TOOLBAR_MAIN,

    ID_FRAMEPANEL,
    ID_SPLITTERWINDOW1,
    ID_NAVTREECTRL,
    ID_PANEL,
    ID_PANEL3,
    ID_SPLITTERWINDOW,
    ID_LISTCTRL,
    ID_PANEL1,
    ID_STATUSBAR,
    ID_TOOLBAR,
    ID_TOOL,
    ID_TOOL2,
    ID_PANEL20,
    ID_BUTTON_NEW_TRANS,
    ID_BUTTON_EDIT_TRANS,
    ID_BUTTON_DELETE_TRANS,

    ID_DIALOG_ABOUT,
    ID_DIALOG_APPSTART,

    ID_PANEL_CHECKING_STATIC_HEADER,
    ID_PANEL_CHECKING_STATIC_BALHEADER,
    ID_PANEL_CHECKING_LISTCTRL_ACCT,
    ID_PANEL_CHECKING_STATIC_BITMAP_VIEW,
    ID_PANEL_CHECKING_STATIC_PANELVIEW,

    /* Transaction Dialog */
    ID_DIALOG_TRANS,
    ID_DIALOG_TRANS_TYPE,
    ID_DIALOG_TRANS_COMBOPAYEE,
    ID_DIALOG_TRANS_TEXTNUMBER,
    ID_DIALOG_TRANS_COMBOFROM,
    ID_DIALOG_TRANS_BUTTONDATE,
    ID_DIALOG_TRANS_COMBOCATEGORY,
    ID_DIALOG_TRANS_BUTTONCATEGS,
    ID_DIALOG_TRANS_TEXTNOTES,
    ID_DIALOG_TRANS_TEXTAMOUNT,
    ID_DIALOG_TRANS_BUTTON_OK,
    ID_DIALOG_TRANS_BUTTON_CANCEL,
    ID_DIALOG_TRANS_STATIC_FROM,
    ID_DIALOG_TRANS_STATIC_PAYEE,
    ID_DIALOG_TRANS_BUTTONPAYEE,
    ID_DIALOG_TRANS_BUTTONTO,
    ID_DIALOG_TRANS_STATUS,
    ID_DIALOG_TRANS_BUTTONADVANCED,
    ID_DIALOG_TRANS_ADVANCED_FROM,
    ID_DIALOG_TRANS_ADVANCED_TO,
    ID_DIALOG_TRANS_SPLITCHECKBOX,
    ID_DIALOG_TRANS_BUTTONTRANSNUM,
    ID_DIALOG_TRANS_PAYEECOMBO,

    /* Budget Entry */
    ID_DIALOG_BUDGETENTRY_COMBO_FREQTYPE,
    ID_DIALOG_BUDGETENTRY_TEXTCTRL_AMOUNT,
    ID_DIALOG_BUDGETENTRY_BUTTON_OK,
    ID_DIALOG_BUDGETENTRY_BUTTON_CANCEL,
    ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME,
    ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES,

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
    ID_DIALOG_NEWACCT_BUTTON_OK,
    ID_DIALOG_NEWACCT_BUTTON_CANCEL,
    ID_DIALOG_NEWACCT_BUTTON_CURRENCY,
    ID_DIALOG_NEWACCT_COMBO_ACCTTYPE,

    ID_PANEL_HOMEPAGE_HTMLWINDOW,

    /* Bills & Deposits */
    ID_BUTTON_NEW_BD,
    ID_BUTTON_DELETE_BD,
    ID_BUTTON_ENTEROCCUR_BD,
    ID_BUTTON_EDIT_BD,
    ID_PANEL15,
    ID_PANEL_BD_STATIC_HEADER,
    ID_PANEL_BD_STATIC_BALHEADER,
    ID_PANEL_BD_LISTCTRL,
    ID_PANEL_BD_STATIC_DETAILS,
    ID_PANEL_BD_STATIC_BITMAP_VIEW,
    ID_PANEL_BD_STATIC_PANELVIEW,

    /* Bills & Deposits Dialog */
    ID_DIALOG_BD,
    ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME,
    ID_DIALOG_BD_BUTTON_NEXTOCCUR,
    ID_DIALOG_BD_COMBOBOX_REPEATS,
    ID_DIALOG_BD_TEXTCTRL_NUM_TIMES,


    ID_PANEL_REPORTS_HTMLWINDOW,
    ID_PANEL_REPORTS_HEADER_PANEL,
    ID_PANEL_REPORTS_STATIC_HEADER,

    /* Category */
    ID_DIALOG_CATEGORY,
    ID_DIALOG_CATEG_TEXTCTRL_CATNAME,
    ID_DIALOG_CATEG_TREECTRL_CATS,
    ID_DIALOG_CATEG_BUTTON_ADD,
    ID_DIALOG_CATEG_BUTTON_DELETE,
    ID_DIALOG_CATEG_BUTTON_SELECT,
    ID_DIALOG_CATEG_BUTTON_OK,
    ID_DIALOG_CATEG_BUTTON_EDIT,

    /* Budget Entry */
    ID_DIALOG_BUDGETENTRY,
    ID_DIALOG_BUDGETYEAR_LISTBOX_BUDGETYEARS,
    ID_DIALOG_BUDGETYEAR_BUTTON_ADD,
    ID_DIALOG_BUDGETYEAR_BUTTON_DELETE,
    ID_DIALOG_BUDGETYEAR_BUTTON_OK,

    /* Budget Year Dialog */
    ID_DIALOG_BUDGETYEAR,
    ID_DIALOG_BUDGETYEARENTRY_TEXTCTRL_YEAR,
    ID_DIALOG_BUDGETYEARENTRY_COMBO_YEARS,
    ID_DIALOG_BUDGETYEARENTRY_BUTTON_OK,
    ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW,
    ID_DIALOG_BUDGETENTRY_COMBO_TYPE,

    MENU_VIEW_ALLBUDGETENTRIES,
    MENU_VIEW_NONZEROBUDGETENTRIES,
    MENU_VIEW_INCOMEBUDGETENTRIES,
    MENU_VIEW_EXPENSEBUDGETENTRIES,

    /* Currency */
    ID_DIALOG_CURRENCY,
    ID_DIALOG_CURRENCY_BUTTON_ADD,
    ID_DIALOG_CURRENCY_BUTTON_SELECT,
    ID_DIALOG_CURRENCY_BUTTON_UPDATE,
    ID_DIALOG_CURRENCY_CHOICE,
    ID_DIALOG_CURRENCY_STATIC_SAMPLE,
    ID_DIALOG_CURRENCY_TEXT_PFX,
    ID_DIALOG_CURRENCY_TEXT_SFX,
    ID_DIALOG_CURRENCY_TEXT_DECIMAL,
    ID_DIALOG_CURRENCY_TEXT_GROUP,
    ID_DIALOG_CURRENCY_TEXT_UNIT,
    ID_DIALOG_CURRENCY_TEXT_CENTS,
    ID_DIALOG_CURRENCY_TEXT_SCALE,
    ID_DIALOG_CURRENCY_TEXT_BASECONVRATE,
    ID_DIALOG_CURRENCY_BUTTON_CANCEL,
	ID_DIALOG_CURRENCY_TEXT_SYMBOL,

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
    ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL,
    ID_DIALOG_OPTIONS_VIEW_TRANS,
    ID_DIALOG_OPTIONS_CHK_ORIG_DATE,
    ID_DIALOG_OPTIONS_CHK_USE_SOUND,
	ID_DIALOG_OPTIONS_FONT_SIZE,
    ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME,
    ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES,
    ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT,
    ID_DIALOG_OPTIONS_UPD_CURRENCY,

    /* About Dialog */
    ID_DIALOG_BUTTON_ABOUT_VERSION_HISTORY,
    ID_DIALOG_BUTTON_ABOUT_CONTRIBUTERS,

    /* APP Start Dialog */
    ID_BUTTON_APPSTART_NEW_DATABASE,
    ID_BUTTON_APPSTART_OPEN_DATABASE,
    ID_BUTTON_APPSTART_HELP,
    ID_BUTTON_APPSTART_WEBSITE,
    ID_CHECKBOX_APPSTART_SHOWAPPSTART,
    ID_BUTTON_APPSTART_LAST_DATABASE,

    /* Stocks Dialog */
    ID_DPC_STOCK_PDATE,
    ID_BUTTON_STOCKS_HELDAT,
    ID_TEXTCTRL_STOCKNAME,
    ID_TEXTCTRL_STOCK_SYMBOL,
    ID_TEXTCTRL_NUMBER_SHARES,
    ID_TEXTCTRL_STOCK_PP,
    ID_TEXTCTRL_STOCK_NOTES,
    ID_TEXTCTRL_STOCK_CP,
    ID_BUTTON_STOCK_CURRENCY,
    ID_STATIC_STOCK_VALUE,
    ID_BUTTON_STOCK_OK,
    ID_BUTTON_STOCK_CANCEL,
    ID_TEXTCTRL_STOCK_COMMISSION,
	ID_BUTTON_STOCK_WEBPRICE,
    ID_BUTTON_HTTP_SETTINGS,
    ID_PANEL_STOCK_UPDATE_LED,
    ID_WEBPROXY_RADIOBOX,
    ID_WEBPROXY_SETTINGSBUTTON,
    ID_BUTTON_REFRESH_STOCK,
    ID_TIMER_REFRESH_STOCK,
    ID_TIMER_SCHEDULE_STOCK,
    ID_TIMER_DOWNLOAD_WATCHDOG,
    ID_DOWNLOAD_THREAD_STOCKS,
    ID_HTTP_DOWNLOAD_COMPLETE_STOCKS,

    ID_DIALOG_STOCKS,
    ID_BUTTON_NEW_STOCK,
    ID_BUTTON_EDIT_STOCK,
    ID_BUTTON_DELETE_STOCK,
    ID_PANEL_STOCKS_LISTCTRL,

    /* Assets */
    ID_BUTTON_NEW_ASSET,
    ID_BUTTON_EDIT_ASSET,
    ID_BUTTON_DELETE_ASSET,
    ID_PANEL_ASSETS_LISTCTRL,
    ID_BUTTON_ASSETDIALOG_OK,
    ID_BUTTON_ASSETDIALOG_CANCEL,
    ID_TEXTCTRL_ASSETNAME,
    ID_DPC_ASSET_PDATE,
    ID_TEXTCTRL_ASSETDIALOG_CHANGERATE,
    ID_DIALOG_ASSETDIALOG_COMBO_ASSETTYPE,
    ID_TEXTCTRL_ASSET_NOTES,
    ID_TEXTCTRL_ASSETDIALOG_VALUE,

    /* File Viewer */
    ID_DIALOG_FILEVIEWER,
    ID_TEXTCTRL_FILEVIEWER,
	/* Navigation Panel */
	/*Popup Menu for Bank Accounts*/

	//New Account    //
	//Delete Account //
	//Edit Account   //
	//Export >       //  
	//Import >       //  

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

};
//----------------------------------------------------------------------------
#endif // _MM_EX_GUIID_H_
//----------------------------------------------------------------------------

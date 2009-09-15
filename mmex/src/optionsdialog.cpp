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
 /*******************************************************/

#include "optionsdialog.h"
#include "defs.h"
#include "maincurrencydialog.h"
#include "util.h"
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include "mmgraphgenerator.h"

enum ViewEnum
{
  VIEW_ALL,
  VIEW_OPEN,
  VIEW_FAVORITES,
  VIEW_MAX // number of elements, must be last
};

enum ViewTransEnum
{
  VIEW_TRANS_ALL,
  VIEW_TRANS_REC,
  VIEW_TRANS_UNREC,
  VIEW_TRANS_TODAY,
  VIEW_TRANS_30,
  VIEW_TRANS_90,
  VIEW_TRANS_LASTMONTH,
  VIEW_TRANS_CURRENTMONTH,
  VIEW_TRANS_MAX // number of elements, must be last
};

enum HtmlFontEnum
{
  HTML_FONT_XSMALL,
  HTML_FONT_SMALL,
  HTML_FONT_NORMAL,
  HTML_FONT_LARGE,
  HTML_FONT_XLARGE,
  HTML_FONT_XXLARGE,
  HTML_FONT_HUGE,
  HTML_FONT_MAX // number of elements, must be last
};


IMPLEMENT_DYNAMIC_CLASS( mmOptionsDialog, wxDialog )

BEGIN_EVENT_TABLE( mmOptionsDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, mmOptionsDialog::OnCurrency)

    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT, mmOptionsDialog::OnDateFormatChanged)

    EVT_CHOICE(ID_DIALOG_OPTIONS_VIEW_ACCOUNTS, mmOptionsDialog::OnViewAccountsChanged)  
    EVT_CHOICE(ID_DIALOG_OPTIONS_VIEW_TRANS, mmOptionsDialog::OnViewTransChanged)  
  	EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE, mmOptionsDialog::OnLanguageChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE, mmOptionsDialog::OnNavTreeColorChanged)

    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0, mmOptionsDialog::OnAlt0Changed)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1, mmOptionsDialog::OnAlt1Changed)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK, mmOptionsDialog::OnListBackgroundChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER, mmOptionsDialog::OnListBorderChanged)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT, mmOptionsDialog::OnRestoreDefaultColors)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS, mmOptionsDialog::OnListDetailsColors)
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES, mmOptionsDialog::OnListFutureDates)

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_CHK_BACKUP, mmOptionsDialog::OnBackupDBChecked)
    EVT_CHECKBOX(ID_DIALOG_OPTIONS_CHK_ORIG_DATE, mmOptionsDialog::OnOriginalDateChecked)

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_CHK_USE_SOUND, mmOptionsDialog::OnUseSoundChecked)
	EVT_CHOICE(ID_DIALOG_OPTIONS_FONT_SIZE, mmOptionsDialog::OnFontSizeChanged)  

    EVT_CHECKBOX(ID_DIALOG_OPTIONS_UPD_CURRENCY, mmOptionsDialog::OnUpdCurrencyChecked)
    
END_EVENT_TABLE()

#include "../resources/htmbook.xpm"
#include "../resources/pgmctrl.xpm"
#include "../resources/exefile.xpm"
#include "../resources/misc.xpm"

mmOptionsDialog::mmOptionsDialog( )
{
}

mmOptionsDialog::~mmOptionsDialog( )
{
    delete m_imageList;

    wxTextCtrl* st = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER);
    wxString delim = st->GetValue();
    if (!delim.IsEmpty())
        mmDBWrapper::setInfoSettingValue(db_, wxT("DELIMITER"), delim); 

    wxTextCtrl* stun = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME);
    wxString username = stun->GetValue();
    mmDBWrapper::setInfoSettingValue(db_, wxT("USERNAME"), username); 
    mmIniOptions::userNameString_ = username;


    wxTextCtrl* url = (wxTextCtrl*)FindWindow(ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL);
    wxString stockURL = url->GetValue();
    if (!stockURL.IsEmpty())
        mmDBWrapper::setInfoSettingValue(db_, wxT("STOCKURL"), stockURL); 
}

mmOptionsDialog::mmOptionsDialog( mmCoreDB* core, wxSQLite3Database* inidb,
                                 wxWindow* parent, wxWindowID id, 
                                 const wxString& caption, 
                                 const wxPoint& pos, const wxSize& size, long style )
                                 : core_(core), inidb_(inidb), db_(core_->db_.get())
{
    Create(parent, id, caption, pos, size, style);
}

bool mmOptionsDialog::Create( wxWindow* parent, wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
    return TRUE;
}

wxString mmOptionsDialog::DisplayDate2FormatDate(wxString strDate)
{
    wxString DateFormat[TOTAL_DATEFORMAT] = 
    {
        wxT("%d/%m/%y"),
        wxT("%d/%m/%Y"),
        wxT("%m/%d/%y"),
        wxT("%m/%d/%Y"),

        wxT("%m/%d'%Y"),
        
        wxT("%y/%m/%d"),
        wxT("%Y/%m/%d"),

        wxT("%d-%m-%y"),
        wxT("%d-%m-%Y"),
        wxT("%m-%d-%y"),
        wxT("%m-%d-%Y"),
		wxT("%d.%m.%y"),
        wxT("%d.%m.%Y"),
        wxT("%Y-%m-%d"),
        wxT("%Y.%m.%d"),
    };

    wxString itemChoice7Strings[TOTAL_DATEFORMAT] = 
    {
        wxT("DD/MM/YY"),
        wxT("DD/MM/YYYY"),
        wxT("MM/DD/YY"),
        wxT("MM/DD/YYYY"),

        wxT("MM/DD'YYYY"),
        
        wxT("YY/MM/DD"),
        wxT("YYYY/MM/DD"),

        wxT("DD-MM-YY"),
        wxT("DD-MM-YYYY"),
        wxT("MM-DD-YY"),
        wxT("MM-DD-YYYY"),
		wxT("DD.MM.YY"),
        wxT("DD.MM.YYYY"),
	    wxT("YYYY-MM-DD"),
        wxT("YYYY.MM.DD"),
    };

    for(int i=0; i<TOTAL_DATEFORMAT; i++) {
        if(strDate == itemChoice7Strings[i]) {
            return DateFormat[i];
        }
    }

    return strDate;
}

wxString mmOptionsDialog::FormatDate2DisplayDate(wxString strDate)
{
    wxString DateFormat[TOTAL_DATEFORMAT] = 
    {
        wxT("%d/%m/%y"),
        wxT("%d/%m/%Y"),
        wxT("%m/%d/%y"),
        wxT("%m/%d/%Y"),

        wxT("%m/%d'%Y"),
        
        wxT("%y/%m/%d"),
        wxT("%Y/%m/%d"),

        wxT("%d-%m-%y"),
        wxT("%d-%m-%Y"),
        wxT("%m-%d-%y"),
        wxT("%m-%d-%Y"),
		wxT("%d.%m.%y"),
        wxT("%d.%m.%Y"),
		wxT("%Y-%m-%d"),
        wxT("%Y.%m.%d"),
    };

    wxString itemChoice7Strings[TOTAL_DATEFORMAT] = 
    {
        wxT("DD/MM/YY"),
        wxT("DD/MM/YYYY"),
        wxT("MM/DD/YY"),
        wxT("MM/DD/YYYY"),

        wxT("MM/DD'YYYY"),
        
        wxT("YY/MM/DD"),
        wxT("YYYY/MM/DD"),

        wxT("DD-MM-YY"),
        wxT("DD-MM-YYYY"),
        wxT("MM-DD-YY"),
        wxT("MM-DD-YYYY"),
		wxT("DD.MM.YY"),
        wxT("DD.MM.YYYY"),
	    wxT("YYYY-MM-DD"),
        wxT("YYYY.MM.DD"),
    };

    for(int i=0; i<TOTAL_DATEFORMAT; i++) {
        if(strDate == DateFormat[i]) {
            return itemChoice7Strings[i];
        }
    }

    return wxT("");
}

void mmOptionsDialog::OnDateFormatChanged(wxCommandEvent& /*event*/)
{
   wxString format = choiceDateFormat_->GetValue();
   if (format.Trim().IsEmpty())
       return;

   try
   {
        mmGetDateForDisplay(db_, wxDateTime::Now());
   }
   catch(...)
   {
       choiceDateFormat_->SetValue(FormatDate2DisplayDate(wxT("%m/%d/%y")));
       return;
   }

   mmOptions::dateFormat = DisplayDate2FormatDate(format);
   mmOptions::saveOptions(db_);
   wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE);
   st->SetLabel(mmGetDateForDisplay(db_, wxDateTime::Now()) + _(" : Restart"));

   // resize dialog window
   Fit();
}

void mmOptionsDialog::OnViewAccountsChanged(wxCommandEvent& /*event*/)
{
   int selection = choiceVisible_->GetSelection();

   wxString viewAcct = wxT("ALL");
   if (selection == VIEW_OPEN)
       viewAcct = wxT("Open");
   else if (selection == VIEW_FAVORITES)
       viewAcct = wxT("Favorites");

   mmDBWrapper::setINISettingValue(inidb_, wxT("VIEWACCOUNTS"), viewAcct);
}

void mmOptionsDialog::OnViewTransChanged(wxCommandEvent& /*event*/)
{
   int selection = choiceTransVisible_->GetSelection();

   wxString viewTrans = wxT("View All Transactions");
   if (selection == VIEW_TRANS_REC)
       viewTrans = wxT("View Reconciled");
   else if (selection == VIEW_TRANS_UNREC)
       viewTrans = wxT("View UnReconciled");
   else if (selection == VIEW_TRANS_TODAY)
       viewTrans = wxT("View Today");
   else if (selection == VIEW_TRANS_30)
       viewTrans = wxT("View 30 days");
   else if (selection == VIEW_TRANS_90)
       viewTrans = wxT("View 90 days");
   else if (selection == VIEW_TRANS_LASTMONTH)
       viewTrans = wxT("View Last Month");
   else if (selection == VIEW_TRANS_CURRENTMONTH)
       viewTrans = wxT("View Current Month");

   mmDBWrapper::setINISettingValue(inidb_, wxT("VIEWTRANSACTIONS"), viewTrans);
}

void mmOptionsDialog::CreateControls()
{    
    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(htmbook_xpm));
    m_imageList->Add(wxBitmap(pgmctrl_xpm));
    m_imageList->Add(wxBitmap(exefile_xpm));
    m_imageList->Add(wxBitmap(misc_xpm));
 
    mmOptionsDialog* itemDialog1 = this;
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, 10064, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizer4);

    wxListbook* newBook = new wxListbook( itemPanel3, ID_DIALOG_OPTIONS_LISTBOOK, 
        wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );

    // ------------------------------------------
    wxPanel* itemPanelGeneral = new wxPanel( newBook, ID_BOOK_PANELGENERAL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    
    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxVERTICAL);
    itemPanelGeneral->SetSizer(itemBoxSizer20);

    wxStaticBox* itemStaticBoxSizer8Static = new wxStaticBox(itemPanelGeneral, 
        wxID_ANY, _("General Settings"));
    wxStaticBoxSizer* itemStaticBoxSizer8 = new wxStaticBoxSizer(itemStaticBoxSizer8Static, 
        wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizer8, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxStaticText* itemStaticText4 = new wxStaticText( itemPanelGeneral, wxID_STATIC, 
        _("Base Currency"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer8->Add(itemStaticText4, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
    wxString currName = _("Set Currency");
    if (currencyID != -1)
    {
        currName = mmDBWrapper::getCurrencyName(db_, currencyID);
    }

    wxButton* itemButton81 = new wxButton( itemPanelGeneral, 
        ID_DIALOG_OPTIONS_BUTTON_CURRENCY, currName, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer8->Add(itemButton81, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer9Static = new wxStaticBox(itemPanelGeneral, 
        wxID_ANY, _("View Settings"));
    wxStaticBoxSizer* itemStaticBoxSizer9 = new wxStaticBoxSizer(itemStaticBoxSizer9Static, 
        wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizer9, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);

    wxStaticText* itemStaticText41 = new wxStaticText( itemPanelGeneral, 
        wxID_STATIC, _("Date Format"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer9->Add(itemStaticText41, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemChoice7Strings[TOTAL_DATEFORMAT] = 
    {
        wxT("DD/MM/YY"),
        wxT("DD/MM/YYYY"),
        wxT("MM/DD/YY"),
        wxT("MM/DD/YYYY"),

        wxT("MM/DD'YYYY"),
        
        wxT("YY/MM/DD"),
        wxT("YYYY/MM/DD"),

        wxT("DD-MM-YY"),
        wxT("DD-MM-YYYY"),
        wxT("MM-DD-YY"),
        wxT("MM-DD-YYYY"),
		wxT("DD.MM.YY"),
        wxT("DD.MM.YYYY"),
        wxT("YYYY-MM-DD"),
        wxT("YYYY.MM.DD"),
    };

    wxString selection = mmDBWrapper::getInfoSettingValue(db_, wxT("DATEFORMAT"), DEFDATEFORMAT);
    choiceDateFormat_ = new wxComboBox( itemPanelGeneral, 
        ID_DIALOG_OPTIONS_DATE_FORMAT, wxT(""), wxDefaultPosition, 
        wxSize(120, -1), TOTAL_DATEFORMAT, itemChoice7Strings, 0 );
    itemStaticBoxSizer9->Add(choiceDateFormat_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceDateFormat_->SetToolTip(_("Specify the date format for display"));
    choiceDateFormat_->SetValue(FormatDate2DisplayDate(selection));

    
    wxButton* itemButtonDF = new wxButton( itemPanelGeneral, 
        ID_DIALOG_OPTIONS_BUTTON_DATEFORMAT, _("Set"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer9->Add(itemButtonDF, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText411 = new wxStaticText( itemPanelGeneral, 
        ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE, _("Sample Date"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer9->Add(itemStaticText411, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    itemStaticText411->SetLabel(mmGetDateForDisplay(db_, wxDateTime::Now()));

  
    // ------------------------------------------    
    wxStaticBox* itemStaticBoxSizer18Static = new wxStaticBox(itemPanelGeneral, wxID_ANY, 
        _("Import/Export Settings"));
    wxStaticBoxSizer* itemStaticBoxSizer18 = new wxStaticBoxSizer(itemStaticBoxSizer18Static, 
        wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizer18, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxStaticText* itemStaticText5 = new wxStaticText( itemPanelGeneral, wxID_STATIC, 
        _("CSV Delimiter"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer18->Add(itemStaticText5, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), DEFDELIMTER);
    wxTextCtrl* textDelimiter = new wxTextCtrl( itemPanelGeneral, ID_DIALOG_OPTIONS_TEXTCTRL_DELIMITER, 
        delimit, wxDefaultPosition, wxDefaultSize, 0 );
    textDelimiter->SetToolTip(_("Specify the delimiter to use when importing/exporting CSV files"));
    itemStaticBoxSizer18->Add(textDelimiter, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

     // ------------------------------------------    
    wxStaticBox* itemStaticBoxUN = new wxStaticBox(itemPanelGeneral, wxID_ANY, 
        _("Display"));
    wxStaticBoxSizer* itemStaticBoxSizerUN = new wxStaticBoxSizer(itemStaticBoxUN, 
        wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizerUN, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxStaticText* itemStaticTextUN = new wxStaticText( itemPanelGeneral, wxID_STATIC, 
        _("User Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizerUN->Add(itemStaticTextUN, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString userName = mmDBWrapper::getInfoSettingValue(db_, wxT("USERNAME"), wxT(""));
    wxTextCtrl* textUN = new wxTextCtrl( itemPanelGeneral, ID_DIALOG_OPTIONS_TEXTCTRL_USERNAME, 
        userName, wxDefaultPosition, wxDefaultSize, 0 );
    textUN->SetToolTip(_("Specify the User Name"));
    itemStaticBoxSizerUN->Add(textUN, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	  // ------------------------------------------    
    wxStaticBox* itemStaticBoxSizer1881Static = new wxStaticBox(itemPanelGeneral, wxID_ANY, 
        _("Language"));
    wxStaticBoxSizer* itemStaticBoxSizerLang = new wxStaticBoxSizer(itemStaticBoxSizer1881Static, 
        wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizerLang, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    
    wxStaticText* itemStaticText555 = new wxStaticText( itemPanelGeneral, wxID_STATIC, 
        _("Language"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizerLang->Add(itemStaticText555, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString lang = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("LANGUAGE"), wxT(""));
	wxButton* itemButtonLanguage = new wxButton( itemPanelGeneral, 
        ID_DIALOG_OPTIONS_BUTTON_LANGUAGE, lang, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizerLang->Add(itemButtonLanguage, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonLanguage->SetToolTip(_("Specify the language to use"));
    
    
    // ------------------------------------------
    wxPanel* itemPanelViews = new wxPanel( newBook, ID_BOOK_PANELVIEWS, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemPanelViews->SetSizer(itemBoxSizer7);

    wxStaticBox* itemStaticBoxSizer10Static = new wxStaticBox(itemPanelViews, 
        wxID_ANY, _("Account View Options"));
    wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(itemStaticBoxSizer10Static,
        wxHORIZONTAL);
    itemBoxSizer7->Add(itemStaticBoxSizer10, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText45 = new wxStaticText( itemPanelViews, 
        wxID_STATIC, _("Accounts Visible"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer10->Add(itemStaticText45, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemChoiceViewAccountStrings[VIEW_MAX] = 
    {
        _("All"),
        _("Open"),
        _("Favorites")
    };  
    
    choiceVisible_ = new wxChoice( itemPanelViews, 
        ID_DIALOG_OPTIONS_VIEW_ACCOUNTS, wxDefaultPosition, 
        wxSize(100, -1), 3, itemChoiceViewAccountStrings, 0 );
    itemStaticBoxSizer10->Add(choiceVisible_, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxString vAccts = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("VIEWACCOUNTS"), wxT("ALL"));
    choiceVisible_->SetSelection(VIEW_ALL);

    if (vAccts == wxT("Open"))
        choiceVisible_->SetSelection(VIEW_OPEN);
    else if (vAccts == wxT("Favorites"))
        choiceVisible_->SetSelection(VIEW_FAVORITES);
           
    choiceVisible_->SetToolTip(_("Specify which accounts are visible"));

    // -----------------------------------------

    wxStaticBox* itemStaticBoxSizerTransViewStatic = new wxStaticBox(itemPanelViews, 
        wxID_ANY, _("Transaction View Options"));
    wxStaticBoxSizer* itemStaticBoxSizerTransView = new wxStaticBoxSizer(itemStaticBoxSizerTransViewStatic,
        wxHORIZONTAL);
    itemBoxSizer7->Add(itemStaticBoxSizerTransView, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticTextTransText = new wxStaticText( itemPanelViews, 
        wxID_STATIC, _("Transactions Visible"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizerTransView->Add(itemStaticTextTransText, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemChoiceViewTransStrings[VIEW_TRANS_MAX] = 
    {
        _("View All Transactions"),
        _("View Reconciled"),
        _("View UnReconciled"),
        _("View Today"),
        _("View 30 days"),
        _("View 90 days"),
        _("View Last Month"),
        _("View Current Month")
    };  
    
    choiceTransVisible_ = new wxChoice( itemPanelViews, 
        ID_DIALOG_OPTIONS_VIEW_TRANS, wxDefaultPosition, 
        wxSize(165, -1), sizeof(itemChoiceViewTransStrings)/sizeof(*itemChoiceViewTransStrings), itemChoiceViewTransStrings, 0 );
    itemStaticBoxSizerTransView->Add(choiceTransVisible_, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString vTrans = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("VIEWTRANSACTIONS"), wxT("View All Transactions"));
    choiceTransVisible_->SetSelection(VIEW_TRANS_ALL);

    if (vTrans == wxT("View Reconciled"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_REC);
    else if (vTrans == wxT("View UnReconciled"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_UNREC);
    else if (vTrans == wxT("View Today"))
	    choiceTransVisible_->SetSelection(VIEW_TRANS_TODAY);
    else if (vTrans == wxT("View 30 days"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_30);
    else if (vTrans == wxT("View 90 days"))
        choiceTransVisible_->SetSelection(VIEW_TRANS_90);
    else if (vTrans == wxT("View Last Month"))
         choiceTransVisible_->SetSelection(VIEW_TRANS_LASTMONTH);
    else if (vTrans == wxT("View Current Month"))
         choiceTransVisible_->SetSelection(VIEW_TRANS_CURRENTMONTH);
           
    choiceTransVisible_->SetToolTip(_("Specify which transactions are visible by default"));

    wxStaticBox* itemStaticBoxSizerFontSizeStatic = new wxStaticBox(itemPanelViews, 
        wxID_ANY, _("Font Size Options"));
    wxStaticBoxSizer* itemStaticBoxSizerFontSize = new wxStaticBoxSizer(itemStaticBoxSizerFontSizeStatic,
        wxHORIZONTAL);
    itemBoxSizer7->Add(itemStaticBoxSizerFontSize, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticTextHTMLFontSizeText = new wxStaticText( itemPanelViews, 
        wxID_STATIC, _("Report Font Size"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizerFontSize->Add(itemStaticTextHTMLFontSizeText, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	const wxString itemChoiceHTMLFontSize[HTML_FONT_MAX] = 
    {
        wxT("XSmall"),
		wxT("Small"),
		wxT("Normal"),
		wxT("Large"),
		wxT("XLarge"),
		wxT("XXLarge"),
		wxT("Huge")
    };  
    
    choiceFontSize_ = new wxChoice( itemPanelViews, 
        ID_DIALOG_OPTIONS_FONT_SIZE, wxDefaultPosition, 
        wxSize(85, -1), 7, itemChoiceHTMLFontSize, 0 );

    wxString vFontSize = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("HTMLFONTSIZE"), wxT("Font Size on the reports"));

    choiceFontSize_->SetSelection(HTML_FONT_NORMAL);

    if (vFontSize == wxT("1"))
        choiceFontSize_->SetSelection(HTML_FONT_XSMALL);
    else if (vFontSize == wxT("2"))
        choiceFontSize_->SetSelection(HTML_FONT_SMALL);
    else if (vFontSize == wxT("3"))
        choiceFontSize_->SetSelection(HTML_FONT_NORMAL);
    else if (vFontSize == wxT("4"))
        choiceFontSize_->SetSelection(HTML_FONT_LARGE);
    else if (vFontSize == wxT("5"))
         choiceFontSize_->SetSelection(HTML_FONT_XLARGE);
    else if (vFontSize == wxT("6"))
         choiceFontSize_->SetSelection(HTML_FONT_XXLARGE);
    else if (vFontSize == wxT("7"))
         choiceFontSize_->SetSelection(HTML_FONT_HUGE);
    
    choiceFontSize_->SetToolTip(_("Specify which font size is used on the report tables"));
    	
	itemStaticBoxSizerFontSize->Add(choiceFontSize_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // ------------------------------------------
    wxPanel* itemPanelColors = new wxPanel( newBook, ID_BOOK_PANELCOLORS, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemPanelColors->SetSizer(itemBoxSizer8);

    wxButton* itemButtonColorDefault = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_RESTOREDEFAULT, _("Restore Defaults"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemButtonColorDefault, 0, wxALIGN_LEFT|wxALL, 5);
    itemButtonColorDefault->SetToolTip(_("Restore Default Colors"));

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer8->Add(itemGridSizer2, 0, wxALL);

    wxButton* itemButtonColorNavTreeCtrl = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE, _("Nav Tree"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorNavTreeCtrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonColorNavTreeCtrl->SetToolTip(_("Specify the color for the nav tree"));
    itemButtonColorNavTreeCtrl->SetBackgroundColour(mmColors::navTreeBkColor);

    wxButton* itemButtonColorListBackground = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK, _("List Background"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorListBackground, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonColorListBackground->SetToolTip(_("Specify the color for the list background"));
    itemButtonColorListBackground->SetBackgroundColour(mmColors::listBackColor);

    wxButton* itemButtonColorListAlt0 = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0, _("List Row 0"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorListAlt0, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonColorListAlt0->SetToolTip(_("Specify the color for the list row 0"));
    itemButtonColorListAlt0->SetBackgroundColour(mmColors::listAlternativeColor0);

    wxButton* itemButtonColorListAlt1 = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1, _("List Row 1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorListAlt1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonColorListAlt1->SetToolTip(_("Specify the color for the list row 1"));
    itemButtonColorListAlt1->SetBackgroundColour(mmColors::listAlternativeColor1);

     wxButton* itemButtonColorListBorder = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER, _("List Border"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorListBorder, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonColorListBorder->SetToolTip(_("Specify the color for the list Border"));
    itemButtonColorListBorder->SetBackgroundColour(mmColors::listBorderColor);

    wxButton* itemButtonColorListDetails = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS, _("List Details"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorListDetails, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonColorListDetails->SetToolTip(_("Specify the color for the list details"));
    itemButtonColorListDetails->SetBackgroundColour(mmColors::listDetailsPanelColor);

     wxButton* itemButtonColorFutureDates = new wxButton( itemPanelColors, 
        ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES, _("Future Transactions"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemButtonColorFutureDates, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButtonColorFutureDates->SetToolTip(_("Specify the color for future transactions"));
    itemButtonColorFutureDates->SetBackgroundColour(mmColors::listFutureDateColor);
   
    // ------------------------------------------

    wxPanel* itemPanelMisc = new wxPanel( newBook, ID_BOOK_PANELMISC, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizerMisc = new wxBoxSizer(wxVERTICAL);
    itemPanelMisc->SetSizer(itemBoxSizerMisc);

    wxString backupDBState =  mmDBWrapper::getINISettingValue(inidb_, wxT("BACKUPDB"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxBackup = new wxCheckBox( itemPanelMisc, 
        ID_DIALOG_OPTIONS_CHK_BACKUP, _("Backup database before opening"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBoxBackup->SetValue(FALSE);
    if (backupDBState == wxT("TRUE"))
        itemCheckBoxBackup->SetValue(TRUE);
    itemBoxSizerMisc->Add(itemCheckBoxBackup, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxBackup->SetToolTip(_("Select whether to create a .bak file when opening the database file"));


    wxBoxSizer* itemBoxSizerStockURL = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerMisc->Add(itemBoxSizerStockURL, 0, wxGROW|wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticTextURL = new wxStaticText( itemPanelMisc, wxID_STATIC, 
        _("Stock Quote Web Page"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizerStockURL->Add(itemStaticTextURL, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxString stockURL = mmDBWrapper::getInfoSettingValue(db_, wxT("STOCKURL"), DEFSTOCKURL);
    wxTextCtrl* itemTextCtrURL = new wxTextCtrl( itemPanelMisc, 
        ID_DIALOG_OPTIONS_TEXTCTRL_STOCKURL, stockURL, 
        wxDefaultPosition, wxSize(250, -1), 0 );
    itemBoxSizerStockURL->Add(itemTextCtrURL, 1, wxGROW|wxALIGN_TOP|wxALL, 5);

    wxString useOriginalDate =  mmDBWrapper::getINISettingValue(inidb_, wxT("USEORIGDATEONCOPYPASTE"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxOrigDate = new wxCheckBox( itemPanelMisc, 
        ID_DIALOG_OPTIONS_CHK_ORIG_DATE, _("Use Original Date when Pasting Transactions"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBoxOrigDate->SetValue(FALSE);
    if (useOriginalDate == wxT("TRUE"))
        itemCheckBoxOrigDate->SetValue(TRUE);
    itemBoxSizerMisc->Add(itemCheckBoxOrigDate, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxOrigDate->SetToolTip(_("Select whether to use the original transaction date or current date when copying/pasting transactions"));


    wxString useSound =  mmDBWrapper::getINISettingValue(inidb_, wxT("USETRANSSOUND"), wxT("TRUE"));
    wxCheckBox* itemCheckBoxUseSound = new wxCheckBox( itemPanelMisc, 
        ID_DIALOG_OPTIONS_CHK_USE_SOUND, _("Use Transaction Sound"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBoxUseSound->SetValue(FALSE);
    if (useSound == wxT("TRUE"))
        itemCheckBoxUseSound->SetValue(TRUE);
    itemBoxSizerMisc->Add(itemCheckBoxUseSound, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxUseSound->SetToolTip(_("Select whether to use sounds when entering transactions"));

    wxString enableCurrencyUpd = mmDBWrapper::getINISettingValue(inidb_, wxT("UPDATECURRENCYRATE"), wxT("FALSE"));
    wxCheckBox* itemCheckBoxOnlineCurrencyUpd = new wxCheckBox( itemPanelMisc, 
        ID_DIALOG_OPTIONS_UPD_CURRENCY, _("Enable online currency update (Get data from European Central Bank)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBoxOnlineCurrencyUpd->SetValue(FALSE);
    if(enableCurrencyUpd == wxT("TRUE")) {
        itemCheckBoxOnlineCurrencyUpd->SetValue(TRUE);
    }
    itemBoxSizerMisc->Add(itemCheckBoxOnlineCurrencyUpd, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemCheckBoxBackup->SetToolTip(_("Enable or disable get data from European Central Bank to update currency rate"));

    // -------------------------------------------

    newBook->SetImageList(m_imageList);

    newBook->InsertPage(0, itemPanelGeneral, _("General"), true, 2);
    newBook->InsertPage(1, itemPanelViews, _("View Options"), false, 0);
    newBook->InsertPage(2, itemPanelColors, _("Colors"), false, 1);
    newBook->InsertPage(3, itemPanelMisc, _("Others"), false, 3);

    itemBoxSizer4->Add(newBook, 1, wxGROW|wxALL, 5);
    itemBoxSizer4->Layout();

    // resize dialog window
    Fit();
}

void mmOptionsDialog::OnCurrency(wxCommandEvent& /*event*/)
{
    int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
     
    mmMainCurrencyDialog *dlg = new mmMainCurrencyDialog(core_, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        currencyID = dlg->currencyID_;
        if (currencyID != -1)
        {
            wxString currName = mmDBWrapper::getCurrencyName(db_, currencyID);
            wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_CURRENCY);
            bn->SetLabel(currName);
            mmDBWrapper::setBaseCurrencySettings(db_, currencyID);

            wxMessageDialog msgDlg(this, _("Remember to update currency rate"), _("Important note"));
            msgDlg.ShowModal();            
        }
    }

    dlg->Destroy();
}

void mmOptionsDialog::OnLanguageChanged(wxCommandEvent& /*event*/)
{
    mmSelectLanguage(inidb_, true);
	wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_LANGUAGE);
	wxString lang = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("LANGUAGE"), wxT(""));
	bn->SetLabel(lang);

    // resize dialog window            
    Fit();

//    if (!mmGraphGenerator::checkGraphFiles())
//        mmIniOptions::enableGraphs_ = false;
}

void mmOptionsDialog::OnNavTreeColorChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::navTreeBkColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::navTreeBkColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_NAVTREE);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnAlt0Changed(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listAlternativeColor0);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listAlternativeColor0 = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT0);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnAlt1Changed(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listAlternativeColor1);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listAlternativeColor1 = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_ALT1);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnListBackgroundChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listBackColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listBackColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBACK);
        bn->SetBackgroundColour(col);
    }
}

void mmOptionsDialog::OnListBorderChanged(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listBorderColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listBorderColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTBORDER);
        bn->SetBackgroundColour(col);
    }
}

void  mmOptionsDialog::OnListDetailsColors(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listDetailsPanelColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listDetailsPanelColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_LISTDETAILS);
        bn->SetBackgroundColour(col);
    }
}

void  mmOptionsDialog::OnListFutureDates(wxCommandEvent& /*event*/)
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(mmColors::listFutureDateColor);

    wxColourDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        mmColors::listFutureDateColor = col;
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_COLOR_FUTUREDATES);
        bn->SetBackgroundColour(col);
    }
}


void mmOptionsDialog::OnRestoreDefaultColors(wxCommandEvent& /*event*/)
{
    mmRestoreDefaultColors();
}

void mmOptionsDialog::OnBackupDBChecked(wxCommandEvent& /*event*/)
{
  wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_BACKUP);
  bool state = itemCheckBox->GetValue();
  if (state)
     mmDBWrapper::setINISettingValue(inidb_, wxT("BACKUPDB"), wxT("TRUE"));
  else
    mmDBWrapper::setINISettingValue(inidb_, wxT("BACKUPDB"), wxT("FALSE"));
}

void mmOptionsDialog::OnOriginalDateChecked(wxCommandEvent& /*event*/)
{
  wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_ORIG_DATE);
  bool state = itemCheckBox->GetValue();
  if (state)
     mmDBWrapper::setINISettingValue(inidb_, wxT("USEORIGDATEONCOPYPASTE"), wxT("TRUE"));
  else
    mmDBWrapper::setINISettingValue(inidb_, wxT("USEORIGDATEONCOPYPASTE"), wxT("FALSE"));
}

void mmOptionsDialog::OnUseSoundChecked(wxCommandEvent& /*event*/)
{
  wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_CHK_USE_SOUND);
  bool state = itemCheckBox->GetValue();
  if (state)
     mmDBWrapper::setINISettingValue(inidb_, wxT("USETRANSSOUND"), wxT("TRUE"));
  else
    mmDBWrapper::setINISettingValue(inidb_, wxT("USETRANSSOUND"), wxT("FALSE"));
}

void mmOptionsDialog::OnUpdCurrencyChecked(wxCommandEvent& /*event*/)
{
  wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_OPTIONS_UPD_CURRENCY);
  bool state = itemCheckBox->GetValue();
  if (state)
     mmDBWrapper::setINISettingValue(inidb_, wxT("UPDATECURRENCYRATE"), wxT("TRUE"));
  else
     mmDBWrapper::setINISettingValue(inidb_, wxT("UPDATECURRENCYRATE"), wxT("FALSE"));
}

void mmOptionsDialog::OnFontSizeChanged(wxCommandEvent& /*event*/)
{
   int size = choiceFontSize_->GetCurrentSelection() + 1;
   mmIniOptions::fontSize_ = wxString::Format(wxT("%d"), size);
   mmDBWrapper::setINISettingValue(inidb_, wxT("HTMLFONTSIZE"), mmIniOptions::fontSize_);
   // resize dialog window
   Fit();
}


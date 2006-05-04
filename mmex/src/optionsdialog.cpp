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
#include "currencydialog.h"
#include "util.h"

#define VIEW_ALL       0
#define VIEW_OPEN      1
#define VIEW_FAVORITES 2

IMPLEMENT_DYNAMIC_CLASS( mmOptionsDialog, wxDialog )

BEGIN_EVENT_TABLE( mmOptionsDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_OPTIONS_BUTTON_CURRENCY, mmOptionsDialog::OnCurrency)
    EVT_CHOICE(ID_DIALOG_OPTIONS_DATE_FORMAT, mmOptionsDialog::OnDateFormatChanged)  
    EVT_CHOICE(ID_DIALOG_OPTIONS_VIEW_ACCOUNTS, mmOptionsDialog::OnViewAccountsChanged)  
END_EVENT_TABLE()

#include "../resources/htmbook.xpm"
#include "../resources/pgmctrl.xpm"
#include "../resources/exefile.xpm"

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
}

mmOptionsDialog::mmOptionsDialog( wxSQLite3Database* db, wxSQLite3Database* inidb,
                                 wxWindow* parent, wxWindowID id, 
                                 const wxString& caption, 
                                 const wxPoint& pos, const wxSize& size, long style )
                                 : db_(db), inidb_(inidb)
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

void mmOptionsDialog::OnDateFormatChanged(wxCommandEvent& event)
{
   wxString format = choiceDateFormat_->GetStringSelection();
   mmDBWrapper::setInfoSettingValue(db_, wxT("DATEFORMAT"), format);
   wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_OPTIONS_STATIC_SAMPLE_DATE);
   st->SetLabel(mmGetDateForDisplay(db_, wxDateTime::Now()));
}

void mmOptionsDialog::OnViewAccountsChanged(wxCommandEvent& event)
{
   int selection = choiceVisible_->GetSelection();

   wxString viewAcct = wxT("ALL");
   if (selection == VIEW_OPEN)
       viewAcct = wxT("Open");
   else if (selection == VIEW_FAVORITES)
       viewAcct = wxT("Favorites");

   mmDBWrapper::setINISettingValue(inidb_, wxT("VIEWACCOUNTS"), viewAcct);
}

void mmOptionsDialog::CreateControls()
{    
    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(htmbook_xpm));
    m_imageList->Add(wxBitmap(pgmctrl_xpm));
    m_imageList->Add(wxBitmap(exefile_xpm));
 
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

    wxString itemChoice7Strings[] = 
    {
        wxT("%d/%m/%y"),
        wxT("%d/%m/%Y"),
        wxT("%m/%d/%y"),
        wxT("%m/%d/%Y"),
        wxT("%y/%m/%d"),
        wxT("%Y/%m/%d"),
    };  
    
    choiceDateFormat_ = new wxChoice( itemPanelGeneral, 
        ID_DIALOG_OPTIONS_DATE_FORMAT, wxDefaultPosition, 
        wxSize(100, -1), 6, itemChoice7Strings, 0 );
    itemStaticBoxSizer9->Add(choiceDateFormat_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxString selection = mmDBWrapper::getInfoSettingValue(db_, wxT("DATEFORMAT"), DEFDATEFORMAT);
    if (!choiceDateFormat_->SetStringSelection(selection))
       choiceDateFormat_->SetSelection(0);
    choiceDateFormat_->SetToolTip(_("Specify the date format for display"));

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

    wxString itemChoiceViewAccountStrings[] = 
    {
        _("All"),
        _("Open"),
        _("Favorites"),
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

    // ------------------------------------------
    wxPanel* itemPanelColors = new wxPanel( newBook, ID_BOOK_PANELCOLORS, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemPanelColors->SetSizer(itemBoxSizer8);
    // ------------------------------------------

    newBook->SetImageList(m_imageList);

    newBook->InsertPage(0, itemPanelGeneral, _("General"), true, 2);
    newBook->InsertPage(1, itemPanelViews, _("View Options"), false, 0);
    newBook->InsertPage(2, itemPanelColors, _("Colors"), false, 1);

    itemBoxSizer4->Add(newBook, 1, wxGROW|wxALL, 5);
    itemBoxSizer4->Layout();
}

void mmOptionsDialog::OnCurrency(wxCommandEvent& event)
{
    int currencyID = mmDBWrapper::getBaseCurrencySettings(db_);
     
    mmCurrencyDialog *dlg = new mmCurrencyDialog(db_, currencyID, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        currencyID = dlg->currencyID_;
        if (currencyID != -1)
        {
            wxString currName = mmDBWrapper::getCurrencyName(db_, currencyID);
            wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_OPTIONS_BUTTON_CURRENCY);
            bn->SetLabel(currName);
            mmDBWrapper::setBaseCurrencySettings(db_, currencyID);
        }
    }

    dlg->Destroy();
}

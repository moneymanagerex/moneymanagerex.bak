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

#include "defs.h"
#include "appstartdialog.h"
#include "guiid.h"
#include "dbwrapper.h"
#include "paths.h"
#include "constants.h"
#include "util.h"

/*******************************************************/
/* Include XPM Support */
#include "../resources/money.xpm"
/*******************************************************/

IMPLEMENT_DYNAMIC_CLASS( mmAppStartDialog, wxDialog )

BEGIN_EVENT_TABLE( mmAppStartDialog, wxDialog )
    EVT_BUTTON( ID_BUTTON_APPSTART_NEW_DATABASE, mmAppStartDialog::OnButtonAppstartNewDatabaseClick )
    EVT_BUTTON( ID_BUTTON_APPSTART_OPEN_DATABASE, mmAppStartDialog::OnButtonAppstartOpenDatabaseClick )
    EVT_BUTTON( ID_BUTTON_APPSTART_HELP, mmAppStartDialog::OnButtonAppstartHelpClick )
    EVT_BUTTON( ID_BUTTON_APPSTART_WEBSITE, mmAppStartDialog::OnButtonAppstartWebsiteClick )
    EVT_BUTTON( ID_BUTTON_APPSTART_LAST_DATABASE, mmAppStartDialog::OnButtonAppstartLastDatabaseClick )
END_EVENT_TABLE()


mmAppStartDialog::mmAppStartDialog() : 
        inidb_(),
        itemCheckBox(),
        retCode_(-1)
{
}


mmAppStartDialog::mmAppStartDialog(wxSQLite3Database* inidb, wxWindow* parent) :
        inidb_(inidb),
        itemCheckBox(),
        retCode_(-1)
{
    wxString caption = mmex::getProgramName() + _(" Start Page");
    Create(parent, ID_DIALOG_APPSTART, caption, wxDefaultPosition, wxSize(400, 300), wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX);
}

bool mmAppStartDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
                              const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    bool ok = wxDialog::Create( parent, id, caption, pos, size, style );

    if (ok) {
        SetIcon(mmex::getProgramIcon());
        CreateControls();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        CentreOnScreen();
    }

    return ok;
}

mmAppStartDialog::~mmAppStartDialog()
{
    try {
        wxString showBeginApp = itemCheckBox->GetValue() ? wxT("TRUE") : wxT("FALSE");
        mmDBWrapper::setINISettingValue(inidb_, wxT("SHOWBEGINAPP"), showBeginApp);
    } catch (...) {
        wxASSERT(false);
    }
}


void mmAppStartDialog::CreateControls()
{    
    mmAppStartDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBitmap itemStaticBitmap4Bitmap;
    itemStaticBitmap4Bitmap.LoadFile(mmex::getPathResource(mmex::SPLASH_ICON), wxBITMAP_TYPE_PNG); 
    
    wxStaticBitmap* itemStaticBitmap4 = 0;
    if (!mmIniOptions::instance().enableCustomLogo_)
    {
        itemStaticBitmap4 = new wxStaticBitmap( itemDialog1, wxID_STATIC, 
            itemStaticBitmap4Bitmap, wxDefaultPosition, wxSize(400, 209), 0 );
    }
    
    if (itemStaticBitmap4) {
        itemBoxSizer3->Add(itemStaticBitmap4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton61 = new wxButton( itemDialog1, ID_BUTTON_APPSTART_LAST_DATABASE, 
        _("Open Last Opened Database"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton61, 0, wxGROW|wxALL, 5);

    if (inidb_)
    {
        wxString val = mmDBWrapper::getLastDbPath(inidb_);
        if (val.IsEmpty())
        {
            itemButton61->Disable();
        }
        else
        {
          itemButton61->SetToolTip(_("Open the previously opened database : ") + val);
        }
    }
    else
        itemButton61->Disable();

    wxButton* itemButton6 = new wxButton( itemDialog1, ID_BUTTON_APPSTART_NEW_DATABASE, 
        _("Create a New Database"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton6->SetToolTip(_("Create a new database file to get started"));
    itemBoxSizer5->Add(itemButton6, 0, wxGROW|wxALL, 5);

    wxButton* itemButton7 = new wxButton( itemDialog1, ID_BUTTON_APPSTART_OPEN_DATABASE, 
        _("Open Existing Database"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton7->SetToolTip(_("Open an already created database file with extension (*.mmb)"));
    itemBoxSizer5->Add(itemButton7, 0, wxGROW|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemDialog1, ID_BUTTON_APPSTART_HELP, 
        _("Read Documentation"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton8->SetToolTip(_("Read the user manual"));
    itemBoxSizer5->Add(itemButton8, 0, wxGROW|wxALL, 5);

    if (mmIniOptions::instance().enableVisitWebsite_)
    {
       wxButton* itemButton9 = new wxButton( itemDialog1, ID_BUTTON_APPSTART_WEBSITE, 
          _("Visit Website for more information"), wxDefaultPosition, wxDefaultSize, 0 );

       wxString s = _("Open the ");
       s += mmex::getProgramName();
       s += _(" website for latest news, updates etc");
       
       itemButton9->SetToolTip(s);
       itemBoxSizer5->Add(itemButton9, 0, wxGROW|wxALL, 5);
    }

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer10, 0, wxALIGN_LEFT|wxALL, 5);

    wxString showAppStartString = _("Show this window next time ");
    showAppStartString += mmex::getProgramName();
    showAppStartString += _(" starts");

    itemCheckBox = new wxCheckBox( itemDialog1, ID_CHECKBOX_APPSTART_SHOWAPPSTART, 
        showAppStartString, wxDefaultPosition, 
        wxDefaultSize, wxCHK_2STATE );
    wxString showBeginApp = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("SHOWBEGINAPP"), wxGetEmptyString());
    if (showBeginApp == wxT("TRUE") )
        itemCheckBox->SetValue(true);
    else
        itemCheckBox->SetValue(false);
    itemBoxSizer10->Add(itemCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void mmAppStartDialog::OnButtonAppstartNewDatabaseClick( wxCommandEvent& /*event*/ )
{
    retCode_ = 0;
    Close(TRUE);   
}

void mmAppStartDialog::OnButtonAppstartOpenDatabaseClick( wxCommandEvent& /*event*/ )
{
    retCode_ = 1;
    Close(TRUE);   
}

void mmAppStartDialog::OnButtonAppstartHelpClick( wxCommandEvent& /*event*/ )
{
   retCode_ = 2;
   Close(TRUE);   
}

void mmAppStartDialog::OnButtonAppstartWebsiteClick( wxCommandEvent& /*event*/ )
{
   retCode_ = 3;
   Close(TRUE);   
}
    
void mmAppStartDialog::OnButtonAppstartLastDatabaseClick( wxCommandEvent& /*event*/ )
{
    retCode_ = 4;
    Close(TRUE);   
}


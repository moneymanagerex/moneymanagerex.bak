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
 *******************************************************/
#include "aboutdialog.h"
#include "fileviewerdialog.h"
#include "defs.h"
#include "dbwrapper.h"
#include "util.h"
#include "paths.h"
#include "constants.h"
/*******************************************************/
/* Include XPM Support */
#include "../resources/money.xpm"
/*******************************************************/

#include <boost/version.hpp>
#include <wx/version.h>
#include <wx/wxsqlite3.h>

namespace
{

enum { 
  IDC_DIALOG_BUTTON_ABOUT_VERSION_HISTORY = wxID_HIGHEST + 1,
  IDC_DIALOG_BUTTON_ABOUT_CONTRIBUTERS
};


wxString get_libs_info()
{
    wxString s;

    s << wxVERSION_STRING
      << wxT(",  Boost C++ ") << (BOOST_VERSION/100000) << wxT('.') 
                  << (BOOST_VERSION / 100 % 1000) << wxT('.') 
                  << (BOOST_VERSION % 100)
      << wxT(",  SQLite3 ") << wxSQLite3Database::GetVersion()
      << wxT(",  wxSQLite by Ulrich Telle");

    return s;
}

} // namespace


IMPLEMENT_DYNAMIC_CLASS(mmAboutDialog, wxDialog)


BEGIN_EVENT_TABLE(mmAboutDialog, wxDialog)
  EVT_BUTTON(IDC_DIALOG_BUTTON_ABOUT_VERSION_HISTORY, mmAboutDialog::OnVersionHistory)
  EVT_BUTTON(IDC_DIALOG_BUTTON_ABOUT_CONTRIBUTERS, mmAboutDialog::OnContributerList)
END_EVENT_TABLE()


mmAboutDialog::mmAboutDialog( wxSQLite3Database* inidb, wxWindow* parent) :
        inidb_(inidb)
{
    wxString caption = _("About ") + mmex::getProgramName();
    Create(parent, ID_DIALOG_ABOUT, caption, wxDefaultPosition, wxSize(500, 220), wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX);
}

bool mmAboutDialog::Create(wxWindow* parent, 
                           wxWindowID id, 
                           const wxString& caption, 
                           const wxPoint& pos, 
                           const wxSize& size, 
                           long style
                          )
{
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);

    bool ok = wxDialog::Create(parent, id, caption, pos, size, style);

    if (ok) {
        CreateControls();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        SetIcon(mmex::getProgramIcon());
        Centre();
    }

    return ok;
}

void mmAboutDialog::OnVersionHistory(wxCommandEvent& /*event*/)
{
    wxString filePath = mmex::getPathDoc(mmex::F_VERSION);
    fileviewer(filePath, this).ShowModal();
}

void mmAboutDialog::OnContributerList(wxCommandEvent& /*event*/)
{
    wxString filePath = mmex::getPathDoc(mmex::F_CONTRIB);
    fileviewer(filePath, this).ShowModal();
}

void mmAboutDialog::CreateControls()
{    
    mmAboutDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizerN = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizerN, 1, wxGROW|wxALL, 5);

    wxBitmap itemStaticBitmap3Bitmap;
    itemStaticBitmap3Bitmap.LoadFile(mmex::getPathResource(mmex::SPLASH_ICON), wxBITMAP_TYPE_PNG); 

    wxStaticBitmap* itemStaticBitmap3 = 0;
    if (!mmIniOptions::enableCustomLogo_)
    {
        itemStaticBitmap3 = new wxStaticBitmap( itemDialog1, wxID_STATIC, 
            itemStaticBitmap3Bitmap, wxDefaultPosition);
    }
    
    if (itemStaticBitmap3) {
        itemBoxSizerN->Add(itemStaticBitmap3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer4, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, 
        wxString(_("Version: ")) + wxPlatformInfo::Get().GetOperatingSystemIdName() + wxT (": ") + mmex::getProgramVersion(), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    if (!mmIniOptions::enableCustomAboutDialog_)
    {
       wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
          wxID_STATIC, wxT("(c) 2005-2011 Madhan Kanagavel"), wxDefaultPosition, wxDefaultSize, 0 );
       itemStaticText8->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxGetEmptyString()));
       itemBoxSizer4->Add(itemStaticText8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxStaticText* itemStaticText91 = new wxStaticText( itemDialog1, 
          wxID_STATIC, _("  and contributors from around the world"), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText91, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxStaticText* itemStaticText911 = new wxStaticText( itemDialog1, 
          wxID_STATIC, _("Released under the GNU GPL License"), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemStaticText911->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxGetEmptyString()));
       itemBoxSizer4->Add(itemStaticText911, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, 
          wxID_STATIC, wxT("http://www.codelathe.com"), wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       itemBoxSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

       wxStaticText* itemStaticText12 = new wxStaticText( itemDialog1, 
          wxID_STATIC, get_libs_info(), wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

       wxButton* itemButton14 = new wxButton( itemDialog1, IDC_DIALOG_BUTTON_ABOUT_VERSION_HISTORY, 
          _("Version History"), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemButton14, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

       wxButton* itemButton18 = new wxButton( itemDialog1, IDC_DIALOG_BUTTON_ABOUT_CONTRIBUTERS, 
          _("Contributors"), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemButton18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

       wxString langStr = mmDBWrapper::getINISettingValue(inidb_, wxT("LANGUAGE")); 

       wxStaticText* itemStaticText15 = new wxStaticText( itemDialog1, 
          wxID_STATIC, _("Using Language : ") + langStr, wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxButton* itemButton19 = new wxButton( itemDialog1, wxID_OK, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
       itemButton19->SetDefault();
       itemButton19->SetFocus();
       itemBoxSizer4->Add(itemButton19, 0, wxALIGN_RIGHT|wxALL, 5);

    }
    else
    {
       wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
          wxID_STATIC, mmIniOptions::aboutCompanyName_ , wxDefaultPosition, wxDefaultSize, 0 );
       itemStaticText8->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxGetEmptyString()));
       itemBoxSizer4->Add(itemStaticText8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    }
}



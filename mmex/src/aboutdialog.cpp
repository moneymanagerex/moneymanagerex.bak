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
#include "aboutdialog.h"
#include "fileviewerdialog.h"
#include "defs.h"
#include "dbwrapper.h"
#include "util.h"
#include "defs.h"

/*******************************************************/
/* Include XPM Support */
#include "../resources/money.xpm"
#ifdef MMEX_CUSTOM_BUILD
#include "../resources/bma.xpm"
#endif
/*******************************************************/
/* Indicate file path for OS X */
#if defined (__WXMAC__)
#define MMEX_INIDB_FNAME wxStandardPaths::Get().GetResourcesDir() + wxT("/mmexini.db3")
#define MMEX_SPLASH_FNAME wxStandardPaths::Get().GetResourcesDir() + wxT("/splash.png")
#define MMEX_ICON_FNAME wxStandardPaths::Get().GetResourcesDir() + wxT("/mmex.ico")

#else
#define MMEX_INIDB_FNAME wxT("/mmexini.db3")
#define MMEX_SPLASH_FNAME wxT("splash.png")
#define MMEX_ICON_FNAME wxT("mmex.ico")
#endif
/*******************************************************/
IMPLEMENT_DYNAMIC_CLASS( mmAboutDialog, wxDialog )

BEGIN_EVENT_TABLE( mmAboutDialog, wxDialog )
  EVT_BUTTON(ID_DIALOG_BUTTON_ABOUT_VERSION_HISTORY, mmAboutDialog::OnVersionHistory)
  EVT_BUTTON(ID_DIALOG_BUTTON_ABOUT_CONTRIBUTERS, mmAboutDialog::OnContributerList)
END_EVENT_TABLE()

mmAboutDialog::mmAboutDialog()
{
}

mmAboutDialog::mmAboutDialog( wxSQLite3Database* inidb, wxWindow* parent, wxWindowID id, 
    const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    inidb_ = inidb;
    Create(parent, id, caption, pos, size, style);
}

bool mmAboutDialog::Create( wxWindow* parent, wxWindowID id, 
    const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    wxIcon icon(MMEX_ICON_FNAME, wxBITMAP_TYPE_ICO, 32, 32);
    SetIcon(icon);

    Centre();
    return TRUE;
}

void mmAboutDialog::OnVersionHistory(wxCommandEvent& event)
{
    wxFileName fname(wxTheApp->argv[0]);
    wxString filePath = fname.GetPath(wxPATH_GET_VOLUME) 
                       + wxFileName::GetPathSeparator()
                       + wxT("version.txt");
#if defined (__WXMAC__) || defined (__WXOSX__)
	filePath = wxT("MMEX.app/Contents/Resources/version.txt");
#endif
    fileviewer* dlg = new fileviewer(filePath, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
    }
    dlg->Destroy();
}

void mmAboutDialog::OnContributerList(wxCommandEvent& event)
{
    wxFileName fname(wxTheApp->argv[0]);
    wxString filePath = fname.GetPath(wxPATH_GET_VOLUME) 
                        + wxFileName::GetPathSeparator()
                        + wxT("contrib.txt");
#if defined (__WXMAC__) || defined (__WXOSX__)
	filePath = wxT("MMEX.app/Contents/Resources/contrib.txt");
#endif
    fileviewer* dlg = new fileviewer(filePath, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
    }
    dlg->Destroy();
}

void mmAboutDialog::CreateControls()
{    
    mmAboutDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizerN = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizerN, 1, wxGROW|wxALL, 5);

    wxBitmap itemStaticBitmap3Bitmap;
    itemStaticBitmap3Bitmap.LoadFile(MMEX_SPLASH_FNAME, wxBITMAP_TYPE_PNG); 

    wxStaticBitmap* itemStaticBitmap3;
    if (!mmIniOptions::enableCustomLogo_)
    {
        itemStaticBitmap3 = new wxStaticBitmap( itemDialog1, wxID_STATIC, 
            itemStaticBitmap3Bitmap, wxDefaultPosition);
    }
    else
    {
#ifdef MMEX_CUSTOM_BUILD
        wxBitmap itemStaticCustomBitmap(bma_xpm);
         itemStaticBitmap3 = new wxStaticBitmap( itemDialog1, wxID_STATIC, 
            itemStaticCustomBitmap, wxDefaultPosition, wxSize(235, 157), 0 );
#endif
    }
    itemBoxSizerN->Add(itemStaticBitmap3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer4, 1, wxGROW|wxALL, 5);

#if 0
    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, 
        mmIniOptions::appName_, 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(16, 
       wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
#endif

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, 
        wxString(_("Version: ")) + MMEXVERSION , wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    if (!mmIniOptions::enableCustomAboutDialog_)
    {
       wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
          wxID_STATIC, wxT("(c) 2005-2009 Madhan Kanagavel"), wxDefaultPosition, wxDefaultSize, 0 );
       itemStaticText8->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
       itemBoxSizer4->Add(itemStaticText8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxStaticText* itemStaticText91 = new wxStaticText( itemDialog1, 
          wxID_STATIC, _("  and contributers from around the world."), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText91, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxStaticText* itemStaticText911 = new wxStaticText( itemDialog1, 
          wxID_STATIC, _("Released under the GNU GPL License"), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemStaticText911->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
       itemBoxSizer4->Add(itemStaticText911, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, 
          wxID_STATIC, wxT("http://www.codelathe.com"), wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       itemBoxSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

       wxStaticText* itemStaticText12 = new wxStaticText( itemDialog1, 
          wxID_STATIC, _("Powered by wxWidgets, SQLite, wxSQLite(by Ulrich Telle)"), wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

       wxButton* itemButton14 = new wxButton( itemDialog1, ID_DIALOG_BUTTON_ABOUT_VERSION_HISTORY, 
          _("Version History"), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemButton14, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

       wxButton* itemButton18 = new wxButton( itemDialog1, ID_DIALOG_BUTTON_ABOUT_CONTRIBUTERS, 
          _("Contributers"), 
          wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemButton18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

       wxString langStr = mmDBWrapper::getINISettingValue(inidb_, 
          wxT("LANGUAGE"), wxT("")); 

       wxStaticText* itemStaticText15 = new wxStaticText( itemDialog1, 
          wxID_STATIC, _("Using Language : ") + langStr, wxDefaultPosition, wxDefaultSize, 0 );
       itemBoxSizer4->Add(itemStaticText15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    }
    else
    {
       wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
          wxID_STATIC, mmIniOptions::aboutCompanyName_ , wxDefaultPosition, wxDefaultSize, 0 );
       itemStaticText8->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
       itemBoxSizer4->Add(itemStaticText8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    }
}



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
#include <wx/app.h>
/*******************************************************/
/* Include XPM Support */
#include "../resources/money.xpm"
/*******************************************************/
IMPLEMENT_DYNAMIC_CLASS( mmAboutDialog, wxDialog )

BEGIN_EVENT_TABLE( mmAboutDialog, wxDialog )
  EVT_BUTTON(ID_DIALOG_BUTTON_ABOUT_VERSION_HISTORY, mmAboutDialog::OnVersionHistory)
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

    wxIcon icon(mainicon_xpm);
    SetIcon(icon);
    Centre();
    return TRUE;
}

void mmAboutDialog::OnVersionHistory(wxCommandEvent& event)
{
    wxFileName fname(wxTheApp->argv[0]);
    wxString filePath = fname.GetPath(wxPATH_GET_VOLUME) + wxT("\\version.txt");
    
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

    wxBitmap itemStaticBitmap3Bitmap(money_xpm);
    wxStaticBitmap* itemStaticBitmap3 = new wxStaticBitmap( itemDialog1, wxID_STATIC, 
        itemStaticBitmap3Bitmap, wxDefaultPosition, wxSize(235, 157), 0 );
    itemBoxSizerN->Add(itemStaticBitmap3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString trans = _("Translations by : ");
    wxStaticText* itemStaticText18 = new wxStaticText( itemDialog1, 
        wxID_STATIC,  trans 
        + wxT("Italian (Claudio), French (Berry As), \n \
Greek (Panagiotis Pentzeridis), Czech (Josef Remes)"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizerN->Add(itemStaticText18, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer4, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, 
        wxT("Money Manager Ex"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText5->SetFont(wxFont(16, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, 
        MMEXVERSION , wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    itemBoxSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
        wxID_STATIC, wxT("(c) 2005-2006 Madhan Kanagavel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText8->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    itemBoxSizer4->Add(itemStaticText8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
   
    wxStaticText* itemStaticText91 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("  and contributers from around the world."), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText91, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

     wxStaticText* itemStaticText911 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Released under the GNU GPL License"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText911->SetFont(wxFont(9, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    itemBoxSizer4->Add(itemStaticText911, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, 
        wxID_STATIC, wxT("http://www.thezeal.com/software"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    itemBoxSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Powered by wxWidgets (c) www.wxwidgets.org"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Powered by SQLite (c) www.sqlite.org"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText13, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxButton* itemButton14 = new wxButton( itemDialog1, ID_DIALOG_BUTTON_ABOUT_VERSION_HISTORY, 
        _("Version History"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemButton14, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxString langStr = mmDBWrapper::getINISettingValue(inidb_, 
        wxT("LANGUAGE"), wxT("")); 

    wxStaticText* itemStaticText15 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Using Language : ") + langStr, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
}



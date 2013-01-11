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

#include <boost/version.hpp>
#include <wx/version.h>
#include <wx/wxsqlite3.h>
#include <wx/statline.h>


enum
{
    IDC_DIALOG_BUTTON_ABOUT_VERSION_HISTORY = wxID_HIGHEST + 600,
    IDC_DIALOG_BUTTON_ABOUT_CONTRIBUTERS
};

IMPLEMENT_DYNAMIC_CLASS(mmAboutDialog, wxDialog)

BEGIN_EVENT_TABLE(mmAboutDialog, wxDialog)
END_EVENT_TABLE()

mmAboutDialog::mmAboutDialog( wxWindow* parent) 
{
    wxString caption = wxString(_("About")) << wxT(" ") << mmex::getProgramName();
    Create(parent, ID_DIALOG_ABOUT, caption, wxDefaultPosition,
        wxSize(500, 220), wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX);
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

    if (ok)
    {
        CreateControls();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        SetIcon(mmex::getProgramIcon());
        Centre();
    }

    return ok;
}

void mmAboutDialog::CreateControls()
{
    wxSizerFlags flags;
    flags.Align(wxALIGN_CENTER).Border(wxALL, 5);

    wxBoxSizer* itemBoxSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer);

    wxBitmap itemStaticBitmap3Bitmap(money_xpm);

    wxStaticBitmap* itemStaticBitmap3;
    itemStaticBitmap3 = new wxStaticBitmap(this,
        wxID_STATIC, itemStaticBitmap3Bitmap);
    itemBoxSizer->Add(itemStaticBitmap3, flags);

    wxStaticText* versionStaticText = new wxStaticText( this, wxID_STATIC,
        wxString(_("Version: ")) + mmex::getProgramVersion());
    int font_size = this->GetFont().GetPointSize() + 2;
    versionStaticText->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxGetEmptyString()));
    itemBoxSizer->Add(versionStaticText, flags);

    wxStaticText* itemStaticText88 = new wxStaticText(this,
        wxID_STATIC, mmex::getProgramCopyright());

    //Read data from file
    wxString filePath = mmex::getPathDoc(mmex::F_CONTRIB);
    wxFileInputStream input(filePath);
    wxTextInputStream text(input);
    wxArrayString data;
    data.Add(wxT(""));

    int part = 0;
    while (!input.Eof())
    {
        wxString line = text.ReadLine() << wxT("\n");
        if (!line.Contains(wxT("-------------\n")))
            data[part] << line;
        else {
            ++part;
            data.Add(wxT(""));
        }
    }

    //Create tabs
    wxNotebook* about_notebook = new wxNotebook(this,
        wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );

    wxPanel* about_tab = new wxPanel(about_notebook, wxID_ANY);
    about_notebook->AddPage(about_tab, _("About"));
    wxBoxSizer *about_sizer = new wxBoxSizer(wxVERTICAL);
    about_tab->SetSizer(about_sizer);

    wxPanel* developers_tab = new wxPanel(about_notebook, wxID_ANY);
    about_notebook->AddPage(developers_tab, _("Developers"));
    wxBoxSizer *developers_sizer = new wxBoxSizer(wxVERTICAL);
    developers_tab->SetSizer(developers_sizer);

    wxPanel* translators_tab = new wxPanel(about_notebook, wxID_ANY);
    about_notebook->AddPage(translators_tab, _("Translators"));
    wxBoxSizer *translators_sizer = new wxBoxSizer(wxVERTICAL);
    translators_tab->SetSizer(translators_sizer);

    wxPanel* artwork_tab = new wxPanel(about_notebook, wxID_ANY);
    about_notebook->AddPage(artwork_tab, _("Artwork"));
    wxBoxSizer *artwork_sizer = new wxBoxSizer(wxVERTICAL);
    artwork_tab->SetSizer(artwork_sizer);

    wxPanel* sponsors_tab = new wxPanel(about_notebook, wxID_ANY);
    about_notebook->AddPage(sponsors_tab, _("Sponsors"));
    wxBoxSizer *sponsors_sizer = new wxBoxSizer(wxVERTICAL);
    sponsors_tab->SetSizer(sponsors_sizer);

    wxSize internal_size = wxSize(400, 200); //developers_tab_->GetBestVirtualSize();

    wxTextCtrl* about_text = new wxTextCtrl( about_tab,
        wxID_STATIC, mmex::getProgramDescription(),
        wxDefaultPosition, internal_size, wxTE_MULTILINE );
    about_text->SetEditable(false);
    about_sizer->Add(about_text, flags);

    wxTextCtrl* developers_text = new wxTextCtrl( developers_tab,
        wxID_STATIC, data[0], wxDefaultPosition, internal_size, wxTE_MULTILINE );
    developers_text->SetEditable(false);
    developers_sizer->Add(developers_text, 1, wxEXPAND);

    if (data.GetCount() > 1) {
        wxTextCtrl* translators_text = new wxTextCtrl( translators_tab,
            wxID_STATIC, data[1], wxDefaultPosition, internal_size, wxTE_MULTILINE );
        translators_text->SetEditable(false);
        translators_sizer->Add(translators_text, 1, wxEXPAND);
    }

    if (data.GetCount() > 2) {
        wxTextCtrl* artwork_text = new wxTextCtrl( artwork_tab,
            wxID_STATIC, data[2], wxDefaultPosition, internal_size, wxTE_MULTILINE );
        artwork_text->SetEditable(false);
        artwork_sizer->Add(artwork_text, 1, wxEXPAND);
    }

    if (data.GetCount() > 3) {
        wxTextCtrl* sponsors_text = new wxTextCtrl( sponsors_tab,
            wxID_STATIC, data[3], wxDefaultPosition, internal_size, wxTE_MULTILINE );
        sponsors_text->SetEditable(false);
        sponsors_sizer->Add(sponsors_text, 1, wxEXPAND);
    }

    itemBoxSizer->Add(about_notebook, flags);
    about_notebook->Layout();

    itemBoxSizer->Add(itemStaticText88, flags);

    wxButton* button_OK = new wxButton(this, wxID_OK);
    button_OK->SetDefault();
    button_OK->SetFocus();
    itemBoxSizer->Add(button_OK, flags);
}

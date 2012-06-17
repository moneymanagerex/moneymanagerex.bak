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
        itemCheckBox(),
        retCode_(-1)
{
}


mmAppStartDialog::mmAppStartDialog(wxWindow* parent) :
        itemCheckBox(),
        retCode_(-1)
{
    wxString caption = mmex::getProgramName() + _(" Start Page");
    Create(parent, ID_DIALOG_APPSTART, caption, wxDefaultPosition,
        wxSize(400, 300), wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX);
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
        wxConfigBase *config = wxConfigBase::Get();
        config->Write("SHOWBEGINAPP", itemCheckBox->GetValue());
}


void mmAppStartDialog::CreateControls()
{    
    wxConfigBase *config = wxConfigBase::Get();
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxSizerFlags flags;
    flags.Align(wxALIGN_CENTER).Expand().Border(wxALL, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER|wxALL, 20);

    wxBitmap itemStaticBitmap4Bitmap(money_xpm);
    
    wxStaticBitmap* itemStaticBitmap4;
    itemStaticBitmap4 = new wxStaticBitmap(this, wxID_STATIC, 
                                            itemStaticBitmap4Bitmap);
    
    if (itemStaticBitmap4) {
        itemBoxSizer3->Add(itemStaticBitmap4, wxSizerFlags(0).Center());
    }

    wxButton* itemButton61 = new wxButton(this, ID_BUTTON_APPSTART_LAST_DATABASE, 
        _("Open Last Opened Database"));
    itemBoxSizer3->Add(itemButton61, flags);

    wxButton* itemButton6 = new wxButton( this, ID_BUTTON_APPSTART_NEW_DATABASE, 
        _("Create a New Database"));
    itemButton6->SetToolTip(_("Create a new database file to get started"));
    itemBoxSizer3->Add(itemButton6, flags);

    wxButton* itemButton7 = new wxButton( this, ID_BUTTON_APPSTART_OPEN_DATABASE, 
        _("Open Existing Database"));
    itemButton7->SetToolTip(_("Open an already created database file with extension (*.mmb)"));
    itemBoxSizer3->Add(itemButton7, flags);

    wxButton* itemButton8 = new wxButton( this, ID_BUTTON_APPSTART_HELP, 
        _("Read Documentation"));
    itemButton8->SetToolTip(_("Read the user manual"));
    itemBoxSizer3->Add(itemButton8, flags);

    if (mmIniOptions::instance().enableVisitWebsite_)
    {
       wxButton* itemButton9 = new wxButton(this, ID_BUTTON_APPSTART_WEBSITE, 
          _("Visit Website for more information"));

       wxString s = _("Open the ");
       s += mmex::getProgramName();
       s += _(" website for latest news, updates etc");
       
       itemButton9->SetToolTip(s);
       itemBoxSizer3->Add(itemButton9, flags);
    }

    wxString showAppStartString = wxString::Format(_("Show this window next time %s starts"), mmex::getProgramName());

    itemCheckBox = new wxCheckBox(this, ID_CHECKBOX_APPSTART_SHOWAPPSTART, 
                                    showAppStartString, wxDefaultPosition, 
                                    wxDefaultSize, wxCHK_2STATE);

    itemCheckBox->SetValue(config->ReadBool("SHOWBEGINAPP", false));
    itemBoxSizer3->Add(itemCheckBox);

    wxStaticLine* line = new wxStaticLine (this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    itemBoxSizer3->Add(line, flags);

    itemButtonClose_ = new wxButton(this, wxID_CANCEL);
    itemButtonClose_->SetDefault();
    itemButtonClose_->SetFocus();
    itemBoxSizer3->Add(itemButtonClose_, wxSizerFlags(0).Right().Border(wxTOP, 5));
    itemButtonClose_->Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mmAppStartDialog::OnClose), NULL, this);


    wxString val = config->Read("LASTFILENAME", "");
    if (val.IsEmpty())
        itemButton61->Disable();
    else
      itemButton61->SetToolTip(_("Open the previously opened database : ") + val);
}

void mmAppStartDialog::SetCloseButtonToExit()
{
    itemButtonClose_->SetLabel(_("Exit"));
}

void mmAppStartDialog::OnButtonAppstartNewDatabaseClick( wxCommandEvent& /*event*/ )
{
    retCode_ = appStartDialog(APP_START_NEW_DB);
    EndModal(wxID_CLOSE);
}

void mmAppStartDialog::OnButtonAppstartOpenDatabaseClick( wxCommandEvent& /*event*/ )
{
    retCode_ = appStartDialog(APP_START_OPEN);
    EndModal(wxID_CLOSE);
}

void mmAppStartDialog::OnButtonAppstartHelpClick( wxCommandEvent& /*event*/ )
{
    retCode_ = appStartDialog(APP_START_HELP);
    int helpFileIndex_ = mmex::HTML_INDEX;
    wxFileName helpIndexFile(mmex::getPathDoc((mmex::EDocFile)helpFileIndex_));
    wxString url = ("file://");
    
    if (mmOptions::instance().language != ("english")) helpIndexFile.AppendDir(mmOptions::instance().language);
    
    if (helpIndexFile.FileExists()) // Load the help file for the given language 
    {
        url << (helpIndexFile.GetPathWithSep() + helpIndexFile.GetFullName());
    }
    else // load the default help file
    {
        url << (mmex::getPathDoc((mmex::EDocFile)helpFileIndex_));
    }
    wxLaunchDefaultBrowser(url);
}

void mmAppStartDialog::OnButtonAppstartWebsiteClick( wxCommandEvent& /*event*/ )
{
   retCode_ = appStartDialog(APP_START_WEB);
   wxString url = ("http://www.codelathe.com/mmex/index.php");
   wxLaunchDefaultBrowser(url);
}
    
void mmAppStartDialog::OnButtonAppstartLastDatabaseClick( wxCommandEvent& /*event*/ )
{
    retCode_ = appStartDialog(APP_START_LAST_DB);
    EndModal(wxID_CLOSE);
}

void mmAppStartDialog::OnClose( wxCommandEvent& /*event*/ )
{
    retCode_ = appStartDialog(APP_CLOSE);
    EndModal(wxID_EXIT);
}


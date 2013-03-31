/*************************************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011, 2012 Nikolay & Stefano Giorgio

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
 *************************************************************************/

#include "mmhelppanel.h"
#include "paths.h"
#include "constants.h"
#include "mmex.h"

BEGIN_EVENT_TABLE(mmHelpPanel, wxPanel)
    EVT_BUTTON(wxID_BACKWARD, mmHelpPanel::OnHelpPageBack)
    EVT_BUTTON(wxID_FORWARD, mmHelpPanel::OnHelpPageForward)
END_EVENT_TABLE()

mmHelpPanel::mmHelpPanel(mmGUIFrame* frame, wxSQLite3Database* db,
    wxWindow *parent, wxWindowID winid,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: db_(db)
, frame_(frame)
{
    Create(parent, winid, pos, size, style, name);
}

bool mmHelpPanel::Create( wxWindow *parent, wxWindowID winid,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    return TRUE;
}

void mmHelpPanel::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( this, ID_PANEL_REPORTS_HEADER_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizerHeader = new wxBoxSizer(wxHORIZONTAL);
    itemPanel3->SetSizer(itemBoxSizerHeader);

    wxButton* buttonBack     = new wxButton(itemPanel3, wxID_BACKWARD, _("&Back"));
    wxButton* buttonFordward = new wxButton(itemPanel3, wxID_FORWARD, _("&Forward") );

    wxString helpHeader = mmex::getProgramName() + _(" Help");
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, ID_PANEL_REPORTS_STATIC_HEADER, 
        helpHeader, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));

    itemBoxSizerHeader->Add(buttonBack, 0, wxLEFT, 5);
    itemBoxSizerHeader->Add(buttonFordward, 0, wxLEFT|wxRIGHT, 5);
    itemBoxSizerHeader->Add(itemStaticText9, 0, wxLEFT|wxTOP, 5);

    htmlWindow_ = new wxHtmlWindow( this, ID_PANEL_REPORTS_HTMLWINDOW, 
        wxDefaultPosition, wxDefaultSize, 
        wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    itemBoxSizer2->Add(htmlWindow_, 1, wxGROW|wxALL, 1);
    
    /**************************************************************************
    Allows help files for a specific language.

    Main default help file name: ./help/index.html
    Default filename names can be found in mmex::getPathDoc(fileIndex)
    
    Language files now reside in their own language subdirectory in ./help/
    example: russian language - changed to: ./help/russian/index.html

    Default help files will be used when the language help file are not found.
    **************************************************************************/
    int helpFileIndex = frame_->getHelpFileIndex();
 
    wxFileName helpIndexFile(mmex::getPathDoc((mmex::EDocFile)helpFileIndex));
    if (mmOptions::instance().language_ != wxT("english")) helpIndexFile.AppendDir(mmOptions::instance().language_);

    if (helpIndexFile.FileExists()) // Load the help file for the given language 
    {
        htmlWindow_->LoadPage(helpIndexFile.GetPathWithSep() + helpIndexFile.GetFullName());
    }
    else // load the default help file
    {
        htmlWindow_ ->LoadPage(mmex::getPathDoc((mmex::EDocFile)helpFileIndex));
    }
}

void mmHelpPanel::OnHelpPageBack(wxCommandEvent& /*event*/)
{
    if (htmlWindow_->HistoryCanBack() )
    {
        htmlWindow_->HistoryBack();
        htmlWindow_->SetFocus();
    }
}

void mmHelpPanel::OnHelpPageForward(wxCommandEvent& /*event*/)
{
    if (htmlWindow_->HistoryCanForward() )
    {
        htmlWindow_->HistoryForward();
        htmlWindow_->SetFocus();
    }
}

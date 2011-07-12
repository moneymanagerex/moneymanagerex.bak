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

#include "mmhelppanel.h"
#include "util.h"
#include "dbwrapper.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "paths.h"
#include "constants.h"

#include <wx/app.h>
#include <vector>
#include <algorithm>

BEGIN_EVENT_TABLE(mmHelpPanel, wxPanel)
    EVT_BUTTON(ID_PANEL_REPORTS_HELP_BUTTON_BACK, mmHelpPanel::OnHelpPageBack)
    EVT_BUTTON(ID_PANEL_REPORTS_HELP_BUTTON_FORWARD, mmHelpPanel::OnHelpPageForward)
END_EVENT_TABLE()

mmHelpPanel::mmHelpPanel( mmGUIFrame* frame, wxSQLite3Database* db, wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name )
{
    db_ = db;
    Create(parent, winid, pos, size, style, name);
    frame_ = frame;
}

wxString mmHelpPanel::getReportText()
{
    return htmlWindow_->ToText();
}

mmHelpPanel::~mmHelpPanel()
{

}

bool mmHelpPanel::Create( wxWindow *parent, wxWindowID winid,
            const wxPoint& pos, const wxSize& size, long style,
            const wxString& name  )
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
    mmHelpPanel* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL_REPORTS_HEADER_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizerHeader = new wxBoxSizer(wxHORIZONTAL);
    itemPanel3->SetSizer(itemBoxSizerHeader);

    wxButton* buttonBack     = new wxButton(itemPanel3, ID_PANEL_REPORTS_HELP_BUTTON_BACK,    wxT("<"));
    wxButton* buttonFordward = new wxButton(itemPanel3, ID_PANEL_REPORTS_HELP_BUTTON_FORWARD, wxT(">"));

    wxString helpHeader = mmex::getProgramName() + _(" Help");
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, ID_PANEL_REPORTS_STATIC_HEADER, 
        helpHeader, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));

    itemBoxSizerHeader->Add(buttonBack, 0, wxALL, 1);
    itemBoxSizerHeader->Add(buttonFordward, 0, wxRIGHT, 31);
    itemBoxSizerHeader->Add(itemStaticText9, 0, wxALL, 1);

    htmlWindow_ = new wxHtmlWindow( itemDialog1, ID_PANEL_REPORTS_HTMLWINDOW, 
        wxDefaultPosition, wxDefaultSize, 
        wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    itemBoxSizer2->Add(htmlWindow_, 1, wxGROW|wxALL, 1);
    
    /**************************************************************************
    Allows language seting to set Help language files.
    Default filename name: index.html
            is changed to: index_language.html
    The language specified help file will be used if found in help directory.
    **************************************************************************/
    wxFileName helpIndexFile(mmex::getPathDoc(mmex::HTML_INDEX));
    wxString foreignHelpFileName = helpIndexFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) +
                                   helpIndexFile.GetName() + wxT("_") + mmOptions::language + wxT(".") +
                                   helpIndexFile.GetExt();
    wxFileName foreignHelpFile(foreignHelpFileName);
    if (foreignHelpFile.FileExists())
        htmlWindow_ ->LoadPage(foreignHelpFileName);
    else
       htmlWindow_ ->LoadPage(mmex::getPathDoc(mmex::HTML_INDEX));
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

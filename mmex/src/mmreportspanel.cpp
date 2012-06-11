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

#include "mmreportspanel.h"
#include "util.h"
#include "htmlbuilder.h"
#include "reportbase.h"

BEGIN_EVENT_TABLE(mmReportsPanel, wxPanel)
END_EVENT_TABLE()

mmReportsPanel::mmReportsPanel( mmGUIFrame* frame, wxSQLite3Database* db,  
                               mmPrintableBase* rb, wxWindow *parent,
                               wxWindowID winid, const wxPoint& pos, 
                               const wxSize& size, long style,
                               const wxString& name )
: mmPanelBase(db, NULL), frame_(frame), rb_(rb)
{
    Create(parent, winid, pos, size, style, name);
}

mmReportsPanel::~mmReportsPanel()
{
    delete rb_;
}

bool mmReportsPanel::Create( wxWindow *parent, wxWindowID winid,
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

wxString mmReportsPanel::getReportText()
{
    return rb_->getHTMLText();
}

void mmReportsPanel::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( this, ID_PANEL_REPORTS_HEADER_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizerVHeader);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, ID_PANEL_REPORTS_STATIC_HEADER, 
        _("REPORTS"), wxDefaultPosition, wxDefaultSize, 0 );
    int font_size = this->GetFont().GetPointSize() + 2;
    itemStaticText9->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxBOLD, FALSE, ("")));
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);

    htmlWindow_ = new wxHtmlWindow( this, ID_PANEL_REPORTS_HTMLWINDOW, 
        wxDefaultPosition, wxDefaultSize, 
        wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    itemBoxSizer2->Add(htmlWindow_, 1, wxGROW|wxALL, 1);

    htmlWindow_ -> SetPage(getReportText());
}


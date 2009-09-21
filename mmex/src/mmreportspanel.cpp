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
#include "dbwrapper.h"
#include "htmlbuilder.h"
#include <algorithm>
#include "reportbase.h"

BEGIN_EVENT_TABLE(mmReportsPanel, wxPanel)
END_EVENT_TABLE()

mmReportsPanel::mmReportsPanel( mmGUIFrame* frame, wxSQLite3Database* db,  
                               mmPrintableBase* rb, wxWindow *parent,
                               wxWindowID winid, const wxPoint& pos, 
                               const wxSize& size, long style,
                               const wxString& name )
{
    db_ = db;
    rb_ = rb;
    Create(parent, winid, pos, size, style, name);
    frame_ = frame;
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
    mmReportsPanel* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL_REPORTS_HEADER_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizerVHeader);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, ID_PANEL_REPORTS_STATIC_HEADER, 
        _("REPORTS"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, 
        wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);

    htmlWindow_ = new wxHtmlWindow( itemDialog1, ID_PANEL_REPORTS_HTMLWINDOW, 
        wxDefaultPosition, wxDefaultSize, 
        wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    itemBoxSizer2->Add(htmlWindow_, 1, wxGROW|wxALL, 1);

    htmlWindow_ -> SetPage(getReportText());
}


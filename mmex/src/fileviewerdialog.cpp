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
#include "fileviewerdialog.h"
#include "defs.h"

IMPLEMENT_DYNAMIC_CLASS( fileviewer, wxDialog )

BEGIN_EVENT_TABLE( fileviewer, wxDialog )
END_EVENT_TABLE()

fileviewer::fileviewer( )
{
}

fileviewer::fileviewer(const wxString& fileName, wxWindow* parent, 
                       wxWindowID id,const wxString& caption, 
                       const wxPoint& pos, const wxSize& size, long style )
{
    fileName_ = fileName;
    Create(parent, id, caption, pos, size, style);
}

bool fileviewer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
                        const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
    return TRUE;
}

void fileviewer::CreateControls()
{    
    fileviewer* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxTextCtrl* itemTextCtrl3 = new wxTextCtrl( itemDialog1, 
        ID_TEXTCTRL_FILEVIEWER, wxT(""), 
        wxDefaultPosition, wxSize(400, 300), 
        wxTE_MULTILINE|wxTE_READONLY );
     
    itemBoxSizer2->Add(itemTextCtrl3, 1, 
        wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 10);

    /* Load the Text from the file */
    wxString fileContents = wxT("Empty Contents");
    if ( !fileName_.empty() )
    {
         wxFileInputStream input( fileName_ );
         wxTextInputStream text( input );
          fileContents = wxT("");  
          while (!input.Eof() )
          {
                wxString line = text.ReadLine();
                if (!line.IsEmpty())
                {
                    fileContents += line;
                    fileContents += wxT("\n");
                }
                else
                {
                    fileContents += wxT("\n");
                }
          }
          itemTextCtrl3->SetValue(fileContents);  
    }
}

/*******************************************************
 Copyright (C) 2007 Madhan Kanagavel

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

#include "customsqldialog.h"
#include "util.h"
#include "defs.h"

#if defined (__WXMAC__) or defined (__WXOSX__)
#define MMEX_ICON_FNAME wxStandardPaths::Get().GetResourcesDir() + wxT("/mmex.ico")
#endif


using namespace std;

IMPLEMENT_DYNAMIC_CLASS( mmCustomSQLDialog, wxDialog )


BEGIN_EVENT_TABLE( mmCustomSQLDialog, wxDialog )
    EVT_BUTTON(ID_CUSTOMSQLBUTTON_LOAD, mmCustomSQLDialog::OnLoad)
    EVT_BUTTON(ID_CUSTOMSQLBUTTON_SAVE, mmCustomSQLDialog::OnSave)
    EVT_BUTTON(ID_CUSTOMSQLBUTTON_QUERY, mmCustomSQLDialog::OnQuery)
END_EVENT_TABLE()


mmCustomSQLDialog::mmCustomSQLDialog( )
{
}

mmCustomSQLDialog::mmCustomSQLDialog(mmCoreDB* core, 
                   wxWindow* parent, wxWindowID id, 
                   const wxString& caption, 
                   const wxPoint& pos, 
                   const wxSize& size, 
                   long style ) : db_ (core->db_.get()), core_(core)
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * MyDialog creator
 */

bool mmCustomSQLDialog::Create(  wxWindow* parent, wxWindowID id, 
                      const wxString& caption, 
                      const wxPoint& pos, 
                      const wxSize& size, 
                      long style )
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

/*!
 * Control creation for MyDialog
 */

void mmCustomSQLDialog::CreateControls()
{    
////@begin MyDialog content construction
    // Generated by DialogBlocks, 06/24/06 17:00:25 (Personal Edition)

    mmCustomSQLDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Custom SQL Query"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

	//Arranger Area
	wxPanel* itemPanel_Arranger = new wxPanel( itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel_Arranger, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer_Arranger = new wxBoxSizer(wxVERTICAL);
    itemPanel_Arranger->SetSizer(itemBoxSizer_Arranger);

	//ListBox of attribute order
    queryBox_ = new wxTextCtrl( itemDialog1, ID_TEXTBOX, _T(""),
        wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE );
    itemBoxSizer3->Add(queryBox_, 1, wxGROW|wxALL, 1);

    wxPanel* itemPanel5 = new wxPanel( itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel5->SetSizer(itemBoxSizer6);

	//Load Template button
    wxButton* itemButton1_Load = new wxButton( itemPanel5, ID_CUSTOMSQLBUTTON_LOAD, _("&Load SQL"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton1_Load, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//Save As Template button
    wxButton* itemButton_Save = new wxButton( itemPanel5, ID_CUSTOMSQLBUTTON_SAVE, _("&Save SQL"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton_Save, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//Import File button
    wxButton* itemButton_Import = new wxButton( itemPanel5, ID_CUSTOMSQLBUTTON_QUERY, _("&Run Query"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton_Import, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end MyDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool mmCustomSQLDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap mmCustomSQLDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin MyDialog bitmap retrieval
    return wxNullBitmap;
////@end MyDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon mmCustomSQLDialog::GetIconResource( const wxString& name )
{
    return wxNullIcon;
}

void mmCustomSQLDialog::OnLoad(wxCommandEvent& event)
{
   wxString fileName = wxFileSelector(wxT("Choose Custom SQL file to load"), 
      wxT(""), wxT(""), wxT(""),  wxT("SQL File(*.sql)|*.sql"), wxFILE_MUST_EXIST);
   if ( !fileName.empty() )
   {
      wxTextFile tFile(fileName);
      if (!tFile.Open())
      {
         mmShowErrorMessage(0, 
            _("Unable to open file."),
            _("Error"));
         return;
      }

      wxString str;
      wxString fullStr;
      for ( str = tFile.GetFirstLine(); !tFile.Eof(); str = tFile.GetNextLine() )
      {
        fullStr += wxT(" ") + str;
      }
      queryBox_->SetValue(fullStr);
      tFile.Close();
   }
}

//Saves the field order to a template file
void mmCustomSQLDialog::OnSave(wxCommandEvent& event)
{
     wxString fileName = wxFileSelector(wxT("Choose Custom SQL file to save"), 
                wxT(""), wxT(""), wxT(""), wxT("SQL File(*.sql)|*.sql"), wxSAVE);
    if ( !fileName.empty() )
    {
         wxTextFile tFile(fileName);
		 //if the file does not exist and cannot be created, throw an error
		 //if the file does exist, then skip to else section
         if ( !tFile.Exists() && !tFile.Create() )
         {
            mmShowErrorMessage(0, 
               _("Unable to write to file."),
               _("Error"));
            return;
         }
		 else{
			 //clear the contents of the current file
			 tFile.Clear();
             tFile.AddLine(queryBox_->GetValue());
		 }
         tFile.Write();
         tFile.Close();
    }
}

void mmCustomSQLDialog::OnQuery(wxCommandEvent& event)
{
   wxString lower = queryBox_->GetValue().Lower();
   if ((lower.Find(wxT("update")) != wxNOT_FOUND) ||
       (lower.Find(wxT("delete")) != wxNOT_FOUND) ||
       (lower.Find(wxT("insert")) != wxNOT_FOUND))
   {
		wxMessageDialog msgDlg(this, _("SQL Query will modify your Data. Proceed??"),
									_("Warning"),
								    wxYES_NO);
        if (msgDlg.ShowModal() == wxID_YES)
		{
            sqlQuery_ = queryBox_->GetValue();
            EndModal(wxID_OK);
        }
        else
        {
            return;
        }
   }
   else
   {
       sqlQuery_ = queryBox_->GetValue();
       EndModal(wxID_OK);
   }
}

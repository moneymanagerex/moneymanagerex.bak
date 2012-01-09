/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011 Stefano Giorgio

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

#include "relocatepayeedialog.h"
#include "paths.h"
#include "payeedialog.h"
#include "wx/statline.h"

IMPLEMENT_DYNAMIC_CLASS( relocatePayeeDialog, wxDialog )

BEGIN_EVENT_TABLE( relocatePayeeDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_PAYEE_SELECT_BUTTON_SOURCE, relocatePayeeDialog::OnSelectSource)
    EVT_BUTTON(ID_DIALOG_PAYEE_SELECT_BUTTON_DEST, relocatePayeeDialog::OnSelectDest)
    EVT_BUTTON(ID_DIALOG_PAYEE_RELOCATE_BUTTON_OK, relocatePayeeDialog::OnOk)
END_EVENT_TABLE()

relocatePayeeDialog::relocatePayeeDialog( )
{
    core_           =  0;
    db_             =  0;

    sourcePayeeID_  = -1;
    destPayeeID_    = -1;
    changedPayees_  = 0;
}

relocatePayeeDialog::relocatePayeeDialog( mmCoreDB* core, wxSQLite3Database* db,
    wxWindow* parent, wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size, long style )
{
    core_           = core;
    db_             = db;

    sourcePayeeID_  = -1;
    destPayeeID_    = -1;
    
    Create(parent, id, caption, pos, size, style);
}

bool relocatePayeeDialog::Create( wxWindow* parent, wxWindowID id,
                           const wxString& caption, const wxPoint& pos,
                           const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    Centre();
    return TRUE;
}

void relocatePayeeDialog::CreateControls()
{
    wxSize btnSize = wxSize(180,-1);
    wxStaticText* headerText = new wxStaticText( this, wxID_STATIC,
        _("Relocate all source payees to the destination payee"), 
        wxDefaultPosition, wxDefaultSize, 0);
    wxStaticLine* lineTop = new wxStaticLine(this,wxID_STATIC,wxDefaultPosition, wxDefaultSize,wxLI_HORIZONTAL); 
    wxStaticText* staticText_1 = new wxStaticText( this, wxID_STATIC,_("Relocate:"),
        wxDefaultPosition, wxDefaultSize, 0 );
    sourceBtn_ = new wxButton( this, ID_DIALOG_PAYEE_SELECT_BUTTON_SOURCE,_("Source Payee"),
        wxDefaultPosition, btnSize, 0 );
    wxStaticText* staticText_2 = new wxStaticText( this, wxID_STATIC,_("to:"),
        wxDefaultPosition, wxDefaultSize, 0 );
    destBtn_ = new wxButton( this, ID_DIALOG_PAYEE_SELECT_BUTTON_DEST,_("Destination Payee"),
        wxDefaultPosition, btnSize, 0 );
    wxStaticLine* lineBottom = new wxStaticLine(this,wxID_STATIC,wxDefaultPosition, wxDefaultSize,wxLI_HORIZONTAL); 
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(topSizer);
    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(boxSizer,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);

    boxSizer->Add(headerText,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);
    boxSizer->Add(lineTop,0,wxGROW|wxALL,5);

    wxBoxSizer* requestBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    requestBoxSizer->Add(staticText_1,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    requestBoxSizer->Add(sourceBtn_,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL);
    requestBoxSizer->Add(5,10,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);
    requestBoxSizer->Add(staticText_2,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL);
    requestBoxSizer->Add(destBtn_,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    boxSizer->Add(requestBoxSizer);
    boxSizer->Add(5,5,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);
    boxSizer->Add(lineBottom,0,wxGROW|wxALL,5);

    wxButton* okButton = new wxButton(this,ID_DIALOG_PAYEE_RELOCATE_BUTTON_OK,_("OK"),wxDefaultPosition,wxDefaultSize);
    wxButton* cancelButton = new wxButton(this,wxID_CANCEL,_("Cancel"),wxDefaultPosition,wxDefaultSize);
    cancelButton-> SetFocus () ;
    wxBoxSizer* buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonBoxSizer->Add(okButton,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    buttonBoxSizer->Add(cancelButton,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    boxSizer->Add(buttonBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
}

void relocatePayeeDialog::OnSelectSource(wxCommandEvent& /*event*/)
{
    mmPayeeDialog* sourcePayee = new mmPayeeDialog(this, core_);
    if (sourcePayee->ShowModal() == wxID_OK)
    {
        sourcePayeeID_    = sourcePayee->getPayeeId();

        wxString payee_name; 
        if (sourcePayeeID_ != -1 )
            payee_name = mmDBWrapper::getPayee(db_, sourcePayeeID_);

        if (payee_name != wxT(""))
            sourceBtn_->SetLabel(payee_name);
    }
}

void relocatePayeeDialog::OnSelectDest(wxCommandEvent& /*event*/)
{
    mmPayeeDialog* destPayee = new mmPayeeDialog(this, core_);
    if (destPayee->ShowModal() == wxID_OK)
    {
        destPayeeID_    = destPayee->getPayeeId();

        wxString payee_name; 
        if (destPayeeID_ != -1 )
            payee_name = mmDBWrapper::getPayee(db_, destPayeeID_);
        if (payee_name != wxT(""))
            destBtn_->SetLabel(payee_name);
    }
}

wxString relocatePayeeDialog::updatedPayeesCount()
{
    wxString countStr;
    countStr << changedPayees_;
    return countStr;
}


void relocatePayeeDialog::OnOk(wxCommandEvent& /*event*/)
{
    if ((sourcePayeeID_ > 0) &&  (destPayeeID_ > 0) ) 
    {
        wxString msgStr = _("Please Confirm:") ;
        msgStr << wxT("\n\n") << _ ("Changing all payees of: ") << sourceBtn_->GetLabelText() << wxT("\n\n") << _("to payee: ") << destBtn_->GetLabelText();

        int ans = wxMessageBox(msgStr,_("Payee Relocation Confirmation"),wxOK|wxCANCEL|wxICON_QUESTION);
        if (ans == wxOK)
        {
            static const char sql[] = "update checkingaccount_v1 set payeeid = ? where payeeid = ? ";
            wxSQLite3Statement st = db_->PrepareStatement(sql);
            st.Bind(1, destPayeeID_);
            st.Bind(2, sourcePayeeID_);
            try {
                changedPayees_ = st.ExecuteUpdate();
                st.Finalize();
			} catch(wxSQLite3Exception e) 
			{
				wxLogDebug(wxT("update checkingaccount_v1 : Exception"), e.GetMessage().c_str());
				wxLogError(wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
			}

            EndModal(wxID_OK);
        }
    }
}


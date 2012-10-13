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

#define SQLITE_OK           0   /* Successful result */

IMPLEMENT_DYNAMIC_CLASS( relocatePayeeDialog, wxDialog )

BEGIN_EVENT_TABLE( relocatePayeeDialog, wxDialog )
    EVT_BUTTON(wxID_CLEAR, relocatePayeeDialog::OnSelectSource)
    EVT_BUTTON(wxID_NEW, relocatePayeeDialog::OnSelectDest)
    EVT_BUTTON(wxID_OK, relocatePayeeDialog::OnOk)
END_EVENT_TABLE()

relocatePayeeDialog::relocatePayeeDialog( )
{
    core_           =  0;

    sourcePayeeID_  = -1;
    destPayeeID_    = -1;
    changedPayees_  = 0;
}

relocatePayeeDialog::relocatePayeeDialog( mmCoreDB* core,
    wxWindow* parent, wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size, long style )
{
    core_           = core;

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
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Center();
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Expand().Center();
    wxSize btnSize = wxSize(180,-1);

    wxStaticText* headerText = new wxStaticText( this, wxID_STATIC,
        _("Relocate all source payees to the destination payee"));
    wxStaticLine* lineTop = new wxStaticLine(this,wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    sourceBtn_ = new wxButton( this, wxID_CLEAR,_("Source Payee"),
        wxDefaultPosition, btnSize);
    destBtn_ = new wxButton( this, wxID_NEW, _("Destination Payee"),
        wxDefaultPosition, btnSize);
    wxStaticLine* lineBottom = new wxStaticLine(this,wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(topSizer);
    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(boxSizer, flags);
    wxFlexGridSizer* request_sizer = new wxFlexGridSizer(0, 2, 0, 0);

    boxSizer->Add(headerText, flags);
    boxSizer->Add(lineTop, flagsExpand);

    request_sizer->Add(new wxStaticText( this, wxID_STATIC,_("Relocate:")), flags);
    request_sizer->Add(sourceBtn_, flags);
    request_sizer->Add(new wxStaticText( this, wxID_STATIC,_("to:")), flags);
    request_sizer->Add(destBtn_, flags);
    boxSizer->Add(request_sizer, flagsExpand);
    boxSizer->Add(lineBottom, flagsExpand);

    wxButton* okButton = new wxButton(this,wxID_OK);
    wxButton* cancelButton = new wxButton(this,wxID_CANCEL);
    cancelButton-> SetFocus () ;
    wxBoxSizer* buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonBoxSizer->Add(okButton, flags);
    buttonBoxSizer->Add(cancelButton, flags);
    boxSizer->Add(buttonBoxSizer, flags);
}

void relocatePayeeDialog::OnSelectSource(wxCommandEvent& /*event*/)
{
    mmPayeeDialog* sourcePayee = new mmPayeeDialog(this, core_);
    if (sourcePayee->ShowModal() == wxID_OK)
    {
        sourcePayeeID_    = sourcePayee->getPayeeId();

        wxString payee_name;
        if (sourcePayeeID_ != -1 )
            payee_name = core_->payeeList_.GetPayeeName(sourcePayeeID_);

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
            payee_name = core_->payeeList_.GetPayeeName(destPayeeID_);
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
        msgStr << wxT("\n\n");
        msgStr << wxString::Format( _("Changing all payees of: %s \n\n  to payee: %s"),
             sourceBtn_->GetLabelText().c_str(), destBtn_->GetLabelText().c_str());

        int ans = wxMessageBox(msgStr,_("Payee Relocation Confirmation"), wxOK|wxCANCEL|wxICON_QUESTION);
        if (ans == wxOK)
        {
            if (core_->bTransactionList_.RelocatePayee(core_, destPayeeID_, sourcePayeeID_, changedPayees_) == 0)
            {


                EndModal(wxID_OK);
            }
        }
    }
}


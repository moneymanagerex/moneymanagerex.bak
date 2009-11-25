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

#include "payeedialog.h"
#include "util.h"
#include "defs.h"
#include "paths.h"

#include <wx/event.h>


IMPLEMENT_DYNAMIC_CLASS( mmPayeeDialog, wxDialog )

BEGIN_EVENT_TABLE( mmPayeeDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_OK, mmPayeeDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_ADD, mmPayeeDialog::OnAdd)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_DELETE, mmPayeeDialog::OnDelete)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_SELECT, mmPayeeDialog::OnBSelect)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_EDIT, mmPayeeDialog::OnEdit)
    EVT_LISTBOX(ID_DIALOG_PAYEE_LISTBOX_PAYEES, mmPayeeDialog::OnSelChanged)
    EVT_LISTBOX_DCLICK(ID_DIALOG_PAYEE_LISTBOX_PAYEES, mmPayeeDialog::OnDoubleClicked)
    EVT_CHAR_HOOK(mmPayeeDialog::OnListKeyDown)    
	EVT_TEXT(ID_DIALOG_PAYEE_TEXTCTRL_PAYEENAME, mmPayeeDialog::OnTextCtrlChanged)
END_EVENT_TABLE()


// EVT_TEXT_ENTER(ID_DIALOG_TRANS_PAYEECOMBO, mmPayeeDialog::OnComboSelected)
//EVT_TEXT(ID_DIALOG_TRANS_PAYEECOMBO, mmPayeeDialog::OnComboSelected)
   
mmPayeeDialog::mmPayeeDialog( )
{
    payeeID_ = -1;
}

void mmPayeeDialog::OnListKeyDown(wxKeyEvent &event)
{
    long keycode = event.GetKeyCode();

    if(keycode == 13 && showSelectButton_)
    {
        wxCommandEvent event;
        OnBSelect(event);
    }
    else if (keycode == 27)
    {
        payeeID_ = -1;
         Close(FALSE);
    }

    event.Skip();
}

mmPayeeDialog::mmPayeeDialog(mmCoreDB* core,
                             wxWindow* parent, 
                             bool showSelectButton, 
                             wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
 
    core_ = core;
    payeeID_ = -1;
    showSelectButton_ = showSelectButton;
    Create(parent, id, caption, pos, size, style | wxWANTS_CHARS);
}

bool mmPayeeDialog::Create( wxWindow* parent, wxWindowID id, 
                           const wxString& caption, const wxPoint& pos, 
                           const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    if (!showSelectButton_)
    {
        selectButton->Disable();
    }

    SetIcon(mmex::getProgramIcon());
    
    fillControls();

    Centre();
    return TRUE;
}

void mmPayeeDialog::fillControls()
{
	listBox_->Clear();
	wxArrayString filtd = mmDBWrapper::filterPayees(core_->db_.get(),textCtrl->GetValue());
	for (int i=0; i < filtd.GetCount(); i++)
	{
		listBox_->Append(filtd.Item(i));
	}
}

void mmPayeeDialog::CreateControls()
{    
    mmPayeeDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

        wxStaticText* itemStaticTextName = new wxStaticText( itemDialog1, wxID_STATIC, 
            _("Find Payee: "), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer2->Add(itemStaticTextName, 0,
            wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);


     wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);
   
    listBox_ = new wxListBox( itemDialog1, ID_DIALOG_PAYEE_LISTBOX_PAYEES, 
        wxDefaultPosition, wxSize(100, 200), wxArrayString(), wxLB_SINGLE);
    itemBoxSizer3->Add(listBox_, 1, wxGROW|wxALL, 1);

    wxStaticText* itemStaticTextName2 = new wxStaticText( itemDialog1, wxID_STATIC, 
            _("Filter Payees: "), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer2->Add(itemStaticTextName2, 0,
            wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    textCtrl = new wxTextCtrl( itemDialog1, ID_DIALOG_PAYEE_TEXTCTRL_PAYEENAME, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(textCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textCtrl->SetToolTip(_("Enter a search string.  You can use % as a wildcard to match zero or more characters or _ to match a single character."));

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxGROW|wxALL, 5);

    addButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_ADD, 
        _("&Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(addButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    addButton->SetToolTip(_("Add a new payee name"));
	addButton->Disable();

    editButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_EDIT, 
        _("&Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(editButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    editButton->SetToolTip(_("Change the name of an existing payee"));
    editButton->Disable();
	
    deleteButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_DELETE, 
        _("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(deleteButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    deleteButton->SetToolTip(_("Delete the selected payee. The payee cannot be used by an existing transaction."));
	deleteButton->Disable();
	
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxALL, 5);

    selectButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_SELECT, 
        _("&Select"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(selectButton, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    selectButton->SetToolTip(_("Select the currently selected payee as the selected payee for the transaction"));
	selectButton->Disable();
}

void mmPayeeDialog::OnTextCtrlChanged(wxCommandEvent& event)
{
	wxString payeeString = listBox_->GetStringSelection();
	fillControls();
	if (textCtrl->GetValue() != wxGetEmptyString())
	{
		addButton->Enable();
		listBox_->SetSelection(0);
	}
	else 
	{
		addButton->Disable();
		listBox_->SetStringSelection(payeeString);
	}
	OnSelChanged(event);
}


void mmPayeeDialog::OnAdd(wxCommandEvent& event)
{
    wxString text = wxGetTextFromUser(_("Enter the name of the payee to add:"), _("Add Payee"), textCtrl->GetValue());
	if (text == wxGetEmptyString())
    {
        //mmShowErrorMessage(this, _("Type a Payee Name in the Text Box and then press Add."), _("Error"));
        return;
    }
    if (core_->payeeList_.payeeExists(text))
    {
        mmShowErrorMessage(this, _("Payee with same name exists"), _("Error"));
    }
    else
    {
        core_->payeeList_.addPayee(text);
		textCtrl->SetValue(wxGetEmptyString());
        fillControls();

        listBox_->SetStringSelection(text);
		
		// SetStringSelection does not emit event, so we need to do it manually.
		// This is important because it is where payeeID_ gets set
		OnSelChanged(event);
    }
}
 
void mmPayeeDialog::OnDelete(wxCommandEvent& event)
{
    if (!core_->payeeList_.deletePayee(payeeID_))
    {
        mmShowErrorMessage(this, _("Payee is in use"), _("Error"));
        return;
    }
	textCtrl->SetValue(wxGetEmptyString());
    fillControls();
	OnSelChanged(event);
}
 
void mmPayeeDialog::OnBSelect(wxCommandEvent& /*event*/)
{
    if (payeeID_ != -1)
    {
        EndModal(wxID_OK);
        return;
    }
}

void mmPayeeDialog::OnOk(wxCommandEvent& /*event*/)
{
    Close(TRUE);
}

void mmPayeeDialog::OnSelChanged(wxCommandEvent& /*event*/)
{
    wxString payeeString = listBox_->GetStringSelection();
    if (payeeString==wxGetEmptyString())
	{
		editButton->Disable();
		deleteButton->Disable();
		selectButton->Disable();
		payeeID_ = -1;
        return;
	}
	if (showSelectButton_)
		selectButton->Enable();
	editButton->Enable();
	deleteButton->Enable();
    payeeID_ = core_->payeeList_.getPayeeID(payeeString);
	
}

void mmPayeeDialog::OnDoubleClicked(wxCommandEvent& event)
{
    OnBSelect(event);
}

void mmPayeeDialog::OnEdit(wxCommandEvent& event)
{
	wxString oldname = listBox_->GetStringSelection();
	
	wxString mesg;
	mesg.Printf(_("Enter a new name for %s"),oldname.c_str());
	wxString newName = wxGetTextFromUser(mesg, _("Edit Payee Name"), oldname);
    if (newName != wxGetEmptyString())
	{
		core_->payeeList_.updatePayee(payeeID_, newName);
		core_->bTransactionList_.updateAllTransactionsForPayee(core_, payeeID_);
		editButton->Disable();
		fillControls();
		
		// Now we need to make sure that the edited name is selected after the dialog is closed
		textCtrl->SetValue(wxGetEmptyString());
		listBox_->SetStringSelection(newName);
		OnSelChanged(event);
	}
}

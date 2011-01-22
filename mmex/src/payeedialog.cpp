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
#include "dbwrapper.h"
#include "mmcoredb.h"

#include <wx/event.h>

namespace
{

enum {
  IDD_BUTTON_SELECT = wxID_OK,
  IDD_TEXTCTRL_PAYEENAME = wxID_HIGHEST + 1,
  IDD_LISTBOX_PAYEES,
  IDD_BUTTON_ADD,
  IDD_BUTTON_DELETE,
  IDD_BUTTON_EDIT
};

} // namespace


IMPLEMENT_DYNAMIC_CLASS( mmPayeeDialog, wxDialog )

BEGIN_EVENT_TABLE( mmPayeeDialog, wxDialog )
    EVT_BUTTON(IDD_BUTTON_ADD, mmPayeeDialog::OnAdd)
    EVT_BUTTON(IDD_BUTTON_DELETE, mmPayeeDialog::OnDelete)
    EVT_BUTTON(IDD_BUTTON_SELECT, mmPayeeDialog::OnBSelect)
    EVT_BUTTON(IDD_BUTTON_EDIT, mmPayeeDialog::OnEdit)
    EVT_LISTBOX(IDD_LISTBOX_PAYEES, mmPayeeDialog::OnSelChanged)
    EVT_LISTBOX_DCLICK(IDD_LISTBOX_PAYEES, mmPayeeDialog::OnDoubleClicked)
    EVT_TEXT(IDD_TEXTCTRL_PAYEENAME, mmPayeeDialog::OnTextCtrlChanged)
    EVT_CHAR_HOOK(mmPayeeDialog::OnListKeyDown)    
END_EVENT_TABLE()


mmPayeeDialog::mmPayeeDialog(wxWindow *parent, mmCoreDB *core, bool showSelectButton) : 
	m_payee_id(-1),
	m_core(core),
	showSelectButton_(showSelectButton)
{
	do_create(parent);
}

void mmPayeeDialog::do_create(wxWindow* parent)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);

    long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER | wxWANTS_CHARS;
    if (!wxDialog::Create(parent, wxID_ANY, _("Organize Payees"), wxDefaultPosition, wxDefaultSize, style)) {
    	return;
    }

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
}

void mmPayeeDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer2);

        wxStaticText* itemStaticTextName = new wxStaticText( this, wxID_STATIC, 
            _("Find Payee: "), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer2->Add(itemStaticTextName, 0,
            wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);


     wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);
   
    listBox_ = new wxListBox( this, IDD_LISTBOX_PAYEES, 
        wxDefaultPosition, wxSize(100, 200), wxArrayString(), wxLB_SINGLE);
    itemBoxSizer3->Add(listBox_, 1, wxGROW|wxALL, 1);

    wxStaticText* itemStaticTextName2 = new wxStaticText( this, wxID_STATIC, _("Filter Payees: "), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer2->Add(itemStaticTextName2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    textCtrl = new wxTextCtrl( this, IDD_TEXTCTRL_PAYEENAME, 
        wxGetEmptyString(), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(textCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textCtrl->SetToolTip(_("Enter a search string.  You can use % as a wildcard to match zero or more characters or _ to match a single character."));
    textCtrl->SetFocus();

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxGROW|wxALL, 5);

    addButton = new wxButton( this, IDD_BUTTON_ADD, 
        _("&Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(addButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    addButton->SetToolTip(_("Add a new payee name"));
	addButton->Disable();

    editButton = new wxButton( this, IDD_BUTTON_EDIT, 
        _("&Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(editButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    editButton->SetToolTip(_("Change the name of an existing payee"));
    editButton->Disable();
	
    deleteButton = new wxButton( this, IDD_BUTTON_DELETE, _("&Delete"));
    itemBoxSizer5->Add(deleteButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    deleteButton->SetToolTip(_("Delete the selected payee. The payee cannot be used by an existing transaction."));
	deleteButton->Disable();
	
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxALL, 5);

    selectButton = new wxButton( this, IDD_BUTTON_SELECT, _("&Select"));
    itemBoxSizer9->Add(selectButton, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    selectButton->SetToolTip(_("Select the currently selected payee as the selected payee for the transaction"));
    selectButton->Disable();
}

void mmPayeeDialog::OnListKeyDown(wxKeyEvent &event)
{
    long keycode = event.GetKeyCode();

    if(keycode == 13 && showSelectButton_) {
        wxCommandEvent event;
        OnBSelect(event);
    } else if (keycode == 27) {
        m_payee_id = -1;
        Close();
    }

    event.Skip();
}

void mmPayeeDialog::fillControls()
{
	wxArrayString filtd = mmDBWrapper::filterPayees(m_core->db_.get(), textCtrl->GetValue());

	listBox_->Clear();

	for (size_t i = 0; i < filtd.GetCount(); ++i) {
		listBox_->Append(filtd.Item(i));
	}
}

void mmPayeeDialog::OnTextCtrlChanged(wxCommandEvent& event)
{
	bool filter_ok = !textCtrl->GetValue().IsEmpty();
	addButton->Enable(filter_ok);
	
	wxString payee = listBox_->GetStringSelection();
	fillControls();
	
	if (!listBox_->IsEmpty() && !listBox_->SetStringSelection(payee)) {
		listBox_->Select(0);
	}

	OnSelChanged(event);
}

void mmPayeeDialog::OnSelChanged(wxCommandEvent& /*event*/)
{
	wxString payee = listBox_->GetStringSelection();

	m_payee_id = payee.IsEmpty() ? -1 : m_core->payeeList_.getPayeeID(payee);
	bool ok = m_payee_id != -1;

	editButton->Enable(ok);
	deleteButton->Enable(ok);
	selectButton->Enable(ok && showSelectButton_);
}

void mmPayeeDialog::OnAdd(wxCommandEvent& event)
{
    wxString text = wxGetTextFromUser(_("Enter the name of the payee to add:"), _("Add Payee"), textCtrl->GetValue());
    if (text.IsEmpty()) {
        //mmShowErrorMessage(this, _("Type a Payee Name in the Text Box and then press Add."), _("Error"));
        return;
    }
    if (m_core->payeeList_.payeeExists(text))
    {
        mmShowErrorMessage(this, _("Payee with same name exists"), _("Error"));
    }
    else
    {
        m_core->payeeList_.addPayee(text);
	textCtrl->Clear();
        fillControls();

        listBox_->SetStringSelection(text);
		
		// SetStringSelection does not emit event, so we need to do it manually.
		// This is important because it is where m_payee_id gets set
		OnSelChanged(event);
    }
}
 
void mmPayeeDialog::OnDelete(wxCommandEvent& event)
{
    if (!m_core->payeeList_.deletePayee(m_payee_id))
    {
        mmShowErrorMessage(this, _("Payee is in use"), _("Error"));
        return;
    }
	textCtrl->Clear();
    fillControls();
	OnSelChanged(event);
}
 
void mmPayeeDialog::OnBSelect(wxCommandEvent& /*event*/)
{
	if (m_payee_id != -1) {
		EndModal(wxID_OK);
	}
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
		m_core->payeeList_.updatePayee(m_payee_id, newName);
		m_core->bTransactionList_.updateAllTransactionsForPayee(m_core, m_payee_id);
		editButton->Disable();
		fillControls();
		
		// Now we need to make sure that the edited name is selected after the dialog is closed
		textCtrl->Clear();
		listBox_->SetStringSelection(newName);
		OnSelChanged(event);
	}
}

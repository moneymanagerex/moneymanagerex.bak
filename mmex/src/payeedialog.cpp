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
    IDD_TEXTCTRL_PAYEENAME = wxID_HIGHEST + 1,
    IDD_LISTBOX_PAYEES,
};

} // namespace


IMPLEMENT_DYNAMIC_CLASS( mmPayeeDialog, wxDialog )

BEGIN_EVENT_TABLE( mmPayeeDialog, wxDialog )
    EVT_BUTTON(wxID_ADD, mmPayeeDialog::OnAdd)
    EVT_BUTTON(wxID_REMOVE, mmPayeeDialog::OnDelete)
    EVT_BUTTON(wxID_OK, mmPayeeDialog::OnBSelect)
    EVT_BUTTON(wxID_EDIT, mmPayeeDialog::OnEdit)
    EVT_LISTBOX(IDD_LISTBOX_PAYEES, mmPayeeDialog::OnSelChanged)
    EVT_LISTBOX_DCLICK(IDD_LISTBOX_PAYEES, mmPayeeDialog::OnDoubleClicked)
    EVT_TEXT(IDD_TEXTCTRL_PAYEENAME, mmPayeeDialog::OnTextCtrlChanged)
    EVT_CHAR_HOOK(mmPayeeDialog::OnListKeyDown)
END_EVENT_TABLE()


mmPayeeDialog::mmPayeeDialog(wxWindow *parent, mmCoreDB *core, bool showSelectButton) :
    m_payee_id(-1),
    core_(core),
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
    const int border = 5;
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, border);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, border).Expand();

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticTextName = new wxStaticText( this, wxID_STATIC, _("Find Payee: "));
    itemBoxSizer2->Add(itemStaticTextName, flags);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, flagsExpand);

    wxArrayString filtd = core_->payeeList_.FilterPayees(wxT(""));
    int vertical_size_ = (filtd.GetCount()>10 ? 320 : 240);
    listBox_ = new wxListBox( this, IDD_LISTBOX_PAYEES, wxDefaultPosition, wxSize(100, vertical_size_), wxArrayString(), wxLB_SINGLE);
    itemBoxSizer3->Add(listBox_, 1, wxGROW|wxALL, 1);

    itemBoxSizer2->Add(new wxStaticText( this, wxID_STATIC, _("Filter Payees: ")), flags);

    textCtrl = new wxTextCtrl( this, IDD_TEXTCTRL_PAYEENAME, wxGetEmptyString());
    itemBoxSizer2->Add(textCtrl, flagsExpand);
    textCtrl->SetFocus();

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, flagsExpand);

    addButton = new wxButton( this, wxID_ADD);
    itemBoxSizer5->Add(addButton, flags.Border(1));
    addButton->Disable();

    editButton = new wxButton( this, wxID_EDIT);
    itemBoxSizer5->Add(editButton, flags);
    editButton->Disable();

    deleteButton = new wxButton( this, wxID_REMOVE);
    itemBoxSizer5->Add(deleteButton, flags);
    deleteButton->Disable();

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, flagsExpand);

    selectButton = new wxButton( this, wxID_OK, _("&Select"));
    itemBoxSizer9->Add(selectButton, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    selectButton->Disable();

    //Some interfaces has no any close buttons, it may confuse user. Cancel button added
    wxButton* itemCancelButton = new wxButton( this, wxID_CANCEL);
    itemBoxSizer9->Add(itemCancelButton,  flags);

    textCtrl->SetToolTip(_("Enter a search string.  You can use * as a wildcard to match zero or more characters or ? to match a single character."));
    addButton->SetToolTip(_("Add a new payee name"));
    editButton->SetToolTip(_("Change the name of an existing payee"));
    deleteButton->SetToolTip(_("Delete the selected payee. The payee cannot be used by an existing transaction."));
    selectButton->SetToolTip(_("Select the currently selected payee as the selected payee for the transaction"));
}

void mmPayeeDialog::OnListKeyDown(wxKeyEvent &event)
{
    long keycode = event.GetKeyCode();

    // Check to see if the up or down arrow is pressed while text control has focus
    wxWindow *win = wxWindow::FindFocus();
    wxTextCtrl *text = wxDynamicCast(win, wxTextCtrl);
    // if text, then text control has focus
    if ( text && (keycode == WXK_DOWN) && ( listBox_->GetSelection() < (int)listBox_->GetCount()-1) )
    {
        listBox_->Select(listBox_->GetSelection()+1);
        wxCommandEvent event;
        OnSelChanged(event);
    }
    else if ( text && (keycode == WXK_UP) && ( listBox_->GetSelection() > 0) )
    {
        listBox_->Select(listBox_->GetSelection()-1);
        wxCommandEvent event;
        OnSelChanged(event);
    }
    else
    {
        // text control doesn't have focus OR up/down not pressed OR up/down not possible
    }

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
    wxArrayString filtd = core_->payeeList_.FilterPayees(textCtrl->GetValue());

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

    m_payee_id = payee.IsEmpty() ? -1 : core_->payeeList_.GetPayeeId(payee);
    bool ok = m_payee_id != -1;

    editButton->Enable(ok);
    deleteButton->Enable(ok);
    selectButton->Enable(ok && showSelectButton_);
}

void mmPayeeDialog::OnAdd(wxCommandEvent& event)
{
    wxString text = wxGetTextFromUser(_("Enter the name for the new payee:")
        , _("Organize Payees: Add Payee"), textCtrl->GetValue());
    if (text.IsEmpty()) {
        return;
    }

    if (core_->payeeList_.PayeeExists(text))
    {
        wxMessageBox(_("Payee with same name exists"), _("Organize Payees: Add Payee"), wxOK|wxICON_ERROR);
    }
    else
    {
        core_->payeeList_.AddPayee(text);
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
    if (!core_->payeeList_.RemovePayee(m_payee_id))
    {
        wxString deletePayeeErrMsg = _("Payee in use.");
        deletePayeeErrMsg
            << wxT("\n\n")
            << _("Tip: Change all transactions using this Payee to another Payee\nusing the relocate command:")
            << wxT("\n\n") << _("Tools -> Relocation of -> Payees");
        wxMessageBox(deletePayeeErrMsg,_("Organize Payees: Delete Error"),wxOK|wxICON_ERROR);
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
    if (showSelectButton_)
        OnBSelect(event);
    else
        OnEdit(event);

}

void mmPayeeDialog::OnEdit(wxCommandEvent& event)
{
    wxString oldname = listBox_->GetStringSelection();

    wxString mesg;
    mesg.Printf(_("Enter a new name for %s"),oldname.c_str());
    wxString newName = wxGetTextFromUser(mesg, _("Edit Payee Name"), oldname);
    if (newName != wxGetEmptyString())
    {
        core_->payeeList_.UpdatePayee(m_payee_id, newName);
        core_->bTransactionList_.UpdateAllTransactionsForPayee(core_, m_payee_id);
        editButton->Disable();
        fillControls();

        // Now we need to make sure that the edited name is selected after the dialog is closed
        textCtrl->Clear();
        listBox_->SetStringSelection(newName);
        OnSelChanged(event);
    }
}

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

#include "payeedialog.h"
#include "util.h"
#include "defs.h"
#include <wx/event.h>

#if defined (__WXMAC__)
#define MMEX_ICON_FNAME wxStandardPaths::Get().GetResourcesDir() + wxT("/mmex.ico")
#else
#define MMEX_ICON_FNAME wxT("mmex.ico")
#endif

IMPLEMENT_DYNAMIC_CLASS( mmPayeeDialog, wxDialog )

BEGIN_EVENT_TABLE( mmPayeeDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_OK, mmPayeeDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_ADD, mmPayeeDialog::OnAdd)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_DELETE, mmPayeeDialog::OnDelete)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_SELECT, mmPayeeDialog::OnBSelect)
    EVT_BUTTON(ID_DIALOG_PAYEE_BUTTON_EDIT, mmPayeeDialog::OnEdit)
    EVT_LISTBOX(ID_DIALOG_PAYEE_LISTBOX_PAYEES, mmPayeeDialog::OnSelChanged)
    EVT_COMBOBOX(ID_DIALOG_TRANS_PAYEECOMBO, mmPayeeDialog::OnComboSelected)
    EVT_LISTBOX_DCLICK(ID_DIALOG_PAYEE_LISTBOX_PAYEES, mmPayeeDialog::OnDoubleClicked)
    EVT_CHAR_HOOK(mmPayeeDialog::OnListKeyDown)    
END_EVENT_TABLE()


// EVT_TEXT_ENTER(ID_DIALOG_TRANS_PAYEECOMBO, mmPayeeDialog::OnComboSelected)
//EVT_TEXT(ID_DIALOG_TRANS_PAYEECOMBO, mmPayeeDialog::OnComboSelected)
   
mmPayeeDialog::mmPayeeDialog( )
{
    payeeID_ = -1;
    selectPayees_ = true;
}

void mmPayeeDialog::OnListKeyDown(wxKeyEvent &event)
{
    long keycode = event.GetKeyCode();

    if(keycode == 13)
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
                             bool selectPayees, 
                             wxWindow* parent, 
                             bool showSelectButton, 
                             wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
 
    core_ = core;
    payeeID_ = -1;
    selectPayees_ = selectPayees;
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

    if (!selectPayees_)
    {
        addButton->Disable();
        editButton->Disable();
        deleteButton->Disable();
    }

    if (!showSelectButton_)
    {
        selectButton->Disable();
    }

    wxIcon icon(MMEX_ICON_FNAME, wxBITMAP_TYPE_ICO, 32, 32);
    SetIcon(icon);
    
    fillControls();

    Centre();
    return TRUE;
}

void mmPayeeDialog::fillControls()
{
    core_->payeeList_.sortPayeeList();
    if (selectPayees_)
    {
        int numPayees = (int)core_->payeeList_.payees_.size();
        for (int idx = 0; idx < numPayees; idx++)
        {
            listBox_->Insert(core_->payeeList_.payees_[idx]->payeeName_, idx, 
                new mmPayeeListBoxItem(core_->payeeList_.payees_[idx]->payeeID_));
            payeeComboBox_->Insert(core_->payeeList_.payees_[idx]->payeeName_, idx, new mmPayeeListBoxItem(core_->payeeList_.payees_[idx]->payeeID_));
        }
        payeeComboBox_->Connect(ID_DIALOG_TRANS_PAYEECOMBO, wxEVT_SET_FOCUS, wxFocusEventHandler(mmPayeeDialog::OnSetFocus), NULL, this);        
        payeeComboBox_->Connect(ID_DIALOG_TRANS_PAYEECOMBO, wxEVT_KILL_FOCUS, wxFocusEventHandler(mmPayeeDialog::OnFocus), NULL, this); 
        payeeComboBox_->SetFocus();
        
       
    }
    else
    {
        int numAccounts = (int) core_->accountList_.accounts_.size();
        int insertCount = 0;
        for (int iAdx = 0; iAdx < numAccounts; iAdx++)
        {
            mmCheckingAccount* pCA = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
            if (pCA)
            {
                listBox_->Insert(pCA->accountName_, insertCount, new mmPayeeListBoxItem(pCA->accountID_));
                insertCount++;
            }
        }

        listBox_->SetFocus();
        if (!listBox_->IsEmpty())
        {
            listBox_->SetSelection(0);
        }
    }
}

void mmPayeeDialog::CreateControls()
{    
    mmPayeeDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    if (selectPayees_)
    {
        wxStaticText* itemStaticTextName = new wxStaticText( itemDialog1, wxID_STATIC, 
            _("Find Payee: "), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer2->Add(itemStaticTextName, 0,
            wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

        payeeComboBox_ = new wxAutoComboBox(itemDialog1,
            ID_DIALOG_TRANS_PAYEECOMBO, wxT(""), wxDefaultPosition, wxSize(250, -1), wxCB_READONLY);
        itemBoxSizer2->Add(payeeComboBox_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        payeeComboBox_->SetToolTip(_("Enter the payee name to be added or make edits to an existing payee name"));
    }

     wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);
   
    listBox_ = new wxListBox( itemDialog1, ID_DIALOG_PAYEE_LISTBOX_PAYEES, 
        wxDefaultPosition, wxSize(100, 200), wxArrayString(), wxLB_SINGLE);
    itemBoxSizer3->Add(listBox_, 1, wxGROW|wxALL, 1);

    //wxBoxSizer* itemBoxSizerPayee = new wxBoxSizer(wxHORIZONTAL);
    //itemBoxSizer2->Add(itemBoxSizerPayee, 1, wxGROW|wxALL, 5);

    //wxStaticText* itemStaticTextName = new wxStaticText( itemDialog1, wxID_STATIC, 
      //  _("Payee Name"), wxDefaultPosition, wxDefaultSize, 0 );
    //itemBoxSizerPayee->Add(itemStaticTextName, 0,
      //  wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticTextName = new wxStaticText( itemDialog1, wxID_STATIC, 
            _("Add/Edit Payee: "), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer2->Add(itemStaticTextName, 0,
            wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    textCtrl = new wxTextCtrl( itemDialog1, ID_DIALOG_PAYEE_TEXTCTRL_PAYEENAME, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(textCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textCtrl->SetToolTip(_("Enter the payee name to be added or make edits to an existing payee name"));

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxGROW|wxALL, 5);

    addButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_ADD, 
        _("&Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(addButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    addButton->SetToolTip(_("Add a new payee name"));

    editButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_EDIT, 
        _("&Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(editButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    editButton->SetToolTip(_("Change the name of an existing payee"));
    
    deleteButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_DELETE, 
        _("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(deleteButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    deleteButton->SetToolTip(_("Delete the selected payee. The payee cannot be used by an existing transaction."));

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxALL, 5);

    selectButton = new wxButton( itemDialog1, ID_DIALOG_PAYEE_BUTTON_SELECT, 
        _("&Select"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(selectButton, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    selectButton->SetToolTip(_("Select the currently selected payee as the selected payee for the transaction"));

   // this->Connect(ID_DIALOG_TRANS_PAYEECOMBO, wxEVT_SET_FOCUS, wxFocusEventHandler(mmPayeeDialog::OnFocus)); 
}

void mmPayeeDialog::OnAdd(wxCommandEvent& event)
{
    wxTextCtrl* payeeText = (wxTextCtrl*)FindWindow(ID_DIALOG_PAYEE_TEXTCTRL_PAYEENAME);
    wxString text = payeeText->GetValue();
    if (text.Trim().IsEmpty())
    {
        mmShowErrorMessage(this, _("Type a Payee Name in the Text Box and then press Add."), _("Error"));
        return;
    }
    if (core_->payeeList_.payeeExists(text))
    {
        mmShowErrorMessage(this, _("Payee with same name exists"), _("Error"));
    }
    else
    {
        core_->payeeList_.addPayee(text);
        listBox_->Clear();
        fillControls();

        listBox_->SetStringSelection(text);
        wxString payeeString = listBox_->GetStringSelection();
        payeeID_ = core_->payeeList_.getPayeeID(payeeString);
        textCtrl->SetValue(payeeString);
    }
}
 
void mmPayeeDialog::OnDelete(wxCommandEvent& event)
{
    if (!core_->payeeList_.deletePayee(payeeID_))
    {
        mmShowErrorMessage(this, _("Payee is in use"), _("Error"));
        return;
    }
    payeeComboBox_->SetValue(wxT(""));
    listBox_->Clear();
    fillControls();
}
 
void mmPayeeDialog::OnBSelect(wxCommandEvent& event)
{
    //wxString payeeString = listBox_->GetStringSelection();
    if (payeeID_ != -1)
    {
        EndModal(wxID_OK);
        return;
    }

    int sel = listBox_->GetSelection();
    if (sel != -1)
    {
        mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)listBox_->GetClientObject(sel);
        payeeID_ = lbitem->getPayeeID();
        EndModal(wxID_OK);
    }
#if 0
    else
    {
        wxString payeeString = payeeComboBox_->GetValue();
        if (listBox_->SetStringSelection(payeeString, true))
        {
            wxString payeeString = listBox_->GetStringSelection();
            int selInd = listBox_->GetSelection();
            if (selInd == -1)
                return;
            mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)listBox_->GetClientObject(selInd);
            payeeID_ = lbitem->getPayeeID();
            EndModal(wxID_OK);
        }
    }
#endif
}

void mmPayeeDialog::OnOk(wxCommandEvent& event)
{
    Close(TRUE);
}

void mmPayeeDialog::OnSelChanged(wxCommandEvent& event)
{
    wxString payeeString = listBox_->GetStringSelection();
    int selInd = listBox_->GetSelection();
    if (selInd == -1)
        return;
    mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)listBox_->GetClientObject(selInd);
    payeeID_ = lbitem->getPayeeID();
    textCtrl->SetValue(payeeString);
    //payeeComboBox_->SetValue(payeeString);
}

void mmPayeeDialog::OnComboSelected(wxCommandEvent& event)
{
    wxString payeeString = payeeComboBox_->GetValue();
    int selInd = payeeComboBox_->GetSelection();
    if (selInd == -1)
        return;

    mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)payeeComboBox_->GetClientObject(selInd);
    payeeID_ = lbitem->getPayeeID();
    textCtrl->SetValue(payeeString);
    //payeeComboBox_->SetFocus();
#if 0
    if (listBox_->SetStringSelection(payeeString, true))
    {
        wxString payeeString = listBox_->GetStringSelection();
        int selInd = listBox_->GetSelection();
        if (selInd == -1)
            return;
        mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)listBox_->GetClientObject(selInd);
        payeeID_ = lbitem->getPayeeID();
        textCtrl->SetValue(payeeString);
        payeeComboBox_->SetFocus();
    }
#endif
    //int selInd = payeeComboBox_->GetCurrentSelection();
    //if (selInd == -1)
      //  return;
    //mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)listBox_->GetClientObject(selInd);
    //payeeID_ = lbitem->getPayeeID();
    
    //textCtrl->SetValue(payeeString);
   // payeeComboBox_->SetValue(payeeString);
}


void mmPayeeDialog::OnSetFocus(wxFocusEvent& event)
{
}

void mmPayeeDialog::OnFocus(wxFocusEvent& event)
{
    wxString payeeString = payeeComboBox_->GetValue();
    int selInd = payeeComboBox_->GetSelection();
    if (selInd == -1)
        return;

    mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)payeeComboBox_->GetClientObject(selInd);
    payeeID_ = lbitem->getPayeeID();
    textCtrl->SetValue(payeeString);
#if 0   
    wxString payeeString = payeeComboBox_->GetValue();
    if (listBox_->SetStringSelection(payeeString, true))
    {
        wxString payeeString = listBox_->GetStringSelection();
        int selInd = listBox_->GetSelection();
        if (selInd == -1)
            return;
        mmPayeeListBoxItem* lbitem = (mmPayeeListBoxItem*)listBox_->GetClientObject(selInd);
        payeeID_ = lbitem->getPayeeID();
        textCtrl->SetValue(payeeString);
        //payeeComboBox_->SetFocus();
    }
#endif
}

void mmPayeeDialog::OnDoubleClicked(wxCommandEvent& event)
{
    OnBSelect(event);
}

void mmPayeeDialog::OnEdit(wxCommandEvent& event)
{
    wxString text = textCtrl->GetValue();
    core_->payeeList_.updatePayee(payeeID_, text);
    core_->bTransactionList_.updateAllTransactionsForPayee(core_, payeeID_);
    listBox_->Clear();
    fillControls();
}

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

#ifndef _MM_EX_PAYEEDIALOG_H_
#define _MM_EX_PAYEEDIALOG_H_

#include "wxautocombobox.h"

class mmCoreDB;


class mmPayeeDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS(mmPayeeDialog)
    DECLARE_EVENT_TABLE()

public:
    mmPayeeDialog() : m_payee_id(-1) {}
    mmPayeeDialog(wxWindow* parent, mmCoreDB* core, bool showSelectButton = true);

    int getPayeeId() const { return m_payee_id; }

private:
    int m_payee_id;
    mmCoreDB *m_core;
    bool showSelectButton_;
    wxListBox* listBox_;
    wxButton* addButton;
    wxButton* editButton;
    wxButton* deleteButton;
    wxButton* selectButton;
    wxTextCtrl* textCtrl;
    wxAutoComboBox* payeeComboBox_;

    bool Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style);
    void CreateControls();

    void OnOk(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnBSelect(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnListKeyDown(wxKeyEvent &event);
    
    void fillControls();
    void OnTextCtrlChanged(wxCommandEvent& event);
    void OnSelChanged(wxCommandEvent& event);
    void OnComboSelected(wxCommandEvent& event);
    void OnDoubleClicked(wxCommandEvent& event);
    void OnFocus(wxFocusEvent& event);
    void OnSetFocus(wxFocusEvent& event);
};

#endif // _MM_EX_PAYEEDIALOG_H_


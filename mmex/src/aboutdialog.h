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

#ifndef _MM_EX_ABOUTDIALOG_H_
#define _MM_EX_ABOUTDIALOG_H_

#include <wx/notebook.h>
#include <wx/dialog.h>

class mmAboutDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(mmAboutDialog)
    DECLARE_EVENT_TABLE()

public:
    mmAboutDialog() {}
    mmAboutDialog(wxWindow* parent);

private:
    bool Create(wxWindow* parent, wxWindowID id,
        const wxString& caption, const wxPoint& pos, const wxSize& size, long style);
    void CreateControls();
    void OnVersionHistory(wxCommandEvent& event);
    void OnContributerList(wxCommandEvent& event);

    wxNotebookPage *developers_tab_;
    wxNotebookPage *translators_tab_;
    wxNotebookPage *artwork_tab_;
    wxNotebookPage *sponsors_tab_;
};

#endif // _MM_EX_ABOUTDIALOG_H_


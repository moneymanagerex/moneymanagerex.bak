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

#ifndef _MM_EX_APPSTARTDIALOG_H_
#define _MM_EX_APPSTARTDIALOG_H_

#include <wx/dialog.h>

class wxCheckBox;
class wxSQLite3Database;


class mmAppStartDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(mmAppStartDialog)
    DECLARE_EVENT_TABLE()

public:
    mmAppStartDialog();
    mmAppStartDialog(wxWindow* parent);
   ~mmAppStartDialog();

    int getReturnCode() const { return retCode_; }
    void SetCloseButtonToExit();

private:
    wxCheckBox* itemCheckBox;
    wxButton* itemButtonClose_;
    int retCode_;

    bool Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style);
    void CreateControls();
    void OnButtonAppstartOpenDatabaseClick( wxCommandEvent& event );
    void OnButtonAppstartNewDatabaseClick( wxCommandEvent& event );
    void OnButtonAppstartHelpClick( wxCommandEvent& event );
    void OnButtonAppstartWebsiteClick( wxCommandEvent& event );
    void OnButtonAppstartLastDatabaseClick( wxCommandEvent& event );
    void OnClose( wxCommandEvent& event );
};
#endif // _MM_EX_APPSTARTDIALOG_H_


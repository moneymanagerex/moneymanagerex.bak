/*******************************************************
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

#ifndef _MM_EX_ASSETDIALOG_H_
#define _MM_EX_ASSETDIALOG_H_

#include "defs.h"

class mmData;
class wxDatePickerCtrl;
class mmCategory;
class mmAdvancedPopup;


class mmAssetDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmAssetDialog )
    DECLARE_EVENT_TABLE()

public:
    mmAssetDialog() : db_(), assetID_(-1), edit_() {}
    mmAssetDialog(wxWindow *parent, wxSQLite3Database* db, int assetID, bool edit);

private:
    wxSQLite3Database* db_;
    int assetID_;
    bool edit_;

    wxTextCtrl* assetName_;
    wxDatePickerCtrl* dpc_;
    wxTextCtrl* notes_;
    wxTextCtrl* value_;
    wxTextCtrl* valueChangeRate_;
    
    wxChoice*  assetType_;
    wxChoice*  valueChange_;
	
    wxStaticText* valueChangeRateLabel_;

    bool Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style);
    void CreateControls();
    
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnChangeAppreciationType(wxCommandEvent& event);
    void enableDisableRate(bool en);
    
    void fillControls();
    void dataToControls();

};

#endif

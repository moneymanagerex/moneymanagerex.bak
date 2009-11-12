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

#define SYMBOL_ASSETDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_ASSETDIALOG_TITLE _("New/Edit Asset")
#define SYMBOL_ASSETDIALOG_IDNAME ID_DIALOG_ASSETS
#define SYMBOL_ASSETDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_ASSETDIALOG_POSITION wxDefaultPosition

#include "defs.h"
#include "guiid.h"

class mmData;
class wxDatePickerCtrl;
class mmCategory;
class mmAdvancedPopup;

class mmAssetDialog : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmAssetDialog )
    DECLARE_EVENT_TABLE()

public:
    mmAssetDialog();
    mmAssetDialog(wxSQLite3Database* db, 
        int assetID, bool edit, wxWindow* parent, 
        wxWindowID id = SYMBOL_ASSETDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_ASSETDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_ASSETDIALOG_POSITION, 
        const wxSize& size = SYMBOL_ASSETDIALOG_SIZE, 
        long style = SYMBOL_ASSETDIALOG_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ASSETDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_ASSETDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_ASSETDIALOG_POSITION, 
        const wxSize& size = SYMBOL_ASSETDIALOG_SIZE, 
        long style = SYMBOL_ASSETDIALOG_STYLE );

    void CreateControls();
    
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnChangeAppreciationType(wxCommandEvent& event);
	void enableDisableRate(bool en);
    
    void fillControls();
    void dataToControls();

private:
    wxSQLite3Database* db_;
    int assetID_;

    wxTextCtrl* assetName_;
    wxDatePickerCtrl* dpc_;
    wxTextCtrl* notes_;
    wxTextCtrl* value_;
    wxTextCtrl* valueChangeRate_;
    
    wxChoice*  assetType_;
    wxChoice*  valueChange_;
	
	wxStaticText* valueChangeRateLabel_;
    

    bool edit_;
};

#endif

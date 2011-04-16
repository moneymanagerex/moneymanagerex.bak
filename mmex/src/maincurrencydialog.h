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

#ifndef _MM_EX_MAINCURRENCY_DIALOG_H_
#define _MM_EX_MAINCURRENCY_DIALOG_H_

#include "guiid.h"
#include "defs.h"
#include "dbwrapper.h"
#include "mmcoredb.h"

#define ID_MYDIALOG8 10040
#define SYMBOL_MAINCURRENCYDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MAINCURRENCYDIALOG_TITLE _("Currency Dialog")
#define SYMBOL_MAINCURRENCYDIALOG_IDNAME ID_MYDIALOG8
#define SYMBOL_MAINCURRENCYDIALOG_SIZE wxSize(300, 300)
#define SYMBOL_MAINCURRENCYDIALOG_POSITION wxDefaultPosition
#define ID_LISTBOX 10090
#define ID_PANEL10 10091
#define ID_MAINCURRENCYBUTTON_ADD 10092
#define ID_MAINCURRENCYBUTTON_EDIT 10093
#define ID_MAINCURRENCYBUTTON_SELECT 10094
#define ID_MAINCURRENCYBUTTON_DELETE 10199
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * MyDialog class declaration
 */

class mmCurrencyListBoxItem: public wxClientData
{
public:
    mmCurrencyListBoxItem(int listIndex) 
        : listIndex_(listIndex){}
          
    int getListIndex() { return listIndex_; }

private:
    int listIndex_;
};

class mmMainCurrencyDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( mmMainCurrencyDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    mmMainCurrencyDialog( ) {}

    mmMainCurrencyDialog( mmCoreDB* core, wxWindow* parent, 
        bool bEnableSelect = true, 
        wxWindowID id = SYMBOL_MAINCURRENCYDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_MAINCURRENCYDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_MAINCURRENCYDIALOG_POSITION, 
        const wxSize& size = SYMBOL_MAINCURRENCYDIALOG_SIZE, 
        long style = SYMBOL_MAINCURRENCYDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, 
        wxWindowID id = SYMBOL_MAINCURRENCYDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_MAINCURRENCYDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_MAINCURRENCYDIALOG_POSITION, 
        const wxSize& size = SYMBOL_MAINCURRENCYDIALOG_SIZE, 
        long style = SYMBOL_MAINCURRENCYDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    void OnBtnAdd(wxCommandEvent& event);
    void OnBtnEdit(wxCommandEvent& event);
    void OnBtnSelect(wxCommandEvent& event);
    void OnBtnDelete(wxCommandEvent& event);
    void OnlistBoxSelection(wxCommandEvent& event);
    void OnlistBoxDoubleClicked(wxCommandEvent& event);
    void fillControls();
    
    wxBitmap GetBitmapResource( const wxString& name );

    wxIcon GetIconResource( const wxString& name );
    static bool ShowToolTips();

     int currencyID_;

private:
    wxSQLite3Database* db_;
    mmCoreDB* core_;
    wxListBox* currencyListBox_;
    bool bEnableSelect_;

    wxButton* itemButtonEdit_;
    wxButton* itemButtonDelete_;

    void displayCurrencyDialogErrorMessage(wxString msg);

};

#endif // _MM_EX_MAINCURRENCY_DIALOG_H_


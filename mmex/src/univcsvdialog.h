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

#ifndef _MM_EX_UNIVCSVDIALOG_H_
#define _MM_EX_UNIVCSVDIALOG_H_

#include "guiid.h"
#include "defs.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include <wx/choice.h>

#define ID_MYDIALOG8 10040
#define SYMBOL_UNIVCSVDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_UNIVCSVDIALOG_TITLE _("Universal CSV Importer Dialog")
#define SYMBOL_UNIVCSVDIALOG_IDNAME ID_MYDIALOG8
#define SYMBOL_UNIVCSVDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_UNIVCSVDIALOG_POSITION wxDefaultPosition
#define ID_LISTBOX 10090
#define ID_PANEL10 10091
//#define ID_UNIVCSVBUTTON_ADD 10092
//#define ID_UNIVCSVBUTTON_REMOVE 10093
#define ID_UNIVCSVBUTTON_IMPORT 10094
//#define ID_UNIVCSVBUTTON_LOAD 10095
//#define ID_UNIVCSVBUTTON_SAVE 10096
//#define ID_UNIVCSVBUTTON_MOVEUP 10097
//#define ID_UNIVCSVBUTTON_MOVEDOWN 10098
#define ID_LISTBOX_CANDICATE 10099
#define wxID_SEARCH 10100
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

class mmListBoxItem: public wxClientData
{
public:
    mmListBoxItem(int index, const wxString& name) 
        : index_(index), name_(name)
    {}
    
    int getIndex() const { return index_; }
    wxString getName() const { return name_; }

private:
    int index_;
    wxString name_;
};

class mmUnivCSVImportDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS(mmUnivCSVImportDialog)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    mmUnivCSVImportDialog();
    mmUnivCSVImportDialog(mmCoreDB* core, wxWindow* parent, 
        wxWindowID id = SYMBOL_UNIVCSVDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_UNIVCSVDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_UNIVCSVDIALOG_POSITION, 
        const wxSize& size = SYMBOL_UNIVCSVDIALOG_SIZE, 
        long style = SYMBOL_UNIVCSVDIALOG_STYLE);

    /// Creation
    bool Create(wxWindow* parent, 
        wxWindowID id = SYMBOL_UNIVCSVDIALOG_IDNAME, 
        const wxString& caption = SYMBOL_UNIVCSVDIALOG_TITLE, 
        const wxPoint& pos = SYMBOL_UNIVCSVDIALOG_POSITION, 
        const wxSize& size = SYMBOL_UNIVCSVDIALOG_SIZE, 
        long style = SYMBOL_UNIVCSVDIALOG_STYLE);

    /// Creates the controls and sizers
    void CreateControls();
    void OnAdd(wxCommandEvent& event);
    void OnImport(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
    bool isIndexPresent(int index) const;
    const wxString getCSVFieldName(int index) const;
    void parseToken(int index,wxString& token);
    void OnLoad(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnMoveUp(wxCommandEvent& event);
    void OnMoveDown(wxCommandEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnListBox(wxCommandEvent& event);

    void update_preview();

    wxBitmap GetBitmapResource(const wxString& name);
    wxIcon GetIconResource(const wxString& name);
    static bool ShowToolTips();

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    std::vector<int> csvFieldOrder_;
    wxListBox* csvFiledCandicate_;
    wxListBox* csvListBox_;

    wxButton* m_button_add_;
    wxButton* m_button_remove_;
    wxChoice* m_choice_account_;
    wxListCtrl* m_list_ctrl_; //preview
    wxTextCtrl* m_text_ctrl_;

    std::map<int, wxString> CSVFieldName_;

    wxString dt_;
    wxString payee_;
    wxString type_;
    wxString amount_;
    wxString categ_;
    wxString subcateg_;
    wxString transNum_;
    wxString notes_;
    int payeeID_;
    int categID_; 
    int subCategID_;
    double val_;
    wxDateTime dtdt_;

};

#endif

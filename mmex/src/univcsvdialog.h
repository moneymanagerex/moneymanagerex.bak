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
#include <vector>
#include <map>

#define SYMBOL_UNIVCSVDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_UNIVCSVDIALOG_TITLE _("Universal CSV Dialog")

#define SYMBOL_UNIVCSVDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_UNIVCSVDIALOG_POSITION wxDefaultPosition

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

class mmCoreDB;

class mmUnivCSVDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(mmUnivCSVDialog)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    mmUnivCSVDialog();
    mmUnivCSVDialog(mmCoreDB* core, wxWindow* parent, bool is_importer = true,
        wxWindowID id = SYMBOL_UNIVCSVDIALOG_IDNAME,
        const wxString& caption = SYMBOL_UNIVCSVDIALOG_TITLE,
        const wxPoint& pos = SYMBOL_UNIVCSVDIALOG_POSITION,
        const wxSize& size = SYMBOL_UNIVCSVDIALOG_SIZE,
        long style = SYMBOL_UNIVCSVDIALOG_STYLE);

    bool InportCompletedSuccessfully()  { return importSuccessful_; }
    int ImportedAccountID()             { return fromAccountID_;    }

private:
    mmCoreDB* core_;
    bool is_importer_;
    wxString delimit_;
    wxSQLite3Database* db_;
    std::vector<int> csvFieldOrder_;
    wxListBox* csvFieldCandicate_;
    wxListBox* csvListBox_;

    wxButton* m_button_add_;
    wxButton* m_button_remove_;
    wxChoice* m_choice_account_;
    wxRadioBox* m_radio_box_;
    wxListCtrl* m_list_ctrl_; //preview
    wxTextCtrl* m_text_ctrl_;
    wxTextCtrl* log_field_;
    wxTextCtrl* textDelimiter4;

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
    wxChoice* choiceDateFormat_;

    int fromAccountID_;
    bool importSuccessful_;

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
    void OnExport(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
    bool isIndexPresent(int index) const;
    const wxString getCSVFieldName(int index) const;
    void parseToken(int index,wxString& token);
    void OnLoad(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnMoveUp(wxCommandEvent& event);
    void OnMoveDown(wxCommandEvent& event);
    void OnStandard(wxCommandEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnAccountChange(wxCommandEvent& event);
    void OnListBox(wxCommandEvent& event);
    void OnCheckOrRadioBox(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnFileNameEntered(wxCommandEvent& event);
    void OnFileNameChanged(wxCommandEvent& event);
    void OnDateFormatChanged(wxCommandEvent& event);

    void update_preview();

    wxBitmap GetBitmapResource(const wxString& name);
    wxIcon GetIconResource(const wxString& name);
    static bool ShowToolTips();

};
#endif

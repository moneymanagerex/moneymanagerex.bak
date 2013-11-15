/*******************************************************
Copyright (C) 2006-2012

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

#ifndef QIF_IMPORT_H
#define QIF_IMPORT_H

#define SYMBOL_QIFIMPORT_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_QIFIMPORT_TITLE _("QIF Import")
#define SYMBOL_QIFIMPORT_IDNAME wxID_HIGHEST+1
#define SYMBOL_QIFIMPORT_SIZE wxSize(500, 300)
#define SYMBOL_QIFIMPORT_POSITION wxDefaultPosition

#include "defs.h"
#include <wx/dataview.h>
#include "model/Model_Checking.h"

class wxDatePickerCtrl;

class mmQIFImportDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS( mmQIFImportDialog )
    DECLARE_EVENT_TABLE()

public:
    mmQIFImportDialog() {}

    mmQIFImportDialog(
        wxWindow* parent,
        wxWindowID id = SYMBOL_QIFIMPORT_IDNAME,
        const wxString& caption = SYMBOL_QIFIMPORT_TITLE,
        const wxPoint& pos = SYMBOL_QIFIMPORT_POSITION,
        const wxSize& size = SYMBOL_QIFIMPORT_SIZE,
        long style = SYMBOL_QIFIMPORT_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_QIFIMPORT_IDNAME,
                 const wxString& caption = SYMBOL_QIFIMPORT_TITLE,
                 const wxPoint& pos = SYMBOL_QIFIMPORT_POSITION,
                 const wxSize& size = SYMBOL_QIFIMPORT_SIZE,
                 long style = SYMBOL_QIFIMPORT_STYLE );
    int get_last_imported_acc()
    {
        return last_imported_acc_id_;
    }

private:
    int mmImportQIF(wxTextFile& tFile);

    void CreateControls();
    void fillControls();

    void OnFileSearch(wxCommandEvent& event);
    bool checkQIFFile(wxTextFile& tFile);
    void OnCheckboxClick(wxCommandEvent& /*event*/);
    void OnDateMaskChange(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnQuit(wxCloseEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnOk(wxCommandEvent& /*event*/);

    std::vector< std::pair<Model_Checking::Data*, Model_Splittransaction::Cache> > vQIF_trxs_;
    wxWindow* parent_;
    wxString dateFormat_;
    wxArrayInt accounts_id_;
    wxArrayInt items_index_;
    wxArrayString accounts_name_;
    int fromAccountID_;

    bool isLineOK(const wxString& line);
    wxString getLineData(const wxString& line);
    wxString getFileLine(wxTextInputStream& textFile, int& lineNumber);
    wxString getFinancistoProject(wxString& sSubCateg);
    bool warning_message();
    wxString sFileName_;

    wxDataViewListCtrl* dataListBox_;
    wxButton* button_search_;
    wxTextCtrl* file_name_ctrl_;
    wxTextCtrl* log_field_;
    wxCheckBox* dateFromCheckBox_;
    wxCheckBox* dateToCheckBox_;
    wxDatePickerCtrl* fromDateCtrl_;
    wxDatePickerCtrl* toDateCtrl_;
    wxComboBox* choiceDateFormat_;
    wxChoice* newAccounts_;
    wxButton* btnOK_;

    wxBitmapButton* bbFile_;
    wxBitmapButton* bbFormat_;
    wxBitmapButton* bbAccounts_;
    int last_imported_acc_id_;

    enum EColumn
    {
        COL_ACCOUNT = 0,
        COL_DATE,
        COL_NUMBER,
        COL_PAYEE,
        COL_STATUS,
        COL_CATEGORY,
        COL_VALUE,
        COL_NOTES,
        COL_MAX, // number of columns
    };
    std::map<int, wxString> ColName_;

    enum qifAccountInfoType
    {
        Name = 1, // N
        AccountType = 2, // T
        Description = 3, // D
        CreditLimit = 4, // L
        BalanceDate = 5, // /
        Balance = 6, // $
        EOT = 7, // ^
        UnknownInfo = 8
    };

    enum qifLineType
    {
        AcctType = 1, // !
        Date = 2, // D
        Amount = 3, // T
        Address = 4, // A
        Payee = 5, // P
        EOTLT = 6, // ^
        TransNumber = 7, // N
        Status = 8, // C
        UnknownType = 9,
        Memo = 10, // M
        Category = 11,  // L
        CategorySplit = 12,  // S
        MemoSplit = 13,  // E
        AmountSplit = 14   // '$'
    };

    qifAccountInfoType accountInfoType(const wxString& line)
    {
        if (line.IsEmpty())
            return UnknownInfo;

        wxChar fChar = line.GetChar(0);
        switch (fChar)
        {
        case 'N':
            return Name;
        case 'T':
            return AccountType;
        case 'D':
            return Description;
        case 'L':
            return CreditLimit;
        case '/':
            return BalanceDate;
        case '$':
            return Balance;
        case '^':
            return EOT;
        default:
            return UnknownInfo;
        }
    }

    qifLineType lineType(const wxString& line)
    {
        if (line.IsEmpty())
            return UnknownType;

        wxChar fChar = line.GetChar(0);
        switch (fChar)
        {
        case '!':
            return AcctType;
        case 'D':
            return Date;
        case 'N':
            return TransNumber;
        case 'P':
            return Payee;
        case 'A':
            return Address;
        case 'T':
            return Amount;
        case '^':
            return EOTLT;
        case 'M':
            return Memo;
        case 'L':
            return Category;
        case 'S':
            return CategorySplit;
        case 'E':
            return MemoSplit;
        case '$':
            return AmountSplit;
        case 'C':
            return Status;
        default:
            return UnknownType;
        }
    }

};
#endif // 

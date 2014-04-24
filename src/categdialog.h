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

#ifndef _MM_EX_CATEGDIALOG_H_
#define _MM_EX_CATEGDIALOG_H_


#include "defs.h"
#include "model/Model_Category.h"

class mmTreeItemCateg : public wxTreeItemData
{
public:
    mmTreeItemCateg(const Model_Category::Data& categData, const Model_Subcategory::Data& subcategData)
        : categData_(categData)
        , subcategData_(subcategData)
    {}
    Model_Category::Data* getCategData() { return &categData_; }
    Model_Subcategory::Data* getSubCategData() { return &subcategData_; }

private:
    Model_Category::Data categData_;
    Model_Subcategory::Data subcategData_;
};

class mmCategDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS( mmCategDialog )
    DECLARE_EVENT_TABLE()

public:
    mmCategDialog();
    mmCategDialog(wxWindow* parent
        , bool bEnableSelect = true
        , bool bEnableRelocate = true);

    bool Create(wxWindow* parent
        , wxWindowID id
        , const wxString& caption
        , const wxPoint& pos
        , const wxSize& size
        , long style);

    void setTreeSelection(const wxString& catName, const wxString& subCatName);
    void setTreeSelection(int &category_id, int &subcategory_id);
    int getCategId()
    {
        return categID_;
    }
    int getSubCategId()
    {
        return subcategID_;
    }
    bool getRefreshRequested()
    {
        return refreshRequested_;
    }
    wxString getFullCategName();

private:
    void CreateControls();
    void fillControls();

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnBSelect(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnDoubleClicked(wxTreeEvent& event);
    void showCategDialogDeleteError(wxString deleteCategoryErrMsg, bool category = true);
    void OnCategoryRelocation(wxCommandEvent& /*event*/);
    void OnExpandChbClick(wxCommandEvent& /*event*/);
    void OnShowHiddenChbClick(wxCommandEvent& /*event*/);
    void OnMenuSelected(wxCommandEvent& event);
    void OnItemRightClick(wxTreeEvent& event);
    bool categShowStatus(int categId, int subCategId);

    wxTreeCtrl* m_treeCtrl;
    wxTextCtrl* m_textCtrl;
    wxButton* m_buttonAdd;
    wxButton* m_buttonEdit;
    wxButton* m_buttonSelect;
    wxButton* m_buttonDelete;
    wxBitmapButton* m_buttonRelocate;
    wxCheckBox* m_cbExpand;
    wxCheckBox* m_cbShowAll;
    wxTreeItemId selectedItemId_;
    wxTreeItemId root_;
    wxTreeItemId getTreeItemFor(const wxTreeItemId& itemID, const wxString& itemText);
    bool bEnableSelect_;
    bool bEnableRelocate_;
    int categID_;
    int subcategID_;
    wxColour NormalColor_;
    wxArrayString hidden_categs_;
    bool refreshRequested_;

    enum
    {
        MENU_ITEM_HIDE = wxID_HIGHEST + 1500,
        MENU_ITEM_UNHIDE,
        MENU_ITEM_CLEAR,
        ID_DIALOG_CATEGORY
    };
};

#endif

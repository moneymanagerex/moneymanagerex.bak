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

#include "relocatecategorydialog.h"
#include "categdialog.h"
#include "util.h"
#include "defs.h"
#include "paths.h"

IMPLEMENT_DYNAMIC_CLASS( mmCategDialog, wxDialog )

BEGIN_EVENT_TABLE( mmCategDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmCategDialog::OnBSelect)
    EVT_BUTTON(wxID_CANCEL, mmCategDialog::OnCancel)
    EVT_BUTTON(wxID_ADD, mmCategDialog::OnAdd)
    EVT_BUTTON(wxID_REMOVE, mmCategDialog::OnDelete)
    EVT_BUTTON(wxID_EDIT, mmCategDialog::OnEdit)
    EVT_TREE_SEL_CHANGED(ID_DIALOG_CATEG_TREECTRL_CATS, mmCategDialog::OnSelChanged)
    EVT_TREE_ITEM_ACTIVATED(ID_DIALOG_CATEG_TREECTRL_CATS,  mmCategDialog::OnDoubleClicked)
END_EVENT_TABLE()

mmCategDialog::mmCategDialog( )
{
    // Initialize fields in constructor
    categID_ = -1;
    subcategID_ = -1;
    selectedItemId_ = 0;
}

mmCategDialog::mmCategDialog(mmCoreDB* core,
                             wxWindow* parent, bool bEnableSelect,
                             wxWindowID id, const wxString& caption,
                             const wxPoint& pos, const wxSize& size, long style )
{
    // Initialize fields in constructor
    core_ = core;
    categID_ = -1;
    subcategID_ = -1;
    selectedItemId_ = 0;
    bEnableSelect_ = bEnableSelect;
    Create(parent, id, caption, pos, size, style);
}

bool mmCategDialog::Create( wxWindow* parent, wxWindowID id,
                           const wxString& caption, const wxPoint& pos,
                           const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    fillControls();

    Centre();
    return TRUE;
}

void mmCategDialog::fillControls()
{
    treeCtrl_->DeleteAllItems();
    root_ = treeCtrl_->AddRoot(_("Categories"));
    selectedItemId_ = root_;
    treeCtrl_->SetItemBold(root_, true);
    treeCtrl_->SetFocus ();

    if (!core_) return;

    std::pair<mmCategoryList::const_iterator, mmCategoryList::const_iterator> range = core_->categoryList_.Range();
    for (mmCategoryList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const boost::shared_ptr<mmCategory> category = *it;
        wxTreeItemId maincat = treeCtrl_->AppendItem(root_, category->categName_);
        treeCtrl_->SetItemData(maincat, new mmTreeItemCateg(category->categID_, -1));

        for (std::vector<boost::shared_ptr<mmCategory> >::const_iterator cit =  category->children_.begin();
                cit != category->children_.end();
                ++ cit)
        {
            const boost::shared_ptr<mmCategory> sub_category = *cit;
            wxTreeItemId subcat = treeCtrl_->AppendItem(maincat, sub_category->categName_);
            treeCtrl_->SetItemData(subcat, new mmTreeItemCateg(category->categID_, sub_category->categID_));

            if (categID_ == category->categID_ && subcategID_ == sub_category->categID_)
                selectedItemId_ = subcat;
        }

        treeCtrl_->SortChildren(maincat);
    }
    treeCtrl_->Expand(root_);

    bool result = core_->iniSettings_->GetBoolSetting(wxT("EXPAND_CATEGS_TREE"), false);
    if (result) treeCtrl_->ExpandAll();
    itemCheckBox_->SetValue(result);

    treeCtrl_->SortChildren(root_);
    treeCtrl_->SelectItem(selectedItemId_);

    textCtrl_->SetValue(wxT (""));
    selectButton_->Disable();
    deleteButton_->Disable();
    editButton_->Disable();
    addButton_->Enable();
}

void mmCategDialog::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 1);
    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer33);

    wxBitmapButton* bCateg_relocate = new wxBitmapButton(this,
        wxID_STATIC, wxBitmap(relocate_categories_xpm));
    bCateg_relocate->Connect(wxID_STATIC, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(mmCategDialog::OnCategoryRelocation), NULL, this);
    bCateg_relocate->SetToolTip(_("Reassign all categories to another category"));

    itemCheckBox_ = new wxCheckBox( this, wxID_STATIC, _("Expand"), wxDefaultPosition,
        wxDefaultSize, wxCHK_2STATE );
    bool showBeginApp = false; //pIniSettings_->GetBoolSetting(wxT("SHOWBEGINAPP"), true);
    itemCheckBox_->SetValue(showBeginApp);
    itemCheckBox_->Connect(wxID_STATIC, wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(mmCategDialog::OnChbClick), NULL, this);

    itemBoxSizer33->Add(bCateg_relocate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemBoxSizer33->AddSpacer(10);
    itemBoxSizer33->Add(itemCheckBox_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

#if defined (__WXGTK__) || defined (__WXMAC__)
    treeCtrl_ = new wxTreeCtrl( this, ID_DIALOG_CATEG_TREECTRL_CATS,
        wxDefaultPosition, wxSize(200, 380));
#else
    treeCtrl_ = new wxTreeCtrl( this, ID_DIALOG_CATEG_TREECTRL_CATS,
        wxDefaultPosition, wxSize(200, 380), wxTR_SINGLE
        | wxTR_HAS_BUTTONS
        | wxTR_ROW_LINES );
#endif
    itemBoxSizer3->Add(treeCtrl_, 1, wxGROW|wxALL, 1);

    textCtrl_ = new wxTextCtrl( this, wxID_STATIC, wxT(""));
    itemBoxSizer2->Add(textCtrl_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 1);
    textCtrl_->SetToolTip(_("Enter the name of the category to add or edit here"));

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxGROW|wxALL, 5);

    addButton_ = new wxButton( this, wxID_ADD);
    itemBoxSizer5->Add(addButton_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
     addButton_->SetToolTip(_("Add a new category"));

    editButton_ = new wxButton( this, wxID_EDIT);
    itemBoxSizer5->Add(editButton_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    editButton_->SetToolTip(_("Edit the name of an existing category"));

    deleteButton_ = new wxButton( this, wxID_REMOVE);
    itemBoxSizer5->Add(deleteButton_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    deleteButton_->SetToolTip(_("Delete an existing category. The category cannot be used by existing transactions."));

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxALL, 5);

    selectButton_ = new wxButton( this, wxID_OK,
        _("&Select"));
    itemBoxSizer9->Add(selectButton_, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    selectButton_->SetToolTip(_("Select the currently selected category as the selected category for the transaction"));

    //Some interfaces has no any close buttons, it may confuse user. Cancel button added
    wxButton* itemCancelButton = new wxButton( this, wxID_CANCEL);
    itemBoxSizer9->Add(itemCancelButton);
}

void mmCategDialog::OnAdd(wxCommandEvent& /*event*/)
{
    if (mmIniOptions::instance().disableCategoryModify_)
        return;

    wxString text = wxGetTextFromUser(_("Enter the name for the new category:")
        , _("Add Category"), textCtrl_->GetValue());
    if (text.IsEmpty())
        return;

    if (selectedItemId_ == root_)
    {
        if (core_->categoryList_.CategoryExists(text))
        {
            wxString errMsg = _("Category with same name exists");
            errMsg << wxT("\n\n") << _("Tip: If category added now, check bottom of list.\nCategory will be in sorted order next time dialog appears");
            wxMessageBox(errMsg, _("Organise Categories: Adding Error"),wxOK|wxICON_ERROR);
            return;
        }
        int categID = core_->categoryList_.AddCategory(text);

        wxTreeItemId tid = treeCtrl_->AppendItem(selectedItemId_, text);
        treeCtrl_->SetItemData(tid, new mmTreeItemCateg( categID, -1));
        treeCtrl_->Expand(selectedItemId_);

        return;
    }

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    int categID = iData->getCategID();
    int subcategID = iData->getSubCategID();
    if (subcategID == -1) // not subcateg
    {
        if (core_->categoryList_.GetSubCategoryID(categID, text) != -1)
        {
            wxMessageBox(_("Sub Category with same name exists"),
                _("Organise Categories: Adding Error"),wxOK|wxICON_ERROR);
            return;
        }
        int subcategID = core_->categoryList_.AddSubCategory(categID, text);

        wxTreeItemId tid = treeCtrl_->AppendItem(selectedItemId_, text);
        treeCtrl_->SetItemData(tid, new mmTreeItemCateg(categID, subcategID));
        treeCtrl_->Expand(selectedItemId_);
        return;
    }

    wxMessageBox(_("Invalid Parent Category. Choose Root or Main Category node."),
        _("Organise Categories: Adding Error"),wxOK|wxICON_ERROR);

 }

void mmCategDialog::showCategDialogDeleteError(wxString deleteCategoryErrMsg, bool category)
{

    if (category)
        deleteCategoryErrMsg << wxT("\n\n") << _("Tip: Change all transactions using this Category to\nanother Category using the relocate command:");
    else
        deleteCategoryErrMsg << wxT("\n\n") << _("Tip: Change all transactions using this Sub-Category to\nanother Sub-Category using the relocate command:");

    deleteCategoryErrMsg << wxT("\n\n") << _("Tools -> Relocation of -> Categories");

    wxMessageBox(deleteCategoryErrMsg,_("Organise Categories: Delete Error"), wxOK|wxICON_ERROR);
}

void mmCategDialog::OnDelete(wxCommandEvent& /*event*/)
{
    if (mmIniOptions::instance().disableCategoryModify_)
        return;

    if (selectedItemId_ == root_ || !selectedItemId_ )
        return;

    mmTreeItemCateg* iData
        = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    int categID = iData->getCategID();
    int subcategID = iData->getSubCategID();

    if (subcategID == -1)
    {
        if (!core_->categoryList_.DeleteCategory(categID))
        {
            showCategDialogDeleteError(_("Category in use."));
            return;
        }
    }
    else
    {
        if (!core_->categoryList_.DeleteSubCategory(categID, subcategID))
        {
            showCategDialogDeleteError(_("Sub-Category in use."), false);
            return;
        }
    }

    treeCtrl_->Delete(selectedItemId_);
}

void mmCategDialog::OnBSelect(wxCommandEvent& /*event*/)
{
    if (selectedItemId_ == root_ || !selectedItemId_ )
    {
        return;
    }

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>
        (treeCtrl_->GetItemData(selectedItemId_));
    categID_ = iData->getCategID();
    subcategID_ = iData->getSubCategID();
    EndModal(wxID_OK);
}

void mmCategDialog::OnDoubleClicked(wxTreeEvent& /*event*/)
{
    if (selectedItemId_ != root_ && selectedItemId_ && bEnableSelect_)
    {
        mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>
            (treeCtrl_->GetItemData(selectedItemId_));
        categID_ = iData->getCategID();
        subcategID_ = iData->getSubCategID();
        EndModal(wxID_OK);
    }
    else
        return;
}

void mmCategDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void mmCategDialog::OnSelChanged(wxTreeEvent& event)
{
    selectedItemId_ = event.GetItem();
    if (!selectedItemId_)
        return;

    textCtrl_->SetValue(treeCtrl_->GetItemText(selectedItemId_));

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>
        (treeCtrl_->GetItemData(selectedItemId_));
    int categID = -1;
    int subcategID = -1;
    if (iData)
    {
        categID    = iData->getCategID();
        subcategID = iData->getSubCategID();
    }

    if (selectedItemId_ == root_ || !selectedItemId_)
    {
        textCtrl_->SetValue(wxT(""));
        selectButton_->Disable();
        deleteButton_->Disable();
        editButton_->Disable();
        addButton_->Enable();
    }
    else
    {
        if(bEnableSelect_ == true) {
            selectButton_->Enable();
        }
        deleteButton_->Enable();
        editButton_->Enable();
        if (subcategID != -1)
        {
            // this is a sub categ, cannot add
            addButton_->Disable();
        }
        else
            addButton_->Enable();
    }
}

void mmCategDialog::OnEdit(wxCommandEvent& /*event*/)
{
    if (mmIniOptions::instance().disableCategoryModify_)
        return;

    if (selectedItemId_ == root_ || !selectedItemId_ )
        return;

    wxString old_name = treeCtrl_->GetItemText(selectedItemId_);
    wxString msg = wxString::Format(_("Enter a new name for %s"), old_name.c_str());
    wxString text = wxGetTextFromUser(msg
        , _("Edit Category"), textCtrl_->GetValue());
    if (text.IsEmpty())
        return;
    textCtrl_->SetValue(text);

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>
        (treeCtrl_->GetItemData(selectedItemId_));
    int categID = iData->getCategID();
    int subcategID = iData->getSubCategID();

    if (!core_->categoryList_.UpdateCategory(categID, subcategID, text))
    {
        wxString errMsg = _("Update Failed");
        errMsg << wxT("          ");  // added to adjust dialog size
        wxMessageBox(errMsg, _("Organise Categories"),wxOK|wxICON_ERROR);
        return;
    }

    core_->bTransactionList_.UpdateAllTransactionsForCategory(core_, categID, subcategID);

    treeCtrl_->SetItemText(selectedItemId_, text);
}

wxTreeItemId mmCategDialog::getTreeItemFor(wxTreeItemId itemID, wxString itemText)
{
    wxTreeItemIdValue treeDummyValue;

    bool searching = true;
    wxTreeItemId catID = treeCtrl_->GetFirstChild(itemID, treeDummyValue);
    while (catID.IsOk() && searching)
    {
        if ( itemText == treeCtrl_->GetItemText(catID))
            searching = false;
        else
            catID = treeCtrl_->GetNextChild(itemID, treeDummyValue);
    }
    return catID;
}

void mmCategDialog::setTreeSelection(wxString catName, wxString subCatName)
{
    if ( !catName.IsEmpty() )
    {
        wxTreeItemId catID = getTreeItemFor(treeCtrl_->GetRootItem(), catName);
        if (catID.IsOk() && treeCtrl_->ItemHasChildren(catID))
        {
            if (subCatName.IsEmpty()) {
                treeCtrl_->SelectItem(catID);
            } else {
                treeCtrl_->ExpandAllChildren(catID);
                wxTreeItemId subCatID = getTreeItemFor(catID, subCatName);
                treeCtrl_->SelectItem(subCatID);
            }
        } else {
            treeCtrl_->SelectItem(catID);
        }
    }
}

void mmCategDialog::OnCategoryRelocation(wxCommandEvent& /*event*/)
{
    relocateCategoryDialog* dlg = new relocateCategoryDialog(core_, this);
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString msgStr;
        msgStr << _("Category Relocation Completed.") << wxT("\n\n")
               << wxString::Format( _("Records have been updated in the database: %s"),
                    dlg->updatedCategoriesCount().c_str())
               << wxT("\n\n")
               << _("MMEX must be shutdown and restarted for all the changes to be seen.");
        wxMessageBox(msgStr,_("Category Relocation Result"));
        mmOptions::instance().databaseUpdated_ = true;
    }
}

void mmCategDialog::OnChbClick(wxCommandEvent& /*event*/)
{
    if (itemCheckBox_->IsChecked())
        treeCtrl_->ExpandAll();
    else
    {
        treeCtrl_->CollapseAll();
        treeCtrl_->Expand(root_);
        treeCtrl_->SelectItem(selectedItemId_);
    }
    core_->iniSettings_->SetBoolSetting(wxT("EXPAND_CATEGS_TREE"), itemCheckBox_->IsChecked());
}

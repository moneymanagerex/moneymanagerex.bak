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

#include "categdialog.h"
#include "mmcoredb.h"
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
    root_ = treeCtrl_->AddRoot(_("Categories"));
    selectedItemId_ = root_;
    treeCtrl_->SetItemBold(root_, true);
    treeCtrl_->SetFocus ();

    if (!core_) return;

    std::pair<mmCategoryList::const_iterator, mmCategoryList::const_iterator> range = core_->rangeCategory();
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
        //Do not expand categories is nice
        //TODO: May be users will want parameter for this
        //treeCtrl_->Expand(maincat);
    }

    treeCtrl_->Expand(root_);
    treeCtrl_->SortChildren(root_);

    wxTextCtrl* textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_CATEG_TEXTCTRL_CATNAME);
    wxButton* addButton = (wxButton*)FindWindow(wxID_ADD);
    wxButton* editButton = (wxButton*)FindWindow(wxID_EDIT);
    wxButton* selectButton = (wxButton*)FindWindow(wxID_OK);
    wxButton* deleteButton = (wxButton*)FindWindow(wxID_REMOVE);

    treeCtrl_->SelectItem(selectedItemId_);

    textCtrl->SetValue(wxT (""));
    selectButton->Disable();
    deleteButton->Disable();
    editButton->Disable();
    addButton->Enable();
}

void mmCategDialog::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

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

    wxTextCtrl* itemTextCtrl6 = new wxTextCtrl( this,
        ID_DIALOG_CATEG_TEXTCTRL_CATNAME, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemTextCtrl6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemTextCtrl6->SetToolTip(_("Enter the name of the category to add or edit here"));

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxGROW|wxALL, 5);

    wxButton* itemButton7 = new wxButton( this, wxID_ADD,
        _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
     itemButton7->SetToolTip(_("Add a new category"));

    wxButton* itemButton71 = new wxButton( this, wxID_EDIT,
        _("&Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton71, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton71->SetToolTip(_("Edit the name of an existing category"));

    wxButton* itemButton8 = new wxButton( this, wxID_REMOVE,
        _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton8->SetToolTip(_("Delete an existing category. The category cannot be used by existing transactions."));

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxALL, 5);

    wxButton* itemButton11 = new wxButton( this, wxID_OK,
        _("&Select"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton11, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton11->SetToolTip(_("Select the currently selected category as the selected category for the transaction"));

    //Some interfaces has no any close buttons, it may confuse user. Cancel button added
    wxButton* itemCancelButton = new wxButton( this, wxID_CANCEL, _("&Cancel"));
    itemBoxSizer9->Add(itemCancelButton);
}

void mmCategDialog::OnAdd(wxCommandEvent& /*event*/)
{
    if (mmIniOptions::instance().disableCategoryModify_)
        return;

    wxTextCtrl* textCtrl;
    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_CATEG_TEXTCTRL_CATNAME);
    wxString text = textCtrl->GetValue();
    if (text == wxT(""))
    {
        wxString errMsg = _("Category cannot be empty");
        errMsg << wxT("          ");  // added to adjust dialog size
        wxMessageBox(errMsg, _("Organise Categories: Adding Error"),wxOK|wxICON_ERROR);
        return;
    }

    if (selectedItemId_ == root_)
    {
        if (core_->categoryExists(text))
        {
            wxString errMsg = _("Category with same name exists");
            errMsg << wxT("\n\n") << _("Tip: If category added now, check bottom of list.\nCategory will be in sorted order next time dialog appears");
            wxMessageBox(errMsg, _("Organise Categories: Adding Error"),wxOK|wxICON_ERROR);
            return;
        }
        int categID = core_->addCategory(text);

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
        if (core_->getSubCategoryID(categID, text) != -1)
        {
            wxMessageBox(_("Sub Category with same name exists"),
                _("Organise Categories: Adding Error"),wxOK|wxICON_ERROR);
            return;
        }
        int subcategID = core_->addSubCategory(categID, text);

        wxTreeItemId tid = treeCtrl_->AppendItem(selectedItemId_, text);
        treeCtrl_->SetItemData(tid, new mmTreeItemCateg(categID, subcategID));
        treeCtrl_->Expand(selectedItemId_);
        return;
    }

    wxMessageBox(_("Invalid Parent Category. Choose Root or Main Category node."),
        _("Organise Categories: Adding Error"),wxOK|wxICON_ERROR);
 }

void mmCategDialog::showCategDialogDeleteError(int error_code, bool category)
{
    if (error_code == wxID_OK) return;

    wxString error_message;
    switch (error_code)
    {
    case  ERR_CAT_USED1 : error_message = _("Category is used in one of the accounts."); break;
    case  ERR_CAT_USED2 : error_message = _("Category is used as split in one of the accounts."); break;
    case  ERR_CAT_USED3: error_message = _("Category is used in one of the repeating transactions."); break;
    case  ERR_CAT_USED4 : error_message = _("Category is used as split in one of the repeating transactions."); break;
    case  ERR_CAT_USED5 : error_message = _("Error"); break;
    case  ERR_SUBCAT_USED1 : error_message = _("Sub-Category is used in one of the accounts."); break;
    case  ERR_SUBCAT_USED2 : error_message = _("Sub-Category is used as split in one of the accounts."); break;
    case  ERR_SUBCAT_USED3 : error_message = _("Sub-Category is used in one of the repeating transactions."); break;
    case  ERR_SUBCAT_USED4 : error_message = _("Sub-Category is used as split in one of the repeating transactions."); break;

    default:
    wxASSERT(false);
    }

    error_message << wxT("\n\n");
    if (category)
        error_message << _("Tip: Change all transactions using this category  to\nanother category using the relocate command:");
    else
        error_message << _("Tip: Change all transactions using this subcategory  to\nanother subcategory using the relocate command:");

    error_message << wxT("\n\n") << _("Tools -> Relocation of -> Categories");

    wxMessageBox(error_message, wxString() << _("Organise Categories: Delete Error"), wxOK|wxICON_WARNING);
}

void mmCategDialog::OnDelete(wxCommandEvent& /*event*/)
{
    if (mmIniOptions::instance().disableCategoryModify_)
        return;

    if (selectedItemId_ == root_ || !selectedItemId_ )
    {
        return;
    }

    mmTreeItemCateg* iData
        = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    int categID = iData->getCategID();
    int subcategID = iData->getSubCategID();

    if (subcategID == -1)
    {
        int error_code = core_->deleteCategory(categID);
        if (error_code != wxID_OK) {
            showCategDialogDeleteError(error_code);
            return;
        }
    }
    else
    {
        int error_code = core_->deleteSubCategory(categID, subcategID);
        if (error_code != wxID_OK) {
            showCategDialogDeleteError(error_code, false);
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

    wxTextCtrl* textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_CATEG_TEXTCTRL_CATNAME);
    textCtrl->SetValue(treeCtrl_->GetItemText(selectedItemId_));

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>
        (treeCtrl_->GetItemData(selectedItemId_));
    int categID = -1;
    int subcategID = -1;
    if (iData)
    {
        categID    = iData->getCategID();
        subcategID = iData->getSubCategID();
    }

    wxButton* addButton = (wxButton*)FindWindow(wxID_ADD);
    wxButton* editButton = (wxButton*)FindWindow(wxID_EDIT);
    wxButton* selectButton = (wxButton*)FindWindow(wxID_OK);
    wxButton* deleteButton = (wxButton*)FindWindow(wxID_REMOVE);
    if (selectedItemId_ == root_ || !selectedItemId_)
    {
        textCtrl->SetValue(wxT(""));
        selectButton->Disable();
        deleteButton->Disable();
        editButton->Disable();
        addButton->Enable();
    }
    else
    {
        if(bEnableSelect_ == true) {
            selectButton->Enable();
        }
        deleteButton->Enable();
        editButton->Enable();
        if (subcategID != -1)
        {
            // this is a sub categ, cannot add
            addButton->Disable();
        }
        else
            addButton->Enable();
    }
}

void mmCategDialog::OnEdit(wxCommandEvent& /*event*/)
{
    if (mmIniOptions::instance().disableCategoryModify_)
        return;

    wxTextCtrl* textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_CATEG_TEXTCTRL_CATNAME);
    wxString text = textCtrl->GetValue();

    if (selectedItemId_ == root_ || !selectedItemId_ )
    {
        return;
    }

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>
        (treeCtrl_->GetItemData(selectedItemId_));
    int categID = iData->getCategID();
    int subcategID = iData->getSubCategID();

    if (!core_->updateCategory(categID, subcategID, text))
    {
        wxString errMsg = _("Update Failed");
        errMsg << wxT("          ");  // added to adjust dialog size
        wxMessageBox(errMsg, _("Organise Categories"),wxOK|wxICON_ERROR);
        return;
    }

    core_->bTransactionList_.updateAllTransactionsForCategory(core_, categID, subcategID);

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

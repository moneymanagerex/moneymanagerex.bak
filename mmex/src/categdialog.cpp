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
 /*******************************************************/

#include "categdialog.h"
#include "util.h"

IMPLEMENT_DYNAMIC_CLASS( mmCategDialog, wxDialog )

BEGIN_EVENT_TABLE( mmCategDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_CATEG_BUTTON_OK, mmCategDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_CATEG_BUTTON_ADD, mmCategDialog::OnAdd)
    EVT_BUTTON(ID_DIALOG_CATEG_BUTTON_DELETE, mmCategDialog::OnDelete)
    EVT_BUTTON(ID_DIALOG_CATEG_BUTTON_SELECT, mmCategDialog::OnBSelect)
    EVT_BUTTON(ID_DIALOG_CATEG_BUTTON_EDIT, mmCategDialog::OnEdit)
    EVT_TREE_SEL_CHANGED(ID_DIALOG_CATEG_TREECTRL_CATS, mmCategDialog::OnSelChanged)
    EVT_TREE_ITEM_ACTIVATED(ID_DIALOG_CATEG_TREECTRL_CATS,  mmCategDialog::OnDoubleClicked)
END_EVENT_TABLE()

mmCategDialog::mmCategDialog( )
{
    categID_ = -1;
    subcategID_ = -1;
    db_ = 0;
}

mmCategDialog::mmCategDialog( wxSQLite3Database* db, wxWindow* parent, 
                             wxWindowID id, const wxString& caption, 
                             const wxPoint& pos, const wxSize& size, long style )
{
    categID_ = -1;
    subcategID_ = -1;
    db_ = db;
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

    wxIcon icon(mainicon_xpm);
    SetIcon(icon);
    
    fillControls();

    Centre();
    return TRUE;
}

void mmCategDialog::fillControls()
{
    root_ = treeCtrl_->AddRoot(wxT("Categories"));
    treeCtrl_->SetItemBold(root_, true);

    if (!db_)
       return;

    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CATEGORY_V1 order by CATEGNAME;");
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
       int categID          = q1.GetInt(wxT("CATEGID"));
       wxString categString = q1.GetString(wxT("CATEGNAME"));
       wxTreeItemId maincat = treeCtrl_->AppendItem(root_, categString);
       treeCtrl_->SetItemData(maincat, new mmTreeItemCateg(categID, -1));  

       wxSQLite3StatementBuffer bufSQL1;
       bufSQL1.Format("select * from SUBCATEGORY_V1 where CATEGID=%d;", categID);
       wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL1); 
       while(q2.NextRow())
       {
           int subcategID          = q2.GetInt(wxT("SUBCATEGID"));
           wxString subcategString    = q2.GetString(wxT("SUBCATEGNAME"));
           wxTreeItemId subcat = treeCtrl_->AppendItem(maincat, subcategString);
           treeCtrl_->SetItemData(subcat, new mmTreeItemCateg(categID, subcategID)); 
       }
       q2.Finalize();
       treeCtrl_->SortChildren(maincat);
       treeCtrl_->Expand(maincat);
    }
    q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
    treeCtrl_->Expand(root_);
    treeCtrl_->SortChildren(root_);
}

void mmCategDialog::CreateControls()
{    
    mmCategDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    treeCtrl_ = new wxTreeCtrl( itemDialog1, ID_DIALOG_CATEG_TREECTRL_CATS, 
        wxDefaultPosition, wxSize(100, 200), wxTR_SINGLE | wxTR_HAS_BUTTONS | wxTR_ROW_LINES );
    itemBoxSizer3->Add(treeCtrl_, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxGROW|wxALL, 5);

    wxTextCtrl* itemTextCtrl6 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CATEG_TEXTCTRL_CATNAME, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemTextCtrl6, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 1);
     itemTextCtrl6->SetToolTip(_("Enter the name of the category to add or edit here"));

    wxButton* itemButton7 = new wxButton( itemDialog1, ID_DIALOG_CATEG_BUTTON_ADD, 
        _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
     itemButton7->SetToolTip(_("Add a new category"));

    wxButton* itemButton71 = new wxButton( itemDialog1, ID_DIALOG_CATEG_BUTTON_EDIT, 
        _("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton71, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton71->SetToolTip(_("Edit the name of an existing category"));
    
    wxButton* itemButton8 = new wxButton( itemDialog1, ID_DIALOG_CATEG_BUTTON_DELETE, 
        _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton8->SetToolTip(_("Delete an existing category. The category cannot be used by existing transactions."));

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxALL, 5);

    wxButton* itemButton11 = new wxButton( itemDialog1, ID_DIALOG_CATEG_BUTTON_SELECT, 
        _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton11, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton11->SetToolTip(_("Select the currently selected category as the selected category for the transaction"));
}

void mmCategDialog::OnAdd(wxCommandEvent& event)
{
    wxTextCtrl* textCtrl;  
    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_CATEG_TEXTCTRL_CATNAME);
    wxString text = textCtrl->GetValue();

    if (selectedItemId_ == root_)
    {
        if (mmDBWrapper::getCategoryID(db_, text) != -1)
        {   
            mmShowErrorMessage(this, _("Category with same name exists"), _("Error"));
            return;
        }
        mmDBWrapper::addCategory(db_, text);
        int categID = mmDBWrapper::getCategoryID(db_, text);

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
        if (mmDBWrapper::getSubCategoryID(db_, categID, text) != -1)
        {   
            mmShowErrorMessage(this, _("Sub Category with same name exists"), _("Error"));
            return;
        }
        mmDBWrapper::addSubCategory(db_, categID, text);
        int subcategID = mmDBWrapper::getSubCategoryID(db_, categID, text);

        wxTreeItemId tid = treeCtrl_->AppendItem(selectedItemId_, text);
        treeCtrl_->SetItemData(tid, new mmTreeItemCateg(categID, subcategID));
        treeCtrl_->Expand(selectedItemId_);
        return;
    }
    
    mmShowErrorMessage(this, _("Invalid Parent Category. Choose Root or Main Category node."),
                             _("Error adding Category"));
 }
 
void mmCategDialog::OnDelete(wxCommandEvent& event)
{
    if (selectedItemId_ == root_)
    {
        return;
    }

    mmTreeItemCateg* iData 
        = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    int categID = iData->getCategID();
    int subcategID = iData->getSubCategID();

    if (subcategID == -1)
    {
        if (!mmDBWrapper::deleteCategoryWithConstraints(db_, categID))
        {
            mmShowErrorMessage(this, _("Category is in use"), _("Error"));
            return;
        }
    }
    else
    {
        if (!mmDBWrapper::deleteSubCategoryWithConstraints(db_, categID, subcategID))
        {
            mmShowErrorMessage(this, _("Sub-Category is in use"), _("Error"));
            return;
        }
    }

    treeCtrl_->Delete(selectedItemId_);
}
 
void mmCategDialog::OnBSelect(wxCommandEvent& event)
{
    if (selectedItemId_ == root_)
    {
        return;
    }

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    categID_ = iData->getCategID();
    subcategID_ = iData->getSubCategID();
    EndModal(wxID_OK);
}

void mmCategDialog::OnDoubleClicked(wxTreeEvent& event)
{
    if (selectedItemId_ == root_)
    {
        return;
    }

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    categID_ = iData->getCategID();
    subcategID_ = iData->getSubCategID();
    EndModal(wxID_OK);
}

void mmCategDialog::OnOk(wxCommandEvent& event)
{
    Close(TRUE);
}

void mmCategDialog::OnSelChanged(wxTreeEvent& event)
{
    selectedItemId_ = event.GetItem();
    if (!selectedItemId_)
        return;

    wxTextCtrl* textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_CATEG_TEXTCTRL_CATNAME);
    textCtrl->SetValue(treeCtrl_->GetItemText(selectedItemId_));

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    int categID = -1;
    int subcategID = -1;
    if (iData)
    {
        categID    = iData->getCategID();
        subcategID = iData->getSubCategID();
    }

    wxButton* addButton = (wxButton*)FindWindow(ID_DIALOG_CATEG_BUTTON_ADD);
    wxButton* editButton = (wxButton*)FindWindow(ID_DIALOG_CATEG_BUTTON_EDIT);
    wxButton* selectButton = (wxButton*)FindWindow(ID_DIALOG_CATEG_BUTTON_SELECT);
    wxButton* deleteButton = (wxButton*)FindWindow(ID_DIALOG_CATEG_BUTTON_DELETE);
    if (selectedItemId_ == root_)
    {
        textCtrl->SetValue(wxT(""));
        selectButton->Disable();
        deleteButton->Disable();
        editButton->Disable();
        addButton->Enable();
    }
    else
    {
        selectButton->Enable();
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

void mmCategDialog::OnEdit(wxCommandEvent& event)
{
    wxTextCtrl* textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_CATEG_TEXTCTRL_CATNAME);
    wxString text = textCtrl->GetValue();

    if (selectedItemId_ == root_)
    {
        return;
    }

    mmTreeItemCateg* iData = dynamic_cast<mmTreeItemCateg*>(treeCtrl_->GetItemData(selectedItemId_));
    int categID = iData->getCategID();
    int subcategID = iData->getSubCategID();
    
    mmDBWrapper::updateCategory(db_, categID, subcategID, text);
    
    treeCtrl_->SetItemText(selectedItemId_, text);
}

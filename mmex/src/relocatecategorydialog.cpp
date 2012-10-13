/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011 Stefano Giorgio

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
#include "paths.h"
#include "categdialog.h"
#include "wx/statline.h"

IMPLEMENT_DYNAMIC_CLASS( relocateCategoryDialog, wxDialog )

BEGIN_EVENT_TABLE( relocateCategoryDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_CATEG_SELECT_BUTTON_SOURCE, relocateCategoryDialog::OnSelectSource)
    EVT_BUTTON(ID_DIALOG_CATEG_SELECT_BUTTON_DEST, relocateCategoryDialog::OnSelectDest)
    EVT_BUTTON(wxID_OK, relocateCategoryDialog::OnOk)
END_EVENT_TABLE()

relocateCategoryDialog::relocateCategoryDialog( )
{
    core_           =  0;

    sourceCatID_    = -1;
    sourceSubCatID_ = -1;

    destCatID_      = -1;
    destSubCatID_   = -1;
    changedCats_    = 0;
    changedSubCats_ = 0;

}

relocateCategoryDialog::relocateCategoryDialog( mmCoreDB* core,
    wxWindow* parent, wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size, long style )
{
    core_           = core;

    sourceCatID_    = -1;
    sourceSubCatID_ = -1;

    destCatID_      = -1;
    destSubCatID_   = -1;

    Create(parent, id, caption, pos, size, style);
}

bool relocateCategoryDialog::Create( wxWindow* parent, wxWindowID id,
                           const wxString& caption, const wxPoint& pos,
                           const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    Centre();
    return TRUE;
}

void relocateCategoryDialog::CreateControls()
{
    wxSize btnSize = wxSize(180,-1);
    wxStaticText* headerText = new wxStaticText( this, wxID_STATIC,
        _("Relocate all source categories to the destination category"));
    wxStaticLine* lineTop = new wxStaticLine(this,wxID_STATIC,wxDefaultPosition, wxDefaultSize,wxLI_HORIZONTAL); 
    wxStaticText* staticText_1 = new wxStaticText( this, wxID_STATIC,_("Relocate:"));
    sourceBtn_ = new wxButton( this, ID_DIALOG_CATEG_SELECT_BUTTON_SOURCE,_("Source Category"));
    wxStaticText* staticText_2 = new wxStaticText( this, wxID_STATIC,_("to:"));
    destBtn_ = new wxButton( this, ID_DIALOG_CATEG_SELECT_BUTTON_DEST,_("Destination Category"));
    wxStaticLine* lineBottom = new wxStaticLine(this,wxID_STATIC,wxDefaultPosition, wxDefaultSize,wxLI_HORIZONTAL); 
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(topSizer);
    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(boxSizer,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);

    boxSizer->Add(headerText,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);
    boxSizer->Add(lineTop,0,wxGROW|wxALL,5);

    wxBoxSizer* requestBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    requestBoxSizer->Add(staticText_1,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    requestBoxSizer->Add(sourceBtn_,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL);
    requestBoxSizer->Add(5,10,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);
    requestBoxSizer->Add(staticText_2,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL);
    requestBoxSizer->Add(destBtn_,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    boxSizer->Add(requestBoxSizer);
    boxSizer->Add(5,5,0,wxALIGN_CENTER_HORIZONTAL|wxALL,5);
    boxSizer->Add(lineBottom,0,wxGROW|wxALL,5);

    wxButton* okButton = new wxButton(this, wxID_OK);
    wxButton* cancelButton = new wxButton(this,wxID_CANCEL);
    cancelButton-> SetFocus () ;
    wxBoxSizer* buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonBoxSizer->Add(okButton,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    buttonBoxSizer->Add(cancelButton,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,5);
    boxSizer->Add(buttonBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
}

void relocateCategoryDialog::OnSelectSource(wxCommandEvent& /*event*/)
{
    mmCategDialog* sourceCat = new mmCategDialog(core_ , this, true);
    if (sourceCat->ShowModal() == wxID_OK)
    {
        sourceCatID_    = sourceCat->categID_;
        sourceSubCatID_ = sourceCat->subcategID_;      
        sourceBtn_->SetLabel(core_->categoryList_.GetFullCategoryString(sourceCatID_, sourceSubCatID_));
    }
    sourceCat->Destroy();
}

void relocateCategoryDialog::OnSelectDest(wxCommandEvent& /*event*/)
{
    mmCategDialog* destCat = new mmCategDialog(core_ , this, true);
    if (destCat->ShowModal() == wxID_OK)
    {
        destCatID_    = destCat->categID_;
        destSubCatID_ = destCat->subcategID_;

        destBtn_->SetLabel(core_->categoryList_.GetFullCategoryString(destCatID_, destSubCatID_));
    }
    destCat->Destroy();
}

wxString relocateCategoryDialog::updatedCategoriesCount()
{
    wxString countStr;
    countStr << (changedCats_ + changedSubCats_);
    return countStr;
}


void relocateCategoryDialog::OnOk(wxCommandEvent& /*event*/)
{
    if ((sourceCatID_ > 0) && (destCatID_ > 0) ) 
    {
        wxString msgStr = _("Please Confirm:");
        msgStr << wxT("\n\n") << _("Changing all categories of: ")
            << sourceBtn_->GetLabelText() << wxT("\n\n") << _("to category: ") << destBtn_->GetLabelText();

        int ans = wxMessageBox(msgStr,_("Category Relocation Confirmation"), wxOK|wxCANCEL|wxICON_QUESTION);
        if (ans == wxOK)
        {
            if (core_->bTransactionList_.RelocateCategory(core_,
                destCatID_, destSubCatID_, sourceCatID_, sourceSubCatID_, changedCats_, changedSubCats_) == 0)
	            EndModal(wxID_OK);
        }
    }
}


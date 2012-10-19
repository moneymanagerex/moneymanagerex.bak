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

#include "maincurrencydialog.h"
#include "currencydialog.h"
#include "util.h"
#include "defs.h"
#include "paths.h"

IMPLEMENT_DYNAMIC_CLASS( mmMainCurrencyDialog, wxDialog )

BEGIN_EVENT_TABLE( mmMainCurrencyDialog, wxDialog )
    EVT_BUTTON(wxID_ADD, mmMainCurrencyDialog::OnBtnAdd)
    EVT_BUTTON(wxID_EDIT, mmMainCurrencyDialog::OnBtnEdit)
    EVT_BUTTON(wxID_SELECTALL, mmMainCurrencyDialog::OnBtnSelect)
    EVT_BUTTON(wxID_REMOVE, mmMainCurrencyDialog::OnBtnDelete)

    EVT_LIST_ITEM_ACTIVATED(ID_LISTBOX,   mmMainCurrencyDialog::OnListItemActivated)
    EVT_LIST_ITEM_SELECTED(ID_LISTBOX,    mmMainCurrencyDialog::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(ID_LISTBOX,  mmMainCurrencyDialog::OnListItemDeselected)
END_EVENT_TABLE()


mmMainCurrencyDialog::mmMainCurrencyDialog(
    mmCoreDB* core,
    wxWindow* parent,
    bool bEnableSelect,
    wxWindowID id,
    const wxString& caption,
    const wxPoint& pos,
    const wxSize& size,
    long style
) : currencyID_(-1),
    db_ (core->db_.get()),
    core_(core),
    currencyListBox_(),
    bEnableSelect_(bEnableSelect)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmMainCurrencyDialog::Create(  wxWindow* parent, wxWindowID id,
                      const wxString& caption,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    //GetSizer()->Fit(this);
    //GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    fillControls();

    Centre();

    return TRUE;
}


void mmMainCurrencyDialog::fillControls()
{
    if (!core_) return;

    currencyListBox_->DeleteAllItems();
    int baseCurrencyID = core_->currencyList_.getBaseCurrencySettings();

    std::pair<mmCurrencyList::const_iterator, mmCurrencyList::const_iterator> range = core_->currencyList_.range();
    int idx = 0;
    for(mmCurrencyList::const_iterator it = range.first; it != range.second; ++ it)
    {
        int currencyID         = (*it)->currencyID_;

        wxListItem item;
        item.SetId(idx);
        item.SetData( currencyID );

        currencyListBox_->InsertItem( item );

        currencyListBox_->SetItem(idx, 0, (*it)->currencySymbol_);
        currencyListBox_->SetItem(idx, 1, (*it)->currencyName_);
        currencyListBox_->SetItem(idx, 2, baseCurrencyID == currencyID ? wxT("X") : wxT(""));
        currencyListBox_->SetItem(idx, 3, wxString()<<(*it)->baseConv_);

    }
    currencyListBox_->SetItemCount(idx);
    if (idx>0)
        currencyListBox_->RefreshItems(0, --idx);
}

void mmMainCurrencyDialog::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( this, wxID_STATIC,
       _("Currency List"));
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    currencyListBox_ = new wxListCtrl( this, ID_LISTBOX, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT);

    // Add first column
    wxListItem col0;
    col0.SetId(0);
    col0.SetText( _("Symbol") );
    col0.SetWidth(50);
    currencyListBox_->InsertColumn(0, col0);

    // Add second column
    wxListItem col1;
    col1.SetId(1);
    col1.SetText( _("Name") );
    col1.SetWidth(150);
    currencyListBox_->InsertColumn(1, col1);

    // Add third column
    wxListItem col2;
    col2.SetId(2);
    col2.SetText( wxT("") );
    col2.SetWidth(20);
    currencyListBox_->InsertColumn(2, col2);

    // Add third column
    wxListItem col3;
    col3.SetId(3);
    col3.SetText( _("Base Rate") );
    col3.SetWidth(120);
    currencyListBox_->InsertColumn(3, col3);

    itemBoxSizer3->Add(currencyListBox_, 1, wxGROW|wxALL, 1);

    wxPanel* itemPanel5 = new wxPanel( this, ID_PANEL10,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel5->SetSizer(itemBoxSizer6);

    wxButton* itemButton7 = new wxButton( itemPanel5, wxID_ADD, _("Add"),
        wxDefaultPosition, wxDefaultSize, 4 );
    itemBoxSizer6->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    itemButtonEdit_ = new wxButton( itemPanel5, wxID_EDIT, _("&Edit"),
        wxDefaultPosition, wxDefaultSize, 4 );
    itemBoxSizer6->Add(itemButtonEdit_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButtonEdit_->Disable();

    itemButtonDelete_ = new wxButton( itemPanel5, wxID_REMOVE, _("Remove"),
        wxDefaultPosition, wxDefaultSize, 4 );
    itemBoxSizer6->Add(itemButtonDelete_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemButtonDelete_->Disable();

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxALL, 4);

    wxButton* itemButtonSelect = new wxButton( this, wxID_SELECTALL, _("&Select"));
    itemBoxSizer9->Add(itemButtonSelect,  4, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    //itemButtonSelect->SetToolTip(_("Select the currently selected currency as the selected currency for the account"));

    if(bEnableSelect_ == false) {
        itemButtonSelect->Disable();
    }

    //Some interfaces has no any close buttons, it may confuse user. Cancel button added
    wxButton* itemCancelButton = new wxButton( this, wxID_CANCEL);
    itemBoxSizer9->Add(itemCancelButton,  0, wxALIGN_CENTER_VERTICAL|wxALL, 4);
    itemCancelButton->SetFocus();
}

bool mmMainCurrencyDialog::ShowToolTips()
{
    return TRUE;
}

wxBitmap mmMainCurrencyDialog::GetBitmapResource( const wxString& /*name*/ )
{
    // Bitmap retrieval
    return wxNullBitmap;
}

wxIcon mmMainCurrencyDialog::GetIconResource( const wxString& /*name*/ )
{
    return wxNullIcon;
}

void mmMainCurrencyDialog::OnBtnAdd(wxCommandEvent& /*event*/)
{
   wxTextEntryDialog dlg(this, _("Name of Currency to Add"), _("Add Currency"));
   if ( dlg.ShowModal() == wxID_OK )
   {
      wxString currText = dlg.GetValue().Trim();
      if (!currText.IsEmpty())
      {
         int currID = core_->currencyList_.getCurrencyID(currText);
         if (currID == -1)
         {
            boost::shared_ptr<mmCurrency> pCurrency(new mmCurrency());
            pCurrency->currencyName_ = currText;
            core_->currencyList_.AddCurrency(pCurrency);
            fillControls();
            currencyID_ = core_->currencyList_.getCurrencyID(currText);
            mmCurrencyDialog(core_, currencyID_, this).ShowModal();
         }
         else
         {
            wxMessageBox(_("Attempt to Add a currency which already exists!")
                ,_("Currency Dialog"), wxOK|wxICON_ERROR);
         }
      }
   }
}

void mmMainCurrencyDialog::OnBtnEdit(wxCommandEvent& /*event*/)
{
    mmCurrencyDialog(core_, currencyID_, this).ShowModal();
    fillControls();
}

void mmMainCurrencyDialog::OnBtnSelect(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ < 0) return;
    EndModal(wxID_OK);
}

void mmMainCurrencyDialog::OnBtnDelete(wxCommandEvent& /*event*/)
{
    if (selectedIndex_ < 0) return;

    int baseCurrencyID = core_->currencyList_.getBaseCurrencySettings();
    bool usedAsBase = currencyID_ == baseCurrencyID;

    if (core_->accountList_.currencyInUse(currencyID_) || usedAsBase)
    {
        wxMessageBox(_("Attempt to delete a currency being used by an account\n or as the base currency.")
            ,_("Currency Dialog"), wxOK|wxICON_ERROR);
    }
    else
    {
        if (wxMessageBox(_("Do you really want to delete the selected Currency?"),
                         _("Currency Dialog"),wxICON_QUESTION|wxYES|wxNO) == wxYES)
        {
            core_->currencyList_.deleteCurrency(currencyID_);
            fillControls();
        }
    }
}

bool mmMainCurrencyDialog::Execute(mmCoreDB* core, wxWindow* parent, int& currencyID)
{
    bool result = false;

    mmMainCurrencyDialog* dlg = new mmMainCurrencyDialog(core, parent);
    if (dlg->ShowModal() == wxID_OK)
    {
        currencyID = dlg->currencyID_;
        result = true;
    }
    dlg->Destroy();

    return result;
}

void mmMainCurrencyDialog::OnListItemDeselected(wxListEvent& /*event*/)
{
    selectedIndex_ = -1;
    currencyID_ = -1;
    itemButtonEdit_->Enable(false);
    if (!bEnableSelect_)
        itemButtonDelete_->Enable(false);
}

void mmMainCurrencyDialog::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    currencyID_ = currencyListBox_->GetItemData(selectedIndex_);
    itemButtonEdit_->Enable();
    if (!bEnableSelect_)    // prevent user deleting currencies when editing accounts.
        itemButtonDelete_->Enable();
}

void mmMainCurrencyDialog::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    currencyID_ = currencyListBox_->GetItemData(selectedIndex_);
    wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, wxID_ANY);

    if (bEnableSelect_)
        OnBtnSelect(event);
    else
        OnBtnEdit(event);
}



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
#include "mmcoredb.h"
#include "dbwrapper.h"
#include "util.h"
#include "defs.h"
#include "paths.h"
 
IMPLEMENT_DYNAMIC_CLASS( mmMainCurrencyDialog, wxDialog )

BEGIN_EVENT_TABLE( mmMainCurrencyDialog, wxDialog )
    EVT_BUTTON(wxID_ADD, mmMainCurrencyDialog::OnBtnAdd)
    EVT_BUTTON(wxID_EDIT, mmMainCurrencyDialog::OnBtnEdit)
    EVT_BUTTON(wxID_SELECTALL, mmMainCurrencyDialog::OnBtnSelect)
    EVT_BUTTON(wxID_REMOVE, mmMainCurrencyDialog::OnBtnDelete)
    EVT_LISTBOX(ID_LISTBOX, mmMainCurrencyDialog::OnlistBoxSelection)
    EVT_LISTBOX_DCLICK(ID_LISTBOX, mmMainCurrencyDialog::OnlistBoxDoubleClicked)
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
    if (!core_)
       return;

    currencyListBox_->Clear();

    std::pair<mmCurrencyList::const_iterator, mmCurrencyList::const_iterator> range = core_->rangeCurrency(); 
    int idx = 0;
    for(mmCurrencyList::const_iterator it = range.first; it != range.second; ++ it)
    {
        int currencyID         = (*it)->currencyID_;
        wxString currencyName  = (*it)->currencyName_;
        currencyListBox_->Insert(currencyName, idx++, new mmListBoxItem(currencyID, currencyName));
    }
}

void mmMainCurrencyDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( this, wxID_STATIC, 
       _("Currency List"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    currencyListBox_ = new wxListBox( this, ID_LISTBOX, 
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_NEEDED_SB );
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
    wxButton* itemCancelButton = new wxButton( this, wxID_CANCEL, _("&Cancel"));
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
         int currID = core_->getCurrencyID(currText);
         if (currID == -1)
         {
            boost::shared_ptr<mmCurrency> pCurrency(new mmCurrency());
            pCurrency->currencyName_ = currText;
            core_->addCurrency(pCurrency);
            fillControls();
            currencyID_ = core_->getCurrencyID(currText);
            mmCurrencyDialog(core_, currencyID_, this).ShowModal();
         }
         else
         {
            displayCurrencyDialogErrorMessage(_("Attempt to Add a currency which already exists!"));
         }
      }
   }
}

void mmMainCurrencyDialog::OnBtnEdit(wxCommandEvent& /*event*/)
{
   int selIndex = currencyListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND)
    {
        mmListBoxItem* pItem = (mmListBoxItem*) currencyListBox_->GetClientObject(selIndex);
        currencyID_ = pItem->getIndex();
        wxASSERT(currencyID_ != -1);

        mmCurrencyDialog(core_, currencyID_, this).ShowModal();
    }
}

void mmMainCurrencyDialog::OnBtnSelect(wxCommandEvent& /*event*/)
{
    int selIndex = currencyListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND)
    {
        mmListBoxItem* pItem = (mmListBoxItem*) currencyListBox_->GetClientObject(selIndex);
        currencyID_ = pItem->getIndex();
        wxASSERT(currencyID_ != -1);
        EndModal(wxID_OK);
    }
}

void mmMainCurrencyDialog::OnlistBoxSelection(wxCommandEvent& /*event*/)
{
    itemButtonEdit_->Enable();
    if (!bEnableSelect_)    // prevent user deleting currencies when editing accounts.
        itemButtonDelete_->Enable();
}

void mmMainCurrencyDialog::OnlistBoxDoubleClicked(wxCommandEvent& event)
{
    if (bEnableSelect_)
        OnBtnSelect(event);
    else
        OnBtnEdit(event);
}

void mmMainCurrencyDialog::OnBtnDelete(wxCommandEvent& /*event*/)
{
    int selIndex = currencyListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND)
    {
        mmListBoxItem* pItem = (mmListBoxItem*) currencyListBox_->GetClientObject(selIndex);
        currencyID_ = pItem->getIndex();
        wxASSERT(currencyID_ != -1);

        int baseCurrencyID = mmDBWrapper::getBaseCurrencySettings(db_);
        bool usedAsBase = currencyID_ == baseCurrencyID;

        if (core_->currencyInUse(currencyID_) || usedAsBase)
        {
            displayCurrencyDialogErrorMessage(_("Attempt to delete a currency being used by an account\n or as the base currency."));
        }
        else
        {
            if (wxMessageBox(_("Please confirm deletion of a selected Currency"),
                             _("Currency Dialog"),wxICON_QUESTION|wxOK|wxCANCEL) == wxOK)
            {
                core_->deleteCurrency(currencyID_);
                fillControls();
            }
        }
    }
}

void mmMainCurrencyDialog::displayCurrencyDialogErrorMessage(wxString msg)
{
    wxMessageBox(msg,_("Currency Dialog"),wxICON_ERROR);
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

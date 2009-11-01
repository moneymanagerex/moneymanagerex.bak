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

 
#define MMEX_ICON_FNAME mmGetBaseWorkingPath() + wxT("/mmex.ico")


using namespace std;

IMPLEMENT_DYNAMIC_CLASS( mmMainCurrencyDialog, wxDialog )

BEGIN_EVENT_TABLE( mmMainCurrencyDialog, wxDialog )
    EVT_BUTTON(ID_MAINCURRENCYBUTTON_ADD, mmMainCurrencyDialog::OnAdd)
    EVT_BUTTON(ID_MAINCURRENCYBUTTON_EDIT, mmMainCurrencyDialog::OnEdit)
    EVT_BUTTON(ID_MAINCURRENCYBUTTON_SELECT, mmMainCurrencyDialog::OnSelect)
    EVT_LISTBOX_DCLICK(ID_LISTBOX, mmMainCurrencyDialog::OnDoubleClicked)
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

/*!
 * MyDialog creator
 */

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

    wxIcon icon(MMEX_ICON_FNAME, wxBITMAP_TYPE_ICO, 32, 32);
    SetIcon(icon);

    fillControls();

    Centre();

    return TRUE;
}


void mmMainCurrencyDialog::fillControls()
{
    if (!core_)
       return;

    currencyListBox_->Clear();

    for (int idx = 0; idx < (int)core_->currencyList_.currencies_.size(); idx++)
    {
        int currencyID         = core_->currencyList_.currencies_[idx]->currencyID_;
        wxString currencyName  = core_->currencyList_.currencies_[idx]->currencyName_;
        currencyListBox_->Insert(currencyName, idx, new mmCurrencyListBoxItem(currencyID));
    }
}
/*!
 * Control creation for MyDialog
 */

void mmMainCurrencyDialog::CreateControls()
{    
////@begin MyDialog content construction
    // Generated by DialogBlocks, 06/24/06 17:00:25 (Personal Edition)

    mmMainCurrencyDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, 
       _("Currency List"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxString* itemListBox4Strings = NULL;
    currencyListBox_ = new wxListBox( itemDialog1, ID_LISTBOX, 
        wxDefaultPosition, wxDefaultSize, 0, itemListBox4Strings, wxLB_SINGLE );
    itemBoxSizer3->Add(currencyListBox_, 1, wxGROW|wxALL, 1);

    wxPanel* itemPanel5 = new wxPanel( itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel5->SetSizer(itemBoxSizer6);

    wxButton* itemButton7 = new wxButton( itemPanel5, ID_MAINCURRENCYBUTTON_ADD, _("&Add"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemPanel5, ID_MAINCURRENCYBUTTON_EDIT, _("&Edit"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton9 = new wxButton( itemPanel5, ID_MAINCURRENCYBUTTON_SELECT, _("&Select"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    if(bEnableSelect_ == false) {
        itemButton9->Disable();
    }

////@end MyDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool mmMainCurrencyDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap mmMainCurrencyDialog::GetBitmapResource( const wxString& /*name*/ )
{
    // Bitmap retrieval
////@begin MyDialog bitmap retrieval
    return wxNullBitmap;
////@end MyDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon mmMainCurrencyDialog::GetIconResource( const wxString& /*name*/ )
{
    return wxNullIcon;
}

void mmMainCurrencyDialog::OnAdd(wxCommandEvent& /*event*/)
{
   wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Name of Currency to Add"), 
      _("Add Currency"));
   if ( dlg->ShowModal() == wxID_OK )
   {
      wxString currText = dlg->GetValue().Trim();
      if (!currText.IsEmpty())
      {
         int currID = mmDBWrapper::getCurrencyID(core_->db_.get(), currText);
         if (currID == -1)
         {

            boost::shared_ptr<mmCurrency> pCurrency(new mmCurrency());
            pCurrency->currencyName_ = currText;
            core_->currencyList_.addCurrency(pCurrency);
            fillControls();

         }
         else
         {
            mmShowErrorMessage(this, _("Currency name already exists!"), _("Error"));

         }
      }
   }
   dlg->Destroy();
}


void mmMainCurrencyDialog::OnEdit(wxCommandEvent& /*event*/)
{
   int selIndex = currencyListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND)
    {
        mmCurrencyListBoxItem* pItem = (mmCurrencyListBoxItem*) currencyListBox_->GetClientObject(selIndex);
        currencyID_ = pItem->getListIndex();
        wxASSERT(currencyID_ != -1);

        mmCurrencyDialog *dlg = new mmCurrencyDialog(core_, currencyID_, this);
        if ( dlg->ShowModal() == wxID_OK )
        {
        }
        dlg->Destroy();
    }
}

void mmMainCurrencyDialog::OnSelect(wxCommandEvent& /*event*/)
{
    int selIndex = currencyListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND)
    {
        mmCurrencyListBoxItem* pItem = (mmCurrencyListBoxItem*) currencyListBox_->GetClientObject(selIndex);
        currencyID_ = pItem->getListIndex();
        wxASSERT(currencyID_ != -1);
        EndModal(wxID_OK);
    }
}

void mmMainCurrencyDialog::OnDoubleClicked(wxCommandEvent& event)
{
    OnSelect(event);
}

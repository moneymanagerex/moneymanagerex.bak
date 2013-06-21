/////////////////////////////////////////////////////////////////////////////
// Name:
// Purpose:
// Author:      Madhan Kanagavel
// Modified by:
// Created:     04/24/07 20:35:10
// RCS-ID:
// Copyright:
// Licence:
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (Personal Edition), 04/24/07 20:35:10
// Manually modified 2011,2012

#include "constants.h"
#include "splittransactionsdialog.h"
#include "splitdetailsdialog.h"
#include "util.h"
#include "mmCurrencyFormatter.h"
#include <wx/statline.h>

/*!
 * SplitTransactionDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( SplitTransactionDialog, wxDialog )

/*!
 * SplitTransactionDialog event table definition
 */

BEGIN_EVENT_TABLE( SplitTransactionDialog, wxDialog )
    EVT_BUTTON( wxID_ADD, SplitTransactionDialog::OnButtonAddClick )
    EVT_BUTTON( wxID_REMOVE, SplitTransactionDialog::OnButtonRemoveClick )
    EVT_BUTTON( wxID_EDIT, SplitTransactionDialog::OnButtonEditClick )
    EVT_LIST_ITEM_ACTIVATED( ID_LISTCTRL3 ,SplitTransactionDialog::OnListDblClick )
END_EVENT_TABLE()

SplitTransactionDialog::SplitTransactionDialog( )
{
}

SplitTransactionDialog::SplitTransactionDialog(
    mmCoreDB* core,
    mmSplitTransactionEntries* split,
    int transType,
    wxWindow* parent, wxWindowID id,
    const wxString& caption,
    const wxPoint& pos,
    const wxSize& size,
    long style )
{
    core_  = core;
    split_ = split;

    transType_ = transType;
    if (transType_ == DEF_TRANSFER)
        transType_ = DEF_WITHDRAWAL;

    Create(parent, id, caption, pos, size, style);
}

bool SplitTransactionDialog::Create( wxWindow* parent, wxWindowID id,
                                    const wxString& caption,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style )
{
    lcSplit_ = NULL;
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();

    DataToControls();

    return TRUE;
}

void SplitTransactionDialog::DataToControls()
{
    lcSplit_->DeleteAllItems();
    long idx = 0;
    for (const auto & entry : split_->entries_)
    {
        lcSplit_->InsertItem((long)idx
            , core_->categoryList_.GetFullCategoryString(entry->categID_, entry->subCategID_)
            , -1);

        lcSplit_->SetItem((long)idx++, 1, CurrencyFormatter::float2String(entry->splitAmount_));
    }
    UpdateSplitTotal();
    itemButtonNew_->SetFocus();
}

void SplitTransactionDialog::CreateControls()
{
    wxBoxSizer* dialogMainSizerV = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(dialogMainSizerV);

    wxStaticText* headingText = new wxStaticText( this, wxID_STATIC, _(" Split Category Details"));
    dialogMainSizerV->Add(headingText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxRIGHT, 10);

    wxBoxSizer* listCtrlSizer = new wxBoxSizer(wxHORIZONTAL);
    dialogMainSizerV->Add(listCtrlSizer, 1, wxGROW|wxALL, 5);

    lcSplit_ = new wxListCtrl( this, ID_LISTCTRL3, wxDefaultPosition
        , wxSize(265, 120), wxLC_REPORT|wxLC_SINGLE_SEL );
    listCtrlSizer->Add(lcSplit_, 1, wxGROW|wxALL, 5);

    wxListItem itemCol;
    itemCol.SetText(_("Category"));
    lcSplit_->InsertColumn(0, itemCol);
    lcSplit_->SetColumnWidth( 0, 180 );

    wxListItem itemCol1;
    itemCol1.SetText(_("Amount"));
    itemCol1.SetAlign(wxLIST_FORMAT_RIGHT);
    lcSplit_->InsertColumn(1, itemCol1);
    lcSplit_->SetColumnWidth( 1, 80 );

    wxBoxSizer* totalAmountSizer = new wxBoxSizer(wxHORIZONTAL);
    wxString totalMessage = _("Total:");
    if (transType_ == DEF_WITHDRAWAL)
    {
        totalMessage.Prepend(" ");
        totalMessage.Prepend(_("Withdrawal"));
    }
    if (transType_ == DEF_DEPOSIT)
    {
        totalMessage.Prepend(" ");
        totalMessage.Prepend(_("Deposit"));
    }
    wxStaticText* transAmountText_ = new wxStaticText( this, wxID_STATIC, totalMessage);
    transAmount_ = new wxStaticText( this, wxID_STATIC, wxEmptyString);
    totalAmountSizer->Add(transAmountText_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    totalAmountSizer->Add(transAmount_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    dialogMainSizerV->Add(totalAmountSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticLine* line = new wxStaticLine (this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    dialogMainSizerV->Add(line, 0, wxGROW|wxLEFT|wxTOP|wxRIGHT, 10);

    wxBoxSizer* mainButtonSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topRowButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* bottomRowButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    mainButtonSizer->Add(topRowButtonSizer, 0, wxALIGN_RIGHT|wxALL, 0);
    mainButtonSizer->Add(bottomRowButtonSizer, 0, wxALIGN_RIGHT|wxALL, 0);
    dialogMainSizerV->Add(mainButtonSizer, 0, wxALIGN_RIGHT|wxALL, 10);

    itemButtonNew_ = new wxButton( this, wxID_ADD);
    itemButtonEdit_ = new wxButton( this, wxID_EDIT);
    itemButtonDelete_ = new wxButton( this, wxID_REMOVE);
    topRowButtonSizer->Add(itemButtonNew_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
    topRowButtonSizer->Add(itemButtonEdit_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
    topRowButtonSizer->Add(itemButtonDelete_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 0);

    itemButtonOK_ = new wxButton( this, wxID_OK);
    wxButton* itemButtonCancel = new wxButton( this, wxID_CANCEL);
    itemButtonCancel->SetFocus();

    bottomRowButtonSizer->Add(itemButtonOK_, 0, wxALIGN_RIGHT|wxTOP|wxRIGHT, 5);
    bottomRowButtonSizer->Add(itemButtonCancel, 0, wxALIGN_RIGHT|wxTOP, 5);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONADD
 */

void SplitTransactionDialog::OnButtonAddClick( wxCommandEvent& /*event*/ )
{
    int categID = -1;
    int subcategID = -1;
    double amount  = 0.0;
    SplitDetailDialog sdd(core_, _("&Select Category"), &categID, &subcategID, &amount, transType_, this);
    if (sdd.ShowModal() == wxID_OK)
    {
        int numToInsert = lcSplit_->GetItemCount();
        lcSplit_->InsertItem(numToInsert, sdd.m_categString_, -1);

        lcSplit_->SetItem(numToInsert, 1, CurrencyFormatter::float2String(*sdd.m_amount_));

        mmSplitTransactionEntry* pSplitEntry(new mmSplitTransactionEntry);
        pSplitEntry->splitAmount_  = *sdd.m_amount_;
        pSplitEntry->categID_      = categID;
        pSplitEntry->subCategID_   = subcategID;
        split_->addSplit(pSplitEntry);

        UpdateSplitTotal();
    }
}

void SplitTransactionDialog::OnButtonEditClick( wxCommandEvent& /*event*/ )
{
    EditEntry();
}

void SplitTransactionDialog::OnButtonRemoveClick( wxCommandEvent& /*event*/ )
{
    long item = GetSelectedItem();
    if (item >= 0) // Item found in list
    {
        lcSplit_->DeleteItem(item);
        split_->removeSplitByIndex(item);
        UpdateSplitTotal();
    }
}

bool SplitTransactionDialog::ShowToolTips()
{
    return TRUE;
}

wxBitmap SplitTransactionDialog::GetBitmapResource( const wxString& /*name*/ )
{
    return wxNullBitmap;
}

wxIcon SplitTransactionDialog::GetIconResource( const wxString& /*name*/ )
{
    return wxNullIcon;
}

void SplitTransactionDialog::UpdateSplitTotal()
{
    transAmount_->SetLabel(CurrencyFormatter::float2String(split_->getTotalSplits()));
}

long SplitTransactionDialog::GetSelectedItem()
{
    // locate the selected item in the list
    long selectedItem = -1;
    long item = -1;
    for ( ;; )
    {
        item = lcSplit_->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;
        // this item is selected - do whatever is needed with it
        selectedItem = item;
    }

    return selectedItem;
}

void SplitTransactionDialog::EditEntry()
{
    // locate the selected item in the list and exit if not found
    long item = this->GetSelectedItem();
    if (item == wxNOT_FOUND) return;

    int categID    = split_->entries_[item]->categID_;
    int subCategID = split_->entries_[item]->subCategID_;
    double amount  = split_->entries_[item]->splitAmount_;
    wxString category = core_->categoryList_.GetFullCategoryString(categID,subCategID);

    SplitDetailDialog sdd(core_, category, &categID, &subCategID, &amount, transType_, this);
    if (sdd.ShowModal() == wxID_OK)
    {
        split_->entries_[item]->categID_     = categID;
        split_->entries_[item]->subCategID_  = subCategID;
        split_->entries_[item]->splitAmount_ = amount;
        DataToControls();
        UpdateSplitTotal();
    }
}

void SplitTransactionDialog::OnListDblClick(wxListEvent& /*event*/)
{
    if (itemButtonEdit_->IsShown()) EditEntry();
}

void SplitTransactionDialog::SetDisplaySplitCategories()
{
    itemButtonNew_->Hide();
    itemButtonEdit_->Hide();
    itemButtonDelete_->Hide();
    itemButtonOK_->Hide();
}


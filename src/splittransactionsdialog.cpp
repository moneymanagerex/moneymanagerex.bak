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
#include "model/Model_Category.h"

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
    EVT_DATAVIEW_ITEM_ACTIVATED(wxID_ANY, SplitTransactionDialog::OnListDblClick)
    EVT_DATAVIEW_SELECTION_CHANGED(wxID_ANY, SplitTransactionDialog::OnListItemSelected)
    //EVT_DATAVIEW_ITEM_CONTEXT_MENU(wxID_ANY, SplitTransactionDialog::OnItemRightClick)
END_EVENT_TABLE()

SplitTransactionDialog::SplitTransactionDialog( )
{
}

SplitTransactionDialog::SplitTransactionDialog(
    Model_Splittransaction::Data_Set &split
    , wxWindow* parent
    , int transType
    , mmSplitTransactionEntries* splt)
{
    split_ = split;
    transType_ = transType;
    split_id_ = 0;
    selectedIndex_ = 0;
    if (transType_ == DEF_TRANSFER)
        transType_ = DEF_WITHDRAWAL;

    long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX;
    Create(parent, wxID_ANY, _("Split Transaction Dialog")
        , wxDefaultPosition, wxSize(400, 300), style);
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
    for (const auto & entry : split_)
    {
        const Model_Category::Data* category = Model_Category::instance().get(entry.CATEGID);
        const Model_Subcategory::Data* sub_category = (entry.SUBCATEGID != -1 ? Model_Subcategory::instance().get(entry.SUBCATEGID) : 0);

        wxVector<wxVariant> data;
        data.push_back(wxVariant(Model_Category::full_name(category, sub_category)));
        data.push_back(wxVariant(CurrencyFormatter::float2String(entry.SPLITTRANSAMOUNT)));
        lcSplit_->AppendItem(data, (wxUIntPtr)entry.SPLITTRANSID);
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

    lcSplit_ = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition
        , wxSize(265, 120));
    lcSplit_->AppendTextColumn(_("Category"), wxDATAVIEW_CELL_INERT, 180);
    lcSplit_->AppendTextColumn(_("Amount"), wxDATAVIEW_CELL_INERT, 180);

    listCtrlSizer->Add(lcSplit_, 1, wxGROW | wxALL, 5);

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

    Model_Splittransaction::Data *split = Model_Splittransaction::instance().create();
    SplitDetailDialog sdd(this, split, _("&Select Category"), &categID, &subcategID, &amount, transType_);
    if (sdd.ShowModal() == wxID_OK)
    {
        split->CATEGID = categID;
        split->SUBCATEGID = subcategID;
        split->SPLITTRANSAMOUNT = *sdd.m_amount_;
        //TODO:
    }
}

void SplitTransactionDialog::OnButtonEditClick( wxCommandEvent& /*event*/ )
{
    EditEntry(split_id_);
}

void SplitTransactionDialog::OnButtonRemoveClick( wxCommandEvent& event )
{
    /*wxDataViewItem item = event.GetItem();
    selectedIndex_ = currencyListBox_->ItemToRow(item);
    long item = GetSelectedItem();
    if (item >= 0) // Item found in list
    {
        lcSplit_->DeleteItem(item);
        split_.erase();  removeSplitByIndex(item);
        UpdateSplitTotal();
    }*/
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
    transAmount_->SetLabel(CurrencyFormatter::float2String(123456));
}

void SplitTransactionDialog::EditEntry(int id)
{
    Model_Splittransaction::Data *split = Model_Splittransaction::instance().get(id);
    if (!split) return;

    int categID    = split->CATEGID;
    int subCategID = split->SUBCATEGID;
    double amount = split->SPLITTRANSAMOUNT;
    const Model_Category::Data* category = Model_Category::instance().get(categID);
    const Model_Subcategory::Data* sub_category = (subCategID != -1 ? Model_Subcategory::instance().get(subCategID) : 0);
    wxString category_name = Model_Category::full_name(category, sub_category);

    SplitDetailDialog sdd(this, split, category_name, &categID, &subCategID, &amount, transType_);
    if (sdd.ShowModal() == wxID_OK)
    {
        split->CATEGID = categID;
        split->SUBCATEGID = subCategID;
        split->SPLITTRANSAMOUNT;
        DataToControls();
        UpdateSplitTotal();
    }
}

void SplitTransactionDialog::OnListItemSelected(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    selectedIndex_ = lcSplit_->ItemToRow(item);
    if (selectedIndex_ >= 0)
    {
        split_id_ = (int) lcSplit_->GetItemData(item);

    }
}

void SplitTransactionDialog::OnListDblClick(wxDataViewEvent& event)
{
    if (itemButtonEdit_->IsShown()) EditEntry(split_id_);
}

void SplitTransactionDialog::SetDisplaySplitCategories()
{
    itemButtonNew_->Hide();
    itemButtonEdit_->Hide();
    itemButtonDelete_->Hide();
    itemButtonOK_->Hide();
}


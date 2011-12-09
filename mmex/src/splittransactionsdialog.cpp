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


#include "splittransactionsdialog.h"
#include "splitdetailsdialog.h"
#include "util.h"

////@begin includes
////@end includes

////@begin XPM images

////@end XPM images

/*!
 * SplitTransactionDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( SplitTransactionDialog, wxDialog )

/*!
 * SplitTransactionDialog event table definition
 */

BEGIN_EVENT_TABLE( SplitTransactionDialog, wxDialog )
    EVT_BUTTON( wxID_NEW, SplitTransactionDialog::OnButtonAddClick )
    EVT_BUTTON( wxID_DELETE, SplitTransactionDialog::OnButtonRemoveClick )
    EVT_BUTTON( wxID_EDIT, SplitTransactionDialog::OnButtonEditClick )
    EVT_BUTTON( wxID_CANCEL, SplitTransactionDialog::OnButtonCloseClick )
END_EVENT_TABLE()

SplitTransactionDialog::SplitTransactionDialog( )
{
}

SplitTransactionDialog::SplitTransactionDialog( 
                                               mmCoreDB* core,
                                               mmSplitTransactionEntries* split,
                                               wxWindow* parent, wxWindowID id, 
                                               const wxString& caption, 
                                               const wxPoint& pos, 
                                               const wxSize& size, 
                                               long style )
{
    core_ = core;
    split_ = split;
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
    for (size_t idx = 0; idx < split_->numEntries(); ++idx)
    {
        boost::shared_ptr<mmCategory> pSharedCategory = split_->entries_[idx]->category_.lock();
        wxASSERT(pSharedCategory);
        boost::shared_ptr<mmCategory> parent = pSharedCategory->parent_.lock();
        wxString fullCatStr;
        if (parent)
        {
            fullCatStr = parent->categName_ + wxT(":") + pSharedCategory->categName_;
        }
        else
        {
            fullCatStr = pSharedCategory->categName_;
        }

        lcSplit_->InsertItem((long)idx,fullCatStr, -1);

        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(split_->entries_[idx]->splitAmount_, dispAmount);

        lcSplit_->SetItem((long)idx, 1, dispAmount);
    }
	UpdateSplitTotal();
}

void SplitTransactionDialog::CreateControls()
{    
////@begin SplitTransactionDialog content construction
    // Generated by DialogBlocks, 04/24/07 20:35:10 (Personal Edition)

    SplitTransactionDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, _("Transaction Details"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer4, 1, wxGROW|wxALL, 5);

    lcSplit_ = new wxListCtrl( itemDialog1, ID_LISTCTRL3, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT|wxLC_SINGLE_SEL );
    itemBoxSizer4->Add(lcSplit_, 1, wxGROW|wxALL, 5);

    wxListItem itemCol;
    itemCol.SetText(_("Category"));
    lcSplit_->InsertColumn(0, itemCol);
    lcSplit_->SetColumnWidth( 0, 140 );

    wxListItem itemCol1;
    itemCol1.SetText(_("Amount"));
    lcSplit_->InsertColumn(1, itemCol1);
    lcSplit_->SetColumnWidth( 1, 100 );
	
	wxBoxSizer* itemBoxSizerTotAmount = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizerTotAmount, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
	wxStaticText* transAmountText_ = new wxStaticText( itemDialog1, wxID_STATIC, _("Total:"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizerTotAmount->Add(transAmountText_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	transAmount_ = new wxStaticText( itemDialog1, wxID_STATIC, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizerTotAmount->Add(transAmount_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer1->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
    wxBoxSizer* itemBoxSizer66 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer1->Add(itemBoxSizer66, 0, wxALIGN_RIGHT|wxALL, 0);

    wxButton* itemButtonNew = new wxButton( itemDialog1, wxID_NEW, _("&New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButtonNew, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButtonDelete = new wxButton( itemDialog1, wxID_DELETE, _("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButtonDelete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButtonEdit = new wxButton( itemDialog1, wxID_EDIT, _("&Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButtonEdit, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButtonOK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer66->Add(itemButtonOK, 0, wxALIGN_RIGHT|wxALL, 5);

    wxButton* itemButtonCancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButtonCancel->SetFocus();
    itemBoxSizer66->Add(itemButtonCancel, 0, wxALIGN_RIGHT|wxALL, 5);

////@end SplitTransactionDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONADD
 */

void SplitTransactionDialog::OnButtonAddClick( wxCommandEvent& event )
{
    int categID = -1;
    int subcategID = -1;
    double amount  = 0.0;
    SplitDetailDialog sdd(core_, _("&Select Category"), &categID, &subcategID, &amount, this);
    if (sdd.ShowModal() == wxID_OK)
    {
        int numToInsert = lcSplit_->GetItemCount();
        lcSplit_->InsertItem(numToInsert, sdd.m_categString_, -1);
       
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(*sdd.m_amount_, dispAmount);
        
        lcSplit_->SetItem(numToInsert, 1, dispAmount);

        boost::shared_ptr<mmSplitTransactionEntry> pSplitEntry(new mmSplitTransactionEntry);
        pSplitEntry->splitAmount_  = *sdd.m_amount_;
        pSplitEntry->categID_      = categID;
        pSplitEntry->subCategID_   = subcategID;
        pSplitEntry->category_      = core_->categoryList_.getCategorySharedPtr(categID, 
                                                                                subcategID);
        wxASSERT(pSplitEntry->category_.lock());
        split_->addSplit(pSplitEntry);
		
		UpdateSplitTotal();
	}
    event.Skip();
}

void SplitTransactionDialog::OnButtonEditClick( wxCommandEvent& event )
{
    long item = -1;
    for ( ;; )
    {
        item = lcSplit_->GetNextItem(item,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;
        //lcSplit_->DeleteItem(item);
        //split_->removeSplitByIndex(item);

        break;
    }
	if (item == -1)
	    return;

    int categID = -1;
    int subcategID = -1;
    double amount  = 0.0;

        wxString fullCatStr;
        wxString CatStr=wxT("");
        wxString subCatStr=wxT("");
        fullCatStr << lcSplit_->GetItemText(item);
        CatStr = fullCatStr.substr(0,fullCatStr.Find(wxT(":")));
        categID = core_->categoryList_.getCategoryID(CatStr);
        
        if (fullCatStr.Find(wxT(":"))){
            subCatStr = fullCatStr.substr(fullCatStr.Find(wxT(":"))+1);
        subcategID = core_->categoryList_.getSubCategoryID(categID, subCatStr);  
	    }     
 
        amount = lcSplit_->GetItemData(item);	
//wxLogMessage(wxString::Format(wxT("Item-%ld is selected."), item)); 
//wxLogMessage(wxString::Format(wxT("Categ-%s."), CatStr.c_str())); 
//wxLogMessage(wxString::Format(wxT("Subcateg-%s."), subCatStr.c_str())); 
//wxLogMessage(wxString::Format(wxT("CategID-%d."), categID)); 
//wxLogMessage(wxString::Format(wxT("SubCategID-%d."), subcategID)); 
    
    SplitDetailDialog sdd(core_, fullCatStr, &categID, &subcategID, &amount, this);
    if (sdd.ShowModal() == wxID_OK)
    {
        lcSplit_->DeleteItem(item);
        split_->removeSplitByIndex(item);
        
        lcSplit_->SetItem(item, 0, fullCatStr);
       
        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(amount, dispAmount);
        
        lcSplit_->SetItem(item, 1, dispAmount);

        boost::shared_ptr<mmSplitTransactionEntry> pSplitEntry(new mmSplitTransactionEntry);
        pSplitEntry->splitAmount_  = *sdd.m_amount_;
        pSplitEntry->categID_      = categID;
        pSplitEntry->subCategID_   = subcategID;
        pSplitEntry->category_      = core_->categoryList_.getCategorySharedPtr(categID, 
                                                                                subcategID);
        wxASSERT(pSplitEntry->category_.lock());
        split_->addSplit(pSplitEntry);
		
		UpdateSplitTotal();
	}
	    DataToControls();

    event.Skip();
}

void SplitTransactionDialog::OnButtonRemoveClick( wxCommandEvent& event )
{
    long item = -1;
    for ( ;; )
    {
        item = lcSplit_->GetNextItem(item,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;
wxLogMessage(wxString::Format(wxT("Item %ld is selected."), item));    
        lcSplit_->DeleteItem(item);
        split_->removeSplitByIndex(item);

        break;
    }
	UpdateSplitTotal();

    event.Skip();
}


void SplitTransactionDialog::OnButtonCloseClick( wxCommandEvent& /*event*/ )
{
    EndModal(wxID_CANCEL);
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
	// Update the total amount of all splits
	wxString splitTotal;
	if (split_->numEntries() > 0)
	{
		mmex::formatDoubleToCurrencyEdit(split_->getTotalSplits(), splitTotal);
	}
	else
	{
		mmex::formatDoubleToCurrencyEdit(0.0, splitTotal);
	}
	transAmount_->SetLabel(splitTotal);
}

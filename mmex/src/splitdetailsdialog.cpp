/////////////////////////////////////////////////////////////////////////////
// Name:        
// Purpose:     
// Author:      Madhan Kanagavel
// Modified by: 
// Created:     04/24/07 21:52:26
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

////@begin includes
#include "splitdetailsdialog.h"
#include "categdialog.h"
#include "util.h"
////@end includes

////@begin XPM images

////@end XPM images

/*!
 * SplitDetailDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( SplitDetailDialog, wxDialog )

/*!
 * SplitDetailDialog event table definition
 */

BEGIN_EVENT_TABLE( SplitDetailDialog, wxDialog )

////@begin SplitDetailDialog event table entries
    EVT_BUTTON( ID_BUTTONCATEGORY, SplitDetailDialog::OnButtonCategoryClick )

    EVT_BUTTON( ID_BUTTONOK, SplitDetailDialog::OnButtonOKClick )

    EVT_BUTTON( ID_BUTTONCANCEL, SplitDetailDialog::OnButtonCancelClick )

////@end SplitDetailDialog event table entries

END_EVENT_TABLE()

/*!
 * SplitDetailDialog constructors
 */

SplitDetailDialog::SplitDetailDialog( )
{
}

SplitDetailDialog::SplitDetailDialog( mmCoreDB* core, 
                                     const wxString& categString, 
                                     int* categID,
                                     int* subcategID,
                                     double* amount,
                                     wxWindow* parent, 
                                     wxWindowID id, 
                                     const wxString& caption, 
                                     const wxPoint& pos, 
                                     const wxSize& size, 
                                     long style )
{
    core_ = core;
    m_categID_  = categID;
    m_subcategID_ = subcategID;
    m_amount_ = amount;
    m_categString_ = categString;
    Create(parent, id, caption, pos, size, style);
}

/*!
 * SplitDetailDialog creator
 */

bool SplitDetailDialog::Create( wxWindow* parent, wxWindowID id, 
                               const wxString& caption, 
                               const wxPoint& pos, 
                               const wxSize& size, 
                               long style )
{
////@begin SplitDetailDialog member initialisation
////@end SplitDetailDialog member initialisation

////@begin SplitDetailDialog creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end SplitDetailDialog creation

    wxString dispAmount;
    mmCurrencyFormatter::formatDoubleToCurrencyEdit(*m_amount_, dispAmount);
    textAmount_->SetValue(dispAmount);

    return TRUE;
}

/*!
 * Control creation for SplitDetailDialog
 */

void SplitDetailDialog::CreateControls()
{    
////@begin SplitDetailDialog content construction
    // Generated by DialogBlocks, 04/24/07 21:52:26 (Personal Edition)

    SplitDetailDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    bCategory_ = new wxButton( itemDialog1, ID_BUTTONCATEGORY, m_categString_, 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemBoxSizer3->Add(bCategory_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Amount"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textAmount_ = new wxTextCtrl( itemDialog1, ID_TEXTCTRLAMOUNT, _T(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(textAmount_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_RIGHT|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemDialog1, ID_BUTTONOK, _("OK"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemButton8->SetDefault();
    itemBoxSizer7->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton9 = new wxButton( itemDialog1, ID_BUTTONCANCEL, _("Cancel"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(itemButton9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end SplitDetailDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONCATEGORY
 */

void SplitDetailDialog::OnButtonCategoryClick( wxCommandEvent& event )
{
    mmCategDialog *dlg = new mmCategDialog(core_, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        *m_categID_ = dlg->categID_;
        *m_subcategID_ = dlg->subcategID_;

        wxString catName = mmDBWrapper::getCategoryName(core_->db_.get(), dlg->categID_);
        catName.Replace (wxT("&"), wxT("&&"));
        m_categString_ = catName;

        if (dlg->subcategID_ != -1)
        {
            wxString subcatName = mmDBWrapper::getSubCategoryName(core_->db_.get(),
                dlg->categID_, dlg->subcategID_);
            subcatName.Replace (wxT("&"), wxT("&&"));
            m_categString_ += wxT(" : ");
            m_categString_ += subcatName;
        }

        bCategory_->SetLabel(m_categString_);
    }
    dlg->Destroy();
    event.Skip();
}

void SplitDetailDialog::OnButtonOKClick( wxCommandEvent& event )
{
    if (*m_categID_ == -1) 
    {
        mmShowErrorMessage(this, _("Invalid Category Entered "), _("Error"));
        return;
    }

    wxString amountStr = textAmount_->GetValue().Trim();
    double amount;
    if (!mmCurrencyFormatter::formatCurrencyToDouble(amountStr, amount) 
        || (amount < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
        return;
    }
    *m_amount_ = amount;

    EndModal(wxID_OK);
    event.Skip();
}

void SplitDetailDialog::OnButtonCancelClick( wxCommandEvent& event )
{
    EndModal(wxID_CANCEL);
    event.Skip();
}

bool SplitDetailDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap SplitDetailDialog::GetBitmapResource( const wxString& /*name*/ )
{
    // Bitmap retrieval
////@begin SplitDetailDialog bitmap retrieval
    return wxNullBitmap;
////@end SplitDetailDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon SplitDetailDialog::GetIconResource( const wxString& /*name*/ )
{
    // Icon retrieval
////@begin SplitDetailDialog icon retrieval
    return wxNullIcon;
////@end SplitDetailDialog icon retrieval
}

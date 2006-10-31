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

#include "univcsv.h"
#include "util.h"
#include "fileviewerdialog.h"

using namespace std;

IMPLEMENT_DYNAMIC_CLASS( mmUnivCSVImportDialog, wxDialog )


BEGIN_EVENT_TABLE( mmUnivCSVImportDialog, wxDialog )
    EVT_BUTTON(ID_UNIVCSVBUTTON_ADD, mmUnivCSVImportDialog::OnAdd)
    EVT_BUTTON(ID_UNIVCSVBUTTON_IMPORT, mmUnivCSVImportDialog::OnImport)
    EVT_BUTTON(ID_UNIVCSVBUTTON_REMOVE, mmUnivCSVImportDialog::OnRemove)
END_EVENT_TABLE()


mmUnivCSVImportDialog::mmUnivCSVImportDialog( )
{
}

mmUnivCSVImportDialog::mmUnivCSVImportDialog(wxSQLite3Database* db, wxWindow* parent, wxWindowID id, 
                   const wxString& caption, 
                   const wxPoint& pos, 
                   const wxSize& size, 
                   long style ) : db_ (db)
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * MyDialog creator
 */

bool mmUnivCSVImportDialog::Create(  wxWindow* parent, wxWindowID id, 
                      const wxString& caption, 
                      const wxPoint& pos, 
                      const wxSize& size, 
                      long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

     wxIcon icon(mainicon_xpm);
    SetIcon(icon);

    Centre();

    return TRUE;
}

/*!
 * Control creation for MyDialog
 */

void mmUnivCSVImportDialog::CreateControls()
{    
////@begin MyDialog content construction
    // Generated by DialogBlocks, 06/24/06 17:00:25 (Personal Edition)

    mmUnivCSVImportDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, _("Specify the order of fields in the CSV file"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxString* itemListBox4Strings = NULL;
    csvListBox_ = new wxListBox( itemDialog1, ID_LISTBOX, 
        wxDefaultPosition, wxDefaultSize, 0, itemListBox4Strings, wxLB_SINGLE );
    itemBoxSizer3->Add(csvListBox_, 1, wxGROW|wxALL, 1);

    wxPanel* itemPanel5 = new wxPanel( itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel5->SetSizer(itemBoxSizer6);

    wxButton* itemButton7 = new wxButton( itemPanel5, ID_UNIVCSVBUTTON_ADD, _("Add"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemPanel5, ID_UNIVCSVBUTTON_REMOVE, _("Remove"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton9 = new wxButton( itemPanel5, ID_UNIVCSVBUTTON_IMPORT, _("Import"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end MyDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool mmUnivCSVImportDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap mmUnivCSVImportDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin MyDialog bitmap retrieval
    return wxNullBitmap;
////@end MyDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon mmUnivCSVImportDialog::GetIconResource( const wxString& name )
{
    return wxNullIcon;
}

#define UNIV_CSV_DATE     0
#define UNIV_CSV_PAYEE    1
#define UNIV_CSV_AMOUNT   2
#define UNIV_CSV_CATEGORY 3
#define UNIV_CSV_SUBCATEGORY 4
#define UNIV_CSV_NOTES    5
#define UNIV_CSV_TRANSNUM 6
#define UNIV_CSV_DONTCARE 7
#define UNIV_CSV_WITHDRAWAL  8
#define UNIV_CSV_DEPOSIT  9

wxString getCSVFieldName(int index)
{
    switch (index)
    {
    case UNIV_CSV_DATE:
        return wxString(_("Date"));
    case UNIV_CSV_PAYEE:
        return wxString(_("Payee"));
    case UNIV_CSV_AMOUNT:
        return wxString(_("Amount(+/-)"));
    case UNIV_CSV_CATEGORY:
        return wxString(_("Category"));
    case UNIV_CSV_SUBCATEGORY:
        return wxString(_("SubCategory"));
    case UNIV_CSV_NOTES:
        return wxString(_("Notes"));
    case UNIV_CSV_TRANSNUM:
        return wxString(_("Transaction Number"));
    case UNIV_CSV_DONTCARE:
        return wxString(_("Don't Care"));
    case UNIV_CSV_WITHDRAWAL:
        return wxString(_("Withdrawal"));
    case UNIV_CSV_DEPOSIT:
        return wxString(_("Deposit"));


    default:
        return wxString(_("Unknown"));
    };
    return wxString(_("Unknown"));
}

void mmUnivCSVImportDialog::OnAdd(wxCommandEvent& event)
{
    wxArrayString csvArray;
    csvArray.Add(_("Date"));
    csvArray.Add(_("Payee"));
    csvArray.Add(_("Amount(+/-)"));
    csvArray.Add(_("Category"));
    csvArray.Add(_("SubCategory"));
    csvArray.Add(_("Notes"));
    csvArray.Add(_("Transaction Number"));
    csvArray.Add(_("Don't Care"));
    csvArray.Add(_("Withdrawal"));
    csvArray.Add(_("Deposit"));

    int index = wxGetSingleChoiceIndex(
                    _("Add CSV field"),
					_("CSV Field"),
					csvArray);

    if (index != -1)
    {
        csvListBox_->Insert(getCSVFieldName(index), csvFieldOrder_.size(), new mmCSVListBoxItem(index));
        csvFieldOrder_.push_back(index);
    }
}

bool mmUnivCSVImportDialog::isIndexPresent(int index)
{
    for (int i = 0; i < csvFieldOrder_.size(); i++)
    {
        if (csvFieldOrder_[i] == index)
            return true;
    }

    return false;
}

void mmUnivCSVImportDialog::OnImport(wxCommandEvent& event)
{
    if (csvFieldOrder_.size() < 3)
    {
         mmShowErrorMessage(0, 
            _("Incorrect fields specified for CSV import! Requires atleast date, amount and payee."),
            _("Error"));
         return;
    }

    // date, amount, payee are required
    if (!isIndexPresent(UNIV_CSV_DATE) || 
        !isIndexPresent(UNIV_CSV_PAYEE) ||
        (!isIndexPresent(UNIV_CSV_AMOUNT) && (!isIndexPresent(UNIV_CSV_WITHDRAWAL) || 
        !isIndexPresent(UNIV_CSV_DEPOSIT))))
    {
         mmShowErrorMessage(0, 
            _("Incorrect fields specified for CSV import! Requires atleast date, amount and payee."),
            _("Error"));
         return;
    }

    wxArrayString as;
    int fromAccountID = -1;

    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * from ACCOUNTLIST_V1 where ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }

    mmENDSQL_LITE_EXCEPTION

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), DEFDELIMTER);
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, _("Choose Account to import to:"), 
        _("CSV Import"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        wxString acctName = scd->GetStringSelection();
        fromAccountID = mmDBWrapper::getAccountID(db_, acctName);
        
        wxString fileName = wxFileSelector(_("Choose MM.NET CSV data file to import"), 
                wxT(""), wxT(""), wxT(""), wxT("*.csv"), wxFILE_MUST_EXIST);
        if ( !fileName.IsEmpty() )
        {
            wxFileInputStream input( fileName );
            wxTextInputStream text( input );

            /* Create Log File */
            wxFileName fname(wxTheApp->argv[0]);
            wxFileName csvName(fileName);
            wxString logFile = fname.GetPath(wxPATH_GET_VOLUME) + wxT("\\") 
                + csvName.GetName() + wxT(".txt");
            wxFileOutputStream outputLog( logFile );
            wxTextOutputStream log( outputLog );

            
            /* date, payeename, amount(+/-), Number, status, category : subcategory, notes */
            int countNumTotal = 0;
            int countImported = 0;
            while ( !input.Eof() )
            {
                wxString line = text.ReadLine();
                if (!line.IsEmpty())
                    countNumTotal++;
                else
                    continue;

                dt_ = wxT("");
                payee_ = wxT("");
                type_ = wxT("");
                amount_ = wxT("");
                categ_ = wxT("");
                subcateg_ = wxT("");
                transNum_ = wxT("");
                notes_ = wxT("");
                payeeID_ = -1;
                categID_ = -1;
                subCategID_ = -1;
                val_ = 0.0;

                wxStringTokenizer tkz(line, delimit, wxTOKEN_RET_EMPTY_ALL);  
                int numTokens = tkz.CountTokens();
                if (numTokens < csvFieldOrder_.size())
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" file contains insufficient number of tokens") << endl;
                    continue;
                }
                
                std::vector<wxString> tokens;
                while ( tkz.HasMoreTokens() )
                {
                    wxString token = tkz.GetNextToken();
                    tokens.push_back(mmCleanQuotes(token.Trim()));
                }


                for (int idx=0;idx<csvFieldOrder_.size(); idx++)
                {
                    if (tokens.size() < idx)
                        continue;
                    parseToken(csvFieldOrder_[idx], tokens[idx]);
                }

                if (dt_.Trim().IsEmpty() || payeeID_ == -1 ||
                    amount_.Trim().IsEmpty() ||  type_.Trim().IsEmpty())
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" One of the following fields: date, payee, amount, type is missing, skipping") << endl;
                    continue;
                }

                if (categID_ == -1)
                {
                    // see if payee already has category associated with it
                    mmDBWrapper::getPayee(db_, payeeID_, categID_, subCategID_);

                    if (categID_ == -1)
                    {
                        categID_ = mmDBWrapper::getCategoryID(db_, wxT("Unknown"));
                        if (categID_ == -1)
                        {
                            mmDBWrapper::addCategory(db_, wxT("Unknown"));
                            categID_ = mmDBWrapper::getCategoryID(db_, wxT("Unknown"));
                        }
                    }
                }

               wxString status = wxT("F");
               int toAccountID = -1;
               mmBEGINSQL_LITE_EXCEPTION;
               wxString bufSQL = wxString::Format(wxT("insert into CHECKINGACCOUNT_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, \
                    TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,                               \
                     CATEGID, SUBCATEGID, TRANSDATE)                                              \
                     values (%d, %d, %d, '%s', '%f', '%s', '%s', '%s', %d, %d, '%s');"),
                     fromAccountID, toAccountID, payeeID_, type_.c_str(), val_,
                     status.c_str(), transNum_.c_str(), notes_.c_str(), categID_, subCategID_, dt_.c_str() );  

               int retVal = db_->ExecuteUpdate(bufSQL);
               mmENDSQL_LITE_EXCEPTION;
               countImported++;
               log << _("Line : " ) << countNumTotal << _(" imported OK.") << endl;
            }

            wxString msg = wxString::Format(_("Total Lines : %d \nTotal Imported : %d\n\nLog file written to : %s.\n\nImported transactions have been flagged so you can review them. "), countNumTotal, countImported, logFile.c_str());
            mmShowErrorMessage(0, msg, _("Import from CSV"));
            outputLog.Close();

            fileviewer* dlg = new fileviewer(logFile, 0);
            dlg->ShowModal();
            dlg->Destroy();

            
        }
    }
    scd->Destroy();
    Close();
}

void mmUnivCSVImportDialog::OnRemove(wxCommandEvent& event)
{
    int selIndex = csvListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND)
    {
        csvListBox_->Delete(selIndex);
        csvFieldOrder_.erase(csvFieldOrder_.begin() + selIndex);
    }
}

void mmUnivCSVImportDialog::parseToken(int index, wxString& token)
{
    switch (index)
    {
    case UNIV_CSV_DATE:
        {
            if (token.Trim().IsEmpty())
                return;

            wxDateTime dtdt = mmParseDisplayStringToDate(db_, token);
            dt_ = dtdt.FormatISODate();
            break;
        }

    case UNIV_CSV_PAYEE:
        {
            if (token.Trim().IsEmpty())
                return;

            if (!mmDBWrapper::getPayeeID(db_, token, payeeID_, categID_, subCategID_))
            {
                mmDBWrapper::addPayee(db_, token, -1, -1);
                mmDBWrapper::getPayeeID(db_, token, payeeID_, categID_, subCategID_);
            }
            break;
        }

    case UNIV_CSV_AMOUNT:
        {
            if (token.Trim().IsEmpty())
                return;
           
            if (!token.ToDouble(&val_))
            {
                return;
            }

            if (val_ <= 0.0)
                type_ = wxT("Withdrawal");
            else
                type_ = wxT("Deposit");

            val_ = fabs(val_);
            amount_ = token;

            break;
        }

    case UNIV_CSV_CATEGORY:
        {
               if (token.Trim().IsEmpty())
               {
                 return;
               }
                
               categID_ = mmDBWrapper::getCategoryID(db_, token);
               if (categID_ == -1)
               {
                  mmDBWrapper::addCategory(db_, token);
                  categID_ = mmDBWrapper::getCategoryID(db_, token);
               }
               break;
        }

    case UNIV_CSV_SUBCATEGORY:
        {
            if (token.Trim().IsEmpty() || (categID_ == -1))
            {
                return;
            }
    
            subCategID_ = mmDBWrapper::getSubCategoryID(db_, categID_, token);
            if (subCategID_ == -1)
            {
                mmDBWrapper::addSubCategory(db_, categID_, token);
                subCategID_ = mmDBWrapper::getSubCategoryID(db_, categID_, token);
            }
            break;
        }

    case UNIV_CSV_NOTES:
        {
            if (token.Trim().IsEmpty())
            {
                return;
            }

            notes_ = token;
            break;
        }

    case UNIV_CSV_TRANSNUM:
        {
            if (token.Trim().IsEmpty())
            {
                return;
            }

            transNum_ = token;
            break;
        }

    case UNIV_CSV_DONTCARE:
        {
            // do nothing
            break;
        }
    
    case UNIV_CSV_DEPOSIT:
        {
            if (token.Trim().IsEmpty())
                return;
           
            if (!token.ToDouble(&val_))
            {
                return;
            }

            if (val_ <= 0.0)
                return;

            type_ = wxT("Deposit");
             amount_ = token;
            break;
        }

    case UNIV_CSV_WITHDRAWAL:
        {
            if (token.Trim().IsEmpty())
                return;
           
            if (!token.ToDouble(&val_))
            {
                return;
            }

            if (val_ <= 0.0)
                return;

            type_ = wxT("Withdrawal");
            amount_ = token;
            break;
        }


    default:
        {
            wxASSERT(true);
        }
    }
}

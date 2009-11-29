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

#include "univcsvdialog.h"
#include "util.h"
#include "fileviewerdialog.h"
#include "defs.h"
#include "paths.h"
#include "platfdep.h"
#include "helpers.h"

IMPLEMENT_DYNAMIC_CLASS( mmUnivCSVImportDialog, wxDialog )


BEGIN_EVENT_TABLE( mmUnivCSVImportDialog, wxDialog )
    EVT_BUTTON(ID_UNIVCSVBUTTON_ADD, mmUnivCSVImportDialog::OnAdd)
    EVT_BUTTON(ID_UNIVCSVBUTTON_IMPORT, mmUnivCSVImportDialog::OnImport)
    EVT_BUTTON(ID_UNIVCSVBUTTON_REMOVE, mmUnivCSVImportDialog::OnRemove)
    EVT_BUTTON(ID_UNIVCSVBUTTON_LOAD, mmUnivCSVImportDialog::OnLoad)
    EVT_BUTTON(ID_UNIVCSVBUTTON_SAVE, mmUnivCSVImportDialog::OnSave)
	EVT_BUTTON(ID_UNIVCSVBUTTON_MOVEUP, mmUnivCSVImportDialog::OnMoveUp)
	EVT_BUTTON(ID_UNIVCSVBUTTON_MOVEDOWN, mmUnivCSVImportDialog::OnMoveDown)
END_EVENT_TABLE()

//----------------------------------------------------------------------------

namespace
{

enum EUnivCvs
{
    UNIV_CSV_DATE,
    UNIV_CSV_PAYEE,
    UNIV_CSV_AMOUNT,
    UNIV_CSV_CATEGORY,
    UNIV_CSV_SUBCATEGORY,
    UNIV_CSV_NOTES,
    UNIV_CSV_TRANSNUM,
    UNIV_CSV_DONTCARE,
    UNIV_CSV_WITHDRAWAL,
    UNIV_CSV_DEPOSIT
};
//----------------------------------------------------------------------------

wxString mmCleanQuotes(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("'"), wxT("`"));
    toReturn.Replace(wxT("\""), wxGetEmptyString());
    toReturn.Trim();
    return toReturn; 
}

} // namespace

//----------------------------------------------------------------------------

mmUnivCSVImportDialog::mmUnivCSVImportDialog( )
{
}

mmUnivCSVImportDialog::mmUnivCSVImportDialog(mmCoreDB* core, 
                   wxWindow* parent, wxWindowID id, 
                   const wxString& caption, 
                   const wxPoint& pos, 
                   const wxSize& size, 
                   long style ) : db_ (core->db_.get()), core_(core)
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

    SetIcon(mmex::getProgramIcon());

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

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, 
       _("Specify the order of fields in the CSV file"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

	//Arranger Area
	wxPanel* itemPanel_Arranger = new wxPanel( itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel_Arranger, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer_Arranger = new wxBoxSizer(wxVERTICAL);
    itemPanel_Arranger->SetSizer(itemBoxSizer_Arranger);

	//Move Up button
    wxButton* itemButton_MoveUp = new wxButton( itemPanel_Arranger, ID_UNIVCSVBUTTON_MOVEUP, _("Move &Up"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer_Arranger->Add(itemButton_MoveUp, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//Move down button
    wxButton* itemButton_MoveDown = new wxButton( itemPanel_Arranger, ID_UNIVCSVBUTTON_MOVEDOWN, _("Move &Down"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer_Arranger->Add(itemButton_MoveDown, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//ListBox of attribute order
    wxString* itemListBox4Strings = NULL;
    csvListBox_ = new wxListBox( itemDialog1, ID_LISTBOX, 
        wxDefaultPosition, wxDefaultSize, 0, itemListBox4Strings, wxLB_SINGLE );
    itemBoxSizer3->Add(csvListBox_, 1, wxGROW|wxALL, 1);

	//Add Remove Area
	wxPanel* itemPanel_AddRemove = new wxPanel( itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel_AddRemove, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer_AddRemove = new wxBoxSizer(wxVERTICAL);
    itemPanel_AddRemove->SetSizer(itemBoxSizer_AddRemove);

	//Add button
    wxButton* itemButton_Add = new wxButton( itemPanel_AddRemove, ID_UNIVCSVBUTTON_ADD, _("&Add"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer_AddRemove->Add(itemButton_Add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//Remove button
    wxButton* itemButton_Remove = new wxButton( itemPanel_AddRemove, ID_UNIVCSVBUTTON_REMOVE, _("&Remove"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer_AddRemove->Add(itemButton_Remove, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxPanel* itemPanel5 = new wxPanel( itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel5->SetSizer(itemBoxSizer6);

	//Load Template button
    wxButton* itemButton1_Load = new wxButton( itemPanel5, ID_UNIVCSVBUTTON_LOAD, _("&Load Template"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton1_Load, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//Save As Template button
    wxButton* itemButton_Save = new wxButton( itemPanel5, ID_UNIVCSVBUTTON_SAVE, _("Save &As Template"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton_Save, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//Import File button
    wxButton* itemButton_Import = new wxButton( itemPanel5, ID_UNIVCSVBUTTON_IMPORT, _("&Import File"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemButton_Import, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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

wxBitmap mmUnivCSVImportDialog::GetBitmapResource( const wxString& /*name*/ )
{
    // Bitmap retrieval
////@begin MyDialog bitmap retrieval
    return wxNullBitmap;
////@end MyDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon mmUnivCSVImportDialog::GetIconResource( const wxString& /*name*/ )
{
    return wxNullIcon;
}

wxString getCSVFieldName(int index)
{
    wxString s;
    
    switch (index)
    {
    case UNIV_CSV_DATE:
        s = _("Date");
        break;
    
    case UNIV_CSV_PAYEE:
        s = _("Payee");
        break;
    
    case UNIV_CSV_AMOUNT:
        s = _("Amount(+/-)");
        break;
    
    case UNIV_CSV_CATEGORY:
        s = _("Category");
        break;
    
    case UNIV_CSV_SUBCATEGORY:
        s = _("SubCategory");
        break;
    
    case UNIV_CSV_NOTES:
        s = _("Notes");
        break;
    
    case UNIV_CSV_TRANSNUM:
        s = _("Transaction Number");
        break;
    
    case UNIV_CSV_DONTCARE:
        s = _("Don't Care");
        break;
    
    case UNIV_CSV_WITHDRAWAL:
        s = _("Withdrawal");
        break;
    
    case UNIV_CSV_DEPOSIT:
        s = _("Deposit");
        break;
    
    default:
        s = _("Unknown");
    };

    return s;
}

//Selection dialog for fields to be added to listbox
void mmUnivCSVImportDialog::OnAdd(wxCommandEvent& /*event*/)
{
    wxArrayString csvArray;
	wxArrayInt csvArrayLocation;
	int i = 0;
	for(i=0;i<10;i++){
		//check if the field is already selected unless it is "Don't Care"
		//multiple fields of "Don't Care" may be necessary
		//the code for "Don't Care" is 7
        std::vector<int>::iterator loc = find(csvFieldOrder_.begin(), csvFieldOrder_.end(), i);
		if( loc == csvFieldOrder_.end() || i == 7 ){
			csvArray.Add((getCSVFieldName(i)));
			csvArrayLocation.Add((i));
		}
	}

    int index = wxGetSingleChoiceIndex(
                    _("Add CSV field"),
					_("CSV Field"),
					csvArray);

    if (index != -1)
    {
        csvListBox_->Insert(getCSVFieldName(csvArrayLocation[index]), 
           (int)csvFieldOrder_.size(), 
           new mmCSVListBoxItem(csvArrayLocation[index]));
        csvFieldOrder_.push_back(csvArrayLocation[index]);
    }
}

bool mmUnivCSVImportDialog::isIndexPresent(int index)
{
    for (int i = 0; i < (int)csvFieldOrder_.size(); i++)
    {
        if (csvFieldOrder_[i] == index)
            return true;
    }

    return false;
}

void mmUnivCSVImportDialog::OnLoad(wxCommandEvent& /*event*/)
{
   wxString fileName = wxFileSelector(wxT("Choose Universal CSV format file to load"), 
      wxGetEmptyString(), wxGetEmptyString(), wxGetEmptyString(),  wxT("CSV Template(*.mcv)|*.mcv"), wxFILE_MUST_EXIST);
   if ( !fileName.empty() )
   {
      wxTextFile tFile(fileName);
      if (!tFile.Open())
      {
         mmShowErrorMessage(0, 
            _("Unable to open file."),
            _("Error"));
         return;
      }
      csvFieldOrder_.clear();
      csvListBox_->Clear();

      wxString str;
      for ( str = tFile.GetFirstLine(); !tFile.Eof(); str = tFile.GetNextLine() )
      {
         long num = 0;
         if (str.ToLong(&num))
         {
            csvListBox_->Insert(getCSVFieldName(num), 
               (int)csvFieldOrder_.size(), 
               new mmCSVListBoxItem(num));
            csvFieldOrder_.push_back(num);
         }
      }

      tFile.Write();
      tFile.Close();
   }
}

//Saves the field order to a template file
void mmUnivCSVImportDialog::OnSave(wxCommandEvent& /*event*/)
{
     wxString fileName = wxFileSelector(wxT("Choose Universal CSV format file to save"), 
                wxGetEmptyString(), wxGetEmptyString(), wxGetEmptyString(), wxT("CSV Template(*.mcv)|*.mcv"), wxSAVE);
    if ( !fileName.empty() )
    {
         wxTextFile tFile(fileName);
		 //if the file does not exist and cannot be created, throw an error
		 //if the file does exist, then skip to else section
         if ( !tFile.Exists() && !tFile.Create() )
         {
            mmShowErrorMessage(0, 
               _("Unable to write to file."),
               _("Error"));
            return;
         }
		 else{
			 //clear the contents of the current file
			 tFile.Clear();
			 for (int idx = 0; idx < (int) csvFieldOrder_.size(); idx++)
			 {
				wxString line = wxString::Format(wxT("%d"), csvFieldOrder_[idx]);
				tFile.AddLine(line);
			 }
		 }
         tFile.Write();
         tFile.Close();
    }
}

void mmUnivCSVImportDialog::OnImport(wxCommandEvent& /*event*/)
{
    if (csvFieldOrder_.size() < 3)
    {
         mmShowErrorMessage(0, 
            _("Incorrect fields specified for CSV import! Requires at least Date, Amount and Payee."),
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
            _("Incorrect fields specified for CSV import! Requires at least Date, Amount and Payee."),
            _("Error"));
         return;
    }

    wxArrayString as;
    int fromAccountID = -1;

    static const char sql[] = 
    "select ACCOUNTNAME "
    "from ACCOUNTLIST_V1 "
    "where ACCOUNTTYPE = 'Checking' "
    "order by ACCOUNTNAME";
    
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    q1.Finalize();

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), mmex::DEFDELIMTER);
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, _("Choose Account to import to:"), 
        _("CSV Import"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        wxString acctName = scd->GetStringSelection();
        fromAccountID = mmDBWrapper::getAccountID(db_, acctName);

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
        wxASSERT(pCurrencyPtr);
        mmCurrencyFormatter::loadSettings(pCurrencyPtr);
             
        wxString fileName = wxFileSelector(_("Choose MM.NET CSV data file to import"), 
                wxGetEmptyString(), wxGetEmptyString(), wxGetEmptyString(), wxT("*.csv"), wxFILE_MUST_EXIST);
        if ( !fileName.IsEmpty() )
        {
            wxFileInputStream input(fileName);
            wxTextInputStream text(input);

            wxFileName logFile = mmex::GetLogDir(true);
            logFile.SetFullName(fileName);
            logFile.SetExt(wxT(".txt"));

            wxFileOutputStream outputLog(logFile.GetFullPath());
            wxTextOutputStream log(outputLog);

            
            /* date, payeename, amount(+/-), Number, status, category : subcategory, notes */
            int countNumTotal = 0;
            int countImported = 0;
            while ( !input.Eof() )
            {
                wxString line = text.ReadLine();
                if (!line.IsEmpty())
                    ++countNumTotal;
                else
                    continue;

                dt_.clear();
                payee_.clear();
                type_.clear();
                amount_.clear();
                categ_.clear();
                subcateg_.clear();
                transNum_.clear();
                notes_.clear();
                payeeID_ = -1;
                categID_ = -1;
                subCategID_ = -1;
                val_ = 0.0;

                wxStringTokenizer tkz(line, delimit, wxTOKEN_RET_EMPTY_ALL);  
                int numTokens = (int)tkz.CountTokens();
                if (numTokens < (int)csvFieldOrder_.size())
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


                for (size_t i = 0; i < csvFieldOrder_.size(); ++i)
                {
                    if (tokens.size() >= i) {
                        parseToken(csvFieldOrder_[i], tokens[i]);
                    }
                }

                if (dt_.Trim().IsEmpty() || payeeID_ == -1 ||
                    amount_.Trim().IsEmpty() ||  type_.Trim().IsEmpty())
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" One of the following fields: Date, Payee, Amount, Type is missing, skipping") << endl;
                    continue;
                }

                if (categID_ == -1)
                {
                   boost::shared_ptr<mmPayee> pPayee =  core_->payeeList_.getPayeeSharedPtr(payeeID_);
                   boost::shared_ptr<mmCategory> pCategory = pPayee->category_.lock();
                   if (!pCategory)
                   {
                       subCategID_ = -1;
                       wxString categ = wxT("Unknown");

                       categID_ = core_->categoryList_.getCategoryID(categ);
                       if (categID_ == -1)
                       {
                           categID_ =  core_->categoryList_.addCategory(categ);
                       }
                   }
                   else
                   {
                       if (pCategory->parent_.lock())
                       {
                           categID_ = pCategory->parent_.lock()->categID_;
                           subCategID_ = pCategory->categID_; 
                       }
                       else
                       {
                           categID_ = pCategory->categID_; 
                           subCategID_ = -1;
                       }
                   }
                }

               wxString status = wxT("F");
               int toAccountID = -1;

               boost::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core_->db_));
               pTransaction->accountID_ = fromAccountID;
               pTransaction->toAccountID_ = toAccountID;
               pTransaction->payee_ = core_->payeeList_.getPayeeSharedPtr(payeeID_);
               pTransaction->transType_ = type_;
               pTransaction->amt_ = val_;
               pTransaction->status_ = status;
               pTransaction->transNum_ = transNum_;
               pTransaction->notes_ = notes_;
               pTransaction->category_ = core_->categoryList_.getCategorySharedPtr(categID_, subCategID_);
               pTransaction->date_ = dtdt_;
               pTransaction->toAmt_ = 0.0;
               pTransaction->updateAllData(core_, fromAccountID, pCurrencyPtr);

               core_->bTransactionList_.addTransaction(core_, pTransaction);

               countImported++;
               log << _("Line : " ) << countNumTotal << _(" imported OK.") << endl;
            }

            wxString msg = wxString::Format(_("Total Lines : %d \nTotal Imported : %d\n\nLog file written to : %s.\n\nImported transactions have been flagged so you can review them. "), countNumTotal, countImported, logFile.GetFullPath().c_str());
            mmShowErrorMessage(0, msg, _("Import from CSV"));
            outputLog.Close();

            boost::shared_ptr<fileviewer> dlg(new fileviewer(logFile.GetFullPath(), 0), mmex::Destroy);
            dlg->ShowModal();
        }
    }
    scd->Destroy();
    Close();
}

//Removes an item from the field list box
void mmUnivCSVImportDialog::OnRemove(wxCommandEvent& /*event*/)
{
    int selIndex = csvListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND)
    {
        csvListBox_->Delete(selIndex);
        csvFieldOrder_.erase(csvFieldOrder_.begin() + selIndex);

        // select the next item after the one that was deleted so a user 
		// can quickly hit Remove to delete fields
		// check if the selected index is the last item
        if (csvFieldOrder_.size())
        {
            if( selIndex > (int)csvFieldOrder_.size() - 1)
            {
                csvListBox_->SetSelection((int)csvFieldOrder_.size() - 1, true);
            }
            else
            {
                //if the selected item is the last one, then 
                // just select the last item in the list
                csvListBox_->SetSelection(selIndex, true);

            }
        }
    }
}

void mmUnivCSVImportDialog::OnMoveUp(wxCommandEvent& /*event*/)
{
    int selIndex = csvListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND && selIndex != 0)
    {
		//reorder the attributes (description string) in the list box
		//source = the selected place in the list (place to be moved up)
		//destination = the place in the list to be replaced (moved down)
		//replace the source with destination
		csvListBox_->Delete(selIndex);
		csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex - 1)), selIndex, 
            new mmCSVListBoxItem(csvFieldOrder_.at(selIndex - 1)));

		//replace the destination with source
		csvListBox_->Delete(selIndex - 1);
		csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex)), selIndex - 1, 
            new mmCSVListBoxItem(csvFieldOrder_.at(selIndex)));

		//reselect the source
		csvListBox_->SetSelection(selIndex - 1, true);

		//reorder the attribute list in the vector
		//get the source field number
		int srcFieldNumber = csvFieldOrder_.at(selIndex);
		//replace the source with destination
		csvFieldOrder_.at(selIndex) = csvFieldOrder_.at(selIndex - 1);
		//replace the destination with source
		csvFieldOrder_.at(selIndex - 1) = srcFieldNumber;
    }
}

void mmUnivCSVImportDialog::OnMoveDown(wxCommandEvent& /*event*/)
{
    int selIndex = csvListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND && selIndex != static_cast<int>(csvFieldOrder_.size()) - 1)
    {
		//reorder the attributes (description string) in the list box
		//source = the selected place in the list (place to be moved up)
		//destination = the place in the list to be replaced (moved down)
		//replace the source with destination
		csvListBox_->Delete(selIndex);
		csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex + 1)), selIndex, 
            new mmCSVListBoxItem(csvFieldOrder_.at(selIndex + 1)));

		//replace the destination with source
		csvListBox_->Delete(selIndex + 1);
		csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex)), selIndex + 1, 
            new mmCSVListBoxItem(csvFieldOrder_.at(selIndex)));

		//reselect the source
		csvListBox_->SetSelection(selIndex + 1, true);

		//reorder the attribute list in the vector
		//get the source field number
		int srcFieldNumber = csvFieldOrder_.at(selIndex);
		//replace the source with destination
		csvFieldOrder_.at(selIndex) = csvFieldOrder_.at(selIndex + 1);

		//replace the destination with source
		csvFieldOrder_.at(selIndex + 1) = srcFieldNumber;

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

            dtdt_ = mmParseDisplayStringToDate(db_, token);
            dt_ = dtdt_.FormatISODate();
            break;
        }

    case UNIV_CSV_PAYEE:
        {
            if (token.Trim().IsEmpty())
                return;

            if (!core_->payeeList_.payeeExists(token))
            {
                payeeID_ = core_->payeeList_.addPayee(token);

            }
            else
            {
                payeeID_ = core_->payeeList_.getPayeeID(token);
            }
            break;
        }

    case UNIV_CSV_AMOUNT:
        {
            if (token.Trim().IsEmpty())
                return;
           
            
            if (!mmCurrencyFormatter::formatCurrencyToDouble(token, val_))
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
                
                categID_ = core_->categoryList_.getCategoryID(token);
                if (categID_ == -1)
                {
                    categID_ =  core_->categoryList_.addCategory(token);
                }

               break;
        }

    case UNIV_CSV_SUBCATEGORY:
        {
            if (token.Trim().IsEmpty() || (categID_ == -1))
            {
                return;
            }
    
            subCategID_ = core_->categoryList_.getSubCategoryID(categID_, token);
            if (subCategID_ == -1)
            {
                subCategID_ = core_->categoryList_.addSubCategory(categID_, token);
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

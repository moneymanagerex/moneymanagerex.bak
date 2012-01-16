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

IMPLEMENT_DYNAMIC_CLASS(mmUnivCSVImportDialog, wxDialog)


BEGIN_EVENT_TABLE(mmUnivCSVImportDialog, wxDialog)
    EVT_BUTTON(wxID_ADD, mmUnivCSVImportDialog::OnAdd)
    EVT_BUTTON(ID_UNIVCSVBUTTON_IMPORT, mmUnivCSVImportDialog::OnImport)
    EVT_BUTTON(wxID_REMOVE, mmUnivCSVImportDialog::OnRemove)
    EVT_BUTTON(wxID_OPEN, mmUnivCSVImportDialog::OnLoad)
    EVT_BUTTON(wxID_SAVEAS, mmUnivCSVImportDialog::OnSave)
    EVT_BUTTON(wxID_UP, mmUnivCSVImportDialog::OnMoveUp)
    EVT_BUTTON(wxID_DOWN, mmUnivCSVImportDialog::OnMoveDown)
    EVT_LISTBOX(ID_LISTBOX, mmUnivCSVImportDialog::OnListBox)
END_EVENT_TABLE()

//----------------------------------------------------------------------------

namespace
{

enum EUnivCvs
{
    UNIV_CSV_DATE = 0,
    UNIV_CSV_PAYEE,
    UNIV_CSV_AMOUNT,
    UNIV_CSV_CATEGORY,
    UNIV_CSV_SUBCATEGORY,
    UNIV_CSV_NOTES,
    UNIV_CSV_TRANSNUM,
    UNIV_CSV_DONTCARE,
    UNIV_CSV_WITHDRAWAL,
    UNIV_CSV_DEPOSIT,
    UNIV_CSV_LAST
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

mmUnivCSVImportDialog::mmUnivCSVImportDialog()
{
}

mmUnivCSVImportDialog::mmUnivCSVImportDialog(
    mmCoreDB* core, 
    wxWindow* parent, wxWindowID id, 
    const wxString& caption, 
    const wxPoint& pos, 
    const wxSize& size, 
    long style
) : 
    core_(core),
    db_ (core->db_.get())
{
    CSVFieldName_[UNIV_CSV_DATE] = _("Date");
    CSVFieldName_[UNIV_CSV_PAYEE] = _("Payee");
    CSVFieldName_[UNIV_CSV_AMOUNT] = _("Amount(+/-)");
    CSVFieldName_[UNIV_CSV_CATEGORY] = _("Category");
    CSVFieldName_[UNIV_CSV_SUBCATEGORY] = _("SubCategory");
    CSVFieldName_[UNIV_CSV_NOTES] = _("Notes");
    CSVFieldName_[UNIV_CSV_TRANSNUM] = _("Transaction Number");
    CSVFieldName_[UNIV_CSV_DONTCARE] = _("Don't Care");
    CSVFieldName_[UNIV_CSV_WITHDRAWAL] = _("Withdrawal");
    CSVFieldName_[UNIV_CSV_DEPOSIT] = _("Deposit");

    Create(parent, id, caption, pos, size, style);
}

bool mmUnivCSVImportDialog::Create(wxWindow* parent, wxWindowID id, 
                      const wxString& caption, 
                      const wxPoint& pos, 
                      const wxSize& size, 
                      long style)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create(parent, id, caption, pos, size, style);

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    Centre();

    return TRUE;
}

void mmUnivCSVImportDialog::CreateControls()
{    
    mmUnivCSVImportDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText(itemDialog1, wxID_STATIC, 
       _("Specify the order of fields in the CSV file"), wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

     //Add Remove Area
    wxPanel* itemPanel_AddRemove = new wxPanel(itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer3->Add(itemPanel_AddRemove, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer_AddRemove = new wxBoxSizer(wxVERTICAL);
    itemPanel_AddRemove->SetSizer(itemBoxSizer_AddRemove);

    //Add button
    wxButton* itemButton_Add = new wxButton(itemPanel_AddRemove, wxID_ADD, _("Add"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_AddRemove->Add(itemButton_Add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //Remove button
    wxButton* itemButton_Remove = new wxButton(itemPanel_AddRemove, wxID_REMOVE, _("Remove"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_AddRemove->Add(itemButton_Remove, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxPanel* itemPanel5 = new wxPanel(itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer2->Add(itemPanel5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel5->SetSizer(itemBoxSizer6);
   
    //ListBox of attribute order
    csvListBox_ = new wxListBox(itemDialog1, ID_LISTBOX, 
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_NEEDED_SB);
    itemBoxSizer3->Add(csvListBox_, 1, wxGROW|wxALL, 1);

   //Arranger Area
    wxPanel* itemPanel_Arranger = new wxPanel(itemDialog1, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer3->Add(itemPanel_Arranger, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer_Arranger = new wxBoxSizer(wxVERTICAL);
    itemPanel_Arranger->SetSizer(itemBoxSizer_Arranger);

    //Move Up button
    wxButton* itemButton_MoveUp = new wxButton(itemPanel_Arranger, wxID_UP, _("&Up"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_Arranger->Add(itemButton_MoveUp, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_MoveUp -> SetToolTip (_("Move Up"));

    //Move down button
    wxButton* itemButton_MoveDown = new wxButton(itemPanel_Arranger, wxID_DOWN, _("&Down"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_Arranger->Add(itemButton_MoveDown, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_MoveDown -> SetToolTip (_("Move &Down"));
 
    //Load Template button
    wxButton* itemButton_Load = new wxButton(itemPanel5, wxID_OPEN, _("&Open"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer6->Add(itemButton_Load, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_Load -> SetToolTip (_("Load Template"));

    //Save As Template button
    wxButton* itemButton_Save = new wxButton(itemPanel5, wxID_SAVEAS, _("Save &As..."), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer6->Add(itemButton_Save, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_Save -> SetToolTip (_("Save Template"));

    //Import File button
    wxButton* itemButton_Import = new wxButton(itemPanel5, ID_UNIVCSVBUTTON_IMPORT, _("&Import"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer6->Add(itemButton_Import, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_Import -> SetToolTip (_("Import File"));

    wxBoxSizer* itemBoxSizer66 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer6->Add(itemBoxSizer66, 1, wxGROW|wxALL, 5);
    
    wxButton* itemCancelButton = new wxButton(itemPanel5, wxID_CANCEL, _("&Cancel"));
    itemBoxSizer66->Add(itemCancelButton);
    itemCancelButton->SetFocus();
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

wxBitmap mmUnivCSVImportDialog::GetBitmapResource(const wxString& /*name*/)
{
    return wxNullBitmap;
}

/*!
 * Get icon resources
 */

wxIcon mmUnivCSVImportDialog::GetIconResource(const wxString& /*name*/)
{
    return wxNullIcon;
}

//Selection dialog for fields to be added to listbox
void mmUnivCSVImportDialog::OnAdd(wxCommandEvent& /*event*/)
{
    wxArrayString csvArray;
    wxArrayInt csvArrayLocation;
    for(int i = 0; i < UNIV_CSV_LAST; i++)
    {
        //check if the field is already selected unless it is "Don't Care"
        //multiple fields of "Don't Care" may be necessary
        std::vector<int>::const_iterator loc = find(csvFieldOrder_.begin(), csvFieldOrder_.end(), i);
        if(loc == csvFieldOrder_.end() || i == UNIV_CSV_DONTCARE)
        {
            csvArray.Add((getCSVFieldName(i)));
            csvArrayLocation.Add((i));
        }
    }

    int index = wxGetSingleChoiceIndex(_("Add CSV field"), _("CSV Field"), csvArray);

    if (index != -1)
    {
        csvListBox_->Append(getCSVFieldName(csvArrayLocation[index]));
        csvFieldOrder_.push_back(csvArrayLocation[index]);
    }
}

bool mmUnivCSVImportDialog::isIndexPresent(int index) const
{
    for(std::vector<int>::const_iterator it = csvFieldOrder_.begin(); it != csvFieldOrder_.end(); ++ it)
    {
        if (*it == index) return true;
    }

    return false;
}

const wxString mmUnivCSVImportDialog::getCSVFieldName(int index) const
{
    std::map<int, wxString>::const_iterator it = CSVFieldName_.find(index);
    if (it != CSVFieldName_.end())
        return it->second;

    return _("Unknown");
}

void mmUnivCSVImportDialog::OnLoad(wxCommandEvent& /*event*/)
{
   wxString fileName = wxFileSelector(_("Choose Universal CSV format file to load"), 
      wxEmptyString, wxEmptyString, wxEmptyString,  wxT("CSV Template(*.mcv)|*.mcv"), wxFD_FILE_MUST_EXIST);
   if (!fileName.empty())
   {
      wxTextFile tFile(fileName);
      if (!tFile.Open())
      {
         wxMessageBox(_("Unable to open file."), _("Universal CSV Import"), wxICON_WARNING);
         return;
      }
      csvFieldOrder_.clear();
      csvListBox_->Clear();

      wxString str;
      for (str = tFile.GetFirstLine(); !tFile.Eof(); str = tFile.GetNextLine())
      {
         long num = 0;
         if (str.ToLong(&num))
         {
            csvListBox_->Append(getCSVFieldName(num));
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
    wxString fileName = wxFileSelector(_("Choose Universal CSV format file to save"), 
                wxEmptyString, wxEmptyString, wxEmptyString, wxT("CSV Template(*.mcv)|*.mcv"), wxFD_SAVE);
    if (!fileName.empty())
    {
        correctEmptyFileExt(wxT("mcv"),fileName);

        wxTextFile tFile(fileName);
        //if the file does not exist and cannot be created, throw an error
        //if the file does exist, then skip to else section
        if (!tFile.Exists() && !tFile.Create())
        {
            wxMessageBox(_("Unable to write to file."), _("Universal CSV Import"), wxICON_WARNING);
            return;
        }
        else
        {
            //clear the contents of the current file
            tFile.Clear();
            for (std::vector<int>::const_iterator it = csvFieldOrder_.begin(); it != csvFieldOrder_.end(); ++ it)
            {
                wxString line = wxString::Format(wxT("%d"), *it);
                tFile.AddLine(line);
            }
        }
        tFile.Write();
        tFile.Close();
    }
}

void mmUnivCSVImportDialog::OnImport(wxCommandEvent& /*event*/)
{
    // date, amount, payee are required
    if (!isIndexPresent(UNIV_CSV_DATE) || 
        !isIndexPresent(UNIV_CSV_PAYEE) ||
        (!isIndexPresent(UNIV_CSV_AMOUNT) && (!isIndexPresent(UNIV_CSV_WITHDRAWAL) || 
        !isIndexPresent(UNIV_CSV_DEPOSIT))))
    {
         wxMessageBox(_("Incorrect fields specified for CSV import! Requires at least Date, Amount and Payee."),
                      _("Universal CSV Import"), wxICON_WARNING);
         return;
    }

    wxArrayString as = mmDBWrapper::getAccountsName(db_);
    int fromAccountID = -1;

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), mmex::DEFDELIMTER);
    
    wxSingleChoiceDialog scd(0, _("Choose Account to import to:"), _("Universal CSV Import"), as);
    if (scd.ShowModal() == wxID_OK)
    {
        wxString acctName = scd.GetStringSelection();
        fromAccountID = mmDBWrapper::getAccountID(db_, acctName);

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
             
        wxString fileName = wxFileSelector(_("Choose MM.NET CSV data file to import"), 
                wxEmptyString, wxEmptyString, wxEmptyString, wxT("*.csv"), wxFD_FILE_MUST_EXIST);
        if (!fileName.IsEmpty())
        {
            wxFileInputStream input(fileName);
            wxTextInputStream text(input);

            wxFileName logFile = mmex::GetLogDir(true);
            logFile.SetFullName(fileName);
            logFile.SetExt(wxT("txt"));

            wxFileOutputStream outputLog(logFile.GetFullPath());
            wxTextOutputStream log(outputLog);
            
            /* date, payeename, amount(+/-), Number, status, category : subcategory, notes */
            int countNumTotal = 0;
            int countImported = 0;

            bool canceledbyuser = false;
            std::vector<int> CSV_transID;

            wxProgressDialog progressDlg(_("Universal CSV Import"), _("Transactions imported from CSV: "), 100,
                NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_CAN_ABORT);
            mmDBWrapper::begin(db_);

            while (!input.Eof())
            {
                wxString progressMsg;
                progressMsg << _("Transactions imported from CSV\nto account ") << acctName << wxT(": ") << countImported;
                progressDlg.Update(static_cast<int>((static_cast<double>(countImported)/100.0 - countNumTotal/100) *99), progressMsg);

                if (!progressDlg.Update(-1)) // if cancel clicked
                {
                    canceledbyuser = true;
                    break; // abort processing
                }

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
                    log << _("Line : ") << countNumTotal 
                        << _(" file contains insufficient number of tokens") << endl;
                    continue;
                }
                
                std::vector<wxString> tokens;
                while (tkz.HasMoreTokens())
                {
                    wxString token = tkz.GetNextToken();
                    tokens.push_back(mmCleanQuotes(token.Trim()));
                }

                for (size_t i = 0; i < csvFieldOrder_.size(); ++i)
                {
                    if (tokens.size() >= i) 
                    {
                        parseToken(csvFieldOrder_[i], tokens[i]);
                    }
                }

                if (dt_.Trim().IsEmpty() || payeeID_ == -1 ||
                    amount_.Trim().IsEmpty() ||  type_.Trim().IsEmpty())
                {
                    log << _("Line : ") << countNumTotal 
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

               int transID = core_->bTransactionList_.addTransaction(core_, pTransaction);
               CSV_transID.push_back(transID);

               countImported++;
               log << _("Line : ") << countNumTotal << _(" imported OK.") << endl;
            }

            progressDlg.Update(100);       

            //wxString msg = wxString::Format(_("Total Lines : %d \nTotal Imported : %d\n\nLog file written to : %s.\n\nImported transactions have been flagged so you can review them. "), countNumTotal, countImported, logFile.GetFullPath().c_str());
            wxString msg = wxString::Format(_("Total Lines : %d"), countNumTotal); 
            msg << wxT ("\n");
            msg << wxString::Format(_("Total Imported : %d"), countImported); 
            msg << wxT ("\n\n");
            msg << wxString::Format(_("Log file written to : %s"), logFile.GetFullPath().c_str());
            msg << wxT ("\n\n");

            wxString confirmMsg = msg + _("Please confirm saving...");
            if (!canceledbyuser && wxMessageBox(confirmMsg, _("Importing CSV MM.NET"), wxOK|wxCANCEL|wxICON_INFORMATION) == wxCANCEL)
                canceledbyuser = true;

            if (countImported > 0)
                msg << _ ("Imported transactions have been flagged so you can review them.");

            // Since all database transactions are only in memory,
            if (!canceledbyuser)
            {
                // we need to save them to the database. 
                mmDBWrapper::commit(db_);
                msg << _("Transactions saved to database in account: ") << acctName;
            }
            else 
            {
                // we need to remove the transactions from the transaction list
                while (countImported > 0)
                {
                    countImported --;
                    int transID = CSV_transID[countImported];
                    core_->bTransactionList_.removeTransaction(fromAccountID,transID);
                }
                // and discard the database changes.
                mmDBWrapper::rollback(db_);
                msg  << _("Imported transactions discarded by user!");
            }

            wxMessageBox(msg, _("Universal CSV Import"), wxICON_INFORMATION);
            outputLog.Close();
            //clear the vector to avoid memory leak - done at same level created.
            CSV_transID.clear();
            fileviewer(logFile.GetFullPath(), this).ShowModal();
        }
    }

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

        if (selIndex < csvListBox_->GetCount())
            csvListBox_->SetSelection(selIndex, true);
        else
            csvListBox_->SetSelection(csvListBox_->GetCount() - 1, true);
    }
}

void mmUnivCSVImportDialog::OnMoveUp(wxCommandEvent& /*event*/)
{
    int selIndex = csvListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND && selIndex != 0)
    {
        //replace the source with destination
        csvListBox_->Delete(selIndex);
        csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex - 1)), selIndex);

        //replace the destination with source
        csvListBox_->Delete(selIndex - 1);
        csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex)), selIndex - 1); 

        //reselect the source
        csvListBox_->SetSelection(selIndex - 1, true);
        std::swap(csvFieldOrder_[selIndex - 1], csvFieldOrder_[selIndex]);
    }
}

void mmUnivCSVImportDialog::OnMoveDown(wxCommandEvent& /*event*/)
{
    int selIndex = csvListBox_->GetSelection();
    if (selIndex != wxNOT_FOUND && selIndex != static_cast<int>(csvFieldOrder_.size()) - 1)
    {
        //replace the source with destination
        csvListBox_->Delete(selIndex);
        csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex + 1)), selIndex); 

        //replace the destination with source
        csvListBox_->Delete(selIndex + 1);
        csvListBox_->Insert(getCSVFieldName(csvFieldOrder_.at(selIndex)), selIndex + 1); 

        //reselect the source
        csvListBox_->SetSelection(selIndex + 1, true);
        std::swap(csvFieldOrder_[selIndex + 1], csvFieldOrder_[selIndex]);
    }
}

void mmUnivCSVImportDialog::OnListBox(wxCommandEvent& event)
{
    int sel = event.GetInt();
    if (sel != wxNOT_FOUND)
    {
        //TODO  update relate widget status
    }
}

void mmUnivCSVImportDialog::parseToken(int index, wxString& token)
{
    if (token.Trim().IsEmpty()) return;

    switch (index)
    {
        case UNIV_CSV_DATE:
            dtdt_ = mmParseDisplayStringToDate(db_, token);
            dt_ = dtdt_.FormatISODate();
            break;

        case UNIV_CSV_PAYEE:
            if (!core_->payeeList_.payeeExists(token))
            {
                payeeID_ = core_->payeeList_.addPayee(token);
            }
            else
            {
                payeeID_ = core_->payeeList_.getPayeeID(token);
            }
            break;

        case UNIV_CSV_AMOUNT:
            token.Replace(wxT(" "), wxEmptyString);

            if (!mmex::formatCurrencyToDouble(token, val_)) return;

            if (val_ <= 0.0)
                type_ = TRANS_TYPE_WITHDRAWAL_STR;
            else
                type_ = TRANS_TYPE_DEPOSIT_STR;

            val_ = fabs(val_);
            amount_ = token;
            break;

        case UNIV_CSV_CATEGORY:
            categID_ = core_->categoryList_.getCategoryID(token);
            if (categID_ == -1)
                categID_ =  core_->categoryList_.addCategory(token);
            break;

        case UNIV_CSV_SUBCATEGORY:
            if (categID_ == -1)
                return;
    
            subCategID_ = core_->categoryList_.getSubCategoryID(categID_, token);
            if (subCategID_ == -1)
                subCategID_ = core_->categoryList_.addSubCategory(categID_, token);
            break;

        case UNIV_CSV_NOTES:
            notes_ = token;
            break;

        case UNIV_CSV_TRANSNUM:
            transNum_ = token;
            break;

        case UNIV_CSV_DONTCARE:
            // do nothing
            break;
        
        case UNIV_CSV_DEPOSIT:
            if (!token.ToDouble(&val_)) return;
            if (val_ <= 0.0) return;

            type_ = TRANS_TYPE_DEPOSIT_STR;
            amount_ = token;
            break;

        case UNIV_CSV_WITHDRAWAL:
            if (!token.ToDouble(&val_)) return;
            if (val_ <= 0.0) return;

            type_ = TRANS_TYPE_WITHDRAWAL_STR;
            amount_ = token;
            break;

        default:
            wxASSERT(true);
            break;
    }
}

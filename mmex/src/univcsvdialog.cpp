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
#include <wx/statline.h>

IMPLEMENT_DYNAMIC_CLASS(mmUnivCSVDialog, wxDialog)


BEGIN_EVENT_TABLE(mmUnivCSVDialog, wxDialog)
    EVT_BUTTON(wxID_ADD, mmUnivCSVDialog::OnAdd)
    EVT_BUTTON(ID_UNIVCSVBUTTON_IMPORT, mmUnivCSVDialog::OnImport)
    EVT_BUTTON(ID_UNIVCSVBUTTON_EXPORT, mmUnivCSVDialog::OnExport)
    EVT_BUTTON(wxID_REMOVE, mmUnivCSVDialog::OnRemove)
    EVT_BUTTON(wxID_OPEN, mmUnivCSVDialog::OnLoad)
    EVT_BUTTON(wxID_SAVEAS, mmUnivCSVDialog::OnSave)
    EVT_BUTTON(wxID_UP, mmUnivCSVDialog::OnMoveUp)
    EVT_BUTTON(wxID_DOWN, mmUnivCSVDialog::OnMoveDown)
    EVT_BUTTON(wxID_STANDARD, mmUnivCSVDialog::OnStandard)
    EVT_BUTTON(wxID_SEARCH, mmUnivCSVDialog::OnSearch)
    EVT_CHOICE(wxID_ACCOUNT, mmUnivCSVDialog::OnAccountChange)
    EVT_LISTBOX(ID_LISTBOX, mmUnivCSVDialog::OnListBox)
    EVT_RADIOBOX(wxID_RADIO_BOX, mmUnivCSVDialog::OnCheckOrRadioBox)
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
    UNIV_CSV_TRANSNUM,
    UNIV_CSV_NOTES,
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

mmUnivCSVDialog::mmUnivCSVDialog()
{
}

mmUnivCSVDialog::mmUnivCSVDialog(
    mmCoreDB* core, 
    wxWindow* parent, 
    bool is_importer,
    wxWindowID id, 
    const wxString& caption, 
    const wxPoint& pos, 
    const wxSize& size, 
    long style
) : 
    core_(core),
    is_importer_(is_importer),
    delimit_(wxT(",")),
    db_ (core->db_.get())
{
    CSVFieldName_[UNIV_CSV_DATE] = _("Date");
    CSVFieldName_[UNIV_CSV_PAYEE] = _("Payee");
    CSVFieldName_[UNIV_CSV_AMOUNT] = _("Amount(+/-)");
    CSVFieldName_[UNIV_CSV_CATEGORY] = _("Category");
    CSVFieldName_[UNIV_CSV_SUBCATEGORY] = _("SubCategory");
    CSVFieldName_[UNIV_CSV_TRANSNUM] = _("Transaction Number");
    CSVFieldName_[UNIV_CSV_NOTES] = _("Notes");
    CSVFieldName_[UNIV_CSV_DONTCARE] = _("Don't Care");
    CSVFieldName_[UNIV_CSV_WITHDRAWAL] = _("Withdrawal");
    CSVFieldName_[UNIV_CSV_DEPOSIT] = _("Deposit");

    Create(parent, id, caption, pos, size, style);
}

bool mmUnivCSVDialog::Create(wxWindow* parent, wxWindowID id, 
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

void mmUnivCSVDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText(this, wxID_STATIC, 
       _("Specify the order of fields in the CSV file"), wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer2->Add(itemStaticText3, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    //CSV fields candicate
    csvFieldCandicate_ = new wxListBox(this, ID_LISTBOX_CANDICATE, 
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_NEEDED_SB);
    itemBoxSizer3->Add(csvFieldCandicate_, 1, wxGROW|wxALL, 1);
    for(std::map<int, wxString>::const_iterator it = CSVFieldName_.begin(); it != CSVFieldName_.end(); it ++)
        csvFieldCandicate_->Append(it->second, new mmListBoxItem(it->first, it->second));

     //Add Remove Area
    wxPanel* itemPanel_AddRemove = new wxPanel(this, ID_PANEL10, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer3->Add(itemPanel_AddRemove, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer_AddRemove = new wxBoxSizer(wxVERTICAL);
    itemPanel_AddRemove->SetSizer(itemBoxSizer_AddRemove);

    //Add button
    m_button_add_= new wxButton(itemPanel_AddRemove, wxID_ADD, _("Add"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_AddRemove->Add(m_button_add_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //Remove button
    m_button_remove_ = new wxButton(itemPanel_AddRemove, wxID_REMOVE, _("Remove"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_AddRemove->Add(m_button_remove_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
    //ListBox of attribute order
    csvListBox_ = new wxListBox(this, ID_LISTBOX, 
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_NEEDED_SB);
    itemBoxSizer3->Add(csvListBox_, 1, wxGROW|wxALL, 1);

   //Arranger Area
    wxPanel* itemPanel_Arranger = new wxPanel(this, ID_PANEL10, 
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

    //Standard MMEX CSV
    wxButton* itemButton_standard = new wxButton(itemPanel_Arranger, wxID_STANDARD, _("&MMEX format"),
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_Arranger->Add(itemButton_standard, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_standard->SetToolTip(_("MMEX standard format"));
 
    //Load Template button
    wxButton* itemButton_Load = new wxButton(itemPanel_Arranger, wxID_OPEN, _("&Open"), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_Arranger->Add(itemButton_Load, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_Load -> SetToolTip (_("Load Template"));

    //Save As Template button
    wxButton* itemButton_Save = new wxButton(itemPanel_Arranger, wxID_SAVEAS, _("Save As..."), 
        wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer_Arranger->Add(itemButton_Save, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton_Save -> SetToolTip (_("Save Template"));

    wxStaticLine*  m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(m_staticline1, 0, wxEXPAND | wxALL, 5 );

    if (this->is_importer_)
    {
        //file to import, file path and search button
        wxPanel* itemPanel6 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        itemBoxSizer2->Add(itemPanel6, 0, wxEXPAND|wxALL, 1);

        wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
        itemPanel6->SetSizer(itemBoxSizer7);

        wxStaticText* itemStaticText5 = new wxStaticText(itemPanel6, wxID_ANY, _("File Name:"), wxDefaultPosition, wxDefaultSize, 0);
        itemBoxSizer7->Add(itemStaticText5, 0, wxALIGN_LEFT|wxALL, 5);

        m_text_ctrl_ = new wxTextCtrl(itemPanel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(300, -1), 0);
        itemBoxSizer7->Add(m_text_ctrl_, 1, wxALL|wxGROW, 5);

        wxButton* button_search = new wxButton(itemPanel6, wxID_SEARCH, _("&Search"));
        itemBoxSizer7->Add(button_search, 0, wxALIGN_RIGHT|wxALL, 5);
    }

    // account to import or export
    wxPanel* itemPanel7 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer2->Add(itemPanel7, 0, wxEXPAND|wxALL, 1);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel7->SetSizer(itemBoxSizer8);

    wxStaticText* itemStaticText6 = new wxStaticText(itemPanel7, wxID_ANY, _("Account  :"), wxDefaultPosition, wxDefaultSize, 0);
    itemBoxSizer8->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString as = core_->getAccountsName();
    m_choice_account_ = new wxChoice(itemPanel7, wxID_ACCOUNT, wxDefaultPosition, wxSize(210, -1), as, 0);
    m_choice_account_->SetSelection(0);
    itemBoxSizer8->Add(m_choice_account_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine*  m_staticline2 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(m_staticline2, 0, wxEXPAND | wxALL, 5 );

    // CSV Delimiter
    wxString choices[] = { _("Comma"), _("Semicolon"), _("TAB")};
    int num = sizeof(choices) / sizeof(wxString);
    m_radio_box_ = new wxRadioBox(this, wxID_RADIO_BOX, _("CSV Delimiter"), wxDefaultPosition, wxDefaultSize, num, choices, 3, wxRA_SPECIFY_COLS);
    m_radio_box_->SetSelection(0);
    itemBoxSizer2->Add(m_radio_box_, 0, wxALL|wxEXPAND, 5);

    // Preview 
    wxStaticBoxSizer* m_staticbox = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _T("&Preview")), wxVERTICAL);

    m_list_ctrl_ = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 100), wxLC_REPORT);
    m_staticbox->Add(m_list_ctrl_, 1, wxGROW|wxALL, 5);
    itemBoxSizer2->Add(m_staticbox, 0, wxALL|wxEXPAND, 5);

    //Import File button
    wxPanel* itemPanel5 = new wxPanel(this, ID_PANEL10, 
    wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer2->Add(itemPanel5, 0, wxALIGN_RIGHT|wxALL, 1);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel5->SetSizer(itemBoxSizer6);

    if (this->is_importer_)
    {
        wxButton* itemButton_Import = new wxButton(itemPanel5, ID_UNIVCSVBUTTON_IMPORT, _("&Import"), 
            wxDefaultPosition, wxDefaultSize, 0);
        itemBoxSizer6->Add(itemButton_Import, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
        itemButton_Import -> SetToolTip (_("Import File"));
    }
    else
    {
        wxButton* itemButton_Export = new wxButton(itemPanel5, ID_UNIVCSVBUTTON_EXPORT, _("&Export"), 
            wxDefaultPosition, wxDefaultSize, 0);
        itemBoxSizer6->Add(itemButton_Export, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
        itemButton_Export -> SetToolTip (_("Export File"));
    }

    wxButton* itemCancelButton = new wxButton(itemPanel5, wxID_CANCEL, _("&Cancel"));
    itemBoxSizer6->Add(itemCancelButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemCancelButton->SetFocus();
}

bool mmUnivCSVDialog::ShowToolTips()
{
    return TRUE;
}

wxBitmap mmUnivCSVDialog::GetBitmapResource(const wxString& /*name*/)
{
    return wxNullBitmap;
}

wxIcon mmUnivCSVDialog::GetIconResource(const wxString& /*name*/)
{
    return wxNullIcon;
}

//Selection dialog for fields to be added to listbox
void mmUnivCSVDialog::OnAdd(wxCommandEvent& /*event*/)
{
    int index = csvFieldCandicate_->GetSelection();
    if (index != wxNOT_FOUND)
    {
        mmListBoxItem* item = (mmListBoxItem*)csvFieldCandicate_->GetClientObject(index);

        csvListBox_->Append(item->getName(), new mmListBoxItem(item->getIndex(), item->getName()));
        csvFieldOrder_.push_back(item->getIndex());

        if (item->getIndex() != UNIV_CSV_DONTCARE) 
        {
            csvFieldCandicate_->Delete(index);
            if (index < (int)csvFieldCandicate_->GetCount())
                csvFieldCandicate_->SetSelection(index, true);
            else
                csvFieldCandicate_->SetSelection(csvFieldCandicate_->GetCount() - 1, true);
        }

        this->update_preview();
    }
}

//Removes an item from the field list box
void mmUnivCSVDialog::OnRemove(wxCommandEvent& /*event*/)
{
    int index = csvListBox_->GetSelection();
    if (index != wxNOT_FOUND)
    {
        mmListBoxItem *item = (mmListBoxItem*)csvListBox_->GetClientObject(index);
        int item_index = item->getIndex();
        wxString item_name = item->getName();

        if (item_index != UNIV_CSV_DONTCARE)
        {
            int pos = 0;
            for (pos = 0; pos < (int)csvFieldCandicate_->GetCount() - 1; pos ++)
            {
                mmListBoxItem *item = (mmListBoxItem*)csvFieldCandicate_->GetClientObject(pos);
                if (item_index < item->getIndex())
                    break;
            }
            csvFieldCandicate_->Insert(item_name, pos, new mmListBoxItem(item_index, item_name));
        }

        csvListBox_->Delete(index);
        csvFieldOrder_.erase(csvFieldOrder_.begin() + index);

        if (index < (int)csvListBox_->GetCount())
            csvListBox_->SetSelection(index, true);
        else
            csvListBox_->SetSelection(csvListBox_->GetCount() - 1, true);

        this->update_preview();
    }
}

bool mmUnivCSVDialog::isIndexPresent(int index) const
{
    for(std::vector<int>::const_iterator it = csvFieldOrder_.begin(); it != csvFieldOrder_.end(); ++ it)
    {
        if (*it == index) return true;
    }

    return false;
}

const wxString mmUnivCSVDialog::getCSVFieldName(int index) const
{
    std::map<int, wxString>::const_iterator it = CSVFieldName_.find(index);
    if (it != CSVFieldName_.end())
        return it->second;

    return _("Unknown");
}

void mmUnivCSVDialog::OnLoad(wxCommandEvent& /*event*/)
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
             wxString item_name = getCSVFieldName(num);
             csvListBox_->Append(item_name, new mmListBoxItem(num, item_name));
             csvFieldOrder_.push_back(num);
         }
      }
      // update csvFieldCandicate_
      csvFieldCandicate_->Clear();
      for (std::map<int, wxString>::const_iterator it = CSVFieldName_.begin(); it != CSVFieldName_.end(); ++ it)
      {
          std::vector<int>::const_iterator loc = find(csvFieldOrder_.begin(), csvFieldOrder_.end(), it->first);
          if (loc == csvFieldOrder_.end() || it->first == UNIV_CSV_DONTCARE)
              csvFieldCandicate_->Append(it->second, new mmListBoxItem(it->first, it->second));
      }

      tFile.Write();
      tFile.Close();

      this->update_preview();
   }
}

//Saves the field order to a template file
void mmUnivCSVDialog::OnSave(wxCommandEvent& /*event*/)
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

void mmUnivCSVDialog::OnImport(wxCommandEvent& /*event*/)
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


//    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), mmex::DEFDELIMTER);
    wxString delimit = this->delimit_;
    wxString acctName = m_choice_account_->GetStringSelection();
    int fromAccountID = core_->getAccountID(acctName);

    if (fromAccountID > 0)
    {
        
        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
             
        wxString fileName = m_text_ctrl_->GetValue();
        if (fileName.IsEmpty())
        {
            return;
        }
        else
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
            db_->Begin();

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
                   boost::shared_ptr<mmPayee> pPayee =  core_->getPayeeSharedPtr(payeeID_);
                   boost::shared_ptr<mmCategory> pCategory = pPayee->category_.lock();
                   if (!pCategory)
                   {
                       subCategID_ = -1;
                       wxString categ = wxT("Unknown");

                       categID_ = core_->getCategoryID(categ);
                       if (categID_ == -1)
                       {
                           categID_ =  core_->addCategory(categ);
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
               pTransaction->payee_ = core_->getPayeeSharedPtr(payeeID_);
               pTransaction->transType_ = type_;
               pTransaction->amt_ = val_;
               pTransaction->status_ = status;
               pTransaction->transNum_ = transNum_;
               pTransaction->notes_ = notes_;
               pTransaction->category_ = core_->getCategorySharedPtr(categID_, subCategID_);
               pTransaction->date_ = dtdt_;
               pTransaction->toAmt_ = 0.0;
               pTransaction->updateAllData(core_, fromAccountID, pCurrencyPtr);

               int transID = core_->bTransactionList_.addTransaction(core_, pTransaction);
               CSV_transID.push_back(transID);

               countImported++;
               log << _("Line : ") << countNumTotal << _(" imported OK.") << endl;
            }

            progressDlg.Update(100);       

            wxString msg = wxString::Format(_("Total Lines : %d"), countNumTotal); 
            msg << wxT ("\n");
            msg << wxString::Format(_("Total Imported : %d"), countImported); 
            msg << wxT ("\n\n");
            msg << wxString::Format(_("Log file written to : %s"), logFile.GetFullPath().c_str());
            msg << wxT ("\n\n");

            wxString confirmMsg = msg + _("Please confirm saving...");
            if (!canceledbyuser && wxMessageBox(confirmMsg, _("Importing CSV"), wxOK|wxCANCEL|wxICON_INFORMATION) == wxCANCEL)
                canceledbyuser = true;

            if (countImported > 0)
                msg << _ ("Imported transactions have been flagged so you can review them.");
 
            // Since all database transactions are only in memory,
            if (!canceledbyuser)
            {
                // we need to save them to the database. 
                db_->Commit();
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
                db_->Rollback();
                msg  << _("Imported transactions discarded by user!");
            }

            //wxMessageBox(msg, _("Universal CSV Import"), wxICON_INFORMATION);
            outputLog.Close();
            //clear the vector to avoid memory leak - done at same level created.
            CSV_transID.clear();
            fileviewer(logFile.GetFullPath(), this).ShowModal();
        }
    }

    Close();
}

void mmUnivCSVDialog::OnExport(wxCommandEvent& /*event*/)
{
    wxString delimit = this->delimit_;
    wxString acctName = m_choice_account_->GetStringSelection();
    int fromAccountID = core_->getAccountID(acctName);

    if (fromAccountID > 0)
    {
        wxString chooseExt;
        chooseExt << _("CSV Files") << wxT(" (*.csv)|*.csv;*.CSV");
        wxString fileName = wxFileSelector(_("Choose CSV data file to Export"),
                            wxEmptyString, wxEmptyString, wxEmptyString, chooseExt, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (fileName.empty()) return;
        
        correctEmptyFileExt(wxT("csv"),fileName);

        wxFileOutputStream output(fileName);
        wxTextOutputStream text(output);

        int numRecords = 0;

        for(std::vector< boost::shared_ptr<mmBankTransaction> >::const_iterator it = core_->bTransactionList_.transactions_.begin();
                it != core_->bTransactionList_.transactions_.end();
                ++ it)
        {
            const mmBankTransaction* pBankTransaction = it->get();
            if (pBankTransaction && pBankTransaction->accountID_ == fromAccountID)
            {
                if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR) continue; //TODO
                double amount = pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR ? pBankTransaction->amt_ : - pBankTransaction->amt_;
                
                for (std::vector<int>::const_iterator sit = csvFieldOrder_.begin(); sit != csvFieldOrder_.end(); ++ sit)
                {
                    switch (*sit)
                    {
                        case UNIV_CSV_DATE:
                            text << pBankTransaction->dateStr_;
                            break;
                        case UNIV_CSV_PAYEE:
                            text << inQuotes(pBankTransaction->payeeStr_);
                            break;
                        case UNIV_CSV_AMOUNT:
                            text << amount;
                            break;
                        case UNIV_CSV_CATEGORY:
                            text << inQuotes(pBankTransaction->catStr_);
                            break;
                        case UNIV_CSV_SUBCATEGORY:
                            text << inQuotes(pBankTransaction->subCatStr_);
                            break;
                        case UNIV_CSV_TRANSNUM:
                            text << pBankTransaction->transNum_;
                            break;
                        case UNIV_CSV_NOTES:
                            text << inQuotes(pBankTransaction->notes_);
                            break;
                        case UNIV_CSV_WITHDRAWAL:
                            text << (amount > 0 ? 0.0 : amount);
                            break;
                        case UNIV_CSV_DEPOSIT:
                            text << (amount > 0 ? amount : 0);
                            break;
                        case UNIV_CSV_DONTCARE:
                        default:
                            break;
                    }
                    
                    text << delimit;
                }

                text << endl;

                ++ numRecords;
            }
        }

        wxString msg = wxString::Format(wxT("%d transactions exported"), numRecords);
        mmShowErrorMessage(0, msg, _("Export to CSV"));
    }
}

void mmUnivCSVDialog::update_preview()
{
    this->m_list_ctrl_->ClearAll();
    long index = 0;
    this->m_list_ctrl_->InsertColumn(index, _("#"));
    for (std::vector<int>::const_iterator it = csvFieldOrder_.begin(); it != csvFieldOrder_.end(); ++ it)
    {
        ++ index;
        this->m_list_ctrl_->InsertColumn(index, this->getCSVFieldName(*it));
    }
    
    //TODO re use code in OnImport & OnExport
    if (this->is_importer_)
    {
        wxString fileName = m_text_ctrl_->GetValue();
        if (!fileName.IsEmpty())
        {
            wxTextFile tFile(fileName);
            if (!tFile.Open())
            {
                 wxMessageBox(_("Unable to open file."), _("Universal CSV Import"), wxICON_WARNING);
                 return;
            }

            wxString delimit = this->delimit_;
            wxString line;
            size_t count = 0;
            int row = 0;
            for (line = tFile.GetFirstLine(); !tFile.Eof(); line = tFile.GetNextLine())
            {
                wxStringTokenizer tkz(line, delimit, wxTOKEN_RET_EMPTY_ALL);

                int col = 0;
                wxString buf;
                buf.Printf(_T("%d"), col);
                long itemIndex = m_list_ctrl_->InsertItem(row, buf, 0);
                buf.Printf(_T("%d"), row + 1);
                m_list_ctrl_->SetItem(itemIndex, col, buf);
                while (tkz.HasMoreTokens())
                {
                    ++ col;
                    wxString token = tkz.GetNextToken();

                    if (col >= m_list_ctrl_->GetColumnCount())
                        break;
                    else
                        m_list_ctrl_->SetItem(itemIndex, col, token);
                }

                if (++ count >= 10) break;
                ++ row;
            }
        }
    }
    else // exporter preview
    {
        wxString acctName = m_choice_account_->GetStringSelection();
        int fromAccountID = core_->getAccountID(acctName);

        if (fromAccountID > 0)
        {
            size_t count = 0;
            int row = 0;
            for(std::vector< boost::shared_ptr<mmBankTransaction> >::const_iterator it = core_->bTransactionList_.transactions_.begin();
                it != core_->bTransactionList_.transactions_.end();
                ++ it)
            {
                const mmBankTransaction* pBankTransaction = it->get();
                if (pBankTransaction && pBankTransaction->accountID_ == fromAccountID)
                {
                    if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR) continue; //TODO
                    double amount = pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR ? pBankTransaction->amt_ : - pBankTransaction->amt_;

                    int col = 0;
                    wxString buf;
                    buf.Printf(_T("%d"), col);
                    long itemIndex = m_list_ctrl_->InsertItem(row, buf, 0);
                    buf.Printf(_T("%d"), row + 1);
                    m_list_ctrl_->SetItem(itemIndex, col, buf);
                    
                    for (std::vector<int>::const_iterator sit = csvFieldOrder_.begin(); sit != csvFieldOrder_.end(); ++ sit)
                    {
                        ++ col;
                        wxString text;
                        switch (*sit)
                        {
                            case UNIV_CSV_DATE:
                                text << pBankTransaction->dateStr_;
                                break;
                            case UNIV_CSV_PAYEE:
                                text << inQuotes(pBankTransaction->payeeStr_);
                                break;
                            case UNIV_CSV_AMOUNT:
                                text << amount;
                                break;
                            case UNIV_CSV_CATEGORY:
                                text << inQuotes(pBankTransaction->catStr_);
                                break;
                            case UNIV_CSV_SUBCATEGORY:
                                text << inQuotes(pBankTransaction->subCatStr_);
                                break;
                            case UNIV_CSV_TRANSNUM:
                                text << pBankTransaction->transNum_;
                                break;
                            case UNIV_CSV_NOTES:
                                text << inQuotes(pBankTransaction->notes_);
                                break;
                            case UNIV_CSV_WITHDRAWAL:
                                text << (amount > 0 ? 0.0 : amount);
                                break;
                            case UNIV_CSV_DEPOSIT:
                                text << (amount > 0 ? amount : 0);
                                break;
                            case UNIV_CSV_DONTCARE:
                            default:
                                break;
                       }
                       if (col >= m_list_ctrl_->GetColumnCount())
                            break;
                       else
                            m_list_ctrl_->SetItem(itemIndex, col, text);
                        
                    }
                    if (++ count >= 10) break;
                    ++ row;
                }
            }
        }
    }
}

void mmUnivCSVDialog::OnMoveUp(wxCommandEvent& /*event*/)
{
    int index = csvListBox_->GetSelection();
    if (index != wxNOT_FOUND && index != 0)
    {
        mmListBoxItem* item = (mmListBoxItem*)csvListBox_->GetClientObject(index);
        int item_index = item->getIndex();
        wxString item_name = item->getName();

        csvListBox_->Delete(index);
        csvListBox_->Insert(item_name, index - 1, new mmListBoxItem(item_index, item_name));

        csvListBox_->SetSelection(index - 1, true);
        std::swap(csvFieldOrder_[index - 1], csvFieldOrder_[index]);

        this->update_preview();
    }
}

void mmUnivCSVDialog::OnMoveDown(wxCommandEvent& /*event*/)
{
    int index = csvListBox_->GetSelection();
    if (index != wxNOT_FOUND && index != (int)csvListBox_->GetCount() - 1)
    {
        mmListBoxItem* item = (mmListBoxItem*)csvListBox_->GetClientObject(index);
        int item_index = item->getIndex();
        wxString item_name = item->getName();

        csvListBox_->Delete(index);
        csvListBox_->Insert(item_name, index + 1, new mmListBoxItem(item_index, item_name)); 

        csvListBox_->SetSelection(index + 1, true);
        std::swap(csvFieldOrder_[index + 1], csvFieldOrder_[index]);

        this->update_preview();
    }
}

void mmUnivCSVDialog::OnStandard(wxCommandEvent& /*event*/)
{
    csvListBox_->Clear(); 
    csvFieldOrder_.clear();
    int standard[] = {UNIV_CSV_DATE, UNIV_CSV_PAYEE, UNIV_CSV_AMOUNT, UNIV_CSV_CATEGORY, UNIV_CSV_SUBCATEGORY, UNIV_CSV_TRANSNUM, UNIV_CSV_NOTES};
    for (int i = 0; i < sizeof(standard)/sizeof(UNIV_CSV_DATE); ++ i)
    {
        csvListBox_->Append(CSVFieldName_[standard[i]], new mmListBoxItem(standard[i], CSVFieldName_[standard[i]]));
        csvFieldOrder_.push_back(standard[i]);
    }

    csvFieldCandicate_->Clear();
    int rest[] = {UNIV_CSV_DONTCARE, UNIV_CSV_WITHDRAWAL, UNIV_CSV_DEPOSIT};
    for (int i = 0; i < sizeof(rest)/sizeof(UNIV_CSV_DATE); ++ i)
    {
        csvFieldCandicate_->Append(CSVFieldName_[rest[i]], new mmListBoxItem(rest[i], CSVFieldName_[rest[i]]));
    }
    
    update_preview();
}

void mmUnivCSVDialog::OnSearch(wxCommandEvent& /*event*/)
{
    wxString fileName = wxFileSelector(_("Choose CSV data file to import"), 
            wxEmptyString, wxEmptyString, wxEmptyString, wxT("*.csv"), wxFD_FILE_MUST_EXIST);
    if (!fileName.IsEmpty())
    {
        m_text_ctrl_->SetValue(fileName);
        this->update_preview();
    }
}

void mmUnivCSVDialog::OnAccountChange(wxCommandEvent& event)
{
    int sel = event.GetInt();
    if (sel != wxNOT_FOUND)
    {
        update_preview();
    }
}

void mmUnivCSVDialog::OnListBox(wxCommandEvent& event)
{
    int sel = event.GetInt();
    if (sel != wxNOT_FOUND)
    {
        //TODO  update relate widget status
    }
}

void mmUnivCSVDialog::OnCheckOrRadioBox(wxCommandEvent& /*event*/)
{
    //TODO match value to choices
    switch(m_radio_box_->GetSelection())
    {
        case 0:
            this->delimit_ = wxT(",");
            break;
        case 1:
            this->delimit_ = wxT(";");
            break;
        case 2:
            this->delimit_ = wxT("\t");
            break;
        default:
            break;
    }

    this->update_preview();
}

void mmUnivCSVDialog::parseToken(int index, wxString& token)
{
    if (token.Trim().IsEmpty()) return;

    switch (index)
    {
        case UNIV_CSV_DATE:
            dtdt_ = mmParseDisplayStringToDate(db_, token);
            dt_ = dtdt_.FormatISODate();
            break;

        case UNIV_CSV_PAYEE:
            if (!core_->payeeExists(token))
            {
                payeeID_ = core_->addPayee(token);
            }
            else
            {
                payeeID_ = core_->getPayeeID(token);
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
            categID_ = core_->getCategoryID(token);
            if (categID_ == -1)
                categID_ =  core_->addCategory(token);
            break;

        case UNIV_CSV_SUBCATEGORY:
            if (categID_ == -1)
                return;
    
            subCategID_ = core_->getSubCategoryID(categID_, token);
            if (subCategID_ == -1)
                subCategID_ = core_->addSubCategory(categID_, token);
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

/*******************************************************
  Copyright (C) 2013 Nikolay
 ********************************************************/

#include "export.h"
#include "../constants.h"
#include "../util.h"
#include "model/Model_Infotable.h"

mmExportTransaction::mmExportTransaction(mmCoreDB* core, mmBankTransaction* pBankTransaction)
    : mmExportBase(core)
    , pBankTransaction_(pBankTransaction)
{}

mmExportTransaction::~mmExportTransaction()
{};

wxString mmExportTransaction::getTransactionQIF()
{
    mmBankTransaction* &transaction = pBankTransaction_;
    wxString buffer = "";
    int trans_id = transaction->transactionID();
    int account_id = transaction->accountID_;
    wxString categ = transaction->fullCatStr_;
    wxString payee = transaction->payeeStr_;
    wxString transNum = transaction->transNum_;
    wxString notes = (transaction->notes_);
    notes.Replace("''", "'");
    notes.Replace("\n", " ");

    if (transaction->transType_ == TRANS_TYPE_TRANSFER_STR)
    {
        categ = wxString::Format("[%s]", transaction->payeeStr_);
        payee = "";

        //Transaction number used to make transaction unique
        // to proper merge transfer records
        if (transNum.IsEmpty() && notes.IsEmpty())
            transNum = wxString::Format("#%i", trans_id);
    }

    buffer << "D" << mmGetDateForDisplay(transaction->date_) << "\n";
    buffer << "T" << transaction->value(account_id) << "\n";
    if (!payee.IsEmpty())
        buffer << "P" << payee << "\n";
    if (!transNum.IsEmpty())
        buffer << "N" << transNum << "\n";
    if (!categ.IsEmpty())
        buffer << "L" << categ << "\n";
    if (!notes.IsEmpty())
        buffer << "M" << notes << "\n";

    //if categ id is empty that mean this is split transaction
    if (transaction->categID_ == -1)
    {
        mmSplitTransactionEntries* splits = transaction->splitEntries_;
        transaction->getSplitTransactions(splits);

        for (const auto &split_entry : splits->entries_)
        {
            double value = split_entry->splitAmount_;
            if (transaction->transType_ == "Withdrawal")
                value = -value;
            const wxString split_amount = wxString()<<value;
            const wxString split_categ = core_->categoryList_.GetFullCategoryString(
                split_entry->categID_, split_entry->subCategID_);
            buffer << "S" << split_categ << "\n"
                << "$" << split_amount << "\n";
        }
    }

    buffer << "^" << "\n";
    return buffer;
}

wxString mmExportTransaction::getTransactionCSV()
{
    mmBankTransaction* &transaction = pBankTransaction_;
    wxString delimit = Model_Infotable::instance().GetStringInfo("DELIMITER", mmex::DEFDELIMTER);
    wxString buffer = "";
    wxString acctName = core_->accountList_.GetAccountName(transaction->accountID_);
    int trans_id = transaction->transactionID();
    wxString categ = transaction->fullCatStr_;
    wxString payee = transaction->payeeStr_;
    wxString transNum = transaction->transNum_;
    wxString notes = (transaction->notes_);
    notes.Replace("''", "'");
    notes.Replace("\n", " ");

    if (transaction->transType_ == TRANS_TYPE_TRANSFER_STR)
    {
        categ = wxString::Format("[%s]", transaction->payeeStr_);
        payee = "";
        //Transaction number used to make transaction unique
        // to proper merge transfer records
        if (transNum.IsEmpty() && notes.IsEmpty())
            transNum = wxString::Format("#%i", trans_id);
    }

    if (transaction->categID_ == -1)
    {
        mmSplitTransactionEntries* splits = transaction->splitEntries_;
        transaction->getSplitTransactions(splits);

        for (const auto &split_entry : splits->entries_)
        {
            double value = split_entry->splitAmount_;
            if (transaction->transType_ == "Withdrawal")
                value = -value;
            const wxString split_amount = wxString()<<value;

            const wxString split_categ = core_->categoryList_.GetFullCategoryString(
                split_entry->categID_, split_entry->subCategID_);

            buffer << trans_id << delimit
                << inQuotes(acctName, delimit) << delimit
                << inQuotes(mmGetDateForDisplay(transaction->date_), delimit) << delimit
                << inQuotes(payee, delimit) << delimit
                << transaction->status_ << delimit
                << transaction->transType_ << delimit
                << inQuotes(split_categ, delimit) << delimit
                << inQuotes(split_amount, delimit) << delimit
                << "" << delimit
                << inQuotes(notes, delimit)
                << "\n";
        }

    }
    return buffer;

}
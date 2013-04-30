/*******************************************************
 Copyright (C) 2013 Stefano Giorgio

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

#pragma once
#include "entry_base.h"

/************************************************************************************
 Class TStockEntry
 ***********************************************************************************/
class TStockEntry : public TEntryBase
{
private:
    friend class TStockList;    // Allows TStockList access to private members

    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);
    void SetDatabaseValues(wxSQLite3Statement& st, int& db_index);

public:
    int heldat_;        // wxT("HELDAT")
    wxString pur_date_; // wxT("PURCHASEDATE")
    wxString name_;     // wxT("STOCKNAME")
    wxString symbol_;   // wxT("SYMBOL")
    double num_shares_; // wxT("NUMSHARES")
    double pur_price_;  // wxT("PURCHASEPRICE")
    wxString notes_;    // wxT("NOTES")
    double cur_price_;  // wxT("CURRENTPRICE")
    double value_;      // wxT("VALUE")
    double commission_; // wxT("COMMISSION")

    /* Constructor used when loading stocks from the database. */
    TStockEntry(wxSQLite3ResultSet& q1);
    /* Copy constructor using a pointer */
    TStockEntry(TStockEntry* pEntry);
    /* Constructor for creating a new stock entry. */
    TStockEntry();

    // Updates the existing data to the SQL file. Add to list first.
    void Update(wxSQLite3Database* db);
    // returns date formated to user requirements.
    wxString DisplayDate();
    // returns number of shares formated to 4 decimal places or whole number.
    wxString NumberOfShares(bool whole_num = true);
    // returns current share price formated to 4 decimal places
    wxString CurrentPrice();
    // returns the value based on share price and number of shares
    double GetValue();
    wxString GetValueCurrencyEditFormat(bool initial_value = false);
    // returns formatted commission value
    wxString CommissionCurrencyEditFormat();
};

/************************************************************************************
 Class TStockList
 ***********************************************************************************/
class TStockList : public TListBase
{
private:
    void LoadEntries(bool load_entries = true);

public:
    std::vector<wxSharedPtr<TStockEntry> >entrylist_;

    TStockList(wxSharedPtr<wxSQLite3Database> db, bool load_entries = true);

    // Allows specialised list loading provided by SQL statement
    void LoadEntriesUsing(const wxString& sql_statement);

    int AddEntry(TStockEntry* pStockEntry);
    void DeleteEntry(int stock_id);

    wxSharedPtr<TStockEntry> GetEntryPtr(int stock_id);
    wxSharedPtr<TStockEntry> GetIndexedEntryPtr(unsigned int list_index);

    int CurrentListSize();
    /// return the balance of all the stocks in the list.
    double GetStockBalance();
    wxString GetStockBalanceCurrencyFormat();
    wxString GetStockBalanceCurrencyEditFormat();
};

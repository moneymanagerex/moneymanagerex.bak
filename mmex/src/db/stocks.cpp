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

#include "stocks.h"

/************************************************************************************
 TStockEntry Methods
 ***********************************************************************************/
/// Constructor used when loading stocks from the database
TStockEntry::TStockEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_         = q1.GetInt(wxT("STOCKID"));
    heldat_     = q1.GetInt(wxT("HELDAT"));
    pur_date_   = q1.GetString(wxT("PURCHASEDATE"));
    name_       = q1.GetString(wxT("STOCKNAME"));
    symbol_     = q1.GetString(wxT("SYMBOL"));
    num_shares_ = q1.GetDouble(wxT("NUMSHARES"));
    pur_price_  = q1.GetDouble(wxT("PURCHASEPRICE"));
    notes_      = q1.GetString(wxT("NOTES"));
    cur_price_  = q1.GetDouble(wxT("CURRENTPRICE"));
    value_      = q1.GetDouble(wxT("VALUE"));
    commission_ = q1.GetDouble(wxT("COMMISSION"));
}

/// Copy constructor using a pointer
TStockEntry::TStockEntry(TStockEntry* pEntry)
: TEntryBase()
{
    heldat_     = pEntry->heldat_;
    pur_date_   = pEntry->pur_date_;
    name_       = pEntry->name_;
    symbol_     = pEntry->symbol_;
    num_shares_ = pEntry->num_shares_;
    pur_price_  = pEntry->pur_price_;
    notes_      = pEntry->notes_;
    cur_price_  = pEntry->cur_price_;
    value_      = pEntry->value_;
    commission_ = pEntry->commission_;
}

void TStockEntry::SetDatabaseValues(wxSQLite3Statement& st, int& db_index)
{
    st.Bind(++db_index, heldat_);    // wxT("HELDAT")
    st.Bind(++db_index, pur_date_);  // wxT("PURCHASEDATE")
    st.Bind(++db_index, name_);      // wxT("STOCKNAME")
    st.Bind(++db_index, symbol_);    // wxT("SYMBOL")
    st.Bind(++db_index, num_shares_);// wxT("NUMSHARES")
    st.Bind(++db_index, pur_price_); // wxT("PURCHASEPRICE")
    st.Bind(++db_index, notes_);     // wxT("NOTES")
    st.Bind(++db_index, cur_price_); // wxT("CURRENTPRICE")
    st.Bind(++db_index, value_);     // wxT("VALUE")
    st.Bind(++db_index, commission_);// wxT("COMMISSION")
}

/// Constructor for creating a new stock entry.
TStockEntry::TStockEntry()
: TEntryBase()
, heldat_(-1)
, num_shares_(0.0)
, pur_price_(0.0)
, cur_price_(0.0)
, value_(0.0)
, commission_(0.0)
{}

int TStockEntry::Add(wxSQLite3Database* db)
{
    const char ADD_STOCK_V1_ROW[] =
    "insert into STOCK_V1 "
    " (HELDAT, PURCHASEDATE, STOCKNAME, SYMBOL, NUMSHARES,"
    " PURCHASEPRICE, NOTES, CURRENTPRICE, VALUE, COMMISSION)"
    " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    wxSQLite3Statement st = db->PrepareStatement(ADD_STOCK_V1_ROW);
    int db_index = 0;
    SetDatabaseValues(st, db_index);
    
    FinaliseAdd(db, st);

    return id_;
}

void TStockEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from STOCK_V1 where STOCKID = ?")); 
}

void TStockEntry::Update(wxSQLite3Database* db)
{
    const char UPDATE_STOCK_V1_ROW[]  =
    "update STOCK_V1 set"
    " HELDAT = ?, PURCHASEDATE = ?, STOCKNAME = ?, SYMBOL = ?, NUMSHARES = ?,"
    " PURCHASEPRICE = ?, NOTES = ?, CURRENTPRICE = ?, VALUE = ?, COMMISSION = ? "
    "where STOCKID = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_STOCK_V1_ROW);
        int db_index = 0;
        SetDatabaseValues(st, db_index);
        st.Bind(++db_index, id_);

        FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        wxLogError(wxT("TStockEntry:Update: %s"), e.GetMessage().c_str());
    }
}

/************************************************************************************
 TStockList Methods
 ***********************************************************************************/
/// Constructor
TStockList::TStockList(boost::shared_ptr<wxSQLite3Database> db, bool load_entries)
: TListBase(db)
{
    LoadEntries(load_entries);
}

void TStockList::LoadEntries(bool load_entries)
{
    try
    {
        if (!db_->TableExists(wxT("STOCK_V1")))
        {
            const char CREATE_TABLE_STOCK_V1[] =
            "CREATE TABLE STOCK_V1(STOCKID integer primary key, "
            "HELDAT numeric, PURCHASEDATE TEXT NOT NULL, STOCKNAME TEXT, SYMBOL TEXT, "
            "NUMSHARES numeric, PURCHASEPRICE numeric NOT NULL, NOTES TEXT, CURRENTPRICE numeric NOT NULL, "
            "VALUE numeric, COMMISSION numeric)";

            db_->ExecuteUpdate(CREATE_TABLE_STOCK_V1);
        }

        if (load_entries)
        {
            LoadEntriesUsing(wxT("select * from STOCK_V1"));
        }
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogError(wxT("TStockList::LoadEntries: %s"), e.GetMessage().c_str());
    }
}

void TStockList::LoadEntriesUsing(const wxString& sql_statement)
{
    try
    {
        entrylist_.clear();
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
        while (q1.NextRow())
        {
            boost::shared_ptr<TStockEntry> pEntry(new TStockEntry(q1));
            entrylist_.push_back(pEntry);
        }
        q1.Finalize();
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogError(wxT("TStockList::LoadEntriesUsing: %s"), e.GetMessage().c_str());
    }
}

int TStockList::AddEntry(TStockEntry* pStockEntry)
{
    boost::shared_ptr<TStockEntry> pEntry(pStockEntry);
    entrylist_.push_back(pEntry);
    pEntry->Add(ListDatabase());

    return pEntry->id_;
}

void TStockList::DeleteEntry(int stock_id)
{
    boost::shared_ptr<TStockEntry> pEntry = GetEntryPtr(stock_id);
    if (pEntry)
    {
        pEntry->Delete(ListDatabase());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

boost::shared_ptr<TStockEntry> TStockList::GetEntryPtr(int stock_id)
{
    boost::shared_ptr<TStockEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->id_ == stock_id)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

boost::shared_ptr<TStockEntry> TStockList::GetIndexedEntryPtr(unsigned int list_index)
{
    boost::shared_ptr<TStockEntry> pEntry;
    if (list_index < entrylist_.size())
    {
        pEntry = entrylist_[list_index];
    }

    return pEntry;
}

int TStockList::CurrentListSize()
{
    return entrylist_.size();
}

double TStockList::GetStockBalance()
{
    double total_value = 0.0;
    for (size_t i = 0; i < entrylist_.size(); ++i)
    {
        total_value = total_value + entrylist_[i]->value_;
    }

    return total_value;
}

wxString TStockList::GetStockBalanceCurrencyFormat()
{
    wxString balance_str;
    mmex::formatDoubleToCurrency(GetStockBalance(), balance_str);

    return balance_str;
}

wxString TStockList::GetStockBalanceCurrencyEditFormat()
{
    wxString balance_str;
    mmex::formatDoubleToCurrencyEdit(GetStockBalance(), balance_str);

    return balance_str;
}

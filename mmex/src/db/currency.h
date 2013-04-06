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
 Class TCurrencyEntry
 ***********************************************************************************/
class TCurrencyEntry : public TEntryBase
{
private:
    friend class TCurrencyList;    // Allows the class, access to private members.
    
    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);
    void SetDatabaseValues(wxSQLite3Statement& st, int& db_index);

public:
    wxString name_;
    wxString pfxSymbol_;
    wxString sfxSymbol_;
    wxString dec_;
    wxString grp_;
    wxString unit_;
    wxString cent_;
    int scaleDl_;
    double baseConv_;
    wxChar decChar_;
    wxChar grpChar_;
    wxString currencySymbol_;

    /// Constructor used when loading Currency from the database
    TCurrencyEntry(wxSQLite3ResultSet& q1);
    
    /// Constructor for creating a new Currency entry
    TCurrencyEntry();

    void Update(wxSQLite3Database* db);

    void TCurrencyEntry::SetCurrencySettings();
};

/************************************************************************************
 Class TCurrencyList
 ***********************************************************************************/
class TCurrencyList : public TListBase
{
private:
    int basecurrency_id_;
    void LoadEntries();

public:
    std::vector<boost::shared_ptr<TCurrencyEntry> > entrylist_;

    TCurrencyList(boost::shared_ptr<wxSQLite3Database> db);

    int AddEntry(TCurrencyEntry* pCurrencyEntry);
    void SetBaseCurrency(int currency_id);

    /// Note: At this level, no checking is done for usage in other tables.
    void DeleteEntry(int currency_id);
    void DeleteEntry(const wxString& name, bool is_symbol = false);

    boost::shared_ptr<TCurrencyEntry> GetEntryPtr(const wxString& name, bool is_symbol = false);
    boost::shared_ptr<TCurrencyEntry> GetEntryPtr(int currency_id);
    boost::shared_ptr<TCurrencyEntry> GetIndexedEntryPtr(int index);

    int GetCurrencyId(const wxString& name, bool is_symbol = false);
    wxString GetCurrencyName(int currency_id);
    bool CurrencyExists(const wxString& name, bool is_symbol = false);
};

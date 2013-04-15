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

enum ASSET_RATE {NONE, APPRECIATE, DEPRECIATE};
const wxString ASSET_RATE_DEF[] = {
    wxT("None"),
    wxT("Appreciates"),
    wxT("Depreciates")
};

enum ASSET_TYPE {PROPERTY, AUTO, HOUSE, ART, JEWELLERY, CASH, OTHER};
const wxString ASSET_TYPE_DEF[] = {
    wxTRANSLATE("Property"),
    wxTRANSLATE("Automobile"),
    wxTRANSLATE("Household Object"),
    wxTRANSLATE("Art"),
    wxTRANSLATE("Jewellery"),
    wxTRANSLATE("Cash"),
    wxTRANSLATE("Other")
};

/************************************************************************************
 Class TAssetEntry
 ***********************************************************************************/
class TAssetEntry : public TEntryBase
{
private:
    friend class TAssetList;    // Allows TAssetList access to private members
    
    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);
    void SetDatabaseValues(wxSQLite3Statement& st, int& db_index);

    double GetAppreciatedValue(const wxDateTime& startDate, double value, double rate);
    double GetDepreciatedValue(const wxDateTime& startDate, double value, double rate);

public:
    wxString name_;         // wxT("ASSETNAME")
    wxString type_;         // wxT("ASSETTYPE")
    wxString date_;         // wxT("STARTDATE")
    wxString notes_;        // wxT("NOTES")
    double value_;          // wxT("VALUE")
    wxString rate_type_;    // wxT("VALUECHANGE")
    double rate_value_;     // wxT("VALUECHANGERATE")

    /// Calculated value
    wxString display_date_;

    /// Constructor used when loading assets from the database.
    TAssetEntry(wxSQLite3ResultSet& q1);
    /// Copy constructor using a pointer
    TAssetEntry(TAssetEntry* pEntry);
    /// Constructor for creating a new asset entry.
    TAssetEntry();

    void Update(wxSQLite3Database* db);
    double GetValue();
    wxString GetValueCurrencyEditFormat(bool initial_value = false);
    wxString DisplayDate();
};

/************************************************************************************
 Class TAssetList
 ***********************************************************************************/
class TAssetList : public TListBase
{
private:
    void LoadEntries(bool load_entries = true);

public:
    std::vector<wxSharedPtr<TAssetEntry> >entrylist_;

    TAssetList(wxSharedPtr<wxSQLite3Database> db, bool load_entries = true);

    /// Allows specialised loads by providing the required SQL statement
    void LoadAssetEntriesUsing(const wxString& sql_statement);

    int AddEntry(TAssetEntry* pAssetEntry);
    void DeleteEntry(int asset_id);

    wxSharedPtr<TAssetEntry> GetEntryPtr(int asset_id);
    wxSharedPtr<TAssetEntry> GetIndexedEntryPtr(unsigned int list_index);

    int CurrentListSize();
    double GetAssetBalance(bool value_today = true);
    wxString GetAssetBalanceCurrencyFormat(bool value_today = true);
    wxString GetAssetBalanceCurrencyEditFormat(bool value_today = true);
};

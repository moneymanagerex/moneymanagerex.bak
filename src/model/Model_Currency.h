﻿/*******************************************************
 Copyright (C) 2013 Guan Lisheng (guanlisheng@gmail.com)

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

#ifndef MODEL_CURRENCY_H
#define MODEL_CURRENCY_H

#include "Model.h"
#include "db/DB_Table_Currencyformats_V1.h"
#include "Model_Infotable.h" // detect base currency setting BASECURRENCYID
#include <wx/numformatter.h>
#include <tuple>

class Model_Currency : public Model<DB_Table_CURRENCYFORMATS_V1>
{
public:
    Model_Currency();
    ~Model_Currency();

public:
    /**
    * Initialize the global Model_Currency table.
    * Reset the Model_Currency table or create the table if it does not exist.
    */
    static Model_Currency& instance(wxSQLite3Database* db);

    /** Return the static instance of Model_Currency table */
    static Model_Currency& instance();

public:
    wxArrayString all_currency_names();
    wxArrayString all_currency_symbols();

    void initialize();

    static Data* GetBaseCurrency();
    static wxString toCurrency(double value, const Data* currency = GetBaseCurrency());
    static wxString os_group_separator();
    static wxString toString(double value, const Data* currency = GetBaseCurrency());
    static wxString fromString(wxString s, const Data* currency);
    static bool fromString(wxString s, double& val, const Data* currency = GetBaseCurrency());
    static int precision(const Data* r);
    static int precision(const Data& r);

    static std::vector<std::tuple<wxString, wxString, wxString, wxString, wxString, wxString, int, int> > all_currencies_template();
};
#endif // 

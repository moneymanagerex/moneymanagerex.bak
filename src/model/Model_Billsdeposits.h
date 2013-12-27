/*******************************************************
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

#ifndef MODEL_BILLSDEPOSITS_H
#define MODEL_BILLSDEPOSITS_H

#include "Model.h"
#include "db/DB_Table_Billsdeposits_V1.h"
#include "Model_Budgetsplittransaction.h"

const int BD_REPEATS_MULTIPLEX_BASE = 100;

class Model_Billsdeposits : public Model<DB_Table_BILLSDEPOSITS_V1>
{
public:
    using Model<DB_Table_BILLSDEPOSITS_V1>::remove;

public:
    enum TYPE { WITHDRAWAL = 0, DEPOSIT, TRANSFER };
    enum STATUS_ENUM { NONE = 0, RECONCILED, VOID_, FOLLOWUP, DUPLICATE_ };
    enum REPEAT_TYPE {
        REPEAT_INACTIVE = -1,
        REPEAT_NONE,
        REPEAT_WEEKLY,
        REPEAT_BI_WEEKLY,      // FORTNIGHTLY
        REPEAT_MONTHLY,
        REPEAT_BI_MONTHLY,
        REPEAT_QUARTERLY,      // TRI_MONTHLY
        REPEAT_HALF_YEARLY,
        REPEAT_YEARLY,
        REPEAT_FOUR_MONTHLY,   // QUAD_MONTHLY
        REPEAT_FOUR_WEEKLY,    // QUAD_WEEKLY
        REPEAT_DAILY,
        REPEAT_IN_X_DAYS,
        REPEAT_IN_X_MONTHS,
        REPEAT_EVERY_X_DAYS,
        REPEAT_EVERY_X_MONTHS,
        REPEAT_MONTHLY_LAST_DAY,
        REPEAT_MONTHLY_LAST_BUSINESS_DAY
    };

    static const std::vector<std::pair<TYPE, wxString> > TYPE_CHOICES;
    static const std::vector<std::pair<STATUS_ENUM, wxString> > STATUS_ENUM_CHOICES;

public:
    Model_Billsdeposits();
    ~Model_Billsdeposits();

public:
    struct Full_Data : public Data
    {
        Full_Data(): Data(0)
        {}
        Full_Data(const Data& r): Data(r)
        {}
        wxString ACCOUNTNAME;
        wxString PAYEENAME;
        wxString CATEGNAME;
    };
    typedef std::vector<Full_Data> Full_Data_Set;

public:
    static wxArrayString all_type();
    static wxArrayString all_status();

public:
    /**
    * Initialize the global Model_Billsdeposits table.
    * Reset the Model_Billsdeposits table or create the table if it does not exist.
    */
    static Model_Billsdeposits& instance(wxSQLite3Database* db);

    /** Return the static instance of Model_Billsdeposits table */
    static Model_Billsdeposits& instance();

public:
    static wxDate NEXTOCCURRENCEDATE(const Data* r);
    static wxDate NEXTOCCURRENCEDATE(const Data& r);
    static TYPE type(const Data* r);
    static TYPE type(const Data& r);
    static STATUS_ENUM status(const Data* r);
    static STATUS_ENUM status(const Data& r);
    static wxString toShortStatus(const wxString& fullStatus);

public:
    /**
    * Remove the Data record instance from memory and the database
    * including any splits associated with the Data Record.
    */
    bool remove(int id);

    static DB_Table_BILLSDEPOSITS_V1::STATUS STATUS(STATUS_ENUM status, OP op = EQUAL);
    static DB_Table_BILLSDEPOSITS_V1::TRANSCODE TRANSCODE(TYPE type, OP op = EQUAL);

    static Model_Budgetsplittransaction::Data_Set splittransaction(const Data* r);
    static Model_Budgetsplittransaction::Data_Set splittransaction(const Data& r);

    void completeBDInSeries(int bdID);
};

#endif // 

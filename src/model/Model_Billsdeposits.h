// -*- C++ -*-
//=============================================================================
/**
 *      Copyright (c) 2013 Guan Lisheng (guanlisheng@gmail.com)
 *
 *      @file
 *
 *      @author [sqlite2cpp.py]
 *
 *      @brief
 *
 *      Revision History:
 *          AUTO GENERATED at 2013-07-03 22:55:49.491495.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef MODEL_BILLSDEPOSITS_H
#define MODEL_BILLSDEPOSITS_H

#include "Model.h"
#include "db/DB_Table_Billsdeposits_V1.h"

class Model_Billsdeposits : public Model, public DB_Table_BILLSDEPOSITS_V1
{
    using DB_Table_BILLSDEPOSITS_V1::all;
    using DB_Table_BILLSDEPOSITS_V1::find;
    using DB_Table_BILLSDEPOSITS_V1::get;
public:
    Model_Billsdeposits(): Model(), DB_Table_BILLSDEPOSITS_V1() 
    {
    };
    ~Model_Billsdeposits() {};

public:
    wxArrayString types_;

public:
    static Model_Billsdeposits& instance()
    {
        return Singleton<Model_Billsdeposits>::instance();
    }
    static Model_Billsdeposits& instance(wxSQLite3Database* db)
    {
        Model_Billsdeposits& ins = Singleton<Model_Billsdeposits>::instance();
        ins.db_ = db;
        return ins;
    }
public:
    Data_Set all(COLUMN col = COLUMN(0), bool asc = true)
    {
        this->ensure(this->db_);
        return all(db_, col, asc);
    }
    template<class V>
    Data_Set find(COLUMN col, const V& v)
    {
        return find(db_, col, v);
    }
    Data* get(int id)
    {
        return this->get(id, this->db_);
    }
    int save(Data* r)
    {
        r->save(this->db_);
        return r->id();
    }
};

#endif // 

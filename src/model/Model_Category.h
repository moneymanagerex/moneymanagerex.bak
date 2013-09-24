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

#ifndef MODEL_CATEGORY_H
#define MODEL_CATEGORY_H

#include "Model.h"
#include "db/DB_Table_Category_V1.h"
#include "Model_Subcategory.h"

class Model_Category : public Model, public DB_Table_CATEGORY_V1
{
    using DB_Table_CATEGORY_V1::all;
    using DB_Table_CATEGORY_V1::find;
    using DB_Table_CATEGORY_V1::get;
public:
    Model_Category(): Model(), DB_Table_CATEGORY_V1() 
    {
    };
    ~Model_Category() {};

public:
    wxArrayString types_;

public:
    static Model_Category& instance()
    {
        return Singleton<Model_Category>::instance();
    }
    static Model_Category& instance(wxSQLite3Database* db)
    {
        Model_Category& ins = Singleton<Model_Category>::instance();
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
    int save(Data* asset)
    {
        asset->save(this->db_);
        return asset->id();
    }
    Model_Subcategory::Data_Set sub_category(const Data* r)
    {
        return Model_Subcategory::instance().find(Model_Subcategory::COL_CATEGID, r->CATEGID);
    }
};

#endif // 

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
 *          AUTO GENERATED at 2013-11-09 10:04:32.192407.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef DB_TABLE_BUDGETTABLE_V1_H
#define DB_TABLE_BUDGETTABLE_V1_H

#include "DB_Table.h"

struct DB_Table_BUDGETTABLE_V1 : public DB_Table
{
    struct Data;
    typedef DB_Table_BUDGETTABLE_V1 Self;
    /** A container to hold list of Data records for the table*/
    typedef std::vector<Self::Data> Data_Set;
    /** A container to hold a list of Data record pointers for the table in memory*/
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_Table_BUDGETTABLE_V1() 
    {
        destroy_cache();
    }
    
    /** Removes all table data stored in memory*/ 
    void destroy_cache()
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
        cache_.clear();
    }

    /** Creates the database table if the table does not exist*/
    bool ensure(wxSQLite3Database* db)
    {
        if (exists(db)) return true;
        destroy_cache();

        try
        {
            db->ExecuteUpdate("CREATE TABLE BUDGETTABLE_V1(BUDGETENTRYID integer primary key, BUDGETYEARID integer, CATEGID integer, SUBCATEGID integer, PERIOD TEXT NOT NULL, AMOUNT numeric NOT NULL)");
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("BUDGETTABLE_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        this->ensure_index(db);

        return true;
    }

    bool ensure_index(wxSQLite3Database* db)
    {
        try
        {

        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("BUDGETTABLE_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct BUDGETENTRYID : public DB_Column<int>
    { 
        static wxString name() { return "BUDGETENTRYID"; } 
        BUDGETENTRYID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct BUDGETYEARID : public DB_Column<int>
    { 
        static wxString name() { return "BUDGETYEARID"; } 
        BUDGETYEARID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct CATEGID : public DB_Column<int>
    { 
        static wxString name() { return "CATEGID"; } 
        CATEGID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct SUBCATEGID : public DB_Column<int>
    { 
        static wxString name() { return "SUBCATEGID"; } 
        SUBCATEGID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct PERIOD : public DB_Column<wxString>
    { 
        static wxString name() { return "PERIOD"; } 
        PERIOD(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct AMOUNT : public DB_Column<double>
    { 
        static wxString name() { return "AMOUNT"; } 
        AMOUNT(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    typedef BUDGETENTRYID PRIMARY;
    enum COLUMN
    {
        COL_BUDGETENTRYID = 0
        , COL_BUDGETYEARID = 1
        , COL_CATEGID = 2
        , COL_SUBCATEGID = 3
        , COL_PERIOD = 4
        , COL_AMOUNT = 5
    };

    /** Returns the column name as a string*/
    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_BUDGETENTRYID: return "BUDGETENTRYID";
            case COL_BUDGETYEARID: return "BUDGETYEARID";
            case COL_CATEGID: return "CATEGID";
            case COL_SUBCATEGID: return "SUBCATEGID";
            case COL_PERIOD: return "PERIOD";
            case COL_AMOUNT: return "AMOUNT";
            default: break;
        }
        
        return "UNKNOWN";
    }

    /** Returns the comumn number from the given column name*/
    COLUMN name_to_column(const wxString& name) const
    {
        if ("BUDGETENTRYID" == name) return COL_BUDGETENTRYID;
        else if ("BUDGETYEARID" == name) return COL_BUDGETYEARID;
        else if ("CATEGID" == name) return COL_CATEGID;
        else if ("SUBCATEGID" == name) return COL_SUBCATEGID;
        else if ("PERIOD" == name) return COL_PERIOD;
        else if ("AMOUNT" == name) return COL_AMOUNT;

        return COLUMN(-1);
    }
    
    /** Contains the table record for the table*/
    struct Data
    {
        friend struct DB_Table_BUDGETTABLE_V1;
        Self* view_;
    
        int BUDGETENTRYID;//  primay key
        int BUDGETYEARID;
        int CATEGID;
        int SUBCATEGID;
        wxString PERIOD;
        double AMOUNT;
        int id() const { return BUDGETENTRYID; }
        void id(int id) { BUDGETENTRYID = id; }
        bool operator < (const Data& r) const
        {
            return this->id() < r.id();
        }
        bool operator < (const Data* r) const
        {
            return this->id() < r->id();
        }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            BUDGETENTRYID = -1;
            BUDGETYEARID = -1;
            CATEGID = -1;
            SUBCATEGID = -1;
            AMOUNT = 0.0;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            BUDGETENTRYID = q.GetInt("BUDGETENTRYID");
            BUDGETYEARID = q.GetInt("BUDGETYEARID");
            CATEGID = q.GetInt("CATEGID");
            SUBCATEGID = q.GetInt("SUBCATEGID");
            PERIOD = q.GetString("PERIOD");
            AMOUNT = q.GetDouble("AMOUNT");
        }

        wxString to_json() const
        {
            json::Object o;
            this->to_json(o);
            std::stringstream ss;
            json::Writer::Write(o, ss);
            return ss.str();
        }
        
        int to_json(json::Object& o) const
        {
            o["BUDGETENTRYID"] = json::Number(this->BUDGETENTRYID);
            o["BUDGETYEARID"] = json::Number(this->BUDGETYEARID);
            o["CATEGID"] = json::Number(this->CATEGID);
            o["SUBCATEGID"] = json::Number(this->SUBCATEGID);
            o["PERIOD"] = json::String(this->PERIOD.ToStdString());
            o["AMOUNT"] = json::Number(this->AMOUNT);
            return 0;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not save");
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not remove");
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 6
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    /** Name of the table*/    
    wxString name() const { return "BUDGETTABLE_V1"; }

    DB_Table_BUDGETTABLE_V1() 
    {
        query_ = "SELECT BUDGETENTRYID, BUDGETYEARID, CATEGID, SUBCATEGID, PERIOD, AMOUNT FROM BUDGETTABLE_V1 ";
    }

    /** Create a new Data record and add to memory table (cache)*/
    Self::Data* create()
    {
        Self::Data* entity = new Self::Data(this);
        cache_.push_back(entity);
        return entity;
    }
    
    /** Create a copy of the Data record and add to memory table (cache)*/
    Self::Data* clone(const Data* e)
    {
        Self::Data* entity = create();
        *entity = *e;
        entity->id(-1);
        return entity;
    }

    /** Saves the Data record to the database.
      * Either create a new record or update the existing record.
    */
    bool save(Self::Data* entity, wxSQLite3Database* db)
    {
        wxString sql = wxEmptyString;
        if (entity->id() < 0) //  new & insert
        {
            sql = "INSERT INTO BUDGETTABLE_V1(BUDGETYEARID, CATEGID, SUBCATEGID, PERIOD, AMOUNT) VALUES(?, ?, ?, ?, ?)";
        }
        else
        {
            sql = "UPDATE BUDGETTABLE_V1 SET BUDGETYEARID = ?, CATEGID = ?, SUBCATEGID = ?, PERIOD = ?, AMOUNT = ? WHERE BUDGETENTRYID = ?";
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->BUDGETYEARID);
            stmt.Bind(2, entity->CATEGID);
            stmt.Bind(3, entity->SUBCATEGID);
            stmt.Bind(4, entity->PERIOD);
            stmt.Bind(5, entity->AMOUNT);
            if (entity->id() > 0)
                stmt.Bind(6, entity->BUDGETENTRYID);

            wxLogDebug(stmt.GetSQL());
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("BUDGETTABLE_V1: Exception %s, %s", e.GetMessage().c_str(), entity->to_json());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }

    /** Remove the Data record from the database and the memory table (cashe)*/
    bool remove(int id, wxSQLite3Database* db)
    {
        if (id < 0) return false;
        try
        {
            wxString sql = "DELETE FROM BUDGETTABLE_V1 WHERE BUDGETENTRYID = ?";
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            wxLogDebug(stmt.GetSQL());
            stmt.ExecuteUpdate();
            stmt.Finalize();

            Cache c;
            for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
            {
                Self::Data* entity = *it;
                if (entity->id() == id) 
                    delete entity;
                else 
                    c.push_back(entity);
            }
            cache_.clear();
            cache_.swap(c);
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("BUDGETTABLE_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    /** Remove the Data record from the database and the memory table (cashe)*/
    bool remove(Self::Data* entity, wxSQLite3Database* db)
    {
        if (remove(entity->id(), db))
        {
            entity->id(-1);
            return true;
        }

        return false;
    }

    
    /** Search the memory table (Cache) for the data record.
      * If not found in memory, search the database and update the cache.
    */
    Self::Data* get(int id, wxSQLite3Database* db)
    {
        if (id < 0) 
        {
            ++ skip_;
            wxLogDebug("%s :%d SKIP (hit %ld, miss %ld, skip %ld)", this->name(), id, this->hit_, this->miss_, this->skip_);
            return 0;
        }
        for(Cache::reverse_iterator it = cache_.rbegin(); it != cache_.rend(); ++ it)
        {
            Self::Data* entity = *it;
            if (entity->id() == id) 
            {
                ++ hit_;
                wxLogDebug("%s :%d HIT (hit %ld, miss %ld, skip %ld)", this->name(), id, this->hit_, this->miss_, this->skip_);
                return entity;
            }
        }
        
        ++ miss_;
        wxLogDebug("%s :%d MISS (hit %ld, miss %ld, skip %ld)", this->name(), id, this->hit_, this->miss_, this->skip_);
        Self::Data* entity = 0;
        wxString where = wxString::Format(" WHERE %s = ?", PRIMARY::name().c_str());
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + where);
            stmt.Bind(1, id);

            wxLogDebug(stmt.GetSQL());
            wxSQLite3ResultSet q = stmt.ExecuteQuery();
            if(q.NextRow())
            {
                entity = new Self::Data(q, this);
                cache_.push_back(entity);
            }
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("%s: Exception %s", this->name().c_str(), e.GetMessage().c_str());
        }
        
        if (!entity) 
        {
            wxLogError("%s: %d not found", this->name().c_str(), id);
        }
 
        return entity;
    }

    /** Return a list of Data records (Data_Set) derived directly from the database.
      * The Data_Set is sorted based on the column number.
    */
    Data_Set all(wxSQLite3Database* db, COLUMN col = COLUMN(0), bool asc = true)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + " ORDER BY " + column_to_name(col) + " COLLATE NOCASE " + (asc ? " ASC " : " DESC ")
                + "," + PRIMARY::name());

            wxLogDebug(q.GetSQL());
            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("%s: Exception %s", this->name().c_str(), e.GetMessage().c_str());
        }

        return result;
    }

};
#endif //

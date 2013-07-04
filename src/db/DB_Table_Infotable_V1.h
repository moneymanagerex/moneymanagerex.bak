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

#ifndef DB_TABLE_INFOTABLE_V1_H
#define DB_TABLE_INFOTABLE_V1_H

#include "DB_Table.h"

struct DB_Table_INFOTABLE_V1 : public DB_Table
{
    struct Data;
    typedef DB_Table_INFOTABLE_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_Table_INFOTABLE_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate("CREATE TABLE INFOTABLE_V1(INFOID integer not null primary key, INFONAME TEXT NOT NULL UNIQUE, INFOVALUE TEXT NOT NULL)");
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("INFOTABLE_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct INFOID { wxString name() const { return "INFOID"; } };
    struct INFONAME { wxString name() const { return "INFONAME"; } };
    struct INFOVALUE { wxString name() const { return "INFOVALUE"; } };
    typedef INFOID PRIMARY;
    enum COLUMN
    {
        COL_INFOID = 0
        , COL_INFONAME = 1
        , COL_INFOVALUE = 2
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_INFOID);
        result.push_back(COL_INFONAME);
        result.push_back(COL_INFOVALUE);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_INFOID: return "INFOID";
            case COL_INFONAME: return "INFONAME";
            case COL_INFOVALUE: return "INFOVALUE";
            default: break;
        }
        
        return "UNKNOWN";
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if ("INFOID" == name) return COL_INFOID;
        else if ("INFONAME" == name) return COL_INFONAME;
        else if ("INFOVALUE" == name) return COL_INFOVALUE;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int INFOID;//  primay key
        wxString INFONAME;
        wxString INFOVALUE;
        int id() const { return INFOID; }
        void id(int id) { INFOID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            INFOID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            INFOID = q.GetInt("INFOID");
            INFONAME = q.GetString("INFONAME");
            INFOVALUE = q.GetString("INFOVALUE");
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_INFOID: ret << INFOID; break;
                case COL_INFONAME: ret << INFONAME; break;
                case COL_INFOVALUE: ret << INFOVALUE; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = ",") const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = ",") const
        {
            wxString ret = wxEmptyString;
            ret << INFOID;
            ret << delimiter << INFONAME;
            ret << delimiter << INFOVALUE;
            return ret;
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
        NUM_COLUMNS = 3
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return "INFOTABLE_V1"; }

    DB_Table_INFOTABLE_V1() 
    {
        query_ = "SELECT INFOID, INFONAME, INFOVALUE FROM INFOTABLE_V1 ";
    }

    Self::Data* create()
    {
        Self::Data* entity = new Self::Data(this);
        cache_.push_back(entity);
        return entity;
    }

    bool save(Self::Data* entity, wxSQLite3Database* db)
    {
        wxString sql = wxEmptyString;
        if (entity->id() < 0) //  new & insert
        {
            sql = "INSERT INTO INFOTABLE_V1(INFONAME, INFOVALUE) VALUES(?, ?)";
        }
        else
        {
            sql = "UPDATE INFOTABLE_V1 SET INFONAME = ?, INFOVALUE = ? WHERE INFOID = ?";
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->INFONAME);
            stmt.Bind(2, entity->INFOVALUE);
            if (entity->id() > 0)
                stmt.Bind(3, entity->INFOID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("INFOTABLE_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = "DELETE FROM INFOTABLE_V1 WHERE INFOID = ?";
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
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
            wxLogError("INFOTABLE_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    bool remove(Self::Data* entity, wxSQLite3Database* db)
    {
        if (remove(entity->id(), db))
        {
            entity->id(-1);
            return true;
        }

        return false;
    }

    
    Self::Data* get(int id, wxSQLite3Database* db)
    {
        for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
        {
            Self::Data* entity = *it;
            if (entity->id() == id) 
                return entity;
        }

        Self::Data* entity = 0;
        wxString where = wxString::Format(" WHERE INFOID = ?");
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + where);
            stmt.Bind(1, id);

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
            wxLogError("INFOTABLE_V1: Exception %s", e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class V>
    Data_Set find(wxSQLite3Database* db, COLUMN col, const V& v)
    {
        Data_Set result;
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + " WHERE " 
                + column_to_name(col) + " = ?"
                + " ORDER BY " + column_to_name(col)
                );
            stmt.Bind(1, v);
            wxSQLite3ResultSet q = stmt.ExecuteQuery();

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("INFOTABLE_V1: Exception %s", e.GetMessage().c_str());
        }

        return result;
    }

    template<class V1, class V2>
    Data_Set find(wxSQLite3Database* db, COLUMN col1, const V1& v1, COLUMN col2, const V2& v2, bool op_and = true)
    {
        Data_Set result;
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + " WHERE "
                                                                + column_to_name(col1) + " = ? "
                                                                + (op_and ? " AND " : " OR ")
                                                                + column_to_name(col2) + " = ?"
                                                                + " ORDER BY " + column_to_name(col1)
                                                                + "," + column_to_name(col2)
                                                                );
            stmt.Bind(1, v1);
            stmt.Bind(2, v2);
            wxSQLite3ResultSet q = stmt.ExecuteQuery();

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("INFOTABLE_V1: Exception %s", e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, COLUMN col = COLUMN(0), bool asc = true)
    {
        Data_Set result;
        PRIMARY primay;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + " ORDER BY " + column_to_name(col) + (asc ? " ASC " : " DESC ")
                + "," + primay.name());

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("INFOTABLE_V1: Exception %s", e.GetMessage().c_str());
        }

        return result;
    }

};
#endif //
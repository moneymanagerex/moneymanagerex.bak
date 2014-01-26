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
 *          AUTO GENERATED at 2014-01-26 18:05:09.061783.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef DB_TABLE_REPORT_V1_H
#define DB_TABLE_REPORT_V1_H

#include "DB_Table.h"

struct DB_Table_REPORT_V1 : public DB_Table
{
    struct Data;
    typedef DB_Table_REPORT_V1 Self;
    /** A container to hold list of Data records for the table*/
    typedef std::vector<Self::Data> Data_Set;
    /** A container to hold a list of Data record pointers for the table in memory*/
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;

    /** Destructor: clears any data records stored in memory */
    ~DB_Table_REPORT_V1() 
    {
        destroy_cache();
    }
    
    /** Show table statistics*/
	void show_statistics()
	{
		size_t cache_size = this->cache_.size();
#ifdef _WIN64
        wxLogDebug("%s : (cache %llu, hit %llu, miss %llu, skip %llu)", this->name(), cache_size, this->hit_, this->miss_, this->skip_);
#else
        wxLogDebug("%s : (cache %lu, hit %lu, miss %lu, skip %lu)", this->name(), cache_size, this->hit_, this->miss_, this->skip_);
#endif
	}
	 
    /** Removes all records stored in memory (cache) for the table*/ 
    void destroy_cache()
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
        cache_.clear();
    }

    /** Creates the database table if the table does not exist*/
    bool ensure(wxSQLite3Database* db)
    {
        if (!exists(db))
		{
			try
			{
				db->ExecuteUpdate("CREATE TABLE REPORT_V1(REPORTID integer not null primary key, REPORTNAME TEXT COLLATE NOCASE NOT NULL UNIQUE, STATUS TEXT COLLATE NOCASE NOT NULL, GROUPNAME TEXT COLLATE NOCASE, SQLCONTENT TEXT, LUACONTENT TEXT, TEMPLATEPATH TEXT)");
			}
			catch(const wxSQLite3Exception &e) 
			{ 
				wxLogError("REPORT_V1: Exception %s", e.GetMessage().c_str());
				return false;
			}
		}

        this->ensure_index(db);

        return true;
    }

    bool ensure_index(wxSQLite3Database* db)
    {
        try
        {
            db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS INDEX_REPORT_NAME ON REPORT_V1(REPORTNAME)");
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("REPORT_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct REPORTID : public DB_Column<int>
    { 
        static wxString name() { return "REPORTID"; } 
        REPORTID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct REPORTNAME : public DB_Column<wxString>
    { 
        static wxString name() { return "REPORTNAME"; } 
        REPORTNAME(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct STATUS : public DB_Column<wxString>
    { 
        static wxString name() { return "STATUS"; } 
        STATUS(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct GROUPNAME : public DB_Column<wxString>
    { 
        static wxString name() { return "GROUPNAME"; } 
        GROUPNAME(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct SQLCONTENT : public DB_Column<wxString>
    { 
        static wxString name() { return "SQLCONTENT"; } 
        SQLCONTENT(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct LUACONTENT : public DB_Column<wxString>
    { 
        static wxString name() { return "LUACONTENT"; } 
        LUACONTENT(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct TEMPLATEPATH : public DB_Column<wxString>
    { 
        static wxString name() { return "TEMPLATEPATH"; } 
        TEMPLATEPATH(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    typedef REPORTID PRIMARY;
    enum COLUMN
    {
        COL_REPORTID = 0
        , COL_REPORTNAME = 1
        , COL_STATUS = 2
        , COL_GROUPNAME = 3
        , COL_SQLCONTENT = 4
        , COL_LUACONTENT = 5
        , COL_TEMPLATEPATH = 6
    };

    /** Returns the column name as a string*/
    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_REPORTID: return "REPORTID";
            case COL_REPORTNAME: return "REPORTNAME";
            case COL_STATUS: return "STATUS";
            case COL_GROUPNAME: return "GROUPNAME";
            case COL_SQLCONTENT: return "SQLCONTENT";
            case COL_LUACONTENT: return "LUACONTENT";
            case COL_TEMPLATEPATH: return "TEMPLATEPATH";
            default: break;
        }
        
        return "UNKNOWN";
    }

    /** Returns the comumn number from the given column name*/
    COLUMN name_to_column(const wxString& name) const
    {
        if ("REPORTID" == name) return COL_REPORTID;
        else if ("REPORTNAME" == name) return COL_REPORTNAME;
        else if ("STATUS" == name) return COL_STATUS;
        else if ("GROUPNAME" == name) return COL_GROUPNAME;
        else if ("SQLCONTENT" == name) return COL_SQLCONTENT;
        else if ("LUACONTENT" == name) return COL_LUACONTENT;
        else if ("TEMPLATEPATH" == name) return COL_TEMPLATEPATH;

        return COLUMN(-1);
    }
    
    /** Data is a single record in the database table*/
    struct Data
    {
        friend struct DB_Table_REPORT_V1;
        /** This is a instance pointer to itself in memory. */
        Self* view_;
    
        int REPORTID;//  primay key
        wxString REPORTNAME;
        wxString STATUS;
        wxString GROUPNAME;
        wxString SQLCONTENT;
        wxString LUACONTENT;
        wxString TEMPLATEPATH;
        int id() const { return REPORTID; }
        void id(int id) { REPORTID = id; }
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
        
            REPORTID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            REPORTID = q.GetInt("REPORTID");
            REPORTNAME = q.GetString("REPORTNAME");
            STATUS = q.GetString("STATUS");
            GROUPNAME = q.GetString("GROUPNAME");
            SQLCONTENT = q.GetString("SQLCONTENT");
            LUACONTENT = q.GetString("LUACONTENT");
            TEMPLATEPATH = q.GetString("TEMPLATEPATH");
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
            o["REPORTID"] = json::Number(this->REPORTID);
            o["REPORTNAME"] = json::String(this->REPORTNAME.ToStdString());
            o["STATUS"] = json::String(this->STATUS.ToStdString());
            o["GROUPNAME"] = json::String(this->GROUPNAME.ToStdString());
            o["SQLCONTENT"] = json::String(this->SQLCONTENT.ToStdString());
            o["LUACONTENT"] = json::String(this->LUACONTENT.ToStdString());
            o["TEMPLATEPATH"] = json::String(this->TEMPLATEPATH.ToStdString());
            return 0;
        }
        row_t to_row_t() const
        {
            row_t row;
            row("REPORTID") = REPORTID;
            row("REPORTNAME") = REPORTNAME;
            row("STATUS") = STATUS;
            row("GROUPNAME") = GROUPNAME;
            row("SQLCONTENT") = SQLCONTENT;
            row("LUACONTENT") = LUACONTENT;
            row("TEMPLATEPATH") = TEMPLATEPATH;
            return row;
        }

        /** Save the record instance in memory to the database. */
        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not save REPORT_V1");
                return false;
            }

            return view_->save(this, db);
        }

        /** Remove the record instance from memory and the database. */
        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not remove REPORT_V1");
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 7
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    /** Name of the table*/    
    wxString name() const { return "REPORT_V1"; }

    DB_Table_REPORT_V1() 
    {
        query_ = "SELECT * FROM REPORT_V1 ";
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

    /**
    * Saves the Data record to the database table.
    * Either create a new record or update the existing record.
    * Remove old record from the memory table (cache)
    */
    bool save(Self::Data* entity, wxSQLite3Database* db)
    {
        wxString sql = wxEmptyString;
        if (entity->id() <= 0) //  new & insert
        {
            sql = "INSERT INTO REPORT_V1(REPORTNAME, STATUS, GROUPNAME, SQLCONTENT, LUACONTENT, TEMPLATEPATH) VALUES(?, ?, ?, ?, ?, ?)";
        }
        else
        {
            sql = "UPDATE REPORT_V1 SET REPORTNAME = ?, STATUS = ?, GROUPNAME = ?, SQLCONTENT = ?, LUACONTENT = ?, TEMPLATEPATH = ? WHERE REPORTID = ?";
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->REPORTNAME);
            stmt.Bind(2, entity->STATUS);
            stmt.Bind(3, entity->GROUPNAME);
            stmt.Bind(4, entity->SQLCONTENT);
            stmt.Bind(5, entity->LUACONTENT);
            stmt.Bind(6, entity->TEMPLATEPATH);
            if (entity->id() > 0)
                stmt.Bind(7, entity->REPORTID);

            //wxLogDebug(stmt.GetSQL());
            stmt.ExecuteUpdate();
            stmt.Finalize();

            if (entity->id() > 0)
            {
                Cache c;
                for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
                {
                    Self::Data* e = *it;
                    if (e->id() == entity->id() && e != entity) 
                        delete e;
                    else 
                        c.push_back(e);
                }
                cache_.clear();
                cache_.swap(c);
            }
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("REPORT_V1: Exception %s, %s", e.GetMessage().c_str(), entity->to_json());
            return false;
        }

        if (entity->id() <= 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }

    /** Remove the Data record from the database and the memory table (cache) */
    bool remove(int id, wxSQLite3Database* db)
    {
        if (id <= 0) return false;
        try
        {
            wxString sql = "DELETE FROM REPORT_V1 WHERE REPORTID = ?";
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            //wxLogDebug(stmt.GetSQL());
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
            wxLogError("REPORT_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    /** Remove the Data record from the database and the memory table (cache) */
    bool remove(Self::Data* entity, wxSQLite3Database* db)
    {
        if (remove(entity->id(), db))
        {
            entity->id(-1);
            return true;
        }

        return false;
    }

    
    /**
    * Search the memory table (Cache) for the data record.
    * If not found in memory, search the database and update the cache.
    */
    Self::Data* get(int id, wxSQLite3Database* db)
    {
        if (id <= 0) 
        {
            ++ skip_;
            return 0;
        }
        for(Cache::reverse_iterator it = cache_.rbegin(); it != cache_.rend(); ++ it)
        {
            Self::Data* entity = *it;
            if (entity->id() == id) 
            {
                ++ hit_;
                return entity;
            }
        }
        
        ++ miss_;
        Self::Data* entity = 0;
        wxString where = wxString::Format(" WHERE %s = ?", PRIMARY::name().c_str());
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + where);
            stmt.Bind(1, id);

            //wxLogDebug(stmt.GetSQL());
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

    /**
    * Return a list of Data records (Data_Set) derived directly from the database.
    * The Data_Set is sorted based on the column number.
    */
    Data_Set all(wxSQLite3Database* db, COLUMN col = COLUMN(0), bool asc = true)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(col == COLUMN(0) ? this->query() : this->query() + " ORDER BY " + column_to_name(col) + " COLLATE NOCASE " + (asc ? " ASC " : " DESC "));

            //wxLogDebug(q.GetSQL());
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

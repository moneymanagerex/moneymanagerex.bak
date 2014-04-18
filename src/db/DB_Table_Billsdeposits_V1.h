// -*- C++ -*-
//=============================================================================
/**
 *      Copyright (c) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 *
 *      @file
 *
 *      @author [sqlite2cpp.py]
 *
 *      @brief
 *
 *      Revision History:
 *          AUTO GENERATED at 2014-04-18 14:26:43.834130.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef DB_TABLE_BILLSDEPOSITS_V1_H
#define DB_TABLE_BILLSDEPOSITS_V1_H

#include "DB_Table.h"

struct DB_Table_BILLSDEPOSITS_V1 : public DB_Table
{
    struct Data;
    typedef DB_Table_BILLSDEPOSITS_V1 Self;
    /** A container to hold list of Data records for the table*/
    struct Data_Set : public std::vector<Self::Data>
    {
        std::string to_json() const
        {
            json::Array a;
            for (const auto & item: *this)
            {
                json::Object o;
                item.to_json(o);
                a.Insert(o);
            }
            std::stringstream ss;
            json::Writer::Write(a, ss);
            return ss.str();
        }
    };
    /** A container to hold a list of Data record pointers for the table in memory*/
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;

    /** Destructor: clears any data records stored in memory */
    ~DB_Table_BILLSDEPOSITS_V1() 
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
				db->ExecuteUpdate("CREATE TABLE BILLSDEPOSITS_V1(BDID integer primary key, ACCOUNTID integer NOT NULL, TOACCOUNTID integer, PAYEEID integer NOT NULL, TRANSCODE TEXT NOT NULL, TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT, CATEGID integer, SUBCATEGID integer, TRANSDATE TEXT, FOLLOWUPID integer, TOTRANSAMOUNT numeric, REPEATS integer, NEXTOCCURRENCEDATE TEXT, NUMOCCURRENCES integer )");
			}
			catch(const wxSQLite3Exception &e) 
			{ 
				wxLogError("BILLSDEPOSITS_V1: Exception %s", e.GetMessage().c_str());
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
            db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS IDX_BILLSDEPOSITS_ACCOUNT ON BILLSDEPOSITS_V1 (ACCOUNTID, TOACCOUNTID)");
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("BILLSDEPOSITS_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct BDID : public DB_Column<int>
    { 
        static wxString name() { return "BDID"; } 
        BDID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct ACCOUNTID : public DB_Column<int>
    { 
        static wxString name() { return "ACCOUNTID"; } 
        ACCOUNTID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct TOACCOUNTID : public DB_Column<int>
    { 
        static wxString name() { return "TOACCOUNTID"; } 
        TOACCOUNTID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct PAYEEID : public DB_Column<int>
    { 
        static wxString name() { return "PAYEEID"; } 
        PAYEEID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct TRANSCODE : public DB_Column<wxString>
    { 
        static wxString name() { return "TRANSCODE"; } 
        TRANSCODE(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct TRANSAMOUNT : public DB_Column<double>
    { 
        static wxString name() { return "TRANSAMOUNT"; } 
        TRANSAMOUNT(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    struct STATUS : public DB_Column<wxString>
    { 
        static wxString name() { return "STATUS"; } 
        STATUS(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct TRANSACTIONNUMBER : public DB_Column<wxString>
    { 
        static wxString name() { return "TRANSACTIONNUMBER"; } 
        TRANSACTIONNUMBER(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct NOTES : public DB_Column<wxString>
    { 
        static wxString name() { return "NOTES"; } 
        NOTES(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
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
    struct TRANSDATE : public DB_Column<wxString>
    { 
        static wxString name() { return "TRANSDATE"; } 
        TRANSDATE(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct FOLLOWUPID : public DB_Column<int>
    { 
        static wxString name() { return "FOLLOWUPID"; } 
        FOLLOWUPID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct TOTRANSAMOUNT : public DB_Column<double>
    { 
        static wxString name() { return "TOTRANSAMOUNT"; } 
        TOTRANSAMOUNT(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    struct REPEATS : public DB_Column<int>
    { 
        static wxString name() { return "REPEATS"; } 
        REPEATS(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct NEXTOCCURRENCEDATE : public DB_Column<wxString>
    { 
        static wxString name() { return "NEXTOCCURRENCEDATE"; } 
        NEXTOCCURRENCEDATE(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct NUMOCCURRENCES : public DB_Column<int>
    { 
        static wxString name() { return "NUMOCCURRENCES"; } 
        NUMOCCURRENCES(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    typedef BDID PRIMARY;
    enum COLUMN
    {
        COL_BDID = 0
        , COL_ACCOUNTID = 1
        , COL_TOACCOUNTID = 2
        , COL_PAYEEID = 3
        , COL_TRANSCODE = 4
        , COL_TRANSAMOUNT = 5
        , COL_STATUS = 6
        , COL_TRANSACTIONNUMBER = 7
        , COL_NOTES = 8
        , COL_CATEGID = 9
        , COL_SUBCATEGID = 10
        , COL_TRANSDATE = 11
        , COL_FOLLOWUPID = 12
        , COL_TOTRANSAMOUNT = 13
        , COL_REPEATS = 14
        , COL_NEXTOCCURRENCEDATE = 15
        , COL_NUMOCCURRENCES = 16
    };

    /** Returns the column name as a string*/
    static wxString column_to_name(COLUMN col)
    {
        switch(col)
        {
            case COL_BDID: return "BDID";
            case COL_ACCOUNTID: return "ACCOUNTID";
            case COL_TOACCOUNTID: return "TOACCOUNTID";
            case COL_PAYEEID: return "PAYEEID";
            case COL_TRANSCODE: return "TRANSCODE";
            case COL_TRANSAMOUNT: return "TRANSAMOUNT";
            case COL_STATUS: return "STATUS";
            case COL_TRANSACTIONNUMBER: return "TRANSACTIONNUMBER";
            case COL_NOTES: return "NOTES";
            case COL_CATEGID: return "CATEGID";
            case COL_SUBCATEGID: return "SUBCATEGID";
            case COL_TRANSDATE: return "TRANSDATE";
            case COL_FOLLOWUPID: return "FOLLOWUPID";
            case COL_TOTRANSAMOUNT: return "TOTRANSAMOUNT";
            case COL_REPEATS: return "REPEATS";
            case COL_NEXTOCCURRENCEDATE: return "NEXTOCCURRENCEDATE";
            case COL_NUMOCCURRENCES: return "NUMOCCURRENCES";
            default: break;
        }
        
        return "UNKNOWN";
    }

    /** Returns the comumn number from the given column name*/
    static COLUMN name_to_column(const wxString& name)
    {
        if ("BDID" == name) return COL_BDID;
        else if ("ACCOUNTID" == name) return COL_ACCOUNTID;
        else if ("TOACCOUNTID" == name) return COL_TOACCOUNTID;
        else if ("PAYEEID" == name) return COL_PAYEEID;
        else if ("TRANSCODE" == name) return COL_TRANSCODE;
        else if ("TRANSAMOUNT" == name) return COL_TRANSAMOUNT;
        else if ("STATUS" == name) return COL_STATUS;
        else if ("TRANSACTIONNUMBER" == name) return COL_TRANSACTIONNUMBER;
        else if ("NOTES" == name) return COL_NOTES;
        else if ("CATEGID" == name) return COL_CATEGID;
        else if ("SUBCATEGID" == name) return COL_SUBCATEGID;
        else if ("TRANSDATE" == name) return COL_TRANSDATE;
        else if ("FOLLOWUPID" == name) return COL_FOLLOWUPID;
        else if ("TOTRANSAMOUNT" == name) return COL_TOTRANSAMOUNT;
        else if ("REPEATS" == name) return COL_REPEATS;
        else if ("NEXTOCCURRENCEDATE" == name) return COL_NEXTOCCURRENCEDATE;
        else if ("NUMOCCURRENCES" == name) return COL_NUMOCCURRENCES;

        return COLUMN(-1);
    }
    
    /** Data is a single record in the database table*/
    struct Data
    {
        friend struct DB_Table_BILLSDEPOSITS_V1;
        /** This is a instance pointer to itself in memory. */
        Self* view_;
    
        int BDID;//  primay key
        int ACCOUNTID;
        int TOACCOUNTID;
        int PAYEEID;
        wxString TRANSCODE;
        double TRANSAMOUNT;
        wxString STATUS;
        wxString TRANSACTIONNUMBER;
        wxString NOTES;
        int CATEGID;
        int SUBCATEGID;
        wxString TRANSDATE;
        int FOLLOWUPID;
        double TOTRANSAMOUNT;
        int REPEATS;
        wxString NEXTOCCURRENCEDATE;
        int NUMOCCURRENCES;
        int id() const { return BDID; }
        void id(int id) { BDID = id; }
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
        
            BDID = -1;
            ACCOUNTID = -1;
            TOACCOUNTID = -1;
            PAYEEID = -1;
            TRANSAMOUNT = 0.0;
            CATEGID = -1;
            SUBCATEGID = -1;
            FOLLOWUPID = -1;
            TOTRANSAMOUNT = 0.0;
            REPEATS = -1;
            NUMOCCURRENCES = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            BDID = q.GetInt(0); // BDID
            ACCOUNTID = q.GetInt(1); // ACCOUNTID
            TOACCOUNTID = q.GetInt(2); // TOACCOUNTID
            PAYEEID = q.GetInt(3); // PAYEEID
            TRANSCODE = q.GetString(4); // TRANSCODE
            TRANSAMOUNT = q.GetDouble(5); // TRANSAMOUNT
            STATUS = q.GetString(6); // STATUS
            TRANSACTIONNUMBER = q.GetString(7); // TRANSACTIONNUMBER
            NOTES = q.GetString(8); // NOTES
            CATEGID = q.GetInt(9); // CATEGID
            SUBCATEGID = q.GetInt(10); // SUBCATEGID
            TRANSDATE = q.GetString(11); // TRANSDATE
            FOLLOWUPID = q.GetInt(12); // FOLLOWUPID
            TOTRANSAMOUNT = q.GetDouble(13); // TOTRANSAMOUNT
            REPEATS = q.GetInt(14); // REPEATS
            NEXTOCCURRENCEDATE = q.GetString(15); // NEXTOCCURRENCEDATE
            NUMOCCURRENCES = q.GetInt(16); // NUMOCCURRENCES
        }

        template<typename C>
        bool match(const C &c) const
        {
            return false;
        }
        bool match(const Self::BDID &in) const
        {
            return this->BDID == in.v_;
        }
        bool match(const Self::ACCOUNTID &in) const
        {
            return this->ACCOUNTID == in.v_;
        }
        bool match(const Self::TOACCOUNTID &in) const
        {
            return this->TOACCOUNTID == in.v_;
        }
        bool match(const Self::PAYEEID &in) const
        {
            return this->PAYEEID == in.v_;
        }
        bool match(const Self::TRANSCODE &in) const
        {
            return this->TRANSCODE.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::TRANSAMOUNT &in) const
        {
            return this->TRANSAMOUNT == in.v_;
        }
        bool match(const Self::STATUS &in) const
        {
            return this->STATUS.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::TRANSACTIONNUMBER &in) const
        {
            return this->TRANSACTIONNUMBER.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::NOTES &in) const
        {
            return this->NOTES.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::CATEGID &in) const
        {
            return this->CATEGID == in.v_;
        }
        bool match(const Self::SUBCATEGID &in) const
        {
            return this->SUBCATEGID == in.v_;
        }
        bool match(const Self::TRANSDATE &in) const
        {
            return this->TRANSDATE.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::FOLLOWUPID &in) const
        {
            return this->FOLLOWUPID == in.v_;
        }
        bool match(const Self::TOTRANSAMOUNT &in) const
        {
            return this->TOTRANSAMOUNT == in.v_;
        }
        bool match(const Self::REPEATS &in) const
        {
            return this->REPEATS == in.v_;
        }
        bool match(const Self::NEXTOCCURRENCEDATE &in) const
        {
            return this->NEXTOCCURRENCEDATE.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::NUMOCCURRENCES &in) const
        {
            return this->NUMOCCURRENCES == in.v_;
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
            o["BDID"] = json::Number(this->BDID);
            o["ACCOUNTID"] = json::Number(this->ACCOUNTID);
            o["TOACCOUNTID"] = json::Number(this->TOACCOUNTID);
            o["PAYEEID"] = json::Number(this->PAYEEID);
            o["TRANSCODE"] = json::String(this->TRANSCODE.ToStdString());
            o["TRANSAMOUNT"] = json::Number(this->TRANSAMOUNT);
            o["STATUS"] = json::String(this->STATUS.ToStdString());
            o["TRANSACTIONNUMBER"] = json::String(this->TRANSACTIONNUMBER.ToStdString());
            o["NOTES"] = json::String(this->NOTES.ToStdString());
            o["CATEGID"] = json::Number(this->CATEGID);
            o["SUBCATEGID"] = json::Number(this->SUBCATEGID);
            o["TRANSDATE"] = json::String(this->TRANSDATE.ToStdString());
            o["FOLLOWUPID"] = json::Number(this->FOLLOWUPID);
            o["TOTRANSAMOUNT"] = json::Number(this->TOTRANSAMOUNT);
            o["REPEATS"] = json::Number(this->REPEATS);
            o["NEXTOCCURRENCEDATE"] = json::String(this->NEXTOCCURRENCEDATE.ToStdString());
            o["NUMOCCURRENCES"] = json::Number(this->NUMOCCURRENCES);
            return 0;
        }
        row_t to_row_t() const
        {
            row_t row;
            row(L"BDID") = BDID;
            row(L"ACCOUNTID") = ACCOUNTID;
            row(L"TOACCOUNTID") = TOACCOUNTID;
            row(L"PAYEEID") = PAYEEID;
            row(L"TRANSCODE") = TRANSCODE;
            row(L"TRANSAMOUNT") = TRANSAMOUNT;
            row(L"STATUS") = STATUS;
            row(L"TRANSACTIONNUMBER") = TRANSACTIONNUMBER;
            row(L"NOTES") = NOTES;
            row(L"CATEGID") = CATEGID;
            row(L"SUBCATEGID") = SUBCATEGID;
            row(L"TRANSDATE") = TRANSDATE;
            row(L"FOLLOWUPID") = FOLLOWUPID;
            row(L"TOTRANSAMOUNT") = TOTRANSAMOUNT;
            row(L"REPEATS") = REPEATS;
            row(L"NEXTOCCURRENCEDATE") = NEXTOCCURRENCEDATE;
            row(L"NUMOCCURRENCES") = NUMOCCURRENCES;
            return row;
        }
        void to_template(html_template& t) const
        {
            t(L"BDID") = BDID;
            t(L"ACCOUNTID") = ACCOUNTID;
            t(L"TOACCOUNTID") = TOACCOUNTID;
            t(L"PAYEEID") = PAYEEID;
            t(L"TRANSCODE") = TRANSCODE;
            t(L"TRANSAMOUNT") = TRANSAMOUNT;
            t(L"STATUS") = STATUS;
            t(L"TRANSACTIONNUMBER") = TRANSACTIONNUMBER;
            t(L"NOTES") = NOTES;
            t(L"CATEGID") = CATEGID;
            t(L"SUBCATEGID") = SUBCATEGID;
            t(L"TRANSDATE") = TRANSDATE;
            t(L"FOLLOWUPID") = FOLLOWUPID;
            t(L"TOTRANSAMOUNT") = TOTRANSAMOUNT;
            t(L"REPEATS") = REPEATS;
            t(L"NEXTOCCURRENCEDATE") = NEXTOCCURRENCEDATE;
            t(L"NUMOCCURRENCES") = NUMOCCURRENCES;
        }

        /** Save the record instance in memory to the database. */
        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not save BILLSDEPOSITS_V1");
                return false;
            }

            return view_->save(this, db);
        }

        /** Remove the record instance from memory and the database. */
        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not remove BILLSDEPOSITS_V1");
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 17
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    /** Name of the table*/    
    wxString name() const { return "BILLSDEPOSITS_V1"; }

    DB_Table_BILLSDEPOSITS_V1() 
    {
        query_ = "SELECT * FROM BILLSDEPOSITS_V1 ";
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
            sql = "INSERT INTO BILLSDEPOSITS_V1(ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT, REPEATS, NEXTOCCURRENCEDATE, NUMOCCURRENCES) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        }
        else
        {
            sql = "UPDATE BILLSDEPOSITS_V1 SET ACCOUNTID = ?, TOACCOUNTID = ?, PAYEEID = ?, TRANSCODE = ?, TRANSAMOUNT = ?, STATUS = ?, TRANSACTIONNUMBER = ?, NOTES = ?, CATEGID = ?, SUBCATEGID = ?, TRANSDATE = ?, FOLLOWUPID = ?, TOTRANSAMOUNT = ?, REPEATS = ?, NEXTOCCURRENCEDATE = ?, NUMOCCURRENCES = ? WHERE BDID = ?";
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->ACCOUNTID);
            stmt.Bind(2, entity->TOACCOUNTID);
            stmt.Bind(3, entity->PAYEEID);
            stmt.Bind(4, entity->TRANSCODE);
            stmt.Bind(5, entity->TRANSAMOUNT);
            stmt.Bind(6, entity->STATUS);
            stmt.Bind(7, entity->TRANSACTIONNUMBER);
            stmt.Bind(8, entity->NOTES);
            stmt.Bind(9, entity->CATEGID);
            stmt.Bind(10, entity->SUBCATEGID);
            stmt.Bind(11, entity->TRANSDATE);
            stmt.Bind(12, entity->FOLLOWUPID);
            stmt.Bind(13, entity->TOTRANSAMOUNT);
            stmt.Bind(14, entity->REPEATS);
            stmt.Bind(15, entity->NEXTOCCURRENCEDATE);
            stmt.Bind(16, entity->NUMOCCURRENCES);
            if (entity->id() > 0)
                stmt.Bind(17, entity->BDID);

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
            wxLogError("BILLSDEPOSITS_V1: Exception %s, %s", e.GetMessage().c_str(), entity->to_json());
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
            wxString sql = "DELETE FROM BILLSDEPOSITS_V1 WHERE BDID = ?";
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
            wxLogError("BILLSDEPOSITS_V1: Exception %s", e.GetMessage().c_str());
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

    template<typename... Args>
    Self::Data* get(const Args& ... args)
    {
        for (auto & item : this->cache_)
            if (item->id() > 0 && match(item, args...)) return item;

        return 0;
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

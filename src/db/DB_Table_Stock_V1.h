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
 *          AUTO GENERATED at 2014-05-09 13:48:54.495677.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef DB_TABLE_STOCK_V1_H
#define DB_TABLE_STOCK_V1_H

#include "DB_Table.h"

struct DB_Table_STOCK_V1 : public DB_Table
{
    struct Data;
    typedef DB_Table_STOCK_V1 Self;
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
    typedef std::map<int, Self::Data*> Index_By_Id;
    Cache cache_;
    Index_By_Id index_by_id_;

    /** Destructor: clears any data records stored in memory */
    ~DB_Table_STOCK_V1() 
    {
        destroy_cache();
    }
	 
    /** Removes all records stored in memory (cache) for the table*/ 
    void destroy_cache()
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
        cache_.clear();
        index_by_id_.clear(); // no memory release since it just stores pointer and the according objects are in cache
    }

    /** Creates the database table if the table does not exist*/
    bool ensure(wxSQLite3Database* db)
    {
        if (!exists(db))
		{
			try
			{
				db->ExecuteUpdate("CREATE TABLE STOCK_V1(STOCKID integer primary key, HELDAT integer , PURCHASEDATE TEXT NOT NULL, STOCKNAME TEXT COLLATE NOCASE NOT NULL, SYMBOL TEXT, NUMSHARES numeric, PURCHASEPRICE numeric NOT NULL, NOTES TEXT, CURRENTPRICE numeric NOT NULL, VALUE numeric, COMMISSION numeric)");
			}
			catch(const wxSQLite3Exception &e) 
			{ 
				wxLogError("STOCK_V1: Exception %s", e.GetMessage().c_str());
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
            db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS IDX_STOCK_HELDAT ON STOCK_V1(HELDAT)");
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("STOCK_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct STOCKID : public DB_Column<int>
    { 
        static wxString name() { return "STOCKID"; } 
        explicit STOCKID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct HELDAT : public DB_Column<int>
    { 
        static wxString name() { return "HELDAT"; } 
        explicit HELDAT(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct PURCHASEDATE : public DB_Column<wxString>
    { 
        static wxString name() { return "PURCHASEDATE"; } 
        explicit PURCHASEDATE(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct STOCKNAME : public DB_Column<wxString>
    { 
        static wxString name() { return "STOCKNAME"; } 
        explicit STOCKNAME(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct SYMBOL : public DB_Column<wxString>
    { 
        static wxString name() { return "SYMBOL"; } 
        explicit SYMBOL(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct NUMSHARES : public DB_Column<double>
    { 
        static wxString name() { return "NUMSHARES"; } 
        explicit NUMSHARES(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    struct PURCHASEPRICE : public DB_Column<double>
    { 
        static wxString name() { return "PURCHASEPRICE"; } 
        explicit PURCHASEPRICE(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    struct NOTES : public DB_Column<wxString>
    { 
        static wxString name() { return "NOTES"; } 
        explicit NOTES(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    struct CURRENTPRICE : public DB_Column<double>
    { 
        static wxString name() { return "CURRENTPRICE"; } 
        explicit CURRENTPRICE(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    struct VALUE : public DB_Column<double>
    { 
        static wxString name() { return "VALUE"; } 
        explicit VALUE(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    struct COMMISSION : public DB_Column<double>
    { 
        static wxString name() { return "COMMISSION"; } 
        explicit COMMISSION(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    typedef STOCKID PRIMARY;
    enum COLUMN
    {
        COL_STOCKID = 0
        , COL_HELDAT = 1
        , COL_PURCHASEDATE = 2
        , COL_STOCKNAME = 3
        , COL_SYMBOL = 4
        , COL_NUMSHARES = 5
        , COL_PURCHASEPRICE = 6
        , COL_NOTES = 7
        , COL_CURRENTPRICE = 8
        , COL_VALUE = 9
        , COL_COMMISSION = 10
    };

    /** Returns the column name as a string*/
    static wxString column_to_name(COLUMN col)
    {
        switch(col)
        {
            case COL_STOCKID: return "STOCKID";
            case COL_HELDAT: return "HELDAT";
            case COL_PURCHASEDATE: return "PURCHASEDATE";
            case COL_STOCKNAME: return "STOCKNAME";
            case COL_SYMBOL: return "SYMBOL";
            case COL_NUMSHARES: return "NUMSHARES";
            case COL_PURCHASEPRICE: return "PURCHASEPRICE";
            case COL_NOTES: return "NOTES";
            case COL_CURRENTPRICE: return "CURRENTPRICE";
            case COL_VALUE: return "VALUE";
            case COL_COMMISSION: return "COMMISSION";
            default: break;
        }
        
        return "UNKNOWN";
    }

    /** Returns the comumn number from the given column name*/
    static COLUMN name_to_column(const wxString& name)
    {
        if ("STOCKID" == name) return COL_STOCKID;
        else if ("HELDAT" == name) return COL_HELDAT;
        else if ("PURCHASEDATE" == name) return COL_PURCHASEDATE;
        else if ("STOCKNAME" == name) return COL_STOCKNAME;
        else if ("SYMBOL" == name) return COL_SYMBOL;
        else if ("NUMSHARES" == name) return COL_NUMSHARES;
        else if ("PURCHASEPRICE" == name) return COL_PURCHASEPRICE;
        else if ("NOTES" == name) return COL_NOTES;
        else if ("CURRENTPRICE" == name) return COL_CURRENTPRICE;
        else if ("VALUE" == name) return COL_VALUE;
        else if ("COMMISSION" == name) return COL_COMMISSION;

        return COLUMN(-1);
    }
    
    /** Data is a single record in the database table*/
    struct Data
    {
        friend struct DB_Table_STOCK_V1;
        /** This is a instance pointer to itself in memory. */
        Self* view_;
    
        int STOCKID;//  primay key
        int HELDAT;
        wxString PURCHASEDATE;
        wxString STOCKNAME;
        wxString SYMBOL;
        double NUMSHARES;
        double PURCHASEPRICE;
        wxString NOTES;
        double CURRENTPRICE;
        double VALUE;
        double COMMISSION;
        int id() const { return STOCKID; }
        void id(int id) { STOCKID = id; }
        bool operator < (const Data& r) const
        {
            return this->id() < r.id();
        }
        bool operator < (const Data* r) const
        {
            return this->id() < r->id();
        }

        explicit Data(Self* view = 0) 
        {
            view_ = view;
        
            STOCKID = -1;
            HELDAT = -1;
            NUMSHARES = 0.0;
            PURCHASEPRICE = 0.0;
            CURRENTPRICE = 0.0;
            VALUE = 0.0;
            COMMISSION = 0.0;
        }

        explicit Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            STOCKID = q.GetInt(0); // STOCKID
            HELDAT = q.GetInt(1); // HELDAT
            PURCHASEDATE = q.GetString(2); // PURCHASEDATE
            STOCKNAME = q.GetString(3); // STOCKNAME
            SYMBOL = q.GetString(4); // SYMBOL
            NUMSHARES = q.GetDouble(5); // NUMSHARES
            PURCHASEPRICE = q.GetDouble(6); // PURCHASEPRICE
            NOTES = q.GetString(7); // NOTES
            CURRENTPRICE = q.GetDouble(8); // CURRENTPRICE
            VALUE = q.GetDouble(9); // VALUE
            COMMISSION = q.GetDouble(10); // COMMISSION
        }

        template<typename C>
        bool match(const C &c) const
        {
            return false;
        }
        bool match(const Self::STOCKID &in) const
        {
            return this->STOCKID == in.v_;
        }
        bool match(const Self::HELDAT &in) const
        {
            return this->HELDAT == in.v_;
        }
        bool match(const Self::PURCHASEDATE &in) const
        {
            return this->PURCHASEDATE.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::STOCKNAME &in) const
        {
            return this->STOCKNAME.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::SYMBOL &in) const
        {
            return this->SYMBOL.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::NUMSHARES &in) const
        {
            return this->NUMSHARES == in.v_;
        }
        bool match(const Self::PURCHASEPRICE &in) const
        {
            return this->PURCHASEPRICE == in.v_;
        }
        bool match(const Self::NOTES &in) const
        {
            return this->NOTES.CmpNoCase(in.v_) == 0;
        }
        bool match(const Self::CURRENTPRICE &in) const
        {
            return this->CURRENTPRICE == in.v_;
        }
        bool match(const Self::VALUE &in) const
        {
            return this->VALUE == in.v_;
        }
        bool match(const Self::COMMISSION &in) const
        {
            return this->COMMISSION == in.v_;
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
            o["STOCKID"] = json::Number(this->STOCKID);
            o["HELDAT"] = json::Number(this->HELDAT);
            o["PURCHASEDATE"] = json::String(this->PURCHASEDATE.ToStdString());
            o["STOCKNAME"] = json::String(this->STOCKNAME.ToStdString());
            o["SYMBOL"] = json::String(this->SYMBOL.ToStdString());
            o["NUMSHARES"] = json::Number(this->NUMSHARES);
            o["PURCHASEPRICE"] = json::Number(this->PURCHASEPRICE);
            o["NOTES"] = json::String(this->NOTES.ToStdString());
            o["CURRENTPRICE"] = json::Number(this->CURRENTPRICE);
            o["VALUE"] = json::Number(this->VALUE);
            o["COMMISSION"] = json::Number(this->COMMISSION);
            return 0;
        }
        row_t to_row_t() const
        {
            row_t row;
            row(L"STOCKID") = STOCKID;
            row(L"HELDAT") = HELDAT;
            row(L"PURCHASEDATE") = PURCHASEDATE;
            row(L"STOCKNAME") = STOCKNAME;
            row(L"SYMBOL") = SYMBOL;
            row(L"NUMSHARES") = NUMSHARES;
            row(L"PURCHASEPRICE") = PURCHASEPRICE;
            row(L"NOTES") = NOTES;
            row(L"CURRENTPRICE") = CURRENTPRICE;
            row(L"VALUE") = VALUE;
            row(L"COMMISSION") = COMMISSION;
            return row;
        }
        void to_template(html_template& t) const
        {
            t(L"STOCKID") = STOCKID;
            t(L"HELDAT") = HELDAT;
            t(L"PURCHASEDATE") = PURCHASEDATE;
            t(L"STOCKNAME") = STOCKNAME;
            t(L"SYMBOL") = SYMBOL;
            t(L"NUMSHARES") = NUMSHARES;
            t(L"PURCHASEPRICE") = PURCHASEPRICE;
            t(L"NOTES") = NOTES;
            t(L"CURRENTPRICE") = CURRENTPRICE;
            t(L"VALUE") = VALUE;
            t(L"COMMISSION") = COMMISSION;
        }

        /** Save the record instance in memory to the database. */
        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not save STOCK_V1");
                return false;
            }

            return view_->save(this, db);
        }

        /** Remove the record instance from memory and the database. */
        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not remove STOCK_V1");
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 11
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    /** Name of the table*/    
    wxString name() const { return "STOCK_V1"; }

    DB_Table_STOCK_V1() 
    {
        query_ = "SELECT * FROM STOCK_V1 ";
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
            sql = "INSERT INTO STOCK_V1(HELDAT, PURCHASEDATE, STOCKNAME, SYMBOL, NUMSHARES, PURCHASEPRICE, NOTES, CURRENTPRICE, VALUE, COMMISSION) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        }
        else
        {
            sql = "UPDATE STOCK_V1 SET HELDAT = ?, PURCHASEDATE = ?, STOCKNAME = ?, SYMBOL = ?, NUMSHARES = ?, PURCHASEPRICE = ?, NOTES = ?, CURRENTPRICE = ?, VALUE = ?, COMMISSION = ? WHERE STOCKID = ?";
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->HELDAT);
            stmt.Bind(2, entity->PURCHASEDATE);
            stmt.Bind(3, entity->STOCKNAME);
            stmt.Bind(4, entity->SYMBOL);
            stmt.Bind(5, entity->NUMSHARES);
            stmt.Bind(6, entity->PURCHASEPRICE);
            stmt.Bind(7, entity->NOTES);
            stmt.Bind(8, entity->CURRENTPRICE);
            stmt.Bind(9, entity->VALUE);
            stmt.Bind(10, entity->COMMISSION);
            if (entity->id() > 0)
                stmt.Bind(11, entity->STOCKID);

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
                    {
                        index_by_id_.erase(entity->id());
                        delete e;
                    }
                    else 
                    {
                        c.push_back(e);
                    }
                }
                cache_.clear();
                cache_.swap(c);
            }
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("STOCK_V1: Exception %s, %s", e.GetMessage().c_str(), entity->to_json());
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
            wxString sql = "DELETE FROM STOCK_V1 WHERE STOCKID = ?";
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
                {
                    index_by_id_.erase(entity->id());
                    delete entity;
                }
                else 
                {
                    c.push_back(entity);
                }
            }
            cache_.clear();
            cache_.swap(c);
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("STOCK_V1: Exception %s", e.GetMessage().c_str());
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
    Self::Data* get_one(const Args& ... args)
    {
        for (Index_By_Id::iterator it = index_by_id_.begin(); it != index_by_id_.end(); ++ it)
        {
            Self::Data* item = it->second;
            if (item->id() > 0 && match(item, args...)) 
            {
                ++ hit_;
                return item;
            }
        }

        ++ miss_;

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

        Index_By_Id::iterator it = index_by_id_.find(id);
        if (it != index_by_id_.end())
        {
            ++ hit_;
            return it->second;
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
                index_by_id_.insert(std::make_pair(id, entity));
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

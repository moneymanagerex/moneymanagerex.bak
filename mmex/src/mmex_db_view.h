// -*- C++ -*-
//=============================================================================
/**
 *      Copyright (c) 2012 Guan Lisheng
 *
 *      @file
 *
 *      @author [utils/mmex.py]
 *
 *      @brief
 *
 *      Revision History:
 *          AUTO GENERATED at 2012-05-09 17:41:32.000733.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef _MM_EX_DB_VIEW_H_
#define _MM_EX_DB_VIEW_H_

#include <vector>
#include <map>
#include <algorithm>

class wxString;
class wxSQLite3Statement;
class wxSQLite3Database;
class wxSQLite3ResultSet;

struct DB_View
{
    DB_View() {};
    virtual ~DB_View() {};
    wxString query_;
    virtual wxString query() const { return this->query_; }
    virtual size_t num_columns() const = 0;
    virtual wxString name() const = 0;

    bool exists(wxSQLite3Database* db) const
    {
       return db->TableExists(this->name()); 
    }

    virtual void begin(wxSQLite3Database* db) const
    {
        db->Begin();
    }

    virtual void commit(wxSQLite3Database* db) const
    {
        db->Commit();
    }
};

struct DB_View_ACCOUNTLIST_V1 : public DB_View
{
    struct Data;
    typedef DB_View_ACCOUNTLIST_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_ACCOUNTLIST_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE ACCOUNTLIST_V1(ACCOUNTID integer primary key, ACCOUNTNAME TEXT NOT NULL , ACCOUNTTYPE TEXT NOT NULL , ACCOUNTNUM TEXT, STATUS TEXT NOT NULL, NOTES TEXT , HELDAT TEXT , WEBSITE TEXT , CONTACTINFO TEXT, ACCESSINFO TEXT , INITIALBAL numeric , FAVORITEACCT TEXT NOT NULL, CURRENCYID integer NOT NULL)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ACCOUNTLIST_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct ACCOUNTID { wxString name() const { return wxT("ACCOUNTID"); } };
    struct ACCOUNTNAME { wxString name() const { return wxT("ACCOUNTNAME"); } };
    struct ACCOUNTTYPE { wxString name() const { return wxT("ACCOUNTTYPE"); } };
    struct ACCOUNTNUM { wxString name() const { return wxT("ACCOUNTNUM"); } };
    struct STATUS { wxString name() const { return wxT("STATUS"); } };
    struct NOTES { wxString name() const { return wxT("NOTES"); } };
    struct HELDAT { wxString name() const { return wxT("HELDAT"); } };
    struct WEBSITE { wxString name() const { return wxT("WEBSITE"); } };
    struct CONTACTINFO { wxString name() const { return wxT("CONTACTINFO"); } };
    struct ACCESSINFO { wxString name() const { return wxT("ACCESSINFO"); } };
    struct INITIALBAL { wxString name() const { return wxT("INITIALBAL"); } };
    struct FAVORITEACCT { wxString name() const { return wxT("FAVORITEACCT"); } };
    struct CURRENCYID { wxString name() const { return wxT("CURRENCYID"); } };
    typedef ACCOUNTID PRIMARY;
    enum COLUMN
    {
        COL_ACCOUNTID = 0
        , COL_ACCOUNTNAME = 1
        , COL_ACCOUNTTYPE = 2
        , COL_ACCOUNTNUM = 3
        , COL_STATUS = 4
        , COL_NOTES = 5
        , COL_HELDAT = 6
        , COL_WEBSITE = 7
        , COL_CONTACTINFO = 8
        , COL_ACCESSINFO = 9
        , COL_INITIALBAL = 10
        , COL_FAVORITEACCT = 11
        , COL_CURRENCYID = 12
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_ACCOUNTID);
        result.push_back(COL_ACCOUNTNAME);
        result.push_back(COL_ACCOUNTTYPE);
        result.push_back(COL_ACCOUNTNUM);
        result.push_back(COL_STATUS);
        result.push_back(COL_NOTES);
        result.push_back(COL_HELDAT);
        result.push_back(COL_WEBSITE);
        result.push_back(COL_CONTACTINFO);
        result.push_back(COL_ACCESSINFO);
        result.push_back(COL_INITIALBAL);
        result.push_back(COL_FAVORITEACCT);
        result.push_back(COL_CURRENCYID);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_ACCOUNTID: return wxT("ACCOUNTID");
            case COL_ACCOUNTNAME: return wxT("ACCOUNTNAME");
            case COL_ACCOUNTTYPE: return wxT("ACCOUNTTYPE");
            case COL_ACCOUNTNUM: return wxT("ACCOUNTNUM");
            case COL_STATUS: return wxT("STATUS");
            case COL_NOTES: return wxT("NOTES");
            case COL_HELDAT: return wxT("HELDAT");
            case COL_WEBSITE: return wxT("WEBSITE");
            case COL_CONTACTINFO: return wxT("CONTACTINFO");
            case COL_ACCESSINFO: return wxT("ACCESSINFO");
            case COL_INITIALBAL: return wxT("INITIALBAL");
            case COL_FAVORITEACCT: return wxT("FAVORITEACCT");
            case COL_CURRENCYID: return wxT("CURRENCYID");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("ACCOUNTID") == name) return COL_ACCOUNTID;
        else if (wxT("ACCOUNTNAME") == name) return COL_ACCOUNTNAME;
        else if (wxT("ACCOUNTTYPE") == name) return COL_ACCOUNTTYPE;
        else if (wxT("ACCOUNTNUM") == name) return COL_ACCOUNTNUM;
        else if (wxT("STATUS") == name) return COL_STATUS;
        else if (wxT("NOTES") == name) return COL_NOTES;
        else if (wxT("HELDAT") == name) return COL_HELDAT;
        else if (wxT("WEBSITE") == name) return COL_WEBSITE;
        else if (wxT("CONTACTINFO") == name) return COL_CONTACTINFO;
        else if (wxT("ACCESSINFO") == name) return COL_ACCESSINFO;
        else if (wxT("INITIALBAL") == name) return COL_INITIALBAL;
        else if (wxT("FAVORITEACCT") == name) return COL_FAVORITEACCT;
        else if (wxT("CURRENCYID") == name) return COL_CURRENCYID;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int ACCOUNTID;//  primay key
        wxString ACCOUNTNAME;
        wxString ACCOUNTTYPE;
        wxString ACCOUNTNUM;
        wxString STATUS;
        wxString NOTES;
        wxString HELDAT;
        wxString WEBSITE;
        wxString CONTACTINFO;
        wxString ACCESSINFO;
        double INITIALBAL;
        wxString FAVORITEACCT;
        int CURRENCYID;
        int id() const { return ACCOUNTID; }
        void id(int id) { ACCOUNTID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            ACCOUNTID = -1;
            INITIALBAL = 0.0;
            CURRENCYID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            ACCOUNTID = q.GetInt(wxT("ACCOUNTID"));
            ACCOUNTNAME = q.GetString(wxT("ACCOUNTNAME"));
            ACCOUNTTYPE = q.GetString(wxT("ACCOUNTTYPE"));
            ACCOUNTNUM = q.GetString(wxT("ACCOUNTNUM"));
            STATUS = q.GetString(wxT("STATUS"));
            NOTES = q.GetString(wxT("NOTES"));
            HELDAT = q.GetString(wxT("HELDAT"));
            WEBSITE = q.GetString(wxT("WEBSITE"));
            CONTACTINFO = q.GetString(wxT("CONTACTINFO"));
            ACCESSINFO = q.GetString(wxT("ACCESSINFO"));
            INITIALBAL = q.GetDouble(wxT("INITIALBAL"));
            FAVORITEACCT = q.GetString(wxT("FAVORITEACCT"));
            CURRENCYID = q.GetInt(wxT("CURRENCYID"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_ACCOUNTID: ret << ACCOUNTID; break;
                case COL_ACCOUNTNAME: ret << ACCOUNTNAME; break;
                case COL_ACCOUNTTYPE: ret << ACCOUNTTYPE; break;
                case COL_ACCOUNTNUM: ret << ACCOUNTNUM; break;
                case COL_STATUS: ret << STATUS; break;
                case COL_NOTES: ret << NOTES; break;
                case COL_HELDAT: ret << HELDAT; break;
                case COL_WEBSITE: ret << WEBSITE; break;
                case COL_CONTACTINFO: ret << CONTACTINFO; break;
                case COL_ACCESSINFO: ret << ACCESSINFO; break;
                case COL_INITIALBAL: ret << INITIALBAL; break;
                case COL_FAVORITEACCT: ret << FAVORITEACCT; break;
                case COL_CURRENCYID: ret << CURRENCYID; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << ACCOUNTID;
            ret << delimiter << ACCOUNTNAME;
            ret << delimiter << ACCOUNTTYPE;
            ret << delimiter << ACCOUNTNUM;
            ret << delimiter << STATUS;
            ret << delimiter << NOTES;
            ret << delimiter << HELDAT;
            ret << delimiter << WEBSITE;
            ret << delimiter << CONTACTINFO;
            ret << delimiter << ACCESSINFO;
            ret << delimiter << INITIALBAL;
            ret << delimiter << FAVORITEACCT;
            ret << delimiter << CURRENCYID;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 13
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("ACCOUNTLIST_V1"); }

    DB_View_ACCOUNTLIST_V1() 
    {
        query_ = wxT("SELECT ACCOUNTID, ACCOUNTNAME, ACCOUNTTYPE, ACCOUNTNUM, STATUS, NOTES, HELDAT, WEBSITE, CONTACTINFO, ACCESSINFO, INITIALBAL, FAVORITEACCT, CURRENCYID FROM ACCOUNTLIST_V1 ");
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
            sql = wxT("INSERT INTO ACCOUNTLIST_V1(ACCOUNTNAME, ACCOUNTTYPE, ACCOUNTNUM, STATUS, NOTES, HELDAT, WEBSITE, CONTACTINFO, ACCESSINFO, INITIALBAL, FAVORITEACCT, CURRENCYID) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE ACCOUNTLIST_V1 SET ACCOUNTNAME = ?, ACCOUNTTYPE = ?, ACCOUNTNUM = ?, STATUS = ?, NOTES = ?, HELDAT = ?, WEBSITE = ?, CONTACTINFO = ?, ACCESSINFO = ?, INITIALBAL = ?, FAVORITEACCT = ?, CURRENCYID = ? WHERE ACCOUNTID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->ACCOUNTNAME);
            stmt.Bind(2, entity->ACCOUNTTYPE);
            stmt.Bind(3, entity->ACCOUNTNUM);
            stmt.Bind(4, entity->STATUS);
            stmt.Bind(5, entity->NOTES);
            stmt.Bind(6, entity->HELDAT);
            stmt.Bind(7, entity->WEBSITE);
            stmt.Bind(8, entity->CONTACTINFO);
            stmt.Bind(9, entity->ACCESSINFO);
            stmt.Bind(10, entity->INITIALBAL);
            stmt.Bind(11, entity->FAVORITEACCT);
            stmt.Bind(12, entity->CURRENCYID);
            if (entity->id() > 0)
                stmt.Bind(13, entity->ACCOUNTID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ACCOUNTLIST_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM ACCOUNTLIST_V1 WHERE ACCOUNTID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ACCOUNTLIST_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE ACCOUNTID = ?"));
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
            wxLogError(wxT("ACCOUNTLIST_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("ACCOUNTLIST_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ACCOUNTLIST_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_ACCOUNTLIST_V1 ACCOUNTLIST_V1;

struct DB_View_ASSETS_V1 : public DB_View
{
    struct Data;
    typedef DB_View_ASSETS_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_ASSETS_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE ASSETS_V1(ASSETID integer primary key, STARTDATE TEXT NOT NULL , ASSETNAME TEXT, VALUE numeric, VALUECHANGE TEXT, NOTES TEXT, VALUECHANGERATE numeric, ASSETTYPE TEXT)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ASSETS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct ASSETID { wxString name() const { return wxT("ASSETID"); } };
    struct STARTDATE { wxString name() const { return wxT("STARTDATE"); } };
    struct ASSETNAME { wxString name() const { return wxT("ASSETNAME"); } };
    struct VALUE { wxString name() const { return wxT("VALUE"); } };
    struct VALUECHANGE { wxString name() const { return wxT("VALUECHANGE"); } };
    struct NOTES { wxString name() const { return wxT("NOTES"); } };
    struct VALUECHANGERATE { wxString name() const { return wxT("VALUECHANGERATE"); } };
    struct ASSETTYPE { wxString name() const { return wxT("ASSETTYPE"); } };
    typedef ASSETID PRIMARY;
    enum COLUMN
    {
        COL_ASSETID = 0
        , COL_STARTDATE = 1
        , COL_ASSETNAME = 2
        , COL_VALUE = 3
        , COL_VALUECHANGE = 4
        , COL_NOTES = 5
        , COL_VALUECHANGERATE = 6
        , COL_ASSETTYPE = 7
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_ASSETID);
        result.push_back(COL_STARTDATE);
        result.push_back(COL_ASSETNAME);
        result.push_back(COL_VALUE);
        result.push_back(COL_VALUECHANGE);
        result.push_back(COL_NOTES);
        result.push_back(COL_VALUECHANGERATE);
        result.push_back(COL_ASSETTYPE);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_ASSETID: return wxT("ASSETID");
            case COL_STARTDATE: return wxT("STARTDATE");
            case COL_ASSETNAME: return wxT("ASSETNAME");
            case COL_VALUE: return wxT("VALUE");
            case COL_VALUECHANGE: return wxT("VALUECHANGE");
            case COL_NOTES: return wxT("NOTES");
            case COL_VALUECHANGERATE: return wxT("VALUECHANGERATE");
            case COL_ASSETTYPE: return wxT("ASSETTYPE");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("ASSETID") == name) return COL_ASSETID;
        else if (wxT("STARTDATE") == name) return COL_STARTDATE;
        else if (wxT("ASSETNAME") == name) return COL_ASSETNAME;
        else if (wxT("VALUE") == name) return COL_VALUE;
        else if (wxT("VALUECHANGE") == name) return COL_VALUECHANGE;
        else if (wxT("NOTES") == name) return COL_NOTES;
        else if (wxT("VALUECHANGERATE") == name) return COL_VALUECHANGERATE;
        else if (wxT("ASSETTYPE") == name) return COL_ASSETTYPE;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int ASSETID;//  primay key
        wxString STARTDATE;
        wxString ASSETNAME;
        double VALUE;
        wxString VALUECHANGE;
        wxString NOTES;
        double VALUECHANGERATE;
        wxString ASSETTYPE;
        int id() const { return ASSETID; }
        void id(int id) { ASSETID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            ASSETID = -1;
            VALUE = 0.0;
            VALUECHANGERATE = 0.0;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            ASSETID = q.GetInt(wxT("ASSETID"));
            STARTDATE = q.GetString(wxT("STARTDATE"));
            ASSETNAME = q.GetString(wxT("ASSETNAME"));
            VALUE = q.GetDouble(wxT("VALUE"));
            VALUECHANGE = q.GetString(wxT("VALUECHANGE"));
            NOTES = q.GetString(wxT("NOTES"));
            VALUECHANGERATE = q.GetDouble(wxT("VALUECHANGERATE"));
            ASSETTYPE = q.GetString(wxT("ASSETTYPE"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_ASSETID: ret << ASSETID; break;
                case COL_STARTDATE: ret << STARTDATE; break;
                case COL_ASSETNAME: ret << ASSETNAME; break;
                case COL_VALUE: ret << VALUE; break;
                case COL_VALUECHANGE: ret << VALUECHANGE; break;
                case COL_NOTES: ret << NOTES; break;
                case COL_VALUECHANGERATE: ret << VALUECHANGERATE; break;
                case COL_ASSETTYPE: ret << ASSETTYPE; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << ASSETID;
            ret << delimiter << STARTDATE;
            ret << delimiter << ASSETNAME;
            ret << delimiter << VALUE;
            ret << delimiter << VALUECHANGE;
            ret << delimiter << NOTES;
            ret << delimiter << VALUECHANGERATE;
            ret << delimiter << ASSETTYPE;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 8
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("ASSETS_V1"); }

    DB_View_ASSETS_V1() 
    {
        query_ = wxT("SELECT ASSETID, STARTDATE, ASSETNAME, VALUE, VALUECHANGE, NOTES, VALUECHANGERATE, ASSETTYPE FROM ASSETS_V1 ");
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
            sql = wxT("INSERT INTO ASSETS_V1(STARTDATE, ASSETNAME, VALUE, VALUECHANGE, NOTES, VALUECHANGERATE, ASSETTYPE) VALUES(?, ?, ?, ?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE ASSETS_V1 SET STARTDATE = ?, ASSETNAME = ?, VALUE = ?, VALUECHANGE = ?, NOTES = ?, VALUECHANGERATE = ?, ASSETTYPE = ? WHERE ASSETID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->STARTDATE);
            stmt.Bind(2, entity->ASSETNAME);
            stmt.Bind(3, entity->VALUE);
            stmt.Bind(4, entity->VALUECHANGE);
            stmt.Bind(5, entity->NOTES);
            stmt.Bind(6, entity->VALUECHANGERATE);
            stmt.Bind(7, entity->ASSETTYPE);
            if (entity->id() > 0)
                stmt.Bind(8, entity->ASSETID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ASSETS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM ASSETS_V1 WHERE ASSETID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ASSETS_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE ASSETID = ?"));
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
            wxLogError(wxT("ASSETS_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("ASSETS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("ASSETS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_ASSETS_V1 ASSETS_V1;

struct DB_View_BILLSDEPOSITS_V1 : public DB_View
{
    struct Data;
    typedef DB_View_BILLSDEPOSITS_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_BILLSDEPOSITS_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE BILLSDEPOSITS_V1(BDID integer primary key, ACCOUNTID integer NOT NULL, TOACCOUNTID integer, PAYEEID integer NOT NULL, TRANSCODE TEXT NOT NULL, TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT, CATEGID integer, SUBCATEGID integer, TRANSDATE TEXT, FOLLOWUPID integer, TOTRANSAMOUNT numeric, REPEATS numeric, NEXTOCCURRENCEDATE TEXT, NUMOCCURRENCES numeric)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BILLSDEPOSITS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct BDID { wxString name() const { return wxT("BDID"); } };
    struct ACCOUNTID { wxString name() const { return wxT("ACCOUNTID"); } };
    struct TOACCOUNTID { wxString name() const { return wxT("TOACCOUNTID"); } };
    struct PAYEEID { wxString name() const { return wxT("PAYEEID"); } };
    struct TRANSCODE { wxString name() const { return wxT("TRANSCODE"); } };
    struct TRANSAMOUNT { wxString name() const { return wxT("TRANSAMOUNT"); } };
    struct STATUS { wxString name() const { return wxT("STATUS"); } };
    struct TRANSACTIONNUMBER { wxString name() const { return wxT("TRANSACTIONNUMBER"); } };
    struct NOTES { wxString name() const { return wxT("NOTES"); } };
    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    struct SUBCATEGID { wxString name() const { return wxT("SUBCATEGID"); } };
    struct TRANSDATE { wxString name() const { return wxT("TRANSDATE"); } };
    struct FOLLOWUPID { wxString name() const { return wxT("FOLLOWUPID"); } };
    struct TOTRANSAMOUNT { wxString name() const { return wxT("TOTRANSAMOUNT"); } };
    struct REPEATS { wxString name() const { return wxT("REPEATS"); } };
    struct NEXTOCCURRENCEDATE { wxString name() const { return wxT("NEXTOCCURRENCEDATE"); } };
    struct NUMOCCURRENCES { wxString name() const { return wxT("NUMOCCURRENCES"); } };
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

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_BDID);
        result.push_back(COL_ACCOUNTID);
        result.push_back(COL_TOACCOUNTID);
        result.push_back(COL_PAYEEID);
        result.push_back(COL_TRANSCODE);
        result.push_back(COL_TRANSAMOUNT);
        result.push_back(COL_STATUS);
        result.push_back(COL_TRANSACTIONNUMBER);
        result.push_back(COL_NOTES);
        result.push_back(COL_CATEGID);
        result.push_back(COL_SUBCATEGID);
        result.push_back(COL_TRANSDATE);
        result.push_back(COL_FOLLOWUPID);
        result.push_back(COL_TOTRANSAMOUNT);
        result.push_back(COL_REPEATS);
        result.push_back(COL_NEXTOCCURRENCEDATE);
        result.push_back(COL_NUMOCCURRENCES);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_BDID: return wxT("BDID");
            case COL_ACCOUNTID: return wxT("ACCOUNTID");
            case COL_TOACCOUNTID: return wxT("TOACCOUNTID");
            case COL_PAYEEID: return wxT("PAYEEID");
            case COL_TRANSCODE: return wxT("TRANSCODE");
            case COL_TRANSAMOUNT: return wxT("TRANSAMOUNT");
            case COL_STATUS: return wxT("STATUS");
            case COL_TRANSACTIONNUMBER: return wxT("TRANSACTIONNUMBER");
            case COL_NOTES: return wxT("NOTES");
            case COL_CATEGID: return wxT("CATEGID");
            case COL_SUBCATEGID: return wxT("SUBCATEGID");
            case COL_TRANSDATE: return wxT("TRANSDATE");
            case COL_FOLLOWUPID: return wxT("FOLLOWUPID");
            case COL_TOTRANSAMOUNT: return wxT("TOTRANSAMOUNT");
            case COL_REPEATS: return wxT("REPEATS");
            case COL_NEXTOCCURRENCEDATE: return wxT("NEXTOCCURRENCEDATE");
            case COL_NUMOCCURRENCES: return wxT("NUMOCCURRENCES");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("BDID") == name) return COL_BDID;
        else if (wxT("ACCOUNTID") == name) return COL_ACCOUNTID;
        else if (wxT("TOACCOUNTID") == name) return COL_TOACCOUNTID;
        else if (wxT("PAYEEID") == name) return COL_PAYEEID;
        else if (wxT("TRANSCODE") == name) return COL_TRANSCODE;
        else if (wxT("TRANSAMOUNT") == name) return COL_TRANSAMOUNT;
        else if (wxT("STATUS") == name) return COL_STATUS;
        else if (wxT("TRANSACTIONNUMBER") == name) return COL_TRANSACTIONNUMBER;
        else if (wxT("NOTES") == name) return COL_NOTES;
        else if (wxT("CATEGID") == name) return COL_CATEGID;
        else if (wxT("SUBCATEGID") == name) return COL_SUBCATEGID;
        else if (wxT("TRANSDATE") == name) return COL_TRANSDATE;
        else if (wxT("FOLLOWUPID") == name) return COL_FOLLOWUPID;
        else if (wxT("TOTRANSAMOUNT") == name) return COL_TOTRANSAMOUNT;
        else if (wxT("REPEATS") == name) return COL_REPEATS;
        else if (wxT("NEXTOCCURRENCEDATE") == name) return COL_NEXTOCCURRENCEDATE;
        else if (wxT("NUMOCCURRENCES") == name) return COL_NUMOCCURRENCES;

        return COLUMN(-1);
    }
    
    struct Data
    {
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
        double REPEATS;
        wxString NEXTOCCURRENCEDATE;
        double NUMOCCURRENCES;
        int id() const { return BDID; }
        void id(int id) { BDID = id; }

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
            REPEATS = 0.0;
            NUMOCCURRENCES = 0.0;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            BDID = q.GetInt(wxT("BDID"));
            ACCOUNTID = q.GetInt(wxT("ACCOUNTID"));
            TOACCOUNTID = q.GetInt(wxT("TOACCOUNTID"));
            PAYEEID = q.GetInt(wxT("PAYEEID"));
            TRANSCODE = q.GetString(wxT("TRANSCODE"));
            TRANSAMOUNT = q.GetDouble(wxT("TRANSAMOUNT"));
            STATUS = q.GetString(wxT("STATUS"));
            TRANSACTIONNUMBER = q.GetString(wxT("TRANSACTIONNUMBER"));
            NOTES = q.GetString(wxT("NOTES"));
            CATEGID = q.GetInt(wxT("CATEGID"));
            SUBCATEGID = q.GetInt(wxT("SUBCATEGID"));
            TRANSDATE = q.GetString(wxT("TRANSDATE"));
            FOLLOWUPID = q.GetInt(wxT("FOLLOWUPID"));
            TOTRANSAMOUNT = q.GetDouble(wxT("TOTRANSAMOUNT"));
            REPEATS = q.GetDouble(wxT("REPEATS"));
            NEXTOCCURRENCEDATE = q.GetString(wxT("NEXTOCCURRENCEDATE"));
            NUMOCCURRENCES = q.GetDouble(wxT("NUMOCCURRENCES"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_BDID: ret << BDID; break;
                case COL_ACCOUNTID: ret << ACCOUNTID; break;
                case COL_TOACCOUNTID: ret << TOACCOUNTID; break;
                case COL_PAYEEID: ret << PAYEEID; break;
                case COL_TRANSCODE: ret << TRANSCODE; break;
                case COL_TRANSAMOUNT: ret << TRANSAMOUNT; break;
                case COL_STATUS: ret << STATUS; break;
                case COL_TRANSACTIONNUMBER: ret << TRANSACTIONNUMBER; break;
                case COL_NOTES: ret << NOTES; break;
                case COL_CATEGID: ret << CATEGID; break;
                case COL_SUBCATEGID: ret << SUBCATEGID; break;
                case COL_TRANSDATE: ret << TRANSDATE; break;
                case COL_FOLLOWUPID: ret << FOLLOWUPID; break;
                case COL_TOTRANSAMOUNT: ret << TOTRANSAMOUNT; break;
                case COL_REPEATS: ret << REPEATS; break;
                case COL_NEXTOCCURRENCEDATE: ret << NEXTOCCURRENCEDATE; break;
                case COL_NUMOCCURRENCES: ret << NUMOCCURRENCES; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << BDID;
            ret << delimiter << ACCOUNTID;
            ret << delimiter << TOACCOUNTID;
            ret << delimiter << PAYEEID;
            ret << delimiter << TRANSCODE;
            ret << delimiter << TRANSAMOUNT;
            ret << delimiter << STATUS;
            ret << delimiter << TRANSACTIONNUMBER;
            ret << delimiter << NOTES;
            ret << delimiter << CATEGID;
            ret << delimiter << SUBCATEGID;
            ret << delimiter << TRANSDATE;
            ret << delimiter << FOLLOWUPID;
            ret << delimiter << TOTRANSAMOUNT;
            ret << delimiter << REPEATS;
            ret << delimiter << NEXTOCCURRENCEDATE;
            ret << delimiter << NUMOCCURRENCES;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
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

    wxString name() const { return wxT("BILLSDEPOSITS_V1"); }

    DB_View_BILLSDEPOSITS_V1() 
    {
        query_ = wxT("SELECT BDID, ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT, REPEATS, NEXTOCCURRENCEDATE, NUMOCCURRENCES FROM BILLSDEPOSITS_V1 ");
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
            sql = wxT("INSERT INTO BILLSDEPOSITS_V1(ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT, REPEATS, NEXTOCCURRENCEDATE, NUMOCCURRENCES) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE BILLSDEPOSITS_V1 SET ACCOUNTID = ?, TOACCOUNTID = ?, PAYEEID = ?, TRANSCODE = ?, TRANSAMOUNT = ?, STATUS = ?, TRANSACTIONNUMBER = ?, NOTES = ?, CATEGID = ?, SUBCATEGID = ?, TRANSDATE = ?, FOLLOWUPID = ?, TOTRANSAMOUNT = ?, REPEATS = ?, NEXTOCCURRENCEDATE = ?, NUMOCCURRENCES = ? WHERE BDID = ?");
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

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BILLSDEPOSITS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM BILLSDEPOSITS_V1 WHERE BDID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BILLSDEPOSITS_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE BDID = ?"));
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
            wxLogError(wxT("BILLSDEPOSITS_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("BILLSDEPOSITS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BILLSDEPOSITS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_BILLSDEPOSITS_V1 BILLSDEPOSITS_V1;

struct DB_View_BUDGETSPLITTRANSACTIONS_V1 : public DB_View
{
    struct Data;
    typedef DB_View_BUDGETSPLITTRANSACTIONS_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_BUDGETSPLITTRANSACTIONS_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE BUDGETSPLITTRANSACTIONS_V1(SPLITTRANSID integer primary key, TRANSID integer NOT NULL, CATEGID integer, SUBCATEGID integer, SPLITTRANSAMOUNT numeric)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETSPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct SPLITTRANSID { wxString name() const { return wxT("SPLITTRANSID"); } };
    struct TRANSID { wxString name() const { return wxT("TRANSID"); } };
    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    struct SUBCATEGID { wxString name() const { return wxT("SUBCATEGID"); } };
    struct SPLITTRANSAMOUNT { wxString name() const { return wxT("SPLITTRANSAMOUNT"); } };
    typedef SPLITTRANSID PRIMARY;
    enum COLUMN
    {
        COL_SPLITTRANSID = 0
        , COL_TRANSID = 1
        , COL_CATEGID = 2
        , COL_SUBCATEGID = 3
        , COL_SPLITTRANSAMOUNT = 4
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_SPLITTRANSID);
        result.push_back(COL_TRANSID);
        result.push_back(COL_CATEGID);
        result.push_back(COL_SUBCATEGID);
        result.push_back(COL_SPLITTRANSAMOUNT);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_SPLITTRANSID: return wxT("SPLITTRANSID");
            case COL_TRANSID: return wxT("TRANSID");
            case COL_CATEGID: return wxT("CATEGID");
            case COL_SUBCATEGID: return wxT("SUBCATEGID");
            case COL_SPLITTRANSAMOUNT: return wxT("SPLITTRANSAMOUNT");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("SPLITTRANSID") == name) return COL_SPLITTRANSID;
        else if (wxT("TRANSID") == name) return COL_TRANSID;
        else if (wxT("CATEGID") == name) return COL_CATEGID;
        else if (wxT("SUBCATEGID") == name) return COL_SUBCATEGID;
        else if (wxT("SPLITTRANSAMOUNT") == name) return COL_SPLITTRANSAMOUNT;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int SPLITTRANSID;//  primay key
        int TRANSID;
        int CATEGID;
        int SUBCATEGID;
        double SPLITTRANSAMOUNT;
        int id() const { return SPLITTRANSID; }
        void id(int id) { SPLITTRANSID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            SPLITTRANSID = -1;
            TRANSID = -1;
            CATEGID = -1;
            SUBCATEGID = -1;
            SPLITTRANSAMOUNT = 0.0;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            SPLITTRANSID = q.GetInt(wxT("SPLITTRANSID"));
            TRANSID = q.GetInt(wxT("TRANSID"));
            CATEGID = q.GetInt(wxT("CATEGID"));
            SUBCATEGID = q.GetInt(wxT("SUBCATEGID"));
            SPLITTRANSAMOUNT = q.GetDouble(wxT("SPLITTRANSAMOUNT"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_SPLITTRANSID: ret << SPLITTRANSID; break;
                case COL_TRANSID: ret << TRANSID; break;
                case COL_CATEGID: ret << CATEGID; break;
                case COL_SUBCATEGID: ret << SUBCATEGID; break;
                case COL_SPLITTRANSAMOUNT: ret << SPLITTRANSAMOUNT; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << SPLITTRANSID;
            ret << delimiter << TRANSID;
            ret << delimiter << CATEGID;
            ret << delimiter << SUBCATEGID;
            ret << delimiter << SPLITTRANSAMOUNT;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 5
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("BUDGETSPLITTRANSACTIONS_V1"); }

    DB_View_BUDGETSPLITTRANSACTIONS_V1() 
    {
        query_ = wxT("SELECT SPLITTRANSID, TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT FROM BUDGETSPLITTRANSACTIONS_V1 ");
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
            sql = wxT("INSERT INTO BUDGETSPLITTRANSACTIONS_V1(TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT) VALUES(?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE BUDGETSPLITTRANSACTIONS_V1 SET TRANSID = ?, CATEGID = ?, SUBCATEGID = ?, SPLITTRANSAMOUNT = ? WHERE SPLITTRANSID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->TRANSID);
            stmt.Bind(2, entity->CATEGID);
            stmt.Bind(3, entity->SUBCATEGID);
            stmt.Bind(4, entity->SPLITTRANSAMOUNT);
            if (entity->id() > 0)
                stmt.Bind(5, entity->SPLITTRANSID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETSPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM BUDGETSPLITTRANSACTIONS_V1 WHERE SPLITTRANSID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETSPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE SPLITTRANSID = ?"));
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
            wxLogError(wxT("BUDGETSPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("BUDGETSPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETSPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_BUDGETSPLITTRANSACTIONS_V1 BUDGETSPLITTRANSACTIONS_V1;

struct DB_View_BUDGETTABLE_V1 : public DB_View
{
    struct Data;
    typedef DB_View_BUDGETTABLE_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_BUDGETTABLE_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE BUDGETTABLE_V1(BUDGETENTRYID integer primary key, BUDGETYEARID integer, CATEGID integer, SUBCATEGID integer, PERIOD TEXT NOT NULL, AMOUNT numeric NOT NULL)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETTABLE_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct BUDGETENTRYID { wxString name() const { return wxT("BUDGETENTRYID"); } };
    struct BUDGETYEARID { wxString name() const { return wxT("BUDGETYEARID"); } };
    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    struct SUBCATEGID { wxString name() const { return wxT("SUBCATEGID"); } };
    struct PERIOD { wxString name() const { return wxT("PERIOD"); } };
    struct AMOUNT { wxString name() const { return wxT("AMOUNT"); } };
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

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_BUDGETENTRYID);
        result.push_back(COL_BUDGETYEARID);
        result.push_back(COL_CATEGID);
        result.push_back(COL_SUBCATEGID);
        result.push_back(COL_PERIOD);
        result.push_back(COL_AMOUNT);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_BUDGETENTRYID: return wxT("BUDGETENTRYID");
            case COL_BUDGETYEARID: return wxT("BUDGETYEARID");
            case COL_CATEGID: return wxT("CATEGID");
            case COL_SUBCATEGID: return wxT("SUBCATEGID");
            case COL_PERIOD: return wxT("PERIOD");
            case COL_AMOUNT: return wxT("AMOUNT");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("BUDGETENTRYID") == name) return COL_BUDGETENTRYID;
        else if (wxT("BUDGETYEARID") == name) return COL_BUDGETYEARID;
        else if (wxT("CATEGID") == name) return COL_CATEGID;
        else if (wxT("SUBCATEGID") == name) return COL_SUBCATEGID;
        else if (wxT("PERIOD") == name) return COL_PERIOD;
        else if (wxT("AMOUNT") == name) return COL_AMOUNT;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int BUDGETENTRYID;//  primay key
        int BUDGETYEARID;
        int CATEGID;
        int SUBCATEGID;
        wxString PERIOD;
        double AMOUNT;
        int id() const { return BUDGETENTRYID; }
        void id(int id) { BUDGETENTRYID = id; }

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
        
            BUDGETENTRYID = q.GetInt(wxT("BUDGETENTRYID"));
            BUDGETYEARID = q.GetInt(wxT("BUDGETYEARID"));
            CATEGID = q.GetInt(wxT("CATEGID"));
            SUBCATEGID = q.GetInt(wxT("SUBCATEGID"));
            PERIOD = q.GetString(wxT("PERIOD"));
            AMOUNT = q.GetDouble(wxT("AMOUNT"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_BUDGETENTRYID: ret << BUDGETENTRYID; break;
                case COL_BUDGETYEARID: ret << BUDGETYEARID; break;
                case COL_CATEGID: ret << CATEGID; break;
                case COL_SUBCATEGID: ret << SUBCATEGID; break;
                case COL_PERIOD: ret << PERIOD; break;
                case COL_AMOUNT: ret << AMOUNT; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << BUDGETENTRYID;
            ret << delimiter << BUDGETYEARID;
            ret << delimiter << CATEGID;
            ret << delimiter << SUBCATEGID;
            ret << delimiter << PERIOD;
            ret << delimiter << AMOUNT;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
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

    wxString name() const { return wxT("BUDGETTABLE_V1"); }

    DB_View_BUDGETTABLE_V1() 
    {
        query_ = wxT("SELECT BUDGETENTRYID, BUDGETYEARID, CATEGID, SUBCATEGID, PERIOD, AMOUNT FROM BUDGETTABLE_V1 ");
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
            sql = wxT("INSERT INTO BUDGETTABLE_V1(BUDGETYEARID, CATEGID, SUBCATEGID, PERIOD, AMOUNT) VALUES(?, ?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE BUDGETTABLE_V1 SET BUDGETYEARID = ?, CATEGID = ?, SUBCATEGID = ?, PERIOD = ?, AMOUNT = ? WHERE BUDGETENTRYID = ?");
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

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETTABLE_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM BUDGETTABLE_V1 WHERE BUDGETENTRYID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETTABLE_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE BUDGETENTRYID = ?"));
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
            wxLogError(wxT("BUDGETTABLE_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("BUDGETTABLE_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETTABLE_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_BUDGETTABLE_V1 BUDGETTABLE_V1;

struct DB_View_BUDGETYEAR_V1 : public DB_View
{
    struct Data;
    typedef DB_View_BUDGETYEAR_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_BUDGETYEAR_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE BUDGETYEAR_V1(BUDGETYEARID integer primary key, BUDGETYEARNAME TEXT NOT NULL)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETYEAR_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct BUDGETYEARID { wxString name() const { return wxT("BUDGETYEARID"); } };
    struct BUDGETYEARNAME { wxString name() const { return wxT("BUDGETYEARNAME"); } };
    typedef BUDGETYEARID PRIMARY;
    enum COLUMN
    {
        COL_BUDGETYEARID = 0
        , COL_BUDGETYEARNAME = 1
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_BUDGETYEARID);
        result.push_back(COL_BUDGETYEARNAME);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_BUDGETYEARID: return wxT("BUDGETYEARID");
            case COL_BUDGETYEARNAME: return wxT("BUDGETYEARNAME");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("BUDGETYEARID") == name) return COL_BUDGETYEARID;
        else if (wxT("BUDGETYEARNAME") == name) return COL_BUDGETYEARNAME;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int BUDGETYEARID;//  primay key
        wxString BUDGETYEARNAME;
        int id() const { return BUDGETYEARID; }
        void id(int id) { BUDGETYEARID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            BUDGETYEARID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            BUDGETYEARID = q.GetInt(wxT("BUDGETYEARID"));
            BUDGETYEARNAME = q.GetString(wxT("BUDGETYEARNAME"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_BUDGETYEARID: ret << BUDGETYEARID; break;
                case COL_BUDGETYEARNAME: ret << BUDGETYEARNAME; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << BUDGETYEARID;
            ret << delimiter << BUDGETYEARNAME;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 2
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("BUDGETYEAR_V1"); }

    DB_View_BUDGETYEAR_V1() 
    {
        query_ = wxT("SELECT BUDGETYEARID, BUDGETYEARNAME FROM BUDGETYEAR_V1 ");
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
            sql = wxT("INSERT INTO BUDGETYEAR_V1(BUDGETYEARNAME) VALUES(?)");
        }
        else
        {
            sql = wxT("UPDATE BUDGETYEAR_V1 SET BUDGETYEARNAME = ? WHERE BUDGETYEARID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->BUDGETYEARNAME);
            if (entity->id() > 0)
                stmt.Bind(2, entity->BUDGETYEARID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETYEAR_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM BUDGETYEAR_V1 WHERE BUDGETYEARID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETYEAR_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE BUDGETYEARID = ?"));
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
            wxLogError(wxT("BUDGETYEAR_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("BUDGETYEAR_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("BUDGETYEAR_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_BUDGETYEAR_V1 BUDGETYEAR_V1;

struct DB_View_CATEGORY_V1 : public DB_View
{
    struct Data;
    typedef DB_View_CATEGORY_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_CATEGORY_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE CATEGORY_V1(CATEGID integer primary key, CATEGNAME TEXT NOT NULL)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CATEGORY_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    struct CATEGNAME { wxString name() const { return wxT("CATEGNAME"); } };
    typedef CATEGID PRIMARY;
    enum COLUMN
    {
        COL_CATEGID = 0
        , COL_CATEGNAME = 1
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_CATEGID);
        result.push_back(COL_CATEGNAME);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_CATEGID: return wxT("CATEGID");
            case COL_CATEGNAME: return wxT("CATEGNAME");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("CATEGID") == name) return COL_CATEGID;
        else if (wxT("CATEGNAME") == name) return COL_CATEGNAME;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int CATEGID;//  primay key
        wxString CATEGNAME;
        int id() const { return CATEGID; }
        void id(int id) { CATEGID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            CATEGID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            CATEGID = q.GetInt(wxT("CATEGID"));
            CATEGNAME = q.GetString(wxT("CATEGNAME"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_CATEGID: ret << CATEGID; break;
                case COL_CATEGNAME: ret << CATEGNAME; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << CATEGID;
            ret << delimiter << CATEGNAME;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 2
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("CATEGORY_V1"); }

    DB_View_CATEGORY_V1() 
    {
        query_ = wxT("SELECT CATEGID, CATEGNAME FROM CATEGORY_V1 ");
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
            sql = wxT("INSERT INTO CATEGORY_V1(CATEGNAME) VALUES(?)");
        }
        else
        {
            sql = wxT("UPDATE CATEGORY_V1 SET CATEGNAME = ? WHERE CATEGID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->CATEGNAME);
            if (entity->id() > 0)
                stmt.Bind(2, entity->CATEGID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CATEGORY_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM CATEGORY_V1 WHERE CATEGID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CATEGORY_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE CATEGID = ?"));
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
            wxLogError(wxT("CATEGORY_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("CATEGORY_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CATEGORY_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_CATEGORY_V1 CATEGORY_V1;

struct DB_View_CHECKINGACCOUNT_V1 : public DB_View
{
    struct Data;
    typedef DB_View_CHECKINGACCOUNT_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_CHECKINGACCOUNT_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE CHECKINGACCOUNT_V1(TRANSID integer primary key, ACCOUNTID integer NOT NULL, TOACCOUNTID integer, PAYEEID integer NOT NULL, TRANSCODE TEXT NOT NULL, TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT, CATEGID integer, SUBCATEGID integer, TRANSDATE TEXT, FOLLOWUPID integer, TOTRANSAMOUNT numeric)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CHECKINGACCOUNT_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct TRANSID { wxString name() const { return wxT("TRANSID"); } };
    struct ACCOUNTID { wxString name() const { return wxT("ACCOUNTID"); } };
    struct TOACCOUNTID { wxString name() const { return wxT("TOACCOUNTID"); } };
    struct PAYEEID { wxString name() const { return wxT("PAYEEID"); } };
    struct TRANSCODE { wxString name() const { return wxT("TRANSCODE"); } };
    struct TRANSAMOUNT { wxString name() const { return wxT("TRANSAMOUNT"); } };
    struct STATUS { wxString name() const { return wxT("STATUS"); } };
    struct TRANSACTIONNUMBER { wxString name() const { return wxT("TRANSACTIONNUMBER"); } };
    struct NOTES { wxString name() const { return wxT("NOTES"); } };
    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    struct SUBCATEGID { wxString name() const { return wxT("SUBCATEGID"); } };
    struct TRANSDATE { wxString name() const { return wxT("TRANSDATE"); } };
    struct FOLLOWUPID { wxString name() const { return wxT("FOLLOWUPID"); } };
    struct TOTRANSAMOUNT { wxString name() const { return wxT("TOTRANSAMOUNT"); } };
    typedef TRANSID PRIMARY;
    enum COLUMN
    {
        COL_TRANSID = 0
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
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_TRANSID);
        result.push_back(COL_ACCOUNTID);
        result.push_back(COL_TOACCOUNTID);
        result.push_back(COL_PAYEEID);
        result.push_back(COL_TRANSCODE);
        result.push_back(COL_TRANSAMOUNT);
        result.push_back(COL_STATUS);
        result.push_back(COL_TRANSACTIONNUMBER);
        result.push_back(COL_NOTES);
        result.push_back(COL_CATEGID);
        result.push_back(COL_SUBCATEGID);
        result.push_back(COL_TRANSDATE);
        result.push_back(COL_FOLLOWUPID);
        result.push_back(COL_TOTRANSAMOUNT);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_TRANSID: return wxT("TRANSID");
            case COL_ACCOUNTID: return wxT("ACCOUNTID");
            case COL_TOACCOUNTID: return wxT("TOACCOUNTID");
            case COL_PAYEEID: return wxT("PAYEEID");
            case COL_TRANSCODE: return wxT("TRANSCODE");
            case COL_TRANSAMOUNT: return wxT("TRANSAMOUNT");
            case COL_STATUS: return wxT("STATUS");
            case COL_TRANSACTIONNUMBER: return wxT("TRANSACTIONNUMBER");
            case COL_NOTES: return wxT("NOTES");
            case COL_CATEGID: return wxT("CATEGID");
            case COL_SUBCATEGID: return wxT("SUBCATEGID");
            case COL_TRANSDATE: return wxT("TRANSDATE");
            case COL_FOLLOWUPID: return wxT("FOLLOWUPID");
            case COL_TOTRANSAMOUNT: return wxT("TOTRANSAMOUNT");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("TRANSID") == name) return COL_TRANSID;
        else if (wxT("ACCOUNTID") == name) return COL_ACCOUNTID;
        else if (wxT("TOACCOUNTID") == name) return COL_TOACCOUNTID;
        else if (wxT("PAYEEID") == name) return COL_PAYEEID;
        else if (wxT("TRANSCODE") == name) return COL_TRANSCODE;
        else if (wxT("TRANSAMOUNT") == name) return COL_TRANSAMOUNT;
        else if (wxT("STATUS") == name) return COL_STATUS;
        else if (wxT("TRANSACTIONNUMBER") == name) return COL_TRANSACTIONNUMBER;
        else if (wxT("NOTES") == name) return COL_NOTES;
        else if (wxT("CATEGID") == name) return COL_CATEGID;
        else if (wxT("SUBCATEGID") == name) return COL_SUBCATEGID;
        else if (wxT("TRANSDATE") == name) return COL_TRANSDATE;
        else if (wxT("FOLLOWUPID") == name) return COL_FOLLOWUPID;
        else if (wxT("TOTRANSAMOUNT") == name) return COL_TOTRANSAMOUNT;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int TRANSID;//  primay key
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
        int id() const { return TRANSID; }
        void id(int id) { TRANSID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            TRANSID = -1;
            ACCOUNTID = -1;
            TOACCOUNTID = -1;
            PAYEEID = -1;
            TRANSAMOUNT = 0.0;
            CATEGID = -1;
            SUBCATEGID = -1;
            FOLLOWUPID = -1;
            TOTRANSAMOUNT = 0.0;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            TRANSID = q.GetInt(wxT("TRANSID"));
            ACCOUNTID = q.GetInt(wxT("ACCOUNTID"));
            TOACCOUNTID = q.GetInt(wxT("TOACCOUNTID"));
            PAYEEID = q.GetInt(wxT("PAYEEID"));
            TRANSCODE = q.GetString(wxT("TRANSCODE"));
            TRANSAMOUNT = q.GetDouble(wxT("TRANSAMOUNT"));
            STATUS = q.GetString(wxT("STATUS"));
            TRANSACTIONNUMBER = q.GetString(wxT("TRANSACTIONNUMBER"));
            NOTES = q.GetString(wxT("NOTES"));
            CATEGID = q.GetInt(wxT("CATEGID"));
            SUBCATEGID = q.GetInt(wxT("SUBCATEGID"));
            TRANSDATE = q.GetString(wxT("TRANSDATE"));
            FOLLOWUPID = q.GetInt(wxT("FOLLOWUPID"));
            TOTRANSAMOUNT = q.GetDouble(wxT("TOTRANSAMOUNT"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_TRANSID: ret << TRANSID; break;
                case COL_ACCOUNTID: ret << ACCOUNTID; break;
                case COL_TOACCOUNTID: ret << TOACCOUNTID; break;
                case COL_PAYEEID: ret << PAYEEID; break;
                case COL_TRANSCODE: ret << TRANSCODE; break;
                case COL_TRANSAMOUNT: ret << TRANSAMOUNT; break;
                case COL_STATUS: ret << STATUS; break;
                case COL_TRANSACTIONNUMBER: ret << TRANSACTIONNUMBER; break;
                case COL_NOTES: ret << NOTES; break;
                case COL_CATEGID: ret << CATEGID; break;
                case COL_SUBCATEGID: ret << SUBCATEGID; break;
                case COL_TRANSDATE: ret << TRANSDATE; break;
                case COL_FOLLOWUPID: ret << FOLLOWUPID; break;
                case COL_TOTRANSAMOUNT: ret << TOTRANSAMOUNT; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << TRANSID;
            ret << delimiter << ACCOUNTID;
            ret << delimiter << TOACCOUNTID;
            ret << delimiter << PAYEEID;
            ret << delimiter << TRANSCODE;
            ret << delimiter << TRANSAMOUNT;
            ret << delimiter << STATUS;
            ret << delimiter << TRANSACTIONNUMBER;
            ret << delimiter << NOTES;
            ret << delimiter << CATEGID;
            ret << delimiter << SUBCATEGID;
            ret << delimiter << TRANSDATE;
            ret << delimiter << FOLLOWUPID;
            ret << delimiter << TOTRANSAMOUNT;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 14
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("CHECKINGACCOUNT_V1"); }

    DB_View_CHECKINGACCOUNT_V1() 
    {
        query_ = wxT("SELECT TRANSID, ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT FROM CHECKINGACCOUNT_V1 ");
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
            sql = wxT("INSERT INTO CHECKINGACCOUNT_V1(ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE CHECKINGACCOUNT_V1 SET ACCOUNTID = ?, TOACCOUNTID = ?, PAYEEID = ?, TRANSCODE = ?, TRANSAMOUNT = ?, STATUS = ?, TRANSACTIONNUMBER = ?, NOTES = ?, CATEGID = ?, SUBCATEGID = ?, TRANSDATE = ?, FOLLOWUPID = ?, TOTRANSAMOUNT = ? WHERE TRANSID = ?");
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
            if (entity->id() > 0)
                stmt.Bind(14, entity->TRANSID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CHECKINGACCOUNT_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM CHECKINGACCOUNT_V1 WHERE TRANSID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CHECKINGACCOUNT_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE TRANSID = ?"));
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
            wxLogError(wxT("CHECKINGACCOUNT_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("CHECKINGACCOUNT_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CHECKINGACCOUNT_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_CHECKINGACCOUNT_V1 CHECKINGACCOUNT_V1;

struct DB_View_CURRENCYFORMATS_V1 : public DB_View
{
    struct Data;
    typedef DB_View_CURRENCYFORMATS_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_CURRENCYFORMATS_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE CURRENCYFORMATS_V1(CURRENCYID integer primary key, CURRENCYNAME TEXT NOT NULL, PFX_SYMBOL TEXT, SFX_SYMBOL TEXT, DECIMAL_POINT TEXT, GROUP_SEPARATOR TEXT, UNIT_NAME TEXT, CENT_NAME TEXT, SCALE numeric, BASECONVRATE numeric, CURRENCY_SYMBOL TEXT)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CURRENCYFORMATS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct CURRENCYID { wxString name() const { return wxT("CURRENCYID"); } };
    struct CURRENCYNAME { wxString name() const { return wxT("CURRENCYNAME"); } };
    struct PFX_SYMBOL { wxString name() const { return wxT("PFX_SYMBOL"); } };
    struct SFX_SYMBOL { wxString name() const { return wxT("SFX_SYMBOL"); } };
    struct DECIMAL_POINT { wxString name() const { return wxT("DECIMAL_POINT"); } };
    struct GROUP_SEPARATOR { wxString name() const { return wxT("GROUP_SEPARATOR"); } };
    struct UNIT_NAME { wxString name() const { return wxT("UNIT_NAME"); } };
    struct CENT_NAME { wxString name() const { return wxT("CENT_NAME"); } };
    struct SCALE { wxString name() const { return wxT("SCALE"); } };
    struct BASECONVRATE { wxString name() const { return wxT("BASECONVRATE"); } };
    struct CURRENCY_SYMBOL { wxString name() const { return wxT("CURRENCY_SYMBOL"); } };
    typedef CURRENCYID PRIMARY;
    enum COLUMN
    {
        COL_CURRENCYID = 0
        , COL_CURRENCYNAME = 1
        , COL_PFX_SYMBOL = 2
        , COL_SFX_SYMBOL = 3
        , COL_DECIMAL_POINT = 4
        , COL_GROUP_SEPARATOR = 5
        , COL_UNIT_NAME = 6
        , COL_CENT_NAME = 7
        , COL_SCALE = 8
        , COL_BASECONVRATE = 9
        , COL_CURRENCY_SYMBOL = 10
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_CURRENCYID);
        result.push_back(COL_CURRENCYNAME);
        result.push_back(COL_PFX_SYMBOL);
        result.push_back(COL_SFX_SYMBOL);
        result.push_back(COL_DECIMAL_POINT);
        result.push_back(COL_GROUP_SEPARATOR);
        result.push_back(COL_UNIT_NAME);
        result.push_back(COL_CENT_NAME);
        result.push_back(COL_SCALE);
        result.push_back(COL_BASECONVRATE);
        result.push_back(COL_CURRENCY_SYMBOL);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_CURRENCYID: return wxT("CURRENCYID");
            case COL_CURRENCYNAME: return wxT("CURRENCYNAME");
            case COL_PFX_SYMBOL: return wxT("PFX_SYMBOL");
            case COL_SFX_SYMBOL: return wxT("SFX_SYMBOL");
            case COL_DECIMAL_POINT: return wxT("DECIMAL_POINT");
            case COL_GROUP_SEPARATOR: return wxT("GROUP_SEPARATOR");
            case COL_UNIT_NAME: return wxT("UNIT_NAME");
            case COL_CENT_NAME: return wxT("CENT_NAME");
            case COL_SCALE: return wxT("SCALE");
            case COL_BASECONVRATE: return wxT("BASECONVRATE");
            case COL_CURRENCY_SYMBOL: return wxT("CURRENCY_SYMBOL");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("CURRENCYID") == name) return COL_CURRENCYID;
        else if (wxT("CURRENCYNAME") == name) return COL_CURRENCYNAME;
        else if (wxT("PFX_SYMBOL") == name) return COL_PFX_SYMBOL;
        else if (wxT("SFX_SYMBOL") == name) return COL_SFX_SYMBOL;
        else if (wxT("DECIMAL_POINT") == name) return COL_DECIMAL_POINT;
        else if (wxT("GROUP_SEPARATOR") == name) return COL_GROUP_SEPARATOR;
        else if (wxT("UNIT_NAME") == name) return COL_UNIT_NAME;
        else if (wxT("CENT_NAME") == name) return COL_CENT_NAME;
        else if (wxT("SCALE") == name) return COL_SCALE;
        else if (wxT("BASECONVRATE") == name) return COL_BASECONVRATE;
        else if (wxT("CURRENCY_SYMBOL") == name) return COL_CURRENCY_SYMBOL;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int CURRENCYID;//  primay key
        wxString CURRENCYNAME;
        wxString PFX_SYMBOL;
        wxString SFX_SYMBOL;
        wxString DECIMAL_POINT;
        wxString GROUP_SEPARATOR;
        wxString UNIT_NAME;
        wxString CENT_NAME;
        double SCALE;
        double BASECONVRATE;
        wxString CURRENCY_SYMBOL;
        int id() const { return CURRENCYID; }
        void id(int id) { CURRENCYID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            CURRENCYID = -1;
            SCALE = 0.0;
            BASECONVRATE = 0.0;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            CURRENCYID = q.GetInt(wxT("CURRENCYID"));
            CURRENCYNAME = q.GetString(wxT("CURRENCYNAME"));
            PFX_SYMBOL = q.GetString(wxT("PFX_SYMBOL"));
            SFX_SYMBOL = q.GetString(wxT("SFX_SYMBOL"));
            DECIMAL_POINT = q.GetString(wxT("DECIMAL_POINT"));
            GROUP_SEPARATOR = q.GetString(wxT("GROUP_SEPARATOR"));
            UNIT_NAME = q.GetString(wxT("UNIT_NAME"));
            CENT_NAME = q.GetString(wxT("CENT_NAME"));
            SCALE = q.GetDouble(wxT("SCALE"));
            BASECONVRATE = q.GetDouble(wxT("BASECONVRATE"));
            CURRENCY_SYMBOL = q.GetString(wxT("CURRENCY_SYMBOL"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_CURRENCYID: ret << CURRENCYID; break;
                case COL_CURRENCYNAME: ret << CURRENCYNAME; break;
                case COL_PFX_SYMBOL: ret << PFX_SYMBOL; break;
                case COL_SFX_SYMBOL: ret << SFX_SYMBOL; break;
                case COL_DECIMAL_POINT: ret << DECIMAL_POINT; break;
                case COL_GROUP_SEPARATOR: ret << GROUP_SEPARATOR; break;
                case COL_UNIT_NAME: ret << UNIT_NAME; break;
                case COL_CENT_NAME: ret << CENT_NAME; break;
                case COL_SCALE: ret << SCALE; break;
                case COL_BASECONVRATE: ret << BASECONVRATE; break;
                case COL_CURRENCY_SYMBOL: ret << CURRENCY_SYMBOL; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << CURRENCYID;
            ret << delimiter << CURRENCYNAME;
            ret << delimiter << PFX_SYMBOL;
            ret << delimiter << SFX_SYMBOL;
            ret << delimiter << DECIMAL_POINT;
            ret << delimiter << GROUP_SEPARATOR;
            ret << delimiter << UNIT_NAME;
            ret << delimiter << CENT_NAME;
            ret << delimiter << SCALE;
            ret << delimiter << BASECONVRATE;
            ret << delimiter << CURRENCY_SYMBOL;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
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

    wxString name() const { return wxT("CURRENCYFORMATS_V1"); }

    DB_View_CURRENCYFORMATS_V1() 
    {
        query_ = wxT("SELECT CURRENCYID, CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT, GROUP_SEPARATOR, UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE, CURRENCY_SYMBOL FROM CURRENCYFORMATS_V1 ");
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
            sql = wxT("INSERT INTO CURRENCYFORMATS_V1(CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT, GROUP_SEPARATOR, UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE, CURRENCY_SYMBOL) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE CURRENCYFORMATS_V1 SET CURRENCYNAME = ?, PFX_SYMBOL = ?, SFX_SYMBOL = ?, DECIMAL_POINT = ?, GROUP_SEPARATOR = ?, UNIT_NAME = ?, CENT_NAME = ?, SCALE = ?, BASECONVRATE = ?, CURRENCY_SYMBOL = ? WHERE CURRENCYID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->CURRENCYNAME);
            stmt.Bind(2, entity->PFX_SYMBOL);
            stmt.Bind(3, entity->SFX_SYMBOL);
            stmt.Bind(4, entity->DECIMAL_POINT);
            stmt.Bind(5, entity->GROUP_SEPARATOR);
            stmt.Bind(6, entity->UNIT_NAME);
            stmt.Bind(7, entity->CENT_NAME);
            stmt.Bind(8, entity->SCALE);
            stmt.Bind(9, entity->BASECONVRATE);
            stmt.Bind(10, entity->CURRENCY_SYMBOL);
            if (entity->id() > 0)
                stmt.Bind(11, entity->CURRENCYID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CURRENCYFORMATS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM CURRENCYFORMATS_V1 WHERE CURRENCYID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CURRENCYFORMATS_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE CURRENCYID = ?"));
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
            wxLogError(wxT("CURRENCYFORMATS_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("CURRENCYFORMATS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("CURRENCYFORMATS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_CURRENCYFORMATS_V1 CURRENCYFORMATS_V1;

struct DB_View_INFOTABLE_V1 : public DB_View
{
    struct Data;
    typedef DB_View_INFOTABLE_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_INFOTABLE_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE INFOTABLE_V1(INFOID integer not null primary key, INFONAME TEXT NOT NULL, INFOVALUE TEXT NOT NULL)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("INFOTABLE_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct INFOID { wxString name() const { return wxT("INFOID"); } };
    struct INFONAME { wxString name() const { return wxT("INFONAME"); } };
    struct INFOVALUE { wxString name() const { return wxT("INFOVALUE"); } };
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
            case COL_INFOID: return wxT("INFOID");
            case COL_INFONAME: return wxT("INFONAME");
            case COL_INFOVALUE: return wxT("INFOVALUE");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("INFOID") == name) return COL_INFOID;
        else if (wxT("INFONAME") == name) return COL_INFONAME;
        else if (wxT("INFOVALUE") == name) return COL_INFOVALUE;

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
        
            INFOID = q.GetInt(wxT("INFOID"));
            INFONAME = q.GetString(wxT("INFONAME"));
            INFOVALUE = q.GetString(wxT("INFOVALUE"));
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

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
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
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
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

    wxString name() const { return wxT("INFOTABLE_V1"); }

    DB_View_INFOTABLE_V1() 
    {
        query_ = wxT("SELECT INFOID, INFONAME, INFOVALUE FROM INFOTABLE_V1 ");
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
            sql = wxT("INSERT INTO INFOTABLE_V1(INFONAME, INFOVALUE) VALUES(?, ?)");
        }
        else
        {
            sql = wxT("UPDATE INFOTABLE_V1 SET INFONAME = ?, INFOVALUE = ? WHERE INFOID = ?");
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
            wxLogError(wxT("INFOTABLE_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM INFOTABLE_V1 WHERE INFOID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("INFOTABLE_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE INFOID = ?"));
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
            wxLogError(wxT("INFOTABLE_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("INFOTABLE_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("INFOTABLE_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_INFOTABLE_V1 INFOTABLE_V1;

struct DB_View_PAYEE_V1 : public DB_View
{
    struct Data;
    typedef DB_View_PAYEE_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_PAYEE_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE PAYEE_V1(PAYEEID integer primary key, PAYEENAME TEXT NOT NULL, CATEGID integer, SUBCATEGID integer)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("PAYEE_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct PAYEEID { wxString name() const { return wxT("PAYEEID"); } };
    struct PAYEENAME { wxString name() const { return wxT("PAYEENAME"); } };
    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    struct SUBCATEGID { wxString name() const { return wxT("SUBCATEGID"); } };
    typedef PAYEEID PRIMARY;
    enum COLUMN
    {
        COL_PAYEEID = 0
        , COL_PAYEENAME = 1
        , COL_CATEGID = 2
        , COL_SUBCATEGID = 3
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_PAYEEID);
        result.push_back(COL_PAYEENAME);
        result.push_back(COL_CATEGID);
        result.push_back(COL_SUBCATEGID);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_PAYEEID: return wxT("PAYEEID");
            case COL_PAYEENAME: return wxT("PAYEENAME");
            case COL_CATEGID: return wxT("CATEGID");
            case COL_SUBCATEGID: return wxT("SUBCATEGID");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("PAYEEID") == name) return COL_PAYEEID;
        else if (wxT("PAYEENAME") == name) return COL_PAYEENAME;
        else if (wxT("CATEGID") == name) return COL_CATEGID;
        else if (wxT("SUBCATEGID") == name) return COL_SUBCATEGID;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int PAYEEID;//  primay key
        wxString PAYEENAME;
        int CATEGID;
        int SUBCATEGID;
        int id() const { return PAYEEID; }
        void id(int id) { PAYEEID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            PAYEEID = -1;
            CATEGID = -1;
            SUBCATEGID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            PAYEEID = q.GetInt(wxT("PAYEEID"));
            PAYEENAME = q.GetString(wxT("PAYEENAME"));
            CATEGID = q.GetInt(wxT("CATEGID"));
            SUBCATEGID = q.GetInt(wxT("SUBCATEGID"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_PAYEEID: ret << PAYEEID; break;
                case COL_PAYEENAME: ret << PAYEENAME; break;
                case COL_CATEGID: ret << CATEGID; break;
                case COL_SUBCATEGID: ret << SUBCATEGID; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << PAYEEID;
            ret << delimiter << PAYEENAME;
            ret << delimiter << CATEGID;
            ret << delimiter << SUBCATEGID;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 4
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("PAYEE_V1"); }

    DB_View_PAYEE_V1() 
    {
        query_ = wxT("SELECT PAYEEID, PAYEENAME, CATEGID, SUBCATEGID FROM PAYEE_V1 ");
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
            sql = wxT("INSERT INTO PAYEE_V1(PAYEENAME, CATEGID, SUBCATEGID) VALUES(?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE PAYEE_V1 SET PAYEENAME = ?, CATEGID = ?, SUBCATEGID = ? WHERE PAYEEID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->PAYEENAME);
            stmt.Bind(2, entity->CATEGID);
            stmt.Bind(3, entity->SUBCATEGID);
            if (entity->id() > 0)
                stmt.Bind(4, entity->PAYEEID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("PAYEE_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM PAYEE_V1 WHERE PAYEEID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("PAYEE_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE PAYEEID = ?"));
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
            wxLogError(wxT("PAYEE_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("PAYEE_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("PAYEE_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_PAYEE_V1 PAYEE_V1;

struct DB_View_SETTING_V1 : public DB_View
{
    struct Data;
    typedef DB_View_SETTING_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_SETTING_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE SETTING_V1(SETTINGID integer not null primary key, SETTINGNAME TEXT NOT NULL, SETTINGVALUE TEXT)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SETTING_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct SETTINGID { wxString name() const { return wxT("SETTINGID"); } };
    struct SETTINGNAME { wxString name() const { return wxT("SETTINGNAME"); } };
    struct SETTINGVALUE { wxString name() const { return wxT("SETTINGVALUE"); } };
    typedef SETTINGID PRIMARY;
    enum COLUMN
    {
        COL_SETTINGID = 0
        , COL_SETTINGNAME = 1
        , COL_SETTINGVALUE = 2
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_SETTINGID);
        result.push_back(COL_SETTINGNAME);
        result.push_back(COL_SETTINGVALUE);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_SETTINGID: return wxT("SETTINGID");
            case COL_SETTINGNAME: return wxT("SETTINGNAME");
            case COL_SETTINGVALUE: return wxT("SETTINGVALUE");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("SETTINGID") == name) return COL_SETTINGID;
        else if (wxT("SETTINGNAME") == name) return COL_SETTINGNAME;
        else if (wxT("SETTINGVALUE") == name) return COL_SETTINGVALUE;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int SETTINGID;//  primay key
        wxString SETTINGNAME;
        wxString SETTINGVALUE;
        int id() const { return SETTINGID; }
        void id(int id) { SETTINGID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            SETTINGID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            SETTINGID = q.GetInt(wxT("SETTINGID"));
            SETTINGNAME = q.GetString(wxT("SETTINGNAME"));
            SETTINGVALUE = q.GetString(wxT("SETTINGVALUE"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_SETTINGID: ret << SETTINGID; break;
                case COL_SETTINGNAME: ret << SETTINGNAME; break;
                case COL_SETTINGVALUE: ret << SETTINGVALUE; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << SETTINGID;
            ret << delimiter << SETTINGNAME;
            ret << delimiter << SETTINGVALUE;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
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

    wxString name() const { return wxT("SETTING_V1"); }

    DB_View_SETTING_V1() 
    {
        query_ = wxT("SELECT SETTINGID, SETTINGNAME, SETTINGVALUE FROM SETTING_V1 ");
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
            sql = wxT("INSERT INTO SETTING_V1(SETTINGNAME, SETTINGVALUE) VALUES(?, ?)");
        }
        else
        {
            sql = wxT("UPDATE SETTING_V1 SET SETTINGNAME = ?, SETTINGVALUE = ? WHERE SETTINGID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->SETTINGNAME);
            stmt.Bind(2, entity->SETTINGVALUE);
            if (entity->id() > 0)
                stmt.Bind(3, entity->SETTINGID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SETTING_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM SETTING_V1 WHERE SETTINGID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SETTING_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE SETTINGID = ?"));
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
            wxLogError(wxT("SETTING_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("SETTING_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SETTING_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_SETTING_V1 SETTING_V1;

struct DB_View_SPLITTRANSACTIONS_V1 : public DB_View
{
    struct Data;
    typedef DB_View_SPLITTRANSACTIONS_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_SPLITTRANSACTIONS_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE SPLITTRANSACTIONS_V1(SPLITTRANSID integer primary key, TRANSID numeric NOT NULL, CATEGID integer, SUBCATEGID integer, SPLITTRANSAMOUNT numeric)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct SPLITTRANSID { wxString name() const { return wxT("SPLITTRANSID"); } };
    struct TRANSID { wxString name() const { return wxT("TRANSID"); } };
    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    struct SUBCATEGID { wxString name() const { return wxT("SUBCATEGID"); } };
    struct SPLITTRANSAMOUNT { wxString name() const { return wxT("SPLITTRANSAMOUNT"); } };
    typedef SPLITTRANSID PRIMARY;
    enum COLUMN
    {
        COL_SPLITTRANSID = 0
        , COL_TRANSID = 1
        , COL_CATEGID = 2
        , COL_SUBCATEGID = 3
        , COL_SPLITTRANSAMOUNT = 4
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_SPLITTRANSID);
        result.push_back(COL_TRANSID);
        result.push_back(COL_CATEGID);
        result.push_back(COL_SUBCATEGID);
        result.push_back(COL_SPLITTRANSAMOUNT);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_SPLITTRANSID: return wxT("SPLITTRANSID");
            case COL_TRANSID: return wxT("TRANSID");
            case COL_CATEGID: return wxT("CATEGID");
            case COL_SUBCATEGID: return wxT("SUBCATEGID");
            case COL_SPLITTRANSAMOUNT: return wxT("SPLITTRANSAMOUNT");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("SPLITTRANSID") == name) return COL_SPLITTRANSID;
        else if (wxT("TRANSID") == name) return COL_TRANSID;
        else if (wxT("CATEGID") == name) return COL_CATEGID;
        else if (wxT("SUBCATEGID") == name) return COL_SUBCATEGID;
        else if (wxT("SPLITTRANSAMOUNT") == name) return COL_SPLITTRANSAMOUNT;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int SPLITTRANSID;//  primay key
        double TRANSID;
        int CATEGID;
        int SUBCATEGID;
        double SPLITTRANSAMOUNT;
        int id() const { return SPLITTRANSID; }
        void id(int id) { SPLITTRANSID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            SPLITTRANSID = -1;
            TRANSID = 0.0;
            CATEGID = -1;
            SUBCATEGID = -1;
            SPLITTRANSAMOUNT = 0.0;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            SPLITTRANSID = q.GetInt(wxT("SPLITTRANSID"));
            TRANSID = q.GetDouble(wxT("TRANSID"));
            CATEGID = q.GetInt(wxT("CATEGID"));
            SUBCATEGID = q.GetInt(wxT("SUBCATEGID"));
            SPLITTRANSAMOUNT = q.GetDouble(wxT("SPLITTRANSAMOUNT"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_SPLITTRANSID: ret << SPLITTRANSID; break;
                case COL_TRANSID: ret << TRANSID; break;
                case COL_CATEGID: ret << CATEGID; break;
                case COL_SUBCATEGID: ret << SUBCATEGID; break;
                case COL_SPLITTRANSAMOUNT: ret << SPLITTRANSAMOUNT; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << SPLITTRANSID;
            ret << delimiter << TRANSID;
            ret << delimiter << CATEGID;
            ret << delimiter << SUBCATEGID;
            ret << delimiter << SPLITTRANSAMOUNT;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy() { delete this; }
    };

    enum
    {
        NUM_COLUMNS = 5
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    wxString name() const { return wxT("SPLITTRANSACTIONS_V1"); }

    DB_View_SPLITTRANSACTIONS_V1() 
    {
        query_ = wxT("SELECT SPLITTRANSID, TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT FROM SPLITTRANSACTIONS_V1 ");
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
            sql = wxT("INSERT INTO SPLITTRANSACTIONS_V1(TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT) VALUES(?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE SPLITTRANSACTIONS_V1 SET TRANSID = ?, CATEGID = ?, SUBCATEGID = ?, SPLITTRANSAMOUNT = ? WHERE SPLITTRANSID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->TRANSID);
            stmt.Bind(2, entity->CATEGID);
            stmt.Bind(3, entity->SUBCATEGID);
            stmt.Bind(4, entity->SPLITTRANSAMOUNT);
            if (entity->id() > 0)
                stmt.Bind(5, entity->SPLITTRANSID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM SPLITTRANSACTIONS_V1 WHERE SPLITTRANSID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE SPLITTRANSID = ?"));
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
            wxLogError(wxT("SPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("SPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SPLITTRANSACTIONS_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_SPLITTRANSACTIONS_V1 SPLITTRANSACTIONS_V1;

struct DB_View_STOCK_V1 : public DB_View
{
    struct Data;
    typedef DB_View_STOCK_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_STOCK_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE STOCK_V1(STOCKID integer primary key, HELDAT integer , PURCHASEDATE TEXT NOT NULL, STOCKNAME TEXT, SYMBOL TEXT, NUMSHARES numeric, PURCHASEPRICE numeric NOT NULL, NOTES TEXT, CURRENTPRICE numeric NOT NULL, VALUE numeric, COMMISSION numeric)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("STOCK_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct STOCKID { wxString name() const { return wxT("STOCKID"); } };
    struct HELDAT { wxString name() const { return wxT("HELDAT"); } };
    struct PURCHASEDATE { wxString name() const { return wxT("PURCHASEDATE"); } };
    struct STOCKNAME { wxString name() const { return wxT("STOCKNAME"); } };
    struct SYMBOL { wxString name() const { return wxT("SYMBOL"); } };
    struct NUMSHARES { wxString name() const { return wxT("NUMSHARES"); } };
    struct PURCHASEPRICE { wxString name() const { return wxT("PURCHASEPRICE"); } };
    struct NOTES { wxString name() const { return wxT("NOTES"); } };
    struct CURRENTPRICE { wxString name() const { return wxT("CURRENTPRICE"); } };
    struct VALUE { wxString name() const { return wxT("VALUE"); } };
    struct COMMISSION { wxString name() const { return wxT("COMMISSION"); } };
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

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_STOCKID);
        result.push_back(COL_HELDAT);
        result.push_back(COL_PURCHASEDATE);
        result.push_back(COL_STOCKNAME);
        result.push_back(COL_SYMBOL);
        result.push_back(COL_NUMSHARES);
        result.push_back(COL_PURCHASEPRICE);
        result.push_back(COL_NOTES);
        result.push_back(COL_CURRENTPRICE);
        result.push_back(COL_VALUE);
        result.push_back(COL_COMMISSION);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_STOCKID: return wxT("STOCKID");
            case COL_HELDAT: return wxT("HELDAT");
            case COL_PURCHASEDATE: return wxT("PURCHASEDATE");
            case COL_STOCKNAME: return wxT("STOCKNAME");
            case COL_SYMBOL: return wxT("SYMBOL");
            case COL_NUMSHARES: return wxT("NUMSHARES");
            case COL_PURCHASEPRICE: return wxT("PURCHASEPRICE");
            case COL_NOTES: return wxT("NOTES");
            case COL_CURRENTPRICE: return wxT("CURRENTPRICE");
            case COL_VALUE: return wxT("VALUE");
            case COL_COMMISSION: return wxT("COMMISSION");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("STOCKID") == name) return COL_STOCKID;
        else if (wxT("HELDAT") == name) return COL_HELDAT;
        else if (wxT("PURCHASEDATE") == name) return COL_PURCHASEDATE;
        else if (wxT("STOCKNAME") == name) return COL_STOCKNAME;
        else if (wxT("SYMBOL") == name) return COL_SYMBOL;
        else if (wxT("NUMSHARES") == name) return COL_NUMSHARES;
        else if (wxT("PURCHASEPRICE") == name) return COL_PURCHASEPRICE;
        else if (wxT("NOTES") == name) return COL_NOTES;
        else if (wxT("CURRENTPRICE") == name) return COL_CURRENTPRICE;
        else if (wxT("VALUE") == name) return COL_VALUE;
        else if (wxT("COMMISSION") == name) return COL_COMMISSION;

        return COLUMN(-1);
    }
    
    struct Data
    {
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

        Data(Self* view = 0) 
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

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            STOCKID = q.GetInt(wxT("STOCKID"));
            HELDAT = q.GetInt(wxT("HELDAT"));
            PURCHASEDATE = q.GetString(wxT("PURCHASEDATE"));
            STOCKNAME = q.GetString(wxT("STOCKNAME"));
            SYMBOL = q.GetString(wxT("SYMBOL"));
            NUMSHARES = q.GetDouble(wxT("NUMSHARES"));
            PURCHASEPRICE = q.GetDouble(wxT("PURCHASEPRICE"));
            NOTES = q.GetString(wxT("NOTES"));
            CURRENTPRICE = q.GetDouble(wxT("CURRENTPRICE"));
            VALUE = q.GetDouble(wxT("VALUE"));
            COMMISSION = q.GetDouble(wxT("COMMISSION"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_STOCKID: ret << STOCKID; break;
                case COL_HELDAT: ret << HELDAT; break;
                case COL_PURCHASEDATE: ret << PURCHASEDATE; break;
                case COL_STOCKNAME: ret << STOCKNAME; break;
                case COL_SYMBOL: ret << SYMBOL; break;
                case COL_NUMSHARES: ret << NUMSHARES; break;
                case COL_PURCHASEPRICE: ret << PURCHASEPRICE; break;
                case COL_NOTES: ret << NOTES; break;
                case COL_CURRENTPRICE: ret << CURRENTPRICE; break;
                case COL_VALUE: ret << VALUE; break;
                case COL_COMMISSION: ret << COMMISSION; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << STOCKID;
            ret << delimiter << HELDAT;
            ret << delimiter << PURCHASEDATE;
            ret << delimiter << STOCKNAME;
            ret << delimiter << SYMBOL;
            ret << delimiter << NUMSHARES;
            ret << delimiter << PURCHASEPRICE;
            ret << delimiter << NOTES;
            ret << delimiter << CURRENTPRICE;
            ret << delimiter << VALUE;
            ret << delimiter << COMMISSION;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
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

    wxString name() const { return wxT("STOCK_V1"); }

    DB_View_STOCK_V1() 
    {
        query_ = wxT("SELECT STOCKID, HELDAT, PURCHASEDATE, STOCKNAME, SYMBOL, NUMSHARES, PURCHASEPRICE, NOTES, CURRENTPRICE, VALUE, COMMISSION FROM STOCK_V1 ");
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
            sql = wxT("INSERT INTO STOCK_V1(HELDAT, PURCHASEDATE, STOCKNAME, SYMBOL, NUMSHARES, PURCHASEPRICE, NOTES, CURRENTPRICE, VALUE, COMMISSION) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        }
        else
        {
            sql = wxT("UPDATE STOCK_V1 SET HELDAT = ?, PURCHASEDATE = ?, STOCKNAME = ?, SYMBOL = ?, NUMSHARES = ?, PURCHASEPRICE = ?, NOTES = ?, CURRENTPRICE = ?, VALUE = ?, COMMISSION = ? WHERE STOCKID = ?");
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

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("STOCK_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM STOCK_V1 WHERE STOCKID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("STOCK_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE STOCKID = ?"));
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
            wxLogError(wxT("STOCK_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("STOCK_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("STOCK_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_STOCK_V1 STOCK_V1;

struct DB_View_SUBCATEGORY_V1 : public DB_View
{
    struct Data;
    typedef DB_View_SUBCATEGORY_V1 Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_SUBCATEGORY_V1() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }

    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("CREATE TABLE SUBCATEGORY_V1(SUBCATEGID integer primary key, SUBCATEGNAME TEXT NOT NULL, CATEGID integer NOT NULL)"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SUBCATEGORY_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct SUBCATEGID { wxString name() const { return wxT("SUBCATEGID"); } };
    struct SUBCATEGNAME { wxString name() const { return wxT("SUBCATEGNAME"); } };
    struct CATEGID { wxString name() const { return wxT("CATEGID"); } };
    typedef SUBCATEGID PRIMARY;
    enum COLUMN
    {
        COL_SUBCATEGID = 0
        , COL_SUBCATEGNAME = 1
        , COL_CATEGID = 2
    };

    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_SUBCATEGID);
        result.push_back(COL_SUBCATEGNAME);
        result.push_back(COL_CATEGID);
        return result;
    }

    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_SUBCATEGID: return wxT("SUBCATEGID");
            case COL_SUBCATEGNAME: return wxT("SUBCATEGNAME");
            case COL_CATEGID: return wxT("CATEGID");
            default: break;
        }
        
        return wxT("UNKNOWN");
    }

    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("SUBCATEGID") == name) return COL_SUBCATEGID;
        else if (wxT("SUBCATEGNAME") == name) return COL_SUBCATEGNAME;
        else if (wxT("CATEGID") == name) return COL_CATEGID;

        return COLUMN(-1);
    }
    
    struct Data
    {
        Self* view_;
    
        int SUBCATEGID;//  primay key
        wxString SUBCATEGNAME;
        int CATEGID;
        int id() const { return SUBCATEGID; }
        void id(int id) { SUBCATEGID = id; }

        Data(Self* view = 0) 
        {
            view_ = view;
        
            SUBCATEGID = -1;
            CATEGID = -1;
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            SUBCATEGID = q.GetInt(wxT("SUBCATEGID"));
            SUBCATEGNAME = q.GetString(wxT("SUBCATEGNAME"));
            CATEGID = q.GetInt(wxT("CATEGID"));
        }

        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_SUBCATEGID: ret << SUBCATEGID; break;
                case COL_SUBCATEGNAME: ret << SUBCATEGNAME; break;
                case COL_CATEGID: ret << CATEGID; break;
                default: break;
            }
            
            return ret;
        }

        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }

        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << SUBCATEGID;
            ret << delimiter << SUBCATEGNAME;
            ret << delimiter << CATEGID;
            return ret;
        }

        bool save(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not save"));
                return false;
            }

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError(wxT("can not remove"));
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

    wxString name() const { return wxT("SUBCATEGORY_V1"); }

    DB_View_SUBCATEGORY_V1() 
    {
        query_ = wxT("SELECT SUBCATEGID, SUBCATEGNAME, CATEGID FROM SUBCATEGORY_V1 ");
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
            sql = wxT("INSERT INTO SUBCATEGORY_V1(SUBCATEGNAME, CATEGID) VALUES(?, ?)");
        }
        else
        {
            sql = wxT("UPDATE SUBCATEGORY_V1 SET SUBCATEGNAME = ?, CATEGID = ? WHERE SUBCATEGID = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->SUBCATEGNAME);
            stmt.Bind(2, entity->CATEGID);
            if (entity->id() > 0)
                stmt.Bind(3, entity->SUBCATEGID);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SUBCATEGORY_V1: Exception %s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }


    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM SUBCATEGORY_V1 WHERE SUBCATEGID = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SUBCATEGORY_V1: Exception %s"), e.GetMessage().c_str());
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
        wxString where = wxString::Format(wxT(" WHERE SUBCATEGID = ?"));
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
            wxLogError(wxT("SUBCATEGORY_V1: Exception %s"), e.GetMessage().c_str());
        }
 
        return entity;
    }

    template<class C, class V>
    Data_Set find(wxSQLite3Database* db, const V& v)
    {
        Data_Set result;
        try
        {
            C c;
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + wxT(" WHERE ") + c.name() + wxT(" = ?"));
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
            wxLogError(wxT("SUBCATEGORY_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

    Data_Set all(wxSQLite3Database* db, const wxString& filter = wxEmptyString)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("SUBCATEGORY_V1: Exception %s"), e.GetMessage().c_str());
        }

        return result;
    }

};
static DB_View_SUBCATEGORY_V1 SUBCATEGORY_V1;

#endif // 


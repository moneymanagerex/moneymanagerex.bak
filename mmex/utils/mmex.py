#!/usr/bin/env python
# vi:tabstop=4:expandtab:shiftwidth=4:softtabstop=4:autoindent:smarttab

import sys
import datetime
import sqlite3

def get_table_list(cursor):
    "Returns a list of table names in the current database."
    # Skip the sqlite_sequence system table used for autoincrement key
    # generation.
    cursor.execute("""
        SELECT name, sql FROM sqlite_master
        WHERE type='table' AND NOT name='sqlite_sequence'
        ORDER BY name""")
    return [(row[0], row[1]) for row in cursor.fetchall()]


def _table_info(cursor, name):
    cursor.execute('PRAGMA table_info(%s)' % name)
    # cid, name, type, notnull, dflt_value, pk
    return [{'name': field[1],
        'type': field[2].upper(),
        'null_ok': not field[3],
        'pk': field[5]     # undocumented
        } for field in cursor.fetchall()]

base_data_types_reverse = {
    'TEXT': 'wxString',
    'NUMERIC': 'double',
    'INTEGER': 'int',
    'REAL': 'double',
    'BLOB': 'wxString',
}

base_data_types_function = {
    'TEXT': 'GetString',
    'NUMERIC': 'GetDouble',
    'INTEGER': 'GetInt',
    'REAL': 'GetDouble',
}

class DB_View:
    def __init__(self, table, fields):
        self._table = table
        self._fields = fields
        self._primay_key = [field['name'] for field in self._fields if field['pk']][0]
    
    def to_string(self, sql = None):
        
        s = '''
struct DB_View_%s : public DB_View
{
    struct Data;
    typedef DB_View_%s Self;
    typedef std::vector<Self::Data> Data_Set;
    typedef std::vector<Self::Data*> Cache;
    Cache cache_;
    ~DB_View_%s() 
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
    }
''' % (self._table, self._table, self._table)
        
        s += '''
    bool ensure(wxSQLite3Database* db) const
    {
        if (exists(db)) return true;

        try
        {
            db->ExecuteUpdate(wxT("%s"));
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("%s: Exception %%s"), e.GetMessage().c_str());
            return false;
        }

        return true;
    }
''' % (sql.replace('\n', ''), self._table)

        for field in self._fields:
            s += '''
    struct %s { wxString name() const { return wxT("%s"); } };''' % (field['name'], field['name'])

        s += '''
    typedef %s PRIMARY;''' % self._primay_key

        s += '''
    enum COLUMN
    {
        COL_%s = 0''' % self._primay_key.upper()
        
        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s += '''
        , COL_%s = %d''' % (name.upper(), index +1)

        s +='''
    };
'''
        s += '''
    std::vector<COLUMN> all_columns() const
    {
        std::vector<COLUMN> result;
        result.push_back(COL_%s);'''% self._primay_key.upper();
        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s += '''
        result.push_back(COL_%s);''' % name.upper()

        s += '''
        return result;
    }
'''
        s += '''
    wxString column_to_name(COLUMN col) const
    {
        switch(col)
        {
            case COL_%s: return wxT("%s");''' % (self._primay_key.upper(), self._primay_key)

        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s += '''
            case COL_%s: return wxT("%s");''' %(name.upper(), name)
        s +='''
            default: break;
        }
        
        return wxT("UNKNOWN");
    }
'''
        s +='''
    COLUMN name_to_column(const wxString& name) const
    {
        if (wxT("%s") == name) return COL_%s;''' % (self._primay_key, self._primay_key.upper())

        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s += '''
        else if (wxT("%s") == name) return COL_%s;''' %(name, name.upper())

        s += '''

        return COLUMN(-1);
    }
    '''
        s += '''
    struct Data
    {
        Self* view_;
    '''
        for field in self._fields:
            s += '''
        %s %s;%s''' % (base_data_types_reverse[field['type']], field['name'], field['pk'] and '//  primay key' or '')

        s +='''
        int id() const { return %s; }
        void id(int id) { %s = id; }
''' % (self._primay_key, self._primay_key)
        
        s += '''
        Data(Self* view = 0) 
        {
            view_ = view;
        '''

        for field in self._fields:
            type = base_data_types_reverse[field['type']]
            if type == 'wxString': 
                continue
            elif type == 'double':
                s += '''
            %s = 0.0;''' % field['name']
            elif type == 'int':
                s += '''
            %s = -1;''' % field['name']
                

        s += '''
        }

        Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        '''
        for field in self._fields:
            func = base_data_types_function[field['type']]
            s += '''
            %s = q.%s(wxT("%s"));''' % (field['name'], func, field['name'])

        s += '''
        }
'''
        s +='''
        wxString to_string(COLUMN col) const
        {
            wxString ret = wxEmptyString;
            switch(col)
            {
                case COL_%s: ret << %s; break;''' % (self._primay_key.upper(), self._primay_key)

        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s += '''
                case COL_%s: ret << %s; break;''' %(name.upper(), name)
        s +='''
                default: break;
            }
            
            return ret;
        }
'''
        s +='''
        wxString to_string(std::vector<COLUMN> columns, const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            std::vector<COLUMN>::const_iterator it = columns.begin(); 
            if (it != columns.end()) ret << to_string(*it);
            for ( ; it != columns.end(); ++ it) ret << delimiter << to_string(*it);

            return ret;
        }
'''
        s += '''
        wxString to_string(const wxString& delimiter = wxT(",")) const
        {
            wxString ret = wxEmptyString;
            ret << %s;''' % self._primay_key

        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s += '''
            ret << delimiter << %s;''' % name
        
        s +='''
            return ret;
        }
'''
        s += '''
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
'''
        s +='''
    enum
    {
        NUM_COLUMNS = %d
    };

    size_t num_columns() const { return NUM_COLUMNS; }
''' % len(self._fields)
        
        s += '''
    wxString name() const { return wxT("%s"); }
''' % self._table
        
        s +='''
    DB_View_%s() 
    {
        query_ = wxT("SELECT %s FROM %s ");
    }
''' % (self._table, ', '.join([field['name'] for field in self._fields]), self._table)
        
        s +='''
    Self::Data* create()
    {
        Self::Data* entity = new Self::Data(this);
        cache_.push_back(entity);
        return entity;
    }
'''
        s +='''
    bool save(Self::Data* entity, wxSQLite3Database* db)
    {
        wxString sql = wxEmptyString;
        if (entity->id() < 0) //  new & insert
        {
            sql = wxT("INSERT INTO %s(%s) VALUES(%s)");
        }''' % (self._table, ', '.join([field['name'] for field in self._fields if not field['pk']]), ', '.join(['?' for field in self._fields if not field['pk']]))
        
        s +='''
        else
        {
            sql = wxT("UPDATE %s SET %s WHERE %s = ?");
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
''' % (self._table, ', '.join([field['name'] + ' = ?' for field in self._fields if not field['pk']]), self._primay_key)
        
        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s +='''
            stmt.Bind(%d, entity->%s);'''% (index + 1, name)
            
        
        s +='''
            if (entity->id() > 0)
                stmt.Bind(%d, entity->%s);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("%s: Exception %%s"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }
''' % (len(self._fields), self._primay_key, self._table)
        s +='''

    bool remove(int id, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM %s WHERE %s = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("%s: Exception %%s"), e.GetMessage().c_str());
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
''' % (self._table, self._primay_key, self._table)
        
        s +='''
    
    Self::Data* get(int id, wxSQLite3Database* db)
    {
        for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
        {
            Self::Data* entity = *it;
            if (entity->id() == id) 
                return entity;
        }

        Self::Data* entity = 0;
        wxString where = wxString::Format(wxT(" WHERE %s = ?"));
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
            wxLogError(wxT("%s: Exception %%s"), e.GetMessage().c_str());
        }
 
        return entity;
    }
''' % (self._primay_key, self._table)

        s +='''
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
            wxLogError(wxT("%s: Exception %%s"), e.GetMessage().c_str());
        }

        return result;
    }
''' % self._table

        s +='''
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
            wxLogError(wxT("%s: Exception %%s"), e.GetMessage().c_str());
        }

        return result;
    }
''' % self._table

            
        s += '''
};
static DB_View_%s %s;
''' % (self._table, self._table)
        return s


if __name__ == '__main__':
    
    conn, cur = None, None
    try:
        conn = sqlite3.connect(sys.argv[1])
        conn.row_factory = sqlite3.Row 
        cur = conn.cursor()
    except:
        pass

    code =  '''// -*- C++ -*-
//=============================================================================
/**
 *      Copyright (c) 2012 Guan Lisheng
 *
 *      @file
 *
 *      @author [%s]
 *
 *      @brief
 *
 *      Revision History:
 *          AUTO GENERATED at %s.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef _MM_EX_DB_VIEW_H_
#define _MM_EX_DB_VIEW_H_
'''% (sys.argv[0], str(datetime.datetime.now()))
    
    code +='''
#include <vector>
#include <map>
#include <algorithm>

class wxString;
class wxSQLite3Statement;
class wxSQLite3Database;
class wxSQLite3ResultSet;
'''
    
    code +='''
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
'''
   
    for table, sql in get_table_list(cur):
        fields = _table_info(cur, table)
        view = DB_View(table, fields)
        code += view.to_string(sql)

    code +='''
#endif // 
'''

    print code


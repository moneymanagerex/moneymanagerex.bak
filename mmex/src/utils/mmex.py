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
        SELECT name FROM sqlite_master
        WHERE type='table' AND NOT name='sqlite_sequence'
        ORDER BY name""")
    return [row[0] for row in cursor.fetchall()]


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
    
    def to_string(self):
        
        s = '''
struct DB_View_%s : public DB_View
{
    typedef DB_View_%s Self;
    ~DB_View_%s() {}
''' % (self._table, self._table, self._table)
    
        s += '''
    struct Data
    {
        Self* view_;
    '''
        for field in self._fields:
            s += '''
        %s %s; %s''' % (base_data_types_reverse[field['type']], field['name'], field['pk'] and '//primay key' or '')

        primay_key = [field['name'] for field in self._fields if field['pk']][0]
        s +='''
        int id() const { return %s; }
        void id(int id) { %s = id; }
''' % (primay_key, primay_key)
        
        s += '''
        Data(Self* view) 
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
            %s = -1; ''' % field['name']
                

        s += '''
        }

        Data(wxSQLite3ResultSet& q)
        {
            view_ = 0;
        '''
        for field in self._fields:
            func = base_data_types_function[field['type']]
            s += '''
            %s = q.%s(wxT("%s"));''' % (field['name'], func, field['name'])

        s += '''
        }

        bool save(wxSQLite3Database* db)
        {
            if (! view_ || !db) return false;

            return view_->save(this, db);
        }

        bool remove(wxSQLite3Database* db)
        {
            if (! view_ || !db) return false;
            
            return view_->remove(this, db);
        }
    };
'''
 
        s += '''
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
        query_ = wxT("SELECT %s FROM %s");
    }
''' % (self._table, ', '.join([field['name'] for field in self._fields]), self._table)
        
        s +='''
    Self::Data* create()
    {
        return new Self::Data(this);
    }
'''
        s +='''
    bool save(Self::Data* entity, wxSQLite3Database* db)
    {
        wxString sql = wxEmptyString;
        if (entity->id() < 0) //new & insert
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
''' % (self._table, ', '.join([field['name'] + ' = ?' for field in self._fields if not field['pk']]), primay_key)
        
        for index, name in enumerate([field['name'] for field in self._fields if not field['pk']]):
            s +='''
            stmt.Bind(%d, entity->%s);'''% (index + 1, name)
            
        
        s +='''
            stmt.Bind(%d, entity->%s);

            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("%s: Exception"), e.GetMessage().c_str());
            return false;
        }

        if (entity->id() < 0) entity->id((db->GetLastRowId()).ToLong());
        return true;
    }
''' % (len(self._fields), primay_key, self._table)
        s +='''

    bool remove(Self::Data* entity, wxSQLite3Database* db)
    {
        try
        {
            wxString sql = wxT("DELETE FROM %s WHERE %s = ?");
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, entity->id());
            stmt.ExecuteUpdate();
            stmt.Finalize();
        }
        catch(wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("%s: Exception"), e.GetMessage().c_str());
            return false;
        }
        entity->id(-1);
        return true;
    }
''' % (self._table, primay_key, self._table)
        
        s +='''
    
    Self::Data* find(int id, wxSQLite3Database* db)
    {
        Self::Data* entity = 0;
        wxString where = wxString::Format(wxT(" WHERE %s = ?"));
        try
        {
           wxSQLite3Statement stmt = db->PrepareStatement(this->query() + where);
           stmt.Bind(1, id);

           wxSQLite3ResultSet q = stmt.ExecuteQuery();
           if(q.NextRow())
                entity = new Self::Data(q);
        }
        catch(wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("%s: Exception"), e.GetMessage().c_str());
        }
 
        return entity;
    }
''' % (primay_key, self._table)

        s +='''
    std::map<int, Self::Data> all(wxSQLite3Database* db, const wxString& filter = wxEmptyString) const
    {
        std::map<int, Self::Data> result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(this->query() + filter);

            while(q.NextRow())
            {
                Self::Data entity(q);
                result.insert(std::make_pair(entity.id(), entity));
            }

            q.Finalize();
        }
        catch(wxSQLite3Exception &e) 
        { 
            wxLogError(wxT("%s: Exception"), e.GetMessage().c_str());
        }

        return result;
    }
''' % self._table

            
        s += '''
};\n''' 
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
 *      Copyright (c)
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
'''% (sys.argv[0], str(datetime.datetime.now()))
    
    code +='''
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
    wxString query() const { return this->query_; }
    virtual size_t num_columns() const = 0;
    virtual wxString name() const = 0;
};
'''
   
    for table in get_table_list(cur):
        fields = _table_info(cur, table)
        view = DB_View(table, fields)
        code += view.to_string()

    print code


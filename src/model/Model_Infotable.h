
#ifndef MODEL_INFOTABLE_H
#define MODEL_INFOTABLE_H

#include "Model.h"
#include "db/DB_Table_Infotable_V1.h"
#include "defs.h"
#include "constants.h"

class Model_Infotable : public Model, public DB_Table_INFOTABLE_V1
{
    using DB_Table_INFOTABLE_V1::all;
public:
    Model_Infotable(): Model(), DB_Table_INFOTABLE_V1() {};
    ~Model_Infotable() 
    {
    };

public:
    static Model_Infotable& instance()
    {
        return Singleton<Model_Infotable>::instance();
    }
    static Model_Infotable& instance(wxSQLite3Database* db)
    {
        Model_Infotable& ins = Singleton<Model_Infotable>::instance();
        ins.db_ = db;
        return ins;
    }

private:
    Data_Set all(COLUMN col = COLUMN(0), bool asc = true)
    {
        this->ensure(this->db_);
        return this->all(this->db_, col, asc);
    }

public:
    // Setter
    void Set(const wxString& key, int value)
    {
        this->Set(key, wxString::Format("%d", value));
    }

    void Set(const wxString& key, const wxDateTime& date)
    {
        this->Set(key, date.FormatISODate());
    }
    void Set(const wxString& key, const wxString& value)
    {
        Data* info = 0;
        for (auto& record: this->all())
        {
            if (record.INFONAME == key)
            {
                info = this->get(record.INFOID, this->db_);
                break;
            }
        }
        if (info)
        {
            info->INFOVALUE= value;
            info->save(this->db_);
        }
        else
        {
            info = this->create();
            info->INFONAME = key;
            info->INFOVALUE = value;
            info->save(this->db_);
        }
    }
public:
    // Getter
    bool GetBoolInfo(const wxString& key, bool default_value)
    {
        wxString value = this->GetStringInfo(key, "");
        if (value == "TRUE") return true;
        if (value == "FALSE") return false;

        return default_value; 
    }
    int GetIntInfo(const wxString& key, int default_value)
    {
        wxString value = this->GetStringInfo(key, "");
        if (!value.IsEmpty() && value.IsNumber()) return wxAtoi(value);

        return default_value;
    }
    wxString GetStringInfo(const wxString& key, const wxString& default_value)
    {
        for (const auto& record: this->all())
        {
            if (record.INFONAME == key) 
                return record.INFOVALUE;
        }

        return default_value;
    }
    int GetBaseCurrencyId()
    {
        return this->GetIntInfo("BASECURRENCYID", -1);
    }
    bool Exists(const wxString& key)
    {
        for (const auto& record: this->all())
        {
            if (record.INFONAME == key) 
                return true;
        }
        return false;
    }

    bool checkDBVersion()
    {
        if (!this->Exists("DATAVERSION")) return false;

        return this->GetIntInfo("DATAVERSION", 0) >= mmex::MIN_DATAVERSION;
    }
};

#endif // 

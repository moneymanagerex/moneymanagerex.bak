
#ifndef MODEL_CHECKING_H
#define MODEL_CHECKING_H

#include "Model.h"
#include "db/DB_Table_Checkingaccount_V1.h"
#include "Model_Splittransaction.h"

class Model_Checking : public Model, public DB_Table_CHECKINGACCOUNT_V1
{
    using DB_Table_CHECKINGACCOUNT_V1::all;
    using DB_Table_CHECKINGACCOUNT_V1::find;
    using DB_Table_CHECKINGACCOUNT_V1::get;
public:
    enum TYPE { WITHDRAWAL = 0, DEPOSIT, TRANSFER };
public:
    Model_Checking(): Model(), DB_Table_CHECKINGACCOUNT_V1() 
    {
        this->types_.Add(wxTRANSLATE("Withdrawal"));
        this->types_.Add(wxTRANSLATE("Deposit"));
        this->types_.Add(wxTRANSLATE("Transfer"));
    };
    ~Model_Checking() {};

public:
    wxArrayString types_;

public:
    static Model_Checking& instance()
    {
        return Singleton<Model_Checking>::instance();
    }
    static Model_Checking& instance(wxSQLite3Database* db)
    {
        Model_Checking& ins = Singleton<Model_Checking>::instance();
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
    template<class V1, class V2>
    Data_Set find(COLUMN col1, const V1& v1, COLUMN col2, const V2& v2, bool op_and = true)
    {
        return find(db_, col1, v1, col2, v2, op_and);
    }
    Data* get(int id)
    {
        return this->get(id, this->db_);
    }
    int save(Data* r)
    {
        r->save(this->db_);
        return r->id();
    }
    int save(Data_Set& rows)
    {
        this->Begin();
        for (auto& r : rows) this->save(&r);
        this->Commit();

        return rows.size();
    }
public:
    static Model_Splittransaction::Data_Set splittransaction(const Data* r)
    {
        return Model_Splittransaction::instance().find(Model_Splittransaction::COL_TRANSID, r->TRANSID);
    }
    static Model_Splittransaction::Data_Set splittransaction(const Data& r)
    {
        return Model_Splittransaction::instance().find(Model_Splittransaction::COL_TRANSID, r.TRANSID);
    }
public:
    static wxDate TRANSDATE(const Data* r) { return Model::to_date(r->TRANSDATE); }
    static wxDate TRANSDATE(const Data& r) { return Model::to_date(r.TRANSDATE); }
    static TYPE type(const Data* r)
    {
        if (r->TRANSCODE.CmpNoCase("Withdrawal") == 0)
            return WITHDRAWAL;
        else if (r->TRANSCODE.CmpNoCase("Deposit") == 0)
            return DEPOSIT;
        else
            return TRANSFER;
    }
    static TYPE type(const Data& r) { return type(&r); }
    static double balance(const Data* r, int account_id = -1)
    {
        double sum = 0;
        switch (type(r))
        {
        case WITHDRAWAL:
            sum -= r->TRANSAMOUNT;
            break;
        case DEPOSIT:
            sum += r->TRANSAMOUNT;
            break;
        case TRANSFER:
            if (account_id == r->ACCOUNTID)     sum -= r->TRANSAMOUNT;
            if (account_id == r->TOACCOUNTID)   sum += r->TRANSAMOUNT;
            break;
        default:
            break;
        }
        return sum;
    }
    static double balance(const Data& r, int account_id = -1) { return balance(&r, account_id); }
};

#endif // 

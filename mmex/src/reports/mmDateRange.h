/*
    Author: Lisheng Guan (guanlisheng@gmail.com) 
*/
#ifndef _MM_EX_DATE_RANGE_H_
#define _MM_EX_DATE_RANGE_H_

#include <wx/datetime.h>

class mmDateRange
{
public:
    mmDateRange(): now_(wxDateTime::Now().GetDateOnly())
    {
        start_date_ = now_;
        end_date_ = now_;
    }
protected:
    wxDateTime now_, start_date_, end_date_;

public:
    const virtual wxDateTime start_date() const  { return this->start_date_; };
    const virtual wxDateTime end_date() const  { return this->end_date_; };
};

class mmCurrentMonth: public mmDateRange
{
public:
    mmCurrentMonth(): mmDateRange()
    {
        this->start_date_ = now_.SetDay(1);
        this->end_date_ = now_.GetLastMonthDay();
    }
};

class mmCurrentMonthToDate: public mmDateRange
{
public:
    mmCurrentMonthToDate(): mmDateRange()
    {
        this->start_date_ = now_.SetDay(1);
        // no change to end_date_
    }
};

class mmLastMonth: public mmDateRange
{
public:
    mmLastMonth(): mmDateRange()
    {
        this->start_date_.Subtract(wxDateSpan::Months(1)).SetDay(1);
        this->end_date_ = wxDateTime(this->start_date_).GetLastMonthDay();
    }
};

class mmLast30Days: public mmDateRange
{
public:
    mmLast30Days(): mmDateRange()
    {
        this->start_date_ = wxDateTime(end_date_).Subtract(wxDateSpan::Months(1)).Add(wxDateSpan::Days(1));
        // no change to end_date_
    }
};

class mmCurrentYear: public mmDateRange
{
public:
    mmCurrentYear(): mmDateRange()
    {
        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        this->end_date_ = wxDateTime(start_date_).SetMonth(wxDateTime::Dec).SetDay(31);  
    }
};

class mmCurrentYearToDate: public mmDateRange
{
public:
    mmCurrentYearToDate(): mmDateRange()
    {
        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        // no change to end_date_
    }
};

class mmLastYear: public mmDateRange
{
public:
    mmLastYear(): mmDateRange()
    {
        this->start_date_.Subtract(wxDateSpan::Years(1))
            .SetDay(1).SetMonth(wxDateTime::Jan);
        this->end_date_ = wxDateTime(start_date_).SetMonth(wxDateTime::Dec).SetDay(31);
    }
};

class mmCurrentFinancialYear: public mmDateRange
{
public:
    mmCurrentFinancialYear(): mmDateRange()
    {
        // TODO
        int day = 1; //core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_DAY"), 1);
        int monthItem = 7; //core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_MONTH"), 7);

        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        this->end_date_ = wxDateTime(start_date_).SetMonth(wxDateTime::Dec).SetDay(31);  
        this->start_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));
        this->end_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1));

        if (now_ < start_date_)
        {
            start_date_.Subtract(wxDateSpan::Years(1));
            end_date_.Subtract(wxDateSpan::Years(1));
        }
    }
};

class mmLastFinancialYear: public mmDateRange
{
public:
    mmLastFinancialYear(): mmDateRange()
    {
        //TODO:
        int day = 1; //core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_DAY"), 1);
        int monthItem = 7; //core_->dbInfoSettings_->GetIntSetting(wxT("FINANCIAL_YEAR_START_MONTH"), 7);

        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        this->end_date_ = wxDateTime(start_date_).SetMonth(wxDateTime::Dec).SetDay(31);  
        start_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1)).Subtract(wxDateSpan::Years(1));
        end_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(monthItem-1)).Subtract(wxDateSpan::Years(1));

        if (now_ >= start_date_)
        {
            this->start_date_.Subtract(wxDateSpan::Years(1));
            this->end_date_.Subtract(wxDateSpan::Years(1));
        }
    }
};

#endif // _MM_EX_DATE_RANGE_H_

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

class mmCurrentYear: public mmDateRange
{
public:
    mmCurrentYear(): mmDateRange()
    {
        // TODO
    }
};

class mmCurrentYearToDate: public mmDateRange
{
public:
    mmCurrentYearToDate(): mmDateRange()
    {
        // TODO
    }
};

class mmLastYear: public mmDateRange
{
public:
    mmLastYear(): mmDateRange()
    {
        // TODO
    }
};

class mmCurrentFinancialYear: public mmDateRange
{
public:
    mmCurrentFinancialYear(): mmDateRange()
    {
        // TODO
    }
};

class mmLastFinancialYear: public mmDateRange
{
public:
    mmLastFinancialYear(): mmDateRange()
    {
        // TODO
    }
};

class mmLast30Days: public mmDateRange
{
public:
    mmLast30Days(): mmDateRange()
    {
        // TODO
    }
};

#endif // _MM_EX_DATE_RANGE_H_

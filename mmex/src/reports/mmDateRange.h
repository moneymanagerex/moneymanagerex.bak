/*
    Author: Lisheng Guan (guanlisheng@gmail.com) 
*/
#ifndef _MM_EX_DATE_RANGE_H_
#define _MM_EX_DATE_RANGE_H_

#include <wx/datetime.h>

class mmDateRange
{
public:
    mmDateRange(): now_(wxDateTime::Now())
    {
        start_date_ = now_;
        end_date_ = now_;
    }
public:
    wxDateTime now_, start_date_, end_date_;
};

class mmCurrentMonth: public mmDateRange
{
public:
    mmCurrentMonth(): mmDateRange()
    {}
};

class mmLastMonth: public mmDateRange
{
public:
    mmLastMonth(): mmDateRange()
    {}
};

class mmCurrentYear: public mmDateRange
{
public:
    mmCurrentYear(): mmDateRange()
    {
        this->start_date_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->end_date_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmCurrentYearToDate: public mmDateRange
{
public:
    mmCurrentYearToDate(): mmDateRange()
    {
        this->start_date_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->end_date_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmLastYear: public mmDateRange
{
public:
    mmLastYear(): mmDateRange()
    {}
};

class mmCurrentFinancialYear: public mmDateRange
{
public:
    mmCurrentFinancialYear(): mmDateRange()
    {
        this->start_date_ = wxDateTime::Now().Subtract(wxDateSpan::Days(wxDateTime::Now().GetDay() - 1));
        this->end_date_ = wxDateTime::Now().GetLastMonthDay();
    }
};

class mmLastFinancialYear: public mmDateRange
{
public:
    mmLastFinancialYear(): mmDateRange()
    {}
};

class mmLast30Days: public mmDateRange
{
public:
    mmLast30Days(): mmDateRange()
    {}
};

#endif // _MM_EX_DATE_RANGE_H_

/*
    Author: Lisheng Guan (guanlisheng@gmail.com) 
*/
#include <wx/datetime.h>

class mmDateRange
{
public:
    mmDateRange(): start_date_(wxDateTime::Now()), end_date_(wxDateTime::Now())
    {}
protected:
    wxDateTime start_date_, end_date_;
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


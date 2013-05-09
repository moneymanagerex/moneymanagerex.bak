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
        title_ = "Date Range";
    }
protected:
    wxDateTime now_, start_date_, end_date_;
    wxString title_;

public:
    const virtual wxDateTime start_date() const  { return this->start_date_; };
    const virtual wxDateTime end_date() const  { return this->end_date_; };
    const virtual bool is_with_date() const { return true; }
    const virtual wxString title() const { return title_;};
};

class mmCurrentMonth: public mmDateRange
{
public:
    mmCurrentMonth(): mmDateRange()
    {
        this->start_date_ = now_.SetDay(1);
        this->end_date_ = now_.GetLastMonthDay();
        this->title_ = _("Current Month");
    }
};

class mmCurrentMonthToDate: public mmDateRange
{
public:
    mmCurrentMonthToDate(): mmDateRange()
    {
        this->start_date_ = now_.SetDay(1);
        // no change to end_date_
        this->title_ = _("Current Month to Date");
    }
};

class mmLastMonth: public mmDateRange
{
public:
    mmLastMonth(): mmDateRange()
    {
        this->start_date_.Subtract(wxDateSpan::Months(1)).SetDay(1);
        this->end_date_ = wxDateTime(this->start_date_).GetLastMonthDay();
        this->title_ = _("Last Month");
    }
};

class mmLast30Days: public mmDateRange
{
public:
    mmLast30Days(): mmDateRange()
    {
        this->start_date_ = wxDateTime(end_date_).Subtract(wxDateSpan::Months(1)).Add(wxDateSpan::Days(1));
        // no change to end_date_
        this->title_ = _("Last 30 Days");
    }
};

class mmLast12Months: public mmDateRange
{
public:
    mmLast12Months(): mmDateRange()
    {
        this->start_date_ = wxDateTime(end_date_).SetDay(1).Add(wxDateSpan::Months(1)).Subtract(wxDateSpan::Years(1));
        // no change to end_date_
        this->title_ = _("Last 12 Months");
    }
};

class mmCurrentYear: public mmDateRange
{
public:
    mmCurrentYear(): mmDateRange()
    {
        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        this->end_date_ = wxDateTime(start_date_).SetMonth(wxDateTime::Dec).SetDay(31);
        this->title_ = _("Current Year");
    }
};

class mmCurrentYearToDate: public mmDateRange
{
public:
    mmCurrentYearToDate(): mmDateRange()
    {
        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        // no change to end_date_
        this->title_ = _("Current Year to Date");
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
        this->title_ = _("Last Year");
    }
};

class mmCurrentFinancialYear: public mmDateRange
{
public:
    mmCurrentFinancialYear(int day, int month): mmDateRange()
    {
        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        this->end_date_ = wxDateTime(start_date_).SetMonth(wxDateTime::Dec).SetDay(31);
        this->start_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(month-1));
        this->end_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(month-1));

        if (now_ < start_date_)
        {
            start_date_.Subtract(wxDateSpan::Years(1));
            end_date_.Subtract(wxDateSpan::Years(1));
        }
        this->title_ = _("Current Financial Year");
    }
};

class mmLastFinancialYear: public mmDateRange
{
public:
    mmLastFinancialYear(int day, int month): mmDateRange()
    {
        this->start_date_.SetDay(1).SetMonth(wxDateTime::Jan);
        this->end_date_ = wxDateTime(start_date_).SetMonth(wxDateTime::Dec).SetDay(31);
        start_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(month-1)).Subtract(wxDateSpan::Years(1));
        end_date_.Add(wxDateSpan::Days(day-1)).Add(wxDateSpan::Months(month-1)).Subtract(wxDateSpan::Years(1));

        if (now_ >= start_date_)
        {
            this->start_date_.Subtract(wxDateSpan::Years(1));
            this->end_date_.Subtract(wxDateSpan::Years(1));
        }
        this->title_ = _("Last Financial Year");
    }
};

class mmAllTime: public mmDateRange
{
public:
    mmAllTime(): mmDateRange()
    {
        this->title_ = _("Over Time");
        this->start_date_.Subtract(now_);
    }
    const bool is_with_date() const { return false; }
};

#endif // _MM_EX_DATE_RANGE_H_

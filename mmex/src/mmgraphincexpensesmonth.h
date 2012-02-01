#ifndef _MM_EX_GRAPHINCEXPENSESMONTH_H_
#define _MM_EX_GRAPHINCEXPENSESMONTH_H_

#include "mmgraphgenerator.h"
#include <boost/scoped_ptr.hpp>

class BarChart;

class mmGraphIncExpensesMonth : public mmGraphGenerator
{
public:
    mmGraphIncExpensesMonth();
   ~mmGraphIncExpensesMonth();

    void init(double income, double expenses);
    bool Generate(const wxString& chartTitle);

private:
	boost::scoped_ptr<BarChart> chart;
};
#endif // _MM_EX_GRAPHINCEXPENSESMONTH_H_

#include "mmgraphgenerator.h"
#include "chart.h"


#ifndef _MM_EX_GRAPHINCEXPENSESMONTH_H_
#define _MM_EX_GRAPHINCEXPENSESMONTH_H_

class mmGraphIncExpensesMonth : public mmGraphGenerator
{
public:
    mmGraphIncExpensesMonth() : mmGraphGenerator(wxT("inc_expenses_month.png"))
    {
		chart = new BarChart(200, 240);
    }

    ~mmGraphIncExpensesMonth()
    {
		delete chart;
    }

    virtual void init(double income, double expenses)
    {
		std::vector<ChartData> barData;

		barData.push_back(ChartData(_("Income"), income));
		barData.push_back(ChartData(_("Expenses"), expenses));

		chart->SetData(barData);
		chart->Init(140, CHART_LEGEND_FLOAT, BAR_CHART_SIMPLE);
    }

	bool Generate(const wxString& chartTitle)
	{
		chart->Render(chartTitle);
		return chart->Save(GetOutputFileName());
	};

private:
	BarChart* chart;
};

#endif

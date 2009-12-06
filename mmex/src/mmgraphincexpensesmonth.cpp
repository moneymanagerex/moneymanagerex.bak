#include "mmgraphincexpensesmonth.h"
#include "chart.h"
#include <vector>

mmGraphIncExpensesMonth::mmGraphIncExpensesMonth() : 
        mmGraphGenerator(EXPENSES_MONTHS),
        chart(new BarChart(200, 240))
{
}

mmGraphIncExpensesMonth::~mmGraphIncExpensesMonth()
{
}

void mmGraphIncExpensesMonth::init(double income, double expenses)
{
        std::vector<ChartData> barData;

	barData.push_back(ChartData(_("Income"), income));
	barData.push_back(ChartData(_("Expenses"), expenses));

	chart->SetData(barData);
	chart->Init(140, CHART_LEGEND_FLOAT, BAR_CHART_SIMPLE);
}

bool mmGraphIncExpensesMonth::Generate(const wxString& chartTitle)
{
        chart->Render(chartTitle);
	return chart->Save(getOutputFileName());
}

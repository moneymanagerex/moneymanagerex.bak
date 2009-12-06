#include "mmgraphpie.h"
#include "chart.h"
#include "util.h"

namespace
{

bool sortValueList(const ValuePair& elem1, const ValuePair& elem2)
{
    return fabs(elem1.amount) > fabs(elem2.amount);
}

} // namespace


mmGraphPie::mmGraphPie() : 
        mmGraphGenerator(PIE),
        chart(new PieChart(480, 320))
{
}

mmGraphPie::~mmGraphPie()
{
}

void mmGraphPie::init(std::vector<ValuePair>& valueList)
{
        std::vector<ChartData> pieData;
        std::sort(valueList.begin(), valueList.end(), sortValueList);

	for (size_t i = 0; i < valueList.size(); ++i) {
                if (i < 15)
	              pieData.push_back(ChartData(valueList[i].label, valueList[i].amount));
	}

	chart->SetData(pieData);
	chart->Init(240, CHART_LEGEND_FIXED, PIE_CHART_ABSOLUT);
}

bool mmGraphPie::Generate(const wxString& chartTitle)
{
        chart->Render(chartTitle);
	return chart->Save(getOutputFileName());
}

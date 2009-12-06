#include "mmgraphpie.h"
#include "chart.h"
#include "util.h"

#include <algorithm>

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
        std::sort(valueList.begin(), valueList.end(), sortValueList);
        size_t cnt = std::min(valueList.size(), size_t(15));

        std::vector<ChartData> pieData;
        pieData.reserve(cnt);

	for (size_t i = 0; i < cnt; ++i) {
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

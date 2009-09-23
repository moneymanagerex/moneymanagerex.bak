//----------------------------------------------------------------------------
#include "mmgraphtopcategories.h"
#include "chart.h"
#include "mmex.h"
//----------------------------------------------------------------------------

mmGraphTopCategories::mmGraphTopCategories() : 
    mmGraphGenerator(wxT("top_categories.png")),
	m_chart(new BarChart(320, 240))
{
}
//----------------------------------------------------------------------------

void mmGraphTopCategories::init(const std::vector<CategInfo> &categList)
{
	std::vector<ChartData> barData;
	barData.reserve(categList.size());

	for (size_t i = 0; i < categList.size(); ++i)
	{
        barData.push_back(ChartData(categList[i].categ, categList[i].amount));
	}

    m_chart->SetData(barData);
	m_chart->Init(140, CHART_LEGEND_FIXED, BAR_CHART_SIMPLE);
}
//----------------------------------------------------------------------------

bool mmGraphTopCategories::Generate(const wxString& chartTitle)
{
	m_chart->Render(chartTitle);
	return m_chart->Save(GetOutputFileName());
}
//----------------------------------------------------------------------------

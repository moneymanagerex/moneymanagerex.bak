//----------------------------------------------------------------------------
#include "mmgraphtopcategories.h"
#include "chart.h"
#include "mmex.h"
//----------------------------------------------------------------------------

mmGraphTopCategories::mmGraphTopCategories() : 
        mmGraphGenerator(TOP_CATEG),
        m_chart(new BarChart(320, 240))
{
}
//----------------------------------------------------------------------------

mmGraphTopCategories::~mmGraphTopCategories()
{
}
//----------------------------------------------------------------------------

void mmGraphTopCategories::init(const std::vector<CategInfo> &categList)
{
	std::vector<ChartData> barData;
	barData.reserve(categList.size());

	for (size_t i = 0; i < categList.size(); ++i) 
	{
		if (i >= 10) //Show top 10 or less
            break;
		if (categList[i].amount < 0.0 ) //Show withdrawal only
		{
        barData.push_back(ChartData(categList[i].categ, -1*(categList[i].amount)));
		}
	}

    m_chart->SetData(barData);
	m_chart->Init(140, CHART_LEGEND_FIXED, BAR_CHART_SIMPLE);
}
//----------------------------------------------------------------------------

bool mmGraphTopCategories::Generate(const wxString& chartTitle)
{
	m_chart->Render(chartTitle);
	return m_chart->Save(getOutputFileName());
}
//----------------------------------------------------------------------------

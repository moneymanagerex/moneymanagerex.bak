#include "mmgraphgenerator.h"
#include "chart.h"

#ifndef _MM_EX_GRAPHTOPCATEGORIES_H_
#define _MM_EX_GRAPHTOPCATEGORIES_H_

class mmGraphTopCategories : public mmGraphGenerator
{
public:
    mmGraphTopCategories() : mmGraphGenerator(wxT("top_categories.png"))
    {
		chart = new BarChart(320, 240);
    }

    ~mmGraphTopCategories()
    {
		delete chart;
    };

    virtual void init(std::vector<mmGUIFrame::CategInfo>& categList)
    {
		std::vector<ChartData> barData;

        for (unsigned int i = 0; i < categList.size(); i++)
        {
			barData.push_back(ChartData(categList[i].categ, categList[i].amount));
        }

		chart->SetData(barData);
		chart->Init(140, CHART_LEGEND_FIXED, BAR_CHART_SIMPLE);
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

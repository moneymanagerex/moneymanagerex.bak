#include "mmgraphgenerator.h"
#include "chart.h"
#include "mmex.h"

#ifndef _MM_EX_GRAPHPIE_H_
#define _MM_EX_GRAPHPIE_H_

bool sortValueList(ValuePair& elem1, ValuePair& elem2)
{
    return fabs(elem1.amount) > fabs(elem2.amount);
}

class mmGraphPie : public mmGraphGenerator
{
public:
    mmGraphPie() : mmGraphGenerator(wxT("pie_chart.png"))
    {
		chart = new PieChart(480, 320);
    };

    ~mmGraphPie()
    {
		delete chart;
    };

    virtual void init(std::vector<ValuePair>& valueList)
    {
		std::vector<ChartData> pieData;
        std::sort(valueList.begin(), valueList.end(), sortValueList);

		for(unsigned int i=0; i<valueList.size(); i++)
		{
            if (i < 15)
			    pieData.push_back(ChartData(valueList[i].label, valueList[i].amount));
		}

		chart->SetData(pieData);
		chart->Init(240, CHART_LEGEND_FIXED, PIE_CHART_ABSOLUT);
    }

	bool Generate(const wxString& chartTitle)
	{
		chart->Render(chartTitle);
		return chart->Save(GetOutputFileName());
	};

private:
	PieChart* chart;
};

#endif

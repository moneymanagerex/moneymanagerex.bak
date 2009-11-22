#ifndef _MM_EX_GRAPHPIE_H_
#define _MM_EX_GRAPHPIE_H_

#include "mmgraphgenerator.h"

#include <boost/scoped_ptr.hpp>
#include <vector>

struct ValuePair;


class PieChart;

class mmGraphPie : public mmGraphGenerator
{
public:
    mmGraphPie();
   ~mmGraphPie();

    void init(std::vector<ValuePair>& valueList);
    bool Generate(const wxString& chartTitle);

private:
	boost::scoped_ptr<PieChart> chart;
};

#endif // _MM_EX_GRAPHPIE_H_


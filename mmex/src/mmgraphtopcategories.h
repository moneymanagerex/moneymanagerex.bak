//----------------------------------------------------------------------------
#ifndef _MM_EX_GRAPHTOPCATEGORIES_H_
#define _MM_EX_GRAPHTOPCATEGORIES_H_
//----------------------------------------------------------------------------
#include "mmgraphgenerator.h"
//----------------------------------------------------------------------------
#include <boost/scoped_ptr.hpp>
//----------------------------------------------------------------------------
class BarChart;
struct CategInfo;
//----------------------------------------------------------------------------

class mmGraphTopCategories : public mmGraphGenerator
{
public:
	mmGraphTopCategories();

	void init(const std::vector<CategInfo> &categList);
	bool Generate(const wxString& chartTitle);

private:
    boost::scoped_ptr<BarChart> m_chart;
};
//----------------------------------------------------------------------------
#endif // _MM_EX_GRAPHTOPCATEGORIES_H_
//----------------------------------------------------------------------------

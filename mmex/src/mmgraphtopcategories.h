/*******************************************************
 Copyright (C) 2006-2012 

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

//----------------------------------------------------------------------------
#ifndef _MM_EX_GRAPHTOPCATEGORIES_H_
#define _MM_EX_GRAPHTOPCATEGORIES_H_
//----------------------------------------------------------------------------
#include "mmgraphgenerator.h"
//----------------------------------------------------------------------------
#include <vector>
#include <wx/scopedptr.h>
//----------------------------------------------------------------------------
class BarChart;
struct CategInfo;
//----------------------------------------------------------------------------

class mmGraphTopCategories : public mmGraphGenerator
{
public:
    mmGraphTopCategories();
    ~mmGraphTopCategories();

    void init(const std::vector<CategInfo> &categList);
    bool Generate(const wxString& chartTitle);

private:
    wxScopedPtr<BarChart> m_chart;
};
//----------------------------------------------------------------------------
#endif // _MM_EX_GRAPHTOPCATEGORIES_H_
//----------------------------------------------------------------------------

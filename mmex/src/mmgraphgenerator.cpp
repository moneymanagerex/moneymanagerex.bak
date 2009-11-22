/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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

#include "mmgraphgenerator.h"
//----------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include <wx/filename.h>
//----------------------------------------------------------------------------

namespace 
{

class Deleter
{
public:
        Deleter() : files(64) {}
       ~Deleter();

        void add(const wxString &f) { files.push_back(f); }
        void delete_all();

private:
        std::vector<wxString> files;
};
//----------------------------------------------------------------------------
Deleter g_deleter;
//----------------------------------------------------------------------------

Deleter::~Deleter()
{
        try {
                delete_all();
        } catch (...) {
                wxASSERT(false);
        }
}
//----------------------------------------------------------------------------

void Deleter::delete_all()
{
        for_each(files.begin(), files.end(), wxRemoveFile);
}
//----------------------------------------------------------------------------

} // namespace 

//----------------------------------------------------------------------------

/*
        FIXME: do not create temp files in public temporary directory.
*/
mmGraphGenerator::mmGraphGenerator() :
        m_htmlString(wxFileName::CreateTempFileName(wxGetEmptyString()))
{
        g_deleter.add(m_htmlString);
}
//----------------------------------------------------------------------------

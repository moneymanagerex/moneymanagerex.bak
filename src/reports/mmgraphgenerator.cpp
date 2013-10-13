/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2009 VaDiM

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
#include "util.h"
#include "singleton.h"
#include <wx/fs_mem.h>
//----------------------------------------------------------------------------

class Eraser
{
public:
    Eraser();
    ~Eraser();
    void cleanup();

    wxString getFilePath();
    static Eraser& instance();

private:
    unsigned long count;
    std::vector<wxString> files;
};
//----------------------------------------------------------------------------

Eraser::Eraser()
    : count(0)
{
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
}

Eraser::~Eraser()
{
}

void Eraser::cleanup()
{
    for(auto& f: files)
        wxMemoryFSHandler::RemoveFile(f);
    files.clear();
}

//----------------------------------------------------------------------------

wxString Eraser::getFilePath()
{
    // limit memory growth size
    if(files.size() > 3) // max number of simultaneous graphs
    {
        wxMemoryFSHandler::RemoveFile(files[0]);
        files.erase(files.begin());
    }

    wxString path = wxString::Format("chart%d.png", ++count);
    files.push_back(path);
    return path.Prepend("memory:");
}
//----------------------------------------------------------------------------

Eraser& Eraser::instance()
{
    return Singleton<Eraser>::instance();
}
//----------------------------------------------------------------------------

mmGraphGenerator::mmGraphGenerator() :
    m_path(Eraser::instance().getFilePath())
{
}

void mmGraphGenerator::cleanup()
{
    Eraser::instance().cleanup();
}
//----------------------------------------------------------------------------

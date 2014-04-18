/*************************************************************************
 Copyright (C) 2011,2012 Stefano Giorgio
 Copyright (C) 2014 Guan Lisheng (guanlisheng@gmail.com)

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
 *************************************************************************/
#ifndef _MM_EX_RECENTFILES_H_
#define _MM_EX_RECENTFILES_H_
#include <wx/arrstr.h>
#include "defs.h"
#include <wx/filehistory.h>

class mmFileHistory: public wxFileHistory
{
public:
    mmFileHistory(size_t maxFiles=9, wxWindowID idBase=wxID_FILE1);
    ~mmFileHistory();

public:
    void Clear();
    void Load();
    void Save();
};

#endif


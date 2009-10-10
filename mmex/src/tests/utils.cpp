/*******************************************************
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

//----------------------------------------------------------------------------
#include "utils.h"
//----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
//----------------------------------------------------------------------------
extern const char* g_exepath; // defined in Main.cpp
//----------------------------------------------------------------------------

namespace
{

const size_t rc_err = static_cast<size_t>(-1);

} // namespace

//----------------------------------------------------------------------------

std::wstring utils::asWString(const char *str)
{
    std::wstring ws;

    size_t len = strlen(str);
    size_t cnt = mbstowcs(0, str, len);
    
    if (cnt != rc_err) 
    {
        wchar_t *buf = new wchar_t[cnt];
        cnt = mbstowcs(buf, str, len);
        ws.assign(buf, cnt);
        delete []buf;
    }

    return ws;
}
//----------------------------------------------------------------------------

std::string utils::asString(const wchar_t *str)
{
    std::string mbs;

    size_t len = wcslen(str);
    size_t cnt = wcstombs(0, str, len);

    if (cnt != rc_err) 
    {
        char *buf = new char[cnt];
        cnt = wcstombs(buf, str, len);
        mbs.assign(buf, cnt);
        delete []buf;
    }

    return mbs;
}
//----------------------------------------------------------------------------

const std::wstring& utils::getExePath()
{
    static std::wstring path;

    if (path.empty()) 
    {
        path = asWString(g_exepath);
        std::string::size_type pos = path.find_last_of(DirSep);
        
        if (pos != std::string::npos)
        {
            path.erase(pos);
        }
    }

    return path;
}
//----------------------------------------------------------------------------

const std::wstring& utils::getTempDir()
{
    static std::wstring path;

    if (path.empty()) 
    {
        if (const char *dir = getenv("TMP")) {
            path = asWString(dir);
        } else if (const char *dir = getenv("TEMP")) {
            path = asWString(dir);
        } else {
            path = L".";
        }
    }

    return path;
}
//----------------------------------------------------------------------------

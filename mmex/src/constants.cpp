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

#include "constants.h"
#include <wx/string.h>
#include <wx/filefn.h>
#include "paths.h"
#include "defs.h"
#include <boost/version.hpp>

//----------------------------------------------------------------------------

wxString mmex::getProgramName()
{
    return wxString(wxT("MoneyManagerEx"));
}

wxString mmex::getProgramVersion()
{
    /******************************************************
     The commented line has been left to act as a template
     for when a development version is created for release.
     ******************************************************/
    //return wxString(wxT("0.9.9.0 DEV:SVN-xxxx"));
    return wxT("0.10.0.0");
}

wxString mmex::getProgramCopyright()
{
    return wxT("(c) 2005-2012 Madhan Kanagavel");
}

wxString mmex::getProgramWebSite()
{
    return wxT("http://codelathe.com/mmex\n");
}

wxString mmex::getProgramDescription()
{
    wxString description = _("wxWidgets-based application!");
    description << wxT("\n\n") << _("System: ")
                << wxPlatformInfo::Get().GetOperatingSystemIdName()
                << wxT("\n\n")
                << _("Released under the GNU GPL License")
                << wxT("\n\n")
                << wxVERSION_STRING
                << wxT(",  Boost C++ ") << (BOOST_VERSION/100000) << wxT('.')
                << (BOOST_VERSION / 100 % 1000) << wxT('.')
                << (BOOST_VERSION % 100) << wxT(",\n")
                << wxT("SQLite3 ") << wxSQLite3Database::GetVersion()
                << wxT(",  wxSQLite by Ulrich Telle")
                << wxT("\n\n");
    return description;
}

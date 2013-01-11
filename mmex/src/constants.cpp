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
#include <boost/version.hpp>

/*************************************************************************
 Revision of last commit: $Revision$
 Author of last commit:   $Author$

 When creating a build:
 Update the MMEX_REVISION_ID string to the latest SVN Revision first,
 OR
 Commit a change to the SVN repository, then create the build.
 *************************************************************************/
const wxString MMEX_REVISION_ID = wxT("$Rev$");
//----------------------------------------------------------------------------
const int mmex::MIN_DATAVERSION = 2;
const wxChar * const mmex::DATAVERSION = wxT("2");
const wxChar *const mmex::DEFDATEFORMAT = wxT("%m/%d/%y");
const wxChar *const mmex::DEFDELIMTER = wxT(",");

// Using google: To specify the exchange, use exch:code 
// Using yahoo: To specify the exchange, use code.exch 

// const wxChar *const mmex::DEFSTOCKURL = wxT("http://www.google.com/finance?q=%s");

// Will display the stock page when using Looks up the current value
const wxChar *const mmex::DEFSTOCKURL = wxT("http://finance.yahoo.com/echarts?s=%s");

// Looks up the current value
// const wxChar *const mmex::DEFSTOCKURL = wxT("http://finance.yahoo.com/lookup?s=%s");

//US Dollar (USD) in Euro (EUR) Chart
//http://www.google.com/finance?q=CURRENCY%3AUSD

//----------------------------------------------------------------------------

wxString mmex::getProgramName()
{
    return wxString(wxT("MoneyManagerEx"));
}
//----------------------------------------------------------------------------

wxString mmex::getProgramVersion()
{
    wxString revision(MMEX_REVISION_ID);
    revision.Replace(wxT("$"), wxEmptyString);

/**************************************************
 Refer to comments in the file: constants.h
 **************************************************/
#ifndef _MM_EX_BUILD_TYPE_RELEASE
    revision.Replace(wxT("Rev: "), wxT("DEV:SVN-"));
#endif

    revision.Trim();
    return wxString::Format(wxT("0.9.9.2  %s"), revision.c_str());
}
//----------------------------------------------------------------------------
wxString mmex::getProgramCopyright()
{
    return wxT("(c) 2005-2012 Madhan Kanagavel");
}

wxString mmex::getProgramWebSite()
{
    return wxT("http://codelathe.com/mmex");
}

wxString mmex::getProgramFacebookSite()
{
    return wxT("http://www.facebook.com/pages/Money-Manager-Ex/242286559144586");
}

wxString mmex::getProgramDescription()
{
    wxString description;
    description << _("MMEX is using the following support products") << wxT(":\n")
                << wxT("======================================\n")
                << wxVERSION_STRING << wxT("\n")
                << wxT("Boost C++ ") << (BOOST_VERSION/100000) << wxT('.')
                << (BOOST_VERSION / 100 % 1000) << wxT('.')
                << (BOOST_VERSION % 100) << wxT("\n")
                << wxT("SQLite3 ") << wxSQLite3Database::GetVersion() << wxT("\n")
                << wxT("wxSQLite 3.0.1")<< wxT("\n")
                << wxT("Lua 5.2.1");
    return description;
}


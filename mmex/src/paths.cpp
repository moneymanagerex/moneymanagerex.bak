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
#include "paths.h"
#include "platfdep.h"
//----------------------------------------------------------------------------
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/icon.h>
//----------------------------------------------------------------------------

/*
        This routine is platform-independent.
*/
wxFileName mmex::GetUserDir(bool create)
{
        static wxFileName fname(wxFileName::DirName(wxStandardPaths::Get().GetUserDataDir()));

        if (create && !fname.DirExists()) {
                bool ok = fname.Mkdir(777, wxPATH_MKDIR_FULL);
                wxASSERT(ok);
        }
        
        return fname;
}
//----------------------------------------------------------------------------

wxFileName mmex::GetLogDir(bool create)
{
        static wxFileName fname;

        if (!fname.IsOk()) {
                fname = GetUserDir(create);
                fname.AppendDir(wxT("logs"));
        }
        
        return fname;
}
//----------------------------------------------------------------------------

wxString mmex::getPathDoc(EDocFile f)
{
        static const wxChar* files[DOC_FILES_MAX] = { 
          wxT("README.TXT"),
          wxT("contrib.txt"),
          wxT("license.txt"),
          wxT("version.txt"),
          wxT("help/index.html")
        };

        wxASSERT(sizeof(files)/sizeof(*files) == DOC_FILES_MAX);
        wxASSERT(f >= 0 && f < DOC_FILES_MAX);

        wxString path = GetDocDir().GetPath();
        path += wxFileName::GetPathSeparator();
        path += files[f];

        return path;
}
//----------------------------------------------------------------------------

wxString mmex::getPathResource(EResFile f)
{
        static const wxChar* files[RES_FILES_MAX] = { 
          wxT("mmex.ico"),
          wxT("splash.png"),
          wxT("kaching.wav")
        };

        wxASSERT(sizeof(files)/sizeof(*files) == RES_FILES_MAX);
        wxASSERT(f >= 0 && f < RES_FILES_MAX);

        wxFileName fname = GetResourceDir();
        fname.SetFullName(files[f]);

        return fname.GetFullPath();
}
//----------------------------------------------------------------------------

wxString mmex::getPathShared(ESharedFile f)
{
        static const wxChar* files[SHARED_FILES_MAX] = { 
          wxT("currency_seed.db3"),
          wxT("i18n")
        };

        wxASSERT(sizeof(files)/sizeof(*files) == SHARED_FILES_MAX);
        wxASSERT(f >= 0 && f < SHARED_FILES_MAX);

        wxFileName fname = GetSharedDir();
        fname.SetFullName(files[f]);

        return fname.GetFullPath();
}
//----------------------------------------------------------------------------

wxString mmex::getPathUser(EUserFile f)
{
        static const wxChar* files[USER_FILES_MAX] = { 
          wxT("mmexini.db3")
        };

        wxASSERT(sizeof(files)/sizeof(*files) == USER_FILES_MAX);
        wxASSERT(f >= 0 && f < USER_FILES_MAX);

        wxFileName fname = GetUserDir(true);
        fname.SetFullName(files[f]);

        return fname.GetFullPath();
}
//----------------------------------------------------------------------------

const wxIcon& mmex::getProgramIcon()
{
        static wxIcon icon(getPathResource(PROGRAM_ICON), wxBITMAP_TYPE_ICO, 32, 32);
        return icon;
}
//----------------------------------------------------------------------------

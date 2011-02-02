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

namespace 
{

inline const wxChar* getSettingsFileName()
{
        return wxT("mmexini.db3");
}
//----------------------------------------------------------------------------

wxFileName getSettingsPathPortable()
{
        wxFileName f = mmex::GetSharedDir();
        f.SetFullName(getSettingsFileName());

        return f;
}

} // namespace 

//----------------------------------------------------------------------------

/*
        This routine is platform-independent.

        MMEX is a portable application which means ability to to run
        without installation, for example, from USB flash drive.

        If mmex finds mmexini.db3 in its folder, it assumes portable 
        mode and GetUserDir() in such case points to that folder.

        FIXME: security issue - temp files will be created on host filesystem.
*/
wxFileName mmex::GetUserDir(bool create)
{
        static wxFileName fname;

        if (!fname.IsOk()) {
                
                fname = getSettingsPathPortable();

                bool ok_portable = fname.IsFileWritable() && fname.IsFileReadable();

                if (!ok_portable) {
                
                        fname.AssignDir(wxStandardPaths::Get().GetUserDataDir());

                        if (create && !fname.DirExists()) {
                                bool ok = fname.Mkdir(0700, wxPATH_MKDIR_FULL); // 0700 - octal, "111 000 000"
                                wxASSERT(ok);
                        }
                }

                fname.SetFullName(wxGetEmptyString());
        }

        return fname;
}
//----------------------------------------------------------------------------

wxFileName mmex::GetLogDir(bool create)
{
        static wxFileName fname;

        if (!fname.IsOk()) {
                fname = GetUserDir(create);
                //FIXME: file not found ERROR
                //fname.AppendDir(wxT("logs"));
        }
        
        return fname;
}
//----------------------------------------------------------------------------

bool mmex::isPortableMode()
{
        wxFileName f = getSettingsPathPortable();
        return f.GetFullPath() == getPathUser(SETTINGS);
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

        wxASSERT(f >= 0 && f < DOC_FILES_MAX);
        wxASSERT(files[f]);

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
          wxT("kaching.wav"),
          wxT("currency_seed.db3")
        };

        wxASSERT(f >= 0 && f < RES_FILES_MAX);
        wxASSERT(files[f]);

        wxFileName fname = GetResourceDir();
        fname.SetFullName(files[f]);

        return fname.GetFullPath();
}
//----------------------------------------------------------------------------

wxString mmex::getPathShared(ESharedFile f)
{
        static const wxChar* files[SHARED_FILES_MAX] = { 
          wxT("po")
        };

        wxASSERT(f >= 0 && f < SHARED_FILES_MAX);
        wxASSERT(files[f]);

        wxFileName fname = GetSharedDir();
        fname.SetFullName(files[f]);

        return fname.GetFullPath();
}
//----------------------------------------------------------------------------

wxString mmex::getPathUser(EUserFile f)
{
        static const wxChar* files[USER_FILES_MAX] = { 
          getSettingsFileName()
        };

        wxASSERT(f >= 0 && f < USER_FILES_MAX);
        wxASSERT(files[f]);

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

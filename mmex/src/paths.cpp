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
                bool ok = fname.Mkdir(0700, wxPATH_MKDIR_FULL);
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
          wxT("i18n")
        };

        wxASSERT(f >= 0 && f < SHARED_FILES_MAX);
        wxASSERT(files[f]);

        wxFileName fname = GetSharedDir();
        fname.SetFullName(files[f]);

        return fname.GetFullPath();
}
//----------------------------------------------------------------------------

/*
        MMEX is a portable application which means ability to to run
        without installation, for example, from USB flash drive.
        
        To make MMEX portable

        1.On Windows (assume F:\ is USB flash drive)
          copy "C:\Program Files\MoneyManagerEx" F:\
          copy %APPDATA%\MoneyManagerEx\mmexini.db3 F:\MoneyManagerEx

        2.On Unix (assume /media/disk is mounted USB flash drive)
          make install prefix=/media/disk
          cp ~/.mmex/mmexini.db3 /media/disk/mmex/share/mmex

        FIXME: GetLogDir() and temp files will be created on host filesystem.
*/
wxString mmex::getPathUser(EUserFile f)
{
        static const wxChar* files[USER_FILES_MAX] = { 
          wxT("mmexini.db3")
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

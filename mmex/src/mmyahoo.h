/*******************************************************
 Copyright (C) 2007 Greg Newton

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

#ifndef _MMYAHOO_H_
#define _MMYAHOO_H_

#include "guiid.h"

class wxSQLite3Database;

class mmYahoo
{
public:
    mmYahoo(wxSQLite3Database* db );
    ~mmYahoo();

private:
public:  /// Really should change these to private and use accessors. One day.

    wxSQLite3Database*  db_;
    wxString            Suffix_;
    wxString            OpenTimeStr_;
    wxString            CloseTimeStr_;
    bool                UpdatingEnabled_;
    wxDateTime          LastRefreshDT_;
    wxString            CSVColumns_;
    wxString            Server_;
    long                UpdateIntervalMinutes_;
    wxString            CSVTemporaryFile_;
    int                 DownloadTimeoutCounter_;

    void                ReadSettings();
    void                WriteSettings();
};

#endif  // _MMYAHOO_H_

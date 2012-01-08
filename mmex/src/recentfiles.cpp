/*************************************************************************
 Copyright (C) 2011,2012 Stefano Giorgio

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

#include "recentfiles.h"
#include "guiid.h"

RecentDatabaseFiles::RecentDatabaseFiles(wxSQLite3Database* ini_db, int listSize, wxString dbIndexName)
:mmex_inidb_(ini_db), menuRecentFiles_(NULL), recentListSize_(listSize), dbIndexName_(dbIndexName)
{
    for( int index = 0; index < recentListSize_; index++);
        recentFileList_.Add(wxEmptyString);
    loadRecentList();
}

RecentDatabaseFiles::RecentDatabaseFiles(wxSQLite3Database* ini_db, wxMenu *menuRecentFiles)
: mmex_inidb_(ini_db), menuRecentFiles_(menuRecentFiles)
{
    recentListSize_ = 6;
    dbIndexName_ = wxT("RECENT_DB_");
    recentFileList_.Add(mmDBWrapper::getLastDbPath(mmex_inidb_));
    for (int index = 1; index < recentListSize_; index ++ )
    {
        recentFileList_.Add(wxEmptyString);
    }
    loadRecentList();
    setMenuFileItems();
}

RecentDatabaseFiles::~RecentDatabaseFiles()
{
    saveRecentList();
}

void RecentDatabaseFiles::loadRecentList()
{
    for(int index = 1; index < recentListSize_; index ++)
    {
        wxString dbIndex = wxString() << dbIndexName_ << index;
        recentFileList_[index] = mmDBWrapper::getINISettingValue(mmex_inidb_,dbIndex,wxT(""));
    }
}

void RecentDatabaseFiles::saveRecentList()
{
    mmex_inidb_->Begin();
    for(int index = 1; index < recentListSize_; index ++)
    {
        wxString dbIndex = wxString() << dbIndexName_ << index;
        mmDBWrapper::setINISettingValue( mmex_inidb_, dbIndex, recentFileList_[index] );
    }
    mmex_inidb_->Commit();
}

void RecentDatabaseFiles::setMenuFileItems()
{
    // exit if no menu pointer provided. Generic Usage. 
    if (!menuRecentFiles_)
        return;

    if (menuRecentFiles_->FindItem(MENU_RECENT_FILES_0))
        menuRecentFiles_->Delete(MENU_RECENT_FILES_0);

    if (menuRecentFiles_->FindItem(MENU_RECENT_FILES_1))
        menuRecentFiles_->Delete(MENU_RECENT_FILES_1);

    if (menuRecentFiles_->FindItem(MENU_RECENT_FILES_2))
        menuRecentFiles_->Delete(MENU_RECENT_FILES_2);

    if (menuRecentFiles_->FindItem(MENU_RECENT_FILES_3))
        menuRecentFiles_->Delete(MENU_RECENT_FILES_3);

    if (menuRecentFiles_->FindItem(MENU_RECENT_FILES_4))
        menuRecentFiles_->Delete(MENU_RECENT_FILES_4);

    if (menuRecentFiles_->FindItem(MENU_RECENT_FILES_5))
        menuRecentFiles_->Delete(MENU_RECENT_FILES_5);

    if (recentFileList_[0].IsEmpty())
        menuRecentFiles_->Append(MENU_RECENT_FILES_0, _("Empty"));
    else 
        menuRecentFiles_->Append(MENU_RECENT_FILES_0, recentFileList_[0]);

    if (! recentFileList_[1].IsEmpty()) 
        menuRecentFiles_->Append(MENU_RECENT_FILES_1, recentFileList_[1]);

    if (! recentFileList_[2].IsEmpty()) 
        menuRecentFiles_->Append(MENU_RECENT_FILES_2, recentFileList_[2]);

    if (! recentFileList_[3].IsEmpty()) 
        menuRecentFiles_->Append(MENU_RECENT_FILES_3, recentFileList_[3]);

    if (! recentFileList_[4].IsEmpty()) 
        menuRecentFiles_->Append(MENU_RECENT_FILES_4, recentFileList_[4]);

    if (! recentFileList_[5].IsEmpty()) 
        menuRecentFiles_->Append(MENU_RECENT_FILES_5, recentFileList_[5]);
}

void RecentDatabaseFiles::updateRecentList(wxString currentFileName)
{
    bool sameFileDetected = false;
    int emptyPosition = 0;
    /*****************************************************************
     If we are using an existing current file, remove the same 
     filename from the current list by making it empty.
    ******************************************************************/
    for (int index = 1; index < recentListSize_; index ++ )
    {
        if (recentFileList_[index] == currentFileName )
        {
            recentFileList_[index].Empty();
            sameFileDetected = true;
            emptyPosition = index;
        }
    }

    /*****************************************************************
     If the same file was found in the list and was made empty,
     move the same file empty position to the top of the list.
    ******************************************************************/
    while (sameFileDetected && (! recentFileList_[0].IsEmpty()))
    {
        for (int index = 1; index < emptyPosition + 1; index ++ )
        {
            if (recentFileList_[index].IsEmpty())
            {
                recentFileList_[index] = recentFileList_[index - 1];
                recentFileList_[index - 1].Empty();
            }
        }
    }

    /*****************************************************************
     For new files, we make room at the top for the new file.
    ******************************************************************/
    if (!sameFileDetected)
    {
        // move all entries down one
        for (int index = 4; index > -1; index--)
        {
            if (!recentFileList_[index].IsEmpty())
                recentFileList_[index + 1] = recentFileList_[index];
        }
    }

    /*****************************************************************
     Add the new file name to the top of the list, and
     set the menu items to reflect the new list.
    ******************************************************************/
    recentFileList_[0] = currentFileName;
    setMenuFileItems();
}

void RecentDatabaseFiles::clearRecentList()
{
    for (int index = 0; index < recentListSize_; index ++ )
    {
        recentFileList_[index].Empty();
    }
    setMenuFileItems();
}

wxString RecentDatabaseFiles::getRecentFile(int fileNum)
{
    return recentFileList_[fileNum];
}

bool RecentDatabaseFiles::validLastListedFile(wxString& lastListedFileName)
{
    bool validFileName = false;
    for (int index = recentListSize_ - 1; index > -1; --index)
    {
        if ( !recentFileList_[index].IsEmpty())
        {
            lastListedFileName = recentFileList_[index];
            validFileName = true;
            break;
        }
    }

    return validFileName;
}


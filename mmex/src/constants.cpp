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

wxString mmex::getProgramDevelopers()
{
    wxString developers;
    developers << _("Active Developers:")
        << wxT("\n==========================================\n\
                Stefano .......... [stef145g]\n\
                Nikolay .......... [Nikolay]\n\
                Lisheng .......... [guanlisheng]\n\
                Pat .............. [omalleypat]\n\
                \n")
        << _("Code Builds/Fixes/Patches")
        << wxT("\n==========================================\n\
                Alex Ukhov\n\
                Bettega Stefano\n\
                Greg Newton\n\
                Ho Yin Kwok\n\
                hykwok\n\
                Igor\n\
                jblough\n\
                Jeancf\n\
                Lisheng .......... [guanlisheng]\n\
                Mike ............. [u4ear]\n\
                Nikolay .......... [Nikolay]\n\
                Pat .............. [omalleypat]\n\
                Paulo Lopes\n\
                Pieter\n\
                Priyank Bolia\n\
                Rohit Kumar\n\
                Stefano .......... [stef145g]\n\
                Stuart\n\
                Vadim\n\
                Vat\n\
                Wesley Ellis ..... [elliswr]");
    return developers;
}

wxString mmex::getProgramTranslators()
{
    return wxT("Brazilian Portuguese\n\
            -- Jose Carlos\n\
            Bulgarian\n\
            -- Ivan Gyokov\n\
            Croatian\n\
            -- Goran Bobanovic\n\
            Czech\n\
            -- Josef Remes\n\
            Dutch\n\
            -- Theo Kamphuis\n\
            Dutch (Belgian)\n\
            -- Rol\n\
            Englsh (UK)\n\
            - mcrossley\n\
            French\n\
            -- Berry As, Jean-Christophe, bipop,\n\
            -- drambeau, Ben, Ghislain Baudichaud\n\
            German\n\
            -- Schrempfi, goe, STiGMaTa_ch,\n\
            -- Kai Kajus Noack\n\
            Greek\n\
            -- Panagiotis Pentzeridis\n\
            Hebrew\n\
            -- Ykpui\n\
            Hungarian\n\
            -- Csaba, Zoltan Levardy [zlevardy]\n\
            Indonesian\n\
            -- Dindin\n\
            Italian\n\
            -- Maurizio [Mau13]\n\
            -- Claudio\n\
            Norwegian\n\
            -- Fredrik Strand, Torbjorn Kvande\n\
            Polish\n\
            -- Damian_pl\n\
            -- Andrzej\n\
            Portuguese (Portugal)\n\
            -- Fatela\n\
            Romanian\n\
            -- Munteanu & Florin\n\
            Russian\n\
            -- KarmaSV, Nikolay, Churzzz, Vadim\n\
            Simple Chinese\n\
            -- Liu Ming\n\
            Slovak\n\
            -- Abadyba, Misko B\n\
            Spanish\n\
            -- jorgeb\n\
            -- Javier, Roberto\n\
            Swedish\n\
            -- Tomplast\n\
            Tr. Chinese\n\
            -- Kysouke\n\
            Tamil\n\
            -- Gopi Gopalakrishnan,\n\
            -- Muguntharaj Subramanian\n\
            Turkish\n\
            -- Murat Bilgen\n\
            Ukrainian\n\
            -- Leonid Shturma\n\
            Vietnamese\n\
            -- Nicky, Luu Cong Thanh");
}

wxString mmex::getProgramDocWriters()
{
    return  wxT("English:\n\
            -- Stefano ... [stef145g]\n\
            -- Tina P, Terry Wick\n\
            Italian ... Maurizio [Mau13]\n\
            French .... catacotox\n\
            Russian ... Nikolay\n\
            Polish .... Damian_pl\n\
            Spanish ... jorgeb");
}

wxString mmex::getProgramArtists()
{
    return wxT("Silk Icons\n\
                ==========================================\n\
                http://www.famfamfam.com/lab/icons/silk/");
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

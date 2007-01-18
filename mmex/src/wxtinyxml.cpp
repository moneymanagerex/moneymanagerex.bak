// This crude code is for using wxWidget streams with TinyXML. 
// This is useful, for example, for loading and saving XML directly
// into a ZIP file (with the new ZIP streams in 2.5.4).

// Copyright (c) 2005 Andrew Ziem. All rights reserved.
// This code is licensed under the three licenses:  wxWindows Library Licence, Version 3;
// Zlib license (like TinyXML); and the GNU General Public License version 2 or later.
               

#include "wxtinyxml.h"

bool wxTiXmlDocument::LoadFile( wxInputStream &istream )
{
        Clear();
        location.Clear();
       
        wxTextInputStream txt(istream);
       
        wxString data;
       
        do
        {
            const wxString s = txt.ReadLine();

            if ( istream.Eof() && s.empty() )
                break;
               
            data += s;

        } while (1);

        Parse( wxConvCurrent->cWX2MB(data), 0, TIXML_ENCODING_UTF8 );

        if (  Error() )
                return false;
        else
                return true;
}



bool wxTiXmlDocument::SaveFile (wxOutputStream &ostream)
{
        wxTextOutputStream txt(ostream);
       
#ifdef TIXML_USE_STL
//#error not implemented
#else
        TiXmlOutStream outs;
        StreamOut (&outs);
        const char *c =  outs.c_str();
        txt << wxString(c, wxConvUTF8);
#endif
    return true;
} 

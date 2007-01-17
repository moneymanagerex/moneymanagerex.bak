#ifndef __WXTINYXML_H__
#define __WXTINYXML_H__

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "tinyxml.h"
#include "wx/txtstrm.h"

class wxTiXmlDocument : public TiXmlDocument
{
public:
bool LoadFile( wxInputStream &istream );
bool SaveFile( wxOutputStream &ostream );

};


#endif
 
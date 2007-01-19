///////////////////////////////////////////////////////////////////////////////
// Name:        wxsqlite3def.h
// Purpose:     wxWidgets wrapper around the SQLite3 embedded database library.
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-07-14
// Changes:     2005-10-03  - Upgrade to SQLite3 version 3.2.7
//              2005-10-09  - Corrected error in wxSQLite3Table::FindColumnIndex
//              2005-10-30  - Added wxGTK build support
//              2005-11-01  - Corrected wxSQLite3ResultSet::GetInt64.
//                            Added wxSQLite3Table::GetInt64
//              2005-11-09  - Optionally load SQLite library dynamically
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file wxsqlite3def.h Compile time switches for the wxSQLite3 class

/** \mainpage wxSQLite3

\section intro What is wxSQLite3?

  \b wxSQLite3 is a C++ wrapper around the public domain <a href="http://www.sqlite.org">SQLite 3.x</a> database
  and is specifically designed for use in programs based on the \b wxWidgets library.

  Several solutions already exist to access SQLite databases. To name just a few:

  - <a href="http://sourceforge.net/projects/wxsqlite">wxSQLite</a> :
    This is a wxWidgets wrapper for version 2.8.x of SQLite. 
    Current version of SQLite is 3.2.7 having a lot more features - which are not supported by this wrapper.

  - <a href="http://www.codeproject.com/database/CppSQLite.asp">CppSQLite</a> : 
    Not wxWidgets specific, but with (partial) support for the newer version 3.x of SQLite.

  - <a href="http://wxcode.sf.net">databaselayer</a> :
    This is a database abstraction and supports SQLite and Firebird databases. 
    It's a wrapper around \b CppSQLite for SQLite resp. \b IBPP for Firebird.

  The component \b wxSQLite3 was inspired by all three mentioned SQLite wrappers.
  \b wxSQLite3 does not try to hide the underlying database, in contrary almost all special features
  of the SQLite3 version 3.2.7 are supported, like for example the creation of user defined
  scalar or aggregate functions.

  Since SQLite stores strings in UTF-8 encoding, the wxSQLite3 methods provide automatic conversion between
  wxStrings and UTF-8 strings. This works best for the \b Unicode builds of \b wxWidgets.
  The feature is not fully tested for \b ANSI builds.

\section version Version history

<dl>
<dt><b>1.4.2</b> - <i>November 2005</i></dt>
<dd>
Optimized code for wxString arguments

</dd>
<dt><b>1.4.1</b> - <i>November 2005</i></dt>
<dd>
Fixed a bug in wxSQLite3Database::TableExists,<br>
Changed the handling of Unicode string conversion,<br>
Added support for different transaction types

</dd>
<dt><b>1.4</b> - <i>November 2005</i></dt>
<dd>
Optionally load the SQLite library dynamically at run time.

</dd>
<dt><b>1.3.1</b> - <i>November 2005</i></dt>
<dd>
Corrected wxSQLite3ResultSet::GetInt64.<br>
Added wxSQLite3Table::GetInt64

</dd>
<dt><b>1.3</b> - <i>October 2005</i></dt>
<dd>
Added wxGTK build support<br>

</dd>
<dt><b>1.2</b> - <i>October 2005</i></dt>
<dd>
Corrected error in wxSQLite3Table::FindColumnIndex<br>

</dd>
<dt><b>1.1</b> - <i>October 2005</i></dt>
<dd>
Upgrade to SQLite version 3.2.7 <br>

</dd>

<dt><b>1.0</b> - <i>July 2005</i></dt>
<dd>
First public release
</dd>
</dl>

\author Ulrich Telle (<a href="&#109;&#97;&#105;&#108;&#116;&#111;:&#117;&#108;&#114;&#105;&#99;&#104;&#46;&#116;&#101;&#108;&#108;&#101;&#64;&#103;&#109;&#120;&#46;&#100;&#101;">ulrich DOT telle AT gmx DOT de</a>)

 */

#ifndef _WX_SQLITE3_DEF_H_
#define _WX_SQLITE3_DEF_H_

// Conditional compilation
// -----------------------

//! If this define is set to 1, then the SQLite library will be loaded dynamically
//! otherwise a link library is required to build wxSQLite3.
#ifndef wxUSE_DYNAMIC_SQLITE3_LOAD
    #define wxUSE_DYNAMIC_SQLITE3_LOAD   0
#endif

#ifdef WXMAKINGDLL_WXSQLITE3
    #define WXDLLIMPEXP_SQLITE3 WXEXPORT
#elif defined(WXUSINGDLL_WXSQLITE3)
    #define WXDLLIMPEXP_SQLITE3 WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_SQLITE3
#endif

#endif // _WX_SQLITE3_DEF_H_


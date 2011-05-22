sqlite3 and wxSQLite3 sources.

Copy to this folder:

1.Files from archive sqlite-amalgamation-<latest_version>.zip.
  For current version sqlite3 3.7.6.1:
  sqlite3.c
  sqlite3.h
  sqlite3ext.h 

2.Files from "wxsqlite\sqlite3\secure\src\codec-c" of the sources of wxsqlite3.
  For current version wxsqlite3-2.1.1:
  codec.c
  codec.h
  codecext.c
  extensionfunctions.c
  rijndael.c
  rijndael.h
  sha2.c
  sha2.h
  sqlite3secure.c

  Headers of wxSQLite3 copy to mmex\lib\wxsqlite\wx:
  wxsqlite3.h
  wxsqlite3def.h
  wxsqlite3dyn.h
  wxsqlite3opt.h

  wxsqlite3.cpp copy to mmex\lib\wxsqlite.

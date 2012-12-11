External libraries' sources here.

wxSQLite3 and sqlite3 sources from wxSQLite3
(ensures wxSQLite3 and sqlite3 are in sync)
--------------------------------------------

Current versions: wxSQLite3 Version: 3.0.1
                  SQLite Version: 3.7.14.1
===============================================================================                
Copy all files From: wxsqlite3\src
               to  : MoneyManagerEX\trunk\mmex\lib\wxsqlite

Copy all files From: wxsqlite3\include\wx
               to  : MoneyManagerEX\trunk\mmex\lib\wxsqlite\wx 

Copy all files from: wxsqlite3\sqlite3\secure\src
               to  : MoneyManagerEX\trunk\mmex\lib\sqlite

Copy file from: wxsqlite3\Readme.txt
          to  : MoneyManagerEX\trunk\mmex\lib\wxsqlite\Readme.txt 
===============================================================================                
Source files for: wxSQLite3 Version 3.0.1
  wxsqlite3.cpp

Headers of for: wxSQLite3 - wx:
  wxsqlite3.h
  wxsqlite3def.h
  wxsqlite3dyn.h
  wxsqlite3opt.h

Source files for: wxSQLite3 - sqlite3 interface
  codec.c
  codec.h
  codecext.c
  extensionfunctions.c
  rijndael.c
  rijndael.h
  sha2.c
  sha2.h
  sqlite3secure.c
  sqlite3.def

Source files for: sqlite3 Version 3.7.14.1
  sqlite3.c
  sqlite3.h
  sqlite3ext.h
  sqlite3dyn.h

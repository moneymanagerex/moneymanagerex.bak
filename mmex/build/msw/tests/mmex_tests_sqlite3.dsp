# Microsoft Developer Studio Project File - Name="mmex_tests_sqlite3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sqlite3 - Win32 Release Multilib WX_PYTHON_0
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mmex_tests_sqlite3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mmex_tests_sqlite3.mak" CFG="sqlite3 - Win32 Release Multilib WX_PYTHON_0"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sqlite3 - Win32 DLL Unicode Debug Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Unicode Debug Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Unicode Debug Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Unicode Debug Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Unicode Release Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Unicode Release Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Unicode Release Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Unicode Release Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Debug Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Debug Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Debug Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Debug Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Release Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Release Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Release Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 DLL Release Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Debug Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Debug Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Debug Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Debug Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Release Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Release Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Release Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Unicode Release Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Debug Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Debug Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Debug Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Debug Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Release Monolithic WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Release Monolithic WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Release Multilib WX_PYTHON_1" (based on "Win32 (x86) Static Library")
!MESSAGE "sqlite3 - Win32 Release Multilib WX_PYTHON_0" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Debug Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_sharud"
# PROP BASE Intermediate_Dir "vc_sharud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_sharud"
# PROP Intermediate_Dir "vc_sharud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Debug Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_sharud"
# PROP BASE Intermediate_Dir "vc_sharud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_sharud"
# PROP Intermediate_Dir "vc_sharud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Debug Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_sharud"
# PROP BASE Intermediate_Dir "vc_sharud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_sharud"
# PROP Intermediate_Dir "vc_sharud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Debug Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_sharud"
# PROP BASE Intermediate_Dir "vc_sharud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_sharud"
# PROP Intermediate_Dir "vc_sharud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_sharud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Release Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_sharu"
# PROP BASE Intermediate_Dir "vc_sharu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_sharu"
# PROP Intermediate_Dir "vc_sharu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Release Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_sharu"
# PROP BASE Intermediate_Dir "vc_sharu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_sharu"
# PROP Intermediate_Dir "vc_sharu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Release Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_sharu"
# PROP BASE Intermediate_Dir "vc_sharu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_sharu"
# PROP Intermediate_Dir "vc_sharu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Unicode Release Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_sharu"
# PROP BASE Intermediate_Dir "vc_sharu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_sharu"
# PROP Intermediate_Dir "vc_sharu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_sharu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_sharu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_sharu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Debug Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_shard"
# PROP BASE Intermediate_Dir "vc_shard\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_shard"
# PROP Intermediate_Dir "vc_shard\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shard\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shard\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Debug Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_shard"
# PROP BASE Intermediate_Dir "vc_shard\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_shard"
# PROP Intermediate_Dir "vc_shard\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shard\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shard\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Debug Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_shard"
# PROP BASE Intermediate_Dir "vc_shard\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_shard"
# PROP Intermediate_Dir "vc_shard\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shard\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shard\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Debug Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_shard"
# PROP BASE Intermediate_Dir "vc_shard\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_shard"
# PROP Intermediate_Dir "vc_shard\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_shard\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shard\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shard\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Release Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_shar"
# PROP BASE Intermediate_Dir "vc_shar\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_shar"
# PROP Intermediate_Dir "vc_shar\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shar\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shar\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Release Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_shar"
# PROP BASE Intermediate_Dir "vc_shar\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_shar"
# PROP Intermediate_Dir "vc_shar\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shar\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shar\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Release Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_shar"
# PROP BASE Intermediate_Dir "vc_shar\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_shar"
# PROP Intermediate_Dir "vc_shar\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shar\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shar\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 DLL Release Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_shar"
# PROP BASE Intermediate_Dir "vc_shar\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_shar"
# PROP Intermediate_Dir "vc_shar\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_shar\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_shar\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_shar\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Debug Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statud"
# PROP BASE Intermediate_Dir "vc_statud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statud"
# PROP Intermediate_Dir "vc_statud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Debug Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statud"
# PROP BASE Intermediate_Dir "vc_statud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statud"
# PROP Intermediate_Dir "vc_statud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Debug Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statud"
# PROP BASE Intermediate_Dir "vc_statud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statud"
# PROP Intermediate_Dir "vc_statud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Debug Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statud"
# PROP BASE Intermediate_Dir "vc_statud\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statud"
# PROP Intermediate_Dir "vc_statud\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statud\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statud\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statud\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Release Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_statu"
# PROP BASE Intermediate_Dir "vc_statu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_statu"
# PROP Intermediate_Dir "vc_statu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Release Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_statu"
# PROP BASE Intermediate_Dir "vc_statu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_statu"
# PROP Intermediate_Dir "vc_statu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Release Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_statu"
# PROP BASE Intermediate_Dir "vc_statu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_statu"
# PROP Intermediate_Dir "vc_statu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Unicode Release Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_statu"
# PROP BASE Intermediate_Dir "vc_statu\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_statu"
# PROP Intermediate_Dir "vc_statu\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_statu\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statu\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statu\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Debug Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statd"
# PROP BASE Intermediate_Dir "vc_statd\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statd"
# PROP Intermediate_Dir "vc_statd\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statd\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statd\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Debug Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statd"
# PROP BASE Intermediate_Dir "vc_statd\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statd"
# PROP Intermediate_Dir "vc_statd\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statd\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statd\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Debug Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statd"
# PROP BASE Intermediate_Dir "vc_statd\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statd"
# PROP Intermediate_Dir "vc_statd\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statd\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statd\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Debug Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_statd"
# PROP BASE Intermediate_Dir "vc_statd\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_statd"
# PROP Intermediate_Dir "vc_statd\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W4 /Zi /Fdvc_statd\sqlite3.pdb /Od /Gm -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_statd\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_statd\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Release Monolithic WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_stat"
# PROP BASE Intermediate_Dir "vc_stat\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_stat"
# PROP Intermediate_Dir "vc_stat\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_stat\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_stat\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Release Monolithic WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_stat"
# PROP BASE Intermediate_Dir "vc_stat\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_stat"
# PROP Intermediate_Dir "vc_stat\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_stat\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_stat\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Release Multilib WX_PYTHON_1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_stat"
# PROP BASE Intermediate_Dir "vc_stat\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_stat"
# PROP Intermediate_Dir "vc_stat\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_stat\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_stat\sqlite3.lib"

!ELSEIF  "$(CFG)" == "sqlite3 - Win32 Release Multilib WX_PYTHON_0"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_stat"
# PROP BASE Intermediate_Dir "vc_stat\sqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_stat"
# PROP Intermediate_Dir "vc_stat\sqlite3"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD CPP /nologo /FD /MD /W4 /Fdvc_stat\sqlite3.pdb /O2 -DSQLITE_CORE -DSQLITE_HAS_CODEC -DSQLITE_ENABLE_COLUMN_METADATA /D "WIN32" /D "_LIB" /D "NDEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"vc_stat\sqlite3.lib"
# ADD LIB32 /nologo /out:"vc_stat\sqlite3.lib"

!ENDIF

# Begin Target

# Name "sqlite3 - Win32 DLL Unicode Debug Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Unicode Debug Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 DLL Unicode Debug Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Unicode Debug Multilib WX_PYTHON_0"
# Name "sqlite3 - Win32 DLL Unicode Release Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Unicode Release Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 DLL Unicode Release Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Unicode Release Multilib WX_PYTHON_0"
# Name "sqlite3 - Win32 DLL Debug Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Debug Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 DLL Debug Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Debug Multilib WX_PYTHON_0"
# Name "sqlite3 - Win32 DLL Release Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Release Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 DLL Release Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 DLL Release Multilib WX_PYTHON_0"
# Name "sqlite3 - Win32 Unicode Debug Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 Unicode Debug Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 Unicode Debug Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 Unicode Debug Multilib WX_PYTHON_0"
# Name "sqlite3 - Win32 Unicode Release Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 Unicode Release Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 Unicode Release Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 Unicode Release Multilib WX_PYTHON_0"
# Name "sqlite3 - Win32 Debug Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 Debug Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 Debug Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 Debug Multilib WX_PYTHON_0"
# Name "sqlite3 - Win32 Release Monolithic WX_PYTHON_1"
# Name "sqlite3 - Win32 Release Monolithic WX_PYTHON_0"
# Name "sqlite3 - Win32 Release Multilib WX_PYTHON_1"
# Name "sqlite3 - Win32 Release Multilib WX_PYTHON_0"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sqlite\sqlite3secure.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sqlite\codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sqlite\rijndael.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sqlite\sqlite3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sqlite\sqlite3ext.h
# End Source File
# End Group
# End Target
# End Project


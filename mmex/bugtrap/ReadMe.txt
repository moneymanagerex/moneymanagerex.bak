This folder contains all BugTrap components required for Win32 projects:
------------------------------------------------------------------------------------
BugTrap[U].dll                     - BugTrap DLL module.
BugTrap[U].lib                     - BugTrap library file used by linker.
dbghelp.dll                        - DbgHelp library (see "BugTrap Developer's Guide"
                                     for details).
BugTrap.h                          - Header file with BugTrap API definitions.
BTTrace.h                          - C++ wrapper of custom logging functions.
BTAtlWindow.h                      - ATL/WTL exception handlers.
BTMfcWindow.h                      - MFC exception handlers.
CrashExplorer.exe                  - MAP file analyzer.
BugTrap.chm                        - BugTrap 1.0 Specification.
------------------------------------------------------------------------------------
File names may vary by the following signs:
  [U]   - Unicode aware version has 'U' letter. ANSI version doesn't have 'U' letter.
  [D]   - Debug version has 'D' letter. Release version doesn't have 'D' letter.

Please see "BugTrap Developer's Guide" for additional information about file types
used by BugTrap for Win32.
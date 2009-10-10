		
		*** Build instruction for MS Windows platform ***


You can build mmex on Windows using VisualC++ or MinGW compilers.
Firstly you can run mmex/bootstrap.bat which will refresh makefiles.
Bakefile must be installed to do that. But you can skip this step 
if you don't add\remove project's files.


	*** Compiling using VisualC++ *** 

1.Build wxWidgets library using VisualStudio.
2.Open "Visual Studio 200X Command Prompt".
3.Change working directory to mmex\build\msw.
4.Edit mmex\build\msw\config.vc to select build options (Ansi\Unicode, Debug\Release, Static\Shared, etc.).
5.Type in command prompt to build mmex
  nmake -f makefile.vc 

  You can use almost any version of Visual Studio because we do not use IDE at all.


	*** Compiling using MinGW *** 

1.Install the latest version of MinGW and GCC compiler for it.

2.Build wxWidgets library using gcc from MinGW. Use these option in config.gcc:
  SHARED ?= 0
  MONOLITHIC ?= 1 

3.You can create .bat file to run gcc with next content (assume MinGW installed in "C:\bin\MinGW")

  set PATH=C:\bin\MinGW\bin
  mingw32-make -f makefile.gcc %* 2> mingw.log

4.Edit mmex\build\msw\config.gcc to select build options (Ansi\Unicode, Debug\Release, Static\Shared, etc.).
5.Run created on step #3 .bat file from mmex\build\msw directory to build mmex.


	*** Your compiler do not present in this document *** 

This means someone should modify bakefile build scripts to add output for your compiler.
Currently bakefile can generate makefiles\projects for these compilers (run bakefile --help)

    borland       Borland C/C++ makefiles
    dmars         Digital Mars makefiles
    dmars_smake   Digital Mars makefiles for SMAKE
    gnu           GNU toolchain makefiles (Unix)
    mingw         MinGW makefiles (mingw32-make)
    msevc4prj     MS eMbedded Visual C++ 4 project files
    msvc          MS Visual C++ nmake makefiles
    msvc6prj      MS Visual C++ 6.0 project files
    msvs2003prj   MS Visual Studio 2003 project files
    msvs2005prj   MS Visual Studio 2005 project files
    msvs2008prj   MS Visual Studio 2008 project files
    symbian       Symbian development files
    watcom        OpenWatcom makefiles
    xcode2        Xcode 2.4 project files

The mmex build scripts resides in mmex\build\bakefiles. 
Firstly modify Bakefiles.bkgen to add missing output formats.
Append new format to tag <add-formats> and add flag to makefile's output directory.
If you are lucky, you will get makefile\project for your compiler after running mmex\bootstrap[.bat].

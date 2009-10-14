		*** Build instruction for MS Windows platform ***


You can build MMEx on Windows using VisualC++ 2003 or later, or MinGW with GCC.
Firstly you can run mmex/bootstrap.bat which will refresh makefiles and projects.
Bakefile must be installed to do that. But you can skip this step if you just want 
to build sources.


	*** Prerequisites ***


1. Install wxWidgets 2.8.0 and above in a directory like c:\wxwidgets2.8.0 and then 
   create an environment variable WXWIN to point to this directory.

2. Follow the instructions given by WxWidgets install on how to build it.
   Build these configurations: Unicode Release, Unicode Debug, Release, Debug.

3. Install GNU GetText for Windows, http://gnuwin32.sourceforge.net/packages/gettext.htm. 
   This software requires to generate binary translation files from .po files.
   Append path to bin folder of installed software to PATH environment variable
   (usually "C:\Program Files\GnuWin32\bin").



	*** Compiling using VisualC++ command prompt *** 


1.Open "Visual Studio 200X Command Prompt".
2.Change working directory to mmex\build\msw.
3.Edit mmex\build\msw\config.vc to select build options (Ansi\Unicode, Debug\Release, Static\Shared, etc.).
4.Type in command prompt
  nmake -f makefile.vc 

  This is the most preferable way to build MMEX.



	*** Compiling using VisualStudio IDE *** 


1. Open build\msw\mmex.sln under mmex directory.
   Select one of following configurations to build (accordingly to wxWidgets builds):
   
   Unicode builds, for Windows 2000 or later
   "Unicode Release Multilib WX_PYTHON_0" <-- choice number one for most people
   "Unicode Debug Multilib WX_PYTHON_0"

   Ansi builds, for Windows 95/98/Me.
   "Release Multilib WX_PYTHON_0" 
   "Debug Multilib WX_PYTHON_0"

   To build unit tests use solution build\msw\tests\mmex_tests.sln. Build tests, 
   copy mmex_tests.exe to mmex\runtime and run one.

2. Copy currency.db3 from mmex\runtime into the appropriate build directory.
   Now you can run the project to launch the exe.

3. IDE cannot translate files from mmex\runtime\en\*.po. You should use one of makefiles
   in that directory to build translations. For example, run Visual Studio Command Prompt,
   change dir to mmex\runtime\en and type "nmake -f makefile.vc".


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

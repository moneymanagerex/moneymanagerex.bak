Build instructions for MS Windows platform
===============================================================================
Microsoft Visual C++ 2010 Express compiler
-------------------------------------------------------------------------------


NOTE:   The directory msw-vc-2010e has the project files for:
                   Microsoft Visual C++ 2010 Express.
                     For other compilers, see the
        readme file in the mmex/build/msw directory.


Prerequisites
-------------------------------------------------------------------------------

1. Installation of the following:
   MS-VC 2010e  http://www.microsoft.com/visualstudio/eng/downloads#d-2010-express
   wxWidgets    http://www.wxwidgets.org/downloads/
   Boost        http://www.boost.org/

   Optional:
   Doxygen      http://www.stack.nl/~dimitri/doxygen/download.html
                Allows documentation to be obtained fromthe source code.
   PoEdit       http://www.poedit.net/
   Install GNU GetText for Windows, http://gnuwin32.sourceforge.net/packages/gettext.htm.
   This software requires to generate binary translation files from .po files.
   Append path to bin folder of installed software to PATH environment variable
   (usually "C:\Program Files\GnuWin32\bin").
                     
2. Create environment variables for wxWidgets and Boost.
   See instructions below
   
3. Follow the instructions given by WxWidgets install on how to build it.
   http://wiki.wxwidgets.org/Microsoft_Visual_C%2B%2B_Guide#Visual_Studio_C.2B.2B_2010

   Build the appropriate configurations for your needs:
   Recommended configurations for mmex: Unicode Release, Unicode Debug

   Note:
   To build wxWidgets 2.8.12 for mmex, the wxWidgets project files will need to be
   upgraded using an earlier version of MS VC++ (2003) This will allow MS VC++ 2010e
   to upgrade the configuration files again, and allowing a successful build.
         
4. To build installation (setup) of MMEX download and install InnoSetup Quick Start Pack
   (which includes InnoSetup itself and Inno Setup Preprocessor). Append path to folder
   of installed software (something like a "C:\Program Files\Inno Setup 5")
   to PATH environment variable.

Compiling using Microsoft Visual C++ 2010 Express IDE
-------------------------------------------------------------------------------

1. Ensure Prerequisites are complete with a build of wxWidgets 2.8.11 or greater
   using "Unicode Release" and/or "Unicode Debug" depending on preferences below.
   
2  Use the directory: mmex\build\msw-vc-2010e

3  Open the project file mmex\build\msw-vc-2010e\mmex.sln
   MS VC++ 2010e will set up the project files to the user's environment.
   
4  In the Solution Explorer set the mmex project as the Startup Project
   
3. Using the Build ->configurations Manager...
   Select one of following configurations to build (accordingly to wxWidgets builds):
   "Unicode Release Multilib WX_PYTHON_0" <-- choice number one for most people
   "Unicode Debug Multilib WX_PYTHON_0"

4. Build the solution...

5. Locate and Run the batch file: update_mmex_build_files.bat
   location: mmex\trunk\mmex\setup\win\update_mmex_build_files.bat

   This will allow MS VC++ 2010e IDE to run mmex.exe successfully.

Allowing Configuration Switching
--------------------------------
New configurations to allow multiple wxWidgets builds.
-------------------------------------------------------------------------------
Variable  | wxWidgets Build  |  Configurations
----------|------------------|--------------------------------------------
WXWIN     | wxWidgets 2.x.xx |  set for generic Windows configurations.
WXWIN_28  | wxWidgets 2.8.xx |  wx28 Debug Unicode Multilib WX_PYTHON_0
          |                  |  wx28 Release Unicode Multilib WX_PYTHON_0
WXWIN_29  | wxWidgets 2.9.xx |  wx29 Debug Unicode Multilib WX_PYTHON_0
          |                  |  wx29 Release Unicode Multilib WX_PYTHON_0

Environment variables need to access the different builds on Windows.

Variable  | Value                    | Comment
----------|--------------------------|-------------------------------
WXWIN     | C:\CPP\wxWidgets-2.8.12  | Location of wxWidgets 2.8.xx
BOOSTDIR  | C:\CPP\boost_1_53_0      | Location of Boost source files
WXWIN_28  | C:\CPP\wxWidgets-2.8.12  | Location of wxWidgets 2.8.xx
WXWIN_29  | C:\CPP\wxWidgets-2.9.4   | Location of wxWidgets 2.9.x


Setting up Environment variables using Windows 7
-------------------------------------------------------------------------------
Control Panel ---> System and Security ---> System
Advanced system settings        (Located on left navigation panel)

System Properties       (Dialog) ---> Advanced  (Tab)
Environment Variables   (Button)

End of File.


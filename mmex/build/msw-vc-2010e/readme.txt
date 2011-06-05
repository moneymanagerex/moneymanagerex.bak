		*** Build instructions for MS Windows platform ***
                            using
        *** Microsoft Visual C++ 2010 Express compiler ***
        
        
NOTE:   The directory msw-vc-2010e has the project files for:
                   Microsoft Visual C++ 2010 Express.
                     For other compilers, see the 
        readme file in the mmex/build/msw directory.


                     *** Prerequisites ***

1. Install wxWidgets 2.8.10 and above in a directory like C:\wxWidgets-2.8.10
   and then create an environment variable WXWIN to point to that directory.

2. Follow the instructions given by WxWidgets install on how to build it.
   Build these configurations: Unicode Release, Unicode Debug, Release, Debug
   (or only those you need).

3. Install Boost C++ Libraries and create an environment variable BOOSTDIR
   that points to directory where Boost were installed.

4. Install GNU GetText for Windows, http://gnuwin32.sourceforge.net/packages/gettext.htm.
   This software requires to generate binary translation files from .po files.
   Append path to bin folder of installed software to PATH environment variable
   (usually "C:\Program Files\GnuWin32\bin").

5. To build installation (setup) of MMEX download and install InnoSetup Quick Start Pack
   (which includes InnoSetup itself and Inno Setup Preprocessor). Append path to folder
   of installed software (something like a "C:\Program Files\Inno Setup 5")
   to PATH environment variable.


   *** Compiling using Microsoft Visual C++ 2010 Express IDE ***

1. Ensure Prerequisites are complete with a build of wxWidgets 2.8.11 or greater
   using "Unicode Release" and/or "Unicode Debug" depending on preferences below.
   
2  Use the directory: mmex/build/msw-vc-2010e

3  Open the project file mmex/build/msw-vc-2010e/mmex.sln.
   MS VC++ 2010e will set up the project files to the user's environment.
   
4  In the Solution Explorer set the mmex project as the Startup Project
   
3. Using the Build ->configurations Manager...
   Select one of following configurations to build (accordingly to wxWidgets builds):
   "Unicode Release Multilib WX_PYTHON_0" <-- choice number one for most people
   "Unicode Debug Multilib WX_PYTHON_0"

4. Build the solution...

                    ---------- 000000000000000 ----------

*** Build instructions for MS Windows platform ***
                         using
*** Microsoft Visual C++ 2010 Express compiler ***

NOTE:   The directory msw-vc-2010e has the project files for:
        Microsoft Visual C++ 2010 Express.
        For other compilers, see the readme file in the
        directory: mmex/build/msw


=====================================================================================
   Setting up the Build Environment
=====================================================================================
1. Install Microsoft Visual C++ 2010e

2. Install wxWidgets 2.9.3 or above in a directory like C:\cpp\wxWidgets-2.9.3
   Ref: http://www.wxwidgets.org/

3. Unzip the Boost C++ Libraries to a directory like c:\cpp\boost_1_46_0
   Ref: http://www.boost.org/
   
4. Build wxWidgets by following the instructions in the directory:
     C:\CPP\wxWidgets-2.9.3\docs\msw\install.txt
   Suggest the following:
   * Using the MS IDE, Open the project file:
     C:\CPP\wxWidgets-2.9.3\build\msw\wx_vc9.sln
     - This will convert the project files to the MS VC++ 2010e format.
   * Build the following configurations:
     - Debug
     - Release

5  Create the following environment variables, assuming the libraries are
   installed in the following directories:
   -----------------------------------------------
   Variable            Value          
   -----------------------------------------------
   BOOSTDIR            c:\cpp\boost_1_49_0
   WXWIN               c:\cpp\wxWidgets-2.9.3
   ----------------------------------------------- 
   
6. Install GNU GetText for Windows, http://gnuwin32.sourceforge.net/packages/gettext.htm.
   This software requires to generate binary translation files from .po files.
   Append path to bin folder of installed software to PATH environment variable
   (usually "C:\Program Files\GnuWin32\bin").
   Note: This step may not be necessary but is included incase it is.
   
7. To build installation (setup) of MMEX download and install InnoSetup Quick Start Pack
   (which includes InnoSetup itself and Inno Setup Preprocessor). Append path to folder
   of installed software (something like a "C:\Program Files\Inno Setup 5")
   to PATH environment variable.


=====================================================================================
   *** Compiling MMEX using Microsoft Visual C++ 2010 Express IDE ***
              Using the directory: mmex/build/msw-vc-2010e
=====================================================================================

1  Using the MS IDE, Open the project file: mmex/build/msw-vc-2010e/mmex.sln
   MS VC++ 2010e will set up the project files to the user's environment.
   
4  Using the Solution Explorer, set the Startup Project to mmex
   
3. Using the Build -> Configuration Manager...
   Select one of following configurations to build (accordingly to wxWidgets build):
   "Unicode Release Multilib WX_PYTHON_0" <-- choice number one for most people
   "Unicode Debug Multilib WX_PYTHON_0"

4. Build the solution...

=====================================================================================
Note: To build MMEX Release Version 0.9.9.x you will need do the following:
1. Install wxWidgets 2.8.12 in a separate directory like: c:\cpp\wxWidgets-2.8.12
   Note 1: As wxWidgets 2.8.12 does not support MS VC++ 2010e, the msw configuration
           will need to be converted by a previous version of MS VC++ first. Then it
           can be can be upgraded to use MS VC++ 2010e
   Note 2: SubVersion 2492 and above can be built using wxWidgets 2.9.3
           SubVersion 2491 and below will need wxWidgets 2.8.12
           
2. Build the wxWidgets 2.8.12 configurations:
   Unicode Debug
   Unicode Release
   
3. Change the environment variable WXWIN to c:\cpp\wxWidgets-2.8.12

4. Build mmex using the same configurations described above.
=====================================================================================
End of Document.
--------------------------
Money Manager Ex [MMEX]
------------------------
Website:      
http://www.thezeal.com/software/managerex

Author:       
Madhan Kanagavel [madhankk AT GMAIL . COM]

Description:
Money Manager Ex is a free, open-source, cross-platform, easy-to-use personal finance software. It primarily helps organize one's finances and keeps track of where, when and how the money goes. It is also a great tool to get a bird's eye view of your financial worth.

Money Manager includes all the basic features that 90% of users would want to see in a personal finance application. The design goals are to concentrate on simplicity and user-friendliness - something one can use everyday.

Requirements:

Required Libraries
- wxWidgets >= 2.8.0 (www.wxwidgets.org)

Prepackaged Libraries
- wxSqlite3
- sqlite
- TinyXML

--------------------------
Building Money Manager Ex
--------------------------

=================
Microsoft Windows
=================

1. Install Microsoft Visual Studio 2008 C++ Express, which is a free download
2. Install the Platform SDK and make sure Visual Studio 2008 is correctly setup.
3. Install wxWidgets 2.8.0 and above in a directory like c:\pgmming\wxwidgets2.8.0 and then create an environment variable WXWIN to point to this directory.
4. Follow the instructions given by WxWidgets install on how to build it in 4 build modes. Static Ansi Debug, Static Ansi Release, Static Unicode Debug, Static Unicode Release.
5. Install GNU GetText for Windows, http://gnuwin32.sourceforge.net/packages/gettext.htm. This software requires to generate binary translation files from .po files.
   Append path to bin folder of installed software to PATH environment variable (usually "C:\Program Files\GnuWin32\bin").
6. Open mmex.sln under mmex directory and do a batch build of all modes neceesary
7. Copy currency.db3 from runtime directory into the appropriate build directories. Now you can run the project to launch the exe.
8. Static Ansi is used under Windows 95/98/Me
9. Static Unicode is used under Windows 2000/Windows XP/Windows 2003.

Windows Installer Setup
------------------------
MMEX uses innosetup to create installer packages.
The script is checked into mmex\setup directory

=================
Building on Linux
=================

Pre-built Binaries
------------------
If you don't want to build MMEX, you can download prebuilt binary packages for UBUNTU and SUSE.

Building from Source
--------------------

GENERIC INSTRUCTIONS

1. Download latest wxWidgets for wxGTK and download and install.
    
  1) Read the install directions in wxWidgets for how to do a build and then install the build on the machine. For eg wxWidgets has been built with --with-gtk --disable-shared --enable-unicode
  2) After installation, wx-config will copied to the /usr/bin and can now correctly identify the command line params required.

3. Navigate to trunk/mmex

   Do 
        >  mkdir buildgtk
        >  cd buildgtk
        >  ../configure --with-wx-config=[LOCATION of WX-CONFIG]
        >  make
        >  cp mmex ../runtime
        
   trunk/runtime/mmex/mmex should run Money Manager Ex.

UBUNTU SPECIFIC INSTRUCTIONS

With apt-get support, it is a little easier to do this in Ubuntu.

1. [Optional] Install Dev Tools by the following: sudo apt-get install build-essential

2. Download and install the following packages
    libwxgtk2.8-0, 
    libwxgtk2.8-dev

3. Do 
        >  mkdir buildgtk
        >  cd buildgtk
        >  ../configure --with-wx-config=/usr/lib/wx/config/gtk2-unicode-release-2.8
        >  make
        >  cp mmex ../runtime
        
   trunk/runtime/mmex/mmex should run Money Manager Ex.


REGENERATING AUTOMAKE FILES

If for some reason, you need to regenerate AutoMake Makefile.in.
This might be required if you are adding new source files to MMEX.

1. Download and install Bakefile (>=0.2.1), www.bakefile.org.
2. Make sure all .bkl files in mmex/build/bakefiles is updated if necessary
3. Execute commands
   cd trunk/mmex
   ./bootstrap


CREATING A DEBIAN PACKAGE

Once the source tree is setup correctly.
Simply do a 
	dpkg -b mmex mmex.deb

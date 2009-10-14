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

Read build\msw\readme.txt for build instructions.


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
    
  1) Read the install directions in wxWidgets for how to do a build and then install the build on the machine. 
     For eg wxWidgets has been built with --with-gtk --disable-shared --enable-unicode
  2) After installation, wx-config will copied to the /usr/bin and can now correctly identify 
     the command line params required.

2. Navigate to trunk/mmex

   Do 
        >  mkdir obj
        >  cd obj
        >  ../configure --with-wx-config=[LOCATION of WX-CONFIG, f.e. /usr/local/lib/wx/config/gtk2-unicode-release-2.8]
        >  make
        >  make check
        >  cp mmex ../runtime
        
   trunk/runtime/mmex/mmex should run Money Manager Ex.


UBUNTU SPECIFIC INSTRUCTIONS

With apt-get support, it is a little easier to do this in Ubuntu.

1. [Optional] Install Dev Tools by the following: sudo apt-get install build-essential

2. Download and install the following packages
    libwxgtk2.8-0, 
    libwxgtk2.8-dev

3. Execute step #2 from generic instructions.


REGENERATING AUTOMAKE FILES

If for some reason, you need to regenerate AutoMake files.
This might be required if you are adding new source files to MMEX.

1. Download and install Bakefile, www.bakefile.org.
2. Make sure all .bkl files in source tree are updated if necessary
3. Do
	>  cd trunk/mmex
	>  chmod a+rx bootstrap
        >  ./bootstrap


CREATING A DEBIAN PACKAGE

Once the source tree is setup correctly.
Simply do a 
	dpkg -b mmex mmex.deb

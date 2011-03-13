# How to build (and install) .deb package
#
# Note: Build support for 32 bit and 64 bit.
#       The control file is set up for i386 (32 bit Version)
#
# Difference in Control file: Line 5
#       32bit:= Architecture: i386
#       64bit:= Architecture: amd64
#
# To build the amd64 (64 bit version), either edit the control file or 
#    rename the file: control to control.i386 then rename control.amd64 to control
#

cd trunk/mmex
./configure --prefix=$HOME/build/mmex/usr
make install

mkdir ~/build/mmex/DEBIAN
cp setup/linux/debian/control setup/linux/debian/debian-binary  ~/build/mmex/DEBIAN

mkdir -p ~/build/mmex/usr/share/applications
cp resources/mmex.desktop ~/build/mmex/usr/share/applications

mkdir -p ~/build/mmex/usr/share/icons/hicolor/scalable/apps
cp graphics/mmex.svg ~/build/mmex/usr/share/icons/hicolor/scalable/apps

mkdir -p ~/build/mmex/usr/share/man/man1
cp doc/mmex.1.gz ~/build/mmex/usr/share/man/man1

cd ~/build

strip mmex/usr/bin/mmex
fakeroot dpkg-deb -b mmex

lintian mmex.deb
# check errorrs against other lintian deb 

# install the package (Have you backed up your databases?)

dpkg -i mmex.deb
mmex&

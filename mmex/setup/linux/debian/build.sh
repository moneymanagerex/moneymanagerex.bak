#!/bin/bash
#
# Note: Build support for 32 bit and 64 bit.
#
# Difference in Control file: Line 13
#       32bit:= Architecture: i386
#       64bit:= Architecture: amd64
#
# To build the amd64 (64 bit version), set variable below to amd64
#

# Specify system Architecture  ("i386" or "amd64")
ARCHITECTURE="i386"

# Specify the build version of mmex
MMEX_VERSION="0.9.8.0"

cd ../../../
./bootstrap
./configure --prefix=$HOME/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr
make install

mkdir ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/DEBIAN
echo "Package: mmex
Version: $MMEX_VERSION
Section: misc
Priority: optional
Architecture: $ARCHITECTURE
Depends: libc6 (>= 2.3.5-1), libwxgtk2.8-0 (>=2.8.0)
Installed-Size: 8600
Maintainer: Nikolay Akimov <vomikan@mail.ru>
Description: Simple to use financial management software
 Money  Manager  Ex (MMEX) is a free, open-source,
 cross-platform, easy-to-use personal finance software.
 It primarily helps organize one's finances and keeps
 track of where, when and how the money goes.
 MMEX includes all the basic features that 90% of users
 would want to see in a personal finance application.
 The design goals are to concentrate  on  simplicity
 and  user friendliness - something one can use everyday." > ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/DEBIAN/control

cp setup/linux/debian/debian-binary  ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/DEBIAN/

mkdir -p ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr/share/applications
cp resources/mmex.desktop ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr/share/applications/

mkdir -p ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr/share/icons/hicolor/scalable/apps
cp graphics/mmex.svg ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr/share/icons/hicolor/scalable/apps/

mkdir -p ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr/share/man/man1
cp doc/mmex.1.gz ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr/share/man/man1/

rm ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr/share/mmex/po/*.po

cd ~/build

strip mmex-$MMEX_VERSION-$ARCHITECTURE/usr/bin/mmex
fakeroot dpkg-deb -b mmex-$MMEX_VERSION-$ARCHITECTURE

lintian mmex-$MMEX_VERSION-$ARCHITECTURE.deb
# check errors against other lintian deb 

# install the package (Have you backed up your databases?)

#sudo dpkg -i mmex.deb
#mmex&

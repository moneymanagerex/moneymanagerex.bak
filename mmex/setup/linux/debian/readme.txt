#!/bin/bash
# How to build (and install) .deb package
#
# Note: Build support for 32 bit and 64 bit.
#
# Difference in Control file: Line 5
#       32bit:= Architecture: i386
#       64bit:= Architecture: amd64
#
# To build the amd64 (64 bit version), set varianle below to x64
#
ARCHITECTURE="x86"

# The corresponding version of mmex must be specified below
MMEX_VERSION="0.9.8.0"

cd ../../../
./configure --prefix=$HOME/build/mmex-$MMEX_VERSION-$ARCHITECTURE/usr
make install

mkdir ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/DEBIAN
cp setup/linux/debian/control.$ARCHITECTURE ~/build/mmex-$MMEX_VERSION-$ARCHITECTURE/DEBIAN/control
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

lintian mmex.deb
# check errorrs against other lintian deb 

# install the package (Have you backed up your databases?)

#dpkg -i mmex.deb
#mmex&

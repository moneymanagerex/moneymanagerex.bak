How to build .deb package

cd trunk/mmex
./configure --prefix=$HOME/build/mmex/usr
make install
mkdir ~/build/mmex/DEBIAN
cp setup/linux/debian/control setup/linux/debian/debian-binary  ~/build/mmex/DEBIAN
cp resources/mmex.desktop ~/build/mmex/usr/share/applications
cp graphics/mmex.svg ~/build/mmex/usr/share/icons/hicolor/scalable/apps
cd ~/build
fakeroot dpkg-deb -b mmex
lintian mmex.deb

dpkg -i mmex.deb
mmex&

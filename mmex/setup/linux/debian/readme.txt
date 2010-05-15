How to build .dep package

cd trunk/mmex
./configure --prefix=$HOME/build/mmex/usr
make install
mkdir ~/build/mmex/DEBIAN
cp setup/linux/debian/control setup/linux/debian/debian-binary  ~/build/mmex/DEBIAN
cd ~/build
dpkg-deb -b mmex
dpkg -i mmex.dep
mmex&

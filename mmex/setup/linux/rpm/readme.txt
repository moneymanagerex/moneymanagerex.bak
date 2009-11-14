#rpmdev-setuptree

cp mmex.spec ~/rpmbuild/SPECS
cp mmex-X.X.X.X.tar.gz ~/rpmbuild/SOURCES

cd ~/rpmbuild/SPECS
rpmbuild -ba mmex.spec

# rpmdev-wipetree

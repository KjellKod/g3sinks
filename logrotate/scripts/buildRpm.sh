#!/bin/bash
set -e

if [[ $# -ne 2 ]] ; then
    echo 'Usage:  sh buildRpm <$VERSION> <PATH_TO_LIBRARY_ROOT>'
    exit 0
fi
 

VERSION="$1"
g3LIBRARY_PATH="$2"
PACKAGE=g3logrotate

PWD=`pwd`
CWD=$PWD/$PACKAGE
DISTDIR=$CWD/dist/$PACKAGE
PATH=$PATH:/usr/local/probe/bin:$PATH

rm -rf ~/rpmbuild
rpmdev-setuptree
cp packaging/$PACKAGE.spec ~/rpmbuild/SPECS
rm -f $PACKAGE-$VERSION.tar.gz
cd ..
tar czf $PACKAGE-$VERSION.tar.gz logrotate 3rdparty
cp $PACKAGE-$VERSION.tar.gz ~/rpmbuild/SOURCES
cd ~/rpmbuild/
rpmbuild --define="version ${VERSION}"  --define="install_root ${g3LIBRARY_PATH}"  -v -bb --target=x86_64 ~/rpmbuild/SPECS/$PACKAGE.spec

# Copy the artifacts to the local distribution directory
rm -rf $DISTDIR
mkdir -p $DISTDIR/include/
cp -r ~/rpmbuild/BUILD/$PACKAGE/src/*.h $DISTDIR/include
mkdir -p $DISTDIR/lib/
cp -r ~/rpmbuild/BUILD/$PACKAGE/*.so $DISTDIR/lib
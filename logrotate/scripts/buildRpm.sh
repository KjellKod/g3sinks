#!/bin/bash
set -e

PACKAGE=logrotate
VERSION=1.0

PWD=`pwd`
CWD=$PWD/$PACKAGE
DISTDIR=$CWD/dist/$PACKAGE
PATH=$PATH:/usr/local/probe/bin:$PATH

rm -rf ~/rpmbuild
rpmdev-setuptree
cp packaging/$PACKAGE.spec ~/rpmbuild/SPECS
rm -f $PACKAGE-$VERSION.tar.gz

tar czf $PACKAGE-$VERSION.tar.gz ./* ../3rdparty
cp $PACKAGE-$VERSION.tar.gz ~/rpmbuild/SOURCES
cd ~/rpmbuild
rpmbuild -v -bb --target=x86_64 ~/rpmbuild/SPECS/$PACKAGE.spec

# Copy the artifacts to the local distribution directory
rm -rf $DISTDIR
mkdir -p $DISTDIR/include/
cp -r ~/rpmbuild/BUILD/$PACKAGE-$VERSION/g3sinks/logrotate/src/*h $DISTDIR/include
mkdir -p $DISTDIR/lib/
cp -r ~/rpmbuild/BUILD/$PACKAGE-$VERSION/g3sinks/logrotate/build/*so $DISTDIR/lib
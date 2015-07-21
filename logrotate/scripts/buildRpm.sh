#!/bin/bash
set -e

PACKAGE=g3_logrotate
VERSION=1.0

PWD=`pwd`
CWD=$PWD/$PACKAGE
DISTDIR=$CWD/dist/$PACKAGE
PATH=$PATH:/usr/local/probe/bin:$PATH

rm -rf ~/rpmbuild
rpmdev-setuptree
cp packaging/$PACKAGE.spec ~/rpmbuild/SPECS
rm -f $PACKAGE-$VERSION.tar.gz
tar czf $PACKAGE-$VERSION.tar.gz ./*
cp $PACKAGE-$VERSION.tar.gz ~/rpmbuild/SOURCES
cd ~/rpmbuild
rpmbuild -v -bb --target=x86_64 ~/rpmbuild/SPECS/$PACKAGE.spec
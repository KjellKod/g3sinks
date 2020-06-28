#o!/bin/bash
set -e

if [[ $# -ne 2 ]] ; then
    echo 'Usage:  sh buildRpm <$VERSION> <PATH_TO_LIBRARY_ROOT>'
    # example: cd g3sinks; cd logrotate; sh scripts/buildRpm.sh 3 /usr/local/xyz/  
    #          (lib, bin must exist under xyz)
    exit 0
fi
 

VERSION="$1"
g3LIBRARY_PATH="$2"
PACKAGE=g3logrotate

PWD=`pwd`
CWD=$PWD/$PACKAGE
DISTDIR=$CWD/dist/$PACKAGE
PATH=$PATH:/usr/local/bin:$PATH

rm -rf ~/rpmbuild
rpmdev-setuptree
cp packaging/$PACKAGE.spec ~/rpmbuild/SPECS
rm -f $PACKAGE-$VERSION.tar.gz
cd ..
tar czf $PACKAGE-$VERSION.tar.gz logrotate 3rdparty
cp $PACKAGE-$VERSION.tar.gz ~/rpmbuild/SOURCES
cd ~/rpmbuild/
rpmbuild --define="version ${VERSION}"  --define="install_root ${g3LIBRARY_PATH}"  -v -bb --target=x86_64 ~/rpmbuild/SPECS/$PACKAGE.spec



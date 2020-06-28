#!/usr/bin/env bash
pwd=`pwd`

function usage () {
  echo "-h : display help & exit"
  echo "-skipg3log : skip building g3log"
}

install_g3log=true
while [ "$1" != "" ]; do
  case $1 in
    -skipg3log ) install_g3log=false
                 ;;
    -h ) usage
         exit
         ;;
    * ) usage
        exit 1
  esac
  shift
done

set -ev
set -x

curdir=${PWD##*/}
if [ $curdir != "g3sinks" ]; then
  echo "not in the right directory. Should be in g3sinks. abord."
  exit 1
fi


function install_g3log () {
  rm -rf g3log-install g3log build
  mkdir g3log-install && cd g3log-install
  git clone https://github.com/KjellKod/g3log.git -b master
  cd g3log
  mkdir -p  build_travis
  cd build_travis
  cmake  -DADD_G3LOG_UNIT_TEST=ON ..
  make -j && sudo make install
  }

if [ "$install_g3log" == true ]; then
  install_g3log
else
  echo "SKIPPING g3log build."
fi

# Build g3sinks
cd $pwd
mkdir build 

cd build && cmake -DBUILD_TEST=ON ..

make -j
echo "FINISHED BUILDING LOGROTATE & SYSLOG"

./logrotate/UnitTestRunner
./syslog/example/syslog_g3log_example || true
echo "FINISHED SYSLOG EXAMPLE"

./snippets/g3log_snippets_file_example
echo "FINISHED FILE DESCRIPTOR EXAMPLE"



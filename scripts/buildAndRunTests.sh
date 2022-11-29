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


# Travis list libraries in case of an update
# ls -alh /usr/lib/x86_64-linux-gnu/ || true 
# ls -alh /usr/local/lib || true

function install_g3log () {
  rm -rf g3log-install g3log build
  mkdir g3log-install && cd g3log-install
  # TEMPORARY normally this should be master
  git clone https://github.com/KjellKod/g3log.git
  cd g3log
  mkdir -p  build_travis
  cd build_travis
  cmake -DADD_G3LOG_UNIT_TEST=OFF -DG3_SHARED_LIB=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local -DCMAKE_INSTALL_PREFIX=/usr/local ..
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
cd build 
cmake -DCMAKE_BUILD_TYPE=Release -DCHOICE_BUILD_STATIC=ON -DCMAKE_PREFIX_PATH=/usr/local -DCMAKE_INSTALL_PREFIX=/usr/local  .. 
cmake --build . 
ctest -V 
sudo make install

# run one snipped example
cd examples
[ -f example_coloredcout ] && ./example_coloredcout || true
[ -f example_logrotate_and_filter ] && ./example_logrotate_and_filter || true


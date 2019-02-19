#!/bin/bash

set -ev
set -x

pwd=`pwd`
mkdir g3log-install && cd g3log-install
git clone https://github.com/KjellKod/g3log.git -b master
cd g3log
unzip -o 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest
mkdir -p  build_travis
cd build_travis
cmake  -DADD_G3LOG_UNIT_TEST=ON ..
make -j && make install


# Build g3sinks
cd $pwd
cd 3rdparty && unzip gtest-1.7.0.zip
cd $pwd

#logrotate
cd logrotate && mkdir build && cd build && cmake -DADD_LOGROTATE_UNIT_TEST=ON ..
make -j && ./UnitTestRunner

#syslog
cd $pwd
cd syslog && mkdir build && cd build && cmake .. && make

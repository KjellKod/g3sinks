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
make -j && sudo make install


# Build g3sinks
cd $pwd
cd 3rdparty && unzip gtest-1.7.0.zip
cd $pwd

#logrotate
echo "BUILD LOGROTATE SINK AND RUN TESTS"
cd logrotate && mkdir build && cd build && cmake -DADD_LOGROTATE_UNIT_TEST=ON ..
make -j && ./UnitTestRunner
echo "FINISHED LOGROTATE BUILD AND TESTS"

#syslog
echo "BUILD SYSLOG SINK AND RUN EXAMPLE"
cd $pwd
cd syslog && mkdir build && cd build && cmake .. && make
./example/syslog_g3log_example || true
echo "FINISHED SYSLOG EXAMPLE"

FROM ubuntu:16.04

# Install
RUN apt-get update -y
RUN apt-get install -y apt-utils | true
RUN apt-get install -y software-properties-common | true
RUN apt-get install -y python-software-properties
RUN apt-get update -y
RUN add-apt-repository -y ppa:jonathonf/gcc-7.1
RUN apt-get update -y 
RUN apt-get install -y cmake software-properties-common git make
RUN apt-get install -y gcc-7 g++-7 
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 90
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 90 

# Build Preparation
RUN mkdir -p /src/
RUN mkdir -p /src/build/
RUN git clone https://github.com/KjellKod/g3log.git
RUN cd g3log/3rdParty/gtest
RUN cd unzip gtest*.zip
RUN cd ../../ 
RUN mkdir build && cd build
RUN make install


# Build g3sinks
# Cleanup
RUN ../../../
RUN rm -rf g3log

# Build
WORKDIR /src/build/
COPY . /src/
RUN cd /src
RUN cd 3rdparty
RUN unzip *zip
RUN cd ../logrotate
RUN mkdir build && cd build
RUN cmake -DADD_LOGROTATE_UNIT_TEST=ON ..
RUN make -j
RUN ./UnitTestRunner
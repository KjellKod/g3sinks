Name:          g3logrotate
Version:       1.0
Release:       1%{?dist}
Summary:       An rotating file sink for G3log
Group:         Development/Tools
License:       UNLICENSE
BuildRequires: probecmake >= 2.8, probezlib
Requires: g3log, boost >= 1.55.0, probezlib
URL: https://github.com/KjellKod/g3sinks
ExclusiveArch: x86_64

%description
A file sink with rotating files, used with g3log

%prep
cd ~/rpmbuild/BUILD
rm -rf %{name}*
mkdir %{name}-%{version}
cd %{name}-%{version}
tar xzf ~/rpmbuild/SOURCES/%{name}-%{version}.tar.gz
if [ $? -ne 0 ]; then
   exit $?
fi

%build
# SKIP_BUILD_RPATH, CMAKE_SKIP_BUILD_RPATH, 
PATH=/usr/local/probe/bin:$PATH
cd %{name}-%{version}
ls > /tmp/out
cd 3rdparty
unzip -u gtest-1.7.0.zip
cd ..
mkdir -p build
cd build
if [ -z "$BOOST_DIR" ] ; then
   BOOST_BASE_DIR=/usr/local/boost
else
   BOOST_BASE_DIR=$BOOST_DIR
fi

rm -f  CMakeCache.txt

/usr/local/probe/bin/cmake -DCMAKE_CXX_COMPILER_ARG1:STRING=' -fPIC -Ofast -m64 -Wl,-rpath -Wl,. -Wl,-rpath -Wl,/usr/local/probe/lib -Wl,-rpath -Wl,/usr/local/probe/lib64 ' -DCMAKE_BUILD_TYPE:STRING=Release -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_CXX_COMPILER=/usr/local/probe/bin/g++ ..

make -j6
./UnitTestRunner
mkdir -p $RPM_BUILD_ROOT/usr/local/probe/lib
cp *.so $RPM_BUILD_ROOT/usr/local/probe/lib
rm $RPM_BUILD_ROOT/usr/local/probe/lib/libgtest_170_lib.so
mkdir -p $RPM_BUILD_ROOT/usr/local/probe/include
cp -r ../src/g3sinks $RPM_BUILD_ROOT/usr/local/probe/include

%post

%preun

%postun

%files
%defattr(-,dpi,dpi,-)
/usr/local/probe/lib
/usr/local/probe/include

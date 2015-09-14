Name:          g3logrotate
Version:       %{version}
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
PATH=çƒ/bin:$PATH
cd %{name}-%{version}
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

%{install_root}/bin/cmake -DCMAKE_CXX_COMPILER_ARG1:STRING=' -fPIC -Ofast -m64 -Wl,-rpath -Wl,. -Wl,-rpath -Wl,%{install_root}/lib -Wl,-rpath -Wl,%{install_root}/lib64 ' -DCMAKE_BUILD_TYPE:STRING=Release -DBUILD_SHARED_LIBS:BOOL=ON -DG3_LIBRARY_PATH=%{install_root}/lib -DG3_HEADER_PATH=%{install_root}/include -DCMAKE_CXX_COMPILER=%{install_root}/bin/g++ ..

make -j6
./UnitTestRunner
mkdir -p $RPM_BUILD_ROOT/%{install_root}/lib
cp *.so $RPM_BUILD_ROOT/%{install_root}/lib
rm $RPM_BUILD_ROOT/%{install_root}/lib/libgtest_170_lib.so
mkdir -p $RPM_BUILD_ROOT/%{install_root}/include
cp -r ../src/g3sinks $RPM_BUILD_ROOT/{install_root}/include

%post

%preun

%postun

%files
%defattr(-,dpi,dpi,-)
%{install_root}/lib
%{install_root}/include

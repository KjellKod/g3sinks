message(
  STATUS
    "\n
  ======== INSTALLATION =========
  ===============================
      CMake build and install example from: g3sinks/build directory

            OSX/Linux:
                cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local -DCMAKE_INSTALL_PREFIX=/usr/local ..
                cmake --build . --config Release
                make install

            Windows:
                # regardless of your VS version make sure to specify the 64 or 32 bit - it should be the same target platform for g3log of course
                cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release -CDHOICE_BUILD_STATIC=ON -DCMAKE_PREFIX_PATH="c:\\Local" -DCMAKE_INSTALL_PREFIX="c:\\Local" ..
                # to just build it and run tests
                cmake --build . --config Release
                # Unfortunately locating the DLL is a pain on Windows.
                # Make sure the PATH to g3log and boost, zlib, etc are set.
                # In the example above the dlls for g3log and zlib exists in c:\\Local\\bin
                set PATH=c:\\Local\\bin;%PATH%
                cmake test -V
                # to build and install it
                cmake --build . --target INSTALL --config Release

      create installation package 
              All platforms: cpack -C Release 
              OSX/Linux: make package 

      install after \"make package\"
              OSX:   sh g3sinks-2.1.0-Darwin.sh
              Linux: sudo dpkg -i g3sinks-***Linux.deb
              Windows:

      Remove options:
              OSX:
              Linux: sudo dpkg -r g3sinks
              Windows:

      View installation targets:
            OSX: tar -ztvf g3sinks-2.1.0-Darwin.tar.gz
            Linux: sudo dpkg --contents g3sinks-***Linux.deb
            Windows:

      Run tests:
              ctest
              ctest -V
              
              # OSX/Linux For ctest with color set 'GTEST_COLOR=1'
              GTEST_COLOR=1 ctest -V
              make test

      Run examples:
             cmake examples
  ===============================\n\n")

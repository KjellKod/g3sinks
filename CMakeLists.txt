cmake_minimum_required(VERSION 3.2)

project(g3sinks)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# Function definitions that are used in various places
include(Functions.cmake)

# option description
include(Options.cmake)
include(Version.cmake)
include(CTest)

# globals
set(LOGRATATE_INCLUDE_DIR ${g3sinks_SOURCE_DIR}/sink_logrotate/src)
set(TEST_MAIN ${g3sinks_SOURCE_DIR}/3rdparty/test_main.cpp)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
  if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX /usr/local)
  endif()

  set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX})
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  message("Install rpath location: ${CMAKE_INSTALL_RPATH}")
endif()

if(CHOICE_BUILD_TESTS OR CHOICE_BUILD_EXAMPLES)
  # Download and add gtest so the sinks can use it for testing
  include(gtest.cmake)
endif()

if(NOT CMAKE_PREFIX_PATH)
  set(CMAKE_PREFIX_PATH /usr/local)
endif()

# Global find g3log
# =========================
include_directories(${CMAKE_PREFIX_PATH}/include)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_library(G3LOG_LIBRARY g3log PATHS /usr/local/lib ${CMAKE_PREFIX_PATH}
                                         ${CMAKE_INSTALL_PREFIX})
else()
  # APPLE and WIN32 use find_package library"
  find_package(G3LOG)
  message(
    "g3log package:
        found: ${G3LOG_FOUND}
        include dir: ${G3LOG_INCLUDE_DIRS}
        libraries: ${G3LOG_LIBRARY}")
endif()

if(NOT G3LOG_LIBRARY)
  message(FATAL "Could not locate G3LOG_LIBRARY: ${G3LOG_LIBRARY}")
endif()
# Fix for find_library, this will otherwise be set by find_package
if(NOT G3LOG_INCLUDE_DIRS)
  set(G3LOG_INCLUDE_DIRS ${CMAKE_PREFIX_PATH}/include)
endif()

include_directories(${G3LOG_INCLUDE_DIRS})
message("g3log library: ${G3LOG_LIBRARY}")
message("g3log include should be: ${G3LOG_INCLUDE_DIRS}")

# Logging Sinks
# =============================
# logrotate, logrotatewithfilter
if(CHOICE_SINK_LOGROTATE)
  add_subdirectory(sink_logrotate)
endif()

# Linux, OSX: syslog
if(CHOICE_SINK_SYSLOG)
  verifySyslogDependencies(SYSLOG_SINK_ERROR)
  if(SYSLOG_SINK_ERROR)
    message(STATUS "${SYSLOG_SINK_ERROR}")
  else()
    message(STATUS "syslog.h is found. Building g3syslog")
    add_subdirectory(sink_syslog)
  endif()
endif()

# Linux, OSX so far but more snippets for windows should be added 
if(CHOICE_SINK_SNIPPETS)
  # header only sinks that are simple
  add_subdirectory(sink_snippets)
endif()

# Setup TEST and EXAMPLES
if(CHOICE_BUILD_TESTS)
  add_subdirectory(test)
endif()

if(CHOICE_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

message(
  STATUS
    "\n
  =====================CLEAN IT ============================
  ===========================================================
  Please use 'make clean-cmake' before next cmake generation.
  It is a good idea to purge your build directory of CMake
  generated cache files
  ===========================================================
       ")
add_custom_target(clean-cmake COMMAND ${CMAKE_COMMAND} -P
                                      ${g3sinks_SOURCE_DIR}/CleanAll.cmake)

# print all CHOIC options. Ref Options.cmake
message(
  STATUS
    "\n
  ========== OPTIONS ============
  ===============================
  ")
dump_cmake_variables(CHOICE)
message(STATUS "===============================")

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
                # Make sure the PATH to g3log and zlib are set.
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

include(CPackLists.txt)

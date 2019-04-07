# CMakeList.txt : CMake project for g3log_tracelogging, include source and define
# project specific logic here.
#
#g3log_tracelogging is a sink that enables logging through Microsoft TraceLogging
#
# To build and install, run the following commands
# 
# cmake -DCMAKE_BUILD_TYPE=<Debug|Release> -DCMAKE_PREFIX_PATH=<path to install folder> -DG3LOG_BASE_DIR=<path to base folder of g3log git clone> -DCMAKE_INSTALL_PREFIX=<path to install folder> ..
# cmake --build . --target INSTALL --config <Release/Debug>
#
#
# Example
# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=c:/usr/local -DG3LOG_BASE_DIR=C:/Projects/third-party/forks/g3log -DCMAKE_INSTALL_PREFIX=c:/usr/local ..
# cmake --build . --target INSTALL --config Debug
#
# This will build in Debug mode and install the include files to c:/usr/local/include/g3log_tracelogging/
# the lib to c:/usr/local/lib/ and the g3log_traceloggingConfig.make file to c:/usr/local/lib/cmake/g3log_tracelogging/
#
#
cmake_minimum_required (VERSION 3.8...3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

list(APPEND CMAKE_CXX_SOURCE_FILE_EXTENSIONS c)


project(g3log_tracelogging VERSION 0.1
                     DESCRIPTION "Build for g3log that supports Microsoft UWP and HoloLens projects as well as Magic Leap Projects"
                     LANGUAGES CXX)
                     
message( STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}" )
message( STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}" )
message( STATUS "CMAKE_LIBRARY_ARCHITECTURE: ${CMAKE_LIBRARY_ARCHITECTURE}")
message( STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )

# search for TraceLoggingProvider.h
include(CheckIncludeFileCXX)
check_include_file_cxx("TraceLoggingProvider.h" HAVE_TRACELOGGINGPROVIDER_H)
if(NOT ${HAVE_TRACELOGGINGPROVIDER_H})
	message(FATAL_ERROR "Could not find TraceLoggingProvider.h.")
else()
  message( STATUS "Verified include file TraceLoggingProvider.h" )
endif()

# search for g3log
find_package(g3logger REQUIRED NO_MODULE)
# add the missing target if needed
if (NOT TARGET g3logger)
    add_library(g3logger SHARED IMPORTED)
    find_package(Threads REQUIRED)
    set_target_properties(g3logger PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${G3LOG_INCLUDE_DIRS}"
        IMPORTED_LOCATION "${G3LOG_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES  Threads::Threads
        )
endif()


message( STATUS "G3LOG_INCLUDE_DIRS: ${G3LOG_INCLUDE_DIRS}" )
message( STATUS "G3LOG_LIBRARY: ${G3LOG_LIBRARY}" )
message( STATUS "G3LOG_BASE_DIR: ${G3LOG_BASE_DIR}" )
set(G3LOG_SRC_DIR="${G3LOG_BASE_DIR}/src")
message( STATUS "G3LOG_SRC_DIR: ${G3LOG_SRC_DIR}" )

# Add source to this project's executable.
add_library ( g3log_tracelogging
              STATIC
              "include/g3log_tracelogging.h"
              "${G3LOG_SRC_DIR}/crashhandler_windows.cpp"
              "${G3LOG_SRC_DIR}/filesink.cpp"
              "${G3LOG_SRC_DIR}/g3log.cpp"
              "${G3LOG_SRC_DIR}/logcapture.cpp"
              "${G3LOG_SRC_DIR}/loglevels.cpp"
              "${G3LOG_SRC_DIR}/logmessage.cpp"
              "${G3LOG_SRC_DIR}/logworker.cpp"
              "${G3LOG_SRC_DIR}/stacktrace_windows.cpp"
              "${G3LOG_SRC_DIR}/time.cpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/active.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/atomicbool.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/crashhandler.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/filesink.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/future.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/g3log.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/logcapture.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/loglevels.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/logmessage.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/logworker.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/moveoncopy.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/shared_queue.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/sink.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/sinkhandle.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/sinkwrapper.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/stacktrace_windows.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/stlpatch_future.hpp"
              "${G3LOG_INCLUDE_DIRS}/g3log/time.hpp"
              "${G3LOG_BASE_DIR}/build/include/g3log/generated_definitions.hpp")

target_include_directories(g3log_tracelogging PUBLIC "${G3LOG_INCLUDE_DIRS}")

if(CMAKE_SYSTEM MATCHES "Windows")
  message( STATUS "Running on Windows" )
  target_include_directories(g3log_tracelogging PUBLIC "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17763.0/shared")
endif(CMAKE_SYSTEM MATCHES "Windows")

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "g3log_tracelogging.lib")

target_compile_definitions(g3log_tracelogging
    PRIVATE _UNICODE UNICODE _WINDOWS WIN32 _DEBUG DISABLE_VECTORED_EXCEPTIONHANDLING
    PUBLIC HOLO)

set_target_properties(g3log_tracelogging PROPERTIES CXX_STANDARD 14)

set_target_properties(g3log_tracelogging PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(g3log_tracelogging PROPERTIES PUBLIC_HEADER "include/g3log_tracelogging.h")

install(TARGETS g3log_tracelogging 
          ARCHIVE DESTINATION "lib"
          PUBLIC_HEADER DESTINATION "include/g3log_tracelogging")
          
          
message( STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}" )

INSTALL( FILES ${PROJECT_SOURCE_DIR}/cmake/g3log_traceloggingConfig.cmake
            DESTINATION lib/cmake/g3log_tracelogging)
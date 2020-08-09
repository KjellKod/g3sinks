cmake_minimum_required(VERSION 3.2)

project(g3sinks)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# Function definitions that are used in various places
include(Functions.cmake)

# option description
include(Options.cmake)
include(Version.cmake)
include(CTest)

# globals
set(LOGRATATE_INCLUDE_DIR ${g3sinks_SOURCE_DIR}/logrotate/src)
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

# Download and add gtest so the sinks can use it for testing
include(gtest.cmake)

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
if(CHOICE_logrotate)
  add_subdirectory(g3logrotate)
endif()

# Linux, OSX: syslog
#if(CHOICE_syslog)
#  verifySyslogDependencies(SYSLOG_SINK_ERROR)
#  if(SYSLOG_SINK_ERROR)
#    message(STATUS "${SYSLOG_SINK_ERROR}")
#  else()
#    message(STATUS "syslog.h is found. Building g3syslog")
#    add_subdirectory(syslog)
#  endif()
#endif()

# Linux, OSX so far but more snippets for windows should be added 
#if(CHOICE_snippets)
#  # header only sinks that are simple
#  add_subdirectory(snippets)
#endif()

# Setup TEST and EXAMPLES
#if(CHOICE_BUILD_TESTS)
#  add_subdirectory(test)
#endif()

#if(CHOICE_BUILD_EXAMPLES)
#  add_subdirectory(examples)
#endif()


# ================== Setup Installation ============================

install(
  EXPORT g3sinksTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/g3sinks
  NAMESPACE g3sinks::
  FILE g3sinksTargets.cmake 
  )

include(CMakePackageConfigHelpers)
configure_package_config_file( 
  "Config.cmake.in" 
  "g3sinksConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/g3sinks
  PATH_VARS
    CMAKE_INSTALL_LIBDIR
  )

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/g3sinksConfigVersion.cmake
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion
  )

### Install Config and ConfigVersion files
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/g3sinksConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/g3sinksConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/g3sinks"
  )





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

# include (HelpMessage.cmake)
#include(CPackLists.txt)

# CMakeList.txt : CMake project for g3log_tracelogging, include source and define
# project specific logic here.
#
# g3log_tracelogging is a sink that enables logging through Microsoft TraceLogging
# To build and install, run the following commands
# 
# cmake -DCMAKE_BUILD_TYPE=<Debug|Release> -DCMAKE_PREFIX_PATH=<path to install folder> -DCMAKE_INSTALL_PREFIX=<path to install folder> ..
# cmake --build . --target INSTALL --config <Release/Debug>
#
#
# Example
# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=c:/usr/local -DCMAKE_INSTALL_PREFIX=c:/usr/local ..
# cmake --build . --target INSTALL --config Debug
#
# This will build in Debug mode and install the include files to c:/usr/local/include/g3log_tracelogging/,
# the g3log_traceloggingConfig.make, g3log_traceloggingConfigVersion.cmake and g3log_traceloggingTargets.cmake files
# to c:/usr/local/lib/cmake/g3log_tracelogging/ and the README.md file to C:/usr/local/share/doc/g3log_tracelogging/
#
#
cmake_minimum_required (VERSION 3.8...3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# detect if g3log_tracelogging is being included as a subdirectory
if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

project(g3log_tracelogging VERSION 0.1
                     DESCRIPTION "Sink for g3log that supports Microsoft UWP and HoloLens projects"
                     LANGUAGES CXX)
                     
message( STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )
message( STATUS "CMAKE_CONFIGURATION_TYPES: ${CMAKE_CONFIGURATION_TYPES}" )
message( STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}" )
message( STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}" )
message( STATUS "CMAKE_LIBRARY_ARCHITECTURE: ${CMAKE_LIBRARY_ARCHITECTURE}")
message( STATUS "PROJECT_NAME: ${PROJECT_NAME}")

option(G3LOG_TRACELOGGING_INSTALL_DOCS "Install documentation alongside library" ON)

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
  message( STATUS "NOT TARGET g3loggger" )
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

# Add g3log_tracelogging as an INTERFACE only library
add_library ( g3log_tracelogging
              INTERFACE)            

target_include_directories(g3log_tracelogging
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# provide a namespaced alias for clients to 'link' against if g3log_tracelogging is included as a sub-project
add_library(g3log_tracelogging::g3log_tracelogging ALIAS g3log_tracelogging)

target_include_directories(g3log_tracelogging INTERFACE "${G3LOG_INCLUDE_DIRS}")
target_link_libraries(g3log_tracelogging INTERFACE ${G3LOG_LIBRARY})


if(CMAKE_SYSTEM MATCHES "Windows")
  message( STATUS "Running on Windows" )
  target_include_directories(g3log_tracelogging INTERFACE "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17763.0/shared")
endif(CMAKE_SYSTEM MATCHES "Windows")

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "g3log_tracelogging.lib")

target_compile_definitions(g3log_tracelogging
    INTERFACE _UNICODE UNICODE _WINDOWS WIN32 _DEBUG DISABLE_VECTORED_EXCEPTIONHANDLING HOLO)

if(NOT_SUBPROJECT)
  # set the config install folder
  set(G3LOG_TRACELOGGING_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/g3log_tracelogging")

  configure_package_config_file(
      ${CMAKE_CURRENT_LIST_DIR}/CMake/g3log_traceloggingConfig.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/g3log_traceloggingConfig.cmake
      INSTALL_DESTINATION
        ${G3LOG_TRACELOGGING_CMAKE_CONFIG_DESTINATION})

  # create and install an export set for catch target as g3log_tracelogging::g3log_tracelogging
  install(
    TARGETS
      g3log_tracelogging
    EXPORT
      g3log_traceloggingTargets
    DESTINATION
      ${CMAKE_INSTALL_LIBDIR})

  install(
    EXPORT
      g3log_traceloggingTargets
    NAMESPACE
      g3log_tracelogging::
    DESTINATION
      ${G3LOG_TRACELOGGING_CMAKE_CONFIG_DESTINATION})

      
  message( STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
  # By default, FooConfigVersion is tied to architecture that it was
  # generated on. Because g3log_tracelogging is header-only, it is arch-independent
  # and thus g3log_traceloggingConfigVersion should not be tied to the architecture
  # it was generated on.
  #
  # CMake does not provide a direct customization point for this in
  # `write_basic_package_version_file`, but it can be accomplished
  # indirectly by temporarily undefining `CMAKE_SIZEOF_VOID_P`.
  set(G3LOG_TRACELOGGING_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
  unset(CMAKE_SIZEOF_VOID_P)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/g3log_traceloggingConfigVersion.cmake"
    COMPATIBILITY
      SameMajorVersion
  )
  set(CMAKE_SIZEOF_VOID_P ${G3LOG_TRACELOGGING_CMAKE_SIZEOF_VOID_P})

  # install include files
  install(
  DIRECTORY
    "include/"
  DESTINATION
    "${CMAKE_INSTALL_INCLUDEDIR}/g3log_tracelogging")
    
  #install config files
  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/g3log_traceloggingConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/g3log_traceloggingConfigVersion.cmake"
    DESTINATION
      ${G3LOG_TRACELOGGING_CMAKE_CONFIG_DESTINATION})

  # Install documentation
  if(G3LOG_TRACELOGGING_INSTALL_DOCS)
    install(
      FILES
        README.md
      DESTINATION
        "${CMAKE_INSTALL_DOCDIR}/g3log_tracelogging")
  endif(G3LOG_TRACELOGGING_INSTALL_DOCS)

endif(NOT_SUBPROJECT)

add_subdirectory(example)

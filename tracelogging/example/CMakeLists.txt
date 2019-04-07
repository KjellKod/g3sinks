cmake_minimum_required (VERSION 3.8...3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(g3log_tracelogging_example VERSION 0.1
                     DESCRIPTION "g3log_tracelogging example"
                     LANGUAGES CXX)
                     
message( STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}" )
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
find_package(g3log_tracelogging REQUIRED NO_MODULE)
# add the missing target if needed
if (NOT TARGET g3log_tracelogging)
    add_library(g3log_tracelogging STATIC IMPORTED)
    find_package(Threads REQUIRED)
    set_target_properties(g3log_tracelogging PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${G3LOG_TRACELOGGING_INCLUDE_DIRS}"
        IMPORTED_LOCATION "${G3LOG_TRACELOGGING_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES  Threads::Threads
        )
endif()


message( STATUS "G3LOG_TRACELOGGING_INCLUDE_DIRS: ${G3LOG_TRACELOGGING_INCLUDE_DIRS}" )
message( STATUS "G3LOG_TRACELOGGING_LIBRARY: ${G3LOG_TRACELOGGING_LIBRARY}" )

# Add source to this project's executable.
add_executable ( g3log_tracelogging_example
              "src/tracelogging_example.cpp")

target_include_directories(g3log_tracelogging_example PRIVATE "${G3LOG_TRACELOGGING_INCLUDE_DIRS}")

target_link_libraries(g3log_tracelogging_example
 PRIVATE ${G3LOG_TRACELOGGING_LIBRARY})

#add_dependencies(g3log_tracelogging_example ${G3LOG_TRACELOGGING_LIBRARY})

if(CMAKE_SYSTEM MATCHES "Windows")
  message( STATUS "Running on Windows" )
  target_include_directories(g3log_tracelogging_example PUBLIC "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17763.0/shared")
  target_include_directories(g3log_tracelogging_example PUBLIC "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17134.0/um")
endif(CMAKE_SYSTEM MATCHES "Windows")

target_compile_definitions(g3log_tracelogging_example
    PRIVATE _UNICODE UNICODE _WINDOWS WIN32 _DEBUG DISABLE_VECTORED_EXCEPTIONHANDLING HOLO)

set_target_properties(g3log_tracelogging_example PROPERTIES CXX_STANDARD 14)

set_target_properties(g3log_tracelogging_example PROPERTIES CXX_EXTENSIONS OFF)
          
message( STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}" )
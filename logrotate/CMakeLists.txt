# =====================================================
#  Public Domain: Inspired by github.com/KjellKod/g3log/CMakeLists.cmake
#  2015, April 30, @author Kjell.Hedstrom
# =====================================================

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
ENABLE_LANGUAGE(CXX)
SET(CMAKE_BUILD_TYPE Release)

PROJECT (LogRotate)
SET(PROJECT_SRC ${LogRotate_SOURCE_DIR}/src)
SET(DIR_UNIT_TEST ${LogRotate_SOURCE_DIR}/test)
SET(DIR_3RDPARTY ${LogRotate_SOURCE_DIR}/../3rdparty)


MESSAGE(" PROJECT_SRC = : ${PROJECT_SRC}")
INCLUDE_DIRECTORIES(${PROJECT_SRC})
SET(ACTIVE_CPP0xx_DIR "Release")

# Load PATHS
INCLUDE (${LogRotate_SOURCE_DIR}/Options.cmake)
INCLUDE_DIRECTORIES(${G3_HEADER_PATH})

#Find all libraries

find_package(Boost COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
option(BOOST_ALL_DYN_LINK OFF)   # force dynamic linking for all libraries

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
find_library(G3LOG ${G3LOG_LIBRARY_TYPE} PATHS ${G3_LIBRARY_PATH})

IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
   MESSAGE("")
   MESSAGE("cmake for Clang ")
   SET(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++11 -stdlib=libc++ -Wunused -D_GLIBCXX_USE_NANOSLEEP")
   IF (${CMAKE_SYSTEM} MATCHES "FreeBSD-([0-9]*)\\.(.*)")
       IF (${CMAKE_MATCH_1} GREATER 9)
           set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
           set(PLATFORM_LINK_LIBRIES execinfo)
       ENDIF()
   ELSEIF (APPLE)
       set(PLATFORM_LINK_LIBRIES c++abi)
   ELSE()
       set(PLATFORM_LINK_LIBRIES rt c++abi)
   ENDIF()

ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   MESSAGE("")
   MESSAGE("cmake for GCC ")
   SET(PLATFORM_LINK_LIBRIES rt)
   SET(CMAKE_CXX_FLAGS " -Wall -Werror -rdynamic -Wunused -std=c++11 -pthread -D_GLIBCXX_USE_NANOSLEEP")
ENDIF()


# GENERIC STEPS
FILE(GLOB SRC_FILES ${PROJECT_SRC}/*.h ${PROJECT_SRC}/*.h ${PROJECT_SRC}/*.ipp ${PROJECT_SRC}/*.cpp) 


# Setup Library name
SET(LIBRARY_TO_BUILD g3logrotate)
MESSAGE("BOOST LIBRARIES: ${Boost_LIBRARIES}")
# Build the library
INCLUDE_DIRECTORIES(${PROJECT_SRC})

# see Options.cmake
IF(BUILD_STATIC)
   ADD_LIBRARY(${LIBRARY_TO_BUILD} ${SRC_FILES})
ELSE()
   ADD_LIBRARY(${LIBRARY_TO_BUILD} SHARED ${SRC_FILES})
ENDIF()

SET(${LIBRARY_TO_BUILD}_VERSION_STRING)
SET_TARGET_PROPERTIES(${LIBRARY_TO_BUILD} PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(${LIBRARY_TO_BUILD} ${TCMALLOC} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${G3LOG})



# Setup unit tests
INCLUDE (${LogRotate_SOURCE_DIR}/test/Test.cmake)

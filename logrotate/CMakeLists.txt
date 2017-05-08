# =====================================================
#  Public Domain: Inspired by github.com/KjellKod/g3log/CMakeLists.cmake
#  2015, April 30, @author Kjell.Hedstrom
# =====================================================

CMAKE_MINIMUM_REQUIRED (VERSION 3.1)
ENABLE_LANGUAGE(CXX)
SET(CMAKE_BUILD_TYPE Release)

PROJECT (LogRotate)
SET(PROJECT_SRC ${LogRotate_SOURCE_DIR}/src)
SET(DIR_UNIT_TEST ${LogRotate_SOURCE_DIR}/test)
SET(DIR_3RDPARTY ${LogRotate_SOURCE_DIR}/../3rdparty)
SET(G3LOG_LIBRARY g3logger)


MESSAGE(" PROJECT_SRC = : ${PROJECT_SRC}")
INCLUDE_DIRECTORIES(${PROJECT_SRC})
SET(ACTIVE_CPP0xx_DIR "Release")


SET(MAJOR_VERSION 1)
IF (${VERSION}.x STREQUAL ".x")
   MESSAGE("Extracting git software version")
   execute_process(COMMAND bash "-c" "git rev-list --branches HEAD | wc -l | tr -d ' ' | tr -d '\n'" OUTPUT_VARIABLE GIT_VERSION)
   SET(MINOR_VERSION 1)
   SET(BUILD_NUMBER ${GIT_VERSION})
   SET(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_NUMBER}) 
ENDIF()
MESSAGE("Software Version: ${VERSION}")

SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output dir for libraries")
SET(CMAKE_INSTALL_HEADERDIR include CACHE PATH "Output dir for headers")

IF (${CPACK_INSTALL_PREFIX}.x STREQUAL ".")
   SET(CPACK_INSTALL_PREFIX "/usr/local")
   MESSAGE("CPACK_INSTALL_PREFIX: " ${CPACK_INSTALL_PREFIX})
ENDIF()





# Load PATHS
INCLUDE (${LogRotate_SOURCE_DIR}/Options.cmake)

#
# Find all libraries
#

 IF (${CMAKE_PREFIX_PATH}.x STREQUAL ".x")
    SET(CMAKE_PREFIX_PATH /usr/local)
    MESSAGE("CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})
 ENDIF()


find_library(g3logger g3logger PATHS /usr/local/probe/lib )
include_directories(${CMAKE_PREFIX_PATH}/include)


find_library(BOOST_FILESYSTEM boost_filesystem PATHS /usr/local/probe/lib ${BOOST_ROOT})
set(Boost_LIBRARIES ${BOOST_FILESYSTEM})

find_library(BOOST_SYSTEM boost_system PATHS /usr/local/probe/lib ${BOOST_ROOT} )
list(APPEND Boost_LIBRARIES ${BOOST_SYSTEM})
include_directories(${Boost_INCLUDE_DIRS})

MESSAGE(" boost root: " ${BOOST_ROOT})
MESSAGE(" boost include : " ${Boost_INCLUDE_DIRS})
MESSAGE("BOOST LIBRARIES: ${Boost_LIBRARIES}")


find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )





#
# Build
#
# GENERIC STEPS
FILE(GLOB SRC_FILES ${PROJECT_SRC}/*.h ${PROJECT_SRC}/*.h ${PROJECT_SRC}/*.ipp ${PROJECT_SRC}/*.cpp) 


IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
   MESSAGE("")
   MESSAGE("cmake for Clang ")
   SET(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++14 -stdlib=libc++ -Wunused -D_GLIBCXX_USE_NANOSLEEP")
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



#
# Setup Library 
#
SET(LIBRARY_TO_BUILD g3logrotate)
INCLUDE_DIRECTORIES(${PROJECT_SRC})

# see Options.cmake
IF(BUILD_STATIC)
   ADD_LIBRARY(${LIBRARY_TO_BUILD} ${SRC_FILES})
ELSE()
   IF(NOT(CMAKE_VERSION LESS 3.4) AND MSVC)
      set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
   ENDIF()
   ADD_LIBRARY(${LIBRARY_TO_BUILD} SHARED ${SRC_FILES})
ENDIF()


SET(${LIBRARY_TO_BUILD}_VERSION_STRING ${VERSION})
SET_TARGET_PROPERTIES(${LIBRARY_TO_BUILD} PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(${LIBRARY_TO_BUILD}  ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${G3LOG})
SET_TARGET_PROPERTIES(${LIBRARY_TO_BUILD} PROPERTIES LINKER_LANGUAGE CXX SOVERSION ${VERSION})

MESSAGE("Libraries, g3log: ${g3logger}")
MESSAGE("Libraries, boost: ${Boost_LIBRARIES}")
MESSAGE("Libraries, zlib: ${ZLIB_LIBRARIES}")

#
# Setup unit tests
#
INCLUDE (${LogRotate_SOURCE_DIR}/test/Test.cmake)


#
# make package or make install
#
IF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   FILE(GLOB HEADER_FILES ${PROJECT_SRC}/g3sinks/*.h)
   INCLUDE (${LogRotate_SOURCE_DIR}/CPackLists.txt)
ENDIF()


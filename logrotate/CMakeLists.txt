# =====================================================
#  Public Domain: Inspired by github.com/KjellKod/g3log/CMakeLists.cmake
#  2015, April 30, @author Kjell.Hedstrom
# =====================================================

CMAKE_MINIMUM_REQUIRED (VERSION 3.2)
ENABLE_LANGUAGE(CXX)
SET(CMAKE_BUILD_TYPE Release)

PROJECT (LogRotate)
SET(PROJECT_SRC ${LogRotate_SOURCE_DIR}/src)
SET(DIR_UNIT_TEST ${LogRotate_SOURCE_DIR}/test)
SET(DIR_3RDPARTY ${LogRotate_SOURCE_DIR}/../3rdparty)


MESSAGE(" PROJECT_SRC = : ${PROJECT_SRC}")
INCLUDE_DIRECTORIES(${PROJECT_SRC})
INCLUDE_DIRECTORIES(${PROJECT_SRC}/../../common)
SET(ACTIVE_CPP0xx_DIR "Release")


SET(MAJOR_VERSION 1)
IF (${VERSION}.x STREQUAL ".x")
   IF (MSVC)
      message("windows: Extracting git software version")
      execute_process(COMMAND powershell "(git rev-list --branches HEAD | Measure-Object -line).Lines" OUTPUT_VARIABLE GIT_VERSION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
   ELSE()
      IF(UNIX OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
         message("nix: Extracting git software version")
      ELSE()
         message("unknown platform: extracting git software version")
      ENDIF()
      execute_process(COMMAND bash "-c" "git rev-list --branches HEAD | wc -l | tr -d ' ' | tr -d '\n'" OUTPUT_VARIABLE GIT_VERSION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
   ENDIF()

   SET(MINOR_VERSION 1)
   math(EXPR VERSION-BASE ${GIT_VERSION}/255)
   math(EXPR VERSION-REMAINDER ${GIT_VERSION}%255)
   message("git build version: ${GIT_VERSION}")
   message("version base: ${VERSION-BASE}")
   message("version remainder: ${VERSION-REMAINDER}")
   # SET(BUILD_NUMBER ${GIT_VERSION})
   SET(BUILD_NUMBER ${VERSION-BASE})
   SET(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_NUMBER}-${VERSION-REMAINDER})
ENDIF()
MESSAGE("Software Version: ${VERSION}")

SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output dir for libraries")
SET(CMAKE_INSTALL_HEADERDIR include CACHE PATH "Output dir for headers")
SET(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

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


find_library(G3LOG_LIBRARY g3logger PATHS /usr/local/probe/lib )
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
SET_TARGET_PROPERTIES(${LIBRARY_TO_BUILD} PROPERTIES 
                      LINKER_LANGUAGE CXX
                      OUTPUT_NAME ${LIBRARY_TO_BUILD}
                      CLEAN_DIRECT_OUTPUT 1
                      CXX_STANDARD 14
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      SOVERSION ${VERSION}
                      )

TARGET_COMPILE_FEATURES(${LIBRARY_TO_BUILD}  PUBLIC cxx_variable_templates)
TARGET_LINK_LIBRARIES(${LIBRARY_TO_BUILD}  ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${G3LOG_LIBRARY})

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
   list(APPEND HEADER_FILES ${PROJECT_SRC}/../../common/g3sinksUtils.hpp)
   INCLUDE (${LogRotate_SOURCE_DIR}/CPackLists.txt)
ENDIF()


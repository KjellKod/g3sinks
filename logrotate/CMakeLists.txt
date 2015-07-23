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
SET(DIR_3RDPARTY ${LogRotate_SOURCE_DIR}/3rdparty)


MESSAGE(" PROJECT_SRC = : ${PROJECT_SRC}")
INCLUDE_DIRECTORIES(${PROJECT_SRC})
SET(ACTIVE_CPP0xx_DIR "Release")

#Find all libraries 
find_library(z boost_filesystem boost_system boost_regex)
find_library(G3LOG g3logger PATHS /usr/local/probe/lib)


IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   MESSAGE("")
   MESSAGE("cmake for GCC ")
   SET(PLATFORM_LINK_LIBRIES rt)
   SET(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++11 -pthread -D_GLIBCXX_USE_NANOSLEEP")
ENDIF()


# GENERIC STEPS
FILE(GLOB SRC_FILES ${PROJECT_SRC}/*.h ${PROJECT_SRC}/*.h ${PROJECT_SRC}/*.ipp ${PROJECT_SRC}/*.cpp) 


# Setup Library name
SET(LIBRARY_TO_BUILD g3logrotate)

# Build the library
INCLUDE_DIRECTORIES(${PROJECT_SRC})
ADD_LIBRARY(${LIBRARY_TO_BUILD} SHARED ${SRC_FILES})
SET(${LIBRARY_TO_BUILD}_VERSION_STRING)
SET_TARGET_PROPERTIES(${LIBRARY_TO_BUILD} PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(${LIBRARY_TO_BUILD} ${TCMALLOC} boost_filesystem boost_system boost_regex z ${G3LOG})



# Setup unit tests
INCLUDE (${LogRotate_SOURCE_DIR}/test/Test.cmake)









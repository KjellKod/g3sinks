# =====================================================
#  Public Domain: Inspired by github.com/KjellKod/g3log/CMakeLists.cmake
#  2015, April 30, @author Kjell.Hedstrom
# =====================================================

option (ADD_LOGROTATE_UNIT_TEST "logrotate unit tests" OFF)

IF (ADD_LOGROTATE_UNIT_TEST)

   include_directories(${CMAKE_PREFIX_PATH}/include)
   MESSAGE("Test include : : " ${CMAKE_PREFIX_PATH}/include " : lib : " ${g3logger})


   set(GTEST_DIR ${DIR_3RDPARTY}/gtest-1.7.0)
   set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
   MESSAGE( "Attempt to build gtest. gtest directory: " ${GTEST_DIR})
   include_directories(${GTEST_INCLUDE_DIRECTORIES})
   add_library(gtest_170_lib ${GTEST_DIR}/src/gtest-all.cc)
   set_target_properties(gtest_170_lib PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")
   enable_testing(true)
   include_directories(test)
   file(GLOB TEST_SRC_FILES "test/*.cpp" "test/*.h" "test/*.hpp")


   # build the unit tests   
   add_executable(UnitTestRunner ${DIR_3RDPARTY}/test_main.cpp ${TEST_SRC_FILES} )
   set_target_properties(UnitTestRunner PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_TR1_TUPLE=0")
   set_target_properties(UnitTestRunner PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")

   IF (MSVC)
      target_link_libraries(UnitTestRunner ${LIBRARY_TO_BUILD} ${g3logger} gtest_170_lib dbghelp ${PLATFORM_LINK_LIBRIES})
   ELSE()
      SET(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -rdynamic -Wunused -std=c++17 -lstdc++fs -pthread -D_GLIBCXX_USE_NANOSLEEP")
       
      TARGET_LINK_LIBRARIES(UnitTestRunner ${LIBRARY_TO_BUILD} ${g3logger} gtest_170_lib ${PLATFORM_LINK_LIBRIES} stdc++fs)
   ENDIF() 
ELSE() 
  MESSAGE("-DADD_LOGROTATE_UNIT_TEST=OFF") 
ENDIF (ADD_LOGROTATE_UNIT_TEST)

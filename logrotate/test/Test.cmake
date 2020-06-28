# =====================================================
#  Public Domain: Inspired by github.com/KjellKod/g3log/CMakeLists.cmake
#  2015, April 30, @author Kjell.Hedstrom
# =====================================================

IF (BUILD_TEST)
   include_directories(${CMAKE_PREFIX_PATH}/include)
   MESSAGE("Test include : : " ${CMAKE_PREFIX_PATH}/include " : lib : " ${g3logger})

   include_directories(test)
   file(GLOB TEST_SRC_FILES "test/*.cpp" "test/*.h" "test/*.hpp")


   # build the unit tests   
   add_executable(UnitTestRunner ${DIR_3RDPARTY}/test_main.cpp ${TEST_SRC_FILES} )
   set_target_properties(UnitTestRunner PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_TR1_TUPLE=0")
   set_target_properties(UnitTestRunner PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")

   IF (MSVC)
     TARGET_LINK_LIBRARIES(UnitTestRunner ${LIBRARY_TO_BUILD} ${g3logger} gtest_main dbghelp ${PLATFORM_LINK_LIBRIES})
   ELSE()
      TARGET_LINK_LIBRARIES(UnitTestRunner ${LIBRARY_TO_BUILD} ${g3logger} gtest_main ${PLATFORM_LINK_LIBRIES})
   ENDIF() 
ELSE() 
  MESSAGE("-DBUILD_TEST=OFF") 
ENDIF (BUILD_TEST)

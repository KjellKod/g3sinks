
project(test)

if (CHOICE_logrotate)
   include_directories(${G3LOG_INCLUDE_DIR} ${g3sinks_SOURCE_DIR}/logrotate/src)
   set(LOGROTATE_TEST_FILES FilterTest.cpp RotateFileTest.cpp RotateTestHelper.cpp)
   add_executable(test_logrotate ${TEST_MAIN} ${LOGROTATE_TEST_FILES})
   target_link_libraries(
     test_logrotate 
     PRIVATE gtest_main 
     PRIVATE ${G3LOG_LIBRARY}
     PRIVATE g3logrotate)
   add_test(test_logrotate test_logrotate)
endif()
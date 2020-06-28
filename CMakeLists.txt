cmake_minimum_required(VERSION 3.2 )

project(g3sink CXX)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

option (BUILD_STATIC "Building sinks as static libraries. Linking with static g3log library" OFF)
option (BUILD_TEST "Build the unit tests" ON)

add_subdirectory(syslog)
add_subdirectory(logrotate)
add_subdirectory(snippets)

if(BUILD_TEST)
   # Download and unpack googletest at configure time
   configure_file(CMakeLists.txt.in
         googletest-download/CMakeLists.txt)
   execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
   execute_process(COMMAND ${CMAKE_COMMAND} --build .
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

   # Prevent GoogleTest from overriding our compiler/linker options
   # when building with Visual Studio
   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

   # Add googletest directly to our build. This adds
   # the following targets: gtest, gtest_main, gmock
   # and gmock_main
   add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
         ${CMAKE_BINARY_DIR}/googletest-build)

   # The gtest/gmock targets carry header search path
   # dependencies automatically when using CMake 2.8.11 or
   # later. Otherwise we have to add them here ourselves.
   if (CMAKE_VERSION VERSION_LESS 2.8.11)
      include_directories("${gtest_SOURCE_DIR}/include"
               "${gmock_SOURCE_DIR}/include")
   endif()

   enable_testing()
endif (BUILD_TEST)

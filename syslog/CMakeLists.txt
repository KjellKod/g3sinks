cmake_minimum_required(VERSION 3.1)
project(g3log_syslog VERSION 0.1.0)

# search for syslog.h
include(CheckIncludeFile)
check_include_file("syslog.h" HAVE_SYSLOG_H)
if(NOT ${HAVE_SYSLOG_H})
	message(FATAL_ERROR "Could not find syslog.h.")
endif()


IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
   MESSAGE("")
   MESSAGE("cmake for Clang ")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14 -stdlib=libc++ -Wunused -D_GLIBCXX_USE_NANOSLEEP")
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
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -rdynamic -Wunused -std=c++14 -pthread -D_GLIBCXX_USE_NANOSLEEP")
ENDIF()


# search for g3log
find_package(g3logger REQUIRED)
# add the missing target if needed
if (NOT TARGET g3logger)
    add_library(g3logger SHARED IMPORTED)
    find_package(Threads REQUIRED)
    set_target_properties(g3logger PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${G3LOG_INCLUDE_DIRS}"
        IMPORTED_LOCATION "${G3LOG_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES  Threads::Threads
        )
endif()


# Target
add_library(g3log_syslog STATIC src/syslogsink.cpp)
target_include_directories(g3log_syslog 
    PRIVATE
      src
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
      $<INSTALL_INTERFACE:include>
    )
set_target_properties(g3log_syslog PROPERTIES CXX_STANDARD 11)
target_link_libraries(g3log_syslog PUBLIC g3logger)
target_include_directories(g3log_syslog
    PRIVATE "src/"
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>    
    )

add_subdirectory(example)

# Installation
install(TARGETS g3log_syslog
	EXPORT g3log_syslogTargets
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include)
install(EXPORT g3log_syslogTargets
	NAMESPACE G3::
	DESTINATION lib/cmake
	)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
	DESTINATION include
	FILES_MATCHING
	PATTERN *.h*
	)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("g3log_syslogVersion.cmake"
	VERSION ${g3log_syslogVersion}
	COMPATIBILITY AnyNewerVersion
	)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3log_syslogVersion.cmake"
	DESTINATION lib/cmake
	)

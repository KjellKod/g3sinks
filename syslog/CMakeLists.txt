
cmake_minimum_required(VERSION 3.2 )
project(g3log_syslog VERSION 0.1.0)

# search for syslog.h
include(CheckIncludeFile)
check_include_file("syslog.h" HAVE_SYSLOG_H)
if(NOT ${HAVE_SYSLOG_H})
	message(FATAL_ERROR "Could not find syslog.h.")
endif()

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
target_include_directories(g3log_syslog PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
)
SET_TARGET_PROPERTIES(g3log_syslog PROPERTIES 
                      LINKER_LANGUAGE CXX
                      OUTPUT_NAME g3log_syslog
                      CLEAN_DIRECT_OUTPUT 1
                      CXX_STANDARD 14
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      ${g3log_syslogVersion}
                      )
target_link_libraries(g3log_syslog PUBLIC g3logger)
TARGET_COMPILE_FEATURES(g3log_syslog PUBLIC cxx_variable_templates)

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
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/g3log/syslogsink.hpp"
	DESTINATION include/g3sinks
	)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../common/g3sinksUtils.hpp"
	DESTINATION include/g3sinks
	)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("g3log_syslogVersion.cmake"
	VERSION ${g3log_syslogVersion}
	COMPATIBILITY AnyNewerVersion
	)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3log_syslogVersion.cmake"
	DESTINATION lib/cmake
	)

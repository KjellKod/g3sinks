project(g3syslog)

# search for syslog.h
verifySyslogDependencies(SYSLOG_SINK_ERROR) 
if (SYSLOG_SINK_ERROR)
   message(FATAL_ERROR "${SYSLOG_SINK_ERROR}")
endif()

# Target
if(CHOICE_BUILD_STATIC)
   add_library(g3syslog STATIC src/syslogsink.cpp)
else()
   if(MSVC)
      set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
   endif()
   add_library(g3syslog SHARED src/syslogsink.cpp)
endif()

include_directories(${G3LOG_INCLUDE_DIRS})

target_include_directories(g3syslog 
    PRIVATE
      src
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
      $<INSTALL_INTERFACE:include>
    )
set_target_properties(g3syslog PROPERTIES 
                      LINKER_LANGUAGE CXX
                      OUTPUT_NAME g3syslog
                      CLEAN_DIRECT_OUTPUT 1
                      ${g3syslogVersion}
                      )
target_link_libraries(g3syslog PUBLIC ${G3LOG_LIBRARY})

if (UNIX)
   target_compile_options(g3syslog PRIVATE )
endif()

target_include_directories(g3syslog
    PRIVATE "src/"
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>    
    )

# INSTALLATION 
# ===================================================
install(TARGETS 
           g3syslog
	      EXPORT 
            g3syslogTargets
	      ARCHIVE DESTINATION lib
	      LIBRARY DESTINATION lib
	      INCLUDES DESTINATION include)

install(EXPORT g3syslogTargets
	NAMESPACE G3::
	DESTINATION lib/cmake/g3sinks
	)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks"
	DESTINATION include
	FILES_MATCHING
	PATTERN *.h*
	)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("g3syslogVersion.cmake"
	VERSION ${VERSION}
	COMPATIBILITY AnyNewerVersion
	)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3syslogVersion.cmake"
	DESTINATION lib/cmake/g3sinks
	)

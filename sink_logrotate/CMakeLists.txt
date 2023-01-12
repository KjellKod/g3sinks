project(logrotate)


# zlib
find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
message("zlib found: ${ZLIB_FOUND} ")

# this is frowned upon but darn convenient. 
set( LOGROTATE_SRC ${logrotate_SOURCE_DIR}/src )
file( GLOB LOGROTATE_SRC_FILES ${LOGROTATE_SRC}/*.ipp ${LOGROTATE_SRC}/*.cpp ) 

# # Add the header files to the global header files list for installation 
# file( GLOB LOGROTATE_HEADER_FILES ${LOGROTATE_SRC}/g3sinks/*.h )
# add_installation_headers( ${LOGROTATE_HEADER_FILES} )

if ( CHOICE_BUILD_STATIC ) 
   add_library( g3logrotate STATIC ${LOGROTATE_SRC_FILES})
else() 
   add_library( g3logrotate SHARED ${LOGROTATE_SRC_FILES})
endif()

include_directories(${LOGROTATE_SRC})


# Build Library 
target_link_libraries(
  g3logrotate
  PUBLIC  ${G3LOG_LIBRARY}
  PRIVATE ${ZLIB_LIBRARY}
  )

message( "target_link_libraries:  
         g3log: ${G3LOG_LIBRARY}, 
         zlib: ${ZLIB_LIBRARIES}" )


# set target properties 
if (MSVC)
   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set_target_properties(g3logrotate 
                      PROPERTIES
                      SOVERSION ${VERSION}
                      )

if (UNIX)
   target_compile_options(g3logrotate PRIVATE )
endif()



if ( CHOICE_INSTALL_G3SINKS )
    # INSTALLATION 
    # ===================================================
    install(TARGETS 
               g3logrotate
            EXPORT 
                g3logrotateTargets
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            INCLUDES DESTINATION include)
    
    install(EXPORT g3logrotateTargets
      NAMESPACE G3::
      DESTINATION lib/cmake/g3sinks
      )
    
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks"
      DESTINATION include
      FILES_MATCHING
      PATTERN *.h*
      )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file("g3logrotateVersion.cmake"
      VERSION ${VERSION}
      COMPATIBILITY AnyNewerVersion
      )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3logrotateVersion.cmake"
      DESTINATION lib/cmake/g3sinks
      )
endif()

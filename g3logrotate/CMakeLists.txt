project(g3logrotate)

# ===============================
# boost filesystem + boost system 
set( Boost_USE_STATIC_LIBS ON ) 
set( Boost_USE_MULTITHREAD ON ) 
find_package( Boost REQUIRED COMPONENTS filesystem system ) 
message( "boost [filesystem, system] found: ${Boost_FOUND} ")
include_directories( ${Boost_INCLUDE_DIRS} ) 


# zlib
find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
message("zlib found: ${ZLIB_FOUND} ")

# this is frowned upon but darn convenient. 
set( G3LOGROTATE_SRC ${g3logrotate_SOURCE_DIR}/src )
file( GLOB G3LOGROTATE_SRC_FILES ${G3LOGROTATE_SRC}/*.ipp ${G3LOGROTATE_SRC}/*.cpp ) 

if ( CHOICE_BUILD_STATIC ) 
   add_library( g3logrotate STATIC ${G3LOGROTATE_SRC_FILES})
else() 
   add_library( g3logrotate SHARED ${G3LOGROTATE_SRC_FILES})
endif()

include_directories(${G3LOGROTATE_SRC})

# Build Library 
target_link_libraries(
  g3logrotate
  PUBLIC  ${G3LOG_LIBRARY}
  #${Boost_LIBRARIES}
  PRIVATE Boost::filesystem
  PRIVATE Boost::system
  PRIVATE ${ZLIB_LIBRARY}
  )

message( "target_link_libraries:  
         g3log: ${G3LOG_LIBRARY}, 
         boost: ${Boost_LIBRARIES}, 
         zlib: ${ZLIB_LIBRARIES}" )


# set target properties 
if (MSVC)
   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set_target_properties(g3logrotate 
                      PROPERTIES
                      SOVERSION ${VERSION}
                      )

#if (UNIX)
#   target_compile_options(g3logrotate PUBLIC )
#endif()



# INSTALLATION 
# ===================================================
add_library(g3sinks::g3logrotate ALIAS g3logrotate)
include(GNUInstallDirs)
install( 
  TARGETS g3logrotate
  EXPORT g3sinksTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION include
  )


target_include_directories(g3logrotate PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks>
  $<INSTALL_INTERFACE:include/g3sinks>
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks"
  DESTINATION include
  FILES_MATCHING
  PATTERN *.h*
)





#install(TARGETS 
#           g3logrotate
#        EXPORT 
#            g3logrotateTargets
#        ARCHIVE DESTINATION lib
#        LIBRARY DESTINATION lib
#
#
#install(EXPORT g3logrotate
#  NAMESPACE G3::
#  DESTINATION lib/cmake/g3sinks
#  )
#
#install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks"
#  DESTINATION include
#  FILES_MATCHING
#  PATTERN *.h*
#  )
#
#include(CMakePackageConfigHelpers)
#write_basic_package_version_file("g3logrotateVersion.cmake"
#  VERSION ${VERSION}
#  COMPATIBILITY AnyNewerVersion
#  )
#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3logrotateVersion.cmake"
#  DESTINATION lib/cmake/g3sinks
#  )


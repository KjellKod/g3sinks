#.rst:
# FindG3sinks
# -------
#
# ```
# target_link_libraries(YourTarget PUBLIC g3sinks)
#```
# Variables and features 
# ----------------------
# * ``g3sinks`` -- if this environment variable is set, it'll be used as a hint as to where the g3sinks files are. 
# * ``G3SINKS_INCLUDE_DIRS`` -- raw cmake variable with include path 
# * ``G3SINKS_LIBRARIES`` -- raw cmake variable with library link line
# * ``G3SINKS_FOUND`` -- check if the lib was found without using the newer ``if(TARGET g3sinks)...``

include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

@PACKAGE_INIT@

find_package(Threads REQUIRED)

if (NOT TARGET g3sinks)
   include("${CMAKE_CURRENT_LIST_DIR}/g3sinksTargets.cmake")

   get_target_property(G3SINKS_INCLUDE_DIR g3sinks INTERFACE_INCLUDE_DIRECTORIES)
   get_target_property(G3SINKS_LIBRARY_DEBUG g3sinks IMPORTED_IMPLIB_DEBUG)
   
   if (G3SINKS_LIBRARY_DEBUG MATCHES ".*-NOTFOUND")
      get_target_property(G3SINKS_LIBRARY_DEBUG g3sinks g3logrotate IMPORTED_LOCATION_DEBUG)
   endif ()

   get_target_property(G3SINKS_LIBRARY_RELEASE g3sinks g3logrotate IMPORTED_IMPLIB_RELEASE)
   if (G3SINKS_LIBRARY_RELEASE MATCHES ".*-NOTFOUND")
      get_target_property(G3SINKS_LIBRARY_RELEASE g3sinks IMPORTED_LOCATION_RELEASE)
   endif ()

   select_library_configurations(G3SINKS)

   if (G3SINKS_LIBRARY)
      list(APPEND G3SINKS_LIBRARY Threads::Threads)
      if (WIN32)
         list(APPEND G3SINKS_LIBRARY DbgHelp.lib)
      endif ()
   endif ()
endif ()

find_package_handle_standard_args(G3SINKS REQUIRED_VARS G3SINKDS_INCLUDE_DIR G3SINKS_LIBRARY)
mark_as_advanced(G3SINKDS_INCLUDE_DIR G3SINKS_LIBRARY)
set(G3SINKS_INCLUDE_DIRS ${G3SINKS_INCLUDE_DIR})
set(G3SINKS_LIBRARIES ${G3SINKS_LIBRARY})

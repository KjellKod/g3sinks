# Findg3log_tracelogging
# -------
#
# Find g3log_tracelogging, g3log_tracelogging is a sinc for g3log that enables logging through Microsoft Windows TraceLogging.
#
# This defines the cmake import target "g3log_tracelogging" you can use like this
#```
# target_link_libraries(YourTarget PUBLIC g3log_tracelogging)
#```
# Variables and features 
# ----------------------
# * ``G3LOG_TRACELOGGING`` -- if this environment variable is set, it'll be used as a hint as to where the g3logger files are. 
# * ``G3LOG_TRACELOGGING_INCLUDE_DIRS`` -- raw cmake variable with include path 
# * ``G3LOG_TRACELOGGING_LIBRARIES`` -- raw cmake variable with library link line
# * ``G3LOG_TRACELOGGING_FOUND`` -- check if the lib was found without using the newer ``if(TARGET g3logger)...``

include(FindPackageHandleStandardArgs)
find_path(G3LOG_TRACELOGGING_INCLUDE_DIR 
    NAMES
         g3log_tracelogging/g3log_tracelogging.h
    PATHS        
        ENV G3LOG_TRACELOGGING
)
find_library(G3LOG_TRACELOGGING_LIBRARY
            NAMES g3log_tracelogging )
find_package_handle_standard_args(g3log_tracelogging
            REQUIRED_VARS G3LOG_TRACELOGGING_INCLUDE_DIR G3LOG_TRACELOGGING_LIBRARY)
mark_as_advanced(G3LOG_TRACELOGGING_INCLUDE_DIR G3LOG_TRACELOGGING_LIBRARY)
set(G3LOG_TRACELOGGING_LIBRARIES ${G3LOG_TRACELOGGING_LIBRARY})
set(G3LOG_TRACELOGGING_INCLUDE_DIRS ${G3LOG_TRACELOGGING_INCLUDE_DIR})

add_library(g3log_tracelogging STATIC IMPORTED)
find_package(Threads REQUIRED)
set_target_properties(g3log_tracelogging PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${G3LOG_TRACELOGGING_INCLUDE_DIRS}"
    IMPORTED_LOCATION "${G3LOG_TRACELOGGING_LIBRARY}"
    IMPORTED_LINK_INTERFACE_LIBRARIES Threads::Threads
    )

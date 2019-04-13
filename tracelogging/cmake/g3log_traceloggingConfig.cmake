# Findg3log_tracelogging
# -------
#
# Find g3log_tracelogging, g3log_tracelogging is a sink for g3log that enables logging through Microsoft Windows TraceLogging.
#
# This defines the cmake import target "g3log_tracelogging" you can use like this
#```
# target_link_libraries(YourTarget PUBLIC g3log_tracelogging)
#```
# Variables and features 
# ----------------------
# * ``G3LOG_TRACELOGGING`` -- if this environment variable is set, it'll be used as a hint as to where the g3log_tracelogging files are. 
# * ``G3LOG_TRACELOGGING_INCLUDE_DIRS`` -- raw cmake variable with include path 
# * ``G3LOG_TRACELOGGING_FOUND`` -- check if the lib was found without using the newer ``if(TARGET g3log_tracelogging)...``

include(FindPackageHandleStandardArgs)

@PACKAGE_INIT@

find_package(g3logger REQUIRED)

if (NOT TARGET g3log_tracelogging)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
  include("${CMAKE_CURRENT_LIST_DIR}/g3log_traceloggingTargets.cmake")
endif(NOT TARGET g3log_tracelogging)

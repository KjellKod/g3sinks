project(g3snippets)

# HEADER ONLY INSTALLATIONS FOR SIMPLE SNIPPET LOGGING SINKS
verifyfilelogdependencies(FILE_LOG_SINK_ERROR)
if(NOT FILE_LOG_SINK_ERROR)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FileLogSinkDefines.h.in
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks)
else()
  message(WARNING "${FILE_LOG_SINK_ERROR}")
  set(FILELOG_EXCLUDE_FILE "FileLogSink.h")
endif()

# ColoredCout sink is currently only implemented for Linux/Unix terminals
if (NOT WIN32)
   set(COLOREDCOUT_EXCLUDE_FILE "ColoredCoutSink.h")
endif() 

# verifyTraceloggingDependencies(TRACELOGGING_SINK_ERROR)
# if(TRACELOGGING_SINK_ERROR)
#   message(WARNING "${TRACELOGGING_SINK_ERROR}")
#   set(TRACELOGGING_EXCLUDE_FILE "g3logtracelogging.h")
# endif()


# HEADER ONLY INSTALLATION
# ===================================================
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks"
  DESTINATION include
  FILES_MATCHING
  PATTERN *.h
  PATTERN "${FILELOG_EXCLUDE_FILE}" EXCLUDE
  PATTERN "${TRACELOGGING_EXCLUDE_FILE}" EXCLUDE
  PATTERN "${COLOREDCOUT_EXCLUDE_FILE}" EXCLUDE
  )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "g3snippetsVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3snippetsVersion.cmake"
        DESTINATION lib/cmake/g3sinks)

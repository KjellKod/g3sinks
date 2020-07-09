project(g3snippets)

# HEADER ONLY INSTALLATIONS FOR SIMPLE SNIPPET LOGGING SINKS
set(EXCLUDE_FILES "")
verifyfilelogdependencies(FILE_LOG_SINK_ERROR)

if(NOT FILE_LOG_SINK_ERROR)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FileLogSinkDefines.h.in
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks)
else()
  message(WARNING "The shared memory logging sink - snippets/FileLogSink.h"
                  " will be ignored as requirements are not met:"
                  "${FILE_LOG_SINK_ERROR}")
  list(APPEND EXCLUDE_FILES "FileLogSink.h")
endif()

# HEADER ONLY INSTALLATION
# ===================================================
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks"
  DESTINATION include
  FILES_MATCHING
  PATTERN *.h
  REGEX "${EXCLUDE_FILES}" EXCLUDE)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "g3snippetsVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3snippetsVersion.cmake"
        DESTINATION lib/cmake/g3sinks)

cmake_minimum_required(VERSION 3.8...3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(
  g3log_tracelogging_example
  VERSION 0.1
  DESCRIPTION "g3log_tracelogging example"
  LANGUAGES CXX)

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# search for g3log_tracelogging

if(WIN32)
  verifytraceloggingdependencies(TRACELOGGING_SINK_ERROR)
  if(NOT TRACELOGGING_SINK_ERROR)
    find_package(g3log_tracelogging REQUIRED NO_MODULE)
    message(
      STATUS
        "G3LOG_TRACELOGGING_INCLUDE_DIRS: ${G3LOG_TRACELOGGING_INCLUDE_DIRS}")

    message(STATUS "G3LOG_TRACELOGGING_LIBRARY: ${G3LOG_TRACELOGGING_LIBRARY}")

    # Add source to this project's executable.
    add_executable(g3log_tracelogging_example "src/tracelogging_example.cpp")

    target_link_libraries(g3log_tracelogging_example
                          g3log_tracelogging::g3log_tracelogging)

  endif()
endif()

# add_executable(example_g3syslog g3syslog_main.cpp)
# target_link_libraries(syslog_g3log_example g3syslog)

project(examples)

# Demonstrates LogRotate and LogRotateWithFilter
if(CHOICE_SINK_LOGROTATE)
  include_directories(${g3sinks_SOURCE_DIR}/sink_logrotate/src
                      ${g3sinks_SOURCE_DIR}/test ${G3LOG_INCLUDE_DIR})
  add_executable(
    example_logrotate_and_filter
    logrotate_main.cpp ${g3sinks_SOURCE_DIR}/test/RotateTestHelper.cpp)
  target_link_libraries(
    example_logrotate_and_filter
    PRIVATE gtest_main
    PRIVATE ${G3LOG_LIBRARY}
    PRIVATE g3logrotate)

  if(UNIX)
    target_compile_options(example_logrotate_and_filter PRIVATE)
  endif()

  # add_example(example_logrotate test_logrotate)
endif()

if(CHOICE_SINK_SYSLOG)
  verifysyslogdependencies(SYSLOG_SINK_ERROR)
  if(NOT SYSLOG_SINK_ERROR)
    include_directories(${g3sinks_SOURCE_DIR}/sink_syslog/src
                        ${G3LOG_INCLUDE_DIR})
    add_executable(example_syslog syslog_main.cpp)
    target_link_libraries(
      example_syslog
      PRIVATE gtest_main
      PRIVATE ${G3LOG_LIBRARY}
      PRIVATE g3syslog)

    if(UNIX)
      target_compile_options(example_syslog PRIVATE)
    endif()

    # add_example(example_logrotate test_logrotate)
  endif()
endif()

if(CHOICE_SINK_SNIPPETS)

  # ColoredCout is currently only implemented for UNIX/Linux terminals
  if(UNIX)
    # Log with color formatting to std::cout
    include_directories(${g3sinks_SOURCE_DIR}/sink_snippets/src
                        ${G3LOG_INCLUDE_DIR})
    add_executable(example_coloredcout snippet_coloredcout_main.cpp)
    target_link_libraries(
      example_coloredcout
      PRIVATE gtest_main
      PRIVATE ${G3LOG_LIBRARY})
    target_compile_options(example_coloredcout PRIVATE)
  endif()

  # shared memory, file descriptor sink.
  verifyfilelogdependencies(FILE_LOG_SINK_ERROR)
  if(NOT FILE_LOG_SINK_ERROR)
    include_directories(${g3sinks_SOURCE_DIR}/sink_snippets/src
                        ${G3LOG_INCLUDE_DIR})
    add_executable(example_filedescriptor snippet_filelog_main.cpp)
    target_link_libraries(
      example_filedescriptor
      PRIVATE gtest_main
      PRIVATE ${G3LOG_LIBRARY})
    if(UNIX)
      target_compile_options(example_filedescriptor PRIVATE)
    endif()
  endif()
endif()

if(WIN32)
  # WIN32 tracelogging verifytraceloggingdependencies(TRACELOGGING_SINK_ERROR)
  # if(NOT TRACELOGGING_SINK_ERROR)
  include_directories(${g3sinks_SOURCE_DIR}/sink_snippets/src
                      ${G3LOG_INCLUDE_DIR})
  add_executable(example_tracelogging snippet_tracelogging_main.cpp)
  target_link_libraries(
    example_tracelogging
    PRIVATE gtest_main
    PRIVATE ${G3LOG_LIBRARY}
    PRIVATE advapi32.lib)
  if(UNIX)
    target_compile_options(example_tracelogging PRIVATE)
  endif()
  # endif()
endif()

# KjellKod TODO add_custom_target(run COMMAND ./TextMiningCompiler <pathword>
# totoro.txt COMMAND cat test.txt | time ./TextMiningApp totoro.txt )
# add_dependencies(run TextMiningCompiler TextMiningApp)

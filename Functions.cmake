# =====================================================
# Public Domain: Inspired by github.com/KjellKod/g3log/Options.cmake 2020,
# @author Kjell.Hedstrom
# =====================================================

# ref:
# https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script#9328525
function(dump_cmake_variables)
  get_cmake_property(_variableNames VARIABLES)
  list(SORT _variableNames)
  foreach(_variableName ${_variableNames})
    if(ARGV0)
      unset(MATCHED)
      string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
      if(NOT MATCHED)
        continue()
      endif()
    endif()
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endfunction()

# Sets FILE_LOG_SINK_ERROR in caller's scope if sys/mman.h or sys/memfd.h cannot
# be found. Example usage: verifyFileLogDependencies(FILE_LOG_SINK_ERROR) if
# (NOT FILE_LOG_SINK_ERROR) .... dependencies fullfilled else() .
# message("FileLogSink dependencies not fullfilled. ${FILE_LOG_SINK_ERROR}")
# endif()
function(verifyFileLogDependencies VARNAME)
  set(FileLogSinkError "")
  find_path(MMMAN_DIR sys/mman.h
            PATHS usr/include /usr/local/include
                  /Library/Developer/CommandLineTools/SDKs/*/usr/include)
  if(NOT MMMAN_DIR)
    list(APPEND FileLogSinkError "Could not locate sys/mman.h")
  endif()

  find_file(MEMFD_H sys/memfd.h PATHS ${MMMAN_DIR})
  if(NOT MEMFD_H)
    list(APPEND FileLogSinkError " Could not locate sys/memfd.h")
  endif()

  if(FileLogSinkError)
    set(${VARNAME}
        "${FileLogSinkError}"
        PARENT_SCOPE)
  endif()
endfunction()

# Usage: 
# 
# verifySyslogDependencies(SYSLOG_SINK_ERROR) 
# if (NOT SYSLOG_SINK_ERROR)
# ... syslog is available, start using it.
function(verifySyslogDependencies VARNAME)
  include(CheckIncludeFile)
  check_include_file("syslog.h" HAVE_SYSLOG_H)
  if(NOT HAVE_SYSLOG_H)
    set(${VARNAME}
        "Could not find syslog.h. g3syslog [sink, test,example] will not be built"
        PARENT_SCOPE)
  endif()
endfunction()

# verifyTraceloggingDependencies(TRACELOGGING_SINK_ERROR) 
# if (NOT TRACELOGGING_SINK_ERROR)
# ... syslog is available, start using it.
function(verifyTraceloggingDependencies VARNAME)
include(CheckIncludeFileCXX)
check_include_file_cxx("TraceLoggingProvider.h" HAVE_TRACELOGGINGPROVIDER_H)
  if(NOT HAVE_TRACELOGGINGPROVIDER_H)
    set(${VARNAME}
        "Could not find TraceLoggingProvider. g3tracelogging [sink, test,example] will not be built"
        PARENT_SCOPE)
  endif()
endfunction()



# ==== print all variables ====
function(printAllCMakeVariables)
  get_cmake_property(_variableNames VARIABLES)
  list(SORT _variableNames)
  foreach(_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endfunction()

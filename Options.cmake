# =====================================================
# Public Domain: Inspired by github.com/KjellKod/g3log/Options.cmake 2020,
# @author Kjell.Hedstrom
# =====================================================

# STATIC/SHARED SETUP - KJELLKOD TEST THIS BETTER
option(CHOICE_BUILD_STATIC
       "Building sinks as static libraries. Linking with static g3log library"
       OFF)

option(CHOICE_BUILD_DEBUG "Build for DEBUG" OFF)

# TEST, EXAMPLES SETUP
option(CHOICE_BUILD_TESTS "Build the unit tests" ON)
option(CHOICE_BUILD_EXAMPLES "Build the examples" ON)

# SINKS
option(CHOICE_logrotate "Build the logrotate sink" ON)
option(CHOICE_syslog "Build the syslog sink" ON)
option(CHOICE_snippets "Build the syslog sink" ON)

if(CHOICE_BUILD_TESTS)
  enable_testing()
endif()

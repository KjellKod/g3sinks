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
option(CHOICE_SINK_LOGROTATE "Build the logrotate sink" ON)
option(CHOICE_SINK_SYSLOG "Build the syslog sink" ON)
option(CHOICE_SINK_SNIPPETS "Build the syslog sink" ON)

if(CHOICE_BUILD_TESTS)
  enable_testing()
endif()

option(CHOICE_INSTALL_G3SINKS "Enable installation of g3sinks. (Projects embedding g3sinks may want to turn this OFF.)" ON)

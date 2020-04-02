
cmake_minimum_required(VERSION 3.2 )

option (BUILD_STATIC "Building sinks as static libraries. Linking with static g3log library" OFF)

add_subdirectory(syslog)
add_subdirectory(logrotate)
add_subdirectory(snippets)

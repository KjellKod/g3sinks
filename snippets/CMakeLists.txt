cmake_minimum_required(VERSION 3.2 )

# Copied from syslog's cmakelists:
# search for g3log
find_package(g3logger REQUIRED)
# add the missing target if needed
if (NOT TARGET g3logger)
    add_library(g3logger SHARED IMPORTED)
    find_package(Threads REQUIRED)
    set_target_properties(g3logger PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${G3LOG_INCLUDE_DIRS}"
        IMPORTED_LOCATION "${G3LOG_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES  Threads::Threads
        )
endif()


add_executable(g3log_snippets_file_example test_FileLog.cpp)

SET_TARGET_PROPERTIES(g3log_snippets_file_example PROPERTIES 
                      LINKER_LANGUAGE CXX
                      OUTPUT_NAME g3log_snippets_file_example
                      CLEAN_DIRECT_OUTPUT 1
                      CXX_STANDARD 14
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      )



target_link_libraries(g3log_snippets_file_example g3logger)





cmake_minimum_required(VERSION 3.2 )

# Copied from syslog's cmakelists:
# search for g3log
# if (NOT ${APPLE})
#    
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
   

   #FileLog Sink example
   check_include_file("sys/mman.h" HAVE_MMAN)
   check_include_file("sys/memfd.h" HAVE_MEMFD)
   CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
   
   add_executable(g3log_snippets_file_example example_FileLog.cpp)
   SET_TARGET_PROPERTIES(g3log_snippets_file_example PROPERTIES 
                         LINKER_LANGUAGE CXX
                         OUTPUT_NAME g3log_snippets_file_example
                         CLEAN_DIRECT_OUTPUT 1
                         )
   target_include_directories(g3log_snippets_file_example PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
   target_link_libraries(g3log_snippets_file_example g3logger)


   #ColoredCout Sink example
   add_executable(g3log_snippets_coloredcout_example example_ColoredCout.cpp)
   SET_TARGET_PROPERTIES(g3log_snippets_coloredcout_example PROPERTIES 
                         LINKER_LANGUAGE CXX
                         OUTPUT_NAME g3log_snippets_coloredcout_example
                         CLEAN_DIRECT_OUTPUT 1
                         )
   target_include_directories(g3log_snippets_coloredcout_example PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
   target_link_libraries(g3log_snippets_coloredcout_example g3logger)





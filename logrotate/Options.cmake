

# Override the default path for g3log library
#example1:  cmake -DG3_LIBRARY_PATH="/usr/local/probe/lib"
#example2:  cmake -DG3_LIBRARY_PATH="/home/kjell/Github/g3log/build"
#http://stackoverflow.com/questions/12984181/specifying-include-directories-on-the-cmake-command-line

#
#set(YOURLIB_INCLUDE_DIR "" CACHE FILEPATH "Path to yourlib includes")
#
#if(NOT YOURLIB_INCLUDE_DIR/header.h)
#  message(SEND_ERROR "Can't find header.h in ${YOURLIB_INCLUDE_DIR})
#endif()
#
#include_directories(${YOURLIB_INCLUDE_DIR})


SET(G3_LIBRARY_PATH "" CACHE FILEPATH "Path to g3log libraries")
IF(EXISTS "${G3_LIBRARY_PATH}/libg3logger_shared.so" )
   MESSAGE("SUCCESS lib")
ELSE()
  MESSAGE(SEND_ERROR "Can't find g3logger library file in G3_LIBRARY_PATH/libg3logger_shared.so:'${G3_LIBRARY_PATH}/libg3logger_shared.so'. Please set it with '-DG3_LIBRARY_PATH'")
ENDIF()



SET(G3_HEADER_PATHa "" CACHE FILEPATH "Path to g3log headers")
IF(EXISTS ${G3_HEADER_PATH}/g3log/g3log.hpp)
   MESSAGE("SUCCESS path")
ELSE()
  MESSAGE(FATAL_ERROR "Can't find g3log.hpp at  G3_HEADER_PATH/g3log/g3log.hpp, i.e. '${G3_HEADER_PATH}/g3log/g3log.hpp'. Please set it with '-DG3_HEADER_PATH'")
ENDIF()



#option (USE_G3_LIBRARY_PATH
#       "Find g3log libraries by this path instead of default" OFF)
#SET(G3_LIBRARY_PATH_DEFAULT "/usr/local/lib/")
#IF (${USE_G3PATH_LIBRARY} )
#   SET(G3_LIBRARY_PATH ${USE_G3_LIBRARY_PATH})
#ELSE()
#   SET(G3_LIBRARY_PATH ${G3_LIBRARY_PATH_DEFAULT})
#ENDIF()

#MESSAGE("G3_LIBRARY_PATH = " ${G3_LIBRARY_PATH})
#MESSAGE("You can change it with  cmake -DUSE_G3_LIBRARY_PATH=path_to_libraries (i.e. something like /usr/local/lib)")



# Override the default path for looking up g3log headers 
#example1:  cmake -DUSE_G3_HEADER_PATH="/usr/local/probe/include/"
#example2:  cmake -DUSE_G3_HEADER_PATH="/home/kjell/Github/g3log/src"
#option (USE_G3_HEADER_PATH
#       "Find g3log headers by this path instead of default" OFF)
#SET(G3_HEADER_PATH_DEFAULT "/usr/local/include/")
#
#IF (${USE_G3_HEADER_PATH} ) 
#   SET(G3_HEADER_PATH ${USE_G3_HEADER_PATH})
#ELSE()
#   SET(G3_HEADER_PATH ${G3_HEADER_PATH_DEFAULT})
#ENDIF()

#MESSAGE("G3_HEADER_PATH = " ${G3_HEADER_PATH})
#MESSAGE("You can change it with  cmake -DUSE_G3_HEADER_PATH=path_to_headers (i.e. something like /usr/local/include)")


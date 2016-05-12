# =====================================================
#  Public Domain: Inspired by github.com/KjellKod/g3log/Options.cmake
#  2015, August 17, @author Kjell.Hedstrom
# =====================================================


# SHARED or STATIC library build --- also will re-use this to 
# use SHARED g3log library or STATIC g3log library
#
# example: cmake -DBUILD_STATIC=ON .. 
option (BUILD_STATIC "Building g3logrotate as static library. Linking with static g3log library" OFF)
IF (BUILD_STATIC)
  SET(G3LOG_LIBRARY_TYPE g3logger)
  MESSAGE("'OPTION: cmake -DBUILD_STATIC=ON'\nLinking statically with g3log. Creating static library for g3logrotate")
  
  IF (MSVC)
    SET(G3LOG_LIBRARY_NAME_DEBUG g3logger_d.lib)
	SET(G3LOG_LIBRARY_NAME_RELEASE g3logger.lib)
  ELSE()
    SET(G3LOG_LIBRARY_NAME_DEBUG libg3logger.a)
	SET(G3LOG_LIBRARY_NAME_RELEASE libg3logger.a)
  ENDIF()
ELSE()
   IF(NOT(CMAKE_VERSION LESS 3.4) AND MSVC)
      set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
   ENDIF()
   
  SET(G3LOG_LIBRARY_TYPE g3logger_shared)
  MESSAGE("'OPTION cmake -DBUILD_STATIC=OFF'\nLinking dynamically with g3log. Creating dynamic library for g3logrotate")
  
  IF (MSVC)
    SET(G3LOG_LIBRARY_NAME_DEBUG g3logger_shared_d.lib)
	SET(G3LOG_LIBRARY_NAME_RELEASE g3logger_shared.lib)
  ELSE()
	SET(G3LOG_LIBRARY_NAME libg3logger_shared.so)
  ENDIF()
ENDIF()


# Override the default path for g3log library
#example1:  cmake -DG3_LIBRARY_PATH="/usr/local/probe/lib"
#example2:  cmake -DG3_LIBRARY_PATH="/home/kjell/Github/g3log/build"
#
SET(G3_LIBRARY_PATH_DEBUG "" CACHE FILEPATH "Path to g3log debug libraries")
SET(G3_LIBRARY_PATH_RELEASE "" CACHE FILEPATH "Path to g3log release libraries")
IF(EXISTS "${G3_LIBRARY_PATH_DEBUG}/${G3LOG_LIBRARY_NAME_DEBUG}")
   MESSAGE("SUCCESS lib")
ELSE()
  MESSAGE(SEND_ERROR "Can't find g3logger library file in G3_LIBRARY_PATH_DEBUG/${G3LOG_LIBRARY_NAME_DEBUG}:' ${G3_LIBRARY_PATH_DEBUG}'. Please set it with '-DG3_LIBRARY_PATH' \nExample run could be something like:\ncmake -DG3_HEADER_PATH=/home/X/ws/g3log/src/ -DG3_LIBRARY_PATH=/home/X/ws/g3log/build -DADD_LOGROTATE_UNIT_TEST=ON")
ENDIF()

IF(EXISTS "${G3_LIBRARY_PATH_RELEASE}/${G3LOG_LIBRARY_NAME_RELEASE}")
   MESSAGE("SUCCESS lib")
ELSE()
  MESSAGE(SEND_ERROR "Can't find g3logger library file in G3_LIBRARY_PATH_RELEASE/${G3LOG_LIBRARY_NAME_RELEASE}:' ${G3_LIBRARY_PATH_RELEASE}'. Please set it with '-DG3_LIBRARY_PATH' \nExample run could be something like:\ncmake -DG3_HEADER_PATH=/home/X/ws/g3log/src/ -DG3_LIBRARY_PATH=/home/X/ws/g3log/build -DADD_LOGROTATE_UNIT_TEST=ON")
ENDIF()


# Override the default include path to g3log library
#example1:  cmake -DUSE_G3_HEADER_PATH="/usr/local/probe/include/"
#example2:  cmake -DUSE_G3_HEADER_PATH="/home/kjell/Github/g3log/src"
#
SET(G3_HEADER_PATH "" CACHE FILEPATH "Path to g3log headers")
IF(EXISTS ${G3_HEADER_PATH}/g3log/g3log.hpp)
   MESSAGE("SUCCESS path")
ELSE()
  MESSAGE(FATAL_ERROR "Can't find g3log.hpp at G3_HEADER_PATH/g3log/g3log.hpp, i.e. '${G3_HEADER_PATH}/g3log/g3log.hpp'. Please set it with '-DG3_HEADER_PATH'. \nExample run could be something like:\ncmake -DG3_HEADER_PATH=/home/X/ws/g3log/src/ -DG3_LIBRARY_PATH=/home/X/ws/g3log/build -DADD_LOGROTATE_UNIT_TEST=ON")
ENDIF()


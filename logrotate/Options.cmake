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

  IF (MSVC)
	SET(G3LOG_LIBRARY_NAME g3logger.lib)
  ELSE()
	SET(G3LOG_LIBRARY_NAME libg3logger.a)
  ENDIF()
  
  MESSAGE("'OPTION: cmake -DBUILD_STATIC=ON'\nLinking statically with g3log. Creating static library for g3logrotate")
ELSE()
  IF (MSVC)
   	 SET(G3LOG_LIBRARY_NAME g3logger.lib)
  ELSE()
 	   SET(G3LOG_LIBRARY_NAME libg3logger.so)
  ENDIF()
  
  MESSAGE("'OPTION cmake -DBUILD_STATIC=OFF'\nLinking dynamically with g3log. Creating dynamic library for g3logrotate")
ENDIF()

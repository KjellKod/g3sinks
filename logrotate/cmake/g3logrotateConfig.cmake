#.rst:
# Findg3LogRotate
# -------
#
# Find libg3logrotate, A sink for the g3log library (https://github.com/KjellKod/g3log)
# g3logrotate is a public domain dedication, http://unlicense.org
#
# Result variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``g3LOGROTATE_INCLUDE_DIRS``
#   where to find LogRotate.h  LogRotateUtility.h  LogRotateWithFilter.h

# ``G3LOGROTATE_LIBRARIES``
#   the libraries to link against to use libLogRotate.h  LogRotateUtility.h  LogRotateWithFilter.h.
#   that includes libgMurMurHash library files.

# ``G3LOGROTATE_FOUND``
#   If false, do not try to use g3logrotate.

include(FindPackageHandleStandardArgs)

find_path(G3LOGROTATE_INCLUDE_DIR ${PROJECT_SRC}/LogRotate.h  ${PROJECT_SRC}/LogRotateUtility.h  ${PROJECT_SRC}/LogRotateWithFilter.h)

find_library(G3LOGROTATE_LIBRARY
            NAMES libg3logrotate g3logrotate)

find_package_handle_standard_args(G3LOGROTATE  DEFAULT_MSG
            G3LOGROTATE_INCLUDE_DIR G3LOGROTATE_LIBRARY)

mark_as_advanced(G3LOGROTATE_INCLUDE_DIR  G3LOGROTATE_LIBRARY)

set( G3LOGROTATE_LIBRARIES ${ G3LOGROTATE_LIBRARY})
set( G3LOGROTATE_INCLUDE_DIRS ${ G3LOGROTATE_INCLUDE_DIR})
# Find Sqlite3
# ~~~~~~~~~~~~
# Copyright (c) 2007, Martin Dobias <wonder.sk at gmail.com>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# CMake module to search for Sqlite3 library
#
# If it's found it sets SQLITE3_FOUND to TRUE
# and following variables are set:
#    SQLITE3_INCLUDE_DIR
#    SQLITE3_LIBRARY


# find_path and find_library normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing.

# try to use framework on mac
# want clean framework path, not unix compatibility path
if(APPLE)
  if(CMAKE_FIND_FRAMEWORK MATCHES "FIRST"
      OR CMAKE_FRAMEWORK_PATH MATCHES "ONLY"
      OR NOT CMAKE_FIND_FRAMEWORK)
    set(CMAKE_FIND_FRAMEWORK_save ${CMAKE_FIND_FRAMEWORK} CACHE STRING "" FORCE)
    set(CMAKE_FIND_FRAMEWORK "ONLY" CACHE STRING "" FORCE)
    #find_path(SQLITE3_INCLUDE_DIR SQLite3/sqlite3.h)
    find_library(SQLITE3_LIBRARY SQLite3)
    if(SQLITE3_LIBRARY)
      # find_path doesn't add "Headers" for a framework
      set(SQLITE3_INCLUDE_DIR ${SQLITE3_LIBRARY}/Headers
        CACHE PATH "Path to a file.")
    endif()
    set(CMAKE_FIND_FRAMEWORK ${CMAKE_FIND_FRAMEWORK_save} CACHE STRING "" FORCE)
  endif()
endif()

find_path(SQLITE3_INCLUDE_DIR sqlite3.h
  "$ENV{LIB_DIR}/include"
  "$ENV{LIB_DIR}/include/sqlite"
  "$ENV{INCLUDE}"
)

find_library(SQLITE3_LIBRARY NAMES sqlite3_i sqlite3 PATHS
  "$ENV{LIB_DIR}/lib"
  "$ENV{LIB}/lib"
)

if(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
  set(SQLITE3_FOUND TRUE)
endif()

if(SQLITE3_FOUND)
  if(NOT SQLITE3_FIND_QUIETLY)
    message(STATUS "Found Sqlite3: ${SQLITE3_LIBRARY}")
  endif()

else()

  if(SQLITE3_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find Sqlite3")
  endif()

endif()

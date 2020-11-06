################################################################################
#
# This file is part of CMake configuration for PROJ library (inspired from SOCI
# CMake,  Copyright (C) 2009-2010 Mateusz Loskot <mateusz@loskot.net> )
#
# Copyright (C) 2011 Nicolas David <nicolas.david@ign.fr>
# Distributed under the MIT license
#
################################################################################
# General settings
################################################################################
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(PROJ
  DESCRIPTION "PROJ coordinate transformation software library"
  LANGUAGES C CXX
)

# Only interpret if() arguments as variables or keywords when unquoted
cmake_policy(SET CMP0054 NEW)

# Set C++ version
# Make CMAKE_CXX_STANDARD available as cache option overridable by user
set(CMAKE_CXX_STANDARD 11
  CACHE STRING "C++ standard version to use (default is 11)")
message(STATUS "Requiring C++${CMAKE_CXX_STANDARD}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "Requiring C++${CMAKE_CXX_STANDARD} - done")

# Set C99 version
# Make CMAKE_C_STANDARD available as cache option overridable by user
set(CMAKE_C_STANDARD 99
  CACHE STRING "C standard version to use (default is 99)")
message(STATUS "Requiring C${CMAKE_C_STANDARD}")
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
message(STATUS "Requiring C${CMAKE_C_STANDARD} - done")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if(APPLE)
    set(MACOSX_RPATH ON)
endif()

# Set global -fvisibility=hidden
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Set warnings as variables, then store as cache options
set(PROJ_common_WARN_FLAGS  # common only to GNU/Clang C/C++
  -Wall
  -Wextra
  -Wswitch
  -Wshadow
  -Wunused-parameter
  -Wmissing-declarations
  -Wformat
  -Wformat-security
)
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set(PROJ_C_WARN_FLAGS ${PROJ_common_WARN_FLAGS}
    -Wmissing-prototypes
  )
  set(PROJ_CXX_WARN_FLAGS ${PROJ_common_WARN_FLAGS})
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
  set(PROJ_C_WARN_FLAGS ${PROJ_common_WARN_FLAGS}
    -Wmissing-prototypes
    -Wfloat-conversion
    -Wc11-extensions
  )
  set(PROJ_CXX_WARN_FLAGS ${PROJ_common_WARN_FLAGS}
    -Wfloat-conversion
  )
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  add_definitions(/D_CRT_SECURE_NO_WARNINGS) # Eliminate deprecation warnings
  set(PROJ_C_WARN_FLAGS
    /W4
    /wd4706  # Suppress warning about assignment within conditional expression
    /wd4996  # Suppress warning about sprintf, etc., being unsafe
  )
  set(PROJ_CXX_WARN_FLAGS /EHsc ${PROJ_C_WARN_FLAGS})
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
  if(MSVC)
    set(PROJ_C_WARN_FLAGS /Wall)
    set(PROJ_CXX_WARN_FLAGS /Wall)
  else()
    set(PROJ_C_WARN_FLAGS -Wall)
    set(PROJ_CXX_WARN_FLAGS -Wall)
  endif()
endif()

set(PROJ_C_WARN_FLAGS "${PROJ_C_WARN_FLAGS}"
  CACHE STRING "C flags used to compile PROJ targets")
set(PROJ_CXX_WARN_FLAGS "${PROJ_CXX_WARN_FLAGS}"
  CACHE STRING "C++ flags used to compile PROJ targets")

################################################################################
# PROJ CMake modules
################################################################################
# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${PROJ_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(ProjUtilities)

message(STATUS "Configuring PROJ:")

################################################################################
#PROJ version information
################################################################################
include(ProjVersion)
proj_version(MAJOR 8 MINOR 0 PATCH 0)
set(PROJ_API_VERSION "19")
set(PROJ_BUILD_VERSION "21.0.2")

################################################################################
# Build features and variants
################################################################################
include(ProjConfig)
include(ProjMac)
include(policies)

################################################################################
# Check for sqlite3
################################################################################
find_program(EXE_SQLITE3 sqlite3)
if(NOT EXE_SQLITE3)
  message(SEND_ERROR "sqlite3 binary not found!")
endif()

find_package(Sqlite3 REQUIRED)
if(NOT SQLITE3_FOUND)
  message(SEND_ERROR "sqlite3 dependency not found!")
endif()

# Would build and run with older versions, but with horrible performance
# See https://github.com/OSGeo/PROJ/issues/1718
if("${SQLITE3_VERSION}" VERSION_LESS "3.11")
  message(SEND_ERROR "sqlite3 >= 3.11 required!")
endif()

################################################################################
# Check for libtiff
################################################################################

option(ENABLE_TIFF "Enable TIFF support to read some grids" ON)
mark_as_advanced(ENABLE_TIFF)
set(TIFF_ENABLED FALSE)
if(ENABLE_TIFF)
  find_package(TIFF REQUIRED)
  if(TIFF_FOUND)
    set(TIFF_ENABLED TRUE)
  else()
    message(SEND_ERROR
      "libtiff dependency not found! Use ENABLE_TIFF=OFF to force it off")
  endif()
else()
  message(WARNING
    "TIFF support is not enabled and will result in the inability to read "
    "some grids")
endif()

################################################################################
# Check for curl
################################################################################

option(ENABLE_CURL "Enable Curl support" ON)
set(CURL_ENABLED FALSE)
if(ENABLE_CURL)
  find_package(CURL REQUIRED)
  if(CURL_FOUND)
    set(CURL_ENABLED TRUE)
  else()
    message(SEND_ERROR "curl dependency not found!")
  endif()
endif()

################################################################################
# threading configuration
################################################################################
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads)

include(CheckIncludeFiles)

include(CheckCSourceCompiles)
if(MSVC)
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS} /WX /W4")
else()
  set(CMAKE_REQUIRED_LIBRARIES m)
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall")
endif()

if(Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
  set(CMAKE_REQUIRED_LIBRARIES
    "${CMAKE_REQUIRED_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}")
  check_c_source_compiles("
#include <pthread.h>

int main(int argc, char* argv[]) {
  (void)PTHREAD_MUTEX_RECURSIVE;
  (void)argv;
  return argc;
}
  " HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)
  if(HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)
    add_definitions(-DHAVE_PTHREAD_MUTEX_RECURSIVE=1)
  endif()
endif()

# Set a default build type for single-configuration cmake generators if
# no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC OR CMAKE_CONFIGURATION_TYPES)
  # For multi-config systems and for Visual Studio, the debug version of
  # the library has _d appended.
  set(CMAKE_DEBUG_POSTFIX _d)
endif()

# Put the libraries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed PROJ libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJ_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJ_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJ_BINARY_DIR}/bin)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

################################################################################
# Installation
################################################################################
include(ProjInstallPath)
set(BINDIR "${DEFAULT_BINDIR}"
  CACHE PATH "The directory to install binaries into.")
set(LIBDIR "${DEFAULT_LIBDIR}"
  CACHE PATH "The directory to install libraries into.")
set(DATADIR "${DEFAULT_DATADIR}"
  CACHE PATH "The directory to install data files into.")
set(DOCDIR "${DEFAULT_DOCDIR}"
  CACHE PATH "The directory to install doc files into.")
set(INCLUDEDIR "${DEFAULT_INCLUDEDIR}"
  CACHE PATH "The directory to install includes into.")
set(CMAKECONFIGDIR "${DEFAULT_CMAKEDIR}"
  CACHE PATH "Parent of the directory to install cmake config files into.")

################################################################################
# Tests
################################################################################
include(CTest)

# Support older option, to be removed by PROJ 8.0
if(DEFINED PROJ_TESTS)
  message(DEPRECATION "PROJ_TESTS has been replaced with BUILD_TESTING")
  set(BUILD_TESTING ${PROJ_TESTS})
endif()

if(BUILD_TESTING)
  include(ProjTest)
else()
  message(STATUS "Testing disabled")
endif()

################################################################################
# Build configured components
################################################################################
include_directories(${PROJ_SOURCE_DIR}/src)

add_subdirectory(data)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(man)
add_subdirectory(cmake)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

set(docfiles COPYING NEWS AUTHORS)
install(FILES ${docfiles}
        DESTINATION "${DOCDIR}")

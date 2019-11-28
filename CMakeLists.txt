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
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# For historic reasons, the CMake PROJECT-NAME is PROJ4
project(PROJ4 LANGUAGES C CXX)
set(PROJECT_INTERN_NAME PROJ)

if(NOT CMAKE_VERSION VERSION_LESS 3.1)
  cmake_policy(SET CMP0054 NEW)
endif()

# Set C++ version
# Make CMAKE_CXX_STANDARD available as cache option overridable by user
set(CMAKE_CXX_STANDARD 11
  CACHE STRING "C++ standard version to use (default is 11)")
message(STATUS "Requiring C++${CMAKE_CXX_STANDARD}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "Requiring C++${CMAKE_CXX_STANDARD} - done")

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
    -Wc99-extensions
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
set(CMAKE_MODULE_PATH ${PROJ4_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(ProjUtilities)

message(STATUS "")
colormsg(_HIBLUE_ "Configuring PROJ:")

################################################################################
#PROJ version information
################################################################################
include(ProjVersion)
proj_version(MAJOR 6 MINOR 2 PATCH 1)
set(PROJ_API_VERSION "17")
set(PROJ_BUILD_VERSION "17.1.2")

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
# Check whether the C99 math function: hypot, atanh, etc. are available.
check_c_source_compiles("
#include <math.h>
int main() {
  int q;
  return (int)(
    hypot(3.0, 4.0) + atanh(0.8) + cbrt(8.0) +
    remquo(100.0, 90.0, &q) +
    remainder(100.0, 90.0) + copysign(1.0, -0.0) +
    log1p(0.1) + asinh(0.1)) + isnan(0.0);
}
" C99_MATH)
if(C99_MATH)
  add_definitions(-DHAVE_C99_MATH=1)
else()
  add_definitions(-DHAVE_C99_MATH=0)
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

option(PROJ_TESTS "Enable build of collection of PROJ tests" ON)
boost_report_value(PROJ_TESTS)
if(PROJ_TESTS)
  enable_testing()
endif()
include(ProjTest)

# Put the libraries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
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
  CACHE PATH "The directory to install cmake config files into.")

################################################################################
# Build configured components
################################################################################
include_directories(${PROJ4_SOURCE_DIR}/src)

message(STATUS "")
add_subdirectory(data)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(man)
add_subdirectory(cmake)
if(PROJ_TESTS)
  add_subdirectory(test)
endif()

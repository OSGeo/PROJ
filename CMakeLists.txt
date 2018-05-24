#################################################################################
#
# This file is part of CMake configuration for PROJ library (inspired from SOCI
# CMake,  Copyright (C) 2009-2010 Mateusz Loskot <mateusz@loskot.net> )
#
# Copyright (C) 2011 Nicolas David <nicolas.david@ign.fr>
# Distributed under the MIT license
#
#################################################################################
# General settings
#################################################################################
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# For historic reasons, the CMake PROJECT-NAME is PROJ4
project(PROJ4 C)
set(PROJECT_INTERN_NAME PROJ)

if (NOT CMAKE_VERSION VERSION_LESS 3.1)
  cmake_policy(SET CMP0054 NEW)
endif ()

# Set warnings
if("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  # Suppress warning 4706 about assignment within conditional expression
  # Suppress warning 4996 about sprintf, etc., being unsafe
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /wd4706 /wd4996 /D_CRT_SECURE_NO_WARNINGS")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wswitch -Wshadow -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wformat -Wformat-security")
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wswitch -Wshadow -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wformat -Wformat-security -Wfloat-conversion -Wc99-extensions -Wc11-extensions")
endif()

# Tell Intel compiler to do arithmetic accurately.  This is needed to
# stop the compiler from ignoring parentheses in expressions like
# (a + b) + c and from simplifying 0.0 + x to x (which is wrong if
# x = -0.0).
if (CMAKE_C_COMPILER_ID STREQUAL "Intel")
  if (MSVC)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:precise")
  else ()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fp-model precise")
  endif ()
endif ()

#################################################################################
# PROJ CMake modules
#################################################################################
# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${PROJ4_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(ProjUtilities)

message(STATUS "")
colormsg(_HIBLUE_ "Configuring PROJ:")

#################################################################################
#PROJ version information
#################################################################################
include(ProjVersion)
proj_version(MAJOR 5 MINOR 0 PATCH 0)
set(PROJ_API_VERSION "14")
set(PROJ_BUILD_VERSION "14.0.1")

#################################################################################
# Build features and variants
#################################################################################
include(ProjSystemInfo)
include(ProjConfig)
include(ProjMac)
include(policies)

#################################################################################
# threading configuration
#################################################################################
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package (Threads)

include(CheckIncludeFiles)
include(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(PTHREAD_MUTEX_RECURSIVE pthread.h HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)
if (HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)
    add_definitions(-DHAVE_PTHREAD_MUTEX_RECURSIVE=1)
endif()

include (CheckCSourceCompiles)
if (MSVC)
  set (CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS} /WX")
else ()
  set (CMAKE_REQUIRED_LIBRARIES m)
  set (CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif ()
# Check whether the C99 math function: hypot, atanh, etc. are available.
check_c_source_compiles (
  "#include <math.h>
int main() {
  int q;
  return (int)(hypot(3.0, 4.0) + atanh(0.8) + cbrt(8.0) +
               remquo(100.0, 90.0, &q) +
               remainder(100.0, 90.0) + copysign(1.0, -0.0) +
               log1p(0.1) + asinh(0.1)) +
               isnan(0.0);
}\n" C99_MATH)
if (C99_MATH)
  add_definitions (-DHAVE_C99_MATH=1)
else ()
  add_definitions (-DHAVE_C99_MATH=0)
endif ()

boost_report_value(PROJ_PLATFORM_NAME)
boost_report_value(PROJ_COMPILER_NAME)

# Set a default build type for single-configuration cmake generators if
# no build type is set.
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif ()

if (MSVC OR CMAKE_CONFIGURATION_TYPES)
  # For multi-config systems and for Visual Studio, the debug version of
  # the library has _d appended.
  set (CMAKE_DEBUG_POSTFIX _d)
endif ()

option(PROJ_TESTS "Enable build of collection of PROJ tests" ON)
boost_report_value(PROJ_TESTS)
if(PROJ_TESTS)
    include(CTest)
    enable_testing()
endif(PROJ_TESTS)
include(ProjTest)

# Put the libraries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#################################################################################
# Installation
#################################################################################
include(ProjInstallPath)
set(BINDIR "${DEFAULT_BINDIR}" CACHE PATH "The directory to install binaries into.")
set(LIBDIR "${DEFAULT_LIBDIR}" CACHE PATH "The directory to install libraries into.")
set(DATADIR "${DEFAULT_DATADIR}" CACHE PATH "The directory to install data files into.")
set(DOCDIR "${DEFAULT_DOCDIR}" CACHE PATH "The directory to install doc files into.")
set(INCLUDEDIR "${DEFAULT_INCLUDEDIR}" CACHE PATH "The directory to install includes into.")
set(CMAKECONFIGDIR "${DEFAULT_CMAKEDIR}" CACHE PATH "The directory to install cmake config files into.")

#################################################################################
# Build configured components
#################################################################################
include_directories(${PROJ4_SOURCE_DIR}/src)

message(STATUS "")
add_subdirectory(nad)
add_subdirectory(src)
add_subdirectory(man)
add_subdirectory(cmake)
add_subdirectory(test)


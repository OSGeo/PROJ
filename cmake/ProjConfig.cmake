################################################################################
# ProjConfig.cmake - CMake build configuration of PROJ library
################################################################################
# Copyright (C) 2010 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)
################################################################################
include(CheckLibraryExists)
include(CheckFunctionExists)

# if C flags have -Werror, temporarily remove these while running some checks
string(FIND "${CMAKE_C_FLAGS}" "-Werror" FIND_WERROR)
if(FIND_WERROR GREATER_EQUAL 0)
  # we must be careful about not matching -Werror=something, so let's append
  # a space at the end of CMAKE_C_FLAGS and match -Werror with a trailing space
  string(REPLACE "-Werror " " " _tmp_CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ")
  set(_prev_CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  set(CMAKE_C_FLAGS "${_tmp_CMAKE_C_FLAGS}")
endif()

# check needed include file
check_function_exists(localeconv HAVE_LOCALECONV)
check_function_exists(strerror HAVE_STRERROR)
if(NOT WIN32)
  check_library_exists(dl dladdr "" HAVE_LIBDL)
  check_library_exists(m exp "" HAVE_LIBM)
endif()

# restore CMAKE_C_FLAGS as before
if(FIND_WERROR GREATER_EQUAL 0)
  set(CMAKE_C_FLAGS "${_prev_CMAKE_C_FLAGS}")
endif()

set(PACKAGE "proj")
set(PACKAGE_BUGREPORT "https://github.com/OSGeo/PROJ/issues")
set(PACKAGE_NAME "PROJ")
set(PACKAGE_STRING "PROJ ${${PROJECT_NAME}_VERSION}")
set(PACKAGE_TARNAME "proj")
set(PACKAGE_URL "https://proj.org")
set(PACKAGE_VERSION "${${PROJECT_NAME}_VERSION}")

# check if a second proj_config.h exists (created by ./configure)
# as this is within CMake's C_INCLUDES / CXX_INCLUDES
set(AUTOCONF_PROJ_CONFIG_H "${PROJ_SOURCE_DIR}/src/proj_config.h")
if(EXISTS ${AUTOCONF_PROJ_CONFIG_H})
  message(WARNING
    "Autoconf's ${AUTOCONF_PROJ_CONFIG_H} may interfere with this "
    "CMake build. Run 'make distclean' in the source directory "
    "before CMake's build.")
endif()

# Get the git hash to write it in the release message.
set(PROJ_GIT_HASH "")
if(DEFINED ENV{PROJ_GITHUB_SHA})
    string(SUBSTRING "$ENV{PROJ_GITHUB_SHA}" 0 7 PROJ_GIT_HASH)
else()
    find_package(Git QUIET)
    if(Git_FOUND AND EXISTS "${PROJ_SOURCE_DIR}/.git")
        # Get the short hash
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY "${PROJ_SOURCE_DIR}"
            RESULT_VARIABLE GIT_REV_RESULT
            OUTPUT_VARIABLE PROJ_GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        if(GIT_REV_RESULT EQUAL 0)
            # Check if the working directory is dirty
            execute_process(
                COMMAND ${GIT_EXECUTABLE} status --porcelain --untracked-files=no
                WORKING_DIRECTORY "${PROJ_SOURCE_DIR}"
                RESULT_VARIABLE GIT_STATUS_RESULT
                OUTPUT_VARIABLE GIT_STATUS_OUTPUT
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
            )

            # If the command succeeds and outputs anything, the tree is dirty
            if(GIT_STATUS_RESULT EQUAL 0 AND NOT GIT_STATUS_OUTPUT STREQUAL "")
                string(APPEND PROJ_GIT_HASH "*")
            endif()
        else()
            set(PROJ_GIT_HASH "")
        endif()
    endif()
endif()
if(PROJ_GIT_HASH)
    add_compile_definitions(PROJ_GIT_HASH=\"${PROJ_GIT_HASH}\")
endif()
message(PROJ_GIT_HASH=\"${PROJ_GIT_HASH}\")

# Tell CMake to re-configure if the Git HEAD or index changes
if(Git_FOUND AND EXISTS "${PROJ_SOURCE_DIR}/.git")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${PROJ_SOURCE_DIR}/.git/HEAD" # for the hash
        "${PROJ_SOURCE_DIR}/.git/index" # for the *
    )
endif()

configure_file(cmake/proj_config.cmake.in src/proj_config.h)

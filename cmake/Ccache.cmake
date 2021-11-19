#
# CMake module to support ccache
#
# see https://crascit.com/2016/04/09/using-ccache-with-cmake/
#

find_program(CCACHE_PROGRAM ccache)
# Note: REQUIRED keyword introduced in CMake 3.18

if(CCACHE_PROGRAM)
  message(STATUS "Configuring ccache with ${CCACHE_PROGRAM}")

  set(C_LAUNCHER   "${CCACHE_PROGRAM}")
  set(CXX_LAUNCHER "${CCACHE_PROGRAM}")

  # Set up wrapper scripts
  set(CCACHE_LAUNCH_C ${CMAKE_BINARY_DIR}/ccache-launch-c)
  file(WRITE ${CCACHE_LAUNCH_C} "\
#!/bin/sh

# Xcode generator doesn't include the compiler as the
# first argument, Ninja and Makefiles do. Handle both cases.
if [ \"$1\" = \"${CMAKE_C_COMPILER}\" ] ; then
    shift
fi

export CCACHE_CPP2=true
exec \"${C_LAUNCHER}\" \"${CMAKE_C_COMPILER}\" \"$@\"
")

  set(CCACHE_LAUNCH_CXX ${CMAKE_BINARY_DIR}/ccache-launch-cxx)
  file(WRITE ${CCACHE_LAUNCH_CXX} "\
#!/bin/sh

# Xcode generator doesn't include the compiler as the
# first argument, Ninja and Makefiles do. Handle both cases.
if [ \"$1\" = \"${CMAKE_CXX_COMPILER}\" ] ; then
    shift
fi

export CCACHE_CPP2=true
exec \"${CXX_LAUNCHER}\" \"${CMAKE_CXX_COMPILER}\" \"$@\"
")

  # Note: file(CHMOD) introduced in CMake 3.19
  execute_process(
    COMMAND chmod a+rx
      "${CCACHE_LAUNCH_C}"
      "${CCACHE_LAUNCH_CXX}"
  )

  if(CMAKE_GENERATOR STREQUAL "Xcode")
    # Set Xcode project attributes to route compilation and linking
    # through our scripts
    set(CMAKE_XCODE_ATTRIBUTE_CC         "${CCACHE_LAUNCH_C}")
    set(CMAKE_XCODE_ATTRIBUTE_CXX        "${CCACHE_LAUNCH_CXX}")
    set(CMAKE_XCODE_ATTRIBUTE_LD         "${CCACHE_LAUNCH_C}")
    set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CCACHE_LAUNCH_CXX}")
  else()
    # Support Unix Makefiles and Ninja
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_LAUNCH_C}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_LAUNCH_CXX}")
  endif()
else()
  message(SEND_ERROR "Ccache requested, but ccache was not found")
endif()

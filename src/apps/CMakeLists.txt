set(CMAKE_UNITY_BUILD OFF)

# Configure executable builds
option(BUILD_APPS
  "Build PROJ applications (default value for BUILD_CCT, BUILD_CS2CS, etc.)"
  ON)

option(BUILD_CCT
  "Build cct (coordinate conversion and transformation tool)"
  "${BUILD_APPS}")
option(BUILD_CS2CS
  "Build cs2cs (coordinate systems to coordinate systems translation tool)"
  "${BUILD_APPS}")
option(BUILD_GEOD
  "Build geod (computation of geodesic lines)"
  "${BUILD_APPS}")
option(BUILD_GIE
  "Build gie (geospatial integrity investigation environment)"
  "${BUILD_APPS}")
option(BUILD_PROJ
  "Build proj (cartographic projection tool)"
  "${BUILD_APPS}")
option(BUILD_PROJINFO
  "Build projinfo (SRS and coordinate operation metadata/query tool)"
  "${BUILD_APPS}")
option(BUILD_PROJSYNC
  "Build projsync (synchronize transformation support data)"
  "${BUILD_APPS}")

if(NOT MSVC)

  # Use relative path so that package is relocatable
  if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
  else()
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
  endif()
  # Other apps can link to libproj using e.g. LDFLAGS -Wl,-rpath,${prefix}/lib

else()

  if(NOT WINDOWS_STORE)
    # Linking to setargv.obj enables wildcard globbing for the
    # command line utilities, when compiling with MSVC
    # https://docs.microsoft.com/cpp/c-language/expanding-wildcard-arguments
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} setargv.obj")
  endif()

endif()

if(BUILD_CCT)
  include(bin_cct.cmake)
  list(APPEND BIN_TARGETS cct)
endif()

if(BUILD_CS2CS)
  include(bin_cs2cs.cmake)
  list(APPEND BIN_TARGETS cs2cs)
endif()

if(BUILD_GEOD)
  include(bin_geod.cmake)
  list(APPEND BIN_TARGETS geod)
endif()

if(BUILD_PROJ)
  include(bin_proj.cmake)
  list(APPEND BIN_TARGETS binproj)
endif()

if(BUILD_PROJINFO)
  include(bin_projinfo.cmake)
  list(APPEND BIN_TARGETS projinfo)
endif()

# Always build gie if testing is requested
if(BUILD_GIE OR BUILD_TESTING)
  include(bin_gie.cmake)
  list(APPEND BIN_TARGETS gie)
endif()

if(BUILD_PROJSYNC)
  if(NOT ENABLE_CURL)
    message(SEND_ERROR "projsync requires Curl")
  endif()
  include(bin_projsync.cmake)
  list(APPEND BIN_TARGETS projsync)
endif()

# global configurations for all apps

if(MSVC OR CMAKE_CONFIGURATION_TYPES)
  if(BIN_TARGETS)
    # Add _d suffix for debug versions of the apps
    set_target_properties(${BIN_TARGETS} PROPERTIES
      DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
  endif()
endif()

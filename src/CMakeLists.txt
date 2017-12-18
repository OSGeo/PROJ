# first include proj library
# always need
include(lib_proj.cmake)

# configure executable build
option(BUILD_CCT     "Build cct (coordinate conversion and transformation tool)" ON)
option(BUILD_CS2CS   "Build cs2cs (coordinate systems to coordinate systems translation tool)" ON)
option(BUILD_GEOD    "Build geod (computation of geodesic lines)" ON)
option(BUILD_GIE     "Build gie (geospatial integrity investigation environment - a PROJ.4 test tool)" ON)
option(BUILD_NAD2BIN "Build nad2bin (format conversion tool)" ON)
option(BUILD_PROJ    "Build proj (cartographic projection tool : latlong <-> projected coordinates)" ON)

if(NOT MSVC)

  if (NOT APPLE)
    # Use relative path so that package is relocatable
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../${LIBDIR}")
  else ()
    set (CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${LIBDIR}")
    # TO DO: cmake 2.8.12 introduces a way to make the install tree
    # relocatable with OSX via
    # (1) set(CMAKE_MACOSX_RPATH ON) and
    # (2) setting the INSTALL_RPATH property on the executables to
    # "@loader_path/../${LIBDIR}"
  endif ()

else ()

    # Linking to setargv.obj enables wildcard globbing for the
    # command line utilities, when compiling with MSVC
    # https://docs.microsoft.com/da-dk/cpp/c-language/expanding-wildcard-arguments
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} setargv.obj")

endif ()

if(BUILD_CCT)
 include(bin_cct.cmake)
 set(BIN_TARGETS ${BIN_TARGETS} cct)
endif(BUILD_CCT)

if(BUILD_CS2CS)
 include(bin_cs2cs.cmake)
 set(BIN_TARGETS ${BIN_TARGETS} cs2cs)
endif(BUILD_CS2CS)

if(BUILD_GEOD)
  include(bin_geod.cmake)
  include(bin_geodtest.cmake)
  set(BIN_TARGETS ${BIN_TARGETS} geod)
endif(BUILD_GEOD)

if(BUILD_NAD2BIN)
  include(bin_nad2bin.cmake)
  set(BIN_TARGETS ${BIN_TARGETS} nad2bin)
endif(BUILD_NAD2BIN)

if(BUILD_PROJ)
  include(bin_proj.cmake)
  set(BIN_TARGETS ${BIN_TARGETS} binproj)
endif(BUILD_PROJ)

if(BUILD_GIE)
 include(bin_gie.cmake)
 set(BIN_TARGETS ${BIN_TARGETS} gie)
endif(BUILD_GIE)

if (MSVC OR CMAKE_CONFIGURATION_TYPES)
  if(BIN_TARGETS)
    # Add _d suffix for your debug versions of the tools
    set_target_properties (${BIN_TARGETS} PROPERTIES
      DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
  endif(BIN_TARGETS)
endif ()

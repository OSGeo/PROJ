message(STATUS "Configuring proj library:")

##############################################
### SWITCH BETWEEN STATIC OR SHARED LIBRARY###
##############################################

# default config, shared on unix and static on Windows
if(UNIX)
  set(BUILD_LIBPROJ_SHARED_DEFAULT ON)
endif()
if(WIN32)
  set(BUILD_LIBPROJ_SHARED_DEFAULT OFF)
endif()
option(BUILD_LIBPROJ_SHARED
  "Build libproj library shared." ${BUILD_LIBPROJ_SHARED_DEFAULT})
if(BUILD_LIBPROJ_SHARED)
  set(PROJ_LIBRARY_TYPE SHARED)
else()
  set(PROJ_LIBRARY_TYPE STATIC)
endif()


option(USE_THREAD "Build libproj with thread/mutex support " ON)
if(NOT USE_THREAD)
  add_definitions(-DMUTEX_stub)
endif()
find_package(Threads QUIET)
if(USE_THREAD AND Threads_FOUND AND CMAKE_USE_WIN32_THREADS_INIT)
  add_definitions(-DMUTEX_win32)
elseif(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
  add_definitions(-DMUTEX_pthread)
elseif(USE_THREAD AND NOT Threads_FOUND)
  message(FATAL_ERROR
    "No thread library found and thread/mutex support is "
    "required by USE_THREAD option")
endif()

option(ENABLE_LTO
  "Build library with LTO/IPO optimization (if available)." OFF)
if(ENABLE_LTO)
  # Determine ENABLE_LTO_METHOD to either "flag" or "property"
  if(CMAKE_C_COMPILER_ID STREQUAL "Intel"
      AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(ENABLE_LTO_METHOD "property")
  elseif(CMAKE_VERSION VERSION_LESS 3.9)
    # Maual checks required
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
      include(CheckCXXSourceCompiles)
      set(CMAKE_REQUIRED_FLAGS "-Wl,-flto")
      check_cxx_source_compiles("int main(){ return 0; }"
        COMPILER_SUPPORTS_FLTO_FLAG)
    else()
      include(CheckCXXCompilerFlag)
      check_cxx_compiler_flag("-flto" COMPILER_SUPPORTS_FLTO_FLAG)
    endif()
    set(ENABLE_LTO_METHOD "flag")
    if(NOT COMPILER_SUPPORTS_FLTO_FLAG)
      set(ENABLE_LTO OFF)
    endif()
  else()  # CMake v3.9
    cmake_policy(SET CMP0069 NEW)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ENABLE_LTO)
    set(ENABLE_LTO_METHOD "property")
  endif()
endif()
print_variable(ENABLE_LTO)


##############################################
###  library source list and include_list  ###
##############################################

set(SRC_LIBPROJ_PROJECTIONS
  projections/aeqd.cpp
  projections/gnom.cpp
  projections/laea.cpp
  projections/mod_ster.cpp
  projections/nsper.cpp
  projections/nzmg.cpp
  projections/ortho.cpp
  projections/stere.cpp
  projections/sterea.cpp
  projections/aea.cpp
  projections/bipc.cpp
  projections/bonne.cpp
  projections/eqdc.cpp
  projections/isea.cpp
  projections/ccon.cpp
  projections/imw_p.cpp
  projections/krovak.cpp
  projections/lcc.cpp
  projections/poly.cpp
  projections/rpoly.cpp
  projections/sconics.cpp
  projections/rouss.cpp
  projections/cass.cpp
  projections/cc.cpp
  projections/cea.cpp
  projections/eqc.cpp
  projections/gall.cpp
  projections/labrd.cpp
  projections/lsat.cpp
  projections/misrsom.cpp
  projections/merc.cpp
  projections/mill.cpp
  projections/ocea.cpp
  projections/omerc.cpp
  projections/somerc.cpp
  projections/tcc.cpp
  projections/tcea.cpp
  projections/times.cpp
  projections/tmerc.cpp
  projections/tobmerc.cpp
  projections/airy.cpp
  projections/aitoff.cpp
  projections/august.cpp
  projections/bacon.cpp
  projections/bertin1953.cpp
  projections/chamb.cpp
  projections/hammer.cpp
  projections/lagrng.cpp
  projections/larr.cpp
  projections/lask.cpp
  projections/latlong.cpp
  projections/nicol.cpp
  projections/ob_tran.cpp
  projections/oea.cpp
  projections/tpeqd.cpp
  projections/vandg.cpp
  projections/vandg2.cpp
  projections/vandg4.cpp
  projections/wag7.cpp
  projections/lcca.cpp
  projections/geos.cpp
  projections/boggs.cpp
  projections/collg.cpp
  projections/comill.cpp
  projections/crast.cpp
  projections/denoy.cpp
  projections/eck1.cpp
  projections/eck2.cpp
  projections/eck3.cpp
  projections/eck4.cpp
  projections/eck5.cpp
  projections/fahey.cpp
  projections/fouc_s.cpp
  projections/gins8.cpp
  projections/gstmerc.cpp
  projections/gn_sinu.cpp
  projections/goode.cpp
  projections/igh.cpp
  projections/hatano.cpp
  projections/loxim.cpp
  projections/mbt_fps.cpp
  projections/mbtfpp.cpp
  projections/mbtfpq.cpp
  projections/moll.cpp
  projections/nell.cpp
  projections/nell_h.cpp
  projections/patterson.cpp
  projections/putp2.cpp
  projections/putp3.cpp
  projections/putp4p.cpp
  projections/putp5.cpp
  projections/putp6.cpp
  projections/qsc.cpp
  projections/robin.cpp
  projections/sch.cpp
  projections/sts.cpp
  projections/urm5.cpp
  projections/urmfps.cpp
  projections/wag2.cpp
  projections/wag3.cpp
  projections/wink1.cpp
  projections/wink2.cpp
  projections/healpix.cpp
  projections/natearth.cpp
  projections/natearth2.cpp
  projections/calcofi.cpp
  projections/eqearth.cpp
)

set(SRC_LIBPROJ_CONVERSIONS
  conversions/axisswap.cpp
  conversions/cart.cpp
  conversions/geoc.cpp
  conversions/geocent.cpp
  conversions/noop.cpp
  conversions/set.cpp
  conversions/unitconvert.cpp
)

set(SRC_LIBPROJ_TRANSFORMATIONS
  transformations/affine.cpp
  transformations/deformation.cpp
  transformations/helmert.cpp
  transformations/hgridshift.cpp
  transformations/horner.cpp
  transformations/molodensky.cpp
  transformations/vgridshift.cpp
  transformations/xyzgridshift.cpp
)

set(SRC_LIBPROJ_ISO19111
  iso19111/static.cpp
  iso19111/util.cpp
  iso19111/metadata.cpp
  iso19111/common.cpp
  iso19111/crs.cpp
  iso19111/datum.cpp
  iso19111/coordinatesystem.cpp
  iso19111/coordinateoperation.cpp
  iso19111/io.cpp
  iso19111/internal.cpp
  iso19111/factory.cpp
  iso19111/c_api.cpp
)

set(SRC_LIBPROJ_CORE
  4D_api.cpp
  aasincos.cpp
  adjlon.cpp
  auth.cpp
  ctx.cpp
  datum_set.cpp
  datums.cpp
  deriv.cpp
  dmstor.cpp
  ell_set.cpp
  ellps.cpp
  errno.cpp
  factors.cpp
  fileapi.cpp
  fwd.cpp
  gauss.cpp
  geocent.cpp
  geocent.h
  geodesic.c
  init.cpp
  initcache.cpp
  internal.cpp
  inv.cpp
  list.cpp
  log.cpp
  malloc.cpp
  mlfn.cpp
  msfn.cpp
  mutex.cpp
  param.cpp
  phi2.cpp
  pipeline.cpp
  pj_list.h
  pr_list.cpp
  proj_internal.h
  proj_mdist.cpp
  qsfn.cpp
  release.cpp
  rtodms.cpp
  strerrno.cpp
  strtod.cpp
  transform.cpp
  tsfn.cpp
  units.cpp
  utils.cpp
  wkt1_generated_parser.c
  wkt1_generated_parser.h
  wkt1_parser.cpp
  wkt1_parser.h
  wkt2_generated_parser.c
  wkt2_generated_parser.h
  wkt2_parser.cpp
  wkt2_parser.h
  wkt_parser.cpp
  wkt_parser.hpp
  zpoly1.cpp
  proj_json_streaming_writer.hpp
  proj_json_streaming_writer.cpp
  tracing.cpp
  grids.hpp
  grids.cpp
  filemanager.hpp
  filemanager.cpp
  networkfilemanager.cpp
  sqlite3_utils.hpp
  sqlite3_utils.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/proj_config.h
)

set(HEADERS_LIBPROJ
  proj_api.h
  proj.h
  proj_experimental.h
  proj_constants.h
  geodesic.h
)

# Group source files for IDE source explorers (e.g. Visual Studio)
source_group("Header Files"
  FILES ${HEADERS_LIBPROJ})
source_group("Source Files\\Core"
  FILES ${SRC_LIBPROJ_CORE})
source_group("Source Files\\Conversions"
  FILES ${SRC_LIBPROJ_CONVERSIONS})
source_group("Source Files\\Projections"
  FILES ${SRC_LIBPROJ_PROJECTIONS})
source_group("Source Files\\Transformations"
  FILES ${SRC_LIBPROJ_TRANSFORMATIONS})
source_group("Source Files\\ISO19111"
  FILES ${SRC_LIBPROJ_ISO19111})

include_directories(${CMAKE_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
source_group("CMake Files" FILES CMakeLists.txt)


# Embed PROJ_LIB data files location
add_definitions(-DPROJ_LIB="${CMAKE_INSTALL_PREFIX}/${DATADIR}")

#################################################
## targets: libproj and proj_config.h
#################################################
set(ALL_LIBPROJ_SOURCES
  ${SRC_LIBPROJ_CORE}
  ${SRC_LIBPROJ_CONVERSIONS}
  ${SRC_LIBPROJ_PROJECTIONS}
  ${SRC_LIBPROJ_TRANSFORMATIONS}
  ${SRC_LIBPROJ_ISO19111}
)
set(ALL_LIBPROJ_HEADERS ${HEADERS_LIBPROJ})

# Core targets configuration
string(TOLOWER "${PROJECT_NAME}" PROJECTNAMEL)
set(PROJ_CORE_TARGET ${PROJECTNAMEL})
proj_target_output_name(${PROJ_CORE_TARGET} PROJ_CORE_TARGET_OUTPUT_NAME)

add_library(
  ${PROJ_CORE_TARGET}
  ${PROJ_LIBRARY_TYPE}
  ${ALL_LIBPROJ_SOURCES}
  ${ALL_LIBPROJ_HEADERS}
  ${PROJ_RESOURCES}
)
target_compile_options(${PROJ_CORE_TARGET}
  PRIVATE $<$<COMPILE_LANGUAGE:C>:${PROJ_C_WARN_FLAGS}>
  PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${PROJ_CXX_WARN_FLAGS}>
)

if(MSVC OR MINGW)
    target_compile_definitions(${PROJ_CORE_TARGET} PRIVATE -DNOMINMAX)
endif()

# Tell Intel compiler to do arithmetic accurately.  This is needed to stop the
# compiler from ignoring parentheses in expressions like (a + b) + c and from
# simplifying 0.0 + x to x (which is wrong if x = -0.0).
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
  if(MSVC)
    set(FP_PRECISE "/fp:precise")
  else()
    set(FP_PRECISE "-fp-model precise")
  endif()
  # Apply to source files that require this option
  set_source_files_properties(
    geodesic.c
    PROPERTIES COMPILE_FLAGS ${FP_PRECISE})
endif()

if(ENABLE_LTO)
  if(ENABLE_LTO_METHOD STREQUAL "property")
    set_property(TARGET ${PROJ_CORE_TARGET}
      PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  elseif(ENABLE_LTO_METHOD STREQUAL "flag")
    # pre-CMake 3.9 workaround
    target_compile_options(${PROJ_CORE_TARGET} PRIVATE -flto)
  endif()
endif()

target_include_directories(${PROJ_CORE_TARGET} INTERFACE
  $<INSTALL_INTERFACE:${INCLUDEDIR}>)

if(WIN32)
  set_target_properties(${PROJ_CORE_TARGET}
    PROPERTIES
    VERSION "${${PROJECT_NAME}_BUILD_VERSION}"
    OUTPUT_NAME "${PROJ_CORE_TARGET_OUTPUT_NAME}"
    ARCHIVE_OUTPUT_NAME "${PROJ_CORE_TARGET}"
    CLEAN_DIRECT_OUTPUT 1)
elseif(BUILD_FRAMEWORKS_AND_BUNDLE)
  set_target_properties(${PROJ_CORE_TARGET}
    PROPERTIES
    VERSION "${${PROJECT_NAME}_BUILD_VERSION}"
    INSTALL_NAME_DIR ${PROJ_INSTALL_NAME_DIR}
    CLEAN_DIRECT_OUTPUT 1)
else()
  set_target_properties(${PROJ_CORE_TARGET}
    PROPERTIES
    VERSION "${${PROJECT_NAME}_BUILD_VERSION}"
    SOVERSION "${${PROJECT_NAME}_API_VERSION}"
    CLEAN_DIRECT_OUTPUT 1)
endif()

set_target_properties(${PROJ_CORE_TARGET}
  PROPERTIES
  LINKER_LANGUAGE CXX)

##############################################
# Link properties
##############################################
set(PROJ_LIBRARIES ${PROJ_CORE_TARGET})
# hack, required for test/unit
set(PROJ_LIBRARIES ${PROJ_LIBRARIES} PARENT_SCOPE)
if(UNIX)
  find_library(M_LIB m)
  if(M_LIB)
    target_link_libraries(${PROJ_CORE_TARGET} -lm)
  endif()
  find_library(DL_LIB dl)
  if(M_LIB)
    target_link_libraries(${PROJ_CORE_TARGET} -ldl)
  endif()
endif()
if(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
  target_link_libraries(${PROJ_CORE_TARGET} ${CMAKE_THREAD_LIBS_INIT})
endif()

target_include_directories(${PROJ_CORE_TARGET} PRIVATE ${SQLITE3_INCLUDE_DIR})
target_link_libraries(${PROJ_CORE_TARGET} ${SQLITE3_LIBRARY})

if(NOT DISABLE_TIFF)
  target_include_directories(${PROJ_CORE_TARGET} PRIVATE ${TIFF_INCLUDE_DIR})
  target_link_libraries(${PROJ_CORE_TARGET} ${TIFF_LIBRARY})
endif()

if(CURL_FOUND)
  target_include_directories(${PROJ_CORE_TARGET} PRIVATE ${CURL_INCLUDE_DIR})
  target_link_libraries(${PROJ_CORE_TARGET} ${CURL_LIBRARY})
endif()

if(MSVC AND BUILD_LIBPROJ_SHARED)
  target_compile_definitions(${PROJ_CORE_TARGET}
    PRIVATE PROJ_MSVC_DLL_EXPORT=1)
endif()

##############################################
# install
##############################################
install(TARGETS ${PROJ_CORE_TARGET}
  EXPORT targets
  RUNTIME DESTINATION ${BINDIR}
  LIBRARY DESTINATION ${LIBDIR}
  ARCHIVE DESTINATION ${LIBDIR}
  FRAMEWORK DESTINATION ${FRAMEWORKDIR})

if(NOT BUILD_FRAMEWORKS_AND_BUNDLE)
  install(FILES ${ALL_LIBPROJ_HEADERS}
    DESTINATION ${INCLUDEDIR})
endif()

##############################################
# Core configuration summary
##############################################
print_variable(PROJ_CORE_TARGET)
print_variable(PROJ_CORE_TARGET_OUTPUT_NAME)
print_variable(PROJ_LIBRARY_TYPE)
print_variable(PROJ_LIBRARIES)

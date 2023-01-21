message(STATUS "Configuring proj library:")

##############################################
### SWITCH BETWEEN STATIC OR SHARED LIBRARY###
##############################################

# default config is shared
option(BUILD_SHARED_LIBS
  "Build PROJ library shared." ON)

find_package(Threads QUIET)
if(Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
  add_definitions(-DPROJ_HAS_PTHREADS)
endif()

option(ENABLE_IPO
  "Build library with interprocedural optimization (if available)." OFF)
if(ENABLE_IPO)
  cmake_policy(SET CMP0069 NEW)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ENABLE_IPO)
endif()
print_variable(ENABLE_IPO)


##############################################
###  library source list and include_list  ###
##############################################

set(SRC_LIBPROJ_PROJECTIONS
  projections/aeqd.cpp
  projections/adams.cpp
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
  projections/som.cpp
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
  projections/igh_o.cpp
  projections/imoll.cpp
  projections/imoll_o.cpp
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
  projections/s2.cpp
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
  projections/col_urban.cpp
)

set(SRC_LIBPROJ_CONVERSIONS
  conversions/axisswap.cpp
  conversions/cart.cpp
  conversions/geoc.cpp
  conversions/geocent.cpp
  conversions/noop.cpp
  conversions/topocentric.cpp
  conversions/set.cpp
  conversions/unitconvert.cpp
)

set(SRC_LIBPROJ_TRANSFORMATIONS
  transformations/affine.cpp
  transformations/deformation.cpp
  transformations/gridshift.cpp
  transformations/helmert.cpp
  transformations/hgridshift.cpp
  transformations/horner.cpp
  transformations/molodensky.cpp
  transformations/vgridshift.cpp
  transformations/xyzgridshift.cpp
  transformations/defmodel.cpp
  transformations/tinshift.cpp
  transformations/vertoffset.cpp
)

set(SRC_LIBPROJ_ISO19111
  iso19111/static.cpp
  iso19111/util.cpp
  iso19111/metadata.cpp
  iso19111/common.cpp
  iso19111/coordinates.cpp
  iso19111/crs.cpp
  iso19111/datum.cpp
  iso19111/coordinatesystem.cpp
  iso19111/io.cpp
  iso19111/internal.cpp
  iso19111/factory.cpp
  iso19111/c_api.cpp
  iso19111/operation/concatenatedoperation.cpp
  iso19111/operation/coordinateoperationfactory.cpp
  iso19111/operation/conversion.cpp
  iso19111/operation/esriparammappings.cpp
  iso19111/operation/oputils.cpp
  iso19111/operation/parammappings.cpp
  iso19111/operation/projbasedoperation.cpp
  iso19111/operation/singleoperation.cpp
  iso19111/operation/transformation.cpp
  iso19111/operation/vectorofvaluesparams.cpp
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
  factors.cpp
  fwd.cpp
  gauss.cpp
  generic_inverse.cpp
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
  tsfn.cpp
  units.cpp
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
  proj.h
  proj_experimental.h
  proj_constants.h
  proj_symbol_rename.h
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

include_directories(${PROJ_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
source_group("CMake Files" FILES CMakeLists.txt)

# Embed PROJ_DATA data files location
add_definitions(-DPROJ_DATA="${PROJ_DATA_PATH}")


###########################################################
# targets to refresh wkt1_parser.cpp and wkt2_parser.cpp
###########################################################

# Those targets need to be run manually each time wkt1_grammar.y / wkt2_grammar.y
# is modified.
# We could of course run them automatically, but that would make building
# PROJ harder.

# This target checks that wkt1_grammar.y md5sum has not changed
# If it has, then it should be updated and the generate_wkt1_parser target
# should be manually run
add_custom_target(check_wkt1_grammar_md5 ALL
                  COMMAND ${CMAKE_COMMAND}
                      "-DIN_FILE=wkt1_grammar.y"
                      "-DTARGET=generate_wkt1_parser"
                      "-DEXPECTED_MD5SUM=04ba1095339aeac27105f5782d6297de"
                      -P "${CMAKE_CURRENT_SOURCE_DIR}/check_md5sum.cmake"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/wkt1_grammar.y"
                  VERBATIM)

add_custom_target(generate_wkt1_parser
                  COMMAND ${CMAKE_COMMAND}
                      "-DPREFIX=pj_wkt1_"
                      "-DIN_FILE=wkt1_grammar.y"
                      "-DOUT_FILE=wkt1_generated_parser.c"
                      -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_wkt_parser.cmake"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  VERBATIM)

# This target checks that wkt2_grammar.y md5sum has not changed
# If it has, then it should be updated and the generate_wkt2_parser target
# should be manually run
add_custom_target(check_wkt2_grammar_md5 ALL
                  COMMAND ${CMAKE_COMMAND}
                      "-DIN_FILE=wkt2_grammar.y"
                      "-DTARGET=generate_wkt2_parser"
                      "-DEXPECTED_MD5SUM=289572eebe9dab3c7225bd48c445c287"
                      -P "${CMAKE_CURRENT_SOURCE_DIR}/check_md5sum.cmake"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/wkt2_grammar.y"
                  VERBATIM)

add_custom_target(generate_wkt2_parser
                  COMMAND ${CMAKE_COMMAND}
                      "-DPREFIX=pj_wkt2_"
                      "-DIN_FILE=wkt2_grammar.y"
                      "-DOUT_FILE=wkt2_generated_parser.c"
                      -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_wkt_parser.cmake"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  VERBATIM)

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

# Configuration for the core target "proj"
proj_target_output_name(proj PROJ_CORE_TARGET_OUTPUT_NAME)

add_library(proj
  ${ALL_LIBPROJ_SOURCES}
  ${ALL_LIBPROJ_HEADERS}
  ${PROJ_RESOURCES}
)
add_library(PROJ::proj ALIAS proj)

if(MSVC OR MINGW)
    target_compile_definitions(proj PRIVATE -DNOMINMAX)
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

if(ENABLE_IPO)
  set_property(TARGET proj
    PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_include_directories(proj INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${PROJ_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(WIN32)
  set_target_properties(proj
    PROPERTIES
    VERSION "${PROJ_VERSION}"
    OUTPUT_NAME "${PROJ_CORE_TARGET_OUTPUT_NAME}"
    ARCHIVE_OUTPUT_NAME proj
    CLEAN_DIRECT_OUTPUT 1)
elseif(BUILD_FRAMEWORKS_AND_BUNDLE)
  set_target_properties(proj
    PROPERTIES
    VERSION "${PROJ_VERSION}"
    INSTALL_NAME_DIR ${PROJ_INSTALL_NAME_DIR}
    CLEAN_DIRECT_OUTPUT 1)
else()
  set_target_properties(proj
    PROPERTIES
    VERSION "${PROJ_BUILD_VERSION}"
    SOVERSION "${PROJ_SOVERSION}"
    CLEAN_DIRECT_OUTPUT 1)
endif()

set_target_properties(proj
  PROPERTIES
  LINKER_LANGUAGE CXX)

##############################################
# Link properties
##############################################
set(PROJ_LIBRARIES proj)
# hack, required for test/unit
set(PROJ_LIBRARIES ${PROJ_LIBRARIES} PARENT_SCOPE)
if(UNIX)
  find_library(M_LIB m)
  if(M_LIB)
    target_link_libraries(proj PRIVATE -lm)
  endif()
  find_library(DL_LIB dl)
  if(DL_LIB)
    target_link_libraries(proj PRIVATE -ldl)
  endif()
endif()
if(Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
  target_link_libraries(proj PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

target_include_directories(proj PRIVATE ${SQLITE3_INCLUDE_DIR})
target_link_libraries(proj PRIVATE ${SQLITE3_LIBRARY})

if(NLOHMANN_JSON STREQUAL "external")
  target_compile_definitions(proj PRIVATE EXTERNAL_NLOHMANN_JSON)
  target_link_libraries(proj
    PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>)
endif()

if(TIFF_ENABLED)
  target_compile_definitions(proj PRIVATE -DTIFF_ENABLED)
  if( CMAKE_VERSION VERSION_LESS 3.11 AND CMAKE_CROSSCOMPILING )
      # Hack needed for ubuntu:18.04 mingw64 cross compiling to avoid
      # -isystem to be emitted (similar to https://discourse.cmake.org/t/use-of-isystem/1574)
      target_include_directories(proj PRIVATE ${TIFF_INCLUDE_DIRS})
      target_link_libraries(proj PRIVATE ${TIFF_LIBRARIES})
  else()
      target_link_libraries(proj PRIVATE TIFF::TIFF)
  endif()
endif()

if(CURL_ENABLED)
  target_compile_definitions(proj PRIVATE -DCURL_ENABLED)
  if( CMAKE_VERSION VERSION_LESS 3.11 AND CMAKE_CROSSCOMPILING )
      # Hack needed for ubuntu:18.04 mingw64 cross compiling to avoid
      # -isystem to be emitted (similar to https://discourse.cmake.org/t/use-of-isystem/1574)
      target_include_directories(proj PRIVATE ${CURL_INCLUDE_DIRS})
      target_link_libraries(proj PRIVATE ${CURL_LIBRARIES})
  else()
      target_link_libraries(proj PRIVATE CURL::libcurl)
  endif()
  target_link_libraries(proj
    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:ws2_32>
      $<$<CXX_COMPILER_ID:MSVC>:wldap32>
      $<$<CXX_COMPILER_ID:MSVC>:advapi32>
      $<$<CXX_COMPILER_ID:MSVC>:crypt32>
      $<$<CXX_COMPILER_ID:MSVC>:normaliz>)
endif()

if(BUILD_SHARED_LIBS)
  if(MSVC)
    target_compile_definitions(proj PRIVATE PROJ_MSVC_DLL_EXPORT=1)
  endif()
else()
  target_compile_definitions(proj PUBLIC PROJ_DLL=)
endif()

##############################################
# install
##############################################
install(TARGETS proj
  EXPORT targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FRAMEWORK DESTINATION ${FRAMEWORKDIR})

if(NOT BUILD_FRAMEWORKS_AND_BUNDLE)
  install(FILES ${ALL_LIBPROJ_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

##############################################
# Core configuration summary
##############################################
print_variable(PROJ_CORE_TARGET_OUTPUT_NAME)
print_variable(BUILD_SHARED_LIBS)
print_variable(PROJ_LIBRARIES)

##############################################
### SWITCH BETWEEN STATIC OR SHARED LIBRARY###
##############################################
colormsg(_HIBLUE_ "Configuring proj library:")
message(STATUS "")

# default config, shared on unix and static on Windows
if(UNIX)
    set(BUILD_LIBPROJ_SHARED_DEFAULT ON )
endif(UNIX)
if( WIN32)
    set(BUILD_LIBPROJ_SHARED_DEFAULT OFF)
endif(WIN32)
option(BUILD_LIBPROJ_SHARED "Build libproj library shared." ${BUILD_LIBPROJ_SHARED_DEFAULT})
if(BUILD_LIBPROJ_SHARED)
  set(PROJ_LIBRARY_TYPE SHARED)
else(BUILD_LIBPROJ_SHARED)
  set(PROJ_LIBRARY_TYPE STATIC)
endif(BUILD_LIBPROJ_SHARED)


option(USE_THREAD "Build libproj with thread/mutex support " ON)
if(NOT USE_THREAD)
   add_definitions( -DMUTEX_stub)
endif(NOT USE_THREAD)
find_package(Threads QUIET)
if(USE_THREAD AND Threads_FOUND AND CMAKE_USE_WIN32_THREADS_INIT )
   add_definitions( -DMUTEX_win32)
elseif(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT )
   add_definitions( -DMUTEX_pthread)
elseif(USE_THREAD AND NOT Threads_FOUND)
  message(FATAL_ERROR "No thread library found and thread/mutex support is required by USE_THREAD option")
endif()

option(ENABLE_LTO "Build library with LTO optimization (if available)." OFF)
if(ENABLE_LTO)
    if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
        include (CheckCXXSourceCompiles)
        SET(CMAKE_REQUIRED_FLAGS "-Wl,-flto")
        check_cxx_source_compiles("int main(){ return 0; }" COMPILER_SUPPORTS_FLTO_FLAG)
        IF(COMPILER_SUPPORTS_FLTO_FLAG)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
        ENDIF()
    else()
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("-flto" COMPILER_SUPPORTS_FLTO_FLAG)
        if(COMPILER_SUPPORTS_FLTO_FLAG)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
        endif()
    endif()
endif()


##############################################
###  library source list and include_list  ###
##############################################

SET(SRC_LIBPROJ_PROJECTIONS
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

SET(SRC_LIBPROJ_CONVERSIONS
        conversions/axisswap.cpp
        conversions/cart.cpp
        conversions/geoc.cpp
        conversions/geocent.cpp
        conversions/unitconvert.cpp
)

SET(SRC_LIBPROJ_TRANSFORMATIONS
        transformations/affine.cpp
        transformations/deformation.cpp
        transformations/helmert.cpp
        transformations/hgridshift.cpp
        transformations/horner.cpp
        transformations/molodensky.cpp
        transformations/vgridshift.cpp
)

SET(SRC_LIBPROJ_ISO19111
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

SET(SRC_LIBPROJ_CORE
        pj_list.h proj_internal.h proj_math.h
        aasincos.cpp adjlon.cpp
        dmstor.cpp auth.cpp
        deriv.cpp ell_set.cpp ellps.cpp errno.cpp
        factors.cpp fwd.cpp init.cpp inv.cpp
        list.cpp malloc.cpp mlfn.cpp msfn.cpp proj_mdist.cpp
        open_lib.cpp param.cpp phi2.cpp pr_list.cpp
        qsfn.cpp strerrno.cpp
        tsfn.cpp units.cpp ctx.cpp log.cpp zpoly1.cpp rtodms.cpp
        release.cpp gauss.cpp
        fileapi.cpp
        gc_reader.cpp gridcatalog.cpp
        nad_cvt.cpp nad_init.cpp nad_intr.cpp
        apply_gridshift.cpp datums.cpp datum_set.cpp transform.cpp
        geocent.cpp geocent.h utils.cpp gridinfo.cpp gridlist.cpp
        jniproj.cpp mutex.cpp initcache.cpp apply_vgridshift.cpp geodesic.c
        strtod.cpp math.cpp
        4D_api.cpp pipeline.cpp
        internal.cpp
        wkt_parser.hpp wkt_parser.cpp
        wkt1_parser.h wkt1_parser.cpp
        wkt1_generated_parser.h wkt1_generated_parser.c
        wkt2_parser.h wkt2_parser.cpp
        wkt2_generated_parser.h wkt2_generated_parser.c
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
source_group("Header Files" FILES ${HEADERS_LIBPROJ})
source_group("Source Files\\Core" FILES ${SRC_LIBPROJ_CORE})
source_group("Source Files\\Conversions" FILES ${SRC_LIBPROJ_CONVERSIONS})
source_group("Source Files\\Projections" FILES ${SRC_LIBPROJ_PROJECTIONS})
source_group("Source Files\\Transformations" FILES ${SRC_LIBPROJ_TRANSFORMATIONS})
source_group("Source Files\\ISO19111" FILES ${SRC_LIBPROJ_ISO19111})

include_directories(${CMAKE_SOURCE_DIR}/include)

include_directories( ${CMAKE_CURRENT_BINARY_DIR})
source_group("CMake Files" FILES CMakeLists.txt)


# Embed PROJ_LIB data files location
add_definitions(-DPROJ_LIB="${CMAKE_INSTALL_PREFIX}/${DATADIR}")

#################################################
## java wrapping with jni
#################################################
option(JNI_SUPPORT "Build support of java/jni wrapping for proj library" OFF)
find_package(JNI QUIET)
if(JNI_SUPPORT AND NOT JNI_FOUND)
  message(FATAL_ERROR "jni support is required but jni is not found")
endif(JNI_SUPPORT AND NOT JNI_FOUND)
boost_report_value(JNI_SUPPORT)
if(JNI_SUPPORT)
  set(SRC_LIBPROJ_CORE ${SRC_LIBPROJ_CORE}
                       jniproj.cpp )
  set(HEADERS_LIBPROJ ${HEADERS_LIBPROJ}
                        org_proj4_PJ.h)
  source_group("Source Files\\JNI" FILES ${SRC_LIBPROJ_JNI})
  add_definitions(-DJNI_ENABLED)
  include_directories( ${JNI_INCLUDE_DIRS})
  boost_report_value(JNI_INCLUDE_DIRS)
endif(JNI_SUPPORT)

#################################################
## targets: libproj and proj_config.h
#################################################
set(ALL_LIBPROJ_SOURCES ${SRC_LIBPROJ_CORE}
                        ${SRC_LIBPROJ_CONVERSIONS}
                        ${SRC_LIBPROJ_PROJECTIONS}
                        ${SRC_LIBPROJ_TRANSFORMATIONS}
                        ${SRC_LIBPROJ_ISO19111}
)
set(ALL_LIBPROJ_HEADERS ${HEADERS_LIBPROJ} )

# Core targets configuration
string(TOLOWER "${PROJECT_INTERN_NAME}" PROJECTNAMEL)
set(PROJ_CORE_TARGET ${PROJECTNAMEL})
proj_target_output_name(${PROJ_CORE_TARGET} PROJ_CORE_TARGET_OUTPUT_NAME)

add_library( ${PROJ_CORE_TARGET}
                    ${PROJ_LIBRARY_TYPE}
                    ${ALL_LIBPROJ_SOURCES}
                    ${ALL_LIBPROJ_HEADERS}
                    ${PROJ_RESOURCES}  )

if (NOT CMAKE_VERSION VERSION_LESS 2.8.11)
  target_include_directories (${PROJ_CORE_TARGET} INTERFACE
    $<INSTALL_INTERFACE:${INCLUDEDIR}>)
endif ()

if(WIN32)
  set_target_properties(${PROJ_CORE_TARGET}
    PROPERTIES
    VERSION "${${PROJECT_INTERN_NAME}_BUILD_VERSION}"
    OUTPUT_NAME "${PROJ_CORE_TARGET_OUTPUT_NAME}"
    CLEAN_DIRECT_OUTPUT 1)
elseif(BUILD_FRAMEWORKS_AND_BUNDLE)
  set_target_properties(${PROJ_CORE_TARGET}
    PROPERTIES
    VERSION "${${PROJECT_INTERN_NAME}_BUILD_VERSION}"
    INSTALL_NAME_DIR ${PROJ_INSTALL_NAME_DIR}
    CLEAN_DIRECT_OUTPUT 1)
else()
  set_target_properties(${PROJ_CORE_TARGET}
    PROPERTIES
    VERSION "${${PROJECT_INTERN_NAME}_BUILD_VERSION}"
    SOVERSION "${${PROJECT_INTERN_NAME}_API_VERSION}"
    CLEAN_DIRECT_OUTPUT 1)
endif()

set_target_properties(${PROJ_CORE_TARGET}
    PROPERTIES
    LINKER_LANGUAGE CXX)

##############################################
# Link properties
##############################################
set(PROJ_LIBRARIES ${PROJ_CORE_TARGET})
set(PROJ_LIBRARIES ${PROJ_LIBRARIES} PARENT_SCOPE) # hack, required for test/unit
if(UNIX)
    find_library(M_LIB m)
    if(M_LIB)
      TARGET_LINK_LIBRARIES(${PROJ_CORE_TARGET} -lm)
    endif()
endif(UNIX)
if(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
   TARGET_LINK_LIBRARIES(${PROJ_CORE_TARGET} ${CMAKE_THREAD_LIBS_INIT})
endif(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)

include_directories(${SQLITE3_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(${PROJ_CORE_TARGET} ${SQLITE3_LIBRARY})

if(MSVC)
    target_compile_definitions(${PROJ_CORE_TARGET} PRIVATE PROJ_MSVC_DLL_EXPORT=1)
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
endif(NOT BUILD_FRAMEWORKS_AND_BUNDLE)

##############################################
# Core configuration summary
##############################################
boost_report_value(PROJ_CORE_TARGET)
boost_report_value(PROJ_CORE_TARGET_OUTPUT_NAME)
boost_report_value(PROJ_LIBRARY_TYPE)
boost_report_value(PROJ_LIBRARIES)

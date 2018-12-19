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
        projections/PJ_aeqd.cpp
        projections/PJ_gnom.cpp
        projections/PJ_laea.cpp
        projections/PJ_mod_ster.cpp
        projections/PJ_nsper.cpp
        projections/PJ_nzmg.cpp
        projections/PJ_ortho.cpp
        projections/PJ_stere.cpp
        projections/PJ_sterea.cpp
        projections/PJ_aea.cpp
        projections/PJ_bipc.cpp
        projections/PJ_bonne.cpp
        projections/PJ_eqdc.cpp
        projections/PJ_isea.cpp
        projections/PJ_ccon.cpp
        projections/PJ_imw_p.cpp
        projections/PJ_krovak.cpp
        projections/PJ_lcc.cpp
        projections/PJ_poly.cpp
        projections/PJ_rpoly.cpp
        projections/PJ_sconics.cpp
        projections/proj_rouss.cpp
        projections/PJ_cass.cpp
        projections/PJ_cc.cpp
        projections/PJ_cea.cpp
        projections/PJ_eqc.cpp
        projections/PJ_gall.cpp
        projections/PJ_labrd.cpp
        projections/PJ_lsat.cpp
        projections/PJ_misrsom.cpp
        projections/PJ_merc.cpp
        projections/PJ_mill.cpp
        projections/PJ_ocea.cpp
        projections/PJ_omerc.cpp
        projections/PJ_somerc.cpp
        projections/PJ_tcc.cpp
        projections/PJ_tcea.cpp
        projections/PJ_times.cpp
        projections/PJ_tmerc.cpp
        projections/PJ_tobmerc.cpp
        projections/PJ_airy.cpp
        projections/PJ_aitoff.cpp
        projections/PJ_august.cpp
        projections/PJ_bacon.cpp
        projections/PJ_bertin1953.cpp
        projections/PJ_chamb.cpp
        projections/PJ_hammer.cpp
        projections/PJ_lagrng.cpp
        projections/PJ_larr.cpp
        projections/PJ_lask.cpp
        projections/PJ_latlong.cpp
        projections/PJ_nocol.cpp
        projections/PJ_ob_tran.cpp
        projections/PJ_oea.cpp
        projections/PJ_tpeqd.cpp
        projections/PJ_vandg.cpp
        projections/PJ_vandg2.cpp
        projections/PJ_vandg4.cpp
        projections/PJ_wag7.cpp
        projections/PJ_lcca.cpp
        projections/PJ_geos.cpp
        projections/proj_etmerc.cpp
        projections/PJ_boggs.cpp
        projections/PJ_collg.cpp
        projections/PJ_comill.cpp
        projections/PJ_crast.cpp
        projections/PJ_denoy.cpp
        projections/PJ_eck1.cpp
        projections/PJ_eck2.cpp
        projections/PJ_eck3.cpp
        projections/PJ_eck4.cpp
        projections/PJ_eck5.cpp
        projections/PJ_fahey.cpp
        projections/PJ_fouc_s.cpp
        projections/PJ_gins8.cpp
        projections/PJ_gstmerc.cpp
        projections/PJ_gn_sinu.cpp
        projections/PJ_goode.cpp
        projections/PJ_igh.cpp
        projections/PJ_hatano.cpp
        projections/PJ_loxim.cpp
        projections/PJ_mbt_fps.cpp
        projections/PJ_mbtfpp.cpp
        projections/PJ_mbtfpq.cpp
        projections/PJ_moll.cpp
        projections/PJ_nell.cpp
        projections/PJ_nell_h.cpp
        projections/PJ_patterson.cpp
        projections/PJ_putp2.cpp
        projections/PJ_putp3.cpp
        projections/PJ_putp4p.cpp
        projections/PJ_putp5.cpp
        projections/PJ_putp6.cpp
        projections/PJ_qsc.cpp
        projections/PJ_robin.cpp
        projections/PJ_sch.cpp
        projections/PJ_sts.cpp
        projections/PJ_urm5.cpp
        projections/PJ_urmfps.cpp
        projections/PJ_wag2.cpp
        projections/PJ_wag3.cpp
        projections/PJ_wink1.cpp
        projections/PJ_wink2.cpp
        projections/PJ_healpix.cpp
        projections/PJ_natearth.cpp
        projections/PJ_natearth2.cpp
        projections/PJ_calcofi.cpp
        projections/PJ_eqearth.cpp
)

SET(SRC_LIBPROJ_CONVERSIONS
        conversions/PJ_axisswap.cpp
        conversions/PJ_cart.cpp
        conversions/PJ_geoc.cpp
        conversions/pj_geocent.cpp
        conversions/PJ_unitconvert.cpp
)

SET(SRC_LIBPROJ_TRANSFORMATIONS
        transformations/PJ_affine.cpp
        transformations/PJ_deformation.cpp
        transformations/PJ_helmert.cpp
        transformations/PJ_hgridshift.cpp
        transformations/PJ_horner.cpp
        transformations/PJ_molodensky.cpp
        transformations/PJ_vgridshift.cpp
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
        pj_list.h proj_internal.h proj_math.h projects.h
        aasincos.cpp adjlon.cpp bch2bps.cpp bchgen.cpp
        biveval.cpp dmstor.cpp mk_cheby.cpp pj_auth.cpp
        pj_deriv.cpp pj_ell_set.cpp pj_ellps.cpp pj_errno.cpp
        pj_factors.cpp pj_fwd.cpp pj_init.cpp pj_inv.cpp
        pj_list.cpp pj_malloc.cpp pj_mlfn.cpp pj_msfn.cpp proj_mdist.cpp
        pj_open_lib.cpp pj_param.cpp pj_phi2.cpp pj_pr_list.cpp
        pj_qsfn.cpp pj_strerrno.cpp
        pj_tsfn.cpp pj_units.cpp pj_ctx.cpp pj_log.cpp pj_zpoly1.cpp rtodms.cpp
        vector1.cpp pj_release.cpp pj_gauss.cpp
        pj_fileapi.cpp
        pj_gc_reader.cpp pj_gridcatalog.cpp
        nad_cvt.cpp nad_init.cpp nad_intr.cpp
        pj_apply_gridshift.cpp pj_datums.cpp pj_datum_set.cpp pj_transform.cpp
        geocent.cpp geocent.h pj_utils.cpp pj_gridinfo.cpp pj_gridlist.cpp
        jniproj.cpp pj_mutex.cpp pj_initcache.cpp pj_apply_vgridshift.cpp geodesic.cpp
        pj_strtod.cpp pj_math.cpp
        proj_4D_api.cpp PJ_pipeline.cpp
        pj_internal.cpp
        pj_wkt_parser.hpp pj_wkt_parser.cpp
        pj_wkt1_parser.h pj_wkt1_parser.cpp
        pj_wkt1_generated_parser.h pj_wkt1_generated_parser.c
        pj_wkt2_parser.h pj_wkt2_parser.cpp
        pj_wkt2_generated_parser.h pj_wkt2_generated_parser.c
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

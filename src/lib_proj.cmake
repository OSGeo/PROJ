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
SET(SRC_LIBPROJ_PJ
        nad_init.cpp
        PJ_aea.cpp
        PJ_aeqd.cpp
        PJ_affine.cpp
        PJ_airy.cpp
        PJ_aitoff.cpp
        PJ_august.cpp
        PJ_axisswap.cpp
        PJ_bacon.cpp
        PJ_bertin1953.cpp
        PJ_bipc.cpp
        PJ_boggs.cpp
        PJ_bonne.cpp
        PJ_calcofi.cpp
        PJ_cart.cpp
        PJ_cass.cpp
        PJ_cc.cpp
        PJ_ccon.cpp
        PJ_cea.cpp
        PJ_chamb.cpp
        PJ_collg.cpp
        PJ_comill.cpp
        PJ_crast.cpp
        PJ_deformation.cpp
        PJ_denoy.cpp
        PJ_eck1.cpp
        PJ_eck2.cpp
        PJ_eck3.cpp
        PJ_eck4.cpp
        PJ_eck5.cpp
        PJ_eqc.cpp
        PJ_eqdc.cpp
        PJ_eqearth.cpp
        PJ_fahey.cpp
        PJ_fouc_s.cpp
        PJ_gall.cpp
        PJ_geoc.cpp
        PJ_geos.cpp
        PJ_gins8.cpp
        PJ_gnom.cpp
        PJ_gn_sinu.cpp
        PJ_goode.cpp
        PJ_gstmerc.cpp
        PJ_hammer.cpp
        PJ_hatano.cpp
        PJ_helmert.cpp
        PJ_hgridshift.cpp
        PJ_horner.cpp
        PJ_igh.cpp
        PJ_isea.cpp
        PJ_imw_p.cpp
        PJ_krovak.cpp
        PJ_labrd.cpp
        PJ_laea.cpp
        PJ_lagrng.cpp
        PJ_larr.cpp
        PJ_lask.cpp
        PJ_latlong.cpp
        PJ_lcca.cpp
        PJ_lcc.cpp
        PJ_loxim.cpp
        PJ_lsat.cpp
        PJ_misrsom.cpp
        PJ_mbt_fps.cpp
        PJ_mbtfpp.cpp
        PJ_mbtfpq.cpp
        PJ_merc.cpp
        PJ_mill.cpp
        PJ_mod_ster.cpp
        PJ_moll.cpp
        PJ_molodensky.cpp
        PJ_natearth.cpp
        PJ_natearth2.cpp
        PJ_nell.cpp
        PJ_nell_h.cpp
        PJ_nocol.cpp
        PJ_nsper.cpp
        PJ_nzmg.cpp
        PJ_ob_tran.cpp
        PJ_ocea.cpp
        PJ_oea.cpp
        PJ_omerc.cpp
        PJ_ortho.cpp
        PJ_patterson.cpp
        PJ_pipeline.cpp
        PJ_poly.cpp
        PJ_putp2.cpp
        PJ_putp3.cpp
        PJ_putp4p.cpp
        PJ_putp5.cpp
        PJ_putp6.cpp
        PJ_qsc.cpp
        PJ_robin.cpp
        PJ_rpoly.cpp
        PJ_sch.cpp
        PJ_sconics.cpp
        PJ_somerc.cpp
        PJ_sterea.cpp
        PJ_stere.cpp
        PJ_sts.cpp
        PJ_tcc.cpp
        PJ_tcea.cpp
        PJ_times.cpp
        PJ_tmerc.cpp
        PJ_tobmerc.cpp
        PJ_tpeqd.cpp
        PJ_unitconvert.cpp
        PJ_urm5.cpp
        PJ_urmfps.cpp
        PJ_vandg.cpp
        PJ_vandg2.cpp
        PJ_vandg4.cpp
        PJ_vgridshift.cpp
        PJ_wag2.cpp
        PJ_wag3.cpp
        PJ_wag7.cpp
        PJ_wink1.cpp
        PJ_wink2.cpp
        proj_etmerc.cpp
)

SET(SRC_LIBPROJ_CORE
        aasincos.cpp
        adjlon.cpp
        bch2bps.cpp
        bchgen.cpp
        biveval.cpp
        dmstor.cpp
        emess.cpp
        emess.h
        geocent.cpp
        geocent.h
        geodesic.cpp
        mk_cheby.cpp
        nad_cvt.cpp
        nad_init.cpp
        nad_intr.cpp
        pj_apply_gridshift.cpp
        pj_apply_vgridshift.cpp
        pj_auth.cpp
        pj_ctx.cpp
        pj_fileapi.cpp
        pj_datum_set.cpp
        pj_datums.cpp
        pj_deriv.cpp
        pj_ell_set.cpp
        pj_ellps.cpp
        pj_errno.cpp
        pj_factors.cpp
        pj_fwd.cpp
        pj_gauss.cpp
        pj_gc_reader.cpp
        pj_geocent.cpp
        pj_gridcatalog.cpp
        pj_gridinfo.cpp
        pj_gridlist.cpp
        PJ_healpix.cpp
        pj_init.cpp
        pj_initcache.cpp
        pj_inv.cpp
        pj_list.cpp
        pj_list.h
        pj_log.cpp
        pj_malloc.cpp
        pj_math.cpp
        pj_mlfn.cpp
        pj_msfn.cpp
        pj_mutex.cpp
        proj_4D_api.cpp
        pj_internal.cpp
        proj_internal.h
        pj_open_lib.cpp
        pj_param.cpp
        pj_phi2.cpp
        pj_pr_list.cpp
        pj_qsfn.cpp
        pj_release.cpp
        pj_strerrno.cpp
        pj_transform.cpp
        pj_tsfn.cpp
        pj_units.cpp
        pj_utils.cpp
        pj_zpoly1.cpp
        proj_mdist.cpp
        proj_math.h
        proj_rouss.cpp
        rtodms.cpp
        vector1.cpp
        pj_strtod.cpp
        pj_wkt1_generated_parser.c
        pj_wkt2_generated_parser.c
        ${CMAKE_CURRENT_BINARY_DIR}/proj_config.h
)

set(SRC_LIBPROJ_CPP
        static.cpp
        common.cpp
        coordinateoperation.cpp
        coordinatesystem.cpp
        crs.cpp
        datum.cpp
        io.cpp
        metadata.cpp
        util.cpp
        internal.cpp
        factory.cpp
        c_api.cpp
        pj_wkt_parser.cpp
        pj_wkt1_parser.cpp
        pj_wkt2_parser.cpp
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
source_group("Source Files\\PJ" FILES ${SRC_LIBPROJ_PJ})
source_group("Source Files\\C++" FILES ${SRC_LIBPROJ_CPP})

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
set(ALL_LIBPROJ_SOURCES ${SRC_LIBPROJ_PJ} ${SRC_LIBPROJ_CORE} ${SRC_LIBPROJ_CPP})
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

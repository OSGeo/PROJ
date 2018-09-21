#
# files containing dictionary of useful projection
#

set(PROJ_DICTIONARY  epsg
                     esri
                     world
                     esri.extra
                     other.extra
                     IGNF
                     nad27
                     GL27
                     nad83
                     nad.lst
                     proj_def.dat
                     CH
                     ITRF2000
                     ITRF2008
                     ITRF2014 )

#
# gridshift file
#

file(GLOB GSB_FILES  *.gsb)
file(GLOB GTX_FILES  *.gtx)
set(GRIDSHIFT_FILES ${GSB_FILES} ${GTX_FILES})
set(GRIDSHIFT_FILES ${GRIDSHIFT_FILES}
                     )
option(CONVERT_DATA "convert some ascii file to binary file for use in proj4" OFF)
if(CONVERT_DATA)
    if (BUILD_NAD2BIN)
       set(LLA_GRID_DIR ${CMAKE_CURRENT_SOURCE_DIR})
       proj_convert_grid_lla2gsb(LLA_GRID_DIR)
       file(GLOB LLA_FILES  *.lla)
       proj_append_lla_output_file(LLA_FILES GRIDSHIFT_FILES)
    else()
       message(ERROR " you need to compile nad2bin exe in order to convert data file" )
    endif(BUILD_NAD2BIN)
endif(CONVERT_DATA)

set(ALL_SQL_IN "${CMAKE_CURRENT_SOURCE_DIR}/all.sql.in")
set(PROJ_DB "${CMAKE_CURRENT_SOURCE_DIR}/proj.db")
include(sql_filelist.cmake)

add_custom_command(OUTPUT ${ALL_SQL_IN}
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_all_sql_in.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS ${SQL_FILES}
    COMMENT "Generating all.sql.in"
    VERBATIM
)

add_custom_target(generate_all_sql_in ALL DEPENDS ${ALL_SQL_IN})

add_custom_command(OUTPUT ${PROJ_DB}
    COMMAND ${CMAKE_COMMAND} -E remove -f ${PROJ_DB}
    COMMAND ${EXE_SQLITE3} -init ${ALL_SQL_IN} ${PROJ_DB} .quit
    DEPENDS generate_all_sql_in ${ALL_SQL_IN}
    COMMENT "Generating proj.db"
    VERBATIM
)

add_custom_target(generate_proj_db ALL DEPENDS ${PROJ_DB})

#
#install
#
set(ALL_DATA_FILE ${PROJ_DICTIONARY}
                  ${GRIDSHIFT_FILES}
                  ${GEOID_FILES} "proj.db")
install(FILES ${ALL_DATA_FILE}
        DESTINATION ${DATADIR})

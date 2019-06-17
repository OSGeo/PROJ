#
# files containing dictionary of useful projection
#

set(PROJ_DICTIONARY
  null
  world
  other.extra
  nad27
  GL27
  nad83
  nad.lst
  CH
  ITRF2000
  ITRF2008
  ITRF2014
)

#
# gridshift file
#

file(GLOB GSB_FILES  *.gsb)
file(GLOB GTX_FILES  *.gtx)
set(GRIDSHIFT_FILES ${GSB_FILES} ${GTX_FILES})

set(ALL_SQL_IN "${CMAKE_CURRENT_BINARY_DIR}/all.sql.in")
set(PROJ_DB "${CMAKE_CURRENT_BINARY_DIR}/proj.db")
include(sql_filelist.cmake)

add_custom_command(
  OUTPUT ${ALL_SQL_IN}
  COMMAND ${CMAKE_COMMAND} "-DALL_SQL_IN=${ALL_SQL_IN}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_all_sql_in.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  DEPENDS ${SQL_FILES}
  COMMENT "Generating all.sql.in"
  VERBATIM
)

add_custom_target(generate_all_sql_in ALL DEPENDS ${ALL_SQL_IN})

add_custom_command(
  OUTPUT ${PROJ_DB}
  COMMAND ${CMAKE_COMMAND} -E remove -f ${PROJ_DB}
  COMMAND ${EXE_SQLITE3} -init ${ALL_SQL_IN} ${PROJ_DB} .quit
  # note: we didn't port yet the foreign_key_check done in Makefile.am
  DEPENDS generate_all_sql_in ${ALL_SQL_IN}
  COMMENT "Generating proj.db"
  VERBATIM
)

add_custom_target(generate_proj_db ALL DEPENDS ${PROJ_DB})

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  foreach(FILE ${PROJ_DICTIONARY} ${GRIDSHIFT_FILES})
    configure_file(${FILE} ${FILE} COPYONLY)
  endforeach()
endif()

#
#install
#
set(ALL_DATA_FILE
  ${PROJ_DICTIONARY}
  ${GRIDSHIFT_FILES}
  ${PROJ_DB}
)
install(
  FILES ${ALL_DATA_FILE}
  DESTINATION ${DATADIR}
)

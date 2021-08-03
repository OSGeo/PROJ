function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

file(WRITE "${ALL_EQNR_SQL_IN}" "")
include(sql_equinor_filelist.cmake)
foreach(SQL_FILE ${SQL_FILES_EQUINOR})
  cat(${SQL_FILE} "${ALL_EQNR_SQL_IN}")
endforeach()

# Do ${PROJ_VERSION} substitution
message(STATUS "INPUT FILE IS: ")
message(STATUS "${ALL_EQNR_SQL_IN} ")
file(READ ${ALL_EQNR_SQL_IN} CONTENTS)
string(REPLACE "\${EQUINOR_VERSION}" "${EQUINOR_VERSION}" CONTENTS_MOD "${CONTENTS}")
file(WRITE "${ALL_EQNR_SQL_IN}" "${CONTENTS_MOD}")


execute_process(COMMAND "${EXE_SQLITE3}" "${EQUINOR_DB}"
                INPUT_FILE "${ALL_EQNR_SQL_IN}"
                RESULT_VARIABLE STATUS)

message(STATUS "STATUS VARIABLE IS ")
message(STATUS "${STATUS}")
message(STATUS "EQUINOR_DB VAR IS ")
message(STATUS "${EQUINOR_DB}")

if(STATUS AND NOT STATUS EQUAL 0)
  message(FATAL_ERROR "SQLite3 failed")
endif()

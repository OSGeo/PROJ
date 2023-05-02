function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

file(WRITE "${ALL_SQL_IN}" "")
include(sql_filelist.cmake)
foreach(SQL_FILE ${SQL_FILES})
  cat(${SQL_FILE} "${ALL_SQL_IN}")
endforeach()

# Do ${PROJ_VERSION} substitution
file(READ ${ALL_SQL_IN} CONTENTS)
string(REPLACE "\${PROJ_VERSION}" "${PROJ_VERSION}" CONTENTS_MOD "${CONTENTS}")
file(WRITE "${ALL_SQL_IN}" "${CONTENTS_MOD}")

if(IS_DIRECTORY ${PROJ_DB_CACHE_DIR})
  set(USE_PROJ_DB_CACHE_DIR TRUE)
  set(PROJ_DB_SQL_MD5_FILE "${PROJ_DB_CACHE_DIR}/proj.db.sql.md5")
  if(EXISTS "${PROJ_DB_SQL_MD5_FILE}")
    file(STRINGS "${PROJ_DB_SQL_MD5_FILE}" CACHED_PROJ_DB_SQL_MD5 LIMIT_COUNT 1)
  endif()
  file(MD5 "${ALL_SQL_IN}" PROJ_DB_SQL_MD5)
  set(CACHED_PROJ_DB "${PROJ_DB_CACHE_DIR}/proj.db")
  if("${PROJ_DB_SQL_MD5}" STREQUAL "${CACHED_PROJ_DB_SQL_MD5}" AND EXISTS "${CACHED_PROJ_DB}")
    message(STATUS "Reusing cached proj.db from ${PROJ_DB_CACHE_DIR}")
    get_filename_component(PROJ_DB_DIR "${PROJ_DB}" DIRECTORY)
    file(COPY "${CACHED_PROJ_DB}" DESTINATION "${PROJ_DB_DIR}")
    return()
  endif()
endif()

execute_process(COMMAND "${EXE_SQLITE3}" "${PROJ_DB}"
                INPUT_FILE "${ALL_SQL_IN}"
                RESULT_VARIABLE STATUS)

if(STATUS AND NOT STATUS EQUAL 0)
  message(FATAL_ERROR "Build of proj.db failed")
elseif(USE_PROJ_DB_CACHE_DIR)
  message(STATUS "Saving cache: ${CACHED_PROJ_DB}")
  file(COPY "${PROJ_DB}" DESTINATION "${PROJ_DB_CACHE_DIR}")
  file(WRITE "${PROJ_DB_SQL_MD5_FILE}" "${PROJ_DB_SQL_MD5}\n")
endif()

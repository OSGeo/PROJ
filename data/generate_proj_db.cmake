function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

# Generates a all.sql.in file from all the individual .sql files, taking
# into account if extra validation checks must be done before inserting data
# records
function(generate_all_sql_in ALL_SQL_IN_FILENAME EXTRA_VALIDATION OUT_MD5)
    set(PROJ_DB_EXTRA_VALIDATION ${EXTRA_VALIDATION})
    file(WRITE "${ALL_SQL_IN_FILENAME}" "")
    include(sql_filelist.cmake)
    foreach(SQL_FILE ${SQL_FILES})
      cat(${SQL_FILE} "${ALL_SQL_IN_FILENAME}")
    endforeach()

    # Compute the MD5 before PROJ_VERSION substitution to avoid updating the
    # expected MD5 if we just bump the PROJ_VERSION
    configure_file("${ALL_SQL_IN_FILENAME}" "${ALL_SQL_IN_FILENAME}.tmp" NEWLINE_STYLE UNIX)
    file(MD5 "${ALL_SQL_IN_FILENAME}.tmp" OUT_MD5_LOCAL)
    set(${OUT_MD5} "${OUT_MD5_LOCAL}" PARENT_SCOPE)

    # Do ${PROJ_VERSION} substitution
    file(READ ${ALL_SQL_IN_FILENAME} CONTENTS)
    string(REPLACE "\${PROJ_VERSION}" "${PROJ_VERSION}" CONTENTS_MOD "${CONTENTS}")
    file(WRITE "${ALL_SQL_IN_FILENAME}" "${CONTENTS_MOD}")
endfunction()

generate_all_sql_in("${ALL_SQL_IN}" OFF PROJ_DB_SQL_MD5)

if (NOT "${PROJ_DB_SQL_MD5}" STREQUAL "${PROJ_DB_SQL_EXPECTED_MD5}")
    message(WARNING "all.sql.in content has changed. Running extra validation checks when building proj.db...")

    set(ALL_SQL_IN_EXTRA_VALIDATION "${ALL_SQL_IN}.extra_validation")
    generate_all_sql_in("${ALL_SQL_IN_EXTRA_VALIDATION}" ON PROJ_DB_SQL_EXTRA_VALIDATION_MD5)

    set(PROJ_DB_EXTRA_VALIDATION_FILENAME "${PROJ_DB}.extra_validation")
    file(REMOVE "${PROJ_DB_EXTRA_VALIDATION_FILENAME}")
    execute_process(COMMAND "${EXE_SQLITE3}" "${PROJ_DB_EXTRA_VALIDATION_FILENAME}"
                    INPUT_FILE "${ALL_SQL_IN_EXTRA_VALIDATION}"
                    RESULT_VARIABLE STATUS)
    if(STATUS AND NOT STATUS EQUAL 0)
      message(FATAL_ERROR "Build of proj.db from ${ALL_SQL_IN_EXTRA_VALIDATION} failed")
    else()
      message(FATAL_ERROR "Update 'set(PROJ_DB_SQL_EXPECTED_MD5 ...)' line in data/CMakeLists.txt with ${PROJ_DB_SQL_MD5} value")
    endif()
endif()

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

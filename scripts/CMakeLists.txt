find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(PC_BASH_COMPLETION QUIET bash-completion)
  if (PC_BASH_COMPLETION_FOUND)
    pkg_get_variable(BASH_COMPLETIONS_FULL_DIR bash-completion completionsdir)
    pkg_get_variable(BASH_COMPLETIONS_PREFIX bash-completion prefix)
    if (BASH_COMPLETIONS_FULL_DIR
        AND BASH_COMPLETIONS_PREFIX
        AND BASH_COMPLETIONS_FULL_DIR MATCHES "^${BASH_COMPLETIONS_PREFIX}/")
      string(REGEX REPLACE "^${BASH_COMPLETIONS_PREFIX}/" "" BASH_COMPLETIONS_DIR_DEFAULT ${BASH_COMPLETIONS_FULL_DIR})
    endif ()
  endif ()
endif ()

if (NOT DEFINED BASH_COMPLETIONS_DIR_DEFAULT)
  include(GNUInstallDirs)
  set(BASH_COMPLETIONS_DIR_DEFAULT ${CMAKE_INSTALL_DATADIR}/bash-completion/completions)
endif ()

set(BASH_COMPLETIONS_DIR
    "${BASH_COMPLETIONS_DIR_DEFAULT}"
    CACHE PATH "Installation sub-directory for bash completion scripts")

if (NOT BASH_COMPLETIONS_DIR STREQUAL "")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/install_bash_completions.cmake.in
                 ${CMAKE_CURRENT_BINARY_DIR}/install_bash_completions.cmake @ONLY)
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_bash_completions.cmake)
endif ()

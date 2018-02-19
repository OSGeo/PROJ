# proj-config.cmake for the install tree.  It's installed in
# ${INSTALL_CMAKE_DIR} and @PROJECT_ROOT_DIR@ is the relative
# path to the root from there.  (Note that the whole install tree can
# be relocated.)
file(RELATIVE_PATH PROJECT_ROOT_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKECONFIGDIR} ${CMAKE_INSTALL_PREFIX})

string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
configure_file (project-config.cmake.in project-config.cmake @ONLY)
configure_file (project-config-version.cmake.in
  project-config-version.cmake @ONLY)
install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/project-config.cmake"
  DESTINATION "${CMAKECONFIGDIR}"
  RENAME "${PROJECT_NAME_LOWER}-config.cmake")
install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/project-config-version.cmake"
  DESTINATION "${CMAKECONFIGDIR}"
  RENAME "${PROJECT_NAME_LOWER}-config-version.cmake")
# Make information about the cmake targets (the library and the tools)
# available.
install (EXPORT targets
  FILE ${PROJECT_NAME_LOWER}-targets.cmake
  DESTINATION "${CMAKECONFIGDIR}")
install (EXPORT targets
  NAMESPACE ${PROJECT_NAME}::
  FILE ${PROJECT_NAME_LOWER}-namespace-targets.cmake
  DESTINATION "${CMAKECONFIGDIR}")

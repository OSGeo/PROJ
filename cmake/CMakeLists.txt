# proj-config.cmake for the install tree.  It's installed in
# ${INSTALL_CMAKE_DIR} and @PROJECT_ROOT_DIR@ is the relative
# path to the root from there.  (Note that the whole install tree can
# be relocated.)
file(RELATIVE_PATH PROJECT_ROOT_DIR
  ${CMAKE_INSTALL_PREFIX}/${CMAKECONFIGDIR} ${CMAKE_INSTALL_PREFIX})

# Variables needed by ${PROJECT_NAME_LOWER}-config-version.cmake
if (MSVC)
  # For checking the compatibility of MSVC_TOOLSET_VERSION; see
  # https://docs.microsoft.com/en-us/cpp/porting/overview-of-potential-upgrade-issues-visual-cpp
  # Assume major version number is obtained by dropping the last decimal
  # digit.
  math (EXPR MSVC_TOOLSET_MAJOR "${MSVC_TOOLSET_VERSION}/10")
else ()
  set (MSVC_TOOLSET_VERSION 0)
  set (MSVC_TOOLSET_MAJOR 0)
endif ()
if (CMAKE_CROSSCOMPILING)
  # Ensure that all "true" (resp. "false") settings are represented by
  # the same string.
  set (CMAKE_CROSSCOMPILING_STR "ON")
else ()
  set (CMAKE_CROSSCOMPILING_STR "OFF")
endif ()

string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
configure_file(project-config.cmake.in project-config.cmake @ONLY)
configure_file(project-config-version.cmake.in
  project-config-version.cmake @ONLY)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/project-config.cmake"
  DESTINATION "${CMAKECONFIGDIR}"
  RENAME "${PROJECT_NAME_LOWER}-config.cmake")
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/project-config-version.cmake"
  DESTINATION "${CMAKECONFIGDIR}"
  RENAME "${PROJECT_NAME_LOWER}-config-version.cmake")
# Make information about the cmake targets (the library and the tools)
# available.
install(EXPORT targets
  NAMESPACE ${PROJECT_NAME}::
  FILE ${PROJECT_NAME_LOWER}-targets.cmake
  DESTINATION "${CMAKECONFIGDIR}")

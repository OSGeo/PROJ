# The old CMake PROJECT-NAME was PROJ4.
set (PROJECT_LEGACY_NAME PROJ4)
string (TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
string (TOLOWER "${PROJECT_LEGACY_NAME}" PROJECT_LEGACY_LOWER)

# Starting with version 7.0, we install config-style find package
# files so that PROJ can be found with both
#
#   find_package(PROJ)
#   find_package(PROJ4)
#
# Here are the details...  The command
#
#   find_package(PROJ)
#
# if successful, will define variables
#
#   PROJ_FOUND
#   PROJ_VERSION
#   PROJ_LIBRARIES = PROJ::proj
#   PROJ_INCLUDE_DIRS
#   etc
#
# and will define targets
#
#   PROJ::proj
#   PROJ4::proj
#
# Similarly
#
#   find_package(PROJ4)
#
# if successful, will define variables
#
#   PROJ4_FOUND
#   PROJ4_VERSION
#   PROJ4_LIBRARIES = PROJ4::proj
#   PROJ4_INCLUDE_DIRS
#   etc
#
# and will define targets
#
#   PROJ::proj
#   PROJ4::proj
#
# Note that targets PROJ::proj and PROJ4::proj are provided in both
# cases.  However, no attempt is made to define both sets of variables
# with the two find_package options.  Doing so would just lead to user
# confusion.
#
# Because pre-7.0 versions of PROJ do not support find_package (PROJ)
# and do not define a target PROJ::proj
#
#   find_package(PROJ4)
#
# (instead of PROJ) should be used in any development scenarios which
# might involve (even indirectly) pre-7.0 versions of PROJ.
#
# At some future dates, find_package (PROJ4) will
#
#   define PROJ4_LIBRARIES = PROJ::proj
#   give WARNING message suggesting migration to find_package(PROJ)
#   be disallowed via the version checking code
#
# At some more distant date, the PROJ4::proj target will be retired.
#
# To support this change, the CACHE variables PROJ_CMAKE_SUBDIR (and
# CMAKECONFIGDIR) is now the "Parent of directory to install cmake
# config files into".
#
# All this messiness is confined to
#
#   cmake/CMakeLists.txt (this file)
#   cmake/project-config.cmake.in
#   cmake/project-config-version.cmake.in

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

foreach (PROJECT_VARIANT_NAME ${PROJECT_NAME} ${PROJECT_LEGACY_NAME})
  string (TOLOWER "${PROJECT_VARIANT_NAME}" PROJECT_VARIANT_LOWER)
  set (CMAKECONFIGSUBDIR "${CMAKECONFIGDIR}/${PROJECT_VARIANT_LOWER}")
  # proj-config.cmake for the install tree.  It's installed in
  # ${CMAKECONFIGSUBDIR} and @PROJECT_ROOT_DIR@ is the relative
  # path to the root from there.  (Note that the whole install tree can
  # be relocated.)
  file (RELATIVE_PATH PROJECT_ROOT_DIR
    ${CMAKE_INSTALL_PREFIX}/${CMAKECONFIGSUBDIR} ${CMAKE_INSTALL_PREFIX})
  configure_file (project-config.cmake.in
    project-${PROJECT_VARIANT_LOWER}-config.cmake @ONLY)
  configure_file (project-config-version.cmake.in
    project-${PROJECT_VARIANT_LOWER}-version.cmake @ONLY)
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/project-${PROJECT_VARIANT_LOWER}-config.cmake"
    DESTINATION "${CMAKECONFIGSUBDIR}"
    RENAME "${PROJECT_VARIANT_LOWER}-config.cmake")
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/project-${PROJECT_VARIANT_LOWER}-version.cmake"
    DESTINATION "${CMAKECONFIGSUBDIR}"
    RENAME "${PROJECT_VARIANT_LOWER}-config-version.cmake")
  # Make information about the cmake targets (the library and the tools)
  # available.
  install(EXPORT targets
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME_LOWER}-targets.cmake
    DESTINATION "${CMAKECONFIGSUBDIR}")
  install(EXPORT targets
    NAMESPACE ${PROJECT_LEGACY_NAME}::
    FILE ${PROJECT_LEGACY_LOWER}-targets.cmake
    DESTINATION "${CMAKECONFIGSUBDIR}")
endforeach ()


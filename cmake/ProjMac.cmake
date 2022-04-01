if(APPLE)
  set(FRAMEWORKDIR "Library/Frameworks" CACHE PATH
    "the path to install framework")
  set(BUNDLEDIR "Applications/OSGEO" CACHE PATH
    "the path to install bundle")
  file(RELATIVE_PATH BUNDLE_FRAME_REL_PATH_AAA "/${FRAMEWORKDIR}" "/aaa")
  string(LENGTH ${BUNDLE_FRAME_REL_PATH_AAA} AAA_LENGTH)
  math(EXPR RELATIVE_PATH_LENGTH "${AAA_LENGTH}-4")
  string(SUBSTRING ${BUNDLE_FRAME_REL_PATH_AAA}
    0 ${RELATIVE_PATH_LENGTH} BUNDLE_FRAME_REL_PATH)
  set(PROJ_INSTALL_NAME_DIR
    "@executable_path/${BUNDLE_FRAME_REL_PATH}/${FRAMEWORKDIR}")
else()
  set(FRAMEWORKDIR "")
  set(BUNDLEDIR "")
  set(PROJ_INSTALL_NAME_DIR "")
endif()

set(PROJ_RESOURCES "")

if(APPLE)
  option(BUILD_FRAMEWORKS_AND_BUNDLE
    "if set to ON, build a library framework and application bundle, \
otherwise install classical UNIX bin/lib" OFF)
  if(BUILD_FRAMEWORKS_AND_BUNDLE)
    set(CMAKE_INSTALL_BINDIR ${BUNDLEDIR})
  endif()
  print_variable(BUNDLEDIR)
  print_variable(PROJ_INSTALL_NAME_DIR)
  print_variable(FRAMEWORKDIR)
endif()

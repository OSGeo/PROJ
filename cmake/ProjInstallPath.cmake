#----------------------------------------------
# installation path settings
#----------------------------------------------
if(WIN32)
  if(DEFINED ENV{OSGEO4W_ROOT})
    set(OSGEO4W_ROOT_DIR $ENV{OSGEO4W_ROOT})
  else()
    set(OSGEO4W_ROOT_DIR c:/OSGeo4W)
  endif()
  set(DEFAULT_PROJ_ROOT_DIR ${OSGEO4W_ROOT_DIR})
endif()
if(UNIX)
  set(DEFAULT_PROJ_ROOT_DIR "/usr/local/")
endif()


if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${DEFAULT_PROJ_ROOT_DIR}
    CACHE PATH "Proj install prefix" FORCE)
endif()

#TODO
# for data install testing the PROJ_LIB envVar

string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
if(UNIX)
  include(GNUInstallDirs)
  set(DEFAULT_BIN_SUBDIR ${CMAKE_INSTALL_BINDIR})
  set(DEFAULT_LIB_SUBDIR ${CMAKE_INSTALL_LIBDIR})
  set(DEFAULT_DATA_SUBDIR ${CMAKE_INSTALL_DATAROOTDIR}/proj)
  set(DEFAULT_INCLUDE_SUBDIR ${CMAKE_INSTALL_INCLUDEDIR})
  set(DEFAULT_DOC_SUBDIR share/doc/${PROJECT_NAME_LOWER})
  set(DEFAULT_CMAKE_SUBDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
else()
  # Common locations for Unix and Mac OS X
  set(DEFAULT_BIN_SUBDIR bin)
  set(DEFAULT_LIB_SUBDIR lib)
  set(DEFAULT_DATA_SUBDIR share/proj)
  set(DEFAULT_DOC_SUBDIR doc/proj)
  set(DEFAULT_INCLUDE_SUBDIR include)
  set(DEFAULT_DOC_SUBDIR share/doc/proj)
  set(DEFAULT_CMAKE_SUBDIR lib/cmake)
endif()

# Locations are changeable by user to customize layout of PROJ installation
# (default values are platform-specific)
set(PROJ_BIN_SUBDIR ${DEFAULT_BIN_SUBDIR} CACHE STRING
  "Subdirectory where executables will be installed")
set(PROJ_LIB_SUBDIR ${DEFAULT_LIB_SUBDIR} CACHE STRING
  "Subdirectory where libraries will be installed")
set(PROJ_INCLUDE_SUBDIR ${DEFAULT_INCLUDE_SUBDIR} CACHE STRING
  "Subdirectory where header files will be installed")
set(PROJ_DATA_SUBDIR ${DEFAULT_DATA_SUBDIR} CACHE STRING
  "Subdirectory where data will be installed")
set(PROJ_DOC_SUBDIR ${DEFAULT_DOC_SUBDIR} CACHE STRING
  "Subdirectory where doc will be installed")
set(PROJ_CMAKE_SUBDIR ${DEFAULT_CMAKE_SUBDIR} CACHE STRING
  "Parent of subdirectory where cmake proj-config file will be installed")

# Mark *DIR variables as advanced and dedicated to use by power-users only.
mark_as_advanced(
  PROJ_ROOT_DIR
  PROJ_BIN_SUBDIR
  PROJ_LIB_SUBDIR
  PROJ_INCLUDE_SUBDIR
  PROJ_DATA_SUBDIR
  PROJ_DOC_SUBDIR
  PROJ_CMAKE_SUBDIR
)

set(DEFAULT_BINDIR "${PROJ_BIN_SUBDIR}")
set(DEFAULT_LIBDIR "${PROJ_LIB_SUBDIR}")
set(DEFAULT_DATADIR "${PROJ_DATA_SUBDIR}")
set(DEFAULT_DOCDIR "${PROJ_DOC_SUBDIR}")
set(DEFAULT_INCLUDEDIR "${PROJ_INCLUDE_SUBDIR}")
set(DEFAULT_CMAKEDIR "${PROJ_CMAKE_SUBDIR}")

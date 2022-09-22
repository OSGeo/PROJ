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

# first include proj library
# always need
include(lib_proj.cmake)

# configure executable build
option(BUILD_CS2CS "Build cs2cs (coordinate systems to coordinate systems translation tool)" OFF)
option(BUILD_PROJ "Build proj (cartographic projection tool : latlong <-> projected coordinates" OFF)
option(BUILD_GEOD "Build geod (computation of geodesic lines)" OFF)
option(BUILD_NAD2BIN "Build nad2bin (format conversion tool) " OFF)

if(BUILD_CS2CS)
 include(bin_cs2cs.cmake)
endif(BUILD_CS2CS)

if(BUILD_PROJ)
  include(bin_proj.cmake)
endif(BUILD_PROJ)

if(BUILD_GEOD)
  include(bin_geod.cmake)
endif(BUILD_GEOD)

if(BUILD_NAD2BIN)
  include(bin_nad2bin.cmake)
endif(BUILD_NAD2BIN)

set(CCT_SRC
  cct.cpp
  proj_strtod.cpp
  proj_strtod.h
)
set(CCT_INCLUDE optargpm.h)

source_group("Source Files\\Bin" FILES ${CCT_SRC})

add_executable(cct ${CCT_SRC} ${CCT_INCLUDE})
target_link_libraries(cct PRIVATE ${PROJ_LIBRARIES})

install(TARGETS cct
  DESTINATION ${CMAKE_INSTALL_BINDIR})

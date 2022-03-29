set(GIE_SRC
  gie.cpp
  proj_strtod.cpp
  proj_strtod.h
)
set(GIE_INCLUDE optargpm.h)

source_group("Source Files\\Bin" FILES ${GIE_SRC})

add_executable(gie ${GIE_SRC} ${GIE_INCLUDE})
target_link_libraries(gie PRIVATE ${PROJ_LIBRARIES})

if(BUILD_GIE)
  install(TARGETS gie
    DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

set(GIE_SRC
  apps/gie.cpp
  apps/proj_strtod.cpp
  apps/proj_strtod.h
)
set(GIE_INCLUDE apps/optargpm.h)

source_group("Source Files\\Bin" FILES ${GIE_SRC})

add_executable(gie ${GIE_SRC} ${GIE_INCLUDE})
target_link_libraries(gie PRIVATE ${PROJ_LIBRARIES})
target_compile_options(gie PRIVATE ${PROJ_CXX_WARN_FLAGS})

if(BUILD_GIE)
  install(TARGETS gie
    DESTINATION ${BINDIR})
endif()

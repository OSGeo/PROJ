set(CCT_SRC
  apps/cct.cpp
  apps/proj_strtod.cpp
  apps/proj_strtod.h
)
set(CCT_INCLUDE apps/optargpm.h)

source_group("Source Files\\Bin" FILES ${CCT_SRC})

add_executable(cct ${CCT_SRC} ${CCT_INCLUDE})
target_link_libraries(cct ${PROJ_LIBRARIES})
target_compile_options(cct PRIVATE ${PROJ_CXX_WARN_FLAGS})

install(TARGETS cct
  RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
  target_compile_definitions(cct PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()

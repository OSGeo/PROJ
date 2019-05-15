set(PROJ_SRC
  apps/proj.cpp
  apps/emess.cpp
  apps/utils.cpp
)

source_group("Source Files\\Bin" FILES ${PROJ_SRC})

add_executable(binproj ${PROJ_SRC})
set_target_properties(binproj
  PROPERTIES
  OUTPUT_NAME proj)
target_link_libraries(binproj ${PROJ_LIBRARIES})
target_compile_options(binproj PRIVATE ${PROJ_CXX_WARN_FLAGS})

install(TARGETS binproj
  RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
  target_compile_definitions(binproj PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()

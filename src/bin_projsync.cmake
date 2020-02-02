set(PROJSYNC_SRC apps/projsync.cpp)

source_group("Source Files\\Bin" FILES ${PROJSYNC_SRC})

add_executable(bin_projsync ${PROJSYNC_SRC})
set_target_properties(bin_projsync
  PROPERTIES
  OUTPUT_NAME projsync)
target_link_libraries(bin_projsync ${PROJ_LIBRARIES})
target_compile_options(bin_projsync PRIVATE ${PROJ_CXX_WARN_FLAGS})
install(TARGETS bin_projsync
  RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
  target_compile_definitions(bin_projsync PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()

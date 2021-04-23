set(PROJSYNC_SRC apps/projsync.cpp)

source_group("Source Files\\Bin" FILES ${PROJSYNC_SRC})

add_executable(bin_projsync ${PROJSYNC_SRC})
set_target_properties(bin_projsync
  PROPERTIES
  OUTPUT_NAME projsync)
target_link_libraries(bin_projsync PRIVATE ${PROJ_LIBRARIES})
target_compile_options(bin_projsync PRIVATE ${PROJ_CXX_WARN_FLAGS})
if(NLOHMANN_JSON STREQUAL "external")
  target_compile_definitions(bin_projsync PRIVATE EXTERNAL_NLOHMANN_JSON)
  target_link_libraries(bin_projsync PRIVATE nlohmann_json::nlohmann_json)
endif()

install(TARGETS bin_projsync
  RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_SHARED_LIBS)
  target_compile_definitions(bin_projsync PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()

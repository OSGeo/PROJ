set(PROJSYNC_SRC projsync.cpp)

source_group("Source Files\\Bin" FILES ${PROJSYNC_SRC})

add_executable(projsync ${PROJSYNC_SRC})
target_link_libraries(projsync PRIVATE ${PROJ_LIBRARIES})

if(NLOHMANN_JSON STREQUAL "external")
  target_compile_definitions(projsync PRIVATE EXTERNAL_NLOHMANN_JSON)
  target_link_libraries(projsync
    PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>)
endif()

install(TARGETS projsync
  DESTINATION ${CMAKE_INSTALL_BINDIR})

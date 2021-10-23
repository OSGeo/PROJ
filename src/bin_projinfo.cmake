set(PROJINFO_SRC apps/projinfo.cpp)

source_group("Source Files\\Bin" FILES ${PROJINFO_SRC})

add_executable(binprojinfo ${PROJINFO_SRC})
set_target_properties(binprojinfo
  PROPERTIES
  OUTPUT_NAME projinfo)
target_link_libraries(binprojinfo PRIVATE ${PROJ_LIBRARIES})
target_compile_options(binprojinfo PRIVATE ${PROJ_CXX_WARN_FLAGS})

install(TARGETS binprojinfo
  DESTINATION ${BINDIR})

if(CURL_ENABLED)
  target_compile_definitions(binprojinfo PRIVATE -DCURL_ENABLED)
endif()

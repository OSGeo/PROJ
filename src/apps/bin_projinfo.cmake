set(PROJINFO_SRC projinfo.cpp)

source_group("Source Files\\Bin" FILES ${PROJINFO_SRC})

add_executable(projinfo ${PROJINFO_SRC})
target_link_libraries(projinfo PRIVATE ${PROJ_LIBRARIES})

install(TARGETS projinfo
  DESTINATION ${BINDIR})

if(CURL_ENABLED)
  target_compile_definitions(projinfo PRIVATE -DCURL_ENABLED)
endif()

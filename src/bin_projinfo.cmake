set(PROJINFO_SRC apps/projinfo.cpp)

source_group("Source Files\\Bin" FILES ${PROJINFO_SRC})

add_executable(binprojinfo ${PROJINFO_SRC})
set_target_properties(binprojinfo
  PROPERTIES
  OUTPUT_NAME projinfo)
target_link_libraries(binprojinfo ${PROJ_LIBRARIES})
target_compile_options(binprojinfo PRIVATE ${PROJ_CXX_WARN_FLAGS})

install(TARGETS binprojinfo
  RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_SHARED_LIBS)
  target_compile_definitions(binprojinfo PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()

if(CURL_ENABLED)
  target_compile_definitions(binprojinfo PRIVATE -DCURL_ENABLED)
endif()

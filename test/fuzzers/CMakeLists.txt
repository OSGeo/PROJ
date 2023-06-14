if(NOT WIN32)

  add_compile_options(${PROJ_CXX_WARN_FLAGS})

  add_executable(proj_crs_to_crs_fuzzer proj_crs_to_crs_fuzzer.cpp)
  target_compile_definitions(proj_crs_to_crs_fuzzer PRIVATE -DSTANDALONE)
  target_link_libraries(proj_crs_to_crs_fuzzer PRIVATE ${PROJ_LIBRARIES})
endif()

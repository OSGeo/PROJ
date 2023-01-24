option(BUILD_BENCHMARKS "Build PROJ benchmark applications" ON)
if(NOT BUILD_BENCHMARKS)
    return()
endif()

add_compile_options(${PROJ_CXX_WARN_FLAGS})

add_executable(bench_proj_trans bench_proj_trans.cpp)
target_link_libraries(bench_proj_trans PRIVATE ${PROJ_LIBRARIES})


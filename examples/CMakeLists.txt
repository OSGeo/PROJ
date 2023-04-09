add_executable(pj_obs_api_mini_demo pj_obs_api_mini_demo.c)
target_link_libraries(pj_obs_api_mini_demo PRIVATE ${PROJ_LIBRARIES})

add_executable(crs_to_geodetic crs_to_geodetic.c)
target_link_libraries(crs_to_geodetic PRIVATE ${PROJ_LIBRARIES})

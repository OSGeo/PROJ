option(TESTING_USE_NETWORK
  "Permit use of network to fetch test requirements (if needed) \
  and run network-dependent tests. Default ON." ON
)

set(HAS_NETWORK OFF)  # evaluate if ON below

if(TESTING_USE_NETWORK)
  if(NOT CMAKE_REQUIRED_QUIET)
    # CMake 3.17+ use CHECK_START/CHECK_PASS/CHECK_FAIL
    message(STATUS "Checking if network is available")
  endif()
  set(NO_CONNECTION 1)
  find_program(HAS_CURL curl)
  if(HAS_CURL)
    # First try with curl as we can get an 'instant' answer if it is there
    execute_process(
      COMMAND ${HAS_CURL} -I https://www.google.com
      OUTPUT_QUIET
      ERROR_QUIET
      RESULT_VARIABLE NO_CONNECTION
    )
  else()
    find_program(HAS_PING ping)
    if(HAS_PING)
      # Then fallback to ping -- https://stackoverflow.com/a/68376537/
      if(WIN32)
        set(PING_COUNT "-n")
      else()
        set(PING_COUNT "-c")
      endif()
      execute_process(
        COMMAND ${HAS_PING} www.google.com ${PING_COUNT} 2
        OUTPUT_QUIET
        ERROR_QUIET
        RESULT_VARIABLE NO_CONNECTION
      )
    else()
      message(WARNING
        "Cannot establish if network is avalable - "
        "'curl' or 'ping' not found"
      )
    endif()
  endif()
  if(NO_CONNECTION EQUAL 0)
    set(HAS_NETWORK ON)
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Checking if network is available - yes")
    endif()
  else()
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS
        "Checking if network is available - no; skipping network-dependent tests."
      )
    endif()
  endif()
else()
  message(STATUS "Network access not premitted (TESTING_USE_NETWORK=OFF)")
endif()

option(RUN_NETWORK_DEPENDENT_TESTS
  "Whether to run tests dependent on network availability"
  ${HAS_NETWORK}
)

# Regression tests
proj_add_gie_test("Builtins" "gie/builtins.gie")
proj_add_gie_test("Builtins2" "gie/more_builtins.gie")
proj_add_gie_test("Axisswap" "gie/axisswap.gie")
proj_add_gie_test("Ellipsoid" "gie/ellipsoid.gie")
proj_add_gie_test("GDA" "gie/GDA.gie")
proj_add_gie_test("4D-API-cs2cs-style" "gie/4D-API_cs2cs-style.gie")
proj_add_gie_test("DHDN_ETRS89" "gie/DHDN_ETRS89.gie")
proj_add_gie_test("Unitconvert" "gie/unitconvert.gie")
proj_add_gie_test("adams_hemi" "gie/adams_hemi.gie")
proj_add_gie_test("adams_ws1" "gie/adams_ws1.gie")
proj_add_gie_test("adams_ws2" "gie/adams_ws2.gie")
proj_add_gie_test("guyou" "gie/guyou.gie")
proj_add_gie_test("peirce_q" "gie/peirce_q.gie")
proj_add_gie_test("tinshift" "gie/tinshift.gie")

if(TIFF_ENABLED)
proj_add_gie_test("Deformation" "gie/deformation.gie")
proj_add_gie_test("geotiff_grids" "gie/geotiff_grids.gie")
proj_add_gie_test("defmodel" "gie/defmodel.gie")
proj_add_gie_test("gridshift" "gie/gridshift.gie")
# Does not really require the network, but test running with PROJ_NETWORK=ON
proj_add_gie_network_dependent_test("gridshift_network_enabled" "gie/gridshift.gie")
endif()

if(TIFF_ENABLED AND CURL_ENABLED AND RUN_NETWORK_DEPENDENT_TESTS)
proj_add_gie_network_dependent_test("nkg" "gie/nkg.gie")
endif()

# GIGS tests. Uncommented tests are expected to fail due to issues with
# various projections. Should be investigated further and fixed.
proj_add_gie_test("GIGS-5101.1-jhs" "gigs/5101.1-jhs.gie")
proj_add_gie_test("GIGS-5101.2-jhs" "gigs/5101.2-jhs.gie")
proj_add_gie_test("GIGS-5101.3-jhs" "gigs/5101.3-jhs.gie")
proj_add_gie_test("GIGS-5101.4-jhs-etmerc" "gigs/5101.4-jhs-etmerc.gie")
# Same as above, but using etmerc instead of tmerc
#proj_add_gie_test("GIGS-5101.4-jhs" "gigs/5101.4-jhs.gie")
proj_add_gie_test("GIGS-5102.1" "gigs/5102.1.gie")
proj_add_gie_test("GIGS-5102.2" "gigs/5102.2.gie")
proj_add_gie_test("GIGS-5103.1" "gigs/5103.1.gie")
proj_add_gie_test("GIGS-5103.2" "gigs/5103.2.gie")
proj_add_gie_test("GIGS-5103.3" "gigs/5103.3.gie")
proj_add_gie_test("GIGS-5104" "gigs/5104.gie")
#proj_add_gie_test("GIGS-5105.1" "gigs/5105.1.gie")
proj_add_gie_test("GIGS-5105.2" "gigs/5105.2.gie")
proj_add_gie_test("GIGS-5106" "gigs/5106.gie")
proj_add_gie_test("GIGS-5107" "gigs/5107.gie")
proj_add_gie_test("GIGS-5108" "gigs/5108.gie")
proj_add_gie_test("GIGS-5109" "gigs/5109.gie")
#proj_add_gie_test("GIGS-5110" "gigs/5110.gie")
proj_add_gie_test("GIGS-5111.1" "gigs/5111.1.gie")
#proj_add_gie_test("GIGS-5111.2" "gigs/5111.2.gie")
proj_add_gie_test("GIGS-5112" "gigs/5112.gie")
proj_add_gie_test("GIGS-5113" "gigs/5113.gie")
proj_add_gie_test("GIGS-5201" "gigs/5201.gie")
#proj_add_gie_test("GIGS-5203" "gigs/5203.1.gie")
#proj_add_gie_test("GIGS-5204.1" "gigs/5204.1.gie")
#proj_add_gie_test("GIGS-5205.1" "gigs/5205.1.gie")
#proj_add_gie_test("GIGS-5206" "gigs/5206.gie")
#proj_add_gie_test("GIGS-5207.1" "gigs/5207.1.gie")
#proj_add_gie_test("GIGS-5207.2" "gigs/5207.2.gie")
proj_add_gie_test("GIGS-5208" "gigs/5208.gie")

#SET(CATCH2_INCLUDE catch.hpp)

#SET(TEST_MAIN_SRC test_main.cpp)
#set(TEST_MAIN_LIBRARIES test_main)
#add_library( ${TEST_MAIN_LIBRARIES}
#                    ${TEST_MAIN_SRC}
#                    ${CATCH2_INCLUDE}  )

add_subdirectory(cli)
add_subdirectory(unit)
add_subdirectory(benchmark)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fuzzers")
  add_subdirectory(fuzzers)
endif()



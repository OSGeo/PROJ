#
# test
#
if(BUILD_CCT)
  set(CCT_EXE "$<TARGET_FILE:cct>")
endif()
if(BUILD_CS2CS)
  set(CS2CS_EXE "$<TARGET_FILE:cs2cs>")
endif()
if(BUILD_PROJ)
  set(PROJ_EXE "$<TARGET_FILE:binproj>")
  if(UNIX)
    # invproj is a symlink
    # set(INVPROJ_EXE "$<TARGET_PROPERTY:invproj,DEPENDS>")  # TODO: find working genexpr
    set(INVPROJ_EXE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/invproj${CMAKE_EXECUTABLE_SUFFIX}")
  else()
    set(INVPROJ_EXE "$<TARGET_FILE:invproj>")
  endif()
endif()
if(BUILD_PROJINFO)
  set(PROJINFO_EXE "$<TARGET_FILE:projinfo>")
endif()
if(BUILD_PROJSYNC)
  set(PROJSYNC_EXE "$<TARGET_FILE:projsync>")
endif()

if(UNIX)
  if(BUILD_CS2CS)
    proj_add_test_script_sh(test_cs2cs_locale.sh CS2CS_EXE)
  endif()
  if(BUILD_PROJINFO)
    proj_add_test_script_sh(test_projinfo.sh PROJINFO_EXE)
  endif()
  if(BUILD_PROJSYNC)
    proj_add_test_script_sh(test_projsync.sh PROJSYNC_EXE)
  endif()
endif()

macro(find_Python_package PACKAGE VARIABLE)
  if(NOT DEFINED "${VARIABLE}")
    if(NOT CMAKE_REQUIRED_QUIET)
      # CMake 3.17+ use CHECK_START/CHECK_PASS/CHECK_FAIL
      message(STATUS "Looking for ${PACKAGE}")
    endif()
    if(Python_VERSION VERSION_GREATER_EQUAL "3.8.0")
      # importlib.metadata was added in version 3.8
      set(CMD "from importlib.metadata import version; print(version('${PACKAGE}'), end='')")
      execute_process(
        COMMAND ${Python_EXECUTABLE} -c "${CMD}"
        RESULT_VARIABLE EXIT_CODE
        OUTPUT_VARIABLE ${PACKAGE}_VERSION
        ERROR_QUIET
      )
    else()
      # importlib_metadata backport needed for older Python versions
      execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import importlib_metadata"
        RESULT_VARIABLE EXIT_CODE
        OUTPUT_QUIET ERROR_QUIET
      )
      if(EXIT_CODE EQUAL 0)
        set(CMD "from importlib_metadata import version; print(version('${PACKAGE}'), end='')")
        execute_process(
            COMMAND ${Python_EXECUTABLE} -c "${CMD}"
            RESULT_VARIABLE EXIT_CODE
            OUTPUT_VARIABLE ${PACKAGE}_VERSION
            ERROR_QUIET
        )
      else()
        message(WARNING "importlib_metadata backport package required for Python <3.8")
      endif()
    endif()
    if(EXIT_CODE EQUAL 0)
      if(NOT CMAKE_REQUIRED_QUIET)
        message(STATUS "Looking for ${PACKAGE} - found version \"${${PACKAGE}_VERSION}\"")
      endif()
      set(${VARIABLE} 1 CACHE INTERNAL "Have package ${PACKAGE}")
    else()
      if(NOT CMAKE_REQUIRED_QUIET)
        message(STATUS "Looking for ${PACKAGE} - not found")
      endif()
      set(${VARIABLE} "" CACHE INTERNAL "Have package ${PACKAGE}")
    endif()
  endif()
endmacro()

# Evaluate if Python can be used for CLI testing
find_package(Python COMPONENTS Interpreter)
set(Python_for_cli_tests OFF)
if(Python_FOUND)
  if(Python_VERSION VERSION_GREATER_EQUAL "3.7.0")
    find_Python_package("ruamel.yaml" HAS_RUAMEL_YAML)
    if(HAS_RUAMEL_YAML)
      set(Python_for_cli_tests ON)
    else()
      find_Python_package("pyyaml" HAS_PYYAML)
      if(HAS_PYYAML)
        set(Python_for_cli_tests ON)
      endif()
    endif()
  else()
    message(WARNING "Python Interpreter version is too old")
  endif()
endif()

if(Python_for_cli_tests)
  if(BUILD_CCT)
    proj_run_cli_test(test_cct.yaml CCT_EXE)
  endif()
  if(BUILD_CS2CS)
    proj_run_cli_test(test_cs2cs_datumfile.yaml CS2CS_EXE)
    proj_run_cli_test(test_cs2cs_flaky.yaml CS2CS_EXE)
    proj_run_cli_test(test_cs2cs_ignf.yaml CS2CS_EXE)
    proj_run_cli_test(test_cs2cs_ntv2.yaml CS2CS_EXE)
    proj_run_cli_test(test_cs2cs_various.yaml CS2CS_EXE)
  endif()
  if(BUILD_PROJ)
    proj_run_cli_test(test_proj.yaml PROJ_EXE)
    proj_run_cli_test(test_proj_nad27.yaml PROJ_EXE)
    proj_run_cli_test(test_proj_nad83.yaml PROJ_EXE)
    proj_run_cli_test(test_invproj.yaml INVPROJ_EXE)
  endif()
  if(BUILD_PROJINFO)
    proj_run_cli_test(test_projinfo.yaml PROJINFO_EXE)
  endif()

  # auto-test run_cli_test.py if pytest available
  find_Python_package("pytest" HAS_PYTEST)
  if(HAS_PYTEST)
    add_test(NAME pytest_run_cli_test
      COMMAND ${Python_EXECUTABLE} -m pytest -vv
        ${PROJ_SOURCE_DIR}/test/cli/run_cli_test.py
    )
  endif()
else()
  message(WARNING "Python/YAML command-line interface tests not run")
endif()

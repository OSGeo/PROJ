@echo off
:: Post-install tests with CMake
::
:: First required argument is the installed prefix, which
:: is used to set CMAKE_PREFIX_PATH
:: Second argument is either shared (default) or static
:: Additionally, several environment variable are required
::  - CMAKE_GENERATOR - (e.g.) Ninja
::  - VCPKG_ROOT - (e.g.) C:\Tools\vcpkg
::  - platform - x64 or x86

If not defined CMAKE_GENERATOR (
  Echo CMAKE_GENERATOR must be set 1>&2
  Exit /B 1
)
If not defined VCPKG_ROOT (
  Echo VCPKG_ROOT not defined for Vcpkg 1>&2
  Exit /B 1
)
Set VCPKG_INSTALLED=%VCPKG_ROOT%\installed\%platform%-windows

Set CMAKE_PREFIX_PATH=%1
If not defined CMAKE_PREFIX_PATH (
  Echo First positional argument CMAKE_PREFIX_PATH required 1>&2
  Exit /B 1
)
Set BUILD_MODE=%2
If not defined BUILD_MODE (
  Set BUILD_MODE=shared
)
Set _ValidBuildMode=0
If %BUILD_MODE% EQU shared (
  Set _ValidBuildMode=1
  Setlocal EnableDelayedExpansion
  Set PATH=!CMAKE_PREFIX_PATH!\bin;!VCPKG_INSTALLED!\bin;!PATH!
  Setlocal DisableDelayedExpansion
)
If %BUILD_MODE% EQU static Set _ValidBuildMode=1
If %_ValidBuildMode% NEQ 1 (
  Echo Second argument must be either shared ^(default^) or static 1>&2
  Exit /B 1
)

Echo Running post-install tests with CMake (%BUILD_MODE%)

Set _InitDir=%~dp0
Cd %_InitDir%

Echo Testing C app
Cd c_app
Call :cmake_build_ctest PROJ || (Cd %_InitDir% & Exit /B 1)
Call :cmake_build_ctest PROJ4 || (Cd %_InitDir% & Exit /B 1)
Cd ..

Echo Testing C++ app
Cd cpp_app
Call :cmake_build_ctest PROJ || (Cd %_InitDir% & Exit /B 1)
Call :cmake_build_ctest PROJ4 || (Cd %_InitDir% & Exit /B 1)
Cd ..

Echo Finished running post-install tests CMake (%BUILD_MODE%)
Goto :EOF

:cmake_build_ctest
If exist build Rd /s /q build || Exit /B 1

cmake ^
  -D CMAKE_BUILD_TYPE=Release ^
  -D CMAKE_COMPILE_WARNING_AS_ERROR=ON ^
  -D CMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake ^
  -D CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% ^
  -D USE_PROJ_NAME=%1 ^
  -S . -B build || Exit /B 1

cmake --build build --config Release || Exit /B 1
Cd build
ctest --output-on-failure -C Release || Exit /B 1

Cd ..
Rd /s /q build

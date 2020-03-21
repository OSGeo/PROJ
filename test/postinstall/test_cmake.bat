@echo off
:: Post-install tests with CMake
::
:: First required argument is the installed prefix, which
:: is used to set CMAKE_PREFIX_PATH

echo Running post-install tests with CMake

set CMAKE_PREFIX_PATH=%1
if not defined CMAKE_PREFIX_PATH (
    echo First positional argument CMAKE_PREFIX_PATH required
    exit /B 1
)

echo CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%

cd %~dp0

cd testappprojinfo
del /f /q build 2> nul

:: Check CMake project name PROJ
md build
cd build
cmake -G "%VS_FULL%" -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -DUSE_PROJ_NAME=PROJ ..  || exit /B 2
cmake --build . --config Release || exit /B 3
ctest --build-config Release -VV || exit /B 4
cd ..
del /f /q build

:: Check legacy CMake project name PROJ4
md build
cd build
cmake -G "%VS_FULL%" -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -DUSE_PROJ_NAME=PROJ4 .. || exit /B 2
cmake --build . --config Release || exit /B 3
ctest --build-config Release -VV || exit /B 4
cd ..
del /f /q build

cd ..

# Post-install tests

The purpose of these tests is to check if an installed PROJ library can be used to configure and build other applications. It provides additional test coverage for the CMake config files and with `proj.pc` used for pkg-config.

## Applications

The two trivial applications `c_app` and `cpp_app` use C and C++ compilers, respectively. They function similarly, taking only one flag `-t` for an example coordinate transform, `-s` to show searchpath, and `-v` for the version of PROJ.

## Configure method

Each configure method is driven using a script with two arguments:
1. Path to a PROJ install prefix (containing `bin`, `lib`, etc.)
2. An optional build mode `shared` (default) or `static`

### CMake

Use `test_cmake.sh` for Unix-like or `test_cmake.bat` for Windows. This checks that `find_package(PROJ)` (or legacy `find_package(PROJ4)`) can be used with `target_link_libraries(mytarget PRIVATE PROJ::proj)`. The C-based app enables C++ when linking to static libproj. These scripts will only work with PROJ built using CMake, and will not work with PROJ built using Autotools.

### Makefile / pkg-config

`test_pkg-config.sh` uses a simple Makefile named `makefile.mak` (so it can co-exist with Autotools) that calls pkg-config to get include and library paths. If static linking is requested on Linux, `LDFLAGS` is modified to specify `-Wl,-Bstatic -lproj -Wl,-Bdynamic`.

### Autotools / pkg-config

`test_autotools.sh` uses Autoconf's `PKG_CHECK_MODULES([PROJ], [proj])` to get include and library paths for Automake. Static linking to libproj is enabled with `--enable-static-proj` configure option, which modifies `PROJ_LIBS` similar to the Makefile example.

## Build mode

Two build modes are tested with each configure method:
1. `shared` for dynamic linking with libproj (or proj.dll for MSVC)
2. `static` for static linking with libproj (or proj.lib for MSVC); note that static linking of dependants is out-of-scope with these tests, so only libproj is assumed to be static

## Tests

All configure methods share the same suite of tests for `c_app` and `cpp_app`:

- `test_libpath` - examines the shared object dependencies to see if "libproj" is found with the correct path (for `shared` build mode), or not listed as a dependency for builds with static libproj.
- `test_transform` - example coordinate transform to ensure basic functions work as expected.
- `test_searchpath` - compares `searchpath` (from `proj_info()`) to either `datadir` via pkg-config or relative path `PROJ_DIR/../../../share/proj` via CMake.
- `test_version` - compares PROJ version components from `proj_info()` to the version from pkg-config or CMake.


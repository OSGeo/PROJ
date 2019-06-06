# PROJ

[![Travis Status](https://travis-ci.com/OSGeo/PROJ.svg?branch=master)](https://travis-ci.com/OSGeo/PROJ)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/por8r52bjt8ocga9?svg=true)](https://ci.appveyor.com/project/OSGeo/PROJ)
[![Coveralls Status](https://coveralls.io/repos/github/OSGeo/PROJ/badge.svg?branch=master)](https://coveralls.io/github/OSGeo/PROJ?branch=master)
[![Gitter](https://badges.gitter.im/OSGeo/proj.4.svg)](https://gitter.im/OSGeo/proj.4)
[![Mailing List](https://img.shields.io/badge/PROJ-mailing%20list-4eb899.svg)](http://lists.osgeo.org/mailman/listinfo/proj)

PROJ is a generic coordinate transformation software, that transforms
coordinates from one coordinate reference system (CRS) to another.
This includes cartographic projections as well as geodetic transformations.

For more information on the PROJ project please see the web page at:

https://proj.org/

The PROJ mailing list can be found at:

https://lists.osgeo.org/mailman/listinfo/proj/

See the NEWS file for changes between versions.

The following command line utilities are included in the PROJ package:

- proj, for cartographic projection of geodetic coordinates
- cs2cs, for transformation from one CRS to another CRS.
- geod, for geodesic (great circle) computations.
- cct, for generic Coordinate Conversions and Transformations.
- gie, the Geospatial Integrity Investigation Environment.
- projinfo, for geodetic object and coordinate operation queries

## Installation
### Build dependencies

PROJ requires C and C++11 compilers.
It also requires SQLite3 (headers, library and executable).

### Building with CMake

    cd proj
    mkdir build
    cd build
    cmake ..
    cmake --build .

On Windows, one may need to specify generator:

    cmake -G "Visual Studio 15 2017" ..

If the SQLite3 dependency is installed in a custom location, specify the
paths to the include directory and the library:

    cmake -DSQLITE3_INCLUDE_DIR=/opt/SQLite/include -DSQLITE3_LIBRARY=/opt/SQLite/lib/libsqlite3.so ..

Tests are run with

    ctest

The test suite requires that the proj-datumgrid package is installed
in `PROJ_LIB`.

### Building on Unix/Linux

FSF's configuration procedure is used to ease installation of the
PROJ system.

The default destination path prefix for installed files is `/usr/local`.
Results from the installation script will be placed into subdirectories
`bin`, `include`, `lib`, `man/man1` and `man/man3`.
If this default path prefix is proper, then execute:

    ./configure

See the note below if you are building PROJ directly from the git repository.

If another path prefix is required, then execute:

    ./configure --prefix=/my/path

In either case, the directory of the prefix path must exist and be
writable by the installer.
After executing configure, execute:

    make
    make install

The install target will create, if necessary, all required sub-directories.

Tests are run with

    make check

The test suite requires that the proj-datumgrid package is installed
in `PROJ_LIB`.

#### Building from the git repository

If you are building from the git repository you have to first run

    ./autogen.sh

which will generate a configure script that can be used as described above.

### Distribution files and format

Sources are distributed in one or more files.  The principle elements
of the system are in a compress tar file named `proj-x.y.z.tar.gz` where
"x" will indicate major release number, "y" indicates minor release
number and "z" indicates the patch number of the release.

In addition to the PROJ software package, distributions of datum
conversion grid files and PROJ parameter files are also available.
The grid package is distributed under the name `proj-datumgrid-x.y.zip`,
where "x" is the major release version and "y" the minor release
version numbers. Similarly regional packages are distributed. The
regional packages contain resources that are not essential to the
functionality of PROJ but still of value to users in the region
specific to the package. All grids that were in proj-datumgrids-1.6
remain in proj-datumgrids-1.7; the regional datumgrid files contain
grids for datums not previously supported (prior to PROJ 5.0.0).

The resource packages can be downloaded from the PROJ.4 website.

More info on the contents of the various resource packages can be
found at the
[proj-datumgrid GitHub repository](https://github.com/OSGeo/proj-datumgrid).

The resource file packages should be extracted to `PROJ_LIB`
where PROJ will find them after installation. The default location of
`PROJ_LIB` on UNIX-based systems is `/usr/local/share/proj` but it may
be changed to a different directory. On Windows you have to define
`PROJ_LIB` yourself.

### Citing PROJ in publications

See [CITATION](CITATION)

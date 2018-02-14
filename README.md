# PROJ

[![Travis Status](https://travis-ci.org/OSGeo/proj.4.svg?branch=master)](https://travis-ci.org/OSGeo/proj.4)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/584j49uguwoo5evi?svg=true)](https://ci.appveyor.com/project/OSGeo/proj-4)
[![Coveralls Status](https://coveralls.io/repos/github/OSGeo/proj.4/badge.svg?branch=master)](https://coveralls.io/github/OSGeo/proj.4?branch=master)

PROJ is a generic coordinate transformation software, that transforms
coordinates from one coordinate reference system (CRS) to another.
This includes cartographic projections as well as geodetic transformations.

For more information on the PROJ.4 project please see the web page at:

http://proj4.org/

The PROJ.4 mailing list can be found at:

http://lists.maptools.org/mailman/listinfo/proj

See the NEWS file for changes between versions.

The following command line utilities are included in the PROJ package:

- proj, for cartographic projection of geodetic coordinates
- cs2cs, for transformation from one CRS to another CRS.
- geod, for geodesic (great circle) computations.
- cct, for generic Coordinate Conversions and Transformations.
- gie, the Geospatial Integrity Investigation Environment.


## Installation
### Building with CMake

    cd proj
    mkdir build
    cd build
    cmake ..
    cmake --build .

On Windows, one may need to specify generator:

    cmake -G "Visual Studio 15 2017" ..

### Building on Unix/Linux

FSF's configuration procedure is used to ease installation of the
PROJ system.

The default destination path prefix for installed files is `/usr/local`.
Results from the installation script will be placed into subdirectories
`bin`, `include`, `lib`, `man/man1` and `man/man3`.
If this default path prefix is proper, then execute:

    ./configure

If another path prefix is required, then execute:

    ./configure --prefix=/my/path

In either case, the directory of the prefix path must exist and be
writable by the installer.

After executing configure, execute:

    make
    make install

The install target will create, if necessary, all required sub-directories.

### Building on Windows with NMAKE

PROJ can be built with Microsoft Visual C/C++ using the `makefile.vc`
in the `PROJ` directory.  First edit the `PROJ\nmake.opt` and
modify the `INSTDIR` value at the top to point to the directory
where the PROJ tree shall be installed.
If you want to install into `C:\PROJ`, it can remain unchanged.
Then use the `makefile.vc` to build the software e.g.:

    C:\> cd proj
    C:\PROJ> nmake /f makefile.vc
    C:\PROJ> nmake /f makefile.vc install-all

Note that you have to have the VC++ environment variables, and path
setup properly.  This may involve running the `VCVARS32.BAT`
script out of the Visual C++ tree.

The `makefile.vc` builds `proj.exe`, `proj.dll` and `proj.lib`.


### Distribution files and format

Sources are distributed in one or more files.  The principle elements
of the system are in a compress tar file named `proj-x.y.z.tar.gz` where
"x" will indicate major release number, "y" indicates minor release
number and "z" indicates the patch number of the release.

In addition to the PROJ software package, a distribution of datum
conversion grid files is also available. The grid package is
distributed under the name `proj-datumgrid-x.y.zip`, where "x" is the
major release version and "y" the minor release version numbers.
The grid package can be downloaded from the PROJ.4 website.

.. _install:

================================================================================
Installation
================================================================================

These pages describe how to install PROJ on your computer without compiling it
yourself. Below are guides for installing on Windows, Linux and Mac OS X. This
is a good place to get started if this is your first time using PROJ. More
advanced users may want to compile the software themselves.

Installation from package management systems
################################################################################


Cross platform
--------------------------------------------------------------------------------

PROJ is also available via cross platform package managers.

Conda
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The conda package manager includes several PROJ packages. We recommend installing
from the ``conda-forge`` channel::

    conda install -c conda-forge proj

Using ``conda`` you can also install the PROJ data package. Here's how to install
the `proj-data` package::

    conda install -c conda-forge proj-data

Available is also the legacy packages ``proj-datumgrid-europe``,
``proj-datumgrid-north-america``, ``proj-datumgrid-oceania`` and
``proj-datumgrid-world``.

.. tip::
   Read more about the various datumgrid packages available :ref:`here<datumgrid>`.

Docker
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

A `Docker`_ image with just PROJ binaries and a full compliment of grid shift
files is available on `DockerHub`_. Get the package with::

    docker pull osgeo/proj

.. _`Docker`: https://www.docker.com/
.. _`DockerHub`: https://hub.docker.com/r/osgeo/proj/

Windows
--------------------------------------------------------------------------------

The simplest way to install PROJ on Windows is to use the `OSGeo4W`_ software
distribution. OSGeo4W provides easy access to many popular open source geospatial
software packages. After installation you can use PROJ from the OSGeo4W shell.
To install PROJ do the following:

.. note::
    If you have already installed software via OSGeo4W on your computer, or if
    you have already installed QGIS on your computer, it is likely that PROJ is 
    already installed. Type "OSGeo4W Shell" in your start menu and check whether
    that gives a match.

1. Download the `64 bit`_ installer.
2. Run the OSGeo4W setup program.
3. Select "Advanced Install" and press Next.
4. Select "Install from Internet" and press Next.
5. Select a installation directory. The default suggestion is fine in most cases. Press Next.
6. Select "Local package directory". The default suggestion is fine in most cases. Press Next.
7. Select "Direct connection" and press Next.
8. Choose the download.osgeo.org server and press Next.
9. Find "proj" under "Commandline_Utilities" and click the package in the "New" column until the version you want to install appears.
10. Press next to install PROJ.

You should now have a "OSGeo" menu in your start menu. Within that menu you can
find the "OSGeo4W Shell" where you have access to all the OSGeo4W applications,
including proj.

For those who are more inclined to the command line, steps 2--10 above can be
accomplished by executing the following command::

   C:\temp\osgeo4w-setup.exe -q -k -r -A -s https://download.osgeo.org/osgeo4w/v2/ -P proj

.. _`OSGeo4W`: https://trac.osgeo.org/osgeo4w/
.. _`64 bit`: https://download.osgeo.org/osgeo4w/osgeo4w-setup.exe

Linux
--------------------------------------------------------------------------------

How to install PROJ on Linux depends on which distribution you are using. Below
is a few examples for some of the more common Linux distributions:

Debian
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

On Debian and similar systems (e.g. Ubuntu) the APT package manager is used::

    sudo apt-get install proj-bin

Fedora
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

On Fedora the :program:`dnf` package manager is used::

    sudo dnf install proj

Red Hat
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

On Red Hat based system packages are installed with :program:`yum`::

    sudo yum install proj


Mac OS X
--------------------------------------------------------------------------------

On OS X PROJ can be installed via the Homebrew package manager::

    brew install proj

PROJ is also available from the MacPorts system::

    sudo ports install proj

Compilation and installation from source code
################################################################################

The classic way of installing PROJ is via the source code distribution. The
most recent version is available from the :ref:`download page<current_release>`.

The following guides show how to compile and install the software using CMake.

  .. note::

    Support for Autotools was maintained until PROJ 8.2 (see :ref:`RFC7`).
    PROJ 9.0 and later releases only support builds using CMake.


Build requirements
--------------------------------------------------------------------------------

- C99 compiler
- C++11 compiler
- CMake >= 3.9
- SQLite3 >= 3.11: headers and library for target architecture, and sqlite3 executable for build architecture.
- libtiff >= 4.0 (optional but recommended)
- curl >= 7.29.0 (optional but recommended)

Build steps
--------------------------------------------------------------------------------

With the CMake build system you can compile and install PROJ on more or less any
platform. After unpacking the source distribution archive step into the source-
tree::

    cd proj-{PROJVERSION}

Create a build directory and step into it::

    mkdir build
    cd build

From the build directory you can now configure CMake, build and install the binaries::

    cmake ..
    cmake --build .
    cmake --build . --target install

On Windows, one may need to specify generator::

    cmake -G "Visual Studio 15 2017" ..

If the SQLite3 dependency is installed in a custom location, specify the
paths to the include directory and the library::

    cmake -DSQLITE3_INCLUDE_DIR=/opt/SQLite/include -DSQLITE3_LIBRARY=/opt/SQLite/lib/libsqlite3.so ..

Alternatively, the custom prefix for SQLite3 can be specified::

    cmake -DCMAKE_PREFIX_PATH=/opt/SQLite ..


Tests are run with::

    ctest

With a successful install of PROJ we can now install data files using the
:program:`projsync` utility::

    projsync --system-directory

which will download all resource files currently available for PROJ. If less than
the entire collection of resource files is needed the call to :program:`projsync`
can be modified to suit the users needs. See :ref:`projsync` for more options.

.. note::

    The use of :program:`projsync` requires that network support is enabled (the
    default option). If the resource files are not installed using
    :program:`projsync` PROJ will attempt to fetch them automatically when a
    transformation needs a specific data file. This  requires that
    :envvar:`PROJ_NETWORK` is set to ``ON``.

    As an alternative on systems where network access is disabled, the
    :ref:`proj-data <datumgrid>`
    package can be downloaded and added to the :envvar:`PROJ_DATA` directory
    (called ``PROJ_LIB`` before PROJ 9.1)




CMake configure options
--------------------------------------------------------------------------------

Options to configure a CMake are provided using ``-D<var>=<value>``.
All cached entries can be viewed using ``cmake -LAH`` from a build directory.

.. option:: BUILD_APPS=ON

    Build PROJ applications. Default is ON. Control the default value for
    BUILD_CCT, BUILD_CS2CS, BUILD_GEOD, BUILD_GIE, BUILD_PROJ, BUILD_PROJINFO
    and BUILD_PROJSYNC.
    Note that changing its value after having configured once will not change
    the value of the individual BUILD_CCT, ... options.

    .. versionchanged:: 8.2

.. option:: BUILD_CCT=ON

    Build :ref:`cct`, default is the value of BUILD_APPS.

.. option:: BUILD_CS2CS=ON

    Build :ref:`cs2cs`,default is the value of BUILD_APPS.

.. option:: BUILD_GEOD=ON

    Build :ref:`geod`, default is the value of BUILD_APPS.

.. option:: BUILD_GIE=ON

    Build :ref:`gie`, default is the value of BUILD_APPS.

.. option:: BUILD_PROJ=ON

    Build :ref:`proj`, default is the value of BUILD_APPS.

.. option:: BUILD_PROJINFO=ON

    Build :ref:`projinfo`, default is the value of BUILD_APPS.

.. option:: BUILD_PROJSYNC=ON

    Build :ref:`projsync`, default is the value of BUILD_APPS.

.. option:: BUILD_SHARED_LIBS

    Build PROJ library shared. Default is ON. See also the CMake
    documentation for `BUILD_SHARED_LIBS
    <https://cmake.org/cmake/help/v3.9/variable/BUILD_SHARED_LIBS.html>`_.

    .. versionchanged:: 7.0
        Renamed from ``BUILD_LIBPROJ_SHARED``

    .. note:: before PROJ 9.0, the default was OFF for Windows builds.

.. option:: BUILD_TESTING=ON

    CTest option to build the testing tree, which also downloads and installs
    Googletest. Default is ON, but can be turned OFF if tests are not required.

    .. versionchanged:: 7.0
        Renamed from ``PROJ_TESTS``

.. option:: CMAKE_BUILD_TYPE

    Choose the type of build, options are: None (default), Debug, Release,
    RelWithDebInfo, or MinSizeRel. See also the CMake documentation for
    `CMAKE_BUILD_TYPE
    <https://cmake.org/cmake/help/v3.9/variable/CMAKE_BUILD_TYPE.html>`_.

    .. note::
        A default build is not optimized without specifying
        ``-DCMAKE_BUILD_TYPE=Release`` (or similar) during configuration,
        or by specifying ``--config Release`` with CMake
        multi-configuration build tools (see example below).

.. option:: CMAKE_C_COMPILER

    C compiler. Ignored for some generators, such as Visual Studio.

.. option:: CMAKE_C_FLAGS

    Flags used by the C compiler during all build types. This is
    initialized by the :envvar:`CFLAGS` environment variable.

.. option:: CMAKE_CXX_COMPILER

    C++ compiler. Ignored for some generators, such as Visual Studio.

.. option:: CMAKE_CXX_FLAGS

    Flags used by the C++ compiler during all build types. This is
    initialized by the :envvar:`CXXFLAGS` environment variable.

.. option:: CMAKE_INSTALL_PREFIX

    Default for Windows is based on the environment variable
    :envvar:`OSGEO4W_ROOT` (if set), otherwise is ``c:/OSGeo4W``.
    Default for Unix-like is ``/usr/local/``.

.. option:: ENABLE_IPO=OFF

    Build library using the compiler's `interprocedural optimization
    <https://en.wikipedia.org/wiki/Interprocedural_optimization>`_
    (IPO), if available, default OFF.

    .. versionchanged:: 7.0
        Renamed from ``ENABLE_LTO``.

.. option:: EXE_SQLITE3

    Path to an ``sqlite3`` or ``sqlite3.exe`` executable.

.. option:: SQLITE3_INCLUDE_DIR

    Path to an include directory with the ``sqlite3.h`` header file.

.. option:: SQLITE3_LIBRARY

    Path to a shared or static library file, such as ``sqlite3.dll``,
    ``libsqlite3.so``, ``sqlite3.lib`` or other name.

.. option:: ENABLE_CURL=ON

    Enable CURL support, default ON.

.. option:: CURL_INCLUDE_DIR

    Path to an include directory with the ``curl`` directory.

.. option:: CURL_LIBRARY

    Path to a shared or static library file, such as ``libcurl.dll``,
    ``libcurl.so``, ``libcurl.lib``, or other name.

.. option:: ENABLE_TIFF=ON

    Enable TIFF support to use PROJ-data resource files, default ON.

.. option:: TIFF_INCLUDE_DIR

    Path to an include directory with the ``tiff.h`` header file.

.. option:: TIFF_LIBRARY_RELEASE

    Path to a shared or static library file, such as ``tiff.dll``,
    ``libtiff.so``, ``tiff.lib``, or other name. A similar variable
    ``TIFF_LIBRARY_DEBUG`` can also be specified to a similar library for
    building Debug releases.

.. option:: USE_CCACHE=OFF

    Configure CMake to use `ccache <https://ccache.dev/>`_ (or
    `clcache <https://github.com/frerich/clcache>`_ for MSVC)
    to build C/C++ objects.


Building on Windows with vcpkg and Visual Studio 2017 or 2019
--------------------------------------------------------------------------------

This method is the preferred one to generate Debug and Release builds.

Install git
+++++++++++

Install `git <https://git-scm.com/download/win>`_

Install Vcpkg
+++++++++++++

Assuming there is a c:\\dev directory

::

    cd c:\dev
    git clone https://github.com/Microsoft/vcpkg.git

    cd vcpkg
    .\bootstrap-vcpkg.bat

Install PROJ dependencies
+++++++++++++++++++++++++

::

    vcpkg.exe install sqlite3[core,tool]:x86-windows tiff:x86-windows curl:x86-windows
    vcpkg.exe install sqlite3[core,tool]:x64-windows tiff:x64-windows curl:x64-windows

.. note:: The tiff and curl dependencies are only needed since PROJ 7.0

Checkout PROJ sources
+++++++++++++++++++++

::

    cd c:\dev
    git clone https://github.com/OSGeo/PROJ.git

Build PROJ
++++++++++

::

    cd c:\dev\PROJ
    mkdir build_vs2019
    cd build_vs2019
    cmake -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake ..
    cmake --build . --config Debug -j 8


Run PROJ tests
++++++++++++++

::

    cd c:\dev\PROJ\build_vs2019
    ctest -V --build-config Debug


Building on Windows with Conda dependencies and Visual Studio 2017 or 2019
--------------------------------------------------------------------------------

Variant of the above method but using Conda for SQLite3, TIFF and CURL dependencies.
It is less appropriate for Debug builds of PROJ than the method based on vcpkg.

Install git
+++++++++++

Install `git <https://git-scm.com/download/win>`_

Install miniconda
+++++++++++++++++

Install `miniconda <https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe>`_

Install PROJ dependencies
+++++++++++++++++++++++++

Start a Conda enabled console and assuming there is a c:\\dev directory

::

    cd c:\dev
    conda create --name proj
    conda activate proj
    conda install sqlite libtiff curl cmake

.. note:: The libtiff and curl dependencies are only needed since PROJ 7.0

Checkout PROJ sources
+++++++++++++++++++++

::

    cd c:\dev
    git clone https://github.com/OSGeo/PROJ.git

Build PROJ
++++++++++

From a Conda enabled console

::

    conda activate proj
    cd c:\dev\PROJ
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    cmake -S . -B _build.vs2019 -DCMAKE_LIBRARY_PATH:FILEPATH="%CONDA_PREFIX%/Library/lib" -DCMAKE_INCLUDE_PATH:FILEPATH="%CONDA_PREFIX%/Library/include"
    cmake --build _build.vs2019 --config Release -j 8

Run PROJ tests
++++++++++++++

::

    cd c:\dev\PROJ
    cd _build.vs2019
    ctest -V --build-config Release

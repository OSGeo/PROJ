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

Using ``conda`` you can also install the PROJ grid packages. Here's how to install
the `proj-datumgrid-europe` package::

    conda install -c conda-forge proj-datumgrid-europe

Available is also the ``proj-datumgrid-north-america``, ``proj-datumgrid-oceania``
and ``proj-datumgrid-world``.

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
    If you have already installed software via OSGeo4W on your computer it is
    likely that PROJ is already installed.

1. Download either the `32 bit`_ or `64 bit`_ installer.
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

   C:\temp\osgeo4w-setup-x86-64.exe -q -k -r -A -s https://download.osgeo.org/osgeo4w/ -a x86_64 -P proj

.. _`OSGeo4W`: https://trac.osgeo.org/osgeo4w/
.. _`32 bit`: https://download.osgeo.org/osgeo4w/osgeo4w-setup-x86.exe
.. _`64 bit`: https://download.osgeo.org/osgeo4w/osgeo4w-setup-x86_64.exe

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

On Fedora the dnf package manager is used::

    sudo dnf install proj

Red Hat
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

On Red Hat based system packages are installed with yum::

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

You will need that and at least the standard *proj-datumgrid* package for a
successful installation.

The following guides show how to compile and install the software using the
Autotools and CMake build systems.

Build requirements
--------------------------------------------------------------------------------

- C99 compiler
- C++11 compiler
- SQLite3 >= 3.11 (headers, library and executable)
- GNU make for autotools build or CMake >= 3.5

Autotools
--------------------------------------------------------------------------------

FSF's configuration procedure is used to ease installation of the PROJ system.

.. note::
    The Autotools build system is only available on UNIX-like systems.
    Follow the CMake installation guide if you are not using a UNIX-like
    operating system.

The default destination path prefix for installed files is ``/usr/local``.
Results from the installation script will be placed into subdirectories ``bin``,
``include``, ``lib``, ``man/man1`` and ``man/man3``. If this default path prefix
is proper, then execute::

    ./configure

If another path prefix is required, then execute::

    ./configure --prefix=/my/path

In either case, the directory of the prefix path must exist and be writable by
the installer.

If you are building from the git repository you have to first run::

    ./autogen.sh

which will generate a configure script that can be used as described above.

Before proceeding with the installation we need to add the datum grids. Unzip
the contents of the *proj-datumgrid* package into ``data/``::

    unzip proj-datumgrid-1.7.zip -d proj-5.0.1/data/

The installation will automatically move the grid files to the correct location.
Alternatively the grids can be installed manually in the directory pointed to
by the :envvar:`PROJ_LIB` environment variable. The default location is
``/usr/local/share/proj``.

With the grid files in place we can now build and install PROJ::

    make
    make install

The install target will create, if necessary, all required sub-directories.

Tests are run with::

    make check

The test suite requires that the proj-datumgrid package is installed in
:envvar:`PROJ_LIB`.


CMake
--------------------------------------------------------------------------------

With the CMake build system you can compile and install PROJ on more or less any
platform. After unpacking the source distribution archive step into the source-
tree::

    cd proj-5.0.1

Create a build directory and step into it::

    mkdir build
    cd build

From the build directory you can now configure CMake and build the binaries::

    cmake ..
    cmake --build .

On Windows, one may need to specify generator::

    cmake -G "Visual Studio 15 2017" ..

If the SQLite3 dependency is installed in a custom location, specify the
paths to the include directory and the library::

    cmake -DSQLITE3_INCLUDE_DIR=/opt/SQLite/include -DSQLITE3_LIBRARY=/opt/SQLite/lib/libsqlite3.so ..

Alternatively, the custom prefix for SQLite3 can be specified::

    cmake -DCMAKE_PREFIX_PATH=/opt/SQLite ..


Tests are run with::

    ctest

The test suite requires that the proj-datumgrid package is installed
in :envvar:`PROJ_LIB`.

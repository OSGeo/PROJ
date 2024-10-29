.. _rfc-8:

===================================================================
PROJ RFC-8: Embedding resource files into libproj
===================================================================

:Author: Even Rouault
:Contact: even.rouault @ spatialys.com
:Status: Adopted, implemented
:Implementation target: PROJ 9.6
:Last Updated: 2024-Oct-01

Summary
-------

This RFC provides an optional way of embedding :file:`proj.db` and
:file:`proj.ini` files directly into libproj, either using C23 ``#embed``
pre-processor directive when supported by compilers, or falling back to
a CMake-based script for older compilers.

Motivation
----------

Most common practical PROJ use critically depends on the availability of
:file:`proj.db`. Locating that resource files on the file system
can be painful in some use cases of PROJ, that involve relocating the PROJ
binary at installation time. One such case could be the PROJ embedded in Rasterio
or Fiona binary wheels where ``PROJ_DATA`` must be correctly set currently.
DuckDB Spatial also patches PROJ to embed :file:`proj.db` in its static build.
Web-assembly (WASM) use cases come also to mind as users of PROJ builds where
resources are directly included in libproj.
Given the existence of several out-of-tree patches to support embedding :file:`proj.db`
(such as https://github.com/OSGeo/PROJ/issues/2998#issuecomment-1004185741),
it makes sense to have a upstream-vetted solution the community can build around,
and potentially use as a point for further works.

Technical solution
------------------

C23 #embed
++++++++++

The C23 standard includes a `#embed "filename" <https://en.cppreference.com/w/c/preprocessor/embed>`__
pre-processor directive that ingests the specified filename and returns its
content as tokens that can be stored in a unsigned char or char array.

Getting the content of a file into a variable is as simple as the following:

.. code-block:: c

    static const unsigned char proj_db[] = {
    #embed "data/proj.db"
    };

Support for that directive is still very new. clang 19.1 is the
first compiler which has a release including it, and has an efficient
implementation of it, able to embed very large files with minimum RAM and CPU
usage.

The development version of GCC 15 also supports it, but in a non-optimized way
for now. i.e. trying to include large files, of several tens of megabytes could
cause significant compilation time, but without impact on runtime. There is
expressed intent from GCC developers to improve this in the future.

Embedding PROJ's :file:`proj.db` of size 9.1 MB with GCC 15dev at time of writing takes
18 seconds and 1.7 GB RAM, compared to 0.4 second and 400 MB RAM for clang 19,
which is still reasonable (Generating :file:`proj.db` itself from its source .sql files
takes one minute on the same system).

There is no timeline for Visual Studio C/C++ at time of writing (it has been
`requested by users <https://developercommunity.visualstudio.com/t/Add-support-for-embed-as-voted-into-the/10451640>`__)

To be noted that currently clang 19.1 only supports ``#embed`` in .c files, not
C++ ones (the C++ standard has not yet adopted this feature). So embedding
resources must be done in a .c file, which is obviously not a problem since
we can easily export symbols/functions from a .c file to be available by C++.

Fallback C99/C++11 mode
+++++++++++++++++++++++

For non C23 capable compilers, a CMake script,
derived from https://jonathanhamberg.com/post/cmake-file-embedding/ (which is MIT licensed)
is used that generates a .c and .h file per file to embed. The C file consists of
a ``const unsigned uint8_t content[] = { .... }`` array, which matches what a
non-optimization implementation of C23 #embed typically does.

This script has been improved because it performed very poorly on large files
such as :file:`proj.db`. Its execution time is now 8 seconds for :file:`proj.db`.

memvfs
++++++

Loading of the embedded :file:`proj.db` involves using the
`SQLite3 memvfs <https://www.sqlite.org/src/doc/tip/ext/misc/memvfs.c>`__,
as done by
`DuckDB Spatial <https://github.com/duckdb/duckdb_spatial/blob/9c14a8b4a9093d981123a7d9f620a675ab29c6d5/spatial/src/spatial/proj/module.cpp#L56>`__

New CMake options
-----------------

Resources will only be embedded if the new ``EMBED_RESOURCE_FILES`` CMake option
is set to ``ON``. This option will default to ``ON`` for static library builds
and if `C23 ``#embed`` is detected to be available. Users might also turn it to ON for
shared library builds. A CMake error is emitted if the option is turned on but
the compiler lacks support for it.

A complementary CMake option ``USE_ONLY_EMBEDDED_RESOURCE_FILES`` will also
be added. It will default to ``OFF``. When set to ON, PROJ will not try to
locate resource files in the PROJ_DATA directory burnt at build time into libproj
(``${install_prefix}/share/proj``), or by the ``PROJ_DATA`` configuration option.

Said otherwise, if ``EMBED_RESOURCE_FILES=ON`` but ``USE_ONLY_EMBEDDED_RESOURCE_FILES=OFF``,
PROJ will first try to locate resource files from the file system, and
fallback to the embedded version if not found.

The resource files will still be installed in ``${install_prefix}/share/proj``,
unless ``USE_ONLY_EMBEDDED_RESOURCE_FILES`` is set to ON.

Impacted code
-------------

- :file:`cmake/FileEmbed.cmake`: compatibility script for non-C23 mode to generate embedded resources
- :file:`data/CMakeLists.txt`: take into account USE_ONLY_EMBEDDED_RESOURCE_FILES to not install proj.db/proj.ini when it is ON
- :file:`docs/source/install.rst`: document EMBED_RESOURCE_FILES and USE_ONLY_EMBEDDED_RESOURCE_FILES
- :file:`src/embedded_resources.c` and .h: new files that use #embed or make a bridge to files generated by FileEmbed.cmake
- :file:`src/filemanager.cpp`: to take into account EMBED_RESOURCE_FILES for proj.ini
- :file:`src/iso19111/factory.cpp`: to take into account EMBED_RESOURCE_FILES for proj.db
- :file:`src/lib_proj.cmake`: takes into account EMBED_RESOURCE_FILES and USE_ONLY_EMBEDDED_RESOURCE_FILES in both C23 and non-C23 modes
- :file:`src/memvfs.c` and .h: code originating from https://www.sqlite.org/src/file/ext/misc/memvfs.c to handle a in-memory proj.db, with bug fixes, and adaptation for PROJ needs
- :file:`src/sqlite3_utils.cpp` and .hpp: interface layer of memvfs with src/iso19111/factory.cpp

Out of scope
------------

Embedding of resource files in PROJ is currently limited to :file:`proj.db` and
:file:`proj.ini`, as those are the ones which are expected to be the most needed
ones in typical embedded use cases. Extension to other resources (ITRFxxxx file)
could potentially be done as follow-up enhancements if the need arose, although
supporting dual C23/non-C23 mode for too many files could be a bit tedious.

The sky is the limit, so potentially grid files could also be embedded. That
would require developing a MemFile implementation in filemanager.cpp (in parallel
to the existing FileStdio, FileWin32 or NetworkFile).

Backward compatibility
----------------------

Fully backwards compatible with default settings.

Static builds will default to EMBED_RESOURCE_FILES=ON, but USE_ONLY_EMBEDDED_RESOURCE_FILES
will default to OFF. So an external :file:`proj.db` and :file:`proj.ini` found
by existing search mechanisms will still have precedence over the embedded files.

Even when EMBED_RESOURCE_FILES an/or USE_ONLY_EMBEDDED_RESOURCE_FILES is enabled,
the user can still use :cpp:func:`proj_context_set_database_path` to provide an
alternate database. Network based fetching of grids is also orthogonal to those
settings.

C23 is not required: it is just an opportunity for faster build time when available.

Documentation
-------------

The 2 new CMake variables will be documented.

Testing
-------

The existing fedora:rawhide continuous integration target, which has now clang
19.1 available, will be modified to test the effect of the new variables.

Local builds using GCC 15dev builds of https://jwakely.github.io/pkg-gcc-latest/
have also be successfully done during the development of the candidate implementation

Related issues and PRs
----------------------

- `GDAL RFC 102 text "Embedding resource files into libgdal" <https://github.com/OSGeo/gdal/pull/10913>`__

- `PROJ candidate implementation <https://github.com/OSGeo/PROJ/pull/4265>`__

- `ticket "RFC 8 (resource embedding) follow-up: update requirements to C23 when generally availability of it" <https://github.com/OSGeo/PROJ/issues/4273>`__

Voting history
--------------

+1 from PSC members KurtS, KristianE, JavierJS, ThomasK and EvenR


.. below is an allow-list for spelling checker.

.. spelling::

    Rasterio
    memvfs
    FileEmbed
    CMakeLists
    iso
    KurtS
    KristianE
    ThomasK
    JavierJS
    EvenR

.. _cmake:

********************************************************************************
Using PROJ in CMake projects
********************************************************************************

The recommended way to use the PROJ library in a CMake project is to
link to the imported library target ``PROJ::proj`` provided by
the CMake configuration which comes with the library. Typical usage is:

.. code::

    find_package(PROJ REQUIRED CONFIG)

    target_link_libraries(MyApp PRIVATE PROJ::proj)

By adding the imported library target ``PROJ::proj`` to the
target link libraries, CMake will also pass the include directories to
the compiler.

The CMake command ``find_package`` will look for the configuration in a
number of places. The lookup can be adjusted for all packages by setting
the cache variable or environment variable ``CMAKE_PREFIX_PATH``. In
particular, CMake will consult (and set) the cache variable
``PROJ_DIR``.

The old CMake name for the PROJ project was "PROJ4" and the switch to
the name "PROJ" was made with version 7.0.  As of PROJ 9.1, using
``find_package(PROJ4)`` will show a CMake Deprecation Warning. The
old project name PROJ4 name will be phased out after the PROJ 9.x series.

.. _cmake:

********************************************************************************
Using Proj.4 in CMake projects
********************************************************************************

Getting Started
---------------

The recommended way to use the Proj.4 library in a CMake project is to link to
the imported library target ``PROJ4::proj`` provided by the CMake configuration
which comes with the library. Typical usage is:

.. code::

    find_package(PROJ4)

    target_link_libraries(MyApp PROJ4::proj)


By adding the imported library target ``PROJ4::proj`` to the target link
libraries, CMake will also pass all usage requirements (definitions, include
directories etc.) to the given target.

The CMake command ``find_package`` will look for the configuration in a number
of places. The lookup can be adjusted for all packages by setting the cache
variable or environment variable ``CMAKE_PREFIX_PATH``. In particular, CMake
will consult (and set) the cache variable ``PROJ4_DIR``.


Deprecated Targets
------------------

The CMake configuration will also create an imported library target ``proj``.
Except for the name, this target is identical with ``PROJ4::proj``. However,
it should not be used in new projects. It is provided for compatibility with
older projects and will be removed in a future release.


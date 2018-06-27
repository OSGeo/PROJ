.. _news:

News
###############################################################################

PROJ 5.1.0
++++++++++++++++++++++++++++++++++++++++
*June 1st 2018*


UPDATES
-------

* Function :c:func:`proj_errno_string()` added to ``proj.h`` API (`#847 <https://github.com/OSGeo/proj.4/issues/847>`_)

* Validate units between pipeline steps and ensure transformation
  sanity (`#906 <https://github.com/OSGeo/proj.4/issues/906>`_)

* Print help when calling :program:`cct` and :program:`gie` without arguments (`#907 <https://github.com/OSGeo/proj.4/issues/907>`_)

* `CITATION` file added to source distribution (`#914 <https://github.com/OSGeo/proj.4/issues/914>`_)

* Webmercator operation added (`#925 <https://github.com/OSGeo/proj.4/issues/925>`_)

* Enhanced numerical precision of forward spherical Mercator near
  the Equator (`#928 <https://github.com/OSGeo/proj.4/issues/928>`_)

* Added ``--skip-lines`` option to :program:`cct` (`#923 <https://github.com/OSGeo/proj.4/issues/923>`_)

* Consistently return ``NaN`` values on ``NaN`` input (`#949 <https://github.com/OSGeo/proj.4/issues/949>`_)

* Removed unused ``src/org_proj4_Projections.h`` file (`#956 <https://github.com/OSGeo/proj.4/issues/956>`_)

* Java Native Interface bindings updated (`#957 <https://github.com/OSGeo/proj.4/issues/957>`_, `#969 <https://github.com/OSGeo/proj.4/issues/969>`_)

* Horizontal and vertical gridshift operations extended to
  the temporal domain (`#1015 <https://github.com/OSGeo/proj.4/issues/1015>`_)


BUG FIXES
------------

* Handle ``NaN`` float cast overflow in ``PJ_robin.c`` and ``nad_intr.c`` (`#887 <https://github.com/OSGeo/proj.4/issues/887>`_)

* Avoid overflow when Horner order is unreasonably large (`#893 <https://github.com/OSGeo/proj.4/issues/893>`_)

* Avoid unwanted NaN conversions in etmerc (`#899 <https://github.com/OSGeo/proj.4/issues/899>`_)

* Avoid memory failure in :program:`gie` when not specifying x,y,z in gie files (`#902 <https://github.com/OSGeo/proj.4/issues/902>`_)

* Avoid memory failure when :option:`+sweep` is initialized incorrectly in geos (`#908 <https://github.com/OSGeo/proj.4/issues/908>`_)

* Return ``HUGE_VAL`` on erroneous input in ortho (`#912 <https://github.com/OSGeo/proj.4/issues/912>`_)

* Handle commented lines correctly in cct (`#933 <https://github.com/OSGeo/proj.4/issues/933>`_)

* Avoid segmentation fault when transformation coordinates outside grid
  area in deformation (`#934 <https://github.com/OSGeo/proj.4/issues/934>`_)

* Avoid doing false easting/northing adjustments on cartesian
  coordinates (`#936 <https://github.com/OSGeo/proj.4/issues/936>`_)

* Thread-safe creation of proj mutex (`#954 <https://github.com/OSGeo/proj.4/issues/954>`_)

* Avoid errors when setting up geos with +lat_0!=0 (`#986 <https://github.com/OSGeo/proj.4/issues/986>`_)

* Reset errno when running :program:`proj` in verbose mode (`#988 <https://github.com/OSGeo/proj.4/issues/988>`_)

* Do not interpolate node values at nodata value in vertical
  grid shifts (`#1004 <https://github.com/OSGeo/proj.4/issues/1004>`_)

* Restrict Horner degrees to positive integer values to avoid
  memory allocation issues (`#1005 <https://github.com/OSGeo/proj.4/issues/1005>`_)



PROJ 5.0.1
++++++++++++++++++++++++++++++++++++++++
*March 1st 2018*

Bug fixes
***********

* Handle ellipsoid change correctly in pipelines when ``+towgs84=0,0,0`` is set (`#881 <https://github.com/OSGeo/proj.4/issues/881>`_)


* Handle the case where nad_ctable2_init returns NULL (`#883 <https://github.com/OSGeo/proj.4/issues/883>`_)


* Avoid shadowed declaration errors with old gcc (`#880 <https://github.com/OSGeo/proj.4/issues/880>`_)


* Expand ``+datum`` properly in pipelines (`#872 <https://github.com/OSGeo/proj.4/issues/872>`_)


* Fail gracefully when incorrect headers are encountered in grid files (`#875 <https://github.com/OSGeo/proj.4/issues/875>`_)


* Improve roundtrip stability in pipelines using ``+towgs84`` (`#871 <https://github.com/OSGeo/proj.4/issues/871>`_)


* Fixed typo in gie error codes (`#861 <https://github.com/OSGeo/proj.4/issues/861>`_)


* Numerical stability fixes to the geodesic package (`#826 <https://github.com/OSGeo/proj.4/issues/826>`_ & `#843 <https://github.com/OSGeo/proj.4/issues/843>`_)


* Make sure that transient errors are returned correctly (`#857 <https://github.com/OSGeo/proj.4/issues/857>`_)


* Make sure that locally installed header files are not used when building PROJ (`#849 <https://github.com/OSGeo/proj.4/issues/849>`_)


* Fix inconsistent parameter names in ``proj.h``/``proj_4D_api.c`` (`#842 <https://github.com/OSGeo/proj.4/issues/842>`_)


* Make sure ``+vunits`` is applied (`#833 <https://github.com/OSGeo/proj.4/issues/833>`_)


* Fix incorrect Web Mercator transformations (`#834 <https://github.com/OSGeo/proj.4/issues/834>`_)



PROJ 5.0.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*February 1st 2018*

This version of PROJ introduces some significant extensions and
improvements to (primarily) the geodetic functionality of the system.

The main driver for introducing the new features is the emergence of
dynamic reference frames, the increasing use of high accuracy GNSS,
and the related growing demand for accurate coordinate
transformations.  While older versions of PROJ included some geodetic
functionality, the new framework lays the foundation for turning PROJ
into a generic geospatial coordinate transformation engine.

The core of the library is still the well established projection code.
The new functionality is primarily exposed in a new programming
interface and a new command line utility, :ref:`cct<cct>`
(for "Coordinate Conversion and Transformation").  The old programming interface is
still available and can - to some extent - use the new geodetic
transformation features.

The internal architecture has also seen many changes and much
improvement.  So far, these improvements respect the existing
programming interface. But the process has revealed a need to simplify
and reduce the code base, in order to support sustained active
development.


**Therefore we have scheduled regular releases over the coming years**
**which will gradually remove the old programming interface.**

**This will cause breaking changes with the next two major version**
**releases, which will affect all projects that depend on PROJ**
**(cf. section "deprecations" below).**

The decision to break the existing API has not been easy, but has
ultimately been deemed necessary to ensure the long term survival of
the project. Not only by improving the maintainability immensely, but
also by extending the potential user (and hence developer) community.

The end goal is to deliver a generic coordinate transformation
software package with a clean and concise code base appealing to
both users and developers.


Versioning and naming
***************************************************************************

For the first time in more than 25 years the major version number of
the software is changed. The decision to do this is based on the many
new features and new API. While backwards compatibility remains -
except in a few rare corner cases - the addition of a new and improved
programming interface warrants a new major release.

The new major version number unfortunately leaves the project in a bit
of a conundrum regarding the name. For the majority of the life-time
of the product it has been known as PROJ.4, but since we have now
reached version 5 the name is no longer aligned with the version
number.

Hence we have decided to decouple the name from the version number and
from this version and onwards the product will simply be called PROJ.

In recognition of the history of the software we are keeping PROJ.4 as
the *name of the organizing project*. The same project team also
produces the datum-grid package.

In summary:

* The PROJ.4 project provides the product PROJ, which is now at
  version 5.0.0.

* The foundational component of PROJ is the library libproj.

* Other PROJ components include the application proj, which provides
  a command line interface to libproj.

* The PROJ.4 project also distributes the datum-grid package,
  which at the time of writing is at version 1.6.0.

Updates
***************************************************************************

* Introduced new API in ``proj.h``.

    - The new API is orthogonal to the existing ``proj_api.h`` API and the internally used ``projects.h`` API.

    - The new API adds the ability to transform spatiotemporal (4D) coordinates.

    - Functions in the new API use the ``proj_`` namespace.

    - Data types in the new API use the ``PJ_`` namespace.

*  Introduced the concept of "transformation pipelines" that makes possible to do complex geodetic transformations of coordinates by daisy chaining simple coordinate operations.

* Introduced :ref:`cct`, the Coordinate Conversion and Transformation application.

* Introduced :ref:`gie`, the Geospatial Integrity Investigation Environment.

  - Selftest invoked by ``-C`` flag in :ref:`proj` has been removed
  - Ported approx. 1300 built-in selftests to :ref:`gie` format
  - Ported approx. 1000 tests from the gigs test framework
  - Added approx. 200 new tests

* Adopted terminology from the OGC/ISO-19100 geospatial standards series. Key definitions are:

    - At the most generic level, a *coordinate operation* is a change of coordinates, based on a one-to-one relationship, from one coordinate reference system to another.

    - A *transformation* is a coordinate operation in which the two coordinate reference systems are based on different datums, e.g. a change from a global reference frame to a regional frame.

    - A *conversion* is a coordinate operation in which both coordinate reference systems are based on the same datum, e.g. change of units of coordinates.

    - A *projection* is a coordinate conversion from an ellipsoidal coordinate system to a plane. Although projections are simply conversions according to the standard, they are treated as separate entities in PROJ as they make up the vast majority of operations in the library.

* New operations

    - :ref:`The pipeline operator<pipeline>` (``pipeline``)

    - Transformations
        + :ref:`Helmert transform<helmert>` (``helmert``)
        + Horner real and complex polynomial evaluation (``horner``)
        + :ref:`Horizontal gridshift<hgridshift>` (``hgridshift``)
        + :ref:`Vertical gridshift<vgridshift>` (``vgridshift``)
        + :ref:`Molodensky transform<molodensky>` (``molodensky``)
        + :ref:`Kinematic gridshift with deformation model<deformation>` (``deformation``)

    - Conversions
        + :ref:`Unit conversion<unitconvert>` (``unitconvert``)
        + :ref:`Axis swap<axisswap>` (``axisswap``)

    - Projections
        + :ref:`Central Conic projection<ccon>` (``ccon``)

* Significant documentation updates, including

    - Overhaul of the structure of the documentation
    - A better introduction to the use of PROJ
    - :ref:`A complete reference to the new API<reference>`
    - a complete rewrite of the section on geodesic calculations
    - Figures for all projections

* New "free format" option for operation definitions, which permits separating tokens by whitespace when specifying key/value- pairs, e.g. ``proj = merc lat_0 = 45``.

* Added metadata to init-files that can be read with the :c:func:`proj_init_info` function in the new ``proj.h`` API.

* Added ITRF2000, ITRF2008 and ITRF2014 init-files with ITRF transformation parameters, including plate motion model parameters.

* Added ellipsoid parameters for GSK2011, PZ90 and "danish". The latter is similar to the already supported andrae ellipsoid, but has a slightly different semimajor axis.

* Added Copenhagen prime meridian.

* Updated EPSG database to version 9.2.0.

* Geodesic library updated to version 1.49.2-c.

* Support for analytical partial derivatives has been removed.

* Improved performance in Winkel Tripel and Aitoff.

* Introduced ``pj_has_inverse()`` function to ``proj_api.h``. Checks if an operation has an inverse. Use this instead of checking whether ``P->inv`` exists, since that can no longer be relied on.

* ABI version number updated to 13:0:0.

* Removed support for Windows CE.

* Removed the VB6 COM interface.

Bug fixes
***************************************************************************

* Fixed incorrect convergence calculation in Lambert Conformal Conic. (`#16 <https://github.com/OSGeo/proj.4/issues/16>`_)


* Handle ellipsoid parameters correctly when using ``+nadgrids=@null``. (`#22 <https://github.com/OSGeo/proj.4/issues/22>`_)


* Return correct latitude when using negative northings in Transverse Mercator. (`#138 <https://github.com/OSGeo/proj.4/issues/138>`_)


* Return correct result at origin in inverse Mod. Stereographic of Alaska. (`#161 <https://github.com/OSGeo/proj.4/issues/161>`_)


* Return correct result at origin in inverse Mod. Stereographic of 48 U.S. (`#162 <https://github.com/OSGeo/proj.4/issues/162>`_)


* Return correct result at origin in inverse Mod. Stereographic of 50 U.S. (`#163 <https://github.com/OSGeo/proj.4/issues/163>`_)


* Return correct result at origin in inverse Lee Oblated Stereographic. (`#164 <https://github.com/OSGeo/proj.4/issues/164>`_)


* Return correct result at origin in inverse Miller Oblated Stereographic. (`#165 <https://github.com/OSGeo/proj.4/issues/165>`_)


* Fixed scaling and wrap-around issues in Oblique Cylindrical Equal Area. (`#166 <https://github.com/OSGeo/proj.4/issues/166>`_)


* Corrected a coefficient error in inverse Transverse Mercator. (`#174 <https://github.com/OSGeo/proj.4/issues/174>`_)

* Respect ``-r`` flag when calling :program:`proj` with ``-V``. (`#184 <https://github.com/OSGeo/proj.4/issues/184>`_)

* Remove multiplication by 2 at the equator error in Stereographic projection. (`#194 <https://github.com/OSGeo/proj.4/issues/194>`_)

* Allow +alpha=0 and +gamma=0 when using Oblique Mercator. (`#195 <https://github.com/OSGeo/proj.4/issues/195>`_)

* Return correct result of inverse Oblique Mercator when alpha is between 90 and 270. (`#331 <https://github.com/OSGeo/proj.4/issues/331>`_)

* Avoid segmentation fault when accessing point outside grid. (`#396 <https://github.com/OSGeo/proj.4/issues/396>`_)

* Avoid segmentation fault on NaN input in Robin inverse. (`#463 <https://github.com/OSGeo/proj.4/issues/463>`_)

* Very verbose use of :program:`proj` (``-V``) on Windows is fixed. (`#484 <https://github.com/OSGeo/proj.4/issues/484>`_)

* Fixed memory leak in General Oblique Transformation. (`#497 <https://github.com/OSGeo/proj.4/issues/497>`_)

* Equations for meridian convergence and partial derivatives have
  been corrected for non-conformal projections. (`#526 <https://github.com/OSGeo/proj.4/issues/526>`_)

* Fixed scaling of cartesian coordinates in ``pj_transform()``. (`#726 <https://github.com/OSGeo/proj.4/issues/726>`_)

* Additional bug fixes courtesy of `Google's OSS-Fuzz program <https://bugs.chromium.org/p/oss-fuzz/issues/list?can=1&q=proj4>`_

.. _news:

News
###############################################################################

6.2.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*September 1st 2019*

Updates
-------

 * Introduced PROJJSON, a JSON encoding of WKT2 (`#1547 <https://github.com/OSGeo/PROJ/issues/15475>`_)
 * Support CRS instantiation of OGC URN's (`#1505 <https://github.com/OSGeo/PROJ/issues/1505>`_)

 * Expose scope and remarks of database objects (`#1537 <https://github.com/OSGeo/PROJ/issues/1537>`_)

 * EPSG Database updated to version 9.7.0 (`#1558 <https://github.com/OSGeo/PROJ/issues/1558>`_)

 * Added C API function proj_grid_get_info_from_database() (`#1494 <https://github.com/OSGeo/PROJ/issues/1494>`_)

 * Added C API function
   proj_operation_factory_context_set_discard_superseded() (`#1534 <https://github.com/OSGeo/PROJ/issues/1534>`_)

 * Added C API function proj_context_set_autoclose_database() (`#1566 <https://github.com/OSGeo/PROJ/issues/1566>`_)

 * Added C API function proj_create_crs_to_crs_from_pj() (`#1567 <https://github.com/OSGeo/PROJ/issues/1567>`_)

 * Added C API function proj_cleanup() (`#1569 <https://github.com/OSGeo/PROJ/issues/1569>`_)



Bug Fixes
---------

 * Fixed build failure on Solaris systems (`#1554 <https://github.com/OSGeo/PROJ/issues/1554>`_)


6.1.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*July 1st 2019*

Updates
-------

* Update EPSG registry to version 9.6.3 (`#1485 <https://github.com/OSGeo/PROJ/issues/1485>`_)

Bug Fixes
---------

* Take the passed authority into account when identifying
  objects (`#1466 <https://github.com/OSGeo/PROJ/issues/1466>`_)

* Avoid exception when transforming from NAD83 to projected
  CRS using NAD83(2011) (`#1477 <https://github.com/OSGeo/PROJ/issues/1477>`_)

* Avoid off-by-one reading of name argument if name of resource
  file has length 1 (`#11489 <https://github.com/OSGeo/PROJ/issues/1489>`_)

* Do not include :envvar:`PROJ_LIB` in ``proj_info().searchpath`` when context
  search path is set (`#1498 <https://github.com/OSGeo/PROJ/issues/1498>`_)

* Use correct delimeter for the current platform when parsing
  PROJ_LIB (`#1497 <https://github.com/OSGeo/PROJ/issues/1497>`_)

* Do not confuse 'ID74' CRS with WKT2 ID[] node (`#1506 <https://github.com/OSGeo/PROJ/issues/1506>`_)

* WKT1 importer: do case insensitive comparison for axis
  direction (`#1509 <https://github.com/OSGeo/PROJ/issues/1509>`_)

* Avoid compile errors on GCC 4.9.3 (`#1512 <https://github.com/OSGeo/PROJ/issues/1512>`_)

* Make sure that pipelines including ``+proj=ob_tran`` can be
  created (`#1526 <https://github.com/OSGeo/PROJ/issues/1526>`_)


6.1.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*May 15th 2019*

Updates
-------

* Include custom ellipsoid definitions from QGIS (`#1137 <https://github.com/OSGeo/proj.4/issues/1337>`_)

* Add ``-k ellipsoid`` option to projinfo (`#1338 <https://github.com/OSGeo/proj.4/issues/1338>`_)

* Make cs2cs support 4D coordinates (`#1355 <https://github.com/OSGeo/proj.4/issues/1355>`_)

* WKT2 parser: update to OGC 18-010r6 (`#1360 <https://github.com/OSGeo/proj.4/issues/1360>`_ `#1366 <https://github.com/OSGeo/proj.4/issues/1366>`_))

* Update internal version of googletest to v1.8.1 (`#1361 <https://github.com/OSGeo/proj.4/issues/1361>`_)

* Database update: EPSG v9.6.2 (`#1462 <https://github.com/OSGeo/proj.4/issues/1462>`_), IGNF v3.0.3, ESRI 10.7.0
  and add operation_version column (`#1368 <https://github.com/OSGeo/proj.4/issues/1368>`_)

* Add :c:func:`proj_normalize_for_visualization` that attempts to apply axis
  ordering as used by most GIS applications and PROJ <6 (`#1387 <https://github.com/OSGeo/proj.4/issues/1387>`_)

* Added noop operation (`#1391 <https://github.com/OSGeo/proj.4/issues/1391>`_)

* Paths set by user take priority over :envvar:`PROJ_LIB` for search paths (`#1398 <https://github.com/OSGeo/proj.4/issues/1398>`_)

* Reduced database size (`#1438 <https://github.com/OSGeo/proj.4/issues/1438>`_)

* add support for compoundCRS and concatenatedOperation named from
  their components (`#1441 <https://github.com/OSGeo/proj.4/issues/1441>`_)

Bug fixes
---------

* Have :program:`gie` return non-zero code when file can't be opened (`#1312 <https://github.com/OSGeo/proj.4/issues/1312>`_)

* CMake cross-compilation fix (`#1316 <https://github.com/OSGeo/proj.4/issues/1316>`_)

* Use 1st eccentricity instead of 2nd eccentricity in Molodensky (`#1324 <https://github.com/OSGeo/proj.4/issues/1324>`_)

* Make sure to include grids when doing Geocentric to CompoundCRS with
  nadgrids+geoidgrids transformations (`#1326 <https://github.com/OSGeo/proj.4/issues/1326>`_)

* Handle coordinates outside of bbox better (`#1333 <https://github.com/OSGeo/proj.4/issues/1333>`_)

* Enable system error messages in command line automatically in builds (`#1336 <https://github.com/OSGeo/proj.4/issues/1336>`_)

* Make sure to install projinfo man page with CMake (`#1347 <https://github.com/OSGeo/proj.4/issues/1347>`_)

* Add data dir to pkg-config file proj.pc (`#1348 <https://github.com/OSGeo/proj.4/issues/1348>`_)

* Fix GCC 9 warning about useless ``std::move()`` (`#1352 <https://github.com/OSGeo/proj.4/issues/1352>`_)

* Grid related fixes (`#1369 <https://github.com/OSGeo/proj.4/issues/1369>`_)

* Make sure that ISO19111 C++ code sets pj_errno on errors (`#1405 <https://github.com/OSGeo/proj.4/issues/1405>`_)

* vgridshift: handle longitude wrap-around for grids with 360deg
  longitude extent (`#1429 <https://github.com/OSGeo/proj.4/issues/1429>`_)

* :program:`proj`/:program:`cs2cs`: validate value of ``-f`` parameter to avoid potential crashes (`#1434 <https://github.com/OSGeo/proj.4/issues/1434>`_)

* Many division by zero and similar bug fixes found by OSS Fuzz.


6.0.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*March 1st 2019*


PROJ 6 has undergone extensive changes to increase its functional scope from a
cartographic projection engine with so-called "early-binding" geodetic datum
transformation capabilities to a more complete library supporting coordinate
transformations and coordinate reference systems.

As a foundation for other enhancements, PROJ now includes a C++ implementation
of the modelisation propopsed by the ISO-19111:2019 standard / OGC Abstract
Specification Topic 2: "Referencing By Coordinates", for geodetic reference
frames (datums), coordinate reference systems and coordinate operations.
Construction and query of those geodetic objects is available through a new C++
API, and also accessible for the most part from bindings in the C API.

Those geodetic objects can be imported and exported from and into the OGC
Well-Known Text format (WKT) in its different variants: ESRI WKT, GDAL WKT 1,
WKT2:2015 (ISO 19162:2015) and WKT2:2018 (ISO 19162:2018). Import and export of
CRS objects from and into PROJ strings is also supported. This functionality
was previously available in the GDAL software library (except WKT2 support
which is a new feature), and is now an integral part of PROJ.

A unified database of geodetic objects, coordinate reference systems and their
metadata, and coordinate operations between those CRS is now available in a
SQLite3 database file, proj.db. This includes definitions imported from the
IOGP EPSG dataset (v9.6.0 release), the IGNF (French national mapping agency)
geodetic registry and the ESRI projection engine database. PROJ is now the
reference software in the "OSGeo C stack" for this CRS and coordinate operation
database, whereas previously this functionality was spread over PROJ, GDAL and
libgeotiff, and used CSV or other adhoc text-based formats.

Late-binding coordinate operation capabilities, that takes  metadata such as
area of use and accuracy into account, has been added. This can avoid in a
number of situations the past requirement of using WGS84 as a pivot system,
which could cause unneeded accuracy loss, or was not doable at all sometimes
when transformation to WGS84 was not available. Those late-binding capabilities
are now used by the proj_create_crs_to_crs() function and the cs2cs utility.

A new command line utility, projinfo, has been added to query information about
a geodetic object of the database, import and export geodetic objects from/into
WKT and PROJ strings, and display coordinate operations available between two
CRSs.

UPDATES
-------

* Removed projects.h as a public interface (`#835 <https://github.com/OSGeo/proj.4/issues/835>`_)

* Deprecated the proj_api.h interface. The header file is still available
  but will be removed with the next major version release of PROJ. It is
  now required to define :c:macro:`ACCEPT_USE_OF_DEPRECATED_PROJ_API_H`
  before the interface can be used (`#836 <https://github.com/OSGeo/proj.4/issues/836>`_)

* Removed support for the nmake build system (`#838 <https://github.com/OSGeo/proj.4/issues/838>`_)

* Removed support for the ``proj_def.dat`` defaults file (`#201 <https://github.com/OSGeo/proj.4/issues/201>`_)

* C++11 required for building PROJ (`#1203 <https://github.com/OSGeo/proj.4/issues/1203>`_)

* Added build dependency on SQLite 3.7 (`#1175 <https://github.com/OSGeo/proj.4/issues/1175>`_)

* Added :program:`projinfo` command line application (`#1189 <https://github.com/OSGeo/proj.4/issues/1189>`_)

* Added many functions to ``proj.h`` for handling ISO19111 functionality (`#1175 <https://github.com/OSGeo/proj.4/issues/1175>`_)

* Added C++ API exposing ISO19111 functionality (`#1175 <https://github.com/OSGeo/proj.4/issues/1175>`_)

* Updated :program:`cs2cs` to use late-binding features (`#1182 <https://github.com/OSGeo/proj.4/issues/1182>`_)

* Removed the ``nad2bin`` application. Now available in the
  `proj-datumgrid <https://github.com/OSGeo/proj-datumgrid>`_
  git repository (`#1236 <https://github.com/OSGeo/proj.4/issues/1236>`_)

* Removed support for Chebyshev polynomials in :program:`proj`
  (`#1226 <https://github.com/OSGeo/proj.4/issues/1226>`_)

* Removed :c:func:`proj_geocentric_latitude` from `proj.h` API
  (`#1170 <https://github.com/OSGeo/proj.4/issues/1170>`_)

* Changed behaviour of :program:`proj`: Now only allow initialization of
  projections (`#1162 <https://github.com/OSGeo/proj.4/issues/1162>`_)

* Changed behaviour of :ref:`tmerc <tmerc>`: Now defaults to the Extended
  Transverse Mercator algorithm (``etmerc``). Old implementation available
  by adding ``+approx``
  (`#404 <https://github.com/OSGeo/proj.4/issues/404>`_)

* Chaged behaviour: Default ellipsoid now set to GRS80 (was WGS84) (`#1210 <https://github.com/OSGeo/proj.4/issues/1210>`_)

* Allow multiple directories in :envvar:`PROJ_LIB` environment variable (`#1281 <https://github.com/OSGeo/proj.4/issues/1281>`_)

* Added :ref:`Lambert Conic Conformal (2SP Michigan) <lcc>` projection (`#1142 <https://github.com/OSGeo/proj.4/issues/1142>`_)

* Added :ref:`Bertin1953 <bertin1953>` projection (`#1133 <https://github.com/OSGeo/proj.4/issues/1133>`_)

* Added :ref:`Tobler-Mercator <tobmerc>` projection (`#1153 <https://github.com/OSGeo/proj.4/issues/1153>`_)

* Added :ref:`Molodensky-Badekas <molobadekas>` transform (`#1160 <https://github.com/OSGeo/proj.4/issues/1160>`_)

* Added :ref:`push <push>` and :ref:`pop <pop>` coordinate operations (`#1250 <https://github.com/OSGeo/proj.4/issues/1250>`_)

* Removed ``+t_obs`` parameter from helmert and deformation (`#1264 <https://github.com/OSGeo/proj.4/issues/1264>`_)

* Added :option:`+dt` parameter to deformation as replacement for
  removed ``+t_obs`` (`#1264 <https://github.com/OSGeo/proj.4/issues/1264>`_)

BUG FIXES
---------

* Read :option:`+towgs84` values correctly on locales not using dot as comma separator (`#1136 <https://github.com/OSGeo/proj.4/issues/1136>`_)

* Fixed file offset for reading of shift values in NTv1 files (`#1144 <https://github.com/OSGeo/proj.4/issues/1144>`_)

* Avoid problems with :c:macro:`PTHREAD_MUTEX_RECURSIVE` when using CMake (`#1158 <https://github.com/OSGeo/proj.4/issues/1158>`_)

* Avoid raising errors when setting ellipsoid flattening to zero (`#1191 <https://github.com/OSGeo/proj.4/issues/1191>`_)

* Fixed lower square calculations in :ref:`rHealpix <rhealpix>` projection (`#1206 <https://github.com/OSGeo/proj.4/issues/1206>`_)

* Allow :ref:`Molodensky <molodensky>` transform parameters to be zero (`#1194 <https://github.com/OSGeo/proj.4/issues/1194>`_)

* Fixed wrong parameter in ``ITRF2000`` init file (`#1240 <https://github.com/OSGeo/proj.4/issues/1240>`_)

* Fixed use of grid paths including spaces (`#1152 <https://github.com/OSGeo/proj.4/issues/1152>`_)

* :ref:`Robinson <robin>`: fix wrong values for forward path for latitudes >= 87.5,
  and fix inaccurate inverse method (`#1172 <https://github.com/OSGeo/proj.4/issues/1172>`_)


PROJ 5.2.0
++++++++++++++++++++++++++++++++++++++++
*September 15th 2018*

UPDATES
-------

* Added support for deg, rad and grad in unitconvert (`#1054 <https://github.com/OSGeo/proj.4/issues/1054>`_)

* Assume :option:`+t_epoch` as time input when not otherwise specified (`#1065 <https://github.com/OSGeo/proj.4/issues/1065>`_)

* Added inverse Lagrange projection (`#1058 <https://github.com/OSGeo/proj.4/issues/1058>`_)

* Added :option:`+multiplier` option to vgridshift (`#1072 <https://github.com/OSGeo/proj.4/issues/1072>`_)

* Added Equal Earth projection (`#1085 <https://github.com/OSGeo/proj.4/issues/1085>`_)

* Added "require_grid" option to gie (`#1088 <https://github.com/OSGeo/proj.4/issues/1088>`_)

* Replace :option:`+transpose` option of Helmert transform with :option:`+convention`.
  From now on the convention used should be explicitly written. An
  error will be returned when using the +transpose option (`#1091 <https://github.com/OSGeo/proj.4/issues/1091>`_)

* Improved numerical precision of inverse spherical Mercator
  projection (`#1105 <https://github.com/OSGeo/proj.4/issues/1105>`_)

* :program:`cct` will now forward text after coordinate input to output
  stream (`#1111 <https://github.com/OSGeo/proj.4/issues/1111>`_)


BUG FIXES
------------

* Do not pivot over WGS84 when doing cs2cs-emulation with geocent (`#1026 <https://github.com/OSGeo/proj.4/issues/1026>`_)

* Do not scan past the end of the read data in :c:func:`pj_ctx_fgets` (`#1042 <https://github.com/OSGeo/proj.4/issues/1042>`_)

* Make sure :c:func:`proj_errno_string()` is available in DLL (`#1050 <https://github.com/OSGeo/proj.4/issues/1050>`_)

* Respect `+to_meter` setting when doing cs2cs-emulation (`#1053 <https://github.com/OSGeo/proj.4/issues/1053>`_)

* Fixed unit conversion factors for :program:`geod` (`#1075 <https://github.com/OSGeo/proj.4/issues/1075>`_)

* Fixed test failures related to GCC 8 (`#1084 <https://github.com/OSGeo/proj.4/issues/1084>`_)

* Improved handling of `+geoc` flag (`#1093 <https://github.com/OSGeo/proj.4/issues/1093>`_)

* Calculate correct projection factors for Webmercator (`#1095 <https://github.com/OSGeo/proj.4/issues/1095>`_)

* :program:`cs2cs` now always outputs degrees when transformed coordinates are
  in angular units (`#1112 <https://github.com/OSGeo/proj.4/issues/1112>`_)


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
---------

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
---------------------

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
-------

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
---------

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

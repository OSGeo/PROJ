# PROJ Release Notes

## 9.7.0

### Updates

* Implement 'exact' authalic latitude->geographic latitude and use it in `+proj=aea/cea/laea/eqearth/healpix/rhealpix` (#4441)

* Implement uniform conversions between auxiliary latitudes (#4446)

* WKT2 parser: recognize `DEFININGTRANSFORMATION` (but ignore it) (#4484)

* Database: add values from `alias_name` table that were removed in older EPSG releases, to improve
  recognizing old WKT strings (#4490)

* `proj_trans_bounds()`: sample points within the source grid to avoid missing extent such as world-wide
  coverage from EPSG:4326 to ESRI:54099 (Spilhaus) (#4513)

* Enable `-Wimplicit-fallthrough`, `-Wdeprecated-copy-dtor`, `-Wweak-vtables`,
  `-Wdocumentation-unknown-command` when possible (#4516)

* Geostationary projection: speed up inverse spherical and ellipsoidal computation (#4523)

* WKT importer: better error message when `BBOX` is invalid (e.g south_lat > north_lat) (#4525)

* Added `proj_geod_direct` for PJ trasformation object (#4532)

* Support exporting Mercator (Spherical) on a sphere as `Mercator_1SP` in WKT1 (#4555)

* Database: update to EPSG v12.022 (#4562)


### Bug Fixes

* `proj_trans_bounds()`: avoid erroring out, in some cases, when PJ* object is directly constructed
  from a PROJ pipeline (#4512)

* Fix issue related to `D_Unknown_based_on_WGS_84_ellipsoid` datum name (#4515)

* `proj_trans_bounds()`: fix 9.6.2 regression when going from long/lat crossing antimeridian to
  projected (#4526)

* opt_parse: avoid potential heap-buffer-overflow when parsing non-ASCII characters (affect
  `cct` and `gie`) (#4530)

* `gie`: fix memory leak on non existing input file (#4531)

* Database: referene `nl_nsgi_nllat2018.tif` and `nl_nsgi_bongeo2004.tif` grids (#4534)

* Fixes related to WKT import/export of `DerivedGeodetic/GeographicCRS` (#4536)

* Fix `tolower()`/`toupper()` implementation to not lead to undefined behavior (#4539)

* IAU2015 CRS: fix wrong code for coordinate system of North Polar and South Polar CRSs (#4545)

* `createOperations()`: fix a case involving 2 CompoundCRS, one with TOWGS84, and the 2
  verticalCRS differing by units (#4552)

* On SQLite query error, show first the error msg than the sql query (#4553)

* Allow geodesic inverse for meridional points on prolate ellipsoid (#4560)



## 9.6.2

### Updates

* Update EPSG to 12.013

### Bug Fixes

* Database: change ESRI deprecated records back to their deprecated state (#4519)

* ProjectedCRS::identify(): more robust identification of old ESRI names using _IntlFeet (#4518)

* Fix issue related to D_Unknown_based_on_WGS_84_ellipsoid datum name (#4514)

* proj_trans_bounds(): sample points within the source grid to avoid missing extent such as world-wide coverage from EPSG:4326 to ESRI:54099 (Spilhaus) (#4513)

* proj_trans_bounds(): avoid erroring out, in some cases, when PJ* object is directly constructed from a PROJ pipeline (#4512)

## 9.6.1

### Updates

* Update EPSG to 12.012 (#4498)

* Add an option to ship PDB files (#4505)

### Bug Fixes

* Deal with South Orientated TMerc presented as regular TMerc but with Scale_Factor=-1 (#4427)

* Add missing 'limits' and 'algorithm' includes (#4444)

* CMake: avoid detection of C23 `#embed` on too old compilers (#4463)

* `createOperations()`: for NAD83(CSRS)[x] to NAD83(CSRS)[y], do not go through NAD83 (#4467)

* Add missing include for `uint32_t` on Windows with gcc 15.1 (#4478)

* `getGeoidModels`: use vertical_crs.datum_code to get vcrss that uses different units (#4480)

* `FileManager::open_resource_file()`: honour known URLs from grid_alternatives that don't point to cdn.proj.org (#4481)

* Database: update ESRI to ArcGIS Pro 3.5 (#4493)

* `createOperationsCompoundToGeog()`: avoid bringing non-sensical transformations (#4494)

* Bug correction in Wagner VI projection parameters (#4453)

* C++: Fix bare `isnan()` to `std::isnan()` (#4472)

* Database: fix duplicated entries with auxiliary database (#4482)

* Use `std::abs` instead of `abs` for NetBSD (#4486)

* Fix Windows build when both `EMBED_RESOURCE_FILES` and `USE_ONLY_EMBEDDED_RESOURCE_FILES` are set (#4508)


## 9.6.0

### Updates

* `projinfo`: display whether an operation is time-dependent (#4255)

* Add CMake options `EMBED_RESOURCE_FILES` and `USE_ONLY_EMBEDDED_RESOURCE_FILES` for `proj.db` and `proj.ini` embedding (#4265, #4274)

* Database: ingest deprecated ESRI names to be able to better import old WKT ESRI (#4282)

* `proj_factors()`: enhance speed when called repeatedly on same compound or projected CRS (#4289)

* Add Airocean projection (formerly Dymaxion) (#4303)

* `createOperations()`: Optimization by avoiding database lookups for CRS (typically PROJ strings) using unknown datums (#4320)

* PROJJSON export: for a Projected CRS, add an explicit type=GeographicCRS/GeodeticCRS members to the base_crs member (#4331)

* `proj_trans_bounds()`: make it work when target CRS is a CompoundCRS (#4333)

* `createOperations()`: do Helmert transformation in 2D when one of source or target CRS is compound (#4337)

* Map EPSG method 'Vertical Offset by Grid Interpolation (asc)' to +proj=vgridshift (#4344)

* Add support for 'Coordinate Frame rotation full matrix (geocen)' and Coordinate Frame rotation full matrix (geog2D)' methods (#4348)

* Add a CMake `EMBED_RESOURCE_DIRECTORY` option to embed .tif/.json files in to libproj (#4349, #4373)

* Take into account new EPSG methods 'Cartesian Grid Offsets by TIN Interpolation (JSON)' and 'Vertical Offset by TIN Interpolation (JSON)' (#4353)

* Added an option `native_ca` to `proj.ini` and an environment variable `PROJ_NATIVE_CA` to be able to configure curl to use the operating system CA store. (#4356)

* Database: add an optional step_direction column to the concatenated_operation_step table (#4357)

* Database: replicate EGM2008 grid transformation record to WGS 84 realizations (#4363)

* Improve ETRFxxx to ETRFyyy, and WGS 84 (xxx) to WGS 84 (yyy) (#4364)

* Build: Update to C++17 version (#4366, #4369)

* Add bash completion script for projinfo (#4371)

* Dispatch content of `4D_api.cpp` into multiple .cpp files (#4393)

* Add `proj_trans_bounds_3D()` (#4394)

* Database: add a engineering_datum and engineering_crs tables; import few related EPSG records (#4396)

* NKG transformations: Add support for EUREF-FIN in Finish transformations (#4399)

* Add Spilhaus projection (#4401)

* Export ESRI:54099 `WGS_1984_Spilhaus_Ocean_Map_in_Square` as PROJ string (#4402)

* Database: update to EPSG v12.004 (#4413)

* `proj_download_file()`: invalidate in-memory caches related to downloaded file (for the current context) (#4414)


### Bug Fixes

* `createObjectsFromName()`: be more tolerant about N/S vs North/South, absence of zone or height (#4318)

* `createFromPROJString`: avoid repeated openings of proj.db and proj.ini and lookup of 'epsg' (#4334)

* Debugging/internals: append sub-grid index to grid name when opening a multi-grid TIFF file (#4381)

* `createOperations()`: use more appropriate operation when using a 'PROJ {grid_name}' geoid model, based on matching the vertical datum (#4379)

* `proj_clone()`: properly propagate `errorIfBestTransformationNotAvailable` and other flags from source object (#4382)

* Inverse +proj=cass: fix non-convergence on inputs where easting=false_easting or northing=false_northing (#4386)

* Fix typo in parameter for ITRF97 in ITRF2014 file (#4408)

* Retry on "SSL connection timeout" (#4419)


## 9.5.1

### Updates

* Database: update to EPSG v11.022 (#4254, #4275, #4297, #4312)

* Speed-up dramatically proj.db build time (#4280)

* Database: ingest deprecated ESRI names to be able to better import old WKT ESRI (#4282)

### Bug fixes

* `findsOpsInRegistryWithIntermediate()`: Restrict to using known source/target CRS that have the same originating authority (#4252)

* `isTimeDependent()`: Do not mark "static" Coordinate Frame helmert transformations as time-dependent, only time-dependent ones (#4259)

* Fix clang-19 "warning: empty paragraph passed to '@throw' command [-Wdocumentation]" (#4263)

* WKT importer: fix `nullptr` dereference on invalid `VERTCS[]` (#4266)

* Fixed `sql_filelist.cmake` relative path issue (#4296)

* Fail consistently on network error and grid transformations (#4302)

* Fix `-Wshadow` warnings between `parammappings.cpp` and `singleoperation.cpp` with unity builds (#4304)

* Removed leftover setting of `CMAKE_REQUIRED_FLAGS` (#4322)

* WKT1 ESRI export: fix wrong mapping of Lambert Cylindrical Equal Area to Behrmann (#4328)

* Numerous documentation fixes (#4248, #4267, #4291, #4293)

## 9.5.0

### Updates

* Database: add constraint for unicity of CRS and operation names (#4071)

* Make a few more functions that accept a `PJ*` to actually accept a `const PJ*` (#4074)

* Database: Increase consistency in trigger checks (#4080)

* Speed-up `+proj=cart +inv` (#4087)

* Added EPSG:9656 "Cartesian Grid Offsets" operation method, and imported related records (#4094)

* Add a `WKTParser::grammarErrorList()` method so that `proj_create_from_wkt()` can behave as documented (#4108)

* `CRS::stripVerticalComponent()`: Redirect it to `demoteTo2D(std::string(), nullptr)` (#4127)

* Add `proj_context_set_user_writable_directory()` to public API (#4144)

* CMake: for Windows builds, defaults PROJ DLL to be just `proj_${PROJ_MAJOR_VERSION}.dll` (#4167)

* Add `toWGS84AutocorrectWrongValues()` method and use it in PROJ.4 and WKT1 CRS import (#4172)

* WKT import: add support for Oracle WKT "Albers_Conical_Equal_Area" spelling (#4181)

* Limit number of C++ exceptions thrown & caught internally (#4183)

* Database: update to EPSG 11.016 (#4241)

* CMake: add option `EMBED_PROJ_DATA_PATH` (#4207)

* Add SEA Inverse projection (#4211)

* projinfo: Add 2 args equivalent to usage of `-s` and `-t` (#4218)

*  CMake: add TESTING_USE_NETWORK configure option (#4220)

* Add `PROJ_ERR_COORD_TRANSFM_MISSING_TIME` and `proj_coordoperation_requires_per_coordinate_input_time()` (#4221)

* Add new Conversion "Local Orthographic" (#4228, #4229)

* Add data/ITRF2020 with ITRF2020 to ITRFother, and ITRF2020 plate motion models (#4235)

### Bug Fixes

* Fix `imoll` and `imoll_o` zone calculations to correct inverse transformations near the "seams" (#4159)

* Fix wrong EPSG conversion code for UTM south (#4166)

* BoundCRS::exportToPROJ(): handle case of NADCON conus grid (#4168)

* `+proj=gridshift`: make projected grids work with `PROJ_NETWORK=ON` (#4174)

* `createOperations()`: make it work when transforming from/to a CompoundCRS with a DerivedVerticalCRS with ellipsoidal height (#4176)

* Support default context as `nullptr` in `CoordinateTransformer` (#4177)

* cct: emit error message and return error code when not being able to open input file (#4200)

* Fix race condition surrounding DB reopening during fork (#4231)

* `GeographicBoundingBox::create()`: accept degenerate bounding box reduced to a point or a line (#4237)

* `createOperation()`: tune so that ITRF2000->ETRS89 does not return only NKG grid based operations but also time-dependent Helmert (#4244)


## 9.4.1

### Updates

* Database: update to EPSG v11.006 (#4086)

* Database: update ESRI dataset to ArcGIS Pro 3.3 (#4143)

* Process `EPSG_CODE_METHOD_VERTICALGRID_GTG` properly with code 1129 (#4213)

### Bug fixes

 * DatabaseContext::lookForGridInfo(): do not trigger network activity in
   PROJ_GRID_AVAILABILITY_KNOWN_AVAILABLE mode when all grids are known (#4090)

 * Retry "Connection reset by peer" (#4107)

 * Use unique names for NKG transformations (#4111)

 * proj_create()/proj_create_from_database(): recall CRS AUTH:CODE in error message (#4114)

 * DerivedProjectedCRS: deal with lack of explicit CS in BASEPROJCRS in WKT (#4139)

 * proj_symbol_rename.h: update with current symbols (#4145)

 * Improve detection of compatible/incompatible celestial bodies (#4150)

## 9.4.0

### Updates

* Add support for transformations involving coordinate epoch changes
  (PointMotionOperation), specifically for Canadian NAD83(CSRS)(#3884)

* SSL_OPTIONS: set SSL options on Curl library (#3936)

* Add support for unity builds (#3962)

* Added ability to install *.tif if present in data (#3970)

* createOperationsCompoundToGeog(): tune selection logic
  when --grid-check known_available is specified (#3990)

* Increase CMake minimum version from 3.9 to 3.16 (#3997)

* CMake: use FetchContent to get googletest-1.12.1 (#4006)

* CMake: Replace custom FindSqlite3 with FindSQLite3 built-in (#4007)

* tpeqd: use numerically stable formula for computing the central angle
  from (phi_1, lam_1) to (phi_2, lam_2) (#4009)

* Move content of proj_experimental.h to proj.h (#4019)

* Add +proj=mod_krovak projection method for Modified Krovak that
  applies to S-JTSK/05 in the Czech Republic (#4021)

* PROJString formatter optimizer: simplify pipelines doing
  [Modified]Krovak (South West) <--> [Modified]Krovak (East North)
  by just doing an axis swap (#4034)

* +proj=gridshift: enhance to support grids referenced in projected CRS,
  and with easting_offset/northing_offset corrections (#4038)

* Tune concatenated operation instantiation, reference CR2005 geoid for
  Czechia and add (disabled by default) records for Czechia S-JTSK/05 based
  transformations (#4044)

* Database: update to EPSG v11.004 (#4062)

### Bug fixes

* Fix missing symbols at link time for Windows target in Visual Studio (#3984)

* Improve error message in axisswap (#3975)

* Avoid convergence errors in +proj=gridshift when using biquadratic
   interpolation (#3985)

## 9.3.1

### Updates

* Update to EPSG 10.098 (#3968)

* Update ESRI objects to v3.2.0 (#3944)

### Bug fixes

* ITRF2008: fix wrong sign for 'dry' parameter of EURA and EURA_T  (#3870)

* Fix build error with MSVC 2019 in /std:c++20 on NN_NO_CHECK() (#3872)

* ESRI WKT import: normalize GCS_unknown to unknown and D_unknown to unknown (#3874)

* CoordinateOperationFactory: deal with CompoundToCompound with a horizontal similarity
  transformation and a ballpark vertical (#3881)

* Ellipsoid::_isEquivalentTo(): fix so that an ellipsoid of semi-major axis A (and
  non-zero inv flattening) isn't equivalent to a sphere of radius A (#3882)

* isEquivalentTo(): make a datum name 'unknown' equivalent to another one (#3883)

* cs2cs: fix handling of input coordinates in grad (#3886)

* Make setargv.obj available on Universal Windows Platform (#3891)

* Allow opening proj.db with a URI (#3892)

* createOperations(): fix GeogCRS 3D with TOWGS84 to geocentric CRS (#3915)

* Fix test suite so that it can pass with ENABLE_TIFF=OFF (#3916)

* GeographicBoundingBox::intersects(): avoid infinite recursion and stack overflow
  on invalid bounding boxes (#3919)

* Fix importing '+proj=topocentric ... +type=crs' by using a geocentric CRS as
  the base CRS (#3924)

* Allow LOCAL_CS with 3 axes (#3928)

* WKT1 parser: in non-strict mode, accept missing UNIT[] in GEOGCS, GEOCCS,
  PROJCS and VERT_CS elements (#3933)

* createOperations(): fix issue with a obscure case involving CompoundCRS of
  unknown horizontal datum + boundCRS of vertical (#3934)

* createOperations(): fix bad PROJ pipeline when converting between Geog3D
  with non-metre height to CompoundCRS (#3943)

* createOperations(): Fix possible null dereference on invalid WKT input (#3946)

* proj_factor: fix when input is a compound CRS of a projected CRS (#3950)

* pj_get_suggested_operation(): tune it to give correct result for RGAF09 to
  RRAF 1991 / UTM zone 20N + Guadeloupe 1988 height transformation (#3954)

* Move static NameSpace::GLOBAL definition in static.cpp to avoid
 'static initialization fiasco' (#3956)

* horner: allow arbitrary input type of coordinate (#3961)

## 9.3.0

### Updates

* Add C++ API to transform coordinate (#3705)

* CMake: add PROJ_DB_CACHE_DIR (#3711)

* Implement EPSG:1026 Mercator (Spherical) method (#3741)

* CMake: remove useless cross-compiling related checks (#3747)

* Add mapping of Equidistant Conic to new EPSG:1119 method (#3812)

* Implement proposal001 from Planetary DWG from OGC (#3816)

* Add option in proj CLI to use a CRS (#3825)

* proj_factors(): make it work with projected CRS with non-metre unit and/or
  northing/easting axis order (#3826)

* PROJ_DEBUG: make ON an alias of 2, and OFF of 1 (#3835)

* Database: update to EPSG 10.094 (#3847)

### Bug fixes

* GeographicBoundingBox::intersection(): avoid infinite recursion and stack
  overflow on invalid bounding boxes (#3748)

* Various fixes related to concatenated operations (#3820)

* Projected CRS identification: fix crash when the base CRS is a
  non-geographic geodetic CRS (#3830)

* Avoid C++ exceptions to be thrown (and caught) when parsing strings
  like '+proj=longlat +datum=WGS84 +type=crs' (#3834)

* BUG: Handle prefix whitespace when guessing WKT dialiect (#3841)

* proj_alter_id(): make it replace an existing ID instead
  of appending a new one (#3846)

* bonne: fix inverse map projection computations when lat_1 < 0 (#3849)

* WKT1 ESRI import/export: fix GCS name for EPSG:8353
  S-JTSK_[JTSK03]_Krovak_East_North (#3851)

## 9.2.1

### Database updates

* Add alias for old ESRI datum/CRS names of EPSG:8353
  S_JTSK_JTSK03_Krovak_East_North (#3678)

* Pseudo Mercator now supported with IAU CRS's (#3645)

* Updated to EPSG 10.088 (#3731)

### Bug fixes

* Fixed build errors with Cygwin (#3642)

* PROJ string CRS parser: make sure that PROJ arguments of the rotated
  string are kept in the WKT representation (#3656)

* Fix slowness on proj_trans() when doing WGS 84 <--> NAD83 conversions (#3661)

* vgridshift/gridshift: accept hydroid_height as valid band description (#3675)

* WKT/PROJJSON: import/export accuracy of ConcatenatedOperation (#3676)

* Coordinate operation factory: count identified concatenated operations as a
  single step (#3677)

* EngineeringCRS: make proj_create_engineering_crs() set a datum name
  and relax isEquivalentTo() comparisons (#3685)

* PROJJSON: fix import/export of integer parameter value, and deal with
  interpolation CRS parameters in conversions (#3695)

* CMake: avoid imbalanced cmake_policy push/pop if TIFF or CURL dependency
   cannot be found (#3697)

* proj_create_crs_to_crs(): restore transformation selection behaviour
   of PROJ 9.1 when grids are missing (#3707)

* ESRI WKT: improve roundtrip of name and definition for UPS WGS84 CRS (#3720)

* CRS instantiation from PROJ.4 string: set 'Unknown based on XXXX ellipsoid'
  datum name when instantiating from known +a, +rf (#3727)

* cs2cs / proj_create_crs_to_crs(): fix regression with geocentric CRS (#3729)

* proj_trans(): set PROJ_ERR_COORD_TRANSFM_NO_OPERATION error when failing
   in ONLY_BEST=YES mode (#3730)

* tinshift: raise maximum size of JSON file to 100 MB (#3736)

## 9.2.0

### Updates

* Database

  * EPSG updated to EPSG 10.082 (#3609)

  * ESRI objects updated to v3.1.0 (#3575)

  * Added Slovenian geoid model SLO-VRP2016/Koper (#3610)

* Projections

  * Added generic Space Oblique Mercator (#3338)

  * Inverse eck4: Add checks for projection domain (#3625)

* Transformations

  * Added support for NADCON5 grids and transformation method (#3510)

  * Added option ONLY_BEST=YES (#3535)

    When enabled only use the best possible transformation between two CRS's can be
    instantiated. If the best possible transformations can't be instantiated it will
    fail instead of opting for the next best transformation.

    With ONLY_BEST=YES a transformation will fail if a needed grid is unavailable.

    The option can be toggled in proj.ini and with --only-best in cs2cs.

  * Implement 'Similarity transformation' (EPSG:9621) and import related
     transformations from EPSG (#3578)

  * Optimise transformations between GDA94, GDA2020 and WGS84 (#3621)

* Performance improvements

  * Pass PJ_COORD by reference for fwd4d and inv4d callbacks (#3503)

  * Improved of inverse meridian length calculations (#3516)

  * Avoid very poor performance on a geographic CRS whose datum name is the alias
    of an official one (#3624)

* Well-Known Text improvements

  * Support WKT:2019 COORDINATEMETADATA[] construct (#2711)

  * implement alterCSLinearUnit for CompoundCRS (#3325)

  * Implement alterCSLinearUnit for BoundCRS (#3327)

  * Added PJ_TYPE_DERIVED_PROJECTED_CRS (#3488)

  * Added support for ISO19111 CoordinateMetadata class (#3562)

  * Update WKT import to support WKT2 corrigendum (#3573)

  * WKT import: Make axis direction and range meaning enumeration case
    insensitive as mandated by spec (#3577)

  * Implement AffineCS and handle it in WKT (#3579)

  * WKT1 parser: Support inf as inverse flattening value (#3626)

* PROJJSON

  * Version number changed to 0.6

  * Added an additional optional source_crs property in abridged_transformation (#3454)

  * Added support for ISO19111 CoordinateMetadata class (#3562)

  * Implement AffineCS and handle it in PROJJSON (#3579)

* C/C++ API

  * C API: add proj_get_domain_count(), proj_get_area_of_use_ex() and proj_get_scope_ex() (#3588)

### Deprecations

The following functions have been deprecated and will be removed in a future version.

* C API function proj_rtodms. Use proj_rtodms2 instead (#3431)

* C experimental API function proj_create_conversion_tunisia_mapping_grid().
    Use proj_create_conversion_tunisia_mining_grid() instead (#3559)

* C++ API function createTunisiaMapingGrid(). Use createTunisiaMiningGrid() instead (#3559)

* Added explicit NaN handling to proj_trans and gie (#3603)

* CMake

  * Added uninstall target (#3362)

  * Adopted use of TIFF:TIFF and CURL::libcurl targets (#3374)

* Security improvements

  * Replaced uses of sprintf with the safer snprintf (#3421, #3431)

* Various other improvements

* Implemented platform independent mutex for better results on lesser used platforms (#3425)

* Improved name and remarks in descriptions of some vertical transformations (#3334)

* Implemented stricter matching by name to avoid false positives (#3487)

* Improved numerical accuracy of the meridian length calculations and its inverse (#3516)

* Added PROJ_IGNORE_CELESTIAL_BODY=YES environment variable that allows better matching of
   non-earth shaped ellipsoids to other celestial bodies (#3569)

### Bug fixes

* rtodms(): fix potential buffer overflow not dealt with (#3441)

* NetworkFile::open(): fix memory leak in unlikely error code path (#3443)

* When importing LOCAL_CS["foo"], generates a non-empty name for the datum (#3491)

* Avoid unit mix-up in CRS::alterCSLinearUnit for DerivedProjectedCRS (#3499)

* WKT to PROJ conversions: Map 'Lambert Azimuthal Equal Area (Spherical)' to '+proj=laea +R_A' (#3526)

* ProjectedCRS::identify(): Relax trust in id/authority in definition to identify (#3532)

* Fix instantion of `http://www.opengis.net/def/crs/IAU/2015/xxx` URNs (#3569)

* Fix proj_normalize_for_visualization() on a transformation with FORCE_OVER=YES (#3570)

* createBoundCRSToWGS84IfPossible(): Do not report +towgs84 for a CRS with multiple extents,
  such as EPSG:6316 (#3587)

* pj_get_suggested_operation(): handle longitudes outside of [-180,180] for coordinate
  operation selection (#3595)

* WKT1 parser: only emit warning when encountering invalid UNKNOWN WKT1 axis direction (#3618)

* projinfo: fix long option name --source-crs (#3601)

## 9.1.1

### Updates

* Database: register at_bev_AT_GIS_GRID_2021_09_28 grid (#3442)

### Bug fixes

* Implement alterGeodeticCRS and stripVerticalComponent for DerivedProjected (#3482)

* Various improvements to handling of DerivedProjectedCRS
  (#3482, #3477, #3342, #3319, #3317)

* Add missing `<cstdint>` includes (#3459)

* cs2cs: better validate value of -W option (#3453)

* DatabaseContext::lookForGridInfo(): fix caching of filenames and set
  correct URLs (#3448)

* Database: register in grid_alternatives grids from PROJ-data that have no
  corresponding transformation record (#3446)

* cass projection: fix forward computation of easting (#3433)

* Implement Geographic/Vertical Offset conversions (#3413)

* vandg projection: handle +over to extend the validity domain outside of
 |lon|>180deg (#3427)

* eqdc projection: avoid floating point division by zero in non-nominal case (#3415)

* createOperations(): fix issues when transforming between Geog3D and
  DerivedGeog3D CRS with Geographic3D offsets method (#3411)

* VerticalCRS::_isEquivalentTo(): do not consider VerticalCRS and
  DerivedVerticalCRS as equivalent (#3408)

* cct and cs2cs: Avoid problems when encountering UTF-8 BOM characters at
   beginning of input files (#3395)

* createFromUserInput(): Improved lookup when approximate name is provided (#3371)

* projinfo / cs2cs : auto promotion to 3D of CRS specified by name (#3367)

* findsOpsInRegistryWithIntermediate(): make it work when source/target geodetic
  CRS has no known id (#3365)

* createOperations(): emulate PROJ < 6 behavior when doing geocentric <-->
  geographic transformation between datum with unknown transformation (#3361)

* Fix issue when transforming from/into a WKT2 Bound VerticalCRS with a
  'Geographic3D to GravityRelatedHeight' method (#3355)

* proj_normalize_for_visualization(): take into account FORCE_OVER property
  from source operation (#3350)

* Link geodtest against libm when available (#3341)

## 9.1.0

### Updates

* Database

  * Database: update to EPSG v10.074 (#3273)

  * Update ESRI objects to version 3.0.0 (#3257)

  * Generate null transformation between geodetic/vertical datum ensemble and
    its members (#3266)

  * Add fk delete cascade to database tables for improved customizability
    of the database (#3237)

  * Add Svalbard geoid grid no_kv_arcgp-2006-sk to grid alternatives (#3292)

  * Added French grid fr_ign_RAF20 to grid alternatives (#3228)

* WKT & PROJJSON

  * WKT2 parser: allow unit-less PARAMETER for coordinate operations (#3271)

  * WKT and PROJJSON adjustments (#3234)

  * Bump PROJJSON to v0.5 (#3216)

  * Change order of horizontal and vertical operations when dealing with
    WKT1 / PROJ4 compound CRS (#3127)

* Coordinate Operations

  * Implement Vertical Offset and slope transformation method (#3200)

  * Add Interrupted Mollweide (land and ocean view) projections (#3139)

  * Add invertible Horner polynomials (#3133)

* Optimizations

  * NTv2 and GTX grid readers: add a cache of lines to speed-up (#3241)

  * IComparable::isEquivalentTo() optimizations (#3233)

* Various Updates

  * createOperations() / proj_trans(): better take into account area of
    interest name / do not only keep first returned coordinate operation (#3282)

  * createOperations(): take into account axis unit and inversion of target
    DerivedProjectedCRS (#3281)

  * Allow a Compound CRS with derived projected (#3283)

  * Make "Vertical Offset and Slope" method usable as a deriving conversion of
    a DerivedVerticalCRS (#3270)

  * PROJ pipeline generator: recognize opposite Helmert transformations using
    a different convention (#3265)

  * createOperations(): prefer simpler pipelines / affects WGS 84 to
    GDA94/GDA2020 (#3248)

  * Make it possible to determine which grid files were actually used during a
    transformation (#3180)

  * Transformation: no longer do vertical transformation when doing compound CRS
    to 2D CRS / add --3d to cs2cs (#3119)

  * Deprecate PROJ4 CMake name (#3094)

  * Use GNUInstallDirs for CMake install locations (#3100)

  * Introduce PROJ_DATA environment variable to deprecate PROJ_LIB (#3253)

### Bug fixes

* projinfo: fix crash on --list-crs when proj.db cannot be opened (#3290)

* getCRSInfoList(): fix retrieval of projected_crs given by text_definition
  only (#3289)

* WKT parser: fix issue when parsing some WKT1 with
  Hotine_Oblique_Mercator_Azimuth_Center and ignoring rectified_grid_angle (#3280)

* WKT1 import: better deal when the angular unit of the GEOGCS[] of the
  PROJCS[] doesn't match the one from the database (#3274)

* createOperations(): avoid potential infinite recursions (#3264)

* WKT ESRI: fix related to projected CRS export and add support for geographic
  3D CRS with LINUNIT node (#3256)

* proj_grid_info(): make it work again with remote grids (#3239)

* createOperations(): fix projected CRS with non-metre horiz&vertical unit and
  towgs84 to gencentric CRS (#3231)

* createOperations(): fix infinite recursion in a complex case (#3226)

## 9.0.1

### Database updates

* Update to EPSG 10.064 (#3208)

* Add OGC:CRS84h (WGS 84 longitude-latitude-height) (#3155)

### Bug fixes

* Use CMAKE_INSTALL_MANDIR to override default (#3081)

* Increase MAX_ITER so Mollweide forward projection works near the poles (#3082)

* Fix wrong results with SQLite 3.38.0 (#3091)

* Fix issue when transforming from/to BoundCRS of 3D CRS with non-Greenwich
 prime meridian, created from WKT (#3098)

* Fix issues with WKT of concatenated operations (#3105)

* CMake: fix installation of proj.pc on Windows (#3109)

* createOperations(): fix issue in transformation northing, easting
  projected CRS -> +proj=longlat +lon_wrap (#3110)

* unitconvert: round to nearest date when converting to yyyymmdd (#3111)

* Fix comparison of GeodeticRefrenceFrame vs DynamicGeodeticReferenceFrame (#3120)

* createOperations(): fix transformation involving CompoundCRS, ToWGS84
  and PROJ4_GRIDS (#3124)

* Fix datum names when importing from PROJ4 crs strings (affects some
  transformations using geoidgrids) (#3129)

* Deal with PARAMETER["EPSG code for Interpolation CRS",crs_code] (#3149)

* createOperations(): fix CompoundCRS[BoundCRS[ProjectedCRS],BoundCRS[VerticalCRS]]
  to Geog3DCrs (#3151)

* ITRF2014: fix ITRF2014:ITRF88,ITRF94 and ITRF96 definitions (#3159)

* createBoundCRSToWGS84IfPossible(): improve selection logic to generate +towgs84=
  taking into account extent (#3160)

* createOperations(): fix some complex cases involving TOWGS84 and CompoundCRS (#3163)

* Fix cmake CURL dependency, update appveyor - vcpkg interaction (#3185)

* WKT import: deal with Projected CRS that is a mix of WKT1:GDAL / WKT1:ESRI (#3189)

* createOperations(): fix/improve result of 'BD72 + Ostend height ' to 'WGS84+EGM96 height' (#3199)

* Identifier::isEquivalentName(): fix when ending by ' + ' which could indirectly cause
  an infinite stack call in master (#3202)

* WKT import: correctly deal with absence of Latitude_Of_Origin parameter in
  WKT1 ESRI with Stereographic projection (#3212)

* PROJJSON parser: do not error out if a datum ensemble member is unknown in the database (#3223)

## 9.0.0

### Breaking changes

* Support for the autotools build system has been removed (#3027)
  See RFC7 for details: <https://proj.org/community/rfc/rfc-7.html>

### Updates

* Database updates:

  * ESRI projection engine db to version 12.9 (#2943)

  * EPSG v10.054 (#3051)

  * Vertical grid files for PL-geoid-2011, Polish geoid model (#2960)

  * Belgian geoid model hBG18 to grid alternatives (#3044)

* Add new option to proj_create_crs_to_crs_from_pj method to force +over on
  transformation operations (#2914)

* Specify CMAKE_INSTALL_RPATH for macOS; use -rpath LDFLAGS for tests (#3009)

* Implement Geographic3D to Depth/Geog2D+Depth as used by ETRS89 to CD Norway
  depth (#3010)

* Allow PROJ_LIB paths wrapped with double quotes (#3031)

* Use external gtest by default when possible (#3035)

* CMake: make BUILD_SHARED_LIBS=ON the default even on Windows (#3042)

* proj.ini: add a 'ca_bundle_path' variable (#3049)

### Bug fixes

* Fix extremely long parsing time on hostile PROJ strings (#2968)

* CMake: fix warning with external googletest (#2980)

* proj_get_crs_info_list_from_database(): report PJ_TYPE_GEODETIC_CRS for
  IAU_2015 -ocentric geodetic CRS (#3013)

* peirce_q: rename +type parameter wrongly introduced in 8.2.1 to +shape (#3014)

* Set more precise error code for parsing errors in proj_create() (#3037)

* createOperations(): fix transformations from/to a BoundCRS of a
  DerivedGeographicCRS coming from WKT (#3046)

* Better deal with importing strings like '+init=epsg:XXXX +over' (#3055)

* Fix importing CRS definition with +proj=peirce_q and +shape different from
  square or diamond (#3057)

## 8.2.1

### Updates

* Database updated with EPSG v. 10.041 (#2974)

### Bug fixes

* BoundCRS WKT import: fix setting of name (#2917)

* PROJStringFormatter::toString(): avoid invalid iterator increment (#2932)

* Ensure CApi test are cross-platform (#2934)

* createOperations(): do not stop at the first operation in the PROJ namespace
  for vertical transformations (#2937)

* createOperationsCompoundToCompound(): fix null pointer dereference when
  connection to proj.db doesn't exist. (#2938)

* Fix windows.h conflict with Criterion::STRICT (#2950)

* Cache result of proj_get_type() to help for performance of
  proj_factors() (#2967)

* createOperations(): improvement for "NAD83(CSRS) + CGVD28 height" to
  "NAD83(CSRS) + CGVD2013(CGG2013) height" (#2977)

* WKT1 import: correctly deal with missing rectified_grid_angle
  parameter (#2986)

* Fix and additional options for Peirce Quincuncial projections (#2978)

* Fix build with Intel C++ compiler (#2995)

## 8.2.0

### Announcements

From PROJ 9.0.0 and onwards CMake will be the only build system bundled
with the PROJ package. As a consequence support for Autotools builds will
stop when the 8.2 branch of PROJ reaches end of life. We encourage
everyone to adjust their build workflows as soon as possible and report
any discrepancies discovered between Autotools and CMake builds.

Details about the build system unification can be found in PROJ RFC 7.

### Updates

* Added the S2 projection (#2749)

* Added support for Degree Sign on input (#2791)

* ESRI WKT: add support for import/export of (non interrupted)
  Goode Homolosine (#2827)

* Make filemanager aware of UWP Win32 API (#2831)

* Add proj_create_conversion_pole_rotation_netcdf_cf_convention() to
  address netCDF datasets using a pole rotation method (#2835)

* Emit better debug message when a grid isn't found (#2838)

* Add support for GeodeticCRS using a Spherical planetocentric
  coordinate system (#2847)

* PROJJSON: support additional properties allowed in id object (version,
  authority_citation, uri) for parity with WKT2:2019 (#2850)

* Database layout modified to include "anchor" field to geodetic_datum and
  vertical_datum tables, consequently database layout version is increased
  to 1.2 (#2859)

* proj_factors(): accept P to be a projected CRS (#2868)

* Add IAU_2015 CRS definitions (#2876)

* CRS::extractGeodeticCRS(): implement for DerivedProjectedCRS (#2877)

* Added proj_trans_bounds() (#2882)

* CMake: add a BUILD_APPS to be able to disable build of all applications (#2895)

* CMake: generate invproj/invgeod binaries (symlinks on Unix, copy otherwise)
  (#2897)

* CMake build: add generate_wkt1_parser and generate_wkt2_parser manual
  target, and logic to detect when they must be run (#2900)

* Add fallback strategy for tinshift transform to use closest triangle for
  points not in any (#2907)

* Database: update to EPSG v10.038 (#2910)

* CMake: revise handling of symbol export and static builds (#2912)

### Bug fixes

* Fix O(n^2) performance patterns where n is the number of steps of
  a pipeline (#2820)

* Detect ESRI WKT better in certain circumstances (#2823)

* Fix performance issue on pipeline instantiation of huge (broken)
  pipelines (#2824)

* Make sure to re-order projection parameters according to their canonical
  order if needed (#2842)

* Fix database access across fork() when SQLite3 doesn't use `pread[64]()` (#2845)

* Fix error in implementation of Inverse ellipsoidal orthographic projection
  that cause convergence to sometimes fail (#2853)

* Fix handling of edge-case coordinates in inverse ortho ellipsoidal
  oblique (#2855)

* proj_normalize_for_visualization(): set input and output units when there
  are several alternative transformations (#2867)

* CRS::identify(): fix ignoring CS order when identifying a geodetic CRS
  by a PROJ string with just the ellipsoid (#2881)

* Fix CRS Equality with PROJ parameter order (#2887)

* WKT concatenated operation parsing: fix when a axis order reversal conversion
  is the first or last operation (#2891)

* WKT1 parser: recognize Lambert_Conformal_Conic as projection name for
  LCC 1SP or 2SP (#2893)

* CMake: Always build gie if testing is requested (#2899)

* Geographic 3D CRS: allow to export to WKT1:ESRI if only the GEOGCS is known
  (and thus extrapolating a VERTCS) (#2902)

* lib_proj.cmake: add a PROJ::proj alias and add BUILD_INTERFACE include
  directories, so that proj can be used as a subdirectory of a larger
  project (#2913)

### Thanks to

* Thomas Knudsen
* Alan D. Snow
* Johannes Schauer Marin Rodrigues
* Howard Butler
* Geoff Evans
* Joris Van den Bossche
* marcus-elia
* Waheed Barghouthi
* snowman2
* Ben Boeckel
* Mike Taves
* Javier Jimenez Shaw
* Brendan Jurd
* Kristian Evers
* Even Rouault

## 8.1.1

### Updates

* EPSG Database updated to version 10.028 (#2773)

### Bug fixes

* Include algorithm header file to avoid build errors on Alpine Linux (#2769)

* CMake: fix installation of executables on iOS (#2766)

* Associate extents to transformations of CRS's that include GEOIDMODEL (#2769)

* Logging: avoid some overhead when logging is not enabled (#2775)

* ortho: remove useless and invalid log trace (#2777)

* CMake: remove external nlohmann_json from INTERFACE_LINK_LIBRARIES target (#2781)
* reateOperations(): fix SourceTargetCRSExtentUse::NONE mode (#2783)

* GeoTIFF grid reading: perf improvements (#2788)

* Conversion::createUTM(): avoid integer overflow (#2796)

* Inverse laea ellipsoidal: return PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN
  when appropriates (#2801)

* Make sure that proj_crs_promote_to_3D returns a derived CRS (#2806)

* createOperations(): fix missing deg<-->rad conversion when transforming with a
  CRS that has a fallback-to-PROJ4-string behaviour and is a BoundCRS of a
  GeographicCRS (#2808)

* WKT2 import/export: preserve PROJ.4 CRS extension string in REMARKS[] (#2812)

* BoundCRS: accept importing/exporting in WKT2 and PROJJSON the
  scope/area/extent/id attributes (#2815)

* ConcatenatedOperation::fixStepsDirection(): fix bad chaining of steps when
  inverse map projection is involved in non-final step (#2819)

### Thanks to

* Brendan Jurd
* Kristian Evers
* Even Rouault

## 8.1.0

### Updates

* Database

  * Update to EPSG v10.027 (#2751)

  * Decrease DB size by using WITHOUT ROWID tables (#2730) (#2647)

  * Add a ANALYZE step during proj.db creation allowing for
    faster lookups (#2729)

  * Added a PROJ.VERSION metadata entry (#2646)

  * Added NGO48 (EPSG:4273) to ETRS89 (EPSG:4258) triangulation-based
    transformation (#2554)

  * Additions to the norwegian NKG2020 transformation (#2548)

  * ESRI projection database updated to version 12.8 (#2717)

* API additions

  * Added proj_get_geoid_models_from_database() function that returns a list of
    geoid models available for a given CRS (#2681)

  * Added proj_get_celestial_body_list_from_database that returns a list
    of celestial bodies in the PROJ database (#2667)

  * Added proj_get_celestial_body_name() (#2662)

* Various improvements

  * proj_trans/cs2cs: If two operations have the same accuracy,
    use the one that is contained within a larger one (#2750)

  * Share SQLite database handle among all contexts (#2738)

  * Add proj/internal/mutex.hpp as compat layer for mingw32 for std::mutex (#2736)

  * projsync: make it filter out files not intended for the current version (#2725)

  * Improvements related to DerivedVerticalCRS using Change Unit and
    Height/Depth reversal methods (#2696)

  * Update internal nlohmann/json to 3.9.1, and add a CMake option to
    be able to use external nlohmann/json (#2686)

  * createFromUserInput(): change name of CRS built from URN combined references
    to match the convention of EPSG projected CRS (#2677)

  * Parse compound id with two authorities, like ESRI:103668+EPSG:5703 (#2669)

  * Added projinfo option --list-crs (supports --area) (#2663)

  * Added support for hyperbolic Cassini-Soldner (#2637)

  * Added capability to get SQL statements to add custom CRS in the database (#2577)

### Bug fixes

* Fix 'Please include winsock2.h before windows.h' warning with msys (#2692)

* Minor changes to address lint in geodesic.c (#2752)

* BoundCRS::identify(): avoid incompatible transformation for
  WKT1 / TOWGS84 export (#2747)

* proj_create(): do not open proj.db if string is a PROJ string,
  even if proj_context_set_autoclose_database() has been set (#2735)

* Fix export of transformation to PROJ string in a particular situation
  where CompoundCRS are involved (#2721)

### Thanks to

* Howard Butler
* Alan D. Snow
* Roel van den Berg
* Heidi Vanparys
* Sveinung Himle
* 積丹尼 Dan Jacobson
* Nyall Dawson
* Javier Jimenez Shaw
* Charles Karney
* Mike Taves
* Kristian Evers
* Even Rouault

## 8.0.1

### Updates

* Database: update to EPSG v10.018 (#2636)

* Add transformations for CHGeo2004, Swiss geoid model (#2604)

* Additions to the norwegian NKG2020 transformation (#2600)

### Bug fixes

* pj_vlog(): fix buffer overflow in case of super lengthy error message (#2693)

* Revert "proj_create_crs_to_crs_from_pj(): do not use PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION
  if area is specified" (#2679)

* UTM: error out when value of +zone= is not an integer (#2672)

* getCRSInfoList(): make result order deterministic (by increasing auth_name,
  code) (#2661)

* createOperation(): make sure no to discard deprecated operations if the
  replacement uses an unknown grid (#2623)

* Fix build on Solaris 11.4 (#2621)

* Add mapping of ESRI Equal_Area projection method to EPSG (#2612)

* Fix incorrect EPGS extent code for EPSG:7789>EPSG:4976 NKG transformation (#2599)

* fix wrong capitalization of CHENyx06_ETRS.gsb (#2597)

* createOperations(): improve handling of vertical transforms when
  when compound CRSs are used (#2592)

* CRS::promoteTo3D(): propagate the extent from the 2D CRS (#2589)

* createFromCRSCodesWithIntermediates(): improve performance when there is
  no match (#2583)

* Fix proj_clone() to work on 'meta' coordinate operation PJ* objects that
  can be returned by proj_create_crs_to_crs() (#2582)

* add PROJ_COMPUTE_VERSION, PROJ_VERSION_NUMBER,
  PROJ_AT_LEAST_VERSION macros (#2581)

* Make proj_lp_dist() and proj_geod() work on a PJ* CRS object (#2570)

* Fix gcc 11 -Wnonnull compilation warnings (#2559)

* Fix use of uninitialized memory in gie tests (#2558)

* createOperations(): fix incorrect height transformation between 3D promoted RGF93 and CH1903+ (#2555)

### Thanks to

* Dan Jacobson
* Sveinung Himle
* Mike Taves
* Javier Jimenez Shaw
* Kristian Evers
* Even Rouault

## 8.0.0

With the release of PROJ 8 the proj_api.h API is finally removed. See
<https://proj.org/development/migration.html> for more info on how to migrate
from the old to the proj.h API.

With the removal of proj_api.h it has been possible to simplify error codes
and messages given by the software. The error codes are exposed in the API.

Several improvements has been made to the command line utilities as well as
tweaks in the underlying API.

### Updates

* Public header file proj_api.h removed (#837)

* Improved accuracy of the Mercator projection (#2397)

* Copyright statement wording updated (#2417)

* Allow cct to instantiate operations via object codes or names (#2419)

* Allow @filename syntax in cct (#2420)

* Added geocentric->topocentric conversion (+proj=topocentric) (#2444)

* Update GeographicLib to version 1.51 (#2445)

* Added option to allow export of Geographic/Projected 3D CRS
  in WKT1_GDAL (#2450)

* Added --area and --bbox options in cs2cs to restrict candidate
  coordinate operations (#2466)

* Added build time option to make PROJ_LIB env var tested last (#2476)

* Added --authority switch in cs2cs to control where coordinate operations
  are looked for. C API function proj_create_crs_to_crs_from_pj() updated
  accordingly (#2477)

* Error codes revised and exposed in the public API (#2487)

* Added --accuracy options to projinfo. C API function
  proj_create_crs_to_crs_from_pj() updated accordingly (#2488)

* Added proj_crs_is_derived() function to C API (#2496)

* Enabled linking against static cURL on Windows (#2514)

* Updated ESRI CRS database to 12.7 (10.8.1/2.6) (#2519)

* Allow a WKT BoundCRS to use a PROJ string transformation (#2521)

* Update to EPSG v10.015 (#2539)

* Default log level set to PJ_LOG_ERROR (#2542)

* CMake installs a pkg-config file proj.pc, where supported (#2547)

### Bug fixes

* Do not restrict longitude to [-90;90] range in spherical transverse Mercator
  forward projection (#2471)

* createOperations(): fix Compound to Geog3D/Projected3D CRS with non-metre ellipsoidal height (#2500)

* Avoid error messages to be emitted log level is set to PJ_LOG_NONE (#2527)

* Close database connection when autoclose set to True (#2532)

### Thanks to

* Zac Miller
* Juan Hernando
* Thomas Knudsen
* Sveinung Himle
* Olli Raisa
* Nomit Rawat
* Modern Slave
* J.H. van de Water
* Guillaume Lostis
* Martin Steinisch
* Javier Jimenez Shaw
* Mateusz Łoskot
* Martijn Visser
* Alan D. Snow
* Mike Taves
* Nyall Dawson
* Charles Karney
* Kristian Evers
* Even Rouault

## 7.2.1

### Updates

* Add metadata with the version number of the database layout (#2474)

* Split coordinateoperation.cpp and test_operation.cpp in several parts (#2484)

* Update to EPSG v10.008 (#2490)

* Added the NKG 2008 and 2020 transformations in proj.db (#2495)

### Bug fixes

* Set CURL_ENABLED definition on projinfo build (#2405)

* createBoundCRSToWGS84IfPossible(): make it return same result with a CRS
  built from EPSG code or WKT1 (#2412)

* WKT2 parsing: several fixes related to map projection parameter units (#2428)

* createOperation(): make it work properly when one of the CRS is a BoundCRS of
  a DerivedGeographicCRS (+proj=ob_tran +o_proj=lonlat +towgs84=....) (#2441)

* WKT parsing: fix ingestion of WKT with a Geocentric CRS as the base of the
  projected CRS (#2443)

* GeographicCRS::_isEquivalentTo(EQUIVALENT_EXCEPT_AXIS_ORDER_GEOGCRS):
  make it work when comparing easting,northing,up and northing,easting,up (#2446)

* createOperation(): add a ballpark vertical transformation when dealing
  with GEOIDMODEL[] (#2449)

* Use same arguments to printf format string for both radians and degrees in
  output by cct (#2453)

* PRIMEM WKT handling: fixes on import for 'sexagesimal DMS' or from WKT1:GDAL/ESRI
  when GEOGCS UNIT != Degree; morph to ESRI the PRIMEM name on export (#2455)

* createObjectsFromName(): in exact match, make looking for 'ETRS89 / UTM zone 32N'
  return only the exact match (#2462)

* Inverse tmerc spherical: fix wrong sign of latitude when lat_0 is used (#2469)

* Add option to allow export of Geographic/Projected 3D CRS in WKT1_GDAL (#2470)

* Fix building proj.db with SQLite built with -DSQLITE_DQS=0 (#2480)

* Include JSON Schema files in CMake builds (#2485)

* createOperations(): fix inconsistent chaining exception when transforming from
  BoundCRS of projected CRS based on NTF Paris to BoundCRS of geog CRS NTF Paris (#2486)

### Thanks to

* Zac Miller
* Nomit Rawat
* Guillaume Lostis
* J.H. van de Water
* Kristian Evers
* Even Rouault

## 7.2.0

### Updates

* Command line tools:

  * Add multi-line PROJ string export capability, and use it by default in
    projinfo (unless --single-line is specified) (#2381)

* Coordinate operations:

  * +proj=col_urban projection, implementing a EPSG projection method
     used by a number of projected CRS in Colombia (#2395)

  * +proj=tinshift for triangulation-based transformations (#2344)

  * Added ellipsoidal formulation of +proj=ortho (#2361)

* Database

  * Update to EPSG 10.003 and make code base robust to dealing with
    WKT CRS with DatumEnsemble (#2370)

  * Added Finland tinshift operations (#2392)

  * Added transformation from JGD2011 Geographic 3D to JGD2011
    height using GSIGEO2011 (#2393)

  * Improve CompoundCRS identification and name morphing in VerticalCRS
    with ESRI WKT1 (#2386)

  * Added OGC:CRS27 and OGC:CRS83 CRS entries for NAD27 and NAD83
    in longitude, latitude order (#2350)

* API

  * Added temporal, engineering, and parametric datum
    PJ_TYPE enumerations (#2274)

  * Various improvements to context handling (#2329, #2331)

  * proj_create_vertical_crs_ex(): add a ACCURACY option to provide
    an explicit accuracy, or derive it from the grid name if it is
    known (#2342)

  * proj_crs_create_bound_crs_to_WGS84(): make it work on
    verticalCRS/compoundCRS such as EPSG:4326+5773 and
    EPSG:4326+3855 (#2365)

  * promoteTo3D(): add a remark with the original CRS identifier (#2369)

  * Added proj_context_clone (#2383)

### Bug fixes

* Avoid core dumps when copying contexts in certain scenarios (#2324)

* proj_trans(): reset errno before attemptying a retry with a new
  coordinate operation (#2353)

* PROJJSON schema corrected to allow prime meridians values with
  explicitly stating a unit (degrees assumed) (#2354)

* Adjust createBoundCRSToWGS84IfPossible() and operation filtering
  (for POSGAR 2007 to WGS84 issues) (#2357)

* createOperations(): several fixes affecting NAD83 -> NAD83(2011) (#2364)

* WKT2:2019 import/export: handle DATUM (at top level object) with PRIMEM

* WKT1_ESRI: fix import and export of CompoundCRS (#2389)

### Thanks to

* Alexander Saprykin
* Jeff McKenna
* Nyall Dawson
* Kai Pastor
* Juan Hernando
* Javier Jimenez Shaw
* Howard Butler
* Alan D. Snow
* Charles Karney
* Kristian Evers
* Even Rouault

## 7.1.1

### Updates

* Added various Brazilian grids to the database #2277

* Added geoid file for Canary Islands to the database #2312

* Updated EPSG database to version 9.8.15 #2310

### Bug fixes

* WKT parser: do not raise warning when parsing a WKT2:2015 TIMECRS
   whose TIMEUNIT is at the CS level, and not inside #2281

* Parse '+proj=something_not_latlong +vunits=' without +geoidgrids as a
   Projected3D CRS and not a compound CRS with a unknown datum #2289

* C API: Avoid crashing due to missing SANITIZE_CTX() in entry points #2293

* CMake build: Check "target_clones" before use #2297

* PROJ string export of +proj=krovak +czech: make sure we export +czech… #2301

* Helmert 2D: do not require a useless +convention= parameter #2305

* Fix a few spelling errors ("vgridshit" vs. "vgridshift") #2307

* Fix ability to identify EPSG:2154 as a candidate for 'RGF93_Lambert_93' #2316

* WKT importer: tune for Oracle WKT and 'Lambert Conformal Conic' #2322

* Revert compiler generated Fused Multiply Addition optimized routines #2328

### Thanks to

* Jeff McKenna
* Kai Pastor
* Javier Jimenez Shaw
* Kristian Evers
* Even Rouault

## 7.1.0

### Updates

* New transformations:

  * Add a +proj=defmodel transformation for multi-component time-based deformation models (#2206)

* New projections:

  * Add square conformal projections from libproject (#2148):
    * Adams Hemisphere in a Square
    * Adams World in a Square I
    * Adams World in a Square II
    * Guyou
    * Pierce Quincuncial

  * Adams Square II: map ESRI WKT to PROJ string, and implement iterative
     inverse method (#2157)

  * Added IGH Oceanic View projection (#2226)

  * Add wink2 inverse by generic inversion of forward method (#2243)

* Database:

  * Update to EPSG 9.8.12, ESRI 10.8.1 and import scope and remarks for
     conversion (#2238) (#2267)

  * Map the Behrmann projection to cae when converting ESRI CRSes (#1986)

  * Support conversion of Flat_Polar_Quartic projection method (#1987)

  * Register 4 new Austrian height grids (see <https://github.com/OSGeo/PROJ-data/pull/13>)
     and handle 'Vertical Offset by Grid Interpolation (BEV AT)' method (#1989)

  * Add ESRI projection method mappings for Mercator_Variant_A, Mercator_Variant_B
     and Transverse_Cylindrical_Equal_Area and various grid mappings (#2020) (#2195)

  * Map ESRI Transverse_Mercator_Complex to Transverse Mercator (#2040)

  * Register grids for New Caledonia (see <https://github.com/OSGeo/PROJ-data/pull/16>) (#2051) (#2239)

  * Register NZGD2000 -> ITRF96 transformation for NZGD2000 database (#2248)

  * Register geoid file for UK added
     (see <https://github.com/OSGeo//PROJ-data/pull/25>) (#2250)

  * Register Slovakian geoid transformations with needed code changes (#2259)

  * Register Spanish SPED2ETV2 grid for ED50->ETRS89 (#2261)

* API:

  * Add API function proj_get_units_from_database() (#2065)

  * Add API function proj_get_suggested_operation() (#2068)

  * Add API functions proj_degree_input() and proj_degree_output() (#2144)

  * Moved proj_context_get_url_endpoint & proj_context_get_user_writable_directory
     from proj_experimental.h to proj.h (#2162)

  * createFromUserInput(): allow compound CRS with the 2 parts given by names,
     e.g. 'WGS 84 + EGM96 height' (#2126)

  * createOperations(): when converting CompoundCRS<-->Geographic3DCrs, do not
     use discard change of ellipsoidal height if a Helmert transformation is
     involved (#2227)

  * proj_list_units() deprecated, superseded by proj_get_units_from_database()

  * proj_list_angular_units() deprecated, superseded by proj_get_units_from_database()

* Optimizations:

  * tmerc/utm: add a +algo=auto/evenden_snyder/poder_engsager parameter (#2030)

  * Extended tmerc (Poder/Engsager): speed optimizations (#2036)

  * Approximate tmerc (Snyder): speed optimizations (#2039)

  * pj_phi2(): speed-up computation (and thus inverse ellipsoidal Mercator and LCC) (#2052)

  * Inverse cart: speed-up computation by 33% (#2145)

  * Extended tmerc: speed-up forward path by ~5% (#2147)

* Various:

  * Follow PDAL's CMake RPATH strategy (#2009)

  * WKT import/export: add support for WKT1_ESRI VERTCS synta (#2024)

  * projinfo: add a --hide-ballpark option (#2127)

  * gie: implement a strict mode with `<gie-strict> </gie-strict>` (#2168)

  * Allow importing WKT1 COMPD_CS with a VERT_DATUM[Ellipsoid,2002] (#2229)

  * Add runtime checking that sqlite3 is >= 3.11 (#2235)

### Bug fixes

* createOperations(): do not remove ballpark transformation if there are only grid
  based operations, even if they cover the whole area of use (#2155)

* createFromProjString(): handle default parameters of '+krovak +type=crs', and
  handle +czech correctly (#2200)

* ProjectedCRS::identify(): fix identification of EPSG:3059 (#2215)

* Database: add a 'WGS84' alias for the EPSG:4326 CRS (#2218)

* Fixes related to CompoundCRS and BoundCRS (#2222)

* Avoid 2 warnings about missing database indices (#2223)

* Make projinfo --3d --boundcrs-to-wgs84 work better (#2224)

* Many fixes regarding BoundCRS, CompoundCRS, Geographic3D CRS with
 non-metre units (#2234)

* Fix identification of (one of the) ESRI WKT formulations of EPSG:3035 (#2240)

* Avoid using deprecated and removed Windows API function with Mingw32 (#2246)

* normalizeForVisualization(): make it switch axis for EPSG:5482
  (RSRGD2000 / RSPS2000) (#2256)

* Fix access violation in proj_context_get_database_metadata (#2260)

### Thanks to

* Martin Raspaud
* Jeroen Ooms
* Jeff McKenna
* Colin Doig
* Chris Mayo
* Chatziargyriou Eleftheria
* Bas Couwenberg
* B R S Recht
* 積丹尼 Dan Jacobson
* Alan D. Snow
* GitHub user @chrodger
* Pedro Venancio
* Olli Räisä
* John Krasting
* Andrei Marshalov
* Javier Jimenez Shaw
* Martin Dobias
* Howard Butler
* Nyall Dawson
* Mike Taves
* Kristian Evers
* Even Rouault

## 7.0.1

### Updates

* Database: update to EPSG v9.8.9 #2141

### Bug fixes

* Make tests independent of proj-datumgrid (#1995)

* Add missing projection property tables (#1996)

* Avoid crash when running against SQLite3 binary built with
  -DSQLITE_OMIT_AUTOINIT (#1999)

* createOperations(): fix wrong pipeline generation with CRS that has +nadgrids=
  and +pm= (#2002)

* Fix bad copy&replace pattern on HEALPix and rHEALPix projection names (#2007)

* createUnitOfMeasure(): use full double resolution for the conversion
  factor (#2014)

* Update README with info on PROJ-data (#2015)

* utm/ups: make sure to set errno to PJD_ERR_ELLIPSOID_USE_REQUIRED if
  es==0 (#2045)

* data/Makefile.am: remove bashism (#2048)

* ProjectedCRS::identify(): tune it to better work with ESRI WKT
  representation of EPSG:2193 (#2059)

* Fix build with gcc 4.8.5 (#2066)

* Autotools/pkg-conf: Define datarootdir (#2069)

* cs2cs: don't require +to for '{source_crs} {target_crs} filename...'
  syntax (#2081)

* CMake: fix bug with find_package(PROJ) with macOS (#2082)

* ESRI WKT import / identification: special case for
  NAD_1983_HARN_StatePlane_Colorado_North_FIPS_0501 with Foot_US unit (#2088)

* ESRI WKT import / identification: special case for
  NAD_1983_HARN_StatePlane_Colorado_North_FIPS_0501 with Foot_US unit (#2089)

* EngineeringCRS: when exporting to WKT1_GDAL, output unit and axis (#2092)

* Use jtsk03-jtsk horizontal grid from CDN (#2098)

* CMake: prefer to use use PROJ_SOURCE_DIR and PROJ_BINARY_DIR (#2100)

* Fix wrong grids file name in esri.sql (#2104)

* Fix identification of projected CRS whose name is close but not strictly
  equal to a ESRI alias (#2106)

* Fix working of Helmert transform between the horizontal part of
  2 compoundCRS (#2111)

* Database: fix registration of custom entries of grid_transformation_custom.sql
  for geoid grids (#2114)

* ESRI_WKT ingestion: make sure to identify to non-deprecated EPSG entry when
  possible (#2119)

* Make sure that importing a Projected 3D CRS from WKT:2019 keeps the base
  geographic CRS as 3D (#2125)

* createOperations(): improve results of compoundCRS to compoundCRS case (#2131)

* hgridshift/vgridshift: defer grid opening when grid has already
  been opened (#2132)

* Resolve a few shadowed declaration warnings (#2142)

* ProjectedCRS identification: deal with switched 1st/2nd std parallels for
  LCC_2SP(#2153)

* Fix Robinson inverse projection (#2154)

* createOperations(): do not remove ballpark transformation if there are only
  grid based operations, even if they cover the whole area of use (#2156)

* createFromCoordinateReferenceSystemCodes(): 'optimization' to avoid using
  C++ exceptions (#2161)

* Ingestion of WKT1_GDAL: correctly map 'Cylindrical_Equal_Area' (#2167)

* Add limited support for non-conformant WKT1 LAS COMPD_CS[] (#2172)

* PROJ4 string import: take into correctly non-metre unit when the string
  looks like the one for WGS 84 / Pseudo Mercator (#2177)

* io.hpp: avoid dependency to proj_json_streaming_writer.hpp (#2184)

* Fix support of WKT1_GDAL with netCDF rotated pole formulation (#2186)

### Thanks to

* Mike Taves
* Chris Mayo
* Kristian Evers
* Even Rouault

## 7.0.0

The major feature in PROJ 7 is significantly improved handling of gridded
models. This was implemented in RFC4 (<https://proj.org/community/rfc/rfc-4.html>).
The main features of the RFC4 work is that PROJ now implements a new grid format,
Geodetic TIFF grids, for exchanging gridded transformation models. In addition
to the new grid format, PROJ can now also access grids online using a data
store in the cloud.

The grids that was previously available via the proj-datumgrid packages are now
available in two places:

1. As a single combined data archive including all available resource files
2. From the cloud via <https://cdn.proj.org>

In Addition, provided with PROJ is a utility called projsync that can be used
download grids from the data store in the cloud.

The use of the new grid format and the data from the cloud requires that
PROJ is build against libtiff and libcurl. Both are optional dependencies
to PROJ but it is highly encouraged that the software is build against both.

ATTENTION: PROJ 7 will be last major release version that includes the proj_api.h
header. The functionality in proj_api.h is deprecated and only supported in
maintenance mode. It is inferior to the functionality provided by functions
in the proj.h header and all projects still relying on proj_api.h are encouraged
to migrate to the new API in proj.h. See <https://proj.org/development/migration.html>
for more info on how to migrate from the old to the new API.

### Updates

* Added new file access API to proj.h #866

* Updated the name of the most recent version of the WKT2 standard from
  WKT2_2018 to WKT2_2019 to reflect the proper name of the standard (#1585)

* Improvements in transformations from/to WGS 84 (Gxxxx) realizations and
  vertical <--> geog transformations #1608

* Update to version 1.50 of the geodesic library (#1629)

* Promote proj_assign_context to proj.h from proj_experimental.h (#1630)

* Add rotation support to the HEALPix projection (#1638)

* Add c function proj_crs_create_bound_vertical_crs() (#1689)

* Use Win32 Unicode APIs and expect all strings to be UTF-8 (#1765)

* Improved name aliases lookup (#1827)

* CMake: Employ better use of CTest with the BUILD_TESTING option (#1870)

* Grid correction: fix handling grids spanning antimeridian (#1882)

* Remove legacy CMake target name "proj" #1883

* projinfo: add --searchpaths switch (#1892)

* Add +proj=set operation to set component(s) of a coordinate to a fixed
  value (#1896)

* Add EPSG records for 'Geocentric translation by Grid Interpolation (IGN)'
  (gr3df97a.txt) and map them to new +proj=xyzgridshift (#1897)

* Remove 'null' grid file as it is now a special hardcoded case in grid
  code (#1898)

* Add projsync utility (#1903)

* Make PROJ the CMake project name #1910

* Use relative directory to locate PROJ resource files (#1921)

### Bug fixes

* Horizontal grid shift: fix failures on points slightly outside a
  subgrid (#209)

* Fix ASAN issue with SQLite3VFS class (#1902)

* tests: force use of bash for proj_add_test_script_sh (#1905)

### Breaking changes

* Reject NTV2 files where GS_TYPE != SECONDS #1294

* On Windows the name of the library is now fixed to ``proj.lib`` instead
  of encoding the version number in the library name (#1581)

* Require C99 compiler (#1624)

* Remove deprecated JNI bindings (#1825)

* Remove -ld option from proj and cs2cs (#1844)

* Increase CMake minimum version from 3.5 to 3.9 (#1907)

### Thanks to

* Jeff McKenna
* Calum Robinson
* Anshul Singhvi
* Bas Couwenberg
* Mike Taves
* Alan D. Snow
* Charles Karney
* Kristian Evers
* Even Rouault

## 6.3.1

### Updates

* Update the EPSG database to version 9.8.6

* Database: add mapping for gg10_smv2.mnt and gg10_sbv2.mnt French grids

* Database: add mapping for TOR27CSv1.GSB

### Bug fixes

* Fix wrong use of derivingConversionRef() that caused issues with use of
  +init=epsg:XXXX by GDAL (affecting R spatial libraries) or in MapServer

* fix exporting CoordinateSystem to PROJ JSON with ID

* projinfo: use No. abbreviation instead of UTF-8 character (#1828)

* CompoundCRS::identify(): avoid exception when horiz/vertical part is a
  BoundCRS

* createOperations(): fix dealing with projected 3D CRS whose Z units != metre

* WKT1_GDAL export: limit datum name massaging to names matching EPSG (#1835)

* unitconvert with mjd time format: avoid potential integer overflow
 (ossfuzz 20072)

* ProjectedCRS::identify(): fix wrong identification of some ESRI WKT linked
  to units

* Database: add a geoid_like value for proj_method column of grid_alternatives,
  fix related entries and simplify/robustify logic to deal with EPSG
  'Geographic3D to GravityRelatedHeight' methods

* Fix ingestion of +proj=cea with +k_0 (#1881)

* Fix performance issue, affecting PROJ.4 string generation of EPSG:7842
  (#1913)

* Fix identification of ESRI-style datum names starting with D_ but without
  alias (#1911)

* cart: Avoid discontinuity at poles in the inverse case (#1906)

* Various updates to make regression test suite pass with gcc on i386 (#1906)

### Thanks to

* Alan D. Snow
* GitHub user @russkel
* Gerrit Holl
* Anshul Singhvi
* Raven Kopelman
* Kristian Evers
* Even Rouault

## 6.3.0

### Updates

* Database: tune accuracy of Canadian NTv1 file w.r.t NTv2 (#1812)

* Modify verbosity level of some debug/trace messages (#1811)

* projinfo: no longer call createBoundCRSToWGS84IfPossible() for WKT1:GDAL
  (#1810)

* proj_trans: add retry logic to select other transformation if the best one
  fails. (#1809)

* BoundCRS::identify(): improvements to discard CRS that aren't relevant
  (#1802)

* Database: update to IGNF v3.1.0 (#1785)

* Build: Only export symbols if building DLL (#1773)

* Database: update ESRI entries with ArcGIS Desktop version 10.8.0 database
  (#1762)

* createOperations(): chain operations whose middle CRSs are not identical but
  have the same datum (#1734)

* import/export PROJJSON: support a interpolation_crs key to geoid_model
  (#1732)

* Database: update to EPSG v9.8.4 (#1725)

* Build: require SQLite 3.11 (#1721)

* Add support for GEOIDMODEL (#1710)

* Better filtering based on extent and performance improvements (#1709)

### Bug fixes

* Horizontal grid shift: fix issue on iterative inverse computation when
  switching between (sub)grids (#1797)

* createOperations(): make filtering out of 'uninteresting' operations less
  aggressive (#1788)

* Make EPSG:102100 resolve to ESRI:102100 (#1786)

* ob_tran: restore traditional handling of +to_meter with pj_transform() and
  proj utility (#1783)

* CRS identification: use case insensitive comparison for authority name
  (#1780)

* normalizeForVisualization() and other methods applying on a ProjectedCRS: do
  not mess the derivingConversion object of the original object (#1746)

* createOperations(): fix transformation computation from/to a CRS with
  +geoidgrids and +vunits != m (#1731)

* Fix proj_assign_context()/pj_set_ctx() with pipelines and alternative coord
  operations (#1726)

* Database: add an auxiliary concatenated_operation_step table to allow
  arbitrary number of steps (#1696)

* Fix errors running gie-based tests in Debug mode on Windows (#1688)

### Thanks to

* Pedro Venancio
* Owen Rudge
* Nyall Dawson
* Mateusz Łoskot
* Markus Neteler
* Juergen E. Fischer
* Joaquim Luis
* Jeff McKenna
* Jakob Egger
* Guillaume Lostis
* GitHub user @yonarw
* Asa Packer
* Joe Mann
* Stephan Hügel
* Simon Schneegans
* R. Schmunk
* Alan D. Snow
* Chris Crook
* Howard Butler
* Fabrice Fontaine
* Kai Pastor
* Martin Desruisseaux
* Dalia Prizginiene
* Mike Taves
* Charles Karney
* Kristian Evers
* Even Rouault

## 6.2.1

### Updates

* Update the EPSG database to version 9.8.2

### Bug fixes

* Fixed erroneous spelling of "Potsdam" (#1573)

* Calculate y-coordinate correctly in bertin1953 in all cases (#1579)

* proj_create_crs_to_crs_from_pj(): make the PJ*arguments const PJ* (#1583)

* PROJStringParser::createFromPROJString(): avoid potential infinite
  recursion (#1574)

* Avoid core dump when setting ctx==NULL in functions
  proj_coordoperation_is_instantiable and
  proj_coordoperation_has_ballpark_transformation (#1590)

* createOperations(): fix conversion from/to PROJ.4 CRS strings with
  non-ISO-kosher options and +towgs84/+nadgrids (#1602)

* proj_trans_generic(): properly set coordinate time to HUGE_VAL when no
  value is passed to the function (#1604)

* Fix support for +proj=ob_tran +o_proj=lonlat/latlong/latlon instead of only
  only allowing +o_proj=longlat (#1601)

* Improve backwards compatibility of vertical transforms (#1613)

* Improve emulation of deprecated +init style initialization (#1614)

* cs2cs: autopromote CRS to 3D when there's a mix of 2D and 3D (#1563)

* Avoid divisions by zero in odd situations (#1620)

* Avoid compile error on Solaris (#1639)

* proj_create_crs_to_crs(): fix when there are only transformations with
  ballpark steps (#1643)

* PROJ string CRS ingester: recognize more unit-less parameters, and general
  handling of +key=string_value parameters (#1645)

* Only call pkg-config in configure when necessary (#1652)

* aeqd: for spherical forward path, go to higher precision ellipsoidal
  case when the point coordinates are super close to the origin (#1654)

* proj_create_crs_to_crs(): remove elimination of Ballpark operations
  that caused transformation failures in some cases (#1665)

* createOperations(): allow transforming from a compoundCRS of a bound
  verticalCRS to a 2D CRS (#1667)

* Avoid segfaults in case of out-of-memory situations (#1679)

* createOperations(): fix double vertical unit conversion from CompoundCRS
  to other CRS when the horizontal part of the projected CRS uses non-metre
  unit (#1683)

* importFromWkt(): fix axis orientation for non-standard ESRI WKT (#1690)

### Thanks to

* R. Schmunk
* Jakob Egger
* Alan D. Snow
* Stephan Hügel
* Kai Pastor
* Kristian Evers
* Even Rouault

## 6.2.0

### Updates

* Introduced PROJJSON, a JSON encoding of WKT2 (#1547)

* Support CRS instantiation of OGC URN's (#1505)

* Expose scope and remarks of database objects (#1537)

* EPSG Database updated to version 9.7.0 (#1558)

* Added C API function proj_grid_get_info_from_database() (#1494)

* Added C API function
  proj_operation_factory_context_set_discard_superseded() (#1534)

* Added C API function proj_context_set_autoclose_database() (#1566)

* Added C API function proj_create_crs_to_crs_from_pj() (#1567)

* Added C API function proj_cleanup() (#1569)

### Bug fixes

* Fixed build failure on Solaris systems (#1554)

### Thanks to

Version 6.2.0 is made possible by the following contributors:

* GitHub user @edechaux
* Michael D. Smith
* Matt Littlemore
* Kristian Evers
* Even Rouault

## 6.1.1

### Updates

* Update EPSG registry to version 9.6.3 (1485)

### Bug fixes

* Take the passed authority into account when identifying
  objects (#1466)

* Avoid exception when transforming from NAD83 to projected
  CRS using NAD83(2011) (#1477)

* Avoid off-by-one reading of name argument if name of resource
  file has length 1 (#1489)

* Do not include PROJ_LIB in proj_info().searchpath when context
  search path is set (#1498)

* Use  correct delimiter for the current platform when parsing
  PROJ_LIB (#1497)

* Do not confuse 'ID74' CRS with WKT2 ID[] node (#1506)

* WKT1 importer: do case insensitive comparison for axis
  direction (#1509)

* Avoid compile errors on GCC 4.9.3 (#1512)

* Make sure that pipelines including +proj=ob_tran can be
  created (#1526)

### Thanks to

Version 6.1.1 is made possible by the following contributors:

* Alan D. Snow
* Paul Menzel
* Mateusz Łoskot
* Bas Couwenberg
* Peter Limkilde Svendsen
* Mike Taves
* Howard Butler
* Nyall Dawson
* Andrew Bell
* Kristian Evers
* Even Rouault

## 6.1.0

### Updates

* Include custom ellipsoid definitions from QGIS (#1337)

* Add "-k ellipsoid" option to projinfo (#1338)

* Make cs2cs support 4D coordinates (#1355)

* WKT2 parser: update to OGC 18-010r6 (#1360 #1366)

* Update internal version of googletest to v1.8.1 (#1361)

* Database update: EPSG v9.6.2 (#1462), IGNF v3.0.3, ESRI 10.7.0
  and add operation_version column (#1368)

* Add proj_normalize_for_visualization() that attempts to apply axis
  ordering as used by most GIS applications and PROJ <6 (#1387)

* Added noop operation (#1391)

* Paths set by user take priority over PROJ_LIB for search paths (#1398)

* Reduced database size (#1438)

* add support for compoundCRS and concatenatedOperation named from
  their components (#1441)

### Bug fixes

* Have gie return non-zero code when file can't be opened (#1312)

* CMake cross-compilation fix (#1316)

* Use 1st eccentricity instead of 2nd eccentricity in Molodensky (#1324)

* Make sure to include grids when doing Geocentric to CompoundCRS with
  nadgrids+geoidgrids transformations (#1326)

* Handle coordinates outside of bbox better (#1333)

* Enable system error messages in command line automatically in builds (#1336)

* Make sure to install projinfo man page with CMake (#1347)

* Add data dir to pkg-config file proj.pc (#1348)

* Fix GCC 9 warning about useless std::move() (#1352)

* Grid related fixes (#1369)

* Make sure that ISO19111 C++ code sets pj_errno on errors (#1405)

* vgridshift: handle longitude wrap-around for grids with 360deg
  longitude extent (#1429)

* proj/cs2cs: validate value of -f parameter to avoid potential crashes (#1434)

* Many division by zero and similar bug fixes found by OSS Fuzz.

### Thanks to

Version 6.1.0 is made possible by the following contributors:

* Andrew Hardin
* Sean Warren
* Dan Baston
* Howard Butler
* Joris Van den Bossche
* Elliott Sales de Andrade
* Alan D. Snow
* Nyall Dawson
* Chris Mayo
* Mike Taves
* Kristian Evers
* Even Rouault

## 6.0.0

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

### Updates

* Removed projects.h as a public interface (#835)

* Deprecated the proj_api.h interface. The header file is still available
  but will be removed with the next major version release of PROJ. It is
  now required to define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H before the
  interface can be used (#836)

* Removed support for the nmake build system (#838)

* Removed support for the proj_def.dat defaults file (#201)

* C++11 required for building PROJ (#1203)

* Added build dependency on SQLite 3.7 (#1175)

* Added projinfo command line application (#1189)

* Added many functions to proj.h for handling ISO19111 functionality (#1175)

* Added C++ API exposing ISO19111 functionality (#1175)

* Updated cs2cs to use late-binding features (#1182)

* Removed the nad2bin application. Now available in the proj-datumgrid
  git repository (#1236)

* Removed support for Chebyshev polynomials in proj (#1226)

* Removed proj_geocentric_latitude from proj.h API (#1170)

* Changed behaviour of proj: Now only allow initialization of
  projections (#1162)

* Changed behaviour of tmerc: Now default to the Extended Transverse
  Mercator algorithm (etmerc). Old implementation available by adding
  +approx (#404)

* Changed behaviour: Default ellipsoid now set to GRS80 (was WGS84) (#1210)

* Allow multiple directories in PROJ_LIB environment variable (#1281)

* Added Lambert Conic Conformal (2SP Michigan) projection (#1142)

* Added Bertin1953 projection (#1133)

* Added Tobler-Mercator projection (#1153)

* Added Molodensky-Badekas transform (#1160)

* Added push and pop coordinate operations (#1250)

* Removed +t_obs parameter from helmert and deformation (#1264)

* Added +dt parameter to deformation as replacement for
  removed +t_obs (#1264)

### Bug fixes

* Read +towgs84 values correctly on locales not using dot as comma separator (#1136)

* Fixed file offset for reading of shift values in NTv1 files (#1144)

* Avoid problems with PTHREAD_MUTEX_RECURSIVE when using CMake (#1158)

* Avoid raising errors when setting ellipsoid flattening to zero (#1191)

* Fixed lower square calculations in rHealpix projection (#1206)

* Allow Molodensky transform parameters to be zero (#1194)

* Fixed wrong parameter in ITRF2000 init file (#1240)

* Fixed use of grid paths including spaces (#1152)

* Robinson: fix wrong values for forward path for latitudes >= 87.5 (#1172),
  and fix inaccurate inverse method.

### Thanks to

Version 6.0.0 is made possible by the following contributors:

* Aaron Puchert
* Thomas Knudsen
* Phil Elson
* Mateusz Łoskot
* Markus Neteler
* Jürgen Fischer
* Charles Karney
* Bas Couwenberg
* Karoline Skaar
* Alan D. Snow
* Howard Butler
* Marco Bernasocchi
* Ben Boeckel
* Ivan Veselov
* Philippe Rivière
* Mike Taves
* Elliott Sales de Andrade
* Kai Pastor
* Kristian Evers
* Even Rouault

## 5.2.0

### Updates

* Added support for deg, rad and grad in unitconvert (#1054)

* Assume +t_epoch as time input when not otherwise specified (#1065)

* Added inverse Lagrange projection (#1058)

* Added +multiplier option to vgridshift (#1072)

* Added Equal Earth projection (#1085)

* Added "require_grid" option to gie (#1088)

* Replace +transpose option of Helmert transform with +convention.
  From now on the convention used should be explicitly written. An
  error will be returned when using the +transpose option (#1091)

* Improved numerical precision of inverse spherical Mercator
  projection (#1105)

* cct will now forward text after coordinate input to output
  stream (#1111)

### Bug fixes

* Do not pivot over WGS84 when doing cs2cs-emulation with geocent (#1026)

* Do not scan past the end of the read data in pj_ctx_fgets (#1042)

* Make sure proj_errno_string() is available in DLL (#1050)

* Respect +to_meter setting when doing cs2cs-emulation (#1053)

* Fixed unit conversion factors for geod (#1075)

* Fixed test failures related to GCC 8 (#1084)

* Improved handling of +geoc flag (#1093)

* Calculate correct projection factors for Webmercator (#1095)

* cs2cs now always outputs degrees when transformed coordinates are
  in angular units (#1112)

All bug fix numbers refer to issues or pull requests indexed at
<https://github.com/OSGeo/proj.4/>

### Thanks to

Version 5.2.0 is made possible by the following contributors:

* Søren Holm
* Mateusz Łoskot
* Jürnjakob Dugge
* Greg Minshall
* Aaron Puchert
* Vedran Stojnović
* Bojan Šavrič
* Charles Karney
* Mateusz Loskot
* Howard Butler
* Mike Toews
* Kurt Schwehr
* Even Rouault
* Kristian Evers

## 5.1.0

### Updates

* Function proj_errno_string() added to proj.h API (#847)

* Validate units between pipeline steps and ensure transformation
  sanity (#906)

* Print help when calling cct and gie without arguments (#907)

* CITATION file added to source distribution (#914)

* Webmercator operation added (#925)

* Enhanced numerical precision of forward spherical Mercator near
  the Equator (#928)

* Added --skip-lines option to cct (#923)

* Consistently return NaN values on NaN input (#949)

* Removed unused src/org_proj4_Projections.h file (#956)

* Java Native Interface bindings updated (#957, #969)

* Horizontal and vertical gridshift operations extended to
  the temporal domain (#1015)

### Bug fixes

* Handle nan float cast overflow in PJ_robin.c and nad_intr.c (#887)

* Avoid overflow when Horner order is unreasonably large (#893)

* Avoid unwanted NaN conversions in etmerc (#899)

* Avoid memory failure in gie when not specifying x,y,z in gie files (#902)

* Avoid memory failure when +sweep is initialized incorrectly in geos (#908)

* Return HUGE_VAL on erroneous input in ortho (#912)

* Handle commented lines correctly in cct (#933)

* Avoid segmentation fault when transformation coordinates outside grid
  area in deformation (#934)

* Avoid doing false easting/northing adjustments on cartesian
  coordinates (#936)

* Thread-safe creation of proj mutex (#954)

* Avoid errors when setting up geos with +lat_0!=0 (#986)

* Reset errno when running proj in verbose mode (#988)

* Do not interpolate node values at nodata value in vertical
  grid shifts (#1004)

* Restrict Horner degrees to positive integer values to avoid
  memory allocation issues (#1005)

All bug fix numbers refer to issues or pull requests indexed at
<https://github.com/OSGeo/proj.4/>

### Thanks to

Version 5.1.0 is made possible by the following contributors:

* Kristian Evers
* Even Rouault
* Kurt Schwehr
* Mike Toews
* Martin Desruisseaux
* Charles Karney
* Thomas Knudsen
* Javier Goizueta
* Bas Couwenberg
* Adam Wulkiewicz
* Aaron Puchert

## 5.0.1

### Bug fixes

All bug fix numbers refer to issues or pull requests indexed at
<https://github.com/OSGeo/proj.4/>

* Handle ellipsoid change correctly in pipelines when
  +towgs84=0,0,0 is set #881

* Handle the case where nad_ctable2_init returns NULL #883

* Avoid shadowed declaration errors with old gcc #880

* Expand +datum properly +datum in pipelines #872

* Fail gracefully when incorrect headers are encountered in grid
  files #875

* Improve roundtrip stability in pipelines using +towgs84 #871

* Fixed typo in gie error codes #861

* Numerical stability fixes to the geodesic package #826 #843

* Make sure that transient errors are returned correctly #857

* Make sure that locally installed header files are not used when
  building PROJ #849

* Fix inconsistent parameter names in proj.h/proj_4D_api.c #842

* Make sure +vunits is applied #833

* Fix incorrect Web Mercator transformations #834

### Thanks to

Version 5.0.1 is made possible by the following contributors:

* Mike Toews
* Kurt Schwehr
* Even Rouault
* Charles Karney
* Thomas Knudsen
* Kristian Evers

## 5.0.0

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
interface and a new command line utility, "cct" (for "Coordinate
Conversion and Transformation").  The old programming interface is
still available and can - to some extent - use the new geodetic
transformation features.

The internal architecture has also seen many changes and much
improvement.  So far, these improvements respect the existing
programming interface. But the process has revealed a need to simplify
and reduce the code base, in order to support sustained active
development.

```text
!!!
!!! Therefore we have scheduled regular releases over the coming years
!!! which will gradually remove the old programming interface.
!!!
!!! This will cause breaking changes with the next two major version
!!! releases, which will affect all projects that depend on PROJ
!!! (cf. section "deprecations" below).
!!!
```

The decision to break the existing API has not been easy, but has
ultimately been deemed necessary to ensure the long term survival of
the project. Not only by improving the maintainability immensely, but
also by extending the potential user (and hence developer) community.

The end goal is to deliver a generic coordinate transformation
software package with a clean and concise code base appealing to
both users and developers.

### Versioning and naming

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

### Updates

* Introduced new API in proj.h.
  * The new API is orthogonal to the existing proj_api.h API and the
    internally used projects.h API.
  * The new API adds the ability to transform spatiotemporal (4D)
    coordinates.
  * Functions in the new API use the "proj_" namespace.
  * Data types in the new API use the "PJ_" namespace, with a few
    historic exceptions such as XY, XYZ, LP and LPZ.

* Introduced the concept of "transformation pipelines" that makes it
  possible to do complex geodetic transformations of spatiotemporal
  coordinates by daisy chaining simple coordinate operations.

* Introduced cct, the Coordinate Conversion and Transformation
  application.

* Introduced gie, the Geospatial Integrity Investigation Environment.
  * Selftest invoked by -C flag in proj has been removed
  * Ported approx. 1300 built-in selftests to gie format
  * Ported approx. 1000 tests from the gigs test framework
  * Added approx. 200 new tests

* Adopted terminology from the OGC/ISO-19100 geospatial standards
  series. Key definitions are:
  * At the most generic level, a *coordinate operation* is a change
    of coordinates, based on a one-to-one relationship, from one
    coordinate reference system to another.
  * A *transformation* is a coordinate operation in which the two
    coordinate reference systems are based on different datums, e.g.
    a change from a global reference frame to a regional frame.
  * A *conversion* is a coordinate operation in which both
    coordinate reference systems are based on the same datum,
    e.g. change of units of coordinates.
  * A *projection* is a coordinate conversion from an ellipsoidal
    coordinate system to a plane. Although projections are simply
    conversions according to the standard, they are treated as
    separate entities in PROJ as they make up the vast majority
    of operations in the library.

* New operations:
  * The pipeline operator (pipeline)
  * Transformations:
    * Helmert transform (helmert)
    * Horner real and complex polynomial evaluation (horner)
    * Horizontal gridshift (hgridshift)
    * Vertical gridshift (vgridshift)
    * Molodensky transform (molodensky)
    * Kinematic gridshift with deformation model (deformation)
  * Conversions:
    * Unit conversion (unitconvert)
    * Axis swap (axisswap)
  * Projections:
    * Central Conic projection (ccon)

* Significant documentation updates, including
  * Overhaul of the structure of the documentation
  * A better introduction to the use of PROJ
  * A complete reference to the new proj.h API
  * a complete rewrite of the section on geodesic calculations
  * Figures for all projections

* New "free format" option for operation definitions, which
  permits separating tokens by whitespace when specifying key/value-
  pairs, e.g. "proj = merc lat_0 = 45".

* Added metadata to init-files that can be read with the
  proj_init_info() function in the new proj.h API.

* Added ITRF2000, ITRF2008 and ITRF2014 init-files with ITRF
  transformation parameters, including plate motion model
  parameters.

* Added ellipsoid parameters for GSK2011, PZ90 and "danish". The
  latter is similar to the already supported andrae ellipsoid,
  but has a slightly different semimajor axis.

* Added Copenhagen prime meridian.

* Updated EPSG database to version 9.2.0.

* Geodesic library updated to version 1.49.2-c.

* Support for analytical partial derivatives has been removed.

* Improved performance in Winkel Tripel and Aitoff.

* Introduced pj_has_inverse() function to proj_api.h. Checks if an
  operation has an inverse. Use this instead of checking whether
  P->inv exists, since that can no longer be relied on.

* ABI version number updated to 13:0:0.

* Removed support for Windows CE.

* Removed the VB6 COM interface.

### Bug fixes

All bug fix numbers refer to issues indexed at
<https://github.com/OSGeo/proj.4/issues/>

* Fixed incorrect convergence calculation in Lambert Conformal
  Conic. #16.

* Handle ellipsoid parameters correctly when using +nadgrids=@null. #22.

* Return correct latitude when using negative northings in
  Transverse Mercator (tmerc). #138.

* Return correct result at origin in inverse Modified Stereographic
  of Alaska. #161.

* Return correct result at origin in inverse Modified Stereographic
  of 48 U.S. #162.

* Return correct result at origin in inverse Modified Stereographic
  of 50 U.S. #163.

* Return correct result at origin in inverse Lee Oblated
  Stereographic. #164.

* Return correct result at origin in inverse Miller Oblated
  Stereographic. #164.

* Fixed scaling and wrap-around issues in Oblique Cylindrical
  Equal Area. #166.

* Corrected a coefficient error in inverse Transverse Mercator. #174.

* Respect -r flag when calling proj with -V. #184.

* Remove multiplication by 2 at the equator error in Stereographic
  projection. #194.

* Allow +alpha=0 and +gamma=0 when using Oblique Mercator. #195.

* Return correct result of inverse Oblique Mercator when alpha is
  between 90 and 270. #331.

* Avoid segmentation fault when accessing point outside grid. #369.

* Avoid segmentation fault on NaN input in Robin inverse. #463.

* Very verbose use of proj (-V) on Windows is fixed. #484.

* Fixed memory leak in General Oblique Transformation. #497.

* Equations for meridian convergence and partial derivatives have
  been corrected for non-conformal projections. #526.

* Fixed scaling of cartesian coordinates in pj_transform(). #726.

* Additional bug fixes courtesy of Google's OSS-Fuzz program:
  <https://bugs.chromium.org/p/oss-fuzz/issues/list?can=1&q=proj4>

### Deprecations

* The projects.h header and the functions related to it is
  considered deprecated from version 5.0.0 and onwards.

  ```text
  !!!     PROJECTS.H WILL BE REMOVED FROM THE LIBRARY     !!!
  !!!                 WITH VERSION 6.0.0                  !!!
  ```

* The nmake build system on Windows will not be supported from
  version 6.0.0 on onwards. Use CMake instead.

  ```text
  !!! NMAKE BUILD SYSTEM WILL BE REMOVED FROM THE LIBRARY !!!
  !!!                 WITH VERSION 6.0.0                  !!!
  ```

* The proj_api.h header and the functions related to it is
  consided deprecated from version 5.0.0 and onwards.

  ```text
  !!!     PROJ_API.H WILL BE REMOVED FROM THE LIBRARY     !!!
  !!!                 WITH VERSION 7.0.0                  !!!
  ```

### Thanks to

Version 5.0.0 is made possible by the following contributors:

* Lukasz Komsta
* Maxim Churilin
* edechaux
* dusanjovic
* Zoltan Siki
* Tom Fili
* Nicolas David
* Mike Toews
* Micah Cochran
* Luke Campbell
* Ilya Oshchepkov
* Adam Wulkiewicz
* Jonas Tittmann
* Mateusz Loskot
* Etienne Jacques
* Bas Couwenberg
* Elliott Sales de Andrade
* Charles Karney
* Aaron Puchert
* Julien Moquet
* Charles Karney
* Howard Butler
* Even Rouault
* Thomas Knudsen
* Kristian Evers

## 4.9.3

* UTM now uses etmerc, following NGA recommendations.  Tests adjusted
  for tiny changes in values.

* new projections: Times, Natural Earth II, Compact Miller, Patterson
  Cylindrical, and inverse for Hammer and Eckert-Greifendorff.

* runtime self tests are now opt-in instead of opt-out

* math constants moved to projects.h

* bugfixes

* New (optional) runtime self tests added to proj

## 4.9.2

* proj_def.dat was missing from source distribution
  see <https://github.com/OSGeo/proj.4/issues/274> for more detail

* Update Geodesic library from GeographicLib

* Remove setlocale() use in pj_init_ctx()

* Renamed PVALUE in pj_param.c to prevent clash with Windows

## 4.9.1

* 4.9.0RC2 release was abandoned because it was not promoted in a
  timely fashion. Subsequent maintenance of tickets has continued,
  and a new 4.9.1 release was issued in its place.

* Implement inverse solution for Winkel Tripel from Drazan Tutic #250

* More CMake configuration tweaks. The CMake configuration is probably
  not at feature parity with the autotools builds at this point but it
  is converging #256

* Tweak initialization ordering around setlocal which may have caused
  issues #237

* Support out-of-tree autoconf builds more completely #247

* Fix NaN handling by geod_inverse and geod_polygon_addedge #251 & #253

* Update config.sub and config.guess #257

* Adapt Charles Karney's CMake patches for smoother build #258

* Define default PROJ_LIB location for CMake compilation #261

* Fix Windows compilation on PJ_aitoff.c

* Align CMake SOVERSION with autotools #263

* Regenerate nad/epsg with GDAL r28536 to avoid precision loss in TOWGS84
  parameters, e.g. on Amersfoort / RD EPSG:4289 (#260)

* Add CMake project-config.cmake scripts (#264 from Charles Karney)

* Dial back test sensitivity #255

## 4.9.0

* Implement CMake as an option for building PROJ.4

* Implement new virtual file api (projFileAPI) so that all access to grid
  shift and init files can be hooked.

* Replace geodesic implementation with one from Charles Karney and add a
  supported public interface (geodesic.h).

* Upgraded to EPSG 8.5.

* Removed old (deprecated) Java bindings in favor of the new api introduced
  in 4.8.0.

* Implement the calcofi (Cal Coop Ocean Fish Invest Lines/Stations) projection

* Install projects.h again for applications that want access to internal
  structures and functions despite the inherent fragility.

* Various bug fixes and cleanup.

* Added the CalCOFI pseudo-projection, #135

## 4.8.0

* Added the Natural Earth projection.

* Added HEALPIX, rHEALPIX and Icosahedral Snyder Equal Area projections.

* nad2bin now produces "CTable2" format grid shift files by default which
  are platform independent.

* nad2nad removed, use cs2cs for datum shift operations.

* projects.h no longer installed as a public include file.  Please try to
  only use proj_api.h.

* Add pj_get_spheroid_defn() accessor.

* Added an alternate version of pj_init() that takes a projCtx (execution
  context) structure to address multithreading issues with error management
  and to provide a support for application hookable error reporting and
  logging.

* Upgrade to EPSG 7.9.  Some changes in ideal datum selection.

* JNI bindings reworked, org.proj4.Projections deprecated in favor of
  org.proj4.PJ.

* Added preliminary vertical datum support.

* Fix various multithreading issues, particular in datum grid handling code.

* Added support for the +axis= option for alternate axis orientations as
  part of a coordinate system (used for TM South Orientated support).

* +proj=omerc implementatioin replaced with code from libproj4.  +rot_conv
  flag no longer works, and some coordinate systems (ie. Malaysian) will
  need to use +gamma instead.  "epsg" init file updated accordingly.

## 4.7.0

* Added in memory caching of init file search results for substantial
  acceleration in some application environments (like MapServer).

* Regenerated nad/epsg init file with EPSG 7.1 database, including new
  support for Google Mercator (EPSG:3857).

* Various thread safety improvements, including preliminary support for
  a mutex lock around some operations, like grid loading.

## 4.6.1

* Upgraded to EPSG 6.17 version for nad/epsg.  Also corrected the precision
  problem introduced in the last version.

* Added logic for tmerc projection to fail rather than return crazy results
  if more than 90 degrees away from the central meridian (#5).  This change
  may only be temporary till a more comprehensive solution is found.

* Fixed handling of extra text in cs2cs.

* Renamed INSTALL to INSTALL.TXT.

* The eqc projection has been generalized to include a latitude of origin.

* Added the glabsgm (Gauss Laborde / Sphere Geometric Mean) projection,
  and gstmerc variation.

* nad/IGNF init catalogue created.

* added the ntf_r93.gsb datum shift file.

* Add /Op in nmake.opt compile options to avoid VC7 optimization bug (#12)

* Fix testvarious test script so it works properly when grid files available

## 4.6.0

* MAJOR: Rework pj_transform() to avoid applying ellipsoid to ellipsoid
  transformations as a datum shift when no datum info is available.

* Avoid applying "change of ellipsoid" during datum shifting based on
  ellipsoids getting changed to spheres for spherical projections.

* Upgrade to EPSG 6.13

* Added "900913" "Google Mercator" code in nad/esri.extra.

* Avoid some static variable based multi-threading problems.

* Improve error recovery if ctable style datum shift grid fails to load.

* Support drive letters properly in PROJ_LIB paths for win32.

* Fix occasional problem with DMS parsing.

* Removed non-functional mpoly projection.

* add lonlat, and latlon as aliases for longlat.

## 4.5.0

* Microsoft Visual Studio 8 compatibility changes.

* Upgraded to EPSG 6.11.1 database.

* Several bug fixes to pj_transform() to handle transient errors properly.

* Fix Krovak projection (include +czech flag)

* Added Roussilhe Stereographic projection from libproj4 (proj=rouss).

* Added experimental +lon_wrap flag for alternate pj_transform() longitude
  wrapping behavior on output.

## 4.4.9

* Win32 build uses proj.dll by default now.

* Added accessor functions for the datums, ellps, prime_meridians, units and
  main projection definition lists so they can be used on windows where
  data objects exported from the DLL don't work easily.

* Added JNI (Java Native Interface) support within the jniwrap directory,
  and some supporting entry points directly in the proj dll/so.   See
  --with-jni configure switch, and jniwrap/README.

* Added the proj=stereoa and proj=geos projections, incorporated from
  Gerald's libproj4.

* A few other bug fixes.

## 4.4.8

* Updated epsg file to EPSG 6.5, added other.extra pseudo-EPSG WMS definitions

* Made significant fixes to prime meridian support.

* Substantially change pj_datum_transform(), so it and pj_transform() will
  work between coordinate systems with different ellipsoids but no datum
  shift information (assume +towgs84=0,0,0).

* Added pj_get_release() function.

* Ensure pj_transform() will try to transform all points in provided list if
  even some might transform properly.

* Improved the accuracy of Geocentric_To_Geodetic() via an iterative
  solution.

* Various other bug fixes.

## 4.4.7

* Added NTv2 (.gsb) grid shift file support.

* Modified datum shift support so that data file data is only loaded if
  needed.  Also added 'null' grid as a fallback option, and support for
  making grids optional (prefix with '@' in +nadgrids).

* Regenerated nad/epsg file with towgs84 parameters for non-greenwich prime
  meridians.

* Updated nad/esri file with better generated form from Paul Ramsey.

* Various bug fixes.

## 4.4.6

* Incorporated new lcca (Lambert Conformal Conic Alternate) projection from
  Gerald.

* Updated 'espg' translation file for EPSG 6.2.2 with better support for
  prime meridians.

* Added Prime Meridians via +pm command switch to cs2cs (and pj_transform).

* Fixed bug with 7 parameter transforms.

* Added 'esri' pseudo-epsg coordinate system file.

* Cleanup so that PROJ.4 compiles clean as C++ code.

* Added pj_get_def() to expand definitions of stuff like +init clauses.

* Added a Krovak implementation (proj=krov).  Note this may change again
  in the next release.

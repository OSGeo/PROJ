.. _projinfo:

================================================================================
projinfo
================================================================================

.. Index:: projinfo

.. only:: html

    .. versionadded:: 6.0.0

    Geodetic object and coordinate operation queries

Synopsis
********

    | **projinfo**
    |    [-o formats] [-k crs|operation] [--summary] [-q]
    |    [[--area name_or_code] | [--bbox west_long,south_lat,east_long,north_lat]]
    |    [--spatial-test contains|intersects]
    |    [--crs-extent-use none|both|intersection|smallest]
    |    [--grid-check none|discard_missing|sort] [--show-superseded]
    |    [--pivot-crs always|if_no_direct_transformation|never|{auth:code[,auth:code]*}]
    |    [--boundcrs-to-wgs84]
    |    [--main-db-path path] [--aux-db-path path]*
    |    [--identify]
    |    [--c-ify] [--single-line]
    |    {object_definition} | (-s {srs_def} -t {srs_def})
    |

    where {object_definition} or {object_definition} is a PROJ string, a
    WKT string, an object name or a AUTHORITY:CODE
    (where AUTHORITY is the name of a CRS authority and CODE the code of a CRS
    found in the proj.db database).

Description
***********

:program:`projinfo` is a program that can query information on a geodetic object,
coordinate reference system (CRS) or coordinate operation, when the ``-s`` and ``-t``
options are specified, and display it under different formats (PROJ string, WKT string).

It can also be used to query coordinate operations available between two CRS.

The program is named with some reference to the GDAL :program:`gdalsrsinfo` that offers
partly similar services.


The following control parameters can appear in any order:

.. program:: projinfo

.. option:: -o formats

    formats is a comma separated combination of:
    ``all``, ``default``, ``PROJ``, ``WKT_ALL``, ``WKT2_2015``, ``WKT2_2018``, ``WKT1_GDAL``, ``WKT1_ESRI``.

    Except ``all`` and ``default``, other formats can be preceded by ``-`` to disable them.

.. option:: -k crs|operation

    When used to query a single object with a AUTHORITY:CODE, determines the (k)ind of the object
    in case there are CRS or coordinate operations with the same CODE.
    The default is crs.

.. option:: --summary

    When listing coordinate operations available between 2 CRS, return the
    result in a summary format, mentionning only the name of the coordinate
    operation, its accuracy and its area of use.

    .. note:: only used for coordinate operation computation

.. option:: -q

    Turn on quiet mode. Quiet mode is only available for queries on single objects,
    and only one output format is selected. In that mode, only the PROJ or WKT
    string is displayed, without other introduction output. The output is then
    potentially compatible of being piped in other utilities.

.. option:: --area name_or_code

    Specify an area of interest to restrict the results when researching
    coordinate operations between 2 CRS. The area of interest can be specified either
    as a name (e.g "Denmark - onshore") or a AUTHORITY:CODE (EPSG:3237)
    This option is exclusive of :option:`--bbox`.

    .. note:: only used for coordinate operation computation

.. option:: --bbox west_long,south_lat,east_long,north_lat

    Specify an area of interest to restrict the results when researching
    coordinate operations between 2 CRS. The area of interest is specified as a
    bounding box with geographic coordinates, expressed in degrees in a
    unspecified geographic CRS.
    `west_long` and `east_long` should be in the [-180,180] range, and
    `south_lat` and `north_lat` in the [-90,90]. `west_long` is generally lower than
    `east_long`, except in the case where the area of interest crosses the antimeridian.

    .. note:: only used for coordinate operation computation

.. option:: --spatial-test contains|intersects

    Specify how the area of use of coordinate operations found in the database
    are compared to the area of use specified explicitly with :option:`--area` or :option:`--bbox`,
    or derivedi implictly from the area of use of the source and target CRS.
    By default, projinfo will only keep coordinate operations whose are of use
    is strictly within the area of interest (``contains`` strategy).
    If using the ``intersects`` strategy, the spatial test is relaxed, and any
    coordinate operation whose area of use at least partly intersects the
    area of interest is listed.

    .. note:: only used for coordinate operation computation

.. option:: --crs-extent-use none|both|intersection|smallest

    Specify which area of interest to consider when no explicit one is specified
    with :option:`--area` or :option:`--bbox` options.
    By default (``smallest`` strategy), the area of
    use of the source or target CRS will be looked, and the one that is the
    smallest one in terms of area will be used as the area of interest.
    If using ``none``, no area of interest is used.
    If using ``both``, only coordinate operations that relate (contain or intersect
    depending of the :option:`--spatial-test` strategy) to the area of use of both CRS
    are selected.
    If using ``intersection``, the area of interest is the intersection of the
    bounding box of the area of use of the source and target CRS

    .. note:: only used for coordinate operation computation

.. option:: --grid-check none|discard_missing|sort

    Specify how the presence or absence of a horizontal or vertical shift grid
    required for a coordinate operation affects the results returned when
    researching coordinate operations between 2 CRS.
    The default strategy is ``sort``: in that case, all candidate
    operations are returned, but the actual availability of the grids is used
    to determine the sorting order. That is, if a coordinate operation involves
    using a grid that is not available in the PROJ resource directories
    (determined by the :envvar:`PROJ_LIB` environment variable, it will be listed in
    the bottom of the results.
    The ``none`` strategy completely disables the checks of presence of grids and
    this returns the results as if all the grids where available.
    The ``discard_missing`` strategy discards results that involve grids not
    present in the PROJ resource directories.

    .. note:: only used for coordinate operation computation

.. option:: -show-superseded

    When enabled, coordinate operations that are superseded by others will be
    listed. Note that supersession is not equivalent to deprecation: superseded
    operations are still considered valid although they have a better equivalent,
    whereas deprecated operations have been determined to be erroneous and are
    not considered at all.

    .. note:: only used for coordinate operation computation

.. option:: --pivot-crs always|if_no_direct_transformation|never|{auth:code[,auth:code]*}

    Determine if intermediate (pivot) CRS can be used when researching coordinate
    operation between 2 CRS. A typical example is the WGS84 pivot. By default,
    projinfo will consider any potential pivot if there is no direct transformation
    ( ``if_no_direct_transformation``). If using the ``never`` strategy,
    only direct transformations between the source and target CRS will be
    used. If using the ``always`` strategy, intermediate CRS will be considered
    even if there are direct transformations.
    It is also possible to restrict the pivot CRS to consider by specifying
    one or several CRS by their AUTHORITY:CODE.

    .. note:: only used for coordinate operation computation

.. option:: --boundcrs-to-wgs84

    When specified, this option researches a coordinate operation from the
    base geographic CRS of the single CRS, source or target CRS to the WGS84
    geographic CRS, and if found, wraps those CRS into a BoundCRS object.
    This is mostly to be used for early-binding approaches.

.. option:: --main-db-path path

    Specify the name and path of the database to be used by projinfo. The
    default is proj.db in the PROJ resource directories.

.. option:: --aux-db-path path

    Specify the name and path of auxiliary databases, that are to be combined
    with the main database. Those auxiliary databases must have a table
    structure that is identical to the main database, but can be partly filled
    and their entries can refer to entries of the main database.
    The option may be repeated to specify several auxiliary databases.

.. option:: --c-ify

    For developers only. Modify the string output of the utility so that it
    is easy to put those strings in C/C++ code

.. option:: --single-line

    Output WKT strings on a single line, instead of multiple intended lines by
    default.

Examples
********

1. Query the CRS object corresponding to EPSG:4326

.. code-block:: console

      projinfo EPSG:4326

Output:

::

    PROJ.4 string:
    +proj=longlat +datum=WGS84 +no_defs +type=crs

    WKT2_2018 string:
    GEOGCRS["WGS 84",
        DATUM["World Geodetic System 1984",
            ELLIPSOID["WGS 84",6378137,298.257223563,
                LENGTHUNIT["metre",1]]],
        PRIMEM["Greenwich",0,
            ANGLEUNIT["degree",0.0174532925199433]],
        CS[ellipsoidal,2],
            AXIS["geodetic latitude (Lat)",north,
                ORDER[1],
                ANGLEUNIT["degree",0.0174532925199433]],
            AXIS["geodetic longitude (Lon)",east,
                ORDER[2],
                ANGLEUNIT["degree",0.0174532925199433]],
        USAGE[
            SCOPE["unknown"],
            AREA["World"],
            BBOX[-90,-180,90,180]],
        ID["EPSG",4326]]


2. List the coordinate operations between NAD27 (designed with its CRS name)
   and NAD83 (designed with its EPSG code 4269) within an area of interest

.. code-block:: console

        projinfo -s NAD27 -t EPSG:4269 --area "USA - Missouri"

Output:

::

    DERIVED_FROM(EPSG):1241, NAD27 to NAD83 (1), 0.15 m, USA - CONUS including EEZ

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=2,1 +step +proj=unitconvert \
    +xy_in=deg +xy_out=rad +step +proj=hgridshift +grids=conus \
    +step +proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1

    WKT2_2018 string:
    COORDINATEOPERATION["NAD27 to NAD83 (1)",
        SOURCECRS[
            GEOGCRS["NAD27",
                DATUM["North American Datum 1927",
                    ELLIPSOID["Clarke 1866",6378206.4,294.978698213898,
                        LENGTHUNIT["metre",1]]],
                PRIMEM["Greenwich",0,
                    ANGLEUNIT["degree",0.0174532925199433]],
                CS[ellipsoidal,2],
                    AXIS["geodetic latitude (Lat)",north,
                        ORDER[1],
                        ANGLEUNIT["degree",0.0174532925199433]],
                    AXIS["geodetic longitude (Lon)",east,
                        ORDER[2],
                        ANGLEUNIT["degree",0.0174532925199433]]]],
        TARGETCRS[
            GEOGCRS["NAD83",
                DATUM["North American Datum 1983",
                    ELLIPSOID["GRS 1980",6378137,298.257222101,
                        LENGTHUNIT["metre",1]]],
                PRIMEM["Greenwich",0,
                    ANGLEUNIT["degree",0.0174532925199433]],
                CS[ellipsoidal,2],
                    AXIS["geodetic latitude (Lat)",north,
                        ORDER[1],
                        ANGLEUNIT["degree",0.0174532925199433]],
                    AXIS["geodetic longitude (Lon)",east,
                        ORDER[2],
                        ANGLEUNIT["degree",0.0174532925199433]]]],
        METHOD["CTABLE2"],
        PARAMETERFILE["Latitude and longitude difference file","conus"],
        OPERATIONACCURACY[0.15],
        USAGE[
            SCOPE["unknown"],
            AREA["USA - CONUS including EEZ"],
            BBOX[23.81,-129.17,49.38,-65.69]],
        ID["DERIVED_FROM(EPSG)",1241]]

.. only:: man

    See also
    ********

    **cs2cs(1)**, **cct(1)**, **geod(1)**, **gie(1)**, **proj(1)**

    Bugs
    ****

    A list of know bugs can be found at https://github.com/OSGeo/proj.4/issues
    where new bug reports can be submitted to.

    Home page
    *********

    https://proj4.org/

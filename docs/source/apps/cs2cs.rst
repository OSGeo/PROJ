.. _cs2cs:

================================================================================
cs2cs
================================================================================

.. only:: html

    Filter for transformations between two coordinate reference systems.

Synopsis
********

    | **cs2cs** [**-eEfIlrstvwW** [args]]
    |           [[--area <name_or_code>] | [--bbox <west_long,south_lat,east_long,north_lat>]]
    |           [--authority <name>] [--3d]
    |           [--accuracy <accuracy>] [--only-best[=yes|=no]] [--no-ballpark]
    |           [--s_epoch {epoch}] [--t_epoch {epoch}]
    |           ([*+opt[=arg]* ...] [+to *+opt[=arg]* ...] | {source_crs} {target_crs})
    |           file ...

    where {source_crs} or {target_crs} is one of the possibilities accepted
    by :c:func:`proj_create`, provided it expresses a CRS

    - a proj-string,
    - a WKT string,
    - an object code (like "EPSG:4326", "urn:ogc:def:crs:EPSG::4326",
      "urn:ogc:def:coordinateOperation:EPSG::1671"),
    - an Object name. e.g "WGS 84", "WGS 84 / UTM zone 31N". In that case as
      uniqueness is not guaranteed, heuristics are applied to determine the appropriate best match.
    - a CRS name and a coordinate epoch, separated with '@'. For example "ITRF2014@2025.0". (*added in 9.2*)
    - a OGC URN combining references for compound coordinate reference systems
      (e.g "urn:ogc:def:crs,crs:EPSG::2393,crs:EPSG::5717" or custom abbreviated
      syntax "EPSG:2393+5717"),
    - a OGC URN combining references for references for projected or derived CRSs
      e.g. for Projected 3D CRS "UTM zone 31N / WGS 84 (3D)":
      "urn:ogc:def:crs,crs:EPSG::4979,cs:PROJ::ENh,coordinateOperation:EPSG::16031"
      (*added in 6.2*)
    - a OGC URN combining references for concatenated operations
      (e.g. "urn:ogc:def:coordinateOperation,coordinateOperation:EPSG::3895,coordinateOperation:EPSG::1618")
    - a PROJJSON string. The jsonschema is at https://proj.org/schemas/v0.4/projjson.schema.json (*added in 6.2*)
    - a compound CRS made from two object names separated with " + ". e.g. "WGS 84 + EGM96 height" (*added in 7.1*)

    .. versionadded:: 6.0.0

    .. note:: before 7.0.1, it was needed to add +to between {source_crs} and {target_crs}
              when adding a filename

Description
***********

:program:`cs2cs` performs transformation between the source and destination
cartographic coordinate reference system on a set of input points. The coordinate
reference system transformation can include translation between projected and
geographic coordinates as well as the application of datum shifts.

The following control parameters can appear in any order:

.. program:: cs2cs

.. option:: -I

    Method to specify inverse translation, convert from *+to* coordinate system to
    the primary coordinate system defined.

.. option:: -t<a>

    Where *a* specifies a character employed as the first character to denote a control
    line to be passed through without processing. This option applicable to
    ASCII input only. (# is the default value).

.. option:: -d <n>

    .. versionadded:: 5.2.0

    Specify the number of decimals to round to in the output.

.. option:: -e <string>

    Where *string* is an arbitrary string to be output if an error is detected during
    data transformations. The default value is a three character string: ``*\t*``.

.. option:: -E

    Causes the input coordinates to be copied to the output line prior to
    printing the converted values.

.. option:: -l<[=id]>

    List projection identifiers that can be selected with *+proj*. ``cs2cs -l=id``
    gives expanded description of projection *id*, e.g. ``cs2cs -l=merc``.

.. option:: -lp

    List of all projection id that can be used with the *+proj* parameter.
    Equivalent to ``cs2cs -l``.

.. option:: -lP

    Expanded description of all projections that can be used with the *+proj*
    parameter.

.. option:: -le

    List of all ellipsoids that can be selected with the *+ellps* parameters.

.. option:: -lm

    List of hard-coded prime meridians that can be selected with the *+pm*
    parameter.  Note that this list is no longer updated,
    and some values may conflict with other sources.

.. option:: -lu

    List of all distance units that can be selected with the *+units* parameter.

.. option:: -r

    This options reverses the order of the first two expected
    inputs from that specified by the CRS to the opposite
    order.  The third coordinate, typically height, remains
    third.

.. option:: -s

    This options reverses the order of the first two expected
    outputs from that specified by the CRS to the opposite
    order.  The third coordinate, typically height, remains
    third.


.. option:: -f <format>

    Where *format* is a printf format string to control the form of the output values.
    For inverse projections, the output will be in degrees when this option is
    employed. If a format is specified for inverse projection the output data
    will be in decimal degrees. The default format is ``"%.2f"`` for forward
    projection and DMS for inverse.

.. option:: -w<n>

    Where *n* is the number of significant fractional digits to employ for seconds
    output (when the option is not specified, ``-w3`` is assumed).

.. option:: -W<n>

    Where *n* is the number of significant fractional digits to employ for seconds
    output. When ``-W`` is employed the fields will be constant width
    with leading zeroes. Valid range: -W0 through -W8.

.. option:: -v

    Causes a listing of cartographic control parameters tested for and used by
    the program to be printed prior to input data.

.. option:: --area <name_or_code>

    .. versionadded:: 8.0.0

    Specify an area of interest to restrict the results when researching
    coordinate operations between 2 CRS. The area of interest can be specified either
    as a name (e.g "Denmark - onshore") or a AUTHORITY:CODE (EPSG:3237)

    This option is mutually exclusive with :option:`--bbox`.

.. option:: --bbox <west_long,south_lat,east_long,north_lat>

    .. versionadded:: 8.0.0

    Specify an area of interest to restrict the results when researching
    coordinate operations between 2 CRS. The area of interest is specified as a
    bounding box with geographic coordinates, expressed in degrees in a
    unspecified geographic CRS.
    `west_long` and `east_long` should be in the [-180,180] range, and
    `south_lat` and `north_lat` in the [-90,90]. `west_long` is generally lower than
    `east_long`, except in the case where the area of interest crosses the antimeridian.

.. option:: --only-best[=yes|=no]

    .. versionadded:: 9.2.0

    Force `cs2cs` to only use the best transformation known by PROJ.
    `cs2cs` will return an error if a grid needed for the best transformation is missing.
    
    Best transformation should be understood as the most accurate transformation
    available among all relevant for the point to transform, and if all known
    grids required to perform such transformation were accessible (either locally
    or through network).
    
    Note that the default value for this option can be also set with the
    :envvar:`PROJ_ONLY_BEST_DEFAULT` environment variable, or with the
    ``only_best_default`` setting of :ref:`proj-ini` (:option:`--only-best`
    when specified overrides such default value).

.. option:: --no-ballpark

    .. versionadded:: 8.0.0

    Disallow any coordinate operation that is, or contains, a
    :term:`Ballpark transformation`

.. option:: --accuracy <accuracy>

    .. versionadded:: 8.0.0

    Sets the minimum desired accuracy for candidate coordinate operations.

.. option:: --authority <name>

    .. versionadded:: 8.0.0

    This option can be used to restrict the authority of coordinate operations
    looked up in the database. When not specified, coordinate
    operations from any authority will be searched, with the restrictions set
    in the ``authority_to_authority_preference`` database table related to the authority
    of the source/target CRS themselves.
    If authority is set to ``any``, then coordinate operations from any authority will be searched
    If authority is a non-empty string different of ``any``, then coordinate operations
    will be searched only in that authority namespace (e.g ``EPSG``).

    This option is mutually exclusive with :option:`--bbox`.

.. option:: --3d

    .. versionadded:: 9.1

    "Promote" 2D CRS(s) to their 3D version, where the vertical axis is the
    ellipsoidal height in metres, using the ellipsoid of the base geodetic CRS.
    Depending on PROJ versions and the exact nature of the CRS involved,
    especially before PROJ 9.1, a mix of 2D and 3D CRS could lead to 2D or 3D
    transformations. Starting with PROJ 9.1, both CRS need to be 3D for vertical
    transformation to possibly happen.

.. option:: --s_epoch

    .. versionadded:: 9.4

    Epoch of coordinates in the source CRS, as decimal year.
    Only applies to a dynamic CRS.

.. option:: --t_epoch

    .. versionadded:: 9.4

    Epoch of coordinates in the target CRS, as decimal year.
    Only applies to a dynamic CRS.

.. only:: man

    The *+opt* run-line arguments are associated with cartographic
    parameters.

.. only:: html

    The *+opt* run-line arguments are associated with cartographic
    parameters. Usage varies with projection and for a complete description
    consult the :ref:`projection pages <projections>`.

The :program:`cs2cs` program requires two coordinate reference system (CRS) definitions. The first (or
primary is defined based on all projection parameters not appearing after the
*+to* argument. All projection parameters appearing after the *+to* argument
are considered the definition of the second CRS. If there is no
second CRS defined, a geographic CRS based on the
datum and ellipsoid of the source CRS is assumed. Note that the
source and destination CRS can both of same or different nature (geographic,
projected, compound CRS), or one of each and may have the same or different datums.

When using a WKT definition or a AUTHORITY:CODE, the axis order of the CRS will
be enforced. So for example if using EPSG:4326, the first value expected (or
returned) will be a latitude.

Internally, :program:`cs2cs` uses the :c:func:`proj_create_crs_to_crs` function
to compute the appropriate coordinate operation, so implementation details of
this function directly impact the results returned by the program.

The environment parameter :envvar:`PROJ_DATA` establishes the
directory for resource files (database, datum shift grids, etc.)

One or more files (processed in left to right order) specify the source of
data to be transformed. A ``-`` will specify the location of processing standard
input. If no files are specified, the input is assumed to be from stdin.
For input data the two data values must be in the first two white space
separated fields and when both input and output are ASCII all trailing portions
of the input line are appended to the output line.

Input geographic data (longitude and latitude) must be in DMS or decimal
degrees format and input cartesian data must be in units consistent with the
ellipsoid major axis or sphere radius units. Output geographic coordinates will
normally be in DMS format (use ``-f %.12f`` for decimal degrees with 12 decimal
places), while projected (cartesian) coordinates will be in linear
(meter, feet) units.

Use of remote grids
-------------------

.. versionadded:: 7.0.0

If the :envvar:`PROJ_NETWORK` environment variable is set to ``ON``,
:program:`cs2cs` will attempt to use remote grids stored on CDN (Content
Delivery Network) storage, when they are not available locally.

More details are available in the :ref:`network` section.

Examples
********

Using PROJ strings
------------------

The following script

::

    cs2cs +proj=latlong +datum=NAD83 +to +proj=utm +zone=10 +datum=NAD27 -r <<EOF
    45°15'33.1" 111.5W
    45d15.551666667N -111d30
    +45.25919444444 111d30'000w
    EOF

will transform the input NAD83 geographic coordinates into NAD27 coordinates in
the UTM projection with zone 10 selected. The geographic values of this
example are equivalent and meant as examples of various forms of DMS input.
The x-y output data will appear as three lines of:

::

    1402285.93  5076292.58 0.00


To get those exact values, you have need to have all current grids installed
(in that instance the NADCON5 :file:`us_noaa_nadcon5_nad27_nad83_1986_conus.tif` grid)
locally or use networking capabilities mentioned above.

To make sure you will get the optimal result, you may add :option:`--only-best`.
Assuming the above mentioned grid is *not* available,

::

    echo -111.5 45.25919444444 | cs2cs --only-best +proj=latlong +datum=NAD83 +to +proj=utm +zone=10 +datum=NAD27

would return:

::

    Attempt to use coordinate operation axis order change (2D) + Inverse of NAD27 to NAD83 (7) + axis order change (2D) + UTM zone 10N failed. Grid us_noaa_nadcon5_nad27_nad83_1986_conus.tif is not available. Consult https://proj.org/resource_files.html for guidance.
    *   * inf

Otherwise, if you don't have the grid available and you don't specify :option:`--only-best`:

::

    echo -111.5 45.25919444444 | cs2cs --only-best +proj=latlong +datum=NAD83 +to +proj=utm +zone=10 +datum=NAD27

would return:

::

    1402224.57  5076275.42 0.00

which is the result when the NAD27 and NAD83 datums are dealt as identical,
which is an approximation at a level of several tens of metres.


Using EPSG CRS codes
--------------------

Transforming from WGS 84 latitude/longitude (in that order) to UTM Zone 31N/WGS 84

::

    cs2cs EPSG:4326 EPSG:32631 <<EOF
    45N 2E
    EOF

outputs

::

    421184.70   4983436.77 0.00

Using EPSG CRS names
--------------------

Transforming from WGS 84 latitude/longitude (in that order) with EGM96 height to
UTM Zone 31N/WGS 84 with WGS84 ellipsoidal height

::

    echo 45 2 0 | cs2cs "WGS 84 + EGM96 height" "WGS 84 / UTM zone 31N" --3d

outputs

::

    421184.70   4983436.77 50.69


.. note::

    To get those exact values, you have need to have the EGM96 grid installed
    locally or use networking capabilities mentioned above.


.. only:: man

    See also
    ********

    **proj(1)**, **cct(1)**, **geod(1)**, **gie(1)**, **projinfo(1)**, **projsync(1)**

    .. include:: common_man.rst

.. _functions:

================================================================================
Functions
================================================================================

Threading contexts
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: PJ_CONTEXT* proj_context_create(void)

    Create a new threading-context.

    :returns: :c:type:`PJ_CONTEXT*`

.. c:function:: void proj_context_destroy(PJ_CONTEXT *ctx)

    Deallocate a threading-context.

    :param PJ_CONTEXT* ctx: Threading context.

Transformation setup
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The objects returned by the functions defined in this section have minimal
interaction with the the functions of the
`C API for ISO-19111 functionality`_, and vice versa. See its introduction
paragraph for more details.

.. c:function:: PJ* proj_create(PJ_CONTEXT *ctx, const char *definition)

    Create a transformation object, or a CRS object, from:

    - a proj-string,
    - a WKT string,
    - an object code (like "EPSG:4326", "urn:ogc:def:crs:EPSG::4326",
      "urn:ogc:def:coordinateOperation:EPSG::1671"),
    - a OGC URN combining references for compound coordinate reference systems
      (e.g "urn:ogc:def:crs,crs:EPSG::2393,crs:EPSG::5717" or custom abbreviated
      syntax "EPSG:2393+5717"),
    - a OGC URN combining references for concatenated operations
      (e.g. "urn:ogc:def:coordinateOperation,coordinateOperation:EPSG::3895,coordinateOperation:EPSG::1618")

    Example call:

    .. code-block:: C

        PJ *P = proj_create(0, "+proj=etmerc +lat_0=38 +lon_0=125 +ellps=bessel");

    If a proj-string contains a +type=crs option, then it is interpreted as
    a CRS definition. In particular geographic CRS are assumed to have axis
    in the longitude, latitude order and with degree angular unit. The use
    of proj-string to describe a CRS is discouraged. It is a legacy means of
    conveying CRS descriptions: use of object codes (EPSG:XXXX typically) or
    WKT description is recommended for better expressivity.

    If a proj-string does not contain +type=crs, then it is interpreted as a
    coordination operation / transformation.

    If creation of the transformation object fails, the function returns `0` and
    the PROJ error number is updated. The error number can be read with
    :c:func:`proj_errno` or :c:func:`proj_context_errno`.

    The returned :c:type:`PJ`-pointer should be deallocated with :c:func:`proj_destroy`.

    :param PJ_CONTEXT* ctx: Threading context.
    :param `definition`: Proj-string of the desired transformation.
    :type `definition`: const char*


.. c:function:: PJ* proj_create_argv(PJ_CONTEXT *ctx, int argc, char **argv)

    Create a transformation object, or a CRS object, with argc/argv-style initialization. For this
    application each parameter in the defining proj-string is an entry in :c:data:`argv`.

    Example call:

    .. code-block:: C

        char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
        PJ* P = proj_create_argv(0, 3, args);

    If there is a type=crs argument, then the arguments are interpreted as
    a CRS definition. In particular geographic CRS are assumed to have axis
    in the longitude, latitude order and with degree angular unit.

    If there is no type=crs argument, then it is interpreted as a
    coordination operation / transformation.

    If creation of the transformation object fails, the function returns `0` and
    the PROJ error number is updated. The error number can be read with
    :c:func:`proj_errno` or :c:func:`proj_context_errno`.

    The returned :c:type:`PJ`-pointer should be deallocated with :c:func:`proj_destroy`.

    :param PJ_CONTEXT* ctx: Threading context
    :param int argc: Count of arguments in :c:data:`argv`
    :param char** argv: Vector of strings with proj-string parameters, e.g. ``+proj=merc``
    :returns: :c:type:`PJ*`

.. c:function:: PJ* proj_create_crs_to_crs(PJ_CONTEXT *ctx, const char *source_crs, const char *target_crs, PJ_AREA *area)

    Create a transformation object that is a pipeline between two known
    coordinate reference systems.

    source_crs and target_crs can be :

        - a "AUTHORITY:CODE", like EPSG:25832. When using that syntax for a source
          CRS, the created pipeline will expect that the values passed to :c:func:`proj_trans`
          respect the axis order and axis unit of the official definition (
          so for example, for EPSG:4326, with latitude first and longitude next,
          in degrees). Similarly, when using that syntax for a target CRS, output
          values will be emitted according to the official definition of this CRS.

        - a PROJ string, like "+proj=longlat +datum=WGS84".
          When using that syntax, the axis order and unit for geographic CRS will
          be longitude, latitude, and the unit degrees.

        - the name of a CRS as found in the PROJ database, e.g "WGS84", "NAD27", etc.

        - more generally any string accepted by :c:func:`proj_create` representing
          a CRS

    An "area of use" can be specified in area. When it is supplied, the more
    accurate transformation between two given systems can be chosen.

    When no area of use is specific and several coordinate operations are possible
    depending on the area of use, this function will internally store those
    candidate coordinate operations in the return PJ object. Each subsequent
    coordinate transformation done with :c:func:`proj_trans` will then select
    the appropriate coordinate operation by comparing the input coordinates with
    the area of use of the candidate coordinate operations.

    Example call:

    .. code-block:: C

        PJ *P = proj_create_crs_to_crs(0, "EPSG:25832", "EPSG:25833", 0);

    If creation of the transformation object fails, the function returns `0` and
    the PROJ error number is updated. The error number can be read with
    :c:func:`proj_errno` or :c:func:`proj_context_errno`.


    The returned :c:type:`PJ`-pointer should be deallocated with :c:func:`proj_destroy`.

    :param PJ_CONTEXT* ctx: Threading context.
    :param `source_crs`: Source CRS.
    :type `source_crs`: const char*
    :param `target_crs`: Destination SRS.
    :type `target_crs`: const char*
    :param `area`: Descriptor of the desired area for the transformation.
    :type `area`: PJ_AREA
    :returns: :c:type:`PJ*`

.. c:function:: PJ* proj_create_crs_to_crs_from_pj(PJ_CONTEXT *ctx, PJ *source_crs, PJ *target_crs, PJ_AREA *area, const char* const *options)

    .. versionadded:: 6.2.0

    Create a transformation object that is a pipeline between two known
    coordinate reference systems.

    This is the same as :c:func:`proj_create_crs_to_crs` except that the source and
    target CRS are passed as PJ* objects which must of the CRS variety.

    :param `options`: should be set to NULL currently.

.. c:function:: PJ *proj_normalize_for_visualization(PJ_CONTEXT *ctx, const PJ* obj)

    .. versionadded:: 6.1.0

    Returns a PJ* object whose axis order is the one expected for
    visualization purposes.

    The input object must be a coordinate operation, that has been created with
    proj_create_crs_to_crs().
    If the axis order of its source or target CRS is northing,easting, then an
    axis swap operation will be inserted.

    The returned :c:type:`PJ`-pointer should be deallocated with :c:func:`proj_destroy`.

    :param PJ_CONTEXT* ctx: Threading context.
    :param `obj`: Object of type CoordinateOperation
    :returns: :c:type:`PJ*`


.. c:function:: PJ* proj_destroy(PJ *P)

    Deallocate a :c:type:`PJ` transformation object.

    :param PJ* P:
    :returns: :c:type:`PJ*`

Area of interest
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. versionadded:: 6.0.0


.. c:function:: PJ_AREA* proj_area_create(void)

    Create an area of use.

    Such an area of use is to be passed to :c:func:`proj_create_crs_to_crs` to
    specify the area of use for the choice of relevant coordinate operations.

    :returns: :c:type:`PJ_AREA*` to be deallocated with :c:func:`proj_area_destroy`


.. c:function:: void proj_area_set_bbox(PJ_AREA *area, double west_lon_degree, double south_lat_degree, double east_lon_degree, double north_lat_degree)

    Set the bounding box of the area of use

    Such an area of use is to be passed to :c:func:`proj_create_crs_to_crs` to
    specify the area of use for the choice of relevant coordinate operations.

    In the case of an area of use crossing the antimeridian (longitude +/- 180 degrees),
    `west_lon_degree` will be greater than `east_lon_degree`.

    :param `area`: Pointer to an object returned by :c:func:`proj_area_create`.
    :param `west_lon_degree`: West longitude, in degrees. In [-180,180] range.
    :param `south_lat_degree`: South latitude, in degrees. In [-90,90] range.
    :param `east_lon_degree`: East longitude, in degrees. In [-180,180] range.
    :param `north_lat_degree`: North latitude, in degrees. In [-90,90] range.

.. c:function:: void proj_area_destroy(PJ_AREA* area)

    Deallocate a :c:type:`PJ_AREA` object.

    :param PJ_AREA* area


.. _coord_trans_functions:

Coordinate transformation
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


.. c:function:: PJ_COORD proj_trans(PJ *P, PJ_DIRECTION direction, PJ_COORD coord)

    Transform a single :c:type:`PJ_COORD` coordinate.

    :param PJ* P:
    :param `direction`: Transformation direction.
    :type `direction`: PJ_DIRECTION
    :param PJ_COORD coord: Coordinate that will be transformed.
    :returns: :c:type:`PJ_COORD`


.. c:function:: size_t proj_trans_generic(PJ *P, PJ_DIRECTION direction, \
                                          double *x, size_t sx, size_t nx, double *y, \
                                          size_t sy, size_t ny, double *z, size_t sz, size_t nz, \
                                          double *t, size_t st, size_t nt)

    Transform a series of coordinates, where the individual coordinate dimension
    may be represented by an array that is either

        1. fully populated
        2. a null pointer and/or a length of zero, which will be treated as a
           fully populated array of zeroes
        3. of length one, i.e. a constant, which will be treated as a fully
           populated array of that constant value

    .. note:: Even though he coordinate components are named :c:data:`x`, :c:data:`y`,
              :c:data:`z` and :c:data:`t`, axis ordering of the to and from CRS
              is respected. Transformations exhibit the same behaviour
              as if they were gathered in a :c:type:`PJ_COORD` struct.


    The strides, :c:data:`sx`, :c:data:`sy`, :c:data:`sz`, :c:data:`st`,
    represent the step length, in bytes, between
    consecutive elements of the corresponding array. This makes it possible for
    :c:func:`proj_trans_generic` to handle transformation of a large class of application
    specific data structures, without necessarily understanding the data structure
    format, as in:

    .. code-block:: C

        typedef struct {
            double x, y;
            int quality_level;
            char surveyor_name[134];
        } XYQS;

        XYQS survey[345];
        double height = 23.45;
        size_t stride = sizeof (XYQS);

        ...

        proj_trans_generic (
            P, PJ_INV,
            &(survey[0].x), stride, 345,  /*  We have 345 eastings  */
            &(survey[0].y), stride, 345,  /*  ...and 345 northings. */
            &height, sizeof(double), 1,   /*  The height is the constant  23.45 m */
            0, 0, 0                       /*  and the time is the constant 0.00 s */
        );

    This is similar to the inner workings of the deprecated :c:func:`pj_transform`
    function, but the stride functionality has been generalized to work for any
    size of basic unit, not just a fixed number of doubles.

    In most cases, the stride will be identical for x, y, z, and t, since they will
    typically be either individual arrays (``stride = sizeof(double)``), or strided
    views into an array of application specific data structures (``stride = sizeof (...)``).

    But in order to support cases where :c:data:`x`, :c:data:`y`, :c:data:`z`,
    and :c:data:`t` come from heterogeneous sources, individual strides,
    :c:data:`sx`, :c:data:`sy`, :c:data:`sz`, :c:data:`st`, are used.

    .. note:: Since :c:func:`proj_trans_generic` does its work *in place*,
              this means that even the supposedly constants (i.e. length 1 arrays)
              will return from the call in altered state. Hence, remember to
              reinitialize between repeated calls.

    :param PJ* P: Transformation object
    :param `direction`: Transformation direction
    :type `PJ_DIRECTION`:
    :param double* x: Array of x-coordinates
    :param double* y: Array of y-coordinates
    :param double* z: Array of z-coordinates
    :param double* t: Array of t-coordinates
    :param size_t sx: Step length, in bytes, between consecutive elements of the corresponding array
    :param size_t nx: Number of elements in the corresponding array
    :param size_t sy: Step length, in bytes, between consecutive elements of the corresponding array
    :param size_t nv: Number of elements in the corresponding array
    :param size_t sz: Step length, in bytes, between consecutive elements of the corresponding array
    :param size_t nz: Number of elements in the corresponding array
    :param size_t st: Step length, in bytes, between consecutive elements of the corresponding array
    :param size_t nt: Number of elements in the corresponding array
    :returns: Number of transformations successfully completed



.. c:function:: size_t proj_trans_array(PJ *P, PJ_DIRECTION direction, size_t n, PJ_COORD *coord)

    Batch transform an array of :c:type:`PJ_COORD`.

    :param PJ* P:
    :param `direction`: Transformation direction
    :type `direction`: PJ_DIRECTION
    :param size_t n: Number of coordinates in :c:data:`coord`
    :returns: :c:type:`size_t` 0 if all observations are transformed without error, otherwise returns error number


Error reporting
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: int proj_errno(PJ *P)

    Get a reading of the current error-state of :c:data:`P`. An non-zero error
    codes indicates an error either with the transformation setup or during a
    transformation. In cases :c:data:`P` is `0` the error number of the default
    context is read. A text representation of the error number can be retrieved
    with :c:func:`proj_errno_string`.

    :param: PJ* P: Transformation object.

    :returns: :c:type:`int`

.. c:function:: int proj_context_errno(PJ_CONTEXT *ctx)

    Get a reading of the current error-state of :c:data:`ctx`. An non-zero error
    codes indicates an error either with the transformation setup or during a
    transformation. A text representation of the error number can be retrieved
    with :c:func:`proj_errno_string`.

    :param: PJ_CONTEXT* ctx: threading context.

    :returns: :c:type:`int`

.. c:function:: void proj_errno_set(PJ *P, int err)

Change the error-state of :c:data:`P` to `err`.

    :param PJ* P: Transformation object.
    :param int err: Error number.

.. c:function:: int proj_errno_reset(PJ *P)

    Clears the error number in :c:data:`P`, and bubbles it up to the context.

    Example:

    .. code-block:: C

        void foo (PJ *P) {
            int last_errno = proj_errno_reset (P);

            do_something_with_P (P);

            /* failure - keep latest error status */
            if (proj_errno(P))
                return;
            /* success - restore previous error status */
            proj_errno_restore (P, last_errno);
            return;
        }

    :param: PJ* P: Transformation object.

    :returns: :c:type:`int` Returns the previous value of the errno, for convenient reset/restore operations.

.. c:function:: void proj_errno_restore(PJ *P, int err)

    Reduce some mental impedance in the canonical reset/restore use case:
    Basically, :c:func:`proj_errno_restore()` is a synonym for
    :c:func:`proj_errno_set()`, but the use cases are very different:
    *set* indicate an error to higher level user code, *restore* passes previously
    set error indicators in case of no errors at this level.

    Hence, although the inner working is identical, we provide both options,
    to avoid some rather confusing real world code.

    See usage example under :c:func:`proj_errno_reset`

    :param PJ* P: Transformation object.
    :param int err: Error code.

.. c:function:: const char* proj_errno_string(int err)

    .. versionadded:: 5.1.0

    Get a text representation of an error number.

    :param int err: Error number.

    :returns: :c:type:`const char*` String with description of error.

Logging
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function::  PJ_LOG_LEVEL proj_log_level (PJ_CONTEXT *ctx, PJ_LOG_LEVEL level)

    Get and set logging level for a given context. Changes the log level to
    :c:data:`level` and returns the previous logging level. If called with
    :c:data:`level` set to :c:type:`PJ_LOG_TELL` the function returns the current
    logging level without changing it.

    :param PJ_CONTEXT* ctx: Threading context.
    :param PJ_LOG_LEVEL level: New logging level.

    :returns: :c:type:`PJ_LOG_LEVEL`

    .. versionadded:: 5.1.0

.. c:function::  void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION logf)

    Override the internal log function of PROJ.

    :param PJ_CONTEXT* ctx: Threading context.
    :param void* app_data: Pointer to data structure used by the calling application.
    :param PJ_LOG_FUNCTION logf: Log function that overrides the PROJ log function.

    .. versionadded:: 5.1.0

Info functions
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: PJ_INFO proj_info(void)

    Get information about the current instance of the PROJ library.

    :returns: :c:type:`PJ_INFO`

.. c:function:: PJ_PROJ_INFO proj_pj_info(const PJ *P)

    Get information about a specific transformation object, :c:data:`P`.

    :param `P`: Transformation object
    :type `P`: const PJ*
    :returns: :c:type:`PJ_PROJ_INFO`

.. c:function:: PJ_GRID_INFO proj_grid_info(const char *gridname)

    Get information about a specific grid.

    :param `gridname`: Gridname in the PROJ searchpath
    :type `gridname`: const char*
    :returns: :c:type:`PJ_GRID_INFO`

.. c:function:: PJ_INIT_INFO proj_init_info(const char *initname)

    Get information about a specific init file.

    :param `initname`: Init file in the PROJ searchpath
    :type `initname`: const char*
    :returns: :c:type:`PJ_INIT_INFO`

Lists
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function::  const PJ_OPERATIONS* proj_list_operations(void)

    Get a pointer to an array of all operations in PROJ. The last entry
    of the returned array is a NULL-entry. The array is statically allocated
    and does not need to be freed after use.

    Print a list of all operations in PROJ:

    .. code-block:: C

        PJ_OPERATIONS *ops;
        for (ops = proj_list_operations(); ops->id; ++ops)
            printf("%s\n", ops->id);


    :returns: :c:type:`PJ_OPERATIONS*`

.. c:function:: const PJ_ELLPS* proj_list_ellps(void)

    Get a pointer to an array of ellipsoids defined in PROJ. The last entry
    of the returned array is a NULL-entry. The array is statically allocated
    and does not need to be freed after use.

    :returns: :c:type:`PJ_ELLPS*`

.. c:function:: const PJ_UNITS* proj_list_units(void)

    Get a pointer to an array of distance units defined in PROJ. The last
    entry of the returned array is a NULL-entry. The array is statically
    allocated and does not need to be freed after use.

    :returns: :c:type:`PJ_UNITS*`

.. c:function:: const PJ_PRIME_MERIDIANS* proj_list_prime_meridians(void)

    Get a pointer to an array of prime meridians defined in PROJ. The last
    entry of the returned array is a NULL-entry. The array is statically
    allocated and does not need to be freed after use.

    :returns: :c:type:`PJ_PRIME_MERIDIANS*`

Distances
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: double proj_lp_dist(const PJ *P, PJ_COORD a, PJ_COORD b)

    Calculate geodesic distance between two points in geodetic coordinates. The
    calculated distance is between the two points located on the ellipsoid.

    :param PJ* P: Transformation object
    :param PJ_COORD a: Coordinate of first point
    :param PJ_COORD b: Coordinate of second point
    :returns: :c:type:`double` Distance between :c:data:`a` and :c:data:`b` in meters.

.. c:function:: double proj_lpz_dist(const PJ *P, PJ_COORD a, PJ_COORD b)

    Calculate geodesic distance between two points in geodetic coordinates.
    Similar to :c:func:`proj_lp_dist` but also takes the height above the ellipsoid
    into account.

    :param PJ* P: Transformation object
    :param PJ_COORD a: Coordinate of first point
    :param PJ_COORD b: Coordinate of second point
    :returns: :c:type:`double` Distance between :c:data:`a` and :c:data:`b` in meters.

.. c:function:: double proj_xy_dist(PJ_COORD a, PJ_COORD b)

    Calculate 2-dimensional euclidean between two projected coordinates.

    :param PJ_COORD a: First coordinate
    :param PJ_COORD b: Second coordinate
    :returns: :c:type:`double` Distance between :c:data:`a` and :c:data:`b` in meters.

.. c:function:: double proj_xyz_dist(PJ_COORD a, PJ_COORD b)

    Calculate 3-dimensional euclidean between two projected coordinates.

    :param PJ_COORD a: First coordinate
    :param PJ_COORD b: Second coordinate
    :returns: :c:type:`double` Distance between :c:data:`a` and :c:data:`b` in meters.


Various
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: PJ_COORD proj_coord(double x, double y, double z, double t)

    Initializer for the :c:type:`PJ_COORD` union. The function is
    shorthand for the otherwise convoluted assignment.
    Equivalent to

    .. code-block:: C

        PJ_COORD c = {{10.0, 20.0, 30.0, 40.0}};

    or

    .. code-block:: C

        PJ_COORD c;
        // Assign using the PJ_XYZT struct in the union
        c.xyzt.x = 10.0;
        c.xyzt.y = 20.0;
        c.xyzt.z = 30.0;
        c.xyzt.t = 40.0;

    Since :c:type:`PJ_COORD` is a union of structs, the above assignment can
    also be expressed in terms of the other types in the union, e.g.
    :c:type:`PJ_UVWT` or :c:type:`PJ_LPZT`.


    :param double x: 1st component in a :c:type:`PJ_COORD`
    :param double y: 2nd component in a :c:type:`PJ_COORD`
    :param double z: 3rd component in a :c:type:`PJ_COORD`
    :param double t: 4th component in a :c:type:`PJ_COORD`
    :returns: :c:type:`PJ_COORD`


.. c:function:: double proj_roundtrip(PJ *P, PJ_DIRECTION direction, int n, PJ_COORD *coord)

    Measure internal consistency of a given transformation. The function
    performs :c:data:`n` round trip transformations starting in either
    the forward or reverse :c:data:`direction`. Returns the euclidean
    distance of the starting point :c:data:`coo` and the resulting
    coordinate after :c:data:`n` iterations back and forth.

    :param PJ* P:
    :type `P`: const PJ*
    :param `direction`: Starting direction of transformation
    :type `direction`: PJ_DIRECTION
    :param int n: Number of roundtrip transformations
    :param PJ_COORD coord: Input coordinate
    :returns: :c:type:`double` Distance between original coordinate and the \
              resulting coordinate after :c:data:`n` transformation iterations.

.. c:function:: PJ_FACTORS proj_factors(PJ *P, PJ_COORD lp)

    Calculate various cartographic properties, such as scale factors, angular
    distortion and meridian convergence. Depending on the underlying projection
    values will be calculated either numerically (default) or analytically.

    The function also calculates the partial derivatives of the given
    coordinate.

    :param `P`: Transformation object
    :type `P`: const PJ*
    :param `lp`: Geodetic coordinate
    :type `lp`: const PJ_COORD
    :returns: :c:type:`PJ_FACTORS`

.. c:function:: double proj_torad(double angle_in_degrees)

    Convert degrees to radians.

    :param double angle_in_degrees: Degrees
    :returns: :c:type:`double` Radians

.. c:function:: double proj_todeg(double angle_in_radians)

    Convert radians to degrees

    :param double angle_in_radians: Radians
    :returns: :c:type:`double` Degrees

.. c:function:: double proj_dmstor(const char *is, char **rs)

    Convert string of degrees, minutes and seconds to radians.
    Works similarly to the C standard library function :c:func:`strtod`.

    :param `is`: Value to be converted to radians
    :type `is`: const  char*
    :param `rs`: Reference to an already allocated char*, whose value is \
                 set by the function to the next character in :c:data:`is` \
                 after the numerical value.

.. c:function:: char *proj_rtodms(char *s, double r, int pos, int neg)

    Convert radians to string representation of degrees, minutes and seconds.

    :param char* s: Buffer that holds the output string
    :param double r: Value to convert to dms-representation
    :param int pos: Character denoting positive direction, typically `'N'` or `'E'`.
    :param int neg: Character denoting negative direction, typically `'S'` or `'W'`.
    :returns: :c:type:`char*` Pointer to output buffer (same as :c:data:`s`)


.. c:function:: int proj_angular_input (PJ *P, enum PJ_DIRECTION dir)

    Check if a operation expects input in radians or not.

    :param `P`: Transformation object
    :type `P`: const PJ*
    :param `direction`: Starting direction of transformation
    :type `direction`: PJ_DIRECTION
    :returns: :c:type:`int` 1 if input units is expected in radians, otherwise 0

.. c:function:: int proj_angular_output (PJ *P, enum PJ_DIRECTION dir)

    Check if an operation returns output in radians or not.

    :param `P`: Transformation object
    :type `P`: const PJ*
    :param `direction`: Starting direction of transformation
    :type `direction`: PJ_DIRECTION
    :returns: :c:type:`int` 1 if output units is expected in radians, otherwise 0


Setting custom I/O functions
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. versionadded:: 7.0.0

.. doxygenfunction:: proj_context_set_fileapi
   :project: doxygen_api

.. doxygenfunction:: proj_context_set_sqlite3_vfs_name
   :project: doxygen_api


Network related functionality
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. versionadded:: 7.0.0

.. doxygenfunction:: proj_context_set_network_callbacks
   :project: doxygen_api

.. doxygenfunction:: proj_context_set_enable_network
   :project: doxygen_api

.. doxygenfunction:: proj_context_is_network_enabled
   :project: doxygen_api

.. doxygenfunction:: proj_context_set_url_endpoint
   :project: doxygen_api

.. doxygenfunction:: proj_grid_cache_set_enable
   :project: doxygen_api

.. doxygenfunction:: proj_grid_cache_set_filename
   :project: doxygen_api

.. doxygenfunction:: proj_grid_cache_set_max_size
   :project: doxygen_api

.. doxygenfunction:: proj_grid_cache_set_ttl
   :project: doxygen_api

.. doxygenfunction:: proj_grid_cache_clear
   :project: doxygen_api

.. doxygenfunction:: proj_is_download_needed
   :project: doxygen_api

.. doxygenfunction:: proj_download_file
   :project: doxygen_api


Cleanup
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: void proj_cleanup()

    .. versionadded:: 6.2.0

    This function frees global resources (grids, cache of +init files). It
    should be called typically before process termination, and *after* having
    freed PJ and PJ_CONTEXT objects.


C API for ISO-19111 functionality
+++++++++++++++++++++++++++++++++

.. versionadded:: 6.0.0

The PJ* objects returned by :c:func:`proj_create_from_wkt`,
:c:func:`proj_create_from_database` and other functions in that section
will have generally minimal interaction with the functions declared in the
previous sections (calling those functions on those objects
will either return an error or default/non-sensical values). The exception is
for ISO19111 objects of type CoordinateOperation that can be exported as a
valid PROJ pipeline. In this case,  objects will work for example with
:c:func:`proj_trans_generic`.
Conversely, objects returned by :c:func:`proj_create` and :c:func:`proj_create_argv`,
which are not of type CRS (can be tested with :c:func:`proj_is_crs`),
will return an error when used with functions of this section.

.. doxygengroup:: iso19111_functions
   :project: doxygen_api
   :content-only:


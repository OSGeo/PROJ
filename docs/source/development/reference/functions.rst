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

    Deallacote a threading-context.

    :param PJ_CONTEXT* ctx: Threading context.

Transformation setup
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: PJ* proj_create(PJ_CONTEXT *ctx, const char *definition)

    Create a transformation object from a proj-string.

    Example call:

    .. code-block:: C

        PJ *P = proj_create(0, "+proj=etmerc +lat_0=38 +lon_0=125 +ellps=bessel");

    The returned :c:type:`PJ`-pointer should be deallocated with :c:func:`proj_destroy`.

    :param PJ_CONTEXT* ctx: Threading context.
    :param `definition`: Proj-string of the desired transformation.
    :type `definition`: const char*


.. c:function:: PJ* proj_create_argv(PJ_CONTEXT *ctx, int argc, char **argv)

    Create transformation object with argc/argv-style initialization. For this
    application each parameter in the defining proj-string is an entry in :c:data:`argv`.

    Example call:

    .. code-block:: C

        char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
        PJ* P = proj_create_argv(0, 3, args);

    The returned :c:type:`PJ`-pointer should be deallocated with :c:func:`proj_destroy`.

    :param PJ_CONTEXT* ctx: Threading context
    :param int argc: Count of arguments in :c:data:`argv`
    :param char** argv: Vector of strings with proj-string parameters, e.g. ``+proj=merc``
    :returns: :c:type:`PJ*`

.. c:function:: PJ* proj_create_crs_to_crs(PJ_CONTEXT *ctx, const char *srid_from, const char *srid_to)

    Create a transformation object that is a pipeline between two known
    coordinate reference systems.

    :c:data:`srid_from` and :c:data:`srid_to` should be the value part of a ``+init=...`` parameter
    set, i.e. "epsg:25833" or "IGNF:AMST63". Any projection definition that
    can be found in a init-file in :envvar:`PROJ_LIB` is a valid input to this function.

    For now the function mimics the cs2cs app: An input and an output CRS is
    given and coordinates are transformed via a hub datum (WGS84). This
    transformation strategy is referred to as "early-binding" by the EPSG. The
    function can be extended to support "late-binding" transformations in the
    future without affecting users of the function.

    Example call:

    .. code-block:: C

        PJ *P = proj_create_crs_to_crs(0, "epsg:25832", "epsg:25833");

    The returned :c:type:`PJ`-pointer should be deallocated with :c:func:`proj_destroy`.

    :param PJ_CONTEXT* ctx: Threading context.
    :param `srid_from`: Source SRID.
    :type `srid_from`: const char*
    :param `srid_to`: Destination SRID.
    :type `srid_to`: const char*
    :returns: :c:type:`PJ*`

.. c:function:: PJ* proj_destroy(PJ *P)

    Deallocate a :c:type:`PJ` transformation object.

    :param PJ* P:
    :returns: :c:type:`PJ*`

Coordinate transformation
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: PJ_COORD proj_trans_coord(PJ *P, enum proj_direction direction, PJ_COORD coord)

    Transform a single :c:type:`PJ_COORD` coordinate.

    :param PJ* P:
    :param `direction`: Transformation direction.
    :type `direction`: enum proj_direction
    :param PJ_COORD coord: Coordinate that will be transformed.
    :returns: :c:type:`PJ_COORD`


.. c:function:: PJ_OBS proj_trans_obs(PJ *P, enum proj_direction direction, PJ_OBS obs)

    Transform a single :c:type:`PJ_OBS` observation.

    :param PJ* P:
    :param `direction`: Transformation direction.
    :type `direction`: enum proj_direction
    :param PJ_OBS obs: Observation data to transform.
    :returns: :c:type:`PJ_OBS`



.. c:function:: size_t proj_transform(PJ *P, enum proj_direction direction, \
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

    The strides, :c:data:`sx`, :c:data:`sy`, :c:data:`sz`, :c:data:`st`,
    represent the step length, in bytes, between
    consecutive elements of the corresponding array. This makes it possible for
    :c:func:`proj_transform` to handle transformation of a large class of application
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

        proj_transform (
            P, PJ_INV, sizeof(XYQS),
            &(survey[0].x), stride, 345,  /*  We have 345 eastings  */
            &(survey[0].y), stride, 345,  /*  ...and 345 northings. */
            &height, 1,                   /*  The height is the constant  23.45 m */
            0, 0                          /*  and the time is the constant 0.00 s */
        );

    This is similar to the inner workings of the deprecated pj_transform function, but the
    stride functionality has been generalized to work for any size of basic unit,
    not just a fixed number of doubles.

    In most cases, the stride will be identical for x, y, z, and t, since they will
    typically be either individual arrays (stride = sizeof(double)), or strided
    views into an array of application specific data structures (stride = sizeof (...)).

    But in order to support cases where :c:data:`x`, :c:data:`y`, :c:data:`z`,
    and :c:data:`t` come from heterogeneous sources, individual strides,
    :c:data:`sx`, :c:data:`sy`, :c:data:`sz`, :c:data:`st`, are used.

    .. note:: Since :c:func:`proj_transform` does its work *in place*, this means that even the
              supposedly constants (i.e. length 1 arrays) will return from the call in altered
              state. Hence, remember to reinitialize between repeated calls.

    :param PJ* P: Transformation object
    :param `direction`: Transformation direction
    :type `enum proj_direction`:
    :param double* x: Array of x-coordinates
    :param double* y: Array of y-coordinates
    :param double* z: Array of z-coordinates
    :param double* t: Array of t-coordinates
    :param size_t sx: Step lenght, in bytes, between consecutive elements of the corresponding array
    :param size_t nx: Number of elements in the corresponding array
    :param size_t sy: Step lenght, in bytes, between consecutive elements of the corresponding array
    :param size_t nv: Number of elements in the corresponding array
    :param size_t sz: Step lenght, in bytes, between consecutive elements of the corresponding array
    :param size_t nz: Number of elements in the corresponding array
    :param size_t st: Step lenght, in bytes, between consecutive elements of the corresponding array
    :param size_t nt: Number of elements in the corresponding array
    :returns: Number of transformations succesfully completed



.. c:function:: size_t proj_transform_coord(PJ *P, enum proj_direction direction, size_t n, PJ_COORD *coord)

    Batch transform an array of :c:type:`PJ_COORD`.

    :param PJ* P:
    :param `direction`: Transformation direction
    :type `direction`: enum proj_direction
    :param size_t n: Number of cordinates in :c:data:`coord`
    :returns: :c:type:`size_t` 0 if all observations are transformed without error, otherwise returns error number

.. c:function:: size_t proj_transform_obs(PJ *P, enum proj_direction direction, size_t n, PJ_OBS *obs)

    Batch transform an array of :c:type:`PJ_OBS`.

    :param PJ* P:
    :param `direction`: Transformation direction
    :type `direction`: enum proj_direction
    :param size_t n: Number of observations in :c:data:`obs`
    :returns: :c:type:`size_t` 0 if all observations are transformed without error, otherwise returns error number


Initializers
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

.. c:function:: PJ_OBS proj_obs(double x, double y, double z, double t,\
                                double o, double p, double k,\
                                int id, unsigned int flags)

    Initializer for the :c:type:`PJ_OBS` union. The function is
    shorthand for the otherwise convoluted assignment.
    Equivalent to

    .. code-block:: C

        PJ_OBS c = {{{1.0, 2.0, 3.0, 4.0}}, {{5.0, 6.0, 7.0}}, 8, 9};

    or

    .. code-block:: C

        PJ_OBS c;
        // Assign using the PJ_COORD part of the struct in the union
        o.coo.v[0] = 1.0;
        o.coo.v[1] = 2.0;
        o.coo.v[2] = 3.0;
        o.coo.v[3] = 4.0;
        o.anc.v[0] = 5.0;
        o.anc.v[1] = 6.0;
        o.anc.v[2] = 7.0;
        o.id       = 8;
        o.flags    = 9;

    which is a bit too verbose in most practical applications.

    :param double x: 1st component in a :c:type:`PJ_COORD`
    :param double y: 2nd component in a :c:type:`PJ_COORD`
    :param double z: 3rd component in a :c:type:`PJ_COORD`
    :param double t: 4th component in a :c:type:`PJ_COORD`
    :param double o: 1st component in a :c:type:`PJ_TRIPLET`
    :param double p: 2nd component in a :c:type:`PJ_TRIPLET`
    :param double k: 3rd component in a :c:type:`PJ_TRIPLET`
    :param int id: Ancillary data, e.g. an ID
    :param `flags`: Flags
    :type `flags`: unsigned int
    :returns: :c:type:`PJ_OBS`

Error reporting
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: int proj_errno(PJ *P)

    Get a reading of the current error-state of :c:data:`P`. An non-zero error
    codes indicates an error either with the transformation setup or during a
    transformation.

    :param: PJ* P: Transformation object.

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



Info functions
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: PJ_INFO proj_info(void)

    Get information about the current instance of the PROJ.4 library.

    :returns: :c:type:`PJ_INFO`

.. c:function:: PJ_PROJ_INFO proj_pj_info(const PJ *P)

    Get information about a specific transformation object, :c:data:`P`.

    :param `P`: Transformation object
    :type `P`: const PJ*
    :returns: :c:type:`PJ_PROJ_INFO`

.. c:function:: PJ_GRID_INFO proj_grid_info(const char *gridname)

    Get information about a specific grid.

    :param `gridname`: Gridname in the PROJ.4 searchpath
    :type `gridname`: const char*
    :returns: :c:type:`PJ_GRID_INFO`

.. c:function:: PJ_INIT_INFO proj_init_info(const char *initname)

    Get information about a specific init file.

    :param `initname`: Init file in the PROJ.4 searchpath
    :type `initname`: const char*
    :returns: :c:type:`PJ_INIT_INFO`


Distances
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: double proj_lp_dist(PJ *P, LP a, LP b)

    Calculate geodesic distance between two points in geodetic coordinates.

    :param PJ* P: Transformation object
    :param LP a: Coordinate of first point
    :param LP b: Coordinate of second point
    :returns: :c:type:`double` Distance between :c:data:`a` and :c:data:`b` in meters.

.. c:function:: double proj_xy_dist(XY a, XY, b)

    Calculate 2-dimensional euclidean between two projected coordinates.

    :param XY a: First coordinate
    :param XY b: Second coordinate
    :returns: :c:type:`double` Distance between :c:data:`a` and :c:data:`b` in meters.

.. c:function:: double proj_xyz_dist(XYZ a, XYZ b)

    Calculate 3-dimensional euclidean between two projected coordinates.

    :param XYZ a: First coordinate
    :param XYZ b: Second coordinate
    :returns: :c:type:`double` Distance between :c:data:`a` and :c:data:`b` in meters.


Various
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: double proj_roundtrip(PJ *P, enum proj_direction direction, int n, PJ_OBS obs)

    Measure internal consistency of a given transformation. The function
    performs :c:data:`n` round trip transformations starting in either
    the forward or reverse :c:data:`direction`. Returns the euclidean
    distance of the starting point :c:data:`obs` and the resulting
    coordinate after :c:data:`n` iterations back and forth.

    :param PJ* P:
    :param `direction`: Starting direction of transformation
    :type `direction`: enum proj_direction
    :param int n: Number of roundtrip transformations
    :param PJ_OBS obs: Input coordinate
    :returns: :c:type:`double` Distance between original coordinate and the \
              resulting coordinate after :c:data:`n` transformation iterations.

.. c:function:: PJ_DERIVS proj_derivatives(const PJ *P, const LP lp)

    Calculate partial derivatives of geodetic coordinates.

    :param `P`: Transformation object
    :type `P`: const PJ*
    :param `lp`: Geodetic coordinate
    :type `lp`: const LP
    :returns: :c:type:`PJ_DERIVS`

.. c:function:: PJ_FACTORS proj_factors(const PJ *P, const  LP lp)

    Calculate various cartographic properties, such as scale factors, angular
    distortion and meridian convergence. Depending on the underlying projection
    values will be calculated either numerically (default) or analytically.

    The function also calculates the partial derivatives of the given
    coordinate.

    :param `P`: Transformation object
    :type `P`: const PJ*
    :param `lp`: Geodetic coordinate
    :type `lp`: const LP
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

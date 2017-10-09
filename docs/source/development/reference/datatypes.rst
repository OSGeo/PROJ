.. _datatypes:

================================================================================
Data types
================================================================================

This section describe the multiplude of data types in use in PROJ.4. As a rule
of thumb PROJ.4 data types are prefixed with ``PJ_``, or in one particular case,
is simply called :c:type:`PJ`. A few notable exceptions can be traced
back to the very early days of PROJ.4 when the ``PJ_`` prefix was not
consistenly used.

Transformation objects
--------------------------------------------------------------------------------

.. c:type:: PJ

    Object containing everything related to a given projection or transformation.
    As a user of the PROJ.4 library your are only exposed to pointers to this
    object and the contents are hidden in the public API. :c:type:`PJ` objects
    are created with :c:func:`proj_create` and destroyed with
    :c:func:`proj_destroy`.

.. c:type:: PJ_DIRECTION

    Enumeration that is used to convey in which direction a given transformation
    should be performed. Used in transformation function call as described in
    the section on :ref:`transformation functions <coord_trans_functions>`.

    Forward transformations are defined with the :c:

    .. code-block:: C

        typedef enum proj_direction {
            PJ_FWD   =  1,   /* Forward    */
            PJ_IDENT =  0,   /* Do nothing */
            PJ_INV   = -1    /* Inverse    */
        } PJ_DIRECTION;

    .. c:member:: PJ_FWD

        Perform transformation in the forward direction.

    .. c:member:: PJ_IDENT

        Identity. Do nothing.

    .. c:member:: PJ_INV

        Perform transformation in the inverse direction.

.. c:type:: PJ_CONTEXT

    Context objects enables safe multi-threaded usage of PROJ.4. Each :c:type:`PJ`
    object is connected to a context (if not specified, the default context is
    used). All operations within a context should be performed in the same thread.
    :c:type:`PJ_CONTEXT` objects are created with :c:func:`proj_context_create`
    and destroyed with :c:func:`proj_context_destroy`.


2 dimensional coordinates
--------------------------------------------------------------------------------

Various 2-dimensional coordinate data types.

.. c:type:: LP

    Geodetic coordinate, latitude and longitude. Usually in radians.

    .. code-block:: C

        typedef struct { double lam, phi; } LP;

    .. c:member:: double LP.lam

        Longitude. Lambda.

    .. c:member:: double LP.phi

        Latitude. Phi.


.. c:type:: XY

    2-dimensional cartesian coordinate.

    .. code-block:: C

        typedef struct { double x, y; } XY;


    .. c:member:: double XY.lam

        Easting.

    .. c:member:: double XY.phi

        Northing.


.. c:type:: UV

    2-dimensional generic coordinate. Usually used when contents can be either
    a :c:type:`XY` or :c:type:`UV`.

    .. code-block:: C

        typedef struct {double u, v; } UV;


    .. c:member:: double UV.u

        Longitude or easting, depending on use.

    .. c:member:: double UV.v

        Latitude or northing, depending on use.


.. c:type:: PJ_EN

    Generic easting/northing coordinate.

    .. code-block:: C

        typedef struct { double e, n; } PJ_EN;

    .. c:member:: double PJ_EN.e

        Easting

    .. c:member:: double PJ_EN.n

        Northing

3 dimensional coordinates
--------------------------------------------------------------------------------

The following data types are the 3-dimensional equivalents to the data
types above.

.. c:type:: LPZ

    3-dimensional version of :c:type:`LP`. Holds longitude, latitude and
    vertical component.

    .. code-block:: C

        typedef struct { double lam, phi, z; } LPZ;

    .. c:member:: double LPZ.lam

        Longitude. Lambda.

    .. c:member:: double LPZ.phi

        Latitude. Phi.

    .. c:member:: double LPZ.z

        Vertical component.


.. c:type:: XYZ

    Cartesian coordinate in 3 dimensions. Extension  of :c:type:`XY`.

    .. code-block:: C

        typedef struct { double x, y, z; } XYZ;

    .. c:member:: double XYZ.x

        Easting.

    .. c:member:: double XYZ.y

        Northing.

    .. c:member:: double XYZ.z

        Vertical component.


.. c:type:: UVW

    3-dimensional extension of :c:type:`UV`.

    .. code-block:: C

        typedef struct {double u, v, w; } UVW;

    .. c:member:: double UVW.u

        Longitude or easting, depending on use.

    .. c:member:: double UVW.v

        Latitude or northing, depending on use.

    .. c:member:: double UVW.w

        Vertical component.


.. c:type:: PJ_ENH

    Easting, northing and height.

    .. code-block:: C

        typedef struct {double e, n, h; } PJ_ENH;

    .. c:member:: double PJ_ENH.e

        Easting

    .. c:member:: double PJ_ENH.n

        Northing

    .. c:member:: double PJ_ENH.h

        Height


Spatiotemporal coordinate types
--------------------------------------------------------------------------------

The following data types are extensions of the triplets above into the time
domain.


.. c:type:: PJ_LPZT

    Spatiotemporal version of :c:type:`LPZ`.

    .. code-block:: C

        typedef struct {
            double lam;
            double phi;
            double z;
            double t;
        } PJ_LPZT;


    .. c:member:: double PJ_LPZT.lam

        Longitude.

    .. c:member:: double PJ_LPZT.phi

        Latitude

    .. c:member:: double PJ_LPZT.z

        Vertical component.

    .. c:member:: double PJ_LPZT.t

        Time component.

.. c:type:: PJ_XYZT

    Generic spatiotemporal coordinate. Usefull for e.g. cartesian coordinates with
    an attached time-stamp.

    .. code-block:: C

        typedef struct {
            double x;
            double y;
            double z;
            double t;
        } PJ_XYZT;


    .. c:member:: double PJ_XYZT.x

        Easting.

    .. c:member:: double PJ_XYZT.y

        Northing.

    .. c:member:: double PJ_XYZT.z

        Vertical component.

    .. c:member:: double PJ_XYZT.t

        Time component.


.. c:type:: PJ_UVWT

    Spatiotemporal version of :c:type:`PJ_UVW`.

    .. code-block:: C

        typedef struct { double u, v, w, t; } PJ_UVWT;

    .. c:member:: double PJ_UVWT.e

        First horizontal component.

    .. c:member:: double PJ_UVWT.n

        Second horizontal component.

    .. c:member:: double PJ_UVWT.w

        Vertical component.

    .. c:member:: double PJ_UVWT.t

        Temporal component.



.. c:type:: PJ_ENHT

    Spatiotemporal version of :c:type:`PJ_ENH`.

    .. code-block:: C

        typedef struct {double e, n, h, t; } PJ_ENHT;

    .. c:member:: double PJ_ENHT.e

        Easting

    .. c:member:: double PJ_ENHT.n

        Northing

    .. c:member:: double PJ_ENHT.h

        Height

    .. c:member:: double PJ_ENHT.t

Ancillary types for geodetic computations
--------------------------------------------------------------------------------


.. c:type:: PJ_OPK

    Rotations, for instance three euler angles.

    .. code-block:: C

        typedef struct { double o, p, k; } PJ_OPK;

    .. c:member:: double PJ_OPK.o

        First rotation angle, omega.

    .. c:member:: double PJ_OPK.p

        Second rotation angle, phi.

    .. c:member:: double PJ_OPK.k

        Third rotation angle, kappa.


.. c:type:: PJ_DMS

    Describe a longitude or latitude by degrees, minutes and seconds.

    .. code-block:: C

        typedef struct { double d, m, s; } PJ_DMS;

    .. c:member:: double PJ_DMS.d

        Degrees.

    .. c:member:: double PJ_DMS.m

        Minutes

    .. c:member:: double PJ_DMS.s

        Seconds.


.. c:type:: PJ_EZN

    Geoid undulation and deflections of the vertical.

    .. code-block:: C

        typedef struct { double e, z, N; } PJ_EZN;

    .. c:member:: double PJ_EZN.e

        Deflection of the vertical, eta.

    .. c:member:: double PJ_EZN.z

        Deflection of the vertical, zeta

    .. c:member:: double PJ_EZN.N

        Geoid undulation.


.. c:type:: PJ_AF

    Ellipsoidal parameters.

    .. code-block:: C

        typedef struct {double a, f; } PJ_AF;

    .. c:member:: double PJ_AF.a

        Major axis of ellipsoid.

    .. c:member:: double PJ_AF.f

        Flattening of ellipsoid.


Complex coordinate types
--------------------------------------------------------------------------------

.. c:type:: PJ_PAIR

    .. code-block:: C

        typedef union {
            XY     xy;
            LP     lp;
            UV     uv;
            PJ_AF  af;
            PJ_EN  en;
            double v[2];
        } PJ_PAIR;

    .. c:member:: XY PJ_PAIR.xy

        Coordinate in projected space.

    .. c:member:: LP PJ_PAIR.lp

        Coordinate in lat/long space.

    .. c:member:: UV PJ_PAIR.uv

        Coordinate either in lat/lon or projected space.

    .. c:member:: PJ_AF PJ_PAIR.af;

        Ellipsoidal parameters.

    .. c:member:: PJ_EN PJ_PAIR.en

        Easting/Northing pair.

    .. c:member:: double PJ_PAIR.v[2]

        Generic 2D-vector.

.. c:type:: PJ_TRIPLET

    Union type that groups all coordinate data types of two and tree dimensions.

    .. code-block:: C

        typedef union {
            PJ_OPK  opk;
            PJ_ENH  enh;
            PJ_EZN  ezn;
            PJ_DMS  dms;
            double v[3];
            XYZ    xyz;
            LPZ    lpz;
            UVW    uvw;
            XY     xy;
            LP     lp;
            UV     uv;
            PJ_AF  af;
        } PJ_TRIPLET;

    .. c:member:: PJ_OPK PJ_TRIPLET.opk

        Rotations.

    .. c:member:: PJ_ENH PJ_TRIPLET.enh

        Easting, northing and height.

    .. c:member:: PJ_EZN PJ_TRIPLET.ezn

        Geoid undulation and deflections of the vertical.

    .. c:member:: PJ_DMS PJ_TRIPLET.dsm

        Degrees, minutes and seconds.

    .. c:member:: double PJ_TRIPLET.v[3]

        Generic 3-dimensional vector.

    .. c:member:: XYZ PJ_TRIPLET.xyz

        Coordinates in projected space.

    .. c:member:: LPZ PJ_TRIPLET.lpz

        Geodetic coordinates, including height.

    .. c:member:: UVW PJ_TRIPLET.uvw

        Either geodetic or projected coordinates.

    .. c:member:: XY PJ_TRIPLET.xy

        Horizontal coordinates in projected space.

    .. c:member:: LP PJ_TRIPLET.lp

        Geodetic coordinates.

    .. c:member:: UV PJ_TRIPLET.uv

        Geodetic or projected coordinate.

    .. c:member:: PJ_AF PJ_TRIPLET.af

        Ellipsoidal paramaters.

.. c:type:: PJ_COORD

    General purpose coordinate union type usefull in two, three and four dimensions.

    .. code-block:: C

        typedef union {
            PJ_XYZT xyzt;
            PJ_UVWT uvwt;
            PJ_ENHT enht;
            PJ_LPZT lpzt;
            PJ_ENH  enh;
            PJ_EN   en;
            double v[4];
            XYZ  xyz;
            UVW  uvw;
            LPZ  lpz;
            XY   xy;
            UV   uv;
            LP   lp;
        } PJ_COORD ;

    .. c:member:: PJ_XYZT PJ_COORD.xyzt

        Spatiotemporal cartesian coordinate.

    .. c:member:: PJ_UVWT PJ_COORD.uvwt

        Spatiotemporal generic coordinate.

    .. c:member:: PJ_ENHT PJ_COORD.enht

        Easting, northing, height and time.

    .. c:member:: PJ_LPZT PJ_COORD.lpzt

        Longitude, latitude, vertical and time components.

    .. c:member:: PJ_ENH PJ_COORD.enh

        Easting, northing and height

    .. c:member:: PJ_EN PJCOORD.en

        Easting and  northing.

    .. c:member:: double v[4]

        Generic four-dimensional vector.

    .. c:member:: XYZ PJ_COORD.xyz

        3-dimensional cartesian coordinate.

    .. c:member:: UVW PJ_COORD.uvw

        3-dimensional generic coordinate.

    .. c:member:: LPZ PJ_COORD.lpz

        Longitude, latitude and vertical component.

    .. c:member:: XY PJ_COORD.xy

        2-dimensional cartesian coordinate.

    .. c:member:: UV PJ_COORD.uv

        2-dimensional generic coordinate.

    .. c:member:: LP PJ_COORD.lp

        Longitude and latitude.


.. c:type:: PJ_OBS

    Geodetic observation data type.

    .. code-block:: C

        typedef struct {
            PJ_COORD coo;
            PJ_TRIPLET anc;
            int id;
            unsigned int flags;
        } PJ_OBS;

    .. c:member:: PJ_COORD PJ_OBS.coo

        Coordinate data

    .. c:member:: PJ_TRIPLET PJ_OBS.anc

        Ancillary data

    .. c:member:: int PJ_OBS.id

        Integer ancillary data, e.g. observation number, EPSG code, etc.

    .. c:member:: unsigned int PJ_OBS.flags

        Additional data intended for flags.


Projection derivatives
-------------------------------------------------------------------------------

.. c:type:: PJ_DERIVS

    Partial derivatives of geodetic coordinate :math:`\left(\lambda,\phi\right)`.
    Calculated with :c:func:`proj_derivatives`.

    .. code-block:: C

        typedef struct {
            double x_l, x_p;
            double y_l, y_p;
        } PJ_DERIVS;

    .. c:member:: double PJ_DERIVS.x_l

        :math:`\frac{\partial x}{\partial \lambda}`

    .. c:member:: double PJ_DERIVS.x_p

        :math:`\frac{\partial x}{\partial \phi}`

    .. c:member:: double PJ_DERIVS.y_l

        :math:`\frac{\partial y}{\partial \lambda}`

    .. c:member:: double PJ_DERIVS.y_p

        :math:`\frac{\partial y}{\partial \phi}`


.. c:type:: PJ_FACTORS

    Various cartographic properties, such as scale factors, angular distortion
    and meridian convergence. Calculated with :c:func:`proj_factors`. Depending
    on the underlying projection, values can be calculated either numerically
    or analytically.

    .. code-block:: C

        typedef struct {
            struct PJ_DERIVS der;
            double h, k;
            double omega, thetap;
            double conv;
            double s;
            double a, b;
            int code;
        } PJ_FACTORS;

    .. c:member:: PJ_DERIVS PJ_FACTORS.der

        Partial derivatives of coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.h

        Meridian scale at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.k

        Parallel scale at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.omega

        Angular distortion at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.thetap

         Meridian/parallel angle, :math:`\theta^\prime`, at coordinate :math:`\left(\lambda,\phi\right)`.

     .. c:member:: double PJ_FACTORS.conv

        Meridian convergence at coordinate :math:`\left(\lambda,\phi\right)`.
        Sometimes also described as *grid declination*.

    .. c:member:: double PJ_FACTORS.s

        Areal scale factor at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.a

        Maximum scale error.

    .. c:member:: double PJ_FACTORS.b

        Minimum scale error.

    .. c:member:: int code

        Bitmask determing if calculation of various factors was done numerically
        or analytically. If a bit flags is set the calculation was done analytically.
        The following bit flags exists:

        .. c:macro:: PJ_IS_ANAL_XL_YL

            Longitude derivatives are calculated analytically

        .. c:macro:: PJ_IS_ANAL_XP_YP

            Latitude derivatives are calculated analyticall.

        .. c:macro:: PJ_IS_ANAL_HK

            Meridinal and parallel scale factors are calculated analytically.

        .. c:macro:: PJ_IS_ANAL_CONV

            Meridian convergence calculated analytically.


Info structures
-------------------------------------------------------------------------------

.. c:type:: PJ_INFO

    Struct holding information about the current instance of PROJ.4. Struct is
    populated by :c:func:`proj_info`.

    .. code-block:: C

        typedef struct {
            char        release[64];
            char        version[64];
            int         major;
            int         minor;
            int         patch;
            char        searchpath[512];
        } PJ_INFO;

    .. c:member:: char PJ_INFO.release[64]

        Release info. Version number and release date,
        e.g. "Rel. 4.9.3, 15 August 2016".

    .. c:member:: char PJ_INFO.version[64]

        Text representation of the full version number,
        e.g. "4.9.3".

    .. c:member:: int PJ_INFO.major

        Major version number.

    .. c:member:: int PJ_INFO.minor

        Minor version number.

    .. c:member:: int PJ_INFO.patch

        Patch level of release.

    .. c:member:: char PJ_INFO.searchpath[512]

        Search path for PROJ.4. List of directories separated by
        semicolons,  e.g. "C:\Users\doctorwho;C:\OSGeo4W64\\share\proj".
        Grids and init files are looked for in directories in the search path.

.. c:type:: PJ_PROJ_INFO

    Struct holding information about a :c:type:`PJ` object. Populated by
    :c:func:`proj_pj_info`.

    .. code-block:: C

        typedef struct {
            char        id[16];
            char        description[128];
            char        definition[512];
            int         has_inverse;
        } PJ_PROJ_INFO;

    .. c:member:: char PJ_PROJ_INFO.id[16]

        Short ID of the operation the :c:type:`PJ` object is based on, that is,
        what comes afther the ``+proj=`` in a proj-string, e.g. "*merc*".

    .. c:member:: char PJ_PROJ_INFO.description[128]

        Long describes of the operation the :c:type:`PJ` object is based on,
        e.g. "*Mercator    Cyl, Sph&Ell   lat_ts=*".

    .. c:member:: char PJ_PROJ_INFO.definition[512]

        The proj-string that was used to create the :c:type:`PJ` object with,
        e.g. "*+proj=merc +lat_0=24 +lon_0=53 +ellps=WGS84*".

    .. c:member:: int PJ_PROJ_INFO.has_inverse

        1 if an inverse mapping of the defined operation exists, otherwise 0.

.. c:type:: PJ_GRID_INFO

    Struct holding information about a specific grid in the search path of
    PROJ.4. Populated with the function :c:func:`proj_grid_info`.

    .. code-block:: C

        typedef struct {
            char        gridname[32];
            char        filename[260];
            char        format[8];
            LP          lowerleft;
            LP          upperright;
            int         n_lon, n_lat;
            double      cs_lon, cs_lat;
        } PJ_GRID_INFO;

    .. c:member:: char PJ_GRID_INFO.gridname[32]

        Name of grid, e.g. "*BETA2007.gsb*".

    .. c:member:: char PJ_GRID_INFO

        Full path of grid file, e.g. "*C:\OSGeo4W64\\share\proj\BETA2007.gsb*"

    .. c:member:: char PJ_GRID_INFO.format[8]

        File format of grid file, e.g. "*ntv2*"

    .. c:member:: LP PJ_GRID_INFO.lowerleft

        Geodetic coordinate of lower left corner of grid.

    .. c:member:: LP PJ_GRID_INFO.upperright

        Geodetic coordinate of upper right corner of grid.

    .. c:member:: int PJ_GRID_INFO.n_lon

        Number of grid cells in the longitudinal direction.

    .. c:member:: int PJ_GRID_INFO.n_lat

        Number of grid cells in the latitudianl direction.

    .. c:member:: double PJ_GRID_INFO.cs_lon

        Cell size in the longitudinal direction. In radians.

    .. c:member:: double PJ_GRID_INFO.cs_lat

        Cell size in the latitudinal direction. In radians.


.. c:type:: PJ_INIT_INFO

    Struct holding information about a specific init file in the search path of
    PROJ.4. Populated with the function :c:func:`proj_init_info`.

    .. code-block:: C

        typedef struct {
            char        name[32];
            char        filename[260];
            char        version[32];
            char        origin[32];
            char        lastupdate[16];
        } PJ_INIT_INFO;

    .. c:member:: char PJ_INIT_INFO.name[32]

        Name of init file, e.g. "*epsg*".

    .. c:member:: char PJ_INIT_INFO.filename[260]

        Full path of init file, e.g. "*C:\OSGeo4W64\share\proj\epsg*"

    .. c:member:: char PJ_INIT_INFO.version[32]

        Version number of init-file, e.g. "*9.0.0*"

    .. c:member:: char PJ_INIT_INFO.origin[32]

        Originating entity of the init file, e.g. "*EPSG*"

    .. c:member:: char PJ_INIT_INFO.lastupdate

        Date of last update of the init-file.

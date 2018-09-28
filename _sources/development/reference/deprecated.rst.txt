.. _api:

********************************************************************************
Deprecated API
********************************************************************************

.. contents:: Contents
   :depth: 3
   :backlinks: none

Introduction
------------

Procedure ``pj_init()`` selects and initializes a cartographic
projection with its argument control parameters. ``argc`` is the number
of elements in the array of control strings argv that each contain
individual cartographic control keyword assignments (+ proj arguments).
The list must contain at least the proj=projection and Earth’s radius or
elliptical parameters. If the initialization of the projection is
successful a valid address is returned otherwise a NULL value.

The ``pj_init_plus()`` function operates similarly to ``pj_init()`` but
takes a single string containing the definition, with each parameter
prefixed with a plus sign. For example
``+proj=utm +zone=11 +ellps=WGS84``.

Once initialization is performed either forward or inverse projections
can be performed with the returned value of ``pj_init()`` used as the
argument proj. The argument structure projUV values u and v contain
respective longitude and latitude or x and y. Latitude and longitude are
in radians. If a projection operation fails, both elements of projUV are
set to ``HUGE_VAL`` (defined in ``math.h``).

Note: all projections have a forward mode, but some do not have an
inverse projection. If the projection does not have an inverse the
projPJ structure element inv will be ``NULL``.

The ``pj_transform`` function may be used to transform points between
the two provided coordinate systems. In addition to converting between
cartographic projection coordinates and geographic coordinates, this
function also takes care of datum shifts if possible between the source
and destination coordinate system. Unlike ``pj_fwd()`` and ``pj_inv()``
it is also allowable for the coordinate system definitions
``(projPJ *)`` to be geographic coordinate systems (defined as
``+proj=latlong``).
The x, y and z arrays contain the input values of the points, and are replaced with the output values.
The function returns zero on success, or the error number (also in ``pj_errno``)
on failure.

Memory associated with the projection may be freed with ``pj_free()``.

Example
-------

The following program reads latitude and longitude values in decimal
degrees, performs Mercator projection with a Clarke 1866 ellipsoid and a
33° latitude of true scale and prints the projected cartesian values in
meters:

.. code::

    #include <proj_api.h>

    main(int argc, char **argv) {
         projPJ pj_merc, pj_latlong;
         double x, y;

         if (!(pj_merc = pj_init_plus("+proj=merc +ellps=clrk66 +lat_ts=33")) )
            exit(1);
         if (!(pj_latlong = pj_init_plus("+proj=latlong +ellps=clrk66")) )
            exit(1);
         while (scanf("%lf %lf", &x, &y) == 2) {
            x *= DEG_TO_RAD;
            y *= DEG_TO_RAD;
            p = pj_transform(pj_latlong, pj_merc, 1, 1, &x, &y, NULL );
            printf("%.2f\t%.2f\n", x, y);
         }
         exit(0);
    }


For this program, an input of ``-16 20.25`` would give a result of
``-1495284.21 1920596.79``.

API Functions
-------------

pj_transform
============

::

    int pj_transform( projPJ srcdefn,
                      projPJ dstdefn,
                      long point_count,
                      int point_offset,
                      double *x,
                      double *y,
                      double *z );


Transform the x/y/z points from the source coordinate system to the
destination coordinate system.

``srcdefn``: source (input) coordinate system.

``dstdefn``: destination (output) coordinate system.

``point_count``: the number of points to be processed (the size of the
x/y/z arrays).

``point_offset``: the step size from value to value (measured in
doubles) within the x/y/z arrays - normally 1 for a packed array. May be
used to operate on xyz interleaved point arrays.

``x``/``y``/``z``: The array of X, Y and Z coordinate values passed as
input, and modified in place for output. The Z may optionally be NULL.

``return``: The return is zero on success, or a PROJ.4 error code.

The ``pj_transform()`` function transforms the passed in list of points
from the source coordinate system to the destination coordinate system.
Note that geographic locations need to be passed in radians, not decimal
degrees, and will be returned similarly. The ``z`` array may be passed
as NULL if Z values are not available.

If there is an overall failure, an error code will be returned from the
function. If individual points fail to transform - for instance due to
being over the horizon - then those x/y/z values will be set to
``HUGE_VAL`` on return. Input values that are ``HUGE_VAL`` will not be
transformed.


pj_init_plus
============

::

   projPJ pj_init_plus(const char *definition);

This function converts a string representation of a coordinate system
definition into a projPJ object suitable for use with other API
functions. On failure the function will return NULL and set pj_errno.
The definition should be of the general form
``+proj=tmerc +lon_0 +datum=WGS84``. Refer to PROJ.4 documentation and
the :ref:`transformation` notes for additional detail.

Coordinate system objects allocated with ``pj_init_plus()`` should be
deallocated with ``pj_free()``.


pj_free
=======

::

    void pj_free( projPJ pj );

Frees all resources associated with pj.


pj_is_latlong
=============

::

    int pj_is_latlong( projPJ pj );

Returns TRUE if the passed coordinate system is geographic
(``proj=latlong``).


pj_is_geocent
=============

::

    int pj_is_geocent( projPJ pj );``

Returns TRUE if the coordinate system is geocentric (``proj=geocent``).

pj_get_def
==========

::

    char *pj_get_def( projPJ pj, int options);``

Returns the PROJ.4 initialization string suitable for use with
``pj_init_plus()`` that would produce this coordinate system, but with
the definition expanded as much as possible (for instance ``+init=`` and
``+datum=`` definitions).

pj_latlong_from_proj
====================

::

    projPJ pj_latlong_from_proj( projPJ pj_in );``

Returns a new coordinate system definition which is the geographic
coordinate (lat/long) system underlying ``pj_in``.

pj_set_finder
==============

::

    void pj_set_finder( const char *(*new_finder)(const char *) );``

Install a custom function for finding init and grid shift files.

pj_set_searchpath
=================

::

    void pj_set_searchpath ( int count, const char **path );``

Set a list of directories to search for init and grid shift files.


pj_deallocate_grids
===================

::

    void pj_deallocate_grids( void );``

Frees all resources associated with loaded and cached datum shift grids.


pj_strerrno
===========

::

    char *pj_strerrno( int );``

Returns the error text associated with the passed in error code.

pj_get_errno_ref
================

::

    int *pj_get_errno_ref( void );``

Returns a pointer to the global pj\_errno error variable.

pj_get_release
==============

::

    const char *pj_get_release( void );``

Returns an internal string describing the release version.

Obsolete Functions
~~~~~~~~~~~~~~~~~~

``XY pj_fwd( LP lp, PJ *P );``

``LP pj_inv( XY xy, PJ *P );``

``projPJ pj_init(int argc, char **argv);``

.. _more info: pj_transform


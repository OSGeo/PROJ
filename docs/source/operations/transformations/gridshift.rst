.. _gridshift:

================================================================================
General grid shift
================================================================================

.. versionadded:: 9.2.0

Translation of geodetic coordinates using a grid shift.

+-----------------+-------------------------------------------------------------------+
| **Alias**       | gridshift                                                         |
+-----------------+-------------------------------------------------------------------+
| **Domain**      | 2D and 3D                                                         |
+-----------------+-------------------------------------------------------------------+
| **Input type**  | Geodetic coordinates (horizontal), meters (vertical)              |
+-----------------+-------------------------------------------------------------------+
| **Output type** | Geodetic coordinates (horizontal), meters (vertical)              |
+-----------------+-------------------------------------------------------------------+

The transformation may apply horizontal geodetic offsetting and/or vertical
(ellipsoidal or orthometric height) offsetting, depending on the type of the
grid(s).

This is a generalization of the :ref:`hgridshift` and :ref:`vgridshift` methods,
that may be used in particular for US NADCON5 grids that contain both horizontal
geodetic and ellipsoidal height offsets.


Example
-------------------------------------------------------------------------------

Transformation of a point from NAD83(NSRS2007) to NAD83(2011) on conterminous USA
using a NADCON5 grid that contains horizontal and ellipsoidal height offsets.

::

    +proj=gridshift +grids=us_noaa_nadcon5_nad83_2007_nad83_2011_conus.tif

Parameters
################################################################################

Required
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +grids=<list>

    Comma-separated list of grids to load. If a grid is prefixed by an ``@`` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids must be in GeoTIFF format (:ref:`geodetictiffgrids`) and have an
    explicit TYPE metadata item whose value is ``HORIZONTAL_OFFSET``,
    ``GEOGRAPHIC_3D_OFFSET``, ``ELLIPSOIDAL_HEIGHT_OFFSET``
    ``VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL`` or ``VERTICAL_OFFSET_VERTICAL_TO_VERTICAL``.

Optional
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +interpolation=bilinear/biquadratic

    Default is bilinear, unless the grid contains a ``interpolation_method``
    metadata item specifying the method.
    Biquadratic is typically used for NADCON5 grids, and is defined in
    `NOAA Technical Memorandum NOS NGS 84 - Biquadratic Interpolation
    <https://geodesy.noaa.gov/library/pdfs/NOAA_TM_NOS_NGS_0084.pdf>`__

.. option:: +no_z_transform

    If specified, vertical coordinate transformation will be skipped.

.. _deformation:

================================================================================
Kinematic datum shifting utilizing a deformation model
================================================================================

.. versionadded:: 5.0.0

Perform datum shifts means of a deformation/velocity model.

+-----------------+--------------------------------------------------------------------+
| **Input type**  | Cartesian coordinates (spatial), decimalyears (temporal).          |
+-----------------+--------------------------------------------------------------------+
| **Output type** | Cartesian coordinates (spatial), decimalyears (temporal).          |
+-----------------+--------------------------------------------------------------------+
| **Domain**      | 4D                                                                 |
+-----------------+--------------------------------------------------------------------+
| **Input type**  | Geodetic coordinates                                               |
+-----------------+--------------------------------------------------------------------+
| **Output type** | Geodetic coordinates                                               |
+-----------------+--------------------------------------------------------------------+


The deformation operation is used to adjust coordinates for intraplate deformations.
Usually the transformation parameters for regional plate-fixed reference frames such as
the ETRS89 does not take intraplate deformation into account. It is assumed that
tectonic plate of the region is rigid. Often times this is true, but near the plate
boundary and in areas with post-glacial uplift the assumption breaks. Intraplate
deformations can be modelled and then applied to the coordinates so that
they represent the physical world better. In PROJ this is done with the deformation
operation.

The horizontal grid is stored in CTable2 format and the vertical grid is stored in the
GTX format. Both grids are expected to contain grid-values in units of mm/year.
Details about the formats can be found in the GDAL documentation. GDAL both reads and
writes both file formats. Using GDAL for construction of new grids is recommended.

Example
-------------------------------------------------------------------------------

In :cite:`Hakli2016` coordinate transformation including a deformation model is described.
The paper describes how coordinates from the global ITRFxx frames are transformed to the
local Nordic realisations of ETRS89. Scandinavia is an area with significant post-glacial
rebound. The deformations from the post-glacial uplift is not accounted for in the
official ETRS89 transformations so in order to get accurate transformations in the Nordic
countries it is necessary to apply the deformation model. The transformation from ITRF2008
to the Danish realisation of ETRS89 is in PROJ described as::


    proj =  pipeline ellps = GRS80
            # ITRF2008@t_obs -> ITRF2000@t_obs
    step    init = ITRF2008:ITRF2000
            # ITRF2000@t_obs -> ETRF2000@t_obs
    step    proj=helmert t_epoch = 2000.0 transpose
            x =  0.054  rx =  0.000891 drx =  8.1e-05
            y =  0.051  ry =  0.00539  dry =  0.00049
            z = -0.048  rz = -0.008712 drz = -0.000792
            # ETRF2000@t_obs -> NKG_ETRF00@2000.0
    step    proj = deformation t_epoch = 2000.0
            xy_grids = ./nkgrf03vel_realigned_xy.ct2
            z_grids  = ./nkgrf03vel_realigned_z.gtx
            # NKG_ETRF@2000.0 -> ETRF92@2000.0
    step    proj=helmert transpose s = -0.009420e
            x = 0.03863 rx = 0.00617753
            y = 0.147   ry = 5.064e-05
            z = 0.02776 rz = 4.729e-05
            # ETRF92@2000.0 -> ETRF92@1994.704
    step    proj = deformation t_epoch = 1994.704 t_obs = 2000.0
            xy_grids = ./nkgrf03vel_realigned_xy.ct2
            z_grids  = ./nkgrf03vel_realigned_z.gtx

From this we can see that the transformation from ITRF2008 to the Danish realisation of
ETRS89 is a combination of Helmert transformations and adjustments with a deformation
model. The first use of the deformation operation is::

    proj = deformation t_epoch = 2000.0
    xy_grids = ./nkgrf03vel_realigned_xy.ct2
    z_grids  = ./nkgrf03vel_realigned_z.gtx

Here we set the central epoch of the transformation, 2000.0. The observation epoch
is expected as part of the input coordinate tuple. The deformation model is
described by two grids, specified with :option:`+xy_grids` and :option:`+z_grids`.
The first is the horizontal part of the model and the second is the vertical
component.

Parameters
-------------------------------------------------------------------------------

Required
################################################################################


.. option:: +xy_grids=<list>

    Comma-separated list of grids to load. If a grid is prefixed by an `@` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids for the horizontla component of a deformation model is expected to be
    in CTable2 format.

.. option:: +z_grids=<list>

    Comma-separated list of grids to load. If a grid is prefixed by an `@` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids for the vertical component of a deformation model is expected to be
    in either GTX format.

.. option:: +t_epoch=<value>

    Central epoch of transformation given in decimalyears.

Optional
################################################################################

.. option:: +t_obs=<value>

    Observation time of coordinate(s) given in decimalyears. If not specified,
    the observation time from the temporal component of 4D input points is used.

Mathematical description
-------------------------------------------------------------------------------

Mathematically speaking, application of a deformation model is simple. The deformation model is
represented as a grid of velocities in three dimensions. Coordinate corrections are
applied in cartesian space. For a given coordinate, :math:`(X, Y, Z)`, velocities
:math:`(V_X, V_Y, V_Z)` can be interpolated from the gridded model. The time span
between :math:`t_c` and :math:`t_{obs}` determine the magnitude of the coordinate
correcton as seen in eq. :eq:`apply_velocity` below.

.. math::
    :label: apply_velocity

    \begin{align}
        \begin{pmatrix}
            X \\
            Y \\
            Z \\
        \end{pmatrix}_B =
        \begin{pmatrix}
            X \\
            Y \\
            Z \\
        \end{pmatrix}_A +
        (t_c - t_{obs})
        \begin{pmatrix}
            V_X \\
            V_Y \\
            V_Z \\
        \end{pmatrix}
    \end{align}

Corrections are done in cartesian space.

Coordinates of the gridded model are in ENU (east, north, up) space because it would
otherwise require an enormous 3 dimensional grid to handle the corrections in cartesian
space. Keeping the correction in lat/long space reduces the complexity of the grid
significantly. Consequently though, the input coordinates needs to be converted to
lat/long space when searching for corrections in the grid. This is done with the *cart*
operation. The converted grid corrections can then be applied to the input coordinates
in cartesian space. The conversion from ENU space to cartesian space is done in the
following way:

.. math::
    :label: enu2xyz

    \begin{align}
        \begin{pmatrix}
            X \\
            Y \\
            Z \\
        \end{pmatrix} =
        \begin{pmatrix}
            -\sin\phi \cos\lambda N - \sin\lambda E + \cos\phi \cos\lambda U \\
            -\sin\phi \sin\lambda N + \sin\lambda E + \cos\phi \sin\lambda U \\
                         \cos\phi N +                 \sin\phi U \\
        \end{pmatrix}
    \end{align}

where :math:`\phi` and :math:`\lambda` are the latitude and longitude of the coordinate
that is searched for in the grid. :math:`(E, N, U)` are the grid values in ENU-space and
:math:`(X, Y, Z)` are the corrections converted to cartesian space.

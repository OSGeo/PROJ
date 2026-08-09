.. _affine:

================================================================================
Affine transformation
================================================================================

.. versionadded:: 6.0.0

The affine transformation applies translation and scaling/rotation terms on the
x,y,z coordinates, and translation and scaling on the temporal coordinate.

+---------------------+----------------------------------------------------------+
| **Alias**           | affine                                                   |
+---------------------+----------------------------------------------------------+
| **Domain**          | 4D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | XYZT                                                     |
+---------------------+----------------------------------------------------------+
| **output type**     | XYZT                                                     |
+---------------------+----------------------------------------------------------+

By default, the parameters are set for an identity transforms. The transformation
is reversible unless the determinant of the sji matrix is 0, or `tscale` is 0

This can be used to implement:

- EPSG "Affine parametric transformation" of code 9624 by setting `A0`, `A1`, `A2`,
  `B0`, `B1`, `B2` EPSG parameters to respectively `xoff`, `s11`, `s12`, `yoff`,
  `s21`, `s22` PROJ parameters.

- EPSG "Similarity transformation" of code 9621 by setting:

    * xoff to :math:`X_{T0}`
    * yoff to :math:`Y_{T0}`
    * s11 to :math:`M \cos \theta`
    * s12 to :math:`M \sin \theta`
    * s21 to :math:`-M \sin \theta`
    * s22 to :math:`M \cos \theta`

  where:

    * :math:`X_{T0}` is the first ordinate of the origin point of the source
      CRS expressed in the target CRS.
    * :math:`Y_{T0}` is the second ordinate of the origin point of the source
      CRS expressed in the target CRS.
    * :math:`M` is the multiplication factor applied to coordinates in the
      source CRS to obtain the correct scale of the target CRS.
    * :math:`\theta` is the angle about which the axes of the source CRS need to
      be rotated to coincide with the axes of the target CRS, counter-clockwise
      being positive

- EPSG "P6 I=J+90 seismic bin grid coordinate operation" of code 9666 and
  "P6 I=J-90 seismic bin grid coordinate operation" of code 1049, which convert
  from the easting/northing of a map grid to the I/J bin numbers of a seismic
  bin grid, by setting:

    * s11 to :math:`\pm s_I \cos \theta`
    * s12 to :math:`\mp s_I \sin \theta`
    * s21 to :math:`s_J \sin \theta`
    * s22 to :math:`s_J \cos \theta`
    * xoff to :math:`I_0 - (s11 \times E_0 + s12 \times N_0)`
    * yoff to :math:`J_0 - (s21 \times E_0 + s22 \times N_0)`

  where:

    * :math:`s_I = Inc_I / (k \times M_I)` and :math:`s_J = Inc_J / (k \times M_J)`
      are the number of bin grid units per map grid unit along the I and J axes,
      with :math:`M_I` and :math:`M_J` the bin widths on the I and J axes,
      :math:`Inc_I` and :math:`Inc_J` the bin node increments on the I and J
      axes, and :math:`k` the scale factor of the bin grid.
    * :math:`I_0` and :math:`J_0` are the bin grid coordinates of the origin
      point of the bin grid, and :math:`E_0` and :math:`N_0` its easting and
      northing in the map grid.
    * :math:`\theta` is the map grid bearing of the bin grid J-axis, clockwise
      being positive.
    * the upper sign of :math:`\pm` and :math:`\mp` applies to the
      right-handed method 9666 (I = J+90°) and the lower one to the
      left-handed method 1049 (I = J-90°).

  :math:`M_I`, :math:`M_J`, :math:`E_0` and :math:`N_0` must all be expressed
  in the same linear unit as the easting and northing fed to the affine
  operation.

Examples
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The `+proj=affine` operation applies an affine transformation on coordinate values.
Note that it does not perform any unit conversion and will accept
coordinates whether  they are angular or linear. Users must ensure that the
values provided to the affine operation are expressed in the appropriate units
for their intended purpose.

The affine operation can be used with the `cct` utility or as a step inside a
PROJ pipeline.

Offsetting a the x-component of a coordinate by 1:

    echo "1 2" | cct +proj=affine +xoff=1

Parameters
################################################################################

Optional
-------------------------------------------------------------------------------

.. option:: +xoff=<value>

    Offset in X. Default value: 0

.. option:: +yoff=<value>

    Offset in Y. Default value: 0

.. option:: +zoff=<value>

    Offset in Z. Default value: 0

.. option:: +toff=<value>

    Offset in T. Default value: 0

.. option:: +s11=<value>

    Rotation/scaling term. Default value: 1

.. option:: +s12=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s13=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s21=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s22=<value>

    Rotation/scaling term. Default value: 1

.. option:: +s23=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s31=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s32=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s33=<value>

    Rotation/scaling term. Default value: 1

.. option:: +tscale=<value>

    Time scaling term. Default value: 1



Mathematical description
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. math::
    :label: formula

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
            T \\
        \end{bmatrix}^{dest} =
        \begin{bmatrix}
            xoff \\
            yoff \\
            zoff \\
            toff \\
        \end{bmatrix} +
        \begin{bmatrix}
             s11 & s12 & s13 & 0 \\
             s21 & s22 & s23 & 0 \\
             s31 & s32 & s33 & 0 \\
             0   & 0   & 0   & tscale \\
        \end{bmatrix}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
            T \\
        \end{bmatrix}^{source}
    \end{align}



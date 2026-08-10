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

  where, naming each quantity after the EPSG parameter that carries it:

    * :math:`I_0` and :math:`J_0` are "Bin grid origin I" (EPSG:8733) and
      "Bin grid origin J" (EPSG:8734), the bin grid coordinates of the origin
      point of the bin grid.
    * :math:`E_0` and :math:`N_0` are "Bin grid origin Easting" (EPSG:8735) and
      "Bin grid origin Northing" (EPSG:8736), the map grid coordinates of that
      same origin point.
    * :math:`SF` is "Scale factor of bin grid" (EPSG:8737), that is the point
      scale factor of the map grid at a chosen reference point.
    * :math:`M_I` and :math:`M_J` are "Bin width on I-axis" (EPSG:8738) and
      "Bin width on J-axis" (EPSG:8739).
    * :math:`Inc_I` and :math:`Inc_J` are "Bin node increment on I-axis"
      (EPSG:8741) and "Bin node increment on J-axis" (EPSG:8742).
    * :math:`\theta` is "Map grid bearing of bin grid J-axis" (EPSG:8740),
      clockwise being positive.
    * :math:`s_I = Inc_I / (SF \times M_I)` and
      :math:`s_J = Inc_J / (SF \times M_J)` are the resulting number of bin
      grid units per map grid unit, along the I and J axes respectively.
    * the upper sign of :math:`\pm` and :math:`\mp` applies to the
      right-handed method 9666 (I = J+90°) and the lower one to the
      left-handed method 1049 (I = J-90°).

  :math:`E_0`, :math:`N_0`, :math:`M_I` and :math:`M_J` must all be expressed
  in the same linear unit as the easting and northing fed to the affine
  operation.

  The resulting bin numbers are not rounded, so that the operation remains
  exactly invertible. EPSG defines bin grid coordinate systems that accept real
  values as well as ones restricted to integer values, so whether bin numbers
  are whole values is a property of the bin grid coordinate system, not of
  these methods.

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



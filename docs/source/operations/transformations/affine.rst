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
      
Units and Limitations
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The :code:`+proj=affine` transformation has specific unit-handling behavior that
is not obvious from the parameter list. These points are important for correct
usage:

**1. Input and output angular units**

When :code:`+proj=affine` is used on angular coordinates (longitude/latitude):

* **Input angular values are interpreted in radians**
* **Output angular values are expressed in degrees**

Example (showing the implicit unit conversion):

    $ echo 0 1 | cs2cs +proj=affine +xoff=1
    57d17'44.806"W  57d17'44.806"N 0.000

The numeric input "1" is interpreted as one radian, and the output is reported
in degrees. Users should be aware of this implicit unit conversion when applying
affine steps to geographic coordinate values.

**2. Affine transformation cannot be used directly with the `proj` utility**

Attempting to run:

    proj +proj=affine

will produce the error:

    can't initialize operations that take non-angular input coordinates

This is expected. :code:`+proj=affine` must be used **as a step inside a
PROJ pipeline**, where its input coordinate type is determined by previous
pipeline steps.

Example of valid usage inside a pipeline:

    proj pipeline +step +proj=affine +xoff=1

**3. Recommended usage**

The affine transformation is generally intended for:

* Modifying projected coordinates
* Applying small adjustments inside pipelines
* Implementing EPSG parametric and similarity transforms

When applied to geographic coordinates, users must explicitly handle/unit-convert
angles as needed.


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



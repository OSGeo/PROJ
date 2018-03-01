.. _molodensky:

================================================================================
Molodensky transform
================================================================================

The Molodensky transformation resembles a :ref:`Helmert` with zero
rotations and a scale of unity, but converts directly from geodetic coordinates to
geodetic coordinates, without the intermediate shifts to and from cartesian
geocentric coordinates, associated with the Helmert transformation.
The Molodensky transformation is simple to implement and to parameterize, requiring
only the 3 shifts between the input and output frame, and the corresponding
differences between the semimajor axes and flattening parameters of the reference
ellipsoids. Due to its algorithmic simplicity, it was popular prior to the
ubiquity of digital computers. Today, it is mostly interesting for historical
reasons, but nevertheless indispensable due to the large amount of data that has
already been transformed that way [EversKnudsen2017]_.

+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates.                                    |
+---------------------+----------------------------------------------------------+
| **Output type**     | Geodetic coordinates.                                    |
+---------------------+----------------------------------------------------------+
| **Options**                                                                    |
+---------------------+----------------------------------------------------------+
| `+da`               | Difference in semimajor axis of the defining ellipsoids. |
+---------------------+----------------------------------------------------------+
| `+df`               | Difference in flattening of the defining ellipsoids.     |
+---------------------+----------------------------------------------------------+
| `+dx`               | Offset of the X-axes of the defining ellipsoids.         |
+---------------------+----------------------------------------------------------+
| `+dy`               | Offset of the Y-axes of the defining ellipsoids.         |
+---------------------+----------------------------------------------------------+
| `+dz`               | Offset of the Z-axes of the defining ellipsoids.         |
+---------------------+----------------------------------------------------------+
| `+ellps`            | Ellipsoid definition of source coordinates.              |
|                     | Any specification can be used (e.g. `+a`, `+rf`, etc).   |
|                     | If not specified, default ellipsoid is used.             |
+---------------------+----------------------------------------------------------+
| `+abridged`         | Use the abridged version of the Molodensky transform.    |
|                     | Optional.                                                |
+---------------------+----------------------------------------------------------+

The Molodensky transform can be used to perform a datum shift from coordinate
:math:`(\phi_1, \lambda_1, h_1)` to :math:`(\phi_2, \lambda_2, h_2)` where the two
coordinates are referenced to different ellipsoids. This is based on three
assumptions:

    1. The cartesian axes, :math:`X, Y, Z`,  of the two ellipsoids are parallel.
    2. The offset, :math:`\delta X, \delta Y, \delta Z`,  between the two ellipsoid
       are known.
    3. The characteristics of the two ellipsoids, expressed as the difference in
       semimajor axis (:math:`\delta a`) and flattening (:math:`\delta f`),  are known.

The Molodensky transform is mostly used for transforming between old systems
dating back to the time before computers. The advantage of the Molodensky transform
is that it is fairly simple to compute by hand. The ease of computation come at the
cost of limited accuracy.

A derivation of the mathematical formulas for the Molodensky transform can be found
in [Deakin2004]_.


Examples
###############################################################################

The abridged Molodensky::

    proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8  dx=-134 dy=-48 dz=149 abridged

The same transformation using the standard Molodensky::

    proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8  dx=-134 dy=-48 dz=149


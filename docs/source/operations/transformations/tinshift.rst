.. _tinshift:

================================================================================
Triangulated Irregular Network based transformation
================================================================================

.. versionadded:: 7.2.0

+---------------------+--------------------------------------------------------------------+
| **Alias**           | tinshift                                                           |
+---------------------+--------------------------------------------------------------------+
| **Input type**      | Projected or geographic coordinates (horizontal), meters (vertical)|
+---------------------+--------------------------------------------------------------------+
| **Output type**     | Projected or geographic coordinates (horizontal), meters (vertical)|
+---------------------+--------------------------------------------------------------------+
| **Domain**          | 2D or 3D                                                           |
+---------------------+--------------------------------------------------------------------+
| **Available forms** | Forward and inverse                                                |
+---------------------+--------------------------------------------------------------------+


The ``tinshift`` transformation takes one mandatory
argument, ``file``, that points to a :ref:`TIN JSON <tin_json>` or
:ref:`TIN GeoPackage <tin_gpkg>` file, which contains the
triangulation and associated metadata. Input and output coordinates must be
geographic or projected coordinates.
Depending on the content of the TIN file, horizontal, vertical or both
components of the coordinates may be transformed.

The transformation is invertible, with the same computational complexity than
the forward transformation.

Parameters
-------------------------------------------------------------------------------

Required
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +file=<filename>

    Filename or URL to the JSON or GeoPackage file for the TIN.


Example
-------------------------------------------------------------------------------

Transforming a point with the Finland EPSG:2393 ("KKJ / Finland Uniform
Coordinate System") projected CRS to EPSG:3067 ("ETRS89 / TM35FIN(E,N)")

::

    $ echo 3210000.0000 6700000.0000 0 2020 | cct +proj=tinshift +file=./triangulation_kkj.json

    209948.3217     6697187.0009    0.0000     2020


Algorithm
+++++++++

Internally, ``tinshift`` ingest the whole file into memory. It is considered that
triangulation should be small enough for that.

When a point is transformed, one must find the triangle into which it falls into.
Instead of iterating over all triangles, we build a in-memory quadtree to speed-up
the identification of candidates triangles.

To determine if a point falls into a triangle, one computes its 3
`barycentric coordinates <https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Conversion_between_barycentric_and_Cartesian_coordinates>`_
from its projected coordinates, :math:`\lambda_i` for :math:`i=1,2,3`.
They are real values (in the [0,1] range for a point inside the triangle),
giving the weight of each of the 3 vertices of the triangles.

Once those weights are known, interpolating the target horizontal
coordinate is a matter of doing the linear combination of those weights with
the target horizontal coordinates at the 3 vertices of the triangle (:math:`Xt_i` and :math:`Yt_i`):

.. math::

    X_{target} = Xt_1 * \lambda_1 + Xt_2 * \lambda_2 + Xt_3 * \lambda_3

    Y_{target} = Yt_1 * \lambda_1 + Yt_2 * \lambda_2 + Yt_3 * \lambda_3

This interpolation is exact at the vertices of the triangulation, and has linear properties
inside each triangle. It is completely equivalent to other formulations of
triangular interpolation, such as

.. math::

    X_{target} = A + X_{source} * B  + Y_{source} * C

    Y_{target} = D + X_{source} * E  + Y_{source} * F

where the A, B, C, D, E, F constants (for a given triangle) are found by solving
the 2 systems of 3 linear equations, constraint by the source and target coordinate pairs
of the 3 vertices of the triangle:

.. math::

    Xt_i = A + Xs_i * B + Ys_i * C

    Yt_i = D + Xs_i * E + Ys_i * F

Similarly for a vertical coordinate transformation, where :math:`Zoff_i` is the vertical
offset at each vertex of the triangle:

.. math::

    Z_{target} = Z_{source} + Zoff_1 * \lambda_1 + Zoff_2 * \lambda_2 + Zoff_3 * \lambda_3

Constraints on the triangulation
++++++++++++++++++++++++++++++++

No check is done on the consistence of the triangulation. It is highly
recommended that triangles do not overlap each other (when considering the
source coordinates or the forward transformation, or the target coordinates for
the inverse transformation), otherwise which triangle will be selected is
unspecified. Besides that, the triangulation does not need to have particular
properties (like being a Delaunay triangulation)

JSON TIN File format
++++++++++++++++++++

See :ref:`tin_json`.

GeoPackage TIN File format
++++++++++++++++++++++++++

.. versionadded:: 9.8.0

See :ref:`tin_gpkg`.

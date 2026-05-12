.. _polyhedral:

********************************************************************************
Polyhedral Snyder Equal Area
********************************************************************************

.. figure:: ./images/polyhedral_title.png
   :width: 800 px
   :align: center
   :alt:   Polyhedral projection (DSEA A5)

The polyhedral Snyder equal-area projection maps the sphere onto the faces of
a polyhedron using the method described in :cite:`Snyder1992`. The projection
is area-preserving and supports both forward and inverse transforms on either
a sphere or an ellipsoid (input geodetic latitudes are converted to authalic
latitudes internally so the equal-area property is preserved).

The implementation is generic: each polyhedral face is subdivided into right
sub-triangles, Snyder's equal-area mapping is applied independently on each
sub-triangle, and the planar results are laid out according to a *net* — a
2D unfolding of the polyhedron.

Three projection aliases are registered:

- ``tsea`` :ref:`tsea`
- ``dsea`` :ref:`dsea`
- ``isea`` :ref:`isea`

Triangles
********************************************************************************

At its core, the projection is an area-preserving mapping from a spherical
right triangle ABC to a planar right triangle XYZ. Both triangles have one
distinguished vertex — the *apex* — which is listed first in either triple:
A on the sphere, X in the plane. The apex is shared by the sub-triangles
that fan out from a single face centre, so the sub-triangles meeting at the
apex tile a complete polyhedral face.

The planar triangles can be chosen with any consistent shape, as long as the
edges around the shared apex match the way the corresponding edges meet on
the sphere. The freedom in choosing this planar shape — together with the
freedom in choosing how to glue the faces together — is what defines a
*net*.

Polyhedra and nets
********************************************************************************

Each polyhedral projection is built from two compact definitions:

- A *polyhedron*: a list of 3D vertices on the unit sphere plus a list of
  face vertex indices (CCW from outside). By convention the first vertex is
  always `{0, 0, 1}`
- A *net*: a parent tree over the faces. The indexing is 1-based, with `0`
  reserved as the marker for the root face (that which has no parent).
  For example, `{2, 3, 0, 3}` means:
   - the 1st face has the **2nd face** as its parent
   - the 2nd face has the **3rd face** as its parent
   - the 3rd face has the **no face** as its parent (root)
   - the 4th face has the **3rd face** as its parent

.. figure:: ./images/polyhedral_tsea_layout.png
   :width: 500 px
   :align: center
   :alt:   TSEA layout

Projection specifications
********************************************************************************

Because the planar layout is decoupled from the spherical triangulation, the
same polyhedron can be unfolded into several different nets, which can be
selected by name using the `+net` parameter, e.g. `+proj=tsea +net=star`


The defaults for ``+orient_lat`` / ``+orient_lon`` / ``+azi`` rotate each
polyhedron into a sensible default pose (e.g. centred on Europe / Africa for
``dsea``; matching Snyder's published Figure 12 for ``isea2``). When
``+lat_0`` / ``+lon_0`` are not supplied, the projected origin (0, 0) lands
on the centroid of the unfold's root face — except for ``isea2``, which
defaults to the bounding-box centre of the root face to match the legacy
``+proj=isea`` layout exactly.

Parameters
********************************************************************************

.. note::
    All parameters are optional. Each polyhedral projection ships with a
    default orientation that places its vertices in symmetric, well-known
    positions; the parameters below override that default.

.. option:: +net=<name>

    Selects the planar unfolding (net). The accepted values depend on the
    projection:

.. option:: +orient_lat=<value>

    Geodetic latitude of the polyhedron's first vertex, in degrees.

    *Default depends on the projection.*

.. option:: +orient_lon=<value>

    Longitude of the polyhedron's first vertex, in degrees.

    *Default depends on the projection.*

.. option:: +azi=<value>

    Azimuth from the first vertex to the second, in degrees. Together with
    ``+orient_lat`` and ``+orient_lon`` this fully constrains the 3D pose of
    the polyhedron on the sphere.

    *Default depends on the projection.*

.. include:: ../options/lat_0.rst

For polyhedral projections, ``+lat_0`` / ``+lon_0`` specify the geographic
point that should land at the projected origin ``(0, 0)``. They translate
the projected output without rotating the polyhedron — the triangle edges
stay where ``+orient_*`` placed them. ``+x_0`` / ``+y_0`` are applied on top
of this translation.

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

Examples:

.. figure:: ./images/polyhedral_tsea.png
   :width: 500 px
   :align: center
   :alt:   TSEA

   proj-string: ``+proj=tsea``

.. figure:: ./images/polyhedral_isea.png
   :width: 500 px
   :align: center
   :alt:   ISEA

   proj-string: ``+proj=isea2``

.. figure:: ./images/polyhedral_dsea_flower.png
   :width: 500 px
   :align: center
   :alt:   DSEA Flower

   proj-string: ``+proj=dsea +net=flower``

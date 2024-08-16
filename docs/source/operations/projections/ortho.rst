.. _ortho:

********************************************************************************
Orthographic
********************************************************************************

The orthographic projection is a perspective azimuthal projection centered
around a given latitude and longitude.

+---------------------+--------------------------------------------------------+
| **Classification**  | Azimuthal                                              |
+---------------------+--------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal         |
+---------------------+--------------------------------------------------------+
| **Defined area**    | Global, although only one hemisphere can be seen at a  |
|                     | time                                                   |
+---------------------+--------------------------------------------------------+
| **Alias**           | ortho                                                  |
+---------------------+--------------------------------------------------------+
| **Domain**          | 2D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                   |
+---------------------+--------------------------------------------------------+
| **Output type**     | Projected coordinates                                  |
+---------------------+--------------------------------------------------------+

.. figure:: ./images/ortho.png
   :width: 500 px
   :align: center
   :alt:   Orthographic

   proj-string: ``+proj=ortho``


.. note:: Before PROJ 7.2, only the spherical formulation was implemented. If
          wanting to replicate PROJ < 7.2 results with newer versions, the
          ellipsoid must be forced to a sphere, for example by adding a ``+f=0``
          parameter.

.. note:: Parameters ``k_0`` and ``alpha`` are added in PROJ 9.5.0

This projection method corresponds to ``EPSG:9840``
(or ``EPSG:1130`` with ``k_0`` or ``alpha``).

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. option:: +alpha=<value>

    .. versionadded:: 9.5.0

    Azimuth clockwise from north at the center of projection.

    *Defaults to 0.0.*

.. option:: +k_0=<value>

   .. versionadded:: 9.5.0

    Scale factor. Determines scale factor used in the projection.

    *Defaults to 1.0.*

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. _ortho:

********************************************************************************
Orthographic
********************************************************************************

The orthographic projection is a perspective azimuthal projection centered
around a given latitude and longitude.

+---------------------+--------------------------------------------------------+
| **Classification**  | Azimuthal                                              |
+---------------------+--------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection              |
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

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

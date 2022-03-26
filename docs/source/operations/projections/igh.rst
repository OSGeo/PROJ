.. _igh:

********************************************************************************
Interrupted Goode Homolosine
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Pseudocylindrical                                        |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | igh                                                      |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/igh.png
   :width: 500 px
   :align: center
   :alt:   Interrupted Goode Homolosine

   proj-string: ``+proj=igh``


The Interrupted Goode Homolosine projection is an equal-area composite
projection intended for making world maps. Low latitudes are comprised of six
separate Sinusoidal projection regions, and high latitudes are comprised of
six separate Mollweide (``homolographic``) projections. The transition latitude
is at 40d 44' 11.8", where the Sinusoidal and Mollweide scales are equal. The
lobes in this projection are chosen to emphasize the land area of the Earth. This
projection was first published in 1925 by J. P. Goode :cite:`Goode1925`.


Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

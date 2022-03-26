.. _igh_o:

********************************************************************************
Interrupted Goode Homolosine (Oceanic View)
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Pseudocylindrical                                        |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | igh_o                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/igh_o.png
   :width: 500 px
   :align: center
   :alt:   Interrupted Goode Homolosine

   proj-string: ``+proj=igh_o +lon_0=-160``


The Interrupted Goode Homolosine (Oceanic View) projection is an equal-area
composite projection intended for making maps of the Earth's oceans. Low
latitudes are comprised of six separate Sinusoidal projection regions, and high
latitudes are comprised of six separate Mollweide (``homolographic``)
projections. The transition latitude is at 40d 44' 11.8", where the Sinusoidal
and Mollweide scales are equal. The lobes in this projection are chosen to
emphasize the ocean area of the Earth when used with a central longitude
of -160 degrees. This projection was first published in 1925 by J. P. Goode
:cite:`Goode1925`.


Parameters
################################################################################

.. note:: All parameters are optional for the projection. A value of +lon_0=-160 is recommended.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. _aea:

********************************************************************************
Albers Equal Area
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Conic                                                    |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and elliptical projection |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | aea                                                      |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/aea.png
   :width: 500 px
   :align: center
   :alt:   Albers Equal Area

   proj-string: ``+proj=aea``

Options
################################################################################

.. note:: All options are optional for the Albers Equal Area projection.

.. include:: ../options/lat_1.rst

.. include:: ../options/lat_2.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst


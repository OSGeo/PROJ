.. _dymaxion:

********************************************************************************
Dymaxion Fuller
********************************************************************************

The Dymaxion Fuller map projection is a icosahedron polyhedral projection that intends
to lay down continental landmasses into a contiguous map with minimal scale and angular
distorsion while having as few land interruptions as possible.
In comparison to the Snyder's Icosahedral Equal Area, this projection relies on a specific way 
to orient and cut the icosahedron so that most interruptions lie within the ocean. The projection
was first invented by Buckminster Fuller in 1943 using a cubostahedron as basis.
The idea to use an icosahedron came later in 1954, and it wasn't until 1978 that the projection 
was formally mathematically defined :cite:`Gray1995`.

+---------------------+----------------------------------------------------------+
| **Classification**  | Polyhedral, equal area                                   |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical                           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | dymaxion                                                 |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/dymaxion.png
   :width: 500 px
   :align: center
   :alt:   Dymaxion Fuller

   proj-string: ``+proj=dymaxion``

.. note::
   The output dymaxion map applied to the whole globe has an extent of roughly
   :math:`[0, 0, 17404000, 36844000]` in meters. Although the dymaxion map has no
   true up or down, the choice was made to lay it out vertically, from Antartica to Australia.


Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

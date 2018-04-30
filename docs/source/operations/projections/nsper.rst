.. _nsper:

********************************************************************************
Near-sided perspective
********************************************************************************

The near-sided perspective projection simulates a view from a height
:math:`h` similar to how a satellite in orbit would see it.

+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal. Neither conformal nor equal area.             |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse spherical projection                 |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, although for one hemisphere at a time.           |
+---------------------+----------------------------------------------------------+
| **Alias**           | nsper                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. image:: ./images/nsper.png
   :scale: 50%
   :alt:   Near-sided perspective

Parameters
################################################################################

Required
-------------------------------------------------------------------------------

.. include:: ../options/h.rst

Optional
-------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

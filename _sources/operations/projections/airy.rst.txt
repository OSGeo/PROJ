.. _airy:

********************************************************************************
Airy
********************************************************************************

The Airy projection is an azimuthal minimum error projection for the region
within the small or great circle defined by an angular distance,
:math:`\phi_b`, from the tangency point of the plane :math:`( \lambda_0, \phi_0 )`.

+---------------------+----------------------------------------------------------+
| **Classification**  |                                                          |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward spherical projection                             |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Implemented by**  | Gerald I. Evenden                                        |
+---------------------+----------------------------------------------------------+
| **Options**                                                                    |
+---------------------+----------------------------------------------------------+
| `+lat_b`            | Angular distance from tangency point of the plane        |
|                     | :math:`( \lambda_0, \phi_0 )` where the error is kept at |
|                     | minimum. Defaults to :math:`90^{\circ}` which is         |
|                     | suitable for hemispherical maps.                         |
+---------------------+----------------------------------------------------------+
| `+no_cut`           | Do not cut at hemisphere limit                           |
+---------------------+----------------------------------------------------------+


.. image:: ./images/airy.png
   :scale: 50%
   :alt:   Airy


.. _tpers:

********************************************************************************
Tilted perspective
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Implemented by**  | Gerald I. Evenden                                        |
+---------------------+----------------------------------------------------------+
| **Options**                                                                    |
+---------------------+----------------------------------------------------------+
| `+h`                | Height (in meters) above the surface. Required.          |
+---------------------+----------------------------------------------------------+
| `+azi`              | Bearing (in degrees) from due north.                     |
+---------------------+----------------------------------------------------------+
| `+tilt`             | Angle (in degrees) away from nadir.                      |
+---------------------+----------------------------------------------------------+
| `+lat_0`            | Latitude (in degrees) of the view position.              |
+---------------------+----------------------------------------------------------+
| `+lon_0`            | Longitude (in degrees) of the view position.             |
+---------------------+----------------------------------------------------------+


.. image:: ./images/tpers.png
   :scale: 50%
   :alt:   Tilted perspective


Tilted Perspective is similar to :ref:`nsper` (``nsper``) in that it simulates a
perspective view from a height. Where ``nsper`` projects onto a plane tangent to
the surface, Tilted Perspective orients the plane towards the direction of the
view. Thus, extra parameters azi and tilt are required beyond `nsper``'s ``h``.
As with ``nsper``, ``lat_0`` & ``lon_0`` are also required
for satellite position.

.. _nsper:

********************************************************************************
Near-sided perspective
********************************************************************************

The near-sided perspective projection simulates a view from a height
:math:`h` similar to how a satellite in orbit would see it.

+---------------------+-------------------------------------------------------------+
| **Classification**  | Azimuthal. Neither conformal nor equal area.                |
+---------------------+-------------------------------------------------------------+
| **Available forms** | Forward and inverse spherical projection                    |
+---------------------+-------------------------------------------------------------+
| **Defined area**    | Global, although for one hemisphere at a time.              |
+---------------------+-------------------------------------------------------------+
| **Options**                                                                       |
+---------------------+-------------------------------------------------------------+
| `+h`                | Height of the view point above the Earth and must be in     |
|                     | the same units as the radius of the sphere. Required.       |
+---------------------+-------------------------------------------------------------+
| `+lat_0`            | Latitude (in degrees) of the view position. Defaults to 0.  |
+---------------------+-------------------------------------------------------------+
| `+lon_0`            | Longitude (in degrees) of the view position. Defaults to 0. |
+---------------------+-------------------------------------------------------------+


.. image:: ./images/nsper.png
   :scale: 50%
   :alt:   Near-sided perspective


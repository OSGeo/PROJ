.. _geos:

********************************************************************************
Geostationary Satellite View
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and elliptical projection |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Implemented by**  | Gerald I. Evenden and Martin Raspaud$                    |
+---------------------+----------------------------------------------------------+
| **Options**                                                                    |
+---------------------+----------------------------------------------------------+
| `+h`                | Satellite height above earth. Required.                  |
+---------------------+----------------------------------------------------------+
| `+sweep`            | Sweep angle axis of the viewing instrument.              |
|                     | Valid options are ``x`` and ``y``. Defaults to ``y``.    |
+---------------------+----------------------------------------------------------+
| `+lon_0`            | Subsatellite longitude point.                            |
+---------------------+----------------------------------------------------------+


.. image:: ./images/geos.png
   :scale: 50%
   :alt:   Geostationary Satellite View

The geos projection pictures how a geostationary satellite scans the earth at regular
scanning angle intervals.


Usage
###############################################################################

In order to project using the geos projection you can do the following::

    proj +proj=geos +h=35785831.0

The required argument ``h`` is the viewing point (satellite position) height above
the earth.

The projection coordinate relate to the scanning angle by the following simple
relation::

    scanning_angle (radians) = projection_coordinate / h


Note on sweep angle
-------------------------------------------------------------------------------

The viewing instrument on-board geostationary satellites described by this
projection have a two-axis gimbal viewing geometry. This means that the different
scanning positions are obtained by rotating the gimbal along a N/S axis (or ``y``)
and a E/W axis (or ``x``).

.. image:: ../../../../images/geos_sweep.png
   :scale: 50%
   :alt:   Gimbal geometry

In the image above, the outer-gimbal axis, or sweep-angle axis, is the N/S axis (``y``)
while the inner-gimbal axis, or fixed-angle axis, is the E/W axis (``x``).

This example represents the scanning geometry of the Meteosat series satellite.
However, the GOES satellite series use the opposite scanning geometry, with the
E/W axis (``x``) as the sweep-angle axis, and the N/S (``y``) as the fixed-angle axis.

The sweep argument is used to tell PROJ.4 which on which axis the outer-gimbal
is rotating. The possible values are x or y, y being the default. Thus, the
scanning geometry of the Meteosat series satellite should take sweep as x, and
GOES should take sweep as y.

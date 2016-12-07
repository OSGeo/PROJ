.. _eqc:

********************************************************************************
Equidistant Cylindrical (Plate Carrée)
********************************************************************************

The simplest of all projections. Standard parallels (0° when omitted) may be specified that determine latitude of true scale (k=h=1).

+---------------------+----------------------------------------------------------+
| **Classification**  | Conformal cylindrical                                    |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse                                      |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, but best used near the equator                   |
+---------------------+----------------------------------------------------------+
| **Implemented by**  | Gerald I. Evenden                                        |
+---------------------+----------------------------------------------------------+
| **Options**                                                                    |
+---------------------+----------------------------------------------------------+
| `+lat_ts`           | Latitude of true scale. Defaults to 0.0                  |
+---------------------+----------------------------------------------------------+
| `+lat_0`            | Center of the map : latitude of origin                   |
+---------------------+----------------------------------------------------------+

.. image:: ./images/eqc.png
   :scale: 50%
   :alt:   Equidistant Cylindrical (Plate Carrée)

Usage
########

Because of the distortions introduced by this projection, it has little use in navigation or cadastral mapping and finds its main use in thematic mapping.
In particular, the plate carrée has become a standard for global raster datasets, such as Celestia and NASA World Wind, because of the particularly simple relationship between the position of an image pixel on the map and its corresponding geographic location on Earth.

The following table gives special cases of the cylindrical equidistant projection.

+---------------------------------------------------------+--------------------------+
| Projection Name                                         | (lat ts=) :math:`\phi_0` |
+---------------------------------------------------------+--------------------------+
| Plain/Plane Chart                                       | 0°                       |
+---------------------------------------------------------+--------------------------+
| Simple Cylindrical                                      | 0°                       |
+---------------------------------------------------------+--------------------------+
| Plate Carrée                                            | 0°                       |
+---------------------------------------------------------+--------------------------+
| Ronald Miller—minimum overall scale distortion          | 37°30′                   |
+---------------------------------------------------------+--------------------------+
| E.Grafarend and A.Niermann                              | 42°                      |
+---------------------------------------------------------+--------------------------+
| Ronald Miller—minimum continental scale distortion      | 43°30′                   |
+---------------------------------------------------------+--------------------------+
| Gall Isographic                                         | 45°                      |
+---------------------------------------------------------+--------------------------+
| Ronald Miller Equirectangular                           | 50°30′                   |
+---------------------------------------------------------+--------------------------+
| E.Grafarend and A.Niermann minimum linear distortion    | 61°7′                    |
+---------------------------------------------------------+--------------------------+


Example using EPSG 32662 (WGS84 Plate Carrée)::

    $ echo 2 47 | proj +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs
    222638.98       5232016.07

Example using Plate Carrée projection with true scale at latitude 30° and central meridian 90°W::

    $ echo -88 30 | proj +proj=eqc +lat_ts=30 +lat_0=90w
    -8483684.61     13358338.90


Mathematical definition
#######################

The formulas describing the Equidistant Cylindrical projection are all taken from Snyder's [Snyder1987]_.

:math:`\phi_{ts}` is the latitude of true scale, that mean the standard parallels where the scale of the projection is true. It can be set with ``+lat_ts``.

:math:`\phi_0` is the latitude of origin that match the center of the map. It can be set with ``+lat_0``.


Forward projection
==================

.. math::

   x = \lambda \cos \phi_{ts}

.. math::

   y = \phi - \phi_0

Inverse projection
==================

.. math::

   \lambda = x / cos \phi_{ts}

.. math::

   \phi = y + \phi_0


Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Equirectangular_projection>`_
#. `Wolfram Mathworld <http://mathworld.wolfram.com/CylindricalEquidistantProjection.html>`_



.. _eqc:

********************************************************************************
Equidistant Cylindrical (Plate Caree)
********************************************************************************

The simplist of all projections. Standard parallels (0° when omitted) may be specified that determine latitude of true scale (k=h=1).

+---------------------+----------------------------------------------------------+
| **Classification**  | Conformal cylindrical                                    |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse                                      |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, but best used near the equator                   |
+---------------------+----------------------------------------------------------+
| **Implemented by**  | Marinus of Tyre                                          |
+---------------------+----------------------------------------------------------+
| **Options**                                                                    |
+---------------------+----------------------------------------------------------+
| `+lat_ts`           | Latitude of true scale. Defaults to 0.0                  |
+---------------------+----------------------------------------------------------+
| `+lat_0`            | Center of the map                                        |
+---------------------+----------------------------------------------------------+

.. image:: ./images/eqc.png
   :scale: 50%
   :alt:   Equidistant Cylindrical (Plate Caree)  

Usage
########

Because of the distortions introduced by this projection, it has little use in navigation or cadastral mapping and finds its main use in thematic mapping. In particular, the plate carrée has become a standard for global raster datasets, such as Celestia and NASA World Wind, because of the particularly simple relationship between the position of an image pixel on the map and its corresponding geographic location on Earth.

The following table gives special cases of the cylindrical equidistant projection. 

+---------------------------------------------------------+--------------------------+
| Projection Name                                         | (lat ts=) :math:`\phi_0` |
+---------------------------------------------------------+--------------------------+
| Plain/Plane Chart                                       | 0◦                       |
+---------------------------------------------------------+--------------------------+
| Simple Cylindrical                                      | 0◦                       |
+---------------------------------------------------------+--------------------------+
| Plate Carrée                                            | 0◦                       |
+---------------------------------------------------------+--------------------------+
| Ronald Miller—minimum overall scale distortion          | 37°30'                   |
+---------------------------------------------------------+--------------------------+
| E.Grafarend and A. Niermann                             | 42◦                      |
+---------------------------------------------------------+--------------------------+
| Ronald Miller—minimum continental scale distortion      | 43°30'                   |
+---------------------------------------------------------+--------------------------+
| Gall Isographic                                         | 45◦                      |
+---------------------------------------------------------+--------------------------+
| Ronald Miller Equirectagular                            | 50◦30'                   |
+---------------------------------------------------------+--------------------------+
| E.Gradarend and A. Niermann minimum linear distortion   | 61°7'                    |
+---------------------------------------------------------+--------------------------+


Example using latitude of true scale::

    $ echo 2 1 | proj +proj=eqc +a=6400000 +lat_1=0.5 +lat_2=2
    223402.14       111701.07

Mathematical definition
#######################

The formulas describing the Equidistant Cylindrical projection are all taken from proj4 sources.

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



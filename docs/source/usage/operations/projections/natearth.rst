.. _natearth:

********************************************************************************
Natural Earth
********************************************************************************
+---------------------+--------------------------------------------------------+
| **Classification**  | Pseudo cylindrical                                     |
+---------------------+--------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection              |
+---------------------+--------------------------------------------------------+
| **Defined area**    | Global                                                 |
+---------------------+--------------------------------------------------------+
| **Implemented by**  | Bernhard Jenny                                         |
+---------------------+--------------------------------------------------------+
| **Options**                                                                  |
+---------------------+--------------------------------------------------------+
| `No special options for this projection`                                     |
+---------------------+--------------------------------------------------------+

.. image:: ./images/natearth.png
   :scale: 50%
   :alt:   Natural Earth


The Natural Earth projection is intended for making world maps. A distinguishing trait
is its slightly rounded corners fashioned to emulate the spherical shape of Earth.
The meridians (except for the central meridian) bend acutely inward as they approach
the pole lines, giving the projection a hint of three-dimensionality. This bending
also suggests that the meridians converge at the poles instead of truncating at the
top and bottom edges. The distortion characteristics of the Natural Earth projection
compare favorably to other world map projections.


Usage
###############################################################################

The Natural Earth projection has no special options so usage is simple. Here is
an example of an inverse projection on a sphere with a radius of 7500 m::

    $ echo 3500 -8000 | proj -I +proj=natearth +a=7500
    37d54'6.091"E  61d23'4.582"S


Further reading
################################################################################

#. `Wikipedia  <https://en.wikipedia.org/wiki/Natural_Earth_projection>`_

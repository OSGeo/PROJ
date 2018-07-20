.. _webmerc:

********************************************************************************
WGS 84 Web Mercator / WGS 84 Pseudo Mercator
********************************************************************************

.. versionadded:: 5.1.0

The WGS 84 Web Mercator / WGS 84 Pseudo Mercator projection is a cylindrical map projection.
It is defined only for ellipsoid WGS 84, motivated by the regular :ref:`merc` projection, but the equations for ellispoid are chosen to be the same as equations for Mercator projection of a sphere, with radius same as semi-major axis of the WGS 84 ellipsoid. As a consequence it is not a conformal projection.

From `Wikipedia <https://en.wikipedia.org/wiki/Web_Mercator>`_:

    This projection is widely used by the Web Mercator, Google Web Mercator,
    Spherical Mercator, WGS 84 Web Mercator[1] or WGS 84/Pseudo-Mercator is a
    variant of the Mercator projection and is the de facto standard for Web
    mapping applications. [...]
    It is used by virtually all major online map providers [...]
    Its official EPSG identifier is EPSG:3857, although others have been used
    historically.


+---------------------+-----------------------------------------------------------------+
| **Classification**  | Non-conformal cylindrical projection of an WGS 84 ellipsoid     |
+---------------------+-----------------------------------------------------------------+
| **Available forms** | Forward and inverse, ellipsoidal projection                     |
+---------------------+-----------------------------------------------------------------+
| **Defined area**    | Global, low linear distortions near the equator                 |
+---------------------+-----------------------------------------------------------------+
| **Alias**           | webmerc                                                         |
+---------------------+-----------------------------------------------------------------+
| **Domain**          | 2D                                                              |
+---------------------+-----------------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                            |
+---------------------+-----------------------------------------------------------------+
| **Output type**     | Projected coordinates                                           |
+---------------------+-----------------------------------------------------------------+


Usage
########

Example::

    $ echo 2 49 | proj +proj=webmerc +datum=WGS84
    222638.98       6274861.39

Parameters
################################################################################

.. note:: Projection does not have any additional parameters. See `EPSG.io  <http://epsg.io/3857>`_


Mathematical definition
#######################

The formulas describing the Mercator projection are all taken from G. Evenden's libproj manuals [Evenden2005]_.

Forward projection
==================

.. math::

    x = \lambda

.. math::

    y = \ln \left[ \tan \left(\frac{\pi}{4} + \frac{\phi}{2} \right) \right]


Inverse projection
==================

.. math::

    \lambda = {x}

.. math::

    \phi = \frac{\pi}{2} - 2 \arctan \left[ e^{-y} \right]



Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Web_Mercator>`_




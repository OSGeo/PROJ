.. _merc:

********************************************************************************
Mercator
********************************************************************************

The Mercator projection is a cylindrical map projection that origins
from the 16th century. It is widely recognized as the first regularly
used map projection.  It is a conformal projection in which the equator
projects to a straight line at constant scale.  The projection has the
property that a rhumb line, a course of constant heading, projects to a
straight line.  This makes it suitable for navigational purposes.


+---------------------+----------------------------------------------------------+
| **Classification**  | Conformal cylindrical                                    |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, but best used near the equator                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | merc                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+

.. figure:: ./images/merc.png
   :width: 500 px
   :align: center
   :alt:   Mercator

   proj-string: ``+proj=merc``

Usage
########

Applications should be limited to equatorial regions, but is frequently
used for navigational charts with latitude of true scale (:option:`+lat_ts`) specified within
or near chart's boundaries.
It is considered to be inappropriate for world maps because of the gross
distortions in area; for example the projected area of Greenland is
larger than that of South America, despite the fact that Greenland's
area is :math:`\frac18` that of South America :cite:`Snyder1987`.


Example using latitude of true scale::

    $ echo 56.35 12.32 | proj +proj=merc +lat_ts=56.5
    3470306.37    759599.90

Example using scaling factor::

    echo 56.35 12.32 | proj +proj=merc +k_0=2
    12545706.61     2746073.80


Note that :option:`+lat_ts` and :option:`+k_0` are mutually exclusive.
If used together, :option:`+lat_ts` takes precedence over :option:`+k_0`.

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lat_ts.rst

.. include:: ../options/k_0.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

Mathematical definition
#######################

Spherical form
**************
For the spherical form of the projection we introduce the scaling factor:

.. math::

    k_0 = \cos \phi_{ts}

Forward projection
==================

.. math::

    x = k_0R \lambda; \qquad y = k_0R \psi

.. math::

    \psi &= \ln \tan \biggl(\frac{\pi}{4} + \frac{\phi}{2} \biggr)\\
         &= \sinh^{-1}\tan\phi

The quantity :math:`\psi` is the isometric latitude.


Inverse projection
==================

.. math::

    \lambda = \frac{x}{k_0R}; \qquad \psi = \frac{y}{k_0R}

.. math::

    \phi &= \frac{\pi}{2} - 2 \tan^{-1} \exp(-\psi)\\
         &= \tan^{-1}\sinh\psi


Ellipsoidal form
****************

For the ellipsoidal form of the projection we introduce the scaling factor:

.. math::

    k_0 = m( \phi_{ts} )

where

.. math::

   m(\phi) = \frac{\cos\phi}{\sqrt{1 - e^2\sin^2\phi}}

:math:`a\,m(\phi)` is the radius of the circle of latitude :math:`\phi`.

Forward projection
==================
.. math::

    x = k_0 a \lambda; \qquad y = k_0 a \psi

.. math::

    \psi &= \ln\tan\biggl(\frac\pi4 + \frac{\phi}2\biggr)
            -\frac12 e
            \ln \biggl(\frac{1 + e \sin\phi}{1 - e \sin\phi}\biggr)\\
         &= \sinh^{-1}\tan\phi - e \tanh^{-1}(e \sin\phi)


Inverse projection
==================

.. math::

    \lambda = \frac{x}{k_0 a}; \quad \psi = \frac{y}{k_0 a}

The latitude :math:`\phi` is found by inverting the equation for
:math:`\psi` iteratively.

Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Mercator_projection>`_
#. `Wolfram Mathworld <http://mathworld.wolfram.com/MercatorProjection.html>`_



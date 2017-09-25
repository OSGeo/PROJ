.. _cass:

********************************************************************************
Cassini (Cassini-Soldner)
********************************************************************************

Although the Cassini projection has been largely replaced by the Transverse Mercator, it is still in limited use outside the United States and was one of the major topographic mapping projections until the early 20th century.

+---------------------+-------------------------------------------------------------------------+
| **Classification**  | Transverse and oblique cylindrical                                      |
+---------------------+-------------------------------------------------------------------------+
| **Available forms** | Forward and inverse, Spherical and Elliptical                           |
+---------------------+-------------------------------------------------------------------------+
| **Defined area**    | Global, but best used near the central meridian with long, narrow areas |
+---------------------+-------------------------------------------------------------------------+
| **Implemented by**  | Gerald I. Evenden                                                       |
+---------------------+-------------------------------------------------------------------------+
| **Options**                                                                                   |
+---------------------+-------------------------------------------------------------------------+
| `+lat_0`            | Latitude of origin (Default to 0)                                       |
+---------------------+-------------------------------------------------------------------------+

.. image:: ./images/cass.png
   :scale: 50%
   :alt:   Cassini

Usage
#####

There has been little usage of the spherical version of the Cassini, but the ellipsoidal Cassini-Soldner version was adopted by the Ordnance Survey for the official survey of Great Britain during the second half of the 19th century [Steers1970]_.
Many of these maps were prepared at a scale of 1:2,500.
The Cassini-Soldner was also used for the detailed mapping of many German states during the same period.


Example using EPSG 30200 (Trinidad 1903, units in clarke's links)::

    $ echo 0.17453293 -1.08210414 | proj +proj=cass +lat_0=10.44166666666667 +lon_0=-61.33333333333334 +x_0=86501.46392051999 +y_0=65379.0134283 +a=6378293.645208759 +b=6356617.987679838 +to_meter=0.201166195164 +no_defs
    66644.94	82536.22

Example using EPSG 3068 (Soldner Berlin)::

    $ echo 13.5 52.4 | proj +proj=cass +lat_0=52.41864827777778 +lon_0=13.62720366666667 +x_0=40000 +y_0=10000 +ellps=bessel +datum=potsdam +units=m +no_defs
    31343.05	7932.76


Mathematical definition
#######################

The formulas describing the Cassini projection are taken from Snyder's [Snyder1987]_.

:math:`\phi_0` is the latitude of origin that match the center of the map (default to 0). It can be set with ``+lat_0``.


Spherical form
==============

Forward projection
------------------

.. math::

  x = \arcsin(\cos(\phi)  \sin(\lambda))

.. math::

  y = \arctan2(\tan(\phi), \cos(\lambda)) - \phi_0

Inverse projection
------------------

.. math::

  \phi = \arcsin(\sin(y+\phi_0) \cos(x))

.. math::

  \lambda = \arctan2(\tan(x), \cos(y+\phi_0))

Elliptical form
===============

Forward projection
------------------

.. math::

  N = (1 - e^2 \sin^2(\phi))^{-1/2}

.. math::

  T = \tan^2(\phi)

.. math::

  A = \lambda \cos(\phi)

.. math::

  C = \frac{e^2}{1-e^2} cos^2(\phi)

.. math::

  x = N ( A - T \frac{A^3}{6} - (8-T+8C)T\frac{A^5}{120})

.. math::

  y = M(\phi) - M(\phi_0) + N \tan(\phi)(\frac{A^2}{2} + (5-T+6C)\frac{A^4}{24})

and M() is the meridionial distance function.

Inverse projection
------------------

.. math::

  \phi' = M^{-1}(M(\phi_0)+y)

if :math:`\phi' = \frac{\pi}{2}` then :math:`\phi=\phi'` and :math:`\lambda=0`

otherwise evaluate T and N above using :math:`\phi'` and

.. math::

  R = (1 - e^2)(1 - e^2 sin^2 \phi')^{-3/2}

.. math::

  D = x/N

.. math::

  \phi = \phi' - \tan \phi' \frac{N}{R}(\frac{D^2}{2}-(1+3T)\frac{D^4}{24})

.. math::

  \lambda = \frac{(D - T\frac{D^3}{3} + (1+3T)T\frac{D^5}{15})}{\cos \phi'}

.. _further-reading:

Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Equirectangular_projection>`_
#. [Snyder1987]_
#. `EPSG, POSC literature pertaining to Coordinate Conversions and Transformations including Formulas  <http://www.ihsenergy.com/epsg/guid7.pdf>`_

.. _vertoffset:

================================================================================
Vertical Offset And Slope
================================================================================

.. versionadded:: 9.1.0

The Vertical Offset And Slope offsets transformation adds an offset to an
orthometric height, taking account a constant offset and a inclinated plane,
defined by its slope along latitude and longitude axis.

+---------------------+----------------------------------------------------------+
| **Alias**           | vertoffset                                               |
+---------------------+----------------------------------------------------------+
| **Domain**          | 3D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates (horizontal), meters (vertical)     |
+---------------------+----------------------------------------------------------+
| **output type**     | Geodetic coordinates (horizontal), meters (vertical)     |
+---------------------+----------------------------------------------------------+

It is documented as coordinate operation method code 1046 in the EPSG dataset (:cite:`IOGP2018`).
It is typically used in Europe, to relate national vertical systems to
pan-European vertical systems (EVRF200, EVRF2007).

Examples
###############################################################################

Vertical offset from LN02 height to EVRF2000 height with horizontal coordinates in ETRS89::

    +proj=vertoffset +lat_0=46.9166666666666666 +lon_0=8.183333333333334 \
        +dh=-0.245 +slope_lat=-0.210 +slope_lon=-0.032 +ellps=GRS80

Parameters
################################################################################

Required
-------------------------------------------------------------------------------

.. option:: +lat_0=<value>

    Latitude of origin of the inclinated plane.

.. option:: +lon_0=<value>

    Longitude of origin of the inclinated plane

.. option:: +dh=<value>

    Offset in height, expressed in meter, to add.

.. option:: +slope_lat=<value>

    Slope parameter in the latitude domain, expressed in arc-seconds.

.. option:: +slope_lon=<value>

    Slope parameter in the longitude domain, expressed in arc-seconds.

.. include:: ../options/ellps.rst

Formula
################################################################################

The :math:`Z_{dest}` destination elevation is obtained from the
:math:`Z_{src}` source elevation with:

.. math::

    \begin{align}
        Z_{dest} = Z_{src} + \left( dh + slope_{lat} * {\rho}_0 * (\phi - {\phi}_0) + slope_{lon} * {\nu}_0 * (\lambda - {\lambda}_0) * cos(\phi) \right)
    \end{align}

where:

* :math:`dh`, :math:`slope_{lat}` and :math:`slope_{lon}` are the above mentioned parameters
* :math:`{\lambda}_0`, :math:`{\phi}_0` is the longitude, latitude of the point of origin of the inclinate plane (``+lon_0``, ``+lat_0``)
* :math:`\lambda`, :math:`\phi` is the longitude, latitude of the point to evaluate
* :math:`{\rho}_0` is the radius of curvature of the meridian at latitude :math:`{\phi}_0`
* :math:`{\nu}_0` is the radius of curvature on the prime vertical (i.e. perpendicular to the meridian) at latitude :math:`{\phi}_0`

The reverse formula is:

.. math::

    \begin{align}
        Z_{src} = Z_{dest} - \left( dh + slope_{lat} * {\rho}_0 * (\phi - {\phi}_0) + slope_{lon} * {\nu}_0 * (\lambda - {\lambda}_0) * cos(\phi) \right)
    \end{align}

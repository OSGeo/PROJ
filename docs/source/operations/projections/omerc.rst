.. _omerc:

********************************************************************************
Oblique Mercator
********************************************************************************

.. image:: ./images/omerc.png
   :scale: 50%
   :alt:   Oblique Mercator

Parameters
################################################################################


Central point and azimuth method
--------------------------------------------------------------------------------

.. option:: +alpha=<value>

    Azimuth of centerline clockwise from north at the center point of the line.
    If :option:`+gamma` is not given then :option:`+alpha` determines the value of
    :option:`+gamma`.

.. option:: +gamma=<value>

    Azimuth of centerline clockwise from north of the rectified
    bearing of centre line. If :option:`+alpha` is not given, then
    :option:`+gamma` is used to determine :option:`+alpha`.

.. option:: +lonc=<value>

    Longitude of the central point.

.. option:: +lat_0=<value>

    Latitude of the central point.

Two point method
--------------------------------------------------------------------------------

.. option:: +lon_1=<value>

    Longitude of first point.

.. option:: +lat_1=<value>

    Latitude of first point.

.. option:: +lon_2=<value>

    Longitude of second point.

.. option:: +lat_2=<value>

    Latitude of second point.

Optional
--------------------------------------------------------------------------------

.. option:: +no_rot

    Do not rotate axis.

.. option:: +no_off

    Do not offset origin to center of projection.

.. include:: ../options/k_0.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

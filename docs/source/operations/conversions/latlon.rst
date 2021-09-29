.. _latlon:

********************************************************************************
Lat/long (Geodetic alias)
********************************************************************************

Passes geodetic coordinates through unchanged.

+---------------------+--------------------------------------------------------+
| **Aliases**         | latlon, latlong, lonlat, longlat                       |
+---------------------+--------------------------------------------------------+
| **Domain**          | 2D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                   |
+---------------------+--------------------------------------------------------+
| **Output type**     | Geodetic coordinates                                   |
+---------------------+--------------------------------------------------------+

.. note:: Can not be used with the :program:`proj` application.


Parameters
################################################################################

No parameters will affect the output of the operation if used on it's own.
However, the parameters below can be used in a declarative manner when used with
:program:`cs2cs` or in a :ref:`transformation pipeline <pipeline>` .

.. include:: ../options/ellps.rst

.. option:: +datum=<value>

    Declare the datum used with the coordinates. Available options are:
    ``WGS84``, ``GGRS87``, ``NAD38``, ``NAD27``, ``potsdam``, ``carthage``,
    ``hermannskogel``, ``ire65``, ``nzgd49``, ``OSGB336``.

    .. note::

        The ``+datum`` option is primarily available to support the legacy
        use of PROJ.4 strings as CRS descriptors and should in most cases
        be avoided.

.. option:: +towgs84=<list>

    A list of three or seven :ref:`Helmert <helmert>` parameters that maps
    the input coordinates to the WGS84 datum.

.. _mod_krovak:

********************************************************************************
Modified Krovak
********************************************************************************

.. versionadded:: 9.4.0

+---------------------+----------------------------------------------------------+
| **Classification**  | Conical                                                  |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Czech Republic                                           |
+---------------------+----------------------------------------------------------+
| **Alias**           | mod_krovak                                               |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/krovak.png
   :width: 500 px
   :align: center
   :alt:   Modified Krovak

   proj-string: ``+proj=mod_krovak``

Modified Krovak builds upon traditional :ref:`krovak`, with corrective terms that
are better suited when using it with the S-JTSK/05 datum. This method is specific
to the Czech Republic. Due to the corrective terms, this projection method is
no longer strictly conformal.

By default, coordinates in the forward direction are output in easting, northing,
and negative in the Czech Republic, with absolute value of easting/westing
being smaller than absolute value of northing/southing.
To distinguish it from regular Krovak, the usual value for ``+x_0`` and ``+y_0``
in Modified Krovak is typically 5,000,000.

Parameters
################################################################################

.. note:: All parameters are optional for the Modified Krovak projection.

          The latitude of pseudo standard parallel is hardcoded to 78.5° and
          the ellipsoid to Bessel.

.. option:: +czech

    Reverse the sign of the output coordinates, as is tradition in the
    Czech Republic, to be westing, southing (positive values in Czech Republic)

.. option:: +lon_0=<value>

    Longitude of projection center.

    *Defaults to 24°50' (24.8333333333333)*

.. option:: +lat_0=<value>

    Latitude of projection center.

    *Defaults to 49.5*

.. option:: +k_0=<value>

    Scale factor. Determines scale factor used in the projection.

    *Defaults to 0.9999*

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

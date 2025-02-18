.. _spilhaus:

********************************************************************************
Spilhaus
********************************************************************************

.. versionadded:: 9.6.0

+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical, ellipsoidal              |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | spilhaus                                                 |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/spilhaus.png
   :width: 500 px
   :align: center
   :alt:   Spilhaus

   proj-string: ``+proj=spilhaus``

Parameters
################################################################################

.. note:: All parameters are optional.

.. option:: +lon_0=<value>

    Longitude of projection centre.

    *Defaults to 66.94970198.*

    .. note::
        The default convention is to interpret this value as decimal degrees. To
        specify radians instead, follow the value with the "r" character.

        Example: `+lat_0=1.570796r`

        See :ref:`Projection Units <projection_units>` for more information.

.. option:: +lat_0=<value>

    Latitude of projection centre.

    *Defaults to -49.56371678.*

    .. note::
        The default convention is to interpret this value as decimal degrees. To
        specify radians instead, follow the value with the "r" character.

        Example: `+lat_0=1.570796r`

        See :ref:`Projection Units <projection_units>` for more information.

.. option:: +azi=<value>

    Azimuth from north at the center of projection.

    *Defaults to 40.17823482.*

.. option:: +rot=<value>

    Rotation of the projection.

    *Defaults to 45.*

.. include:: ../options/R.rst

.. option:: +k_0=<value>

    Scale factor. Determines scale factor used in the projection.
    To reproduce the output of ESRI:54099, use +k_0=1.41421356237 (sqrt(2))

    *Defaults to 1.0.*

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

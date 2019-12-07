.. _isea:

********************************************************************************
Icosahedral Snyder Equal Area
********************************************************************************

The Icosaheadra Snyder Equal Area projection is a series of equal-area map
projections. Lines of constant distortion follow approximately oval or
rectangular paths instead of the circles of the Lambert Azimuthal Equal-Area
projection or the straight lines of the Cylindrical Equal-Area projection. The
projection series permits design of equal-area maps of oblong regions with less
overall distortion of shape and scale than equal-area maps on other projections
:cite:`Snyder1988`.

+---------------------+----------------------------------------------------------+
| **Classification**  | Polyhedral, equal Area                                   |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward, spherical                                       |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | isea                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/isea.png
   :width: 500 px
   :align: center
   :alt:   Icosahedral Snyder Equal Area

   proj-string: ``+proj=isea``

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. option:: +orient=<string>

    Can be set to either ``isea`` or ``pole``.

.. option:: +azi=<value>

    Azimuth.

    *Defaults to 0.0*

.. option:: +aperture=<value>

    *Defaults to 3.0*

.. option:: +resolution=<value>

    *Defaults to 4.0*

.. option:: +mode=<string>

    Can be either ``plane``, ``di``, ``dd`` or ``hex``.

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

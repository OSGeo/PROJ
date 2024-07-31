.. _isea:

********************************************************************************
Icosahedral Snyder Equal Area
********************************************************************************

Snyder's Icosahedral Equal Area map projection on an icosahedron polyhedral globe
offers relatively low scale and angular distortion. The equations involved are
relatively straight-forward. The interruptions remain a disadvantage, as with
any low-error projection system applied to the entire globe :cite:`Snyder1992`.
This projection is used as a basis for defining discrete global grid hierarchies.

+---------------------+----------------------------------------------------------+
| **Classification**  | Polyhedral, equal area                                   |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical                           |
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

.. note::
   As the projection is only defined on a sphere, it should only be used
   with a spherical ellipsoid e.g., ``+R=6371007.18091875`` for a sphere with the
   authalic radius of the WGS84 ellipsoid. For mapping coordinates on the WGS84
   ellipsoid to the authalic sphere, the input latitude should be converted
   from geodetic latitude to authalic latitude. A future version may
   automatically perform this conversion when using a non-spherical ellipsoid.

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. option:: +orient=<string>

    Can be set to either ``isea`` or ``pole``.  See Snyder's Figure 12 for pole orientation :cite:`Snyder1992`.

    *Defaults to isea*

.. option:: +azi=<value>

    Azimuth.

    Not supported by the inverse.

    *Defaults to 0.0*

.. option:: +aperture=<value>

    Not supported by the inverse.

    *Defaults to 3.0*

.. option:: +resolution=<value>

    Not supported by the inverse.

    *Defaults to 4.0*

.. option:: +mode=<string>

    Can be either ``plane``, ``di``, ``dd`` or ``hex``.

    Only ``plane`` supported by the inverse.

    *Defaults to plane*

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. _som:

********************************************************************************
Space Oblique Mercator (SOM)
********************************************************************************

The Space Oblique Mercator (SOM) projection is a generalization of the Oblique
Mercator projection, itself an (oblique) aspect of the Mercator projection with
origins in the 16th century. In the previously available map projections, ground
tracks of satellites traced curved lines which complicated their use in
representing satellite remote sensing data. In response, SOM was specifically
designed to represent satellite ground tracks as straight lines. This had the
effect of minimizing distortions of data along a ground track and allowing
satellite data to be represented accurately as a rectangular array.
 


+---------------------+----------------------------------------------------------+
| **Classification**  | Conformal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | som                                                      |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/som.png
   :width: 500 px
   :align: center
   :alt:   Space Oblique Mercator (SOM)


   proj-string: ``+proj=som +inc_angle=98.303820000243860022 +ps_rev=0.06866666666666667 +asc_lon=64.412896137498847793``

   proj-string: ``+proj=som +inc_angle=1.7157253262878522r +ps_rev=0.06866666666666667 +asc_lon=1.1242171183417042r``

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

.. option:: +asc_lon=<value>

    Longitude of the ascending node for the orbit (decimal degrees).

.. option:: +inc_angle=<value>

    Inclination (vertical tilt) of the orbit with respect to the reference plane, measured at the ascending node (decimal degrees).

.. option:: +ps_rev=<value>

    Time required for a single orbit (days).

.. note::

    To specify asc_lon or inc_angle in radians (instead of degrees). Follow the value with the "r" character.

    As an example, to specify the ascending longitude as 1.1242171183417042 radians (~64.4129 decimal degrees): `+asc_lon=1.1242171183417042r`.

   For more information see :doc:`projections usage<../../usage/projections>`.

Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Further reading
###############

#. `Space Oblique Mercator Wikipedia <https://en.wikipedia.org/wiki/Space-oblique_Mercator_projection>`_
#. `USGS Report on Space Oblique Mercator Mathematical Development <https://web.archive.org/web/20211213011807/https://pubs.usgs.gov/bul/1518/report.pdf>`_

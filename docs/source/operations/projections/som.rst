.. _som:

********************************************************************************
Space Oblique Mercator (SOM)
********************************************************************************

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

   proj-string: ``+proj=som +inc_angle=1.7157253262878522 +ps_rev=0.06866666666666667 +asc_lon=1.1242171183417042``

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

.. option:: +asc_lon=<value>

    Longitude of the ascending node for the orbit (radians).

.. option:: +inc_angle=<value>

    Inclination (vertical tilt) of the orbit with respect to the reference plane, measured at the ascending node (radians).

.. option:: +ps_rev=<value>

    Time required for a single orbit (days).


Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

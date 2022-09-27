.. _misrsom:

********************************************************************************
Space oblique for MISR
********************************************************************************

Space oblique for MISR is a specialization of :doc:`Space Oblique Mercator<som>`
which specifies the appropriate ascending longitude, inclination angle, and orbital
period for Multi-angle Imaging Spectroradiometer (MISR) orbits, based on the path
number for the orbit.

"Paths" are the repeating ground tracks that are traced by sun-synchronous
satellites like Terra on which the MISR instrument is aboard.


+---------------------+----------------------------------------------------------+
| **Classification**  | Conformal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | misrsom                                                  |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/misrsom.png
   :width: 500 px
   :align: center
   :alt:   Space oblique for MISR

   proj-string: ``+proj=misrsom +path=1``

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

.. option:: +path=<value>

    Selected path of satellite. Value between 1 and 233.


Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

#. `MISR Wikipedia <https://en.wikipedia.org/wiki/Landsat_program>`_
#. `Terra Wikipedia <https://en.wikipedia.org/wiki/Terra_(satellite)>`_
#. :doc:`Space Oblique Mercator Projection<som>`


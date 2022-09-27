.. _lsat:

********************************************************************************
Space oblique for LANDSAT
********************************************************************************

Space oblique for LANDSAT is a specialization of :doc:`Space Oblique Mercator<som>`
which specifies the appropriate ascending longitude, inclination angle, and 
orbital period for Landsat satellites, based on their satellite designation / id
and path number.

"Paths" are the repeating ground tracks that are traced by sun-synchronous
satellites like those of the Landsat program.

+---------------------+----------------------------------------------------------+
| **Classification**  | Cylindrical                                              |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | lsat                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/lsat.png
   :width: 500 px
   :align: center
   :alt:   Space oblique for LANDSAT

   proj-string: ``+proj=lsat +ellps=GRS80 +lat_1=-60 +lat_2=60 +lsat=2 +path=2``

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

.. option:: +lsat=<value>

    Landsat satellite used for the projection. Value between 1 and 5.

.. option:: +path=<value>

    Selected path of satellite. Value between 1 and 253 when :option:`+lsat` is
    set to 1,2 or 3, otherwise valid input is between 1 and 233.


Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Further reading
###############

#. `Landsat Wikipedia <https://en.wikipedia.org/wiki/Landsat_program>`_
#. :doc:`Space Oblique Mercator Projection<som>`


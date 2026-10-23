.. _rrdot:

********************************************************************************
Range Doppler
********************************************************************************
+---------------------+--------------------------------------------------------+
| **Classification**  | Miscellaneous                                          |
+---------------------+--------------------------------------------------------+
| **Available forms** | Forward and inverse.                                   |
+---------------------+--------------------------------------------------------+
| **Defined area**    | Global                                                 |
+---------------------+--------------------------------------------------------+
| **Alias**           | rrdot                                                  |
+---------------------+--------------------------------------------------------+
| **Domain**          | 3D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | 3D coordinates                                         |
+---------------------+--------------------------------------------------------+
| **Output type**     | Projected coordinates                                  |
+---------------------+--------------------------------------------------------+

   proj-string: ``+proj=rrdot +x=XX +y=XX +z=XX +vx=XX +vy=XX +vz=XX``

The Range Doppler coordinate system is a sensor aligned coordinate system used
by  radar mapping missions. A number of Synthetic Aperture Radar (SAR) data
providers distribute high resolution Level-1 data in Polar Format slant range
geometry and this projection system can be directly used to describe the whole
image. Wide area imaging missions like  Sentinel-1 and NISAR use a time 
dependent coordinate system and this coordinate can be used to describe the 
imaging geometry of a single slant range line.

See :cite:`NISAR_2023`


Parameters
################################################################################

Required
-------------------------------------------------------------------------------

.. option:: +x=<value>

    Platform position along X-coordinate (in metre)

.. option:: +y=<value>

    Platform position along Y-coordinate (in metre)

.. option:: +z=<value>

    Platform position along Z-coordinate (in metre)

.. option:: +vx=<value>

    Platform velocity along X-coordinate (in metre-per-sec)

.. option:: +vy=<value>

    Platform velocity along Y-coordinate (in metre-per-sec)

.. option:: +vz=<value>

    Platform velocity along Z-coordinate (in metre)

Optional
-------------------------------------------------------------------------------

.. option:: +left

    Add this flag when using the platform is left-looking.

    *Default assumes a right-looking platform*

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

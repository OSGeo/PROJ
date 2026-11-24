.. _alsk:

********************************************************************************
Modified Stereographic of Alaska
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Modified azimuthal                                       |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Alaska                                                   |
+---------------------+----------------------------------------------------------+
| **Alsk**            | alsk                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/alsk.png
   :width: 500 px
   :align: center
   :alt:   Modified Stereographic of Alaska

   proj-string: ``+proj=alsk``

Options
################################################################################

.. note:: All options are optional for the projection.

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

A nonzero ellipsoid eccentricity selects the fixed ellipsoidal form, with
semi-major axis 6378206.4 m and squared eccentricity 0.00676866. A sphere selects
the spherical form with a fixed radius of 6370997 m.

.. include:: ../options/ellps.rst

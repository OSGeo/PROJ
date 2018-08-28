.. _isea:

********************************************************************************
Icosahedral Snyder Equal Area
********************************************************************************

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

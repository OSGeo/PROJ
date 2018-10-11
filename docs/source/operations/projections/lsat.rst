.. _lsat:

********************************************************************************
Space oblique for LANDSAT
********************************************************************************

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

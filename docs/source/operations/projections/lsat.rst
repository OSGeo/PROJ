.. _lsat:

********************************************************************************
Space oblique for LANDSAT
********************************************************************************

.. image:: ./images/lsat.png
   :scale: 50%
   :alt:   Space oblique for LANDSAT

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

.. option:: +lsat=<value>

    Landsat satelite used for the projection. Value between 1 and 5.

.. option:: +path=<value>

    Selected path of satelite. Value between 1 and 253 when :option:`+lsat` is
    set to 1,2 or 3, otherwise valid input is between 1 and 233.


Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

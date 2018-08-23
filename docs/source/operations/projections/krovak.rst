.. _krovak:

********************************************************************************
Krovak
********************************************************************************

.. figure:: ./images/krovak.png
   :width: 500 px
   :align: center
   :alt:   Krovak

   proj-string: ``+proj=krovak``

Parameters
################################################################################

.. note:: All parameters are optional for the Krovak projection.

.. option:: +czech

    Reverse the sign of the output coordinates, as is tradition in the
    Czech Republic.

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. option:: +k_0=<value>

    Scale factor. Determines scale factor used in the projection.

    *Defaults to 0.9999.*

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

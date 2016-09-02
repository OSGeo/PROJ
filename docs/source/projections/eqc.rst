.. _eqc:

********************************************************************************
Equidistant Cylindrical (Plate Caree)
********************************************************************************

.. image:: ./images/eqc.png
   :scale: 50%
   :alt:   Equidistant Cylindrical (Plate Caree)  

Marinus of Tyre was attributed this equirectangular projection :

.. math::

   x = \lambda cos \phi1 

.. math::

   y = \phi - \phi0

.. math::

   \phi0 = the center of the map
   \phi1 = standard parallels where the scale of the projection is true 

Reverse operation :

.. math::

   \lambda = x / cos \phi1

.. math::

   \phi = y + \phi0

.. math::

   \phi0 = the center of the map
   \phi1 = standard parallels where the scale of the projection is true


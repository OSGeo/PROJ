.. _eqc:

********************************************************************************
Equidistant Cylindrical (Plate Caree)
********************************************************************************

.. image:: ./images/eqc.png
   :scale: 50%
   :alt:   Equidistant Cylindrical (Plate Caree)  

The simplist of all projections. Standard parallels (0° when omitted) may be specified that determine latitude of true scale (k=h=1).

.. math::

   x = \lambda \cos \phi_{ts}

.. math::

   y = \phi - \phi_0

Reverse operation :

.. math::

   \lambda = x / cos \phi_{ts}

.. math::

   \phi = y + \phi_0


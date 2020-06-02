.. _euref2019_helmert:

The Helmert transformation
++++++++++++++++++++++++++++

In this set of exercises we investigate the Helmert Transformation and some of
its properties. The Helmert transformation covers a wide range of configurations,
from simple translations of coordinates to the fully-fledged spatiotemporal
transformation. Consult `Wikipedia <https://en.wikipedia.org/wiki/Helmert_transformation>`_
and :ref:`helmert` for in-depth technical details on the transformation.

The Helmert transformation works on geocentric, cartesian coordinates. See the
:ref:`euref2019_conversions` exercise for more info. In this exercise all coordinates are
already given as cartesian coordinates, so you only need to focus on the actual
transformation setup.

Download the :program:`gie` file for the exercise: :download:`helmert.gie <helmert.gie>`.

1. Simple coordinate translation
-------------------------------------------------------------------------------

The most basic application of the Helmert transformation is a translation of
the x-, y- and z-components of the coordinate. This is rarely a good fit for
large areas but locally a 3 parameter Helmert shift can be very effective. Even
though the 3 parameter translation doesn't fit particularly well in large areas
of use it is still commonly used between legacy and modern systems. In this
exercise we will use a transformation between ED50 in Italy (Sardinia) and
WGS84.


.. hint::
    You can find the transformations parameters by running the following
    command::

        projinfo -o WKT2_2019 -k operation EPSG:1142

    which returns the WKT2 definition of the transformation.

.. hint:: 
    Remember that the input to the Helmert transform is cartesian geocentric
    coordinates (x, y z).

.. hint::
    See :ref:`helmert` to find out how to set up the transformation

.. literalinclude:: helmert.gie
  :lines: 53-57

2.  Translation and rotation
------------------------------

For larger areas it is not enough to just rely on translation of coordinates.
In that case the solution is to include rotations of the axes and a scale
adjustment.
The rotational aspect of the transformation is handled by a set of three
rotation matrices. Two conventions for the application of the rotations are
in common use: Position Vector and Coordinate Frame. In this exercise we will
using Position Vector. See the PROJ documentation on the Helmert transformation
for further detail.


.. hint::
    
    You can find the transformations parameters by running the following
    command::

        projinfo -o WKT2_2019 -k operation EPSG:1626

.. hint:: Remember to specify the rotation convention with +convention

.. literalinclude:: helmert.gie
  :lines: 85-89

3. Position vector/Coordinate frame
------------------------------------

Effectively, the difference between the two rotation convetions is the sign of
the rotation parameters. In this exercise we will examine this property by
looking at a transformation between the German DHDN reference system and ETRS89.

First find the parameters for the EPSG:1309 transformation. In 3a below, enter
the parameters as reported by projinfo (using the coordinate frame convention).
Adapt the parameters to the position vector convention in 3b.

.. hint::

    You can find the transformations parameters by running the following
    command::

        projinfo -o WKT2_2019 -k operation EPSG:1309

.. literalinclude:: helmert.gie
  :lines: 114-125

4. Kinematic transformation between ITRF2008 and ITRF2014
----------------------------------------------------------

The Helmert transformation also exists in a kinematic, or spatiotemporal,
version. This takes the evolution of a coordinate reference system over time
into account as well. This is done by pivoting about a reference epoch. The
14-parameter spatiotemporal Helmert is used when super high accuracy
transformations are needed. A good example of such a use case is transformations
between various realizations of ITRS and ETRS89. In this exercise we look at the
transformation from ITRF2008 to ITRF2014.

.. hint::
    
    You can find the transformations parameters by running the following
    command::

        projinfo -o WKT2_2019 -s ITRF2008 -t ITRF2014

    which returns the WKT2 definition of the transformation.

.. hint:: Not all parameters need to be set.

.. hint:: 

    Pay attention to the units of the parameteres output by projinfo,
    you may have to convert them to standard units (e.g. mm -> m).

.. hint:: 

    Note that the coordinates in the test now also includes a time tag in the
    form of a decimalyear - This is always needed when doing spatiotemporal
    transformations in PROJ. The time tag is the observation time of the
    coordinate. Note that the time component of the coordinate is never
    affected by the transformation.


.. literalinclude:: helmert.gie
  :lines: 164-169

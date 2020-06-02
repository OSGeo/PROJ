.. _euref2019_conversions:

Coordinate conversions
++++++++++++++++++++++++++++

In ISO19111 terms, a coordinate conversion is an operation that changes
coordinates in a source coordinate reference system to coordinates in a target
coordinate reference system in which both coordinate reference systems are based
on the same datum. Technically this is also true for PROJ although the term is
usually narrowed a bit, excluding projections since those are the origin of
PROJ they get special treatment.

In this exercise we will be looking at three coordinate conversions that in
combination with other operations are very powerful: Unit conversions, axis
swapping and geodetic to cartesian conversion.  Here we only look at the
conversions by themselves which isn't particularly useful, apart from the
educational purpose. When used in transformation pipelines they reveal their
true power. We will explore that further in the pipelines.gie exercises.

See :ref:`conversions_list` for a list of all conversions available in PROJ.

Download the :program:`gie` file for the exercise: :download:`conversions.gie <conversions.gie>`.

1. Unit conversion from meters to feet
----------------------------------------

Many projected coordinate systems are defined in terms of units other than the
meter. In this exercise we will transform the horizontal part of a coordinate
from meters to feet.

.. hint::

    Consult :ref:`unitconvert` to find out how to set up the transformation

.. hint:: You can use ``proj -lu`` to learn which units is supported by PROJ.

.. hint::

    Note that the horizontal, vertical and temporal parts are treated
    separately by the unit convert operator.

.. literalinclude:: conversions.gie
  :lines: 45-49


2. Swapping axes
------------------

Many coordinate reference systems are defined such that the axis order is
different than the (east, north, up, time) PROJ defaults to. An example of this
is the standard representation of a latitude/longitude-pair, where the north
component of the coordinate comes first. For this reason we need an operation
that can swap the axes around so that a given coordinate reference system can
be represented correctly, conforming to the intention of the defining authority.

Set up an operation that puts a longitude/latitude pair on the standard
latitude/longitude form.

.. hint::

    Consult :ref:`axisswap` to find out how to set up the transformation

.. hint::

    Remember that the axisswap operation doesn't know anything about the
    nature of the coordinate that is passed to it - it only cares about the
    the order of the input.

.. literalinclude:: conversions.gie
  :lines: 77-81

3. Geodetic to cartesian conversion
------------------------------------
Some transformations, most notably the Helmert transformation, operate on
cartesian geocentric coordinates. Geodetic coordinates (latitude and longitude)
is the most commonly used coordinate representation. It is only natural to have
a way to convert between the two representations.

Set up a transformation that convert geodetic coordinates to cartesian
geocentric coordinates on the Hayford ellipsoid.

.. hint::

    Consult :ref:`cart` to find out how to set up the transformation

.. hint:: Remember that `proj -le` returns a list of available ellipsoid models

.. hint::
    Remember that the Hayford ellipsoid is known under a number of other
    names - most of them including the term "international".


.. literalinclude:: conversions.gie
  :lines: 109-113

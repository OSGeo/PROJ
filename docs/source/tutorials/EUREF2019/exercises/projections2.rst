.. euref2019_projections2:

The Lambert Conic Conformal projection
++++++++++++++++++++++++++++++++++++++

In this exercise we will introduce the Lambert Conic Conformal (LCC) projection
as well as general map projection concepts such as projection center, scale
factor, false easting/northing and ellipsoid models.

We will do so by defining various versions of the LCC optimised for use in and
around the Gulf of Finland.

In case you are not familiar with the LCC projection, `Wikipedia`_ describes
the Lambert Conic Conformal projection as:

    "A Lambert conformal conic projection (LCC) is a conic map projection used for
    aeronautical charts, portions of the State Plane Coordinate System, and many
    national and regional mapping systems. It is one of seven projections introduced
    by Johann Heinrich Lambert in his 1772 publication Anmerkungen und Zusätze zur
    Entwerfung der Land- und Himmelscharten (Notes and Comments on the Composition
    of Terrestrial and Celestial Maps).

    Conceptually, the projection seats a cone over the sphere of the Earth and
    projects the surface conformally onto the cone. The cone is unrolled, and the
    parallel that was touching the sphere is assigned unit scale. That parallel is
    called the reference parallel or standard parallel.

    By scaling the resulting map, two parallels can be assigned unit scale, with
    scale decreasing between the two parallels and increasing outside them. This
    gives the map two standard parallels. In this way, deviation from unit scale can
    be minimized within a region of interest that lies largely between the two
    standard parallels. Unlike other conic projections, no true secant form of the
    projection exists because using a secant cone does not yield the same scale
    along both standard parallels"

.. _`Wikipedia`: https://en.wikipedia.org/wiki/Lambert_conformal_conic_projection

Download the :program:`gie` file for the exercise: :download:`projections2.gie <projections2.gie>`.

Exercise 1: Standard parallel
------------------------------

Set up a LCC projection with one standard parallel placed halfway between
Helsinki and Tallinn.

.. hint::
  
  * Approximate location of Helsinki: 60.171N 24.938E
  * Approximate location of Tallinn: 59.437N 24.745E
  * Approximate mid point between Helsinki and Tallinn: 59.8N 24.8E


.. hint:: See :ref:`lcc` for more on the Lambert Conformal Conic projection


.. literalinclude:: projections2.gie
  :lines: 54-61

Exercise 2: Distance distortion
--------------------------------

Improve the distance distortion of the projection.

The projection defined above is conformal, i.e. has no angular distortions
for infinitesimal objects - but there is a minor distortion of distances.
This cannot be avoided in general, but locally distortions can be minimized
by adjusting the scale factor.

With the default factor of 1, the distance error is 0  for (infinitesimal)
line segments at the latitude of the standard parallel.

By reducing the scale factor, the line of zero distance distortion degenerates
into two lines, north and south of the standard parallel.

For the abovementioned point in Tallinn, the distance error is around 0.002%,
but this can be reduced to zero by varying the scaling factor.

Even by trial and error it is not hard to reduce it to a few millionths of a
percent. So try to find a more optimal scaling factor by trial and error using
the projection analysis functionality of the proj application.

.. hint::

  Modify and use the command below to find a better scaling factor::

      echo 24.745 59.437 | proj -V <your projection setup here>

.. hint::  Look for the lines displaying the meridian and parallel scale.

.. hint:: Consult the LCC documentation to find out how to set the scaling factor.

.. hint:: Scaling factors usually deviates from unity by the order of 1e-3 to 1e-5


.. literalinclude:: projections2.gie
  :lines: 99-103

Exercise 3: Using two standard parallels
-----------------------------------------

Set up a similar LCC projection, this time using two standard parallels.
The first passing through Helsinki, the second through Tallinn.

.. hint::
  
  * Approximate location of Helsinki: 60.171N 24.938E
  * Approximate location of Talinn:   59.437N 24.745E

.. literalinclude:: projections2.gie
  :lines: 119-123

Exercise 4: Projection center
-----------------------------

Change the projection center to the midway point between Helsinki and Tallinn.

By changing the projection center we move the origin of the output
coordinates to be coinciding with the position where the projection is
most accurate.

.. hint:: Start with the projection you designed in exercise 1 above

.. hint:: The mid point between Helsinki and Tallinn is roughly 59.8N 24.8E

.. hint:: :ref:`LCC` explains how to change the projection center


.. literalinclude:: projections2.gie
  :lines: 143-147


Exercie 5: False easting and northing
--------------------------------------

Introduce false eastings and northings.

Oftentimes it is preferable to keep coordinate values positive even when
moving west and south of the projection center. This can be achieved by
introducing false easting and northing. Add false easting and northing
of 1000000

.. hint:: :ref:`LCC` explains how to set false easting/northing

.. literalinclude:: projections2.gie
  :lines: 165-169

.. _euref2019_projections3:

The Transverse Mercator projection
+++++++++++++++++++++++++++++++++++

In this exercise we will introduce the Transverse Mercator projection, the
UTM projection and the relationship between the two.

In addition we will investigate the differences between the two Transverse
Mercator implementations available in PROJ and when one should be used in
favour of the other.

Download the :program:`gie` file for the exercise: :download:`projections3.gie <projections3.gie>`.

Exercise 1. Transverse Mercator with default parameters
--------------------------------------------------------

Set up a Transverse Mercator projection using the default parameters.

.. hint:: Consult :ref:`tmerc`

.. literalinclude:: projections3.gie
  :lines: 25-29

Exercise 2: Use the Transverse Mercator to model the UTM projection
----------------------------------------------------------------------

The backbone of the UTM projection is a Transverse Mercator projection. In
this exercise we will model the behaviour of the UTM projection using the
Transverse Mercator.


.. hint:: 
  Remember that the UTM projection on the Northern Hemisphere uses a scale
  factor of 0.9996, a false easting of 500000 and a false northing of 0.
  UTM on the Southern Hemisphere is similar but with a false northing of
  10000000.

.. hint:: 
  The projection center is determined from the UTM zone and can be
  determined by zone*6 - 183

.. hint:: Tranverse Mercator parameters are documented at :ref:`tmerc`

.. literalinclude:: projections3.gie
  :lines: 60-64

Exercise 3: The less accurate, but faster, version of the Tranverse Mercator
-----------------------------------------------------------------------------

As mentioned in the introduction to this set of exercises, two versions of the
Transverse Mercator is implemented in PROJ. The default uses the Engsager/Poder
algorithm which is accurate far away from the central meridian of the
projection. The downside to this accuracy is that the algorithm is slower. The
alternative algorithm, which is toggled by the +approx parameter, is faster but
usage is recommended only within a few degrees away from the central meridian.

In this and the following exercises we will explore the accuracy of the two
algorithms by checking the roundtrip stability of a number of transformations.
A coordinate in Greenland will be used, as it is common practice to store
geospatial data covering the whole country in the same UTM zone. This is only
possible when using the correct algorithm. For the sake of simplicity,
all operations in the following exercises are expressed as UTM projections. The
UTM projection also has the +approx parameter which toggles the use of the
faster, less accurate transverse mercator algorithm.

We will try to determine the approximate roundtrip accuracy of the +approx
algorithm several UTM zones away from the actual zone for the given coordinate.
For all the exercises below the aim is to find the lowest tolerance for each
roundtrip test. You can of course make alle tests pass by setting a tolerance of
1000 km - that's not the point: How low can you go?

After you have answered all exercise 3 questions below, based on your findings
consider in which situation use of each of the algorithms is  appropriate.

.. hint:: 
  :program:`gie` accepts most common SI unit prefixes to the meter when
  specifying the tolerance, e.g. km, m, dm, cm, mm, um, nm.

.. hint:: 
  Look at the output :program:`gie` produces - the difference between the
  actual result and the expected result is reported when tests fail.

Exercise 3a
===========

As a baseline, determine the roundtrip accuracy of the default algorirthm
using UTM zone 22.

.. literalinclude:: projections3.gie
  :lines: 112-117

Exercise 3b
===========

Determine the roundtrip accuracy of the ``+approx`` algorithm using UTM zone 22:

.. literalinclude:: projections3.gie
  :lines: 121-125

Exercise 3c
===========

Determine the roundtrip accuracy of the ``+approx`` algorithm using UTM zone 23.

.. literalinclude:: projections3.gie
  :lines: 130-134

Exercise 3d
===========

Determine the roundtrip accuracy of the ``+approx`` algorithm using UTM zone 24:

.. literalinclude:: projections3.gie
  :lines: 139-143

Exercise 3e
============

Determine the roundtrip accuracy of the ``+approx`` algorithm using UTM zone 25:

.. literalinclude:: projections3.gie
  :lines: 148-152


Exercise 3f
===========

Determine the roundtrip accuracy of the ``+approx`` algorithm using UTM zone 26:

.. literalinclude:: projections3.gie
  :lines: 157-161

Exercise 3g
============

Determine the roundtrip accuracy of the ``+approx`` algorithm using UTM zone 27:

.. literalinclude:: projections3.gie
  :lines: 166-170


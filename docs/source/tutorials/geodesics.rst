.. _geodesics_tutorial:

Geodesics and planar approximations using PROJ and geod
++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The PROJ system supports computations with :ref:`geodesic`, through API calls or
through the :ref:`geod` command line interface. This exercise will proceed mostly
by having you copy sample commands from the text onto the command line, and
noting the results.

The tutorial is written with the Windows command line in mind but it should
be straight forward to use it on Unix-based systems as well.

.. hint:: 

    Users of Unix-based systems can replace ``set`` with ``export`` and ``%VAR%``
    with ``$VAR`` to follow the tutorial.

1. Objective
------------------------------------------------------------------------------

The objective of this exercise is to investigate how well a straight line in
a projected plane approximates the geodesic between the same end points.


2. The geodesic
------------------------------------------------------------------------------

The geodesic we will consider runs from Helsinki to Tallinn, using the
following coordinates:

.. code-block::

    Helsinki  60.171 N 24.938 E
    Tallinn   59.437 N 24.745 E

So to save some typing, let's define some environment variables. On Windows:

.. code-block::

    set HEL="60.171  24.938"
    set TAL="59.437  24.745"

.. code-block::

    export HEL="60.171  24.938"
    export TAL="59.437  24.745"
    
(i.e. mark, copy, and paste these lines into the command prompt)

First, we want to solve the "inverse geodetic problem" for the geodesic,
i.e. finding the azimuth and distance between the two points::

    echo %HEL% %TAL% | geod -I +ellps=GRS80

where the ``-I`` option indicates "the inverse geodetic problem".

:program:`geod` replies with these 3 numbers::

    -172d22'12.772"    7d27'46.693"    82488.500

The first is the forward azimuth from Helsinki to Tallinn, the second is the
return azimuth, while the third is the distance in meters.

We would actually rather have the azimuths in fractional degrees, so we provide
an explicit output format, using the ``-f`` option to geod::

    echo %HEL% %TAL% | geod -I -f "%.12f" +ellps=GRS80

getting us::

    -172.370214337896    7.462970382137    82488.500

We assert that the planar approximation is worst at the midpoint of the
geodesic, i.e. ``(82488.5 m) / 2  =  41244.25 m``, from Helsinki. So now, we
want to find the coordinates of this point, by solving the "direct geodetic
problem"::

    echo %HEL%  -172.370214337896    41244.25 | geod -f "%.12f" +ellps=GRS80

resulting in::

    59.804045696236    24.840439590098    7.545306054713

i.e. the latitude, the longitude, and the return azimuth of the mid point
of the geodesic.

Now, for convenience, define this environment variable::

    set MID="59.804045696236  24.840439590098"

.. note::
    
    You can save a bit of manual copying by capturing the geod output into
    the clipboard, using the "clip" command on Windows, i.e. by appending ``| clip``

to the ``geod`` command above.


1. The planar approximation
------------------------------------------------------------------------------

We will be working on ETRS89/UTM35 coordinates, so first define an environment
variable, to save some typing::

    set utm35=proj -rs +proj=utm +zone=35 +ellps=GRS80

.. note:: 
    The ``-rs`` option switches the proj coordinate i/o order to latitude/longitude
    and northing/easting, respectively. We do this to comply with the expected order
    for the geod program.

Now, find the UTM coordinates of the two end points::

    echo %HEL% | %utm35%
    echo %TAL% | %utm35%

Note your results and compute the mean of the northings, and the mean of the eastings,
to obtain the planar approximation of the mid point

.. hint::
    you can use python as a makeshift command line calculator by saying::

        python -c print((4+8)/2)

Now, compute the geographical coordinates corresponding to the UTM mid point::

    echo <your northing   your easting> | %utm35% -f "%.12f" -I

(note the -I option for doing the inverse projection)

For convenience define::

    set MID_APPROX=<your result here>

Finally, compute the distance between the geodesic mid point and its planar
approximation, by stating it as another case of the inverse geodetic problem::

    echo %MID%  %MID_APPROX% | geod -I +ellps=GRS80

resulting in::

    107d0'16.673"    -72d59'43.193"    2.527

i.e. a deviation of 2.5 m over a stretch of about 85 km.


4. Suggested meditations
------------------------------------------------------------------------------

Consider these aspects:

#. Given that geod is available, fast, and reliable - is it really worth the
   effort doing approximate calculations in the projected plane?

#. :program:`geod` includes functionality for computing intermediate points along a
   geodesic. Check the manual, especially the description of the
   ``n_S`` option, and try to compute the geodesic mid point directly, by
   setting ``n_S=2``.

#. One should actually expect the result of meditation 2 above to be slightly
   superior to the result obtained in the exercise. Why?


Answers
------------

.. code-block::

        Helsinki,  UTM:  6672241.54   385592.95
        Tallinn,   UTM:  6590881.40   372106.37
        Mid point, UTM:  6631561.47   378849.66
        Mid point, GEO:  59.804039062602   24.840482644156

Meditations:

#. Probably not
#. ``geod +lat_1=60.171 +lon_1=24.938 +lat_2=59.437 +lon_2=24.745 +n_S=2 +ellps=GRS80 -f "%.12f"``
#. Avoids a bit of truncation of the azimuth, and *may* use a slightly superior algorithm (check the code!)

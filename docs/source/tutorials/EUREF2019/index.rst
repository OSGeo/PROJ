.. _transformation_tutorial:

================================================================================
Transformation tutorial
================================================================================


Introduction
------------

This tutorial was developed for the EUREF 2019 Symposium as an introduction
to geodetic transformations in PROJ. The intended audience was geodesist in
governental geodetic agencies but the tutorial should be of interest to a
broader group of people within GIS, remote sensing and related fields.

The tutorials gives insight into how transformations are set up using the
"PROJ language" as well as introduce most of the command line applications
packaged with PROJ. In the tutorial properties of several common projections
and transformations are investigated.

The turotial requires a working installation of PROJ. Consult the
:ref:`chapter on installing PROJ <install>`.

How to fill out the PROJ tutorial exercises
-------------------------------------------

All the exercises are made as :program:`gie` tests where one or more inputs are
missing. Replace occurrences of ``<your answer here>`` with the relevant input.
Below is an annotated example of a complete :program:`gie` test. Your objective is to fill
out the exercises from 1 and onwards in a similar fashion.

Below is a simple exercise before the answer has been filled in::

    1. Set up an operation that returns the input it is given
    -------------------------------------------------------------------------------
    operation   <your answer here>
    tolerance   10 mm

    accept      12.3    45.6
    expect      12.3    45.6
    -------------------------------------------------------------------------------

An annotated answer could look like this::

    <gie>
    operation   +proj=latlon    # The latlon "projection" returns the same output as
                                # given in the input
    tolerance   10 mm

    accept      12.3    45.6
    expect      12.3    45.6
    </gie>

The :program:`gie` application can be used to check if the exercise has been answered
correctly::

    gie exercises\<exercise_name>.gie

If all answers are correct :program:`gie` will return output similar to::

    -------------------------------------------------------------------------------
    Reading file 'exercises\instructions.md'
    -------------------------------------------------------------------------------
    total:  1 tests succeeded,  0 tests skipped,  0 tests failed.
    -------------------------------------------------------------------------------

If incorrect, :program:`gie` will output something like::

    -------------------------------------------------------------------------------
    Reading file 'exercises\instructions.md'
         -----
         FAILURE in instructions.md(48):
         expected: 12.3 45.6
         got:      1369229.736757265171   5685937.873489554040
         deviation:  999999999.999000 mm,  expected:  10.000000 mm
    -------------------------------------------------------------------------------
    total:  0 tests succeeded,  0 tests skipped,  1 tests FAILED!
    -------------------------------------------------------------------------------

Where the number of failing exercises (or tests in :program:`gie` terminology)
is given, as well as the line number of the failing test (in parenthesis after
the file name).

Exercises
----------

.. toctree::
   :maxdepth: 1

   exercises/projections1
   exercises/projections2
   exercises/ellipsoids
   exercises/gridshift
   exercises/helmert
   exercises/conversions
   exercises/pipelines
   exercises/projections3
   answers

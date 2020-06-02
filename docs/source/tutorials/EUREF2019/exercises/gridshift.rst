.. euref2019_gridshift:

Gridshifting
+++++++++++++

Sometimes it is simpler to define a transformation as a gridded model of
displacements rather than a mathematical model. This approach, pioneered
by the US National Geodetic Survey in their 1980s era transformation from
the old NAD27 datum to the (back then) new NAD83, has become increasingly
popular as disk space and CPU cycles have gotten cheaper.

Gridded models of displacements offer the ability to make local adjustments
that are not possible with e.g. a Helmert transformation. This fact is
especially leveraged in transformations of heights since the geoid is too
bumpy to retain enough detail with a simple mathematical definition.

The main problem with grid shifting is availability of the grids needed for a
given transformation. The PROJ package requires that the
:ref:`PROJ-data package <datumgrid>` is installed alongside it.

Download the :program:`gie` file for the exercise: :download:`gridshift.gie <gridshift.gie>`.

Exercises 1. Horizontal gridshifting
-------------------------------------

In this exercise we will apply a horizontal grid shift as it is done in the
transformation from the `German DHDN to ETRS89 transformation <http://crs.bkg.bund.de/crseu/crs/descrtrans/BeTA/de_dhdn2etrs_beta.php>`_.

Define the operation that converts from DE_DHDN to ETRS89 using a gridshift.

.. hint:: See :ref:`hgridshift`

.. hint:: 
    
    Find the relevant grid name by inspecting the output of

        ``projinfo -s DHDN -t ETRS89 -o WKT2_2019``

      Note that two operations are returned, look for the one which uses the
      "NTv2" method.

.. literalinclude:: gridshift.gie
  :lines: 49-53

2. Vertical gridshifting
-------------------------

The most common use case for vertical grid shifts is transformation from
ellipsoidal heights to physical heights. In most cases this is equvialent to
applying an offset from a geoid model, which is exactly what we will do in this
exercise.

Set up an operation that transforms ellipsoidal heights to physical heights
using the EGM96 model.


.. hint:: See :ref:`vgridshift`

.. hint:: The relevant grid name is "us_nga_egm96_15.tif"

.. literalinclude:: gridshift.gie
  :lines: 78-82
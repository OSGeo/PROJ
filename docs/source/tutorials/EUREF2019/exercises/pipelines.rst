.. euref2019_pipelines:

Transformation pipelines
++++++++++++++++++++++++++++

In these exercises we will learn how to use Transformation Pipelines.
Transformation Pipelines is a powerful construct in PROJ that allow users to
combine several operations into one "super operation". Pipelines are used
extensively internally in PROJ when a user asks for a transformation between
two CRS's (for example when using :program:`cs2cs`).  However, users can also write their
own custom pipelines for specific purposes.

Before moving on to the exercises below, familiarize yourself with the
:ref:`pipeline operation in the documentation<pipeline>`. Pay special attention to the rules that
govern the use of the pipeline operation.


Download the :program:`gie` file for the exercise: :download:`pipelines.gie <pipelines.gie>`.

1. Geodetic -> Helmert -> Geodetic
-------------------------------------------------------------------------------

The signature use case for transformation pipelines is the Helmert
transformation of geodetic coordinates.

As we have seen in the previous exercises, fundamentally, the Helmert
operation works on cartesian coordinates. So when the input coordinates
are geodetic, we must convert them to the cartesian space before
applying the Helmert transformation.

Similarly, we must convert the transformed cartesian coordinates back to
geodetic space to be able to compare the coordinates before and after
transformation.

With that in mind, now create a transformation pipeline that take geodetic
(longitude/latitude) coordinates, applies a Helmert transform, and returns
geodetic coordinates (in a different datum).

To keep it simple we will use the GRS80 ellipsoid for both input and output
datum, and a basic 3 parameter transformation, with translations 100,200,300,
and using the position vector convention.

.. hint:: 
    Remember that in a pipeline step, the inverse of an operation is
    selected by adding ``+inv`` to the parameter list of the operation.

.. literalinclude:: pipelines.gie
  :lines: 53-57


2. Sequential grid shifts
-------------------------------------------------------------------------------
Create a transformation that applies both a horizontal and a vertical grid
adjustment. Use the ``de_adv_BETA2007.tif`` and the ``us_nga_egm96_15.tif`` grids.

#. Does it matter which grid is applied first?
#. If yes, which goes first? Why?

.. hint:: See :ref:`hgridshift` and :ref:`vgridshift`

.. literalinclude:: pipelines.gie
  :lines: 78-82

3. ED50/UTM32 -> ETRS89/UTM33
-------------------------------

A transformation from one coordinate reference system to another. A very common
use case of transformation pipelines.

In this exercise we have UTM Zone 32 coordinates related to the ED50 datum,
which we want to transform to UTM Zone 33 related to ETRS89.

The transformation goes

#. Back-projection
#. Geodetic to cartesian conversion
#. Helmert transformation
#. Cartesian to geodetic conversion
#. Re-projection

which is a very common recipe for transformations between projected coordinate systems.

.. hint:: 
    
    Find the relevant Helmert parameters by running::

        projinfo -k operation EPSG:1626 -o WKT2_2019

.. hint:: 
    
    If you can't remember which ellipsoids is used by ED50 and ETRS89
    you can look them up with::

        projinfo <datum>

.. hint:: 

    Relevant documentation:

        #. :ref:`utm`
        #. :ref:`cart`
        #. :ref:`helmert`

.. literalinclude:: pipelines.gie
  :lines: 121-125


4. A pipeline from the Real World
----------------------------------

The transformation between the German DHDN and ETRS89 is a transformation
pipeline consisting of eight steps. Lookup the transformation with projinfo::

    projinfo -k operation EPSG:1776

and copy and paste the pipeline definition as your answer below. Try to decipher
the pipeline and answer the following questions:

#. What is the input coordinate type and units?
#. What is the output coordinate type and units?
#. What is the purpose of the +proj=push and +proj=pop steps?


.. hint:: 
    A pipeline definition can span several lines in the gie format. Use this
    to your advantage and put each step on it's own separate line for easier
    reading

.. hint:: 
    You can use projinfo to get more knownledge about a particular CRS::

        projinfo <CRS name or code>

.. hint::
    Look at :ref:`push` and :ref:`pop` to find out how to set up the transformation


.. literalinclude:: pipelines.gie
  :lines: 162-166
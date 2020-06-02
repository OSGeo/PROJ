.. euref2019_ellipsoids:

Specifying ellipsoid models
+++++++++++++++++++++++++++

In this exercise we will introduce some of the many ways one can specify the
ellipsoid model used in projections and transformations.

We will be using the Mercator projection to illustrate the different ellipsoid
settings, but note that the same parameter settings can be used for setting
the ellipsoid used for most PROJ projections and transformations.

Download the :program:`gie` file for the exercise: :download:`ellipsoids.gie <ellipsoids.gie>`.


Exercise 1: Ellipsoid radius
-----------------------------

Use a sphere of radius 1 as the ellipsoid model

.. hint:: 
    
    Check the :ref:`Mercator documentation <merc>` to find out how to specify an ellipsoid

.. literalinclude:: ellipsoids.gie
  :lines: 24-28

Exercise 2. Specifying a ellipsoid
-------------------------------------------------------------------------------

Set up a Mercator projection that uses the Hayford (International) ellipsoid

.. hint:: 
    Check the :ref:`Mercator documentation <merc>` to find out how to specify an ellipsoid

.. hint:: Running ``proj -le`` will return a list of ellipsoids and their parameters.

.. literalinclude:: ellipsoids.gie
    :lines: 43-47

Exercise 3. Semimajor axis and inverse flattening
---------------------------------------------------------------------------------------------

Specify the Hayford ellipsoid by its semimajor axis radius and inverse flattening

.. hint:: Find the parameters in the list returned by ``proj -le``

.. literalinclude:: ellipsoids.gie
    :lines: 61-65

Exercise 4. Semimajor and semiminor ellipsoid axes
------------------------------------------------------------------------------------

Set the Clarke 1866 ellipsoid by its semimajor and semiminor axis radii

.. literalinclude:: ellipsoids.gie
    :lines: 73-77

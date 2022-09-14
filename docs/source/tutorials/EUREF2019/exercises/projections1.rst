.. euref2019_projections1:

The UTM projection
++++++++++++++++++

In this exercise we will learn how to define a simple projection using PROJ.
We will investigate a few aspects of the UTM projection, as well as learn how
to use a different ellipsoid than the default (GRS80).


Download the :program:`gie` file for the exercise: :download:`projections1.gie <projections1.gie>`.

Exercise 1: Seting up a projection
-----------------------------------

Set up a UTM projection for use in Estonia.

.. hint:: See :ref:`UTM` for more on the projection

.. literalinclude:: projections1.gie
  :lines: 21-25

Exercise 2: UTM on the southern hemisphere
-------------------------------------------

Set up a UTM projection for use on the North Island of New Zealand.

.. hint:: See :ref:`UTM` for more on the projection


.. literalinclude:: projections1.gie
  :lines: 40-44

Exercise 3: Using a non-standard ellipsoid
-------------------------------------------

Set up a UTM projection based on the Hayford ellipsoid for use in Estonia.

.. hint:: See :ref:`UTM` for more on the projection

.. hint:: Run ``proj -le`` to get a list of available ellipsoids

.. literalinclude:: projections1.gie
  :lines: 63-67

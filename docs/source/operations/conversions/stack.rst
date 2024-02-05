.. _stack:

================================================================================
Pipeline stack manipulation
================================================================================

.. versionadded:: 9.4.0

Push/pop/swap operation operating on a stack of coordinate tuples, attached to
a pipeline.

+---------------------+--------------------------------------------------------+
| **Alias**           | stack                                                  |
+---------------------+--------------------------------------------------------+
| **Domain**          | 4D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Any                                                    |
+---------------------+--------------------------------------------------------+
| **Output type**     | Any                                                    |
+---------------------+--------------------------------------------------------+

This operation allows the user to manipulate a stack of coordinate tuples which
is held by a :ref:`pipeline<pipeline>` instance.
Each coordinate tuple in the stack can have up to 4 components. Different tuples
in the stack may have a different number of components.
This operation is a no-operation if used outside of a pipeline.

``stack`` supersedes the :ref:`push<push>` and :ref:`pop<pop>` operations.
Note that the stacks of ``stack`` and ``push/pop`` are totally separated.


Modes
################################################################################

The stack operation has several (mutually exclusive in a single step) modes:

* ``push``: causes one or several components of coordinates to be saved for
  application in a later step. A saved coordinate component is moved, or
  *pushed*, to a memory stack that is part of a :ref:`pipeline<pipeline>`. The
  pipeline coordinate stack is inspired by the stack data structure that is
  commonly used in computer science.

  The value of the ``push`` option is a comma-separated list of components, whose
  value is 1,2,3 or 4. The specified components are saved in a tuple, in the
  order they are specified. So ``+proj=stack +push=3,1`` will push on top of the
  stack a tuple with the 3rd component of the current coordinate value followed
  by the 1st component of the current coordinate value.

  When such a step is applied in the reverse direction, a ``push`` is executed
  as a ``pop``.

* ``pop``: causes one or several components of coordinates, pushed previously,
  to be loaded, or *popped*, from the memory stack, into the current coordinate
  value.

  The value of the ``pop`` option is a comma-separated list of components, whose
  value is 1,2,3 or 4. The first value of the top stack tuple is loaded into
  the first specified index, the second value into the second specified index,
  etc. So ``+proj=pop +push=3,1`` will load into the 3rd component of the current
  coordinate value the first element of the tuple at the top of the stack, and
  will load into the 1st compoment of the current coordinate value the second
  element of the tuple at the top of the stack.

  The number of components specified should exactly match the number of values
  in the top stack tuple.

  The top stack tuple is removed from the stack after the operation has completed.

  When such a step is applied in the reverse direction, a ``pop`` is executed
  as a ``push``.

* ``swap``: causes the top and antepenultimate elements of the stack to be
  inverted. (This implements the semantics of the swap operator of the Forth
  programming language)

  This is an advanced mode that is typically used when doing
  coordinate transformations on compound CRS, where different steps expect/output
  orthometric heights versus ellipsoidal height.

  There must be at least 2 elements in the stack for this operation to be valid.

  The common use case of swap involves a pattern like: push component, modify
  compoment, push modified component, swap, pop component, do something, pop
  component (realistic example given below).

  The swap operator does not require that the number of values in the top and
  antepenultimate elements is the same, although in practical cases, it is
  expected that they should be identical.

  A ``swap`` is executed in the same way in the forward and reverve directions.


Example involving push and pop
################################################################################

A common use of the push and pop sub-operations is in 3D
:ref:`Helmert<helmert>` transformations where only the horizontal components
are needed. This is often the case when combining heights from a legacy
vertical reference with a modern geocentric reference. Below is a an example of
such a transformation, where the horizontal part is transformed with a Helmert
operation but the vertical part is kept exactly as the input was.

::

   $ echo 12 56 12.3 2020 | cct +proj=pipeline  \
   +step +proj=stack +push=3                    \
   +step +proj=cart                             \
   +step +proj=helmert +x=3000 +y=1000 +z=2000  \
   +step +proj=cart +inv                        \
   +step +proj=stack +pop=3

    12.0056753463   55.9866540552       12.3000     2000.0000

Note that the third coordinate component in the output is the same as the input.

The same transformation without the push and pop operations would look like this::

   $ echo 12 56 12.3 2020 | cct +proj=pipeline  \
   +step +proj=cart                             \
   +step +proj=helmert +x=3000 +y=1000 +z=2000  \
   +step +proj=cart +inv

         12.0057        55.9867     3427.7404     2000.0000

Here the vertical component is adjusted significantly.

Example involving swap
################################################################################

The below example demonstrates a pipeline transforming coordinates in ETRS 89
(longitude, latitude, ellipsoidal height) to (longitude, latitude) in the
S-JTSK/05 datum and orthometric height in the Baltic 1957 datum.

::

    echo 15 50 100 | cct -d 10 +proj=pipeline \
                +step +proj=stack +push=3 +omit_inv \           # (1)
                +step +proj=vgridshift +grids=CR2005.tif \      # (2)
                +step +proj=stack +push=3 \                     # (3)
                +step +proj=stack +swap \                       # (4)
                +step +proj=stack +pop=3 \                      # (5)
                +step +proj=cart +ellps=GRS80 \                 # (6)
                +step +inv +proj=helmert +x=572.213 +y=85.334 +z=461.94 \
                    +rx=-4.9732 +ry=-1.529 +rz=-5.2484 +s=3.5378 +convention=coordinate_frame \
                +step +inv +proj=cart +ellps=bessel \
                +step +proj=stack +pop=3                        # (7)

         15.0011680291   50.0007534747  55.0384863419

Let's examine step by step, when executing the pipeline in the forward direction:

1. Save the ETRS89 ellipsoidal height on the stack
2. Apply the geoid model to transform the ETRS89 ellipsoidal height into a Baltic 1957 orthometric height
3. Save the Baltic 1957 on the stack ("above" the ETRS89 ellipsoidal height)
4. Swap the top 2 tuples of the stack, that is now the ETRS89 height will be on top of the Baltic 1957 one.
5. Pop the ETRS89 height from the stack as the active Z value.
6. Apply a 3D Helmert transformation to go from ETRS89 to S-JTSK/05
7. Pop the Baltic 1957 height from the stack as the active Z value.

When run in the inverse direction, the steps are interpreted as:

7. Push the Baltic 1957 height on the stack
6. Apply the inverse 3D Helmert transformation to go from S-JTSK/05 to ETRS89
5. Push the ETRS89 height on the stack
4. Swap the top 2 tuples of the stack, that is now the Baltic 1957 height will be on top of the ETRS89 one.
3. Pop the Baltic 1957 height from the stack as the active Z value.
2. Apply the inverse geoid model to transform the Baltic 1957 orthometric height into a ETRS89 ellipsoidal one.
1. Do not apply this step in the reverse direction ! We got what we want.

Parameters
################################################################################

.. option:: +push=idx1[,idx2,[,idx3[,idx4]]]

   Push up to 4 components from the current coordinate on the stack.
   Each index is between 1 and 4.

.. option:: +pop=idx1[,idx2,[,idx3[,idx4]]]

   Pop the top stack value into the specified components. Each index is between 1 and 4.

.. option:: +swap

   Swap the top and antepenultimate elements of the stack.


Further reading
################################################################################

#. `Stack data structure on Wikipedia <https://en.wikipedia.org/wiki/Stack_(abstract_data_type)>`_

#. `Forth stack operators <https://wiki.laptop.org/go/Forth_stack_operators>`_

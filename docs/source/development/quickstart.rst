.. _dev_quickstart:

================================================================================
Quick start
================================================================================

This is a short introduction to the PROJ API. In the following section we
create a simple program that transforms a geodetic coordinate to UTM and back
again. The program is explained a few lines at a time. The complete program can
be seen at the end of the section.

See the following sections for more in-depth descriptions of different parts of
the PROJ API or consult the :doc:`API reference <reference/index>` for specifics.

Before the PROJ API can be used it is necessary to include the ``proj.h`` header
file. Here ``stdio.h`` is also included so we can print some text to the screen:

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 39-40

Let's declare a few variables that'll be used later in the program. Each variable
will be discussed below.
See the :doc:`reference for more info on data types <reference/datatypes>`.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 43-45
  :dedent: 4

For use in multi-threaded programs the :c:type:`PJ_CONTEXT` threading-context is used.
In this particular example it is not needed, but for the sake of completeness
it created here. The section on :doc:`threads <threads>` discusses
this in detail.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 49
  :dedent: 4

Next we create the :c:type:`PJ` transformation object ``P`` with the function
:c:func:`proj_create`. :c:func:`proj_create` takes the threading context ``C``
created above, and a proj-string that defines the desired transformation.
Here we transform from geodetic coordinate to UTM zone 32N.
It is recommended to create one threading-context per thread used by the program.
This ensures that all :c:type:`PJ` objects created in the same context will be
sharing resources such as error-numbers and loaded grids.
In case the creation of the :c:type:`PJ` object fails an error message is
displayed and the program returns. See :doc:`errorhandling` for further
details.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 51-53
  :dedent: 4

PROJ uses it's own data structures for handling coordinates. Here we use a
:c:type:`PJ_COORD` which is easily assigned with the function :c:func:`proj_coord`.
Note that the input values are converted to radians with :c:func:`proj_torad`.
This is necessary since PROJ is using radians internally. See :doc:`transformations`
for further details.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 57
  :dedent: 4

The coordinate defined above is transformed with :c:func:`proj_trans`. For this
a :c:type:`PJ` object, a transformation direction (either forward or inverse)
and the coordinate is needed. The transformed coordinate is returned in ``b``.
Here the forward (:c:type:`PJ_FWD`) transformation from geodetic to UTM is made.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 60-61
  :dedent: 4

The inverse transformation (UTM to geodetic) is done similar to above,
this time using :c:type:`PJ_INV` as the direction.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 62-63
  :dedent: 4

Before ending the program the allocated memory needs to be released again:

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 66-67
  :dedent: 4


A complete compilable version of the above can be seen here:

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :linenos:
  :lines: 39-


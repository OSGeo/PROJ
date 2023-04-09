.. _dev_quickstart_cpp:

================================================================================
Quick start for C++ API usage
================================================================================

This is a short introduction to the PROJ :ref:`C++ API <cpp>`. In the following section
we create a simple program that illustrate how to transform points
between two different coordinate systems.

Note: the essential osgeo::proj::operation::CoordinateOperation::coordinateTransformer()
method was only added in PROJ 9.3. For earlier versions, coordinate operations
must be exported as a PROJ string with the osgeo::proj::operation::CoordinateOperation::exportToPROJ() method, and
passed to :c:func:`proj_create` to instance a PJ* object to use with :c:func:`proj_trans`
(cf the :ref:`dev_quickstart`). The use of the C API :c:func:`proj_create_crs_to_crs`
might still be easier to let PROJ automatically select the "best" coordinate
operation when several ones are possible, as the corresponding automation is not
currently available in the C++ API.

Before the PROJ API can be used it is necessary to include the various header
files:

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 9-18

For convenience, we also declare using a few namespaces:

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 20-23

We start by creating a database context (:cpp:class:`osgeo::proj::io::DatabaseContext`)
with the default settings to find the PROJ database.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 26
  :dedent: 4

We then instantiate a generic authority factory (:cpp:class:`osgeo::proj::io::AuthorityFactory`),
that is not tied to a particular authority, to be able to get transformations
registered by different authorities. This can only be used for a
:cpp:class:`osgeo::proj::operation::CoordinateOperationContext`, and not to
instantiate objects of the database which are all tied to a non-generic authority.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 31

We create a coordinate operation context, that can be customized to ammend
the way coordinate operations are computed. Here we ask for default settings,
as we have a coordinate operation that just involves a "simple" map projection
in the same datum.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 36-37
  :dedent: 4

We instantiate a authority factory for EPSG related objects.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 40
  :dedent: 4

We instantiate the source CRS from its code: 4326, for WGS 84 latitude/longitude.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 43
  :dedent: 4

We instantiate the source CRS from its PROJ.4 string (it would be possible
to instantiate it from its 32631 code, similarly to above), and cast the
generic :cpp:class:`osgeo::proj::util::BaseObject` to the
:cpp:class:`osgeo::proj::crs::CRS` class required later.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 49-51
  :dedent: 4

.. warning::

    The use of PROJ strings to describe a CRS is not recommended.  One of the
    main weaknesses of PROJ strings is their inability to describe a geodetic
    datum, other than the few ones hardcoded in the ``+datum`` parameter.

We ask for the list of operations available to transform from the source
to the target CRS with the :cpp:func:`osgeo::proj::operation::CoordinateOperationFactory::createOperations`
method.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 55-56
  :dedent: 4

We check that we got a non-empty list of operations. The list is sorted from
the most relevant to the less relevant one. Cf :ref:`operations_computation_filtering`
for more details on the sorting of those operations. For a transformation
between a projected CRS and its base CRS, like we do here, there will be only
one operation.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 65
  :dedent: 4

We create an execution context (must only be used by one thread at a time)
with the :c:func:`proj_context_create` function.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 68
  :dedent: 4

We create a coordinate transformer (:cpp:class:`osgeo::proj::operation::CoordinateTransformer`)
from the first operation of the list:

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 71
  :dedent: 4

We can now transform a point with the :cpp:func:`osgeo::proj::operation::CoordinateTransformer::transform`
method. Note that the the expected input values should be passed in the order
and the unit of the successive axis of the input CRS. Similarly the values
returned in the v[] array of the output PJ_COORD are in the order and the unit
of the successive axis of the output CRS. For coordinate operations involving a
time-dependent operation, coord.v[3] is the decimal year of the coordinate epoch
of the input (or HUGE_VAL to indicate none).

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 74-80
  :dedent: 4

and output the result:

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 83-85
  :dedent: 4

We need to clean up the PJ_CONTEXT handle before exiting with the
:c:func:`proj_context_destroy` function.

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :lines: 88
  :dedent: 4

A complete compilable version of the example code can be seen below:

.. literalinclude:: ../../../examples/EPSG_4326_to_32631.cpp
  :language: c++
  :linenos:
  :lines: 9-

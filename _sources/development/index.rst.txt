.. _development:

================================================================================
Development
================================================================================

These pages are primarily focused towards developers either contributing to the
PROJ project or using the library in their own software.


.. toctree::
   :maxdepth: 1

   quickstart
   transformations
   errorhandling
   threads
   reference/index
   cmake
   bindings
   migration


.. attention::

    The ``projects.h`` header and the functions related to it is considered
    deprecated from version 5.0.0 and onwards. The header  will be removed
    from PROJ in version 6.0.0 scheduled for release February 1st 2019.

.. attention::

    The nmake build system on Windows will not be supported from
    version 6.0.0 on onwards. Use CMake instead.

.. attention::

    The ``proj_api.h`` header and the functions related to it is
    considered deprecated from version 5.0.0 and onwards. The header will be
    removed from PROJ in version 7.0.0 scheduled for release February 1st
    2020.

.. attention::

    With the introduction of PROJ 5, behavioural changes has been made to
    existing functionality. Consult :ref:`differences` for the details.

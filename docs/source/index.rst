.. _home:

PROJ
###############################################################################

.. toctree::
   :maxdepth: 3
   :hidden:

   about
   news
   download
   install
   usage/index
   apps/index
   operations/index
   resource_files
   geodesic
   development/index
   community/index
   faq
   glossary
   references

.. only:: html

    PROJ is a standard UNIX filter function which converts geographic longitude
    and latitude coordinates into cartesian coordinates (and vice versa), and it is
    a C API for software developers to include coordinate transformation in their
    own software. PROJ is maintained on `GitHub <https://github.com/OSGeo/proj.4/>`_.

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

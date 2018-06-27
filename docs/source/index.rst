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

    PROJ is a generic coordinate transformation software, that transforms geospatial
    coordinates from one coordinate reference system (CRS) to another. This
    includes cartographic projections as well as geodetic transformations.

    PROJ includes :ref:`command line applications<apps>` for easy conversion of
    coordinates from text files or directly from user input. In addition to the
    command line utilities PROJ also exposes an
    :ref:`application programming interface<reference>`, or API in short. The API
    let developers use the functionality of PROJ in their own software without having
    to implement similar functionality themselves.

    PROJ started purely as a cartography application letting users convert geodetic
    coordinates into projected coordinates using a number of different cartographic
    projections. Over the years, as the need has become apparent, support for datum
    shifts has slowly worked it's way into PROJ as well. Today PROJ support more
    than a hundred different map projections and can transform coordinates between
    datums using all but the most obscure geodetic techniques.

    You can download the source code for PROJ on the :ref:`download section<download>`
    and find links to prepackaged executables in the
    :ref:`installation section<install>`.

    In addition to this website the PROJ documentation is also available in `PDF`_
    form.

.. _`PDF`: https://raw.githubusercontent.com/OSGeo/proj.4/gh-pages/proj.pdf

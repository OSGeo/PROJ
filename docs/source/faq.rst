.. _faq:

******************************************************************************
FAQ
******************************************************************************

.. only:: not latex

    .. contents::
       :depth: 3
       :backlinks: none

Which file formats does PROJ support?
--------------------------------------------------------------------------------

The :ref:`command line applications <apps>` that come with PROJ only support text
input and output (apart from :program:`proj` which accepts a simple binary data
stream as well). :program:`proj`, :program:`cs2cs` and :program:`cct` expects
text files with one coordinate per line with each coordinate dimension in a
separate column.

.. note::

    If your data is stored in a common geodata file format chances are that
    you can use `GDAL <https://gdal.org/>`_ as a frontend to PROJ and transform your data with the
    :program:`ogr2ogr` application.

Can I transform from *abc* to *xyz*?
--------------------------------------------------------------------------------

Probably. PROJ supports transformations between most coordinate reference systems
registered in the EPSG registry, as well as a number of other coordinate reference
systems. The best way to find out is to test it with the :program:`projinfo`
application. Here's an example checking if there's a transformation between
ETRS89/UTM32N (EPSG:25832) and ETRS89/DKTM1 (EPSG:4093):

.. code-block:: console

    $ ./projinfo -s EPSG:25832 -t EPSG:4093 -o PROJ
    Candidate operations found: 1
    -------------------------------------
    Operation n°1:

    unknown id, Inverse of UTM zone 32N + DKTM1, 0 m, World

    PROJ string:
    +proj=pipeline +step +inv +proj=utm +zone=32 +ellps=GRS80
    +step +proj=tmerc +lat_0=0 +lon_0=9 +k=0.99998 +x_0=200000 +y_0=-5000000 +ellps=GRS80

See the :program:`projinfo` :ref:`documentation <projinfo>` for more info on
how to use it.

Coordinate reference system *xyz* is not in the EPSG registry, what do I do?
--------------------------------------------------------------------------------

Generally PROJ will accept coordinate reference system descriptions in the form
of WKT, WKT2 and PROJ strings. If you are able to describe your desired CRS
in either of those formats there's a good chance that PROJ will be able to make
sense of it.

If it is important to you that a given CRS is added to the EPSG registry, you
should contact your local geodetic authority and ask them to submit the CRS for
inclusion in the registry.

I found a bug in PROJ, how do I get it fixed?
--------------------------------------------------------------------------------

Please report bugs that you find to the issue tracker on GitHub. :ref:`Here's how
<add_bug_report>`.

If you know how to program you can also try to fix it yourself. You are welcome
to ask for guidance on one of the :ref:`communication channels <channels>` used
by the project.

How do I contribute to PROJ?
--------------------------------------------------------------------------------

Any contributions from the PROJ community is welcome. See :ref:`contributing` for
more details.

How do I calculate distances/directions on the surface of the earth?
--------------------------------------------------------------------------------

These are called geodesic calculations. There is a page about it here:
:ref:`geodesic`.

What is the best format for describing coordinate reference systems?
--------------------------------------------------------------------------------

A coordinate reference system (CRS) can in PROJ be described in several ways:
As PROJ strings, Well-Known Text (WKT) and as spatial reference ID's (such as EPSG
codes). Generally, WKT or SRID's are preferred over PROJ strings as they can
contain more information about a given CRS. Conversions between WKT and PROJ
strings will in most cases cause a loss of information, potentially leading to
erroneous transformations.

For compatibility reasons PROJ supports several WKT dialects
(see :option:`projinfo -o`). If possible WKT2 should be used.

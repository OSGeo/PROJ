.. _apps:

================================================================================
Applications
================================================================================

Bundled with PROJ comes a set of small command line utilities. The ``proj``
program is limited to converting between geographic and projection coordinates
within one datum. The ``cs2cs`` program operates similarly, but allows
translation between any pair of definable coordinate systems, including support
for datum transformation. The ``geod`` program provides the ability to do
geodesic (great circle) computations. ``gie`` is the program used for
regression tests in PROJ. ``cct`` a 4D equivalent to the ``proj`` program,
performs transformation coordinate systems on a set of input points.
``projinfo`` performs queries for geodetic objects and coordinate operations.

.. toctree::
   :maxdepth: 1

   cct
   cs2cs
   geod
   gie
   proj
   projinfo

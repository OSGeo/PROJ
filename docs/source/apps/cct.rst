.. _cct:

================================================================================
cct
================================================================================


.. Index:: cct

``cct`` performs transformation between the source and destination cartographic  coordinate  system
on a set of input points.  The coordinate system transformation can include  translation  between
projected  and geographic coordinates as well as the application of datum shifts.

``cct`` is a first attempt at a 4D equivalent to the ``proj`` projection
program.

cct is an acromyn meaning ``Coordinate Conversion and Transformation``.

The acronym refers to definitions given in the OGC 08-015r2/ISO-19111
standard "Geographical Information -- Spatial Referencing by Coordinates",
which defines two different classes of coordinate operations:

*Coordinate Conversions*, which are coordinate operations where input
and output datum are identical (e.g. conversion from geographical to
cartesian coordinates) and

*Coordinate Transformations*, which are coordinate operations where
input and output datums differ (e.g. change of reference frame).




Synopsis
********

::

       cct [ -cotvz [ args ] ]... +opts[=arg]...  file...

Description
***********
The following control parameters can  appear  in any order:

::

       -c x,y,z,t
       --columns=x,y,z,t
              Specify from which columns in the input file first, second, third, and fourth
              coordinate should be taken.

       -o <output file name>
       --output=<output file name>
              Specify the name of the output file.

       -t <time>
       --time=<time>
              Specify a fixed observation time to be used for all input data.

      -z <height>
      --height=<height>
              Specify a fixed observation height to be used for all input data.

       -v
       --verbose
              Write non-essential, but potentially useful, information to stderr.
              Repeat for additional information (-vv)

::


The ``+args`` arguments are associated with cartographic parameters. Usage varies with
projection and for a complete description consult the `projection pages <../projections/index.html>`_


Example
*******
The following script

::

     echo 12 55 0 0 | cct +proj=utm +zone=32 +ellps=GRS80

will  transform the input geographic coordinates into UTM zone 32 coordinates.



.. _cct:

================================================================================
cct
================================================================================


.. Index:: cct




``cct`` a 4D equivalent to the ``proj`` projection
program, performs transformation coordinate systems on a set of input points.
The coordinate system transformation can include  translation  between
projected  and geographic coordinates as well as the application of datum shifts.

cct is an acronym meaning *Coordinate Conversion and Transformation*.

The acronym refers to definitions given in the OGC 08-015r2/ISO-19111
standard "Geographical Information -- Spatial Referencing by Coordinates",
which defines two different classes of *coordinate operations*:

*Coordinate Conversions*, which are coordinate operations where input
and output datum are identical (e.g. conversion from geographical to
cartesian coordinates) and

*Coordinate Transformations*, which are coordinate operations where
input and output datums differ (e.g. change of reference frame).

``cct``, however, also refers to Carl Christian Tscherning (1942--2014),
professor of Geodesy at the University of Copenhagen, mentor and advisor
for a generation of Danish geodesists, colleague and collaborator for
two generations of global geodesists, Secretary General for the
International Association of Geodesy, IAG (1995--2007), fellow of the
American Geophysical Union (1991), recipient of the IAG Levallois Medal
(2007), the European Geosciences Union Vening Meinesz Medal (2008), and
of numerous other honours.

cct, or Christian, as he was known to most of us, was recognized for his
good mood, his sharp wit, his tireless work, and his great commitment to
the development of geodesy -- both through his scientific contributions,
comprising more than 250 publications, and by his mentoring and teaching
of the next generations of geodesists.

As Christian was an avid Fortran programmer, and a keen Unix connoisseur,
he would have enjoyed to know that his initials would be used to name a
modest Unix style transformation filter, hinting at the tireless aspect
of his personality, which was certainly one of the reasons he accomplished
so much, and meant so much to so many people.

Hence, in honour of cct (the geodesist) this is cct (the program).



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
              Specify input columns for (up to) 4 input parameters. Defaults to 1,2,3,4

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
              Repeat for additional information (-vv, -vvv, etc.)

The ``+args`` arguments are associated with coordinate operation parameters. Usage varies with
operation. For a complete description consult the `projection pages <../projections/index.html>`_


Examples
********

1. The operator specs describe the action to be performed by cct. So the following script

::

     echo 12 55 0 0 | cct +proj=utm +zone=32 +ellps=GRS80

will transform the input geographic coordinates into UTM zone 32 coordinates.
Hence, the command

::

      echo 12 55 | cct -z0 -t0 +proj=utm +zone=32 +ellps=GRS80

Should give results comparable to the classic proj command

::

      echo 12 55 | proj +proj=utm +zone=32 +ellps=GRS80

2. Convert geographical input to UTM zone 32 on the GRS80 ellipsoid:

::

      cct +proj=utm +ellps=GRS80 +zone=32

3. Roundtrip accuracy check for the case above:

::

      cct +proj=pipeline +proj=utm +ellps=GRS80 +zone=32 +step +step +inv

4. As (2) but specify input columns for longitude, latitude, height and time:

::

      cct -c 5,2,1,4  +proj=utm +ellps=GRS80 +zone=32

5. As (2) but specify fixed height and time, hence needing only 2 cols in input:

::

      cct -t 0 -z 0  +proj=utm  +ellps=GRS80  +zone=32

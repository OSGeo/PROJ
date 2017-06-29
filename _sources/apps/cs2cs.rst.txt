.. _cs2cs:

================================================================================
cs2cs
================================================================================


.. Index:: cs2cs

``cs2cs`` performs transformation between the source and destination cartographic  coordinate  system
on a set of input points.  The coordinate system transformation can include  translation  between
projected  and geographic coordinates as well as the application of datum shifts.


Synopsis
********

::

       cs2cs [ -eEfIlrstvwW [ args ] ] [ +opts[=arg] ] [ +to [+opts[=arg]] ] file[s]

Description
***********
The following control parameters can  appear  in any order:

::

       -I     method to specify inverse translation, convert from +to coordinate system to the
              primary coordinate system defined.

       -ta    A specifies a character employed as the first character to denote a control  line
              to  be passed through without processing. This option  applicable  to  ascii  input
              only. (# is the default value).

       -e string
              String is an arbitrary string to be output if an error is detected  during  data
              transformations. The  default value is: *\t*. Note that if the -b, -i or -o
              options are employed, an error is returned as HUGE_VAL value for both
              return values.

       -E     causes the input coordinates to be copied to the output line prior to printing  the
              converted values.

       -l[p|P|=|e|u|d]id
              List projection identifiers with -l, -lp or -lP (expanded) that can be selected
              with +proj. -l=id gives expanded description of projection id. List
              ellipsoid identifiers with -le, that can be selected with +ellps,-lu list  of
              cartesian to meter conversion factors that can be selected with +units or -ld
              list of datums that can be selected with +datum.

       -r     This options reverses the order of the expected input from longitude-latitude or
              x-y to latitude-longitude or y-x.

       -s     This options reverses the order of the output from x-y or longitude-latitude to
              y-x or latitude-longitude.

       -f format
              Format is a printf format string to control the form of the output values. For
              inverse projections, the output will be in degrees when this option is employed.
              If a format is specified for inverse projection the output data will be in deci-
              mal  degrees. The default format is "%.2f" for forward projection and DMS for
              inverse.

       -[w|W]n
              N is the number of significant fractional digits to employ for seconds output (when
              the option is not specified, -w3 is assumed). When -W is employed the fields
              will be constant width and with leading zeroes.

       -v     causes a listing of cartographic control parameters tested for and used by the
              program to be printed prior to input data.

The ``+args`` run-line arguments are associated with cartographic parameters. Usage varies with
projection and for a complete description consult the `projection pages <../projections/index.html>`_

The cs2cs program requires two coordinate system
definitions.  The first (or primary  is  defined
based on all projection parameters not appearing
after the +to argument.  All projection  parameters
appearing  after the ``+to`` argument are considered
the definition of the second  coordinate
system.  If there is no second coordinate system
defined, a geographic coordinate system based on
the datum and ellipsoid of the source coordinate
system is assumed.  Note  that  the  source  and
destination  coordinate  system can both be projections,
both be geographic, or one of each and
may have the same or different datums.

Additional  projection control parameters may be
contained in two auxiliary  control  files:  the
first   is   optionally   referenced   with  the
``+init=file:id`` and the second is always processed
after the name of the projection has been established
from either the run-line or the  contents
of  ``+init``   file.   The  environment  parameter
PROJ_LIB establishes the default directory for a
file  reference  without an absolute path.  This
is also used for  supporting  files  like  datum
shift files.

One  or  more  files (processed in left to right
order) specify the source of data to  be  transformed.
A  - will specify the location of processing standard input.
If no files are  specified,  the  input  is  assumed to be from stdin.
For input data the two data values  must  be  in
the  first  two white space separated fields and
when both input and output are ASCII all  trailing
portions  of the input line are appended to
the output line.

Input geographic data (longitude  and  latitude)
must  be  in  DMS  or decimal degrees format and
input cartesian data must be in units consistent
with  the  ellipsoid major axis or sphere radius
units.  Output geographic coordinates will  normally
be in DMS format (use ``-f %.12f`` for decimal
degrees with 12 decimal places), while projected
(cartesian)   coordinates   will  be  in  linear
(meter, feet) units.


Example
*******
The following script

::

     cs2cs +proj=latlong +datum=NAD83
           +to +proj=utm +zone=10  +datum=NAD27
     -r <<EOF
     45d15'33.1"   111.5W
     45d15.551666667N   -111d30
     +45.25919444444    111d30'000w
     EOF

will  transform the input NAD83 geographic coordinates into NAD27
coordinates in the  UTM  projection  with  zone 10 selected.  The geographic
values of this example are equivalent and  meant as  examples of various forms of DMS input.
The x-y output data will appear as three lines of:

::

     1402285.99      5076292.42 0.000



:. _proj:

================================================================================
proj
================================================================================


.. Index:: proj

Proj and invproj perform respective forward  and inverse  transformation  of cartographic data to
or from cartesian data  with  a  wide  range  of selectable projection functions.


Synopsis
********
::

    proj [ -bcCeEfiIlmorsStTvVwW [ args ] ] [ +args ] file[s]
    invproj [ -bcCeEfiIlmorsStTwW [ args ] ] [ +args ] file[s]


Description
***********
The  following  control parameters can appear in any order

::

       -b     Special option for binary coordinate data input and output through standard input
              and standard output. Data is assumed to be in system type double floating point
              words. This option is to be used when proj is a son process and allows bypassing
              formatting operations.

       -i     Selects binary input only (see -b option).

       -C     Check. Invoke all built in self tests and report. Get more verbose report by
              preceding with the -V option).

       -I     alternate method to specify inverse projection. Redundant when used with invproj.

       -o     Selects binary output only (see -b option).

       -ta    A specifies a character employed as the first character to denote a control line
              to be passed through without  processing. This  option applicable to ascii input
              only. (# is the default value).

       -e string
              String is an arbitrary string to be output if an error is detected during data
              transformations. The default value is: *\t*. Note that if the -b, -i or -o
              options are employed, an error is returned as HUGE_VAL value for both return values.

       -E     causes the input coordinates to be copied to the output line prior to printing the
              converted values.


       -l[p|P|=|e|u|d]id
              List projection identifiers with -l, -lp or -lP (expanded) that can be selected
              with +proj. -l=id gives expanded description of projection id. List
              ellipsoid identifiers with -le, that can be selected with +ellps, -lu list of
              cartesian to meter conversion factors that can be selected with +units or -ld
              list of datums that can be selected with +datum.

       -r     This options reverses the order of the expected input from longitude-latitude or
              x-y to latitude-longitude or y-x.

       -s     This options reverses the order of the output from x-y or longitude-latitude to
              y-x or latitude-longitude.

       -S     Causes estimation of meridional and parallel scale factors, area scale factor and
              angular distortion, and maximum and minimum scale factors to be listed between <>
              for each input point. For conformal projections meridional and parallel scales
              factors will be equal and angular distortion zero. Equal area projections will
              have an area factor of 1.

       -m mult
              The cartesian data may be scaled by the mult parameter. When processing data in
              a forward projection mode the cartesian output values are multiplied by mult
              otherwise the input cartesian values are divided by mult before inverse projection.
              If the first two characters of mult are 1/ or 1: then the reciprocal value of mult
              is employed.

       -f format
              Format is a printf format string to control the form of the output values. For
              inverse projections, the output will be in degrees when this option is employed.
              The  default format is "%.2f" for forward projection and DMS for inverse.

       -[w|W]n
              N is the number of significant fractional digits to employ for seconds output (when
              the  option  is  not  specified, -w3 is assumed). When -W is employed the fields
              will be constant width and  with  leading zeroes.

       -v     causes a listing of cartographic control parameters tested for and used by the
              program to be printed prior to input data. Should not be used with the -T
              option.

       -V     This option causes an expanded annotated listing of the characteristics of the
              projected point. -v is implied with this option.

       -T ulow,uhi,vlow,vhi,res[,umax,vmax]
              This option creates a set of bivariate Chebyshev polynomial coefficients that
              approximate the selected  cartographic projection on stdout. The values low and
              hi denote the range of the input where the u or v prefixes apply to respective
              longitude-x or latitude-y depending upon whether a forward or inverse projection
              is selected. Res is an integer number specifying the power of 10 precision of
              the approximation. For example, a res of -3 specifies an approximation with an
              accuracy better than .001. Umax, and vmax specify maximum degree of the polynomials
              (default: 15).


The ``+args`` run-line arguments are associated with cartographic parameters. Usage varies with
projection and for a complete description consult the `projection pages <../projections/index.html>`_

Additional projection control parameters may  be
contained  in  two  auxiliary control files: the
first  is   optionally   referenced   with   the
``+init=file:id`` and the second is always processed
after the name of the projection has been established
from either the run-line or the contents
of  +init  file.   The   environment   parameter
``PROJ_LIB`` establishes the default directory for a
file reference without an absolute  path.   This
is  also  used  for  supporting files like datum
shift files.

One or more files (processed in  left  to  right
order)  specify  the source of data to be transformed.
A - will specify the location  of  processing
standard input.  If no files are specified,
the input is assumed  to  be  from  stdin.
For ASCII input data the two data values must be
in the first two white  space  separated  fields
and  when  both  input  and output are ASCII all
trailing portions of the input line are appended
to the output line.

Input  geographic  data (longitude and latitude)
must be in DMS format and input  cartesian  data
must  be  in units consistent with the ellipsoid
major axis or sphere radius units.  Output
geographic  coordinates  will  be in DMS (if the -w
switch is not employed) and  precise  to  0.001"
with  trailing, zero-valued minute-second fields
deleted.

Example
*******
The following script

::

     proj +proj=utm  +lon_0=112w  +ellps=clrk66
     -r <<EOF
     45d15'33.1"   111.5W
     45d15.551666667N   -111d30
     +45.25919444444    111d30'000w
     EOF

will perform UTM forward projection with a standard
UTM  central  meridian  nearest  longitude
112W.  The geographic values of this example are
equivalent and  meant  as  examples  of  various
forms  of  DMS  input.  The x-y output data will
appear as three lines of::

     460769.27     5011648.45

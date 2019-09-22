.. _cct:

================================================================================
cct
================================================================================

.. Index:: cct

.. only:: html

    Coordinate Conversion and Transformation.

Synopsis
********

    **cct** [**-cIostvz** [args]] *+opt[=arg]* ... file ...

Description
***********

:program:`cct` a 4D equivalent to the :program:`proj` projection program,
performs transformation coordinate systems on a set of input points. The
coordinate system transformation can include translation between projected
and geographic coordinates as well as the application of datum shifts.


The following control parameters can appear in any order:

.. program:: cct

.. option:: -c <x,y,z,t>

    Specify input columns for (up to) 4 input parameters. Defaults to 1,2,3,4.

.. option:: -d <n>

.. versionadded:: 5.2.0

    Specify the number of decimals in the output.

.. option:: -I

    Do the inverse transformation.

.. option:: -o <output file name>, --output=<output file name>

    Specify the name of the output file.

.. option:: -t <time>, --time=<time>

    Specify a fixed observation *time* to be used for all input data.

.. option:: -z <height>, --height=<height>

    Specify a fixed observation *height* to be used for all input data.

.. option:: -s <n>, --skip-lines=<n>

    .. versionadded:: 5.1.0

    Skip the first *n* lines of input. This applies to any kind of input, whether
    it comes from ``STDIN``, a file or interactive user input.

.. option:: -v, --verbose

    Write non-essential, but potentially useful, information to stderr.
    Repeat for additional information (``-vv``, ``-vvv``, etc.)

.. option:: --version

    Print version number.

The *+opt* arguments are associated with coordinate operation parameters.
Usage varies with operation.

.. only:: html

    For a complete description consult the :ref:`projection pages <projections>`.


:program:`cct` is an acronym meaning *Coordinate Conversion and Transformation*.

The acronym refers to definitions given in the OGC 08-015r2/ISO-19111
standard "Geographical Information -- Spatial Referencing by Coordinates",
which defines two different classes of *coordinate operations*:

*Coordinate Conversions*, which are coordinate operations where input
and output datum are identical (e.g. conversion from geographical to
cartesian coordinates) and

*Coordinate Transformations*, which are coordinate operations where
input and output datums differ (e.g. change of reference frame).

Examples
********

1. The operator specs describe the action to be performed by :program:`cct`. So
   the following script

.. code-block:: console

      echo 12 55 0 0 | cct +proj=utm +zone=32 +ellps=GRS80

will transform the input geographic coordinates into UTM zone 32 coordinates.
Hence, the command

.. code-block:: console

      echo 12 55 | cct -z0 -t0 +proj=utm +zone=32 +ellps=GRS80

Should give results comparable to the classic :program:`proj` command

.. code-block:: console

      echo 12 55 | proj +proj=utm +zone=32 +ellps=GRS80

2. Convert geographical input to UTM zone 32 on the GRS80 ellipsoid:

.. code-block:: console

      cct +proj=utm +ellps=GRS80 +zone=32

3. Roundtrip accuracy check for the case above:

.. code-block:: console

      cct +proj=pipeline +proj=utm +ellps=GRS80 +zone=32 +step +step +inv

4. As (2) but specify input columns for longitude, latitude, height and time:

.. code-block:: console

      cct -c 5,2,1,4 +proj=utm +ellps=GRS80 +zone=32

5. As (2) but specify fixed height and time, hence needing only 2 cols in
   input:

.. code-block:: console

      cct -t 0 -z 0 +proj=utm +ellps=GRS80 +zone=32

6. Auxiliary data following the coordinate input is forward to the output
   stream:

.. code-block:: console

    $ echo 12 56 100 2018.0 auxiliary data | cct +proj=merc
    1335833.8895   7522963.2411      100.0000     2018.0000 auxiliary data

Background
**********

:program:`cct` also refers to Carl Christian Tscherning (1942--2014),
professor of Geodesy at the University of Copenhagen, mentor and advisor
for a generation of Danish geodesists, colleague and collaborator for
two generations of global geodesists, Secretary General for the
International Association of Geodesy, IAG (1995--2007), fellow of the
American Geophysical Union (1991), recipient of the IAG Levallois Medal
(2007), the European Geosciences Union Vening Meinesz Medal (2008), and
of numerous other honours.

*cct*, or Christian, as he was known to most of us, was recognized for his
good mood, his sharp wit, his tireless work, and his great commitment to
the development of geodesy -- both through his scientific contributions,
comprising more than 250 publications, and by his mentoring and teaching
of the next generations of geodesists.

As Christian was an avid Fortran programmer, and a keen Unix connoisseur,
he would have enjoyed to know that his initials would be used to name a
modest Unix style transformation filter, hinting at the tireless aspect
of his personality, which was certainly one of the reasons he accomplished
so much, and meant so much to so many people.

Hence, in honour of *cct* (the geodesist) this is :program:`cct` (the program).


.. only:: man

    See also
    ********

    **proj(1)**, **cs2cs(1)**, **geod(1)**, **gie(1)**, **projinfo(1)**

    Bugs
    ****

    A list of know bugs can be found at https://github.com/OSGeo/PROJ/issues
    where new bug reports can be submitted to.

    Home page
    *********

    https://proj.org/

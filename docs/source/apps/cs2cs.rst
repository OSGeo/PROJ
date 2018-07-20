.. _cs2cs:

================================================================================
cs2cs
================================================================================

.. only:: html

    Cartographic coordinate system filter.

Synopsis
********

    **cs2cs** [ **-eEfIlrstvwW** [ args ] ] [ *+opts[=arg]* ] [ +to [*+opts[=arg]*] ] file[s]

Description
***********

:program:`cs2cs` performs transformation between the source and destination
cartographic coordinate system on a set of input points. The coordinate
system transformation can include translation between projected and
geographic coordinates as well as the application of datum shifts.

The following control parameters can appear in any order:

.. program:: cs2cs

.. option:: -I

    Method to specify inverse translation, convert from *+to* coordinate system to
    the primary coordinate system defined.

.. option:: -t<a>

    Where *a* specifies a character employed as the first character to denote a control
    line to be passed through without processing. This option applicable to
    ASCII input only. (# is the default value).

.. option:: -e <string>

    Where *string* is an arbitrary string to be output if an error is detected during
    data transformations. The default value is a three character string: ``*\t*``.

.. option:: -E

    Causes the input coordinates to be copied to the output line prior to
    printing the converted values.

.. option:: -l<[=id]>

    List projection identifiers that can be selected with *+proj*. ``cs2cs -l=id``
    gives expanded description of projection *id*, e.g. ``cs2cs -l=merc``.

.. option:: -lp

    List of all projection id that can be used with the *+proj* parameter.
    Equivalent to ``cs2cs -l``.

.. option:: -lP

    Expanded description of all projections that can be used with the *+proj*
    parameter.

.. option:: -le

    List of all ellipsoids that can be selected with the *+ellps* parameters.

.. option:: -lu

    List of all distance units that can be selected with the *+units* parameter.

.. option:: -ld

    List of datums that can be selected with the *+datum* parameter.

.. option:: -r

    This options reverses the order of the expected input from
    longitude-latitude or x-y to latitude-longitude or y-x.

.. option:: -s

    This options reverses the order of the output from x-y or longitude-latitude
    to y-x or latitude-longitude.

.. option:: -f <format>

    Where *format* is a printf format string to control the form of the output values.
    For inverse projections, the output will be in degrees when this option is
    employed. If a format is specified for inverse projection the output data
    will be in decimal degrees. The default format is ``"%.2f"`` for forward
    projection and DMS for inverse.

.. option:: -w<n>

    Where *n* is the number of significant fractional digits to employ for seconds
    output (when the option is not specified, ``-w3`` is assumed).

.. option:: -W<n>

    Where *n* is the number of significant fractional digits to employ for seconds
    output. When ``-W`` is employed the fields will be constant width
    with leading zeroes.

.. option:: -v

    Causes a listing of cartographic control parameters tested for and used by
    the program to be printed prior to input data.


.. only:: man

    The *+args* run-line arguments are associated with cartographic
    parameters.

.. only:: html

    The *+args* run-line arguments are associated with cartographic
    parameters. Usage varies with projection and for a complete description
    consult the :ref:`projection pages <projections>`.

The :program:`cs2cs` program requires two coordinate system definitions. The first (or
primary is defined based on all projection parameters not appearing after the
*+to* argument. All projection parameters appearing after the *+to* argument
are considered the definition of the second coordinate system. If there is no
second coordinate system defined, a geographic coordinate system based on the
datum and ellipsoid of the source coordinate system is assumed. Note that the
source and destination coordinate system can both be projections, both be
geographic, or one of each and may have the same or different datums.

Additional projection control parameters may be contained in two auxiliary
control files: the first is optionally referenced with the
*+init=file:id* and the second is always processed after the name of the
projection has been established from either the run-line or the contents of
*+init* file. The environment parameter :envvar:`PROJ_LIB` establishes the default
directory for a file reference without an absolute path. This is also used
for supporting files like datum shift files.

One or more files (processed in left to right order) specify the source of
data to be transformed. A ``-`` will specify the location of processing standard
input. If no files are specified, the input is assumed to be from stdin.
For input data the two data values must be in the first two white space
separated fields and when both input and output are ASCII all trailing portions
of the input line are appended to the output line.

Input geographic data (longitude and latitude) must be in DMS or decimal
degrees format and input cartesian data must be in units consistent with the
ellipsoid major axis or sphere radius units. Output geographic coordinates will
normally be in DMS format (use ``-f %.12f`` for decimal degrees with 12 decimal
places), while projected (cartesian) coordinates will be in linear
(meter, feet) units.


Example
*******

The following script

::

    cs2cs +proj=latlong +datum=NAD83 +to +proj=utm +zone=10 +datum=NAD27 -r <<EOF
    45d15'33.1" 111.5W
    45d15.551666667N -111d30
    +45.25919444444 111d30'000w
    EOF

will transform the input NAD83 geographic coordinates into NAD27 coordinates in
the UTM projection with zone 10 selected. The geographic values of this
example are equivalent and meant as examples of various forms of DMS input.
The x-y output data will appear as three lines of:

::

    1402285.98  5076292.42 -0.00

.. only:: man

    See also
    ********

    **proj(1)**, **cct(1)**, **geod(1)**, **gie(1)**

    Bugs
    ****

    A list of know bugs can be found at https://github.com/OSGeo/proj.4/issues
    where new bug reports can be submitted to.

    Home page
    *********

    https://proj4.org/

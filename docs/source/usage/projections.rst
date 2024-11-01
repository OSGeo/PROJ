.. _projections_intro:

================================================================================
Cartographic projection
================================================================================

The foundation of PROJ is the large number of
:doc:`projections<../operations/projections/index>` available in the library. This section
is devoted to the generic parameters that can be used on any projection in the
PROJ library.

Below is a list of PROJ parameters which can be applied to most coordinate
system definitions. This table does not attempt to describe the parameters
particular to particular projection types. These can be found on the pages
documenting the individual :doc:`projections<../operations/projections/index>`.

    ==========   ================================================================
    Parameter    Description
    ==========   ================================================================
    +a           Semimajor radius of the ellipsoid axis (always in meters)
    +axis        Axis orientation
    +b           Semiminor radius of the ellipsoid axis (always in meters)
    +ellps       Ellipsoid name (see ``proj -le``)
    +k           Scaling factor (deprecated)
    +k_0         Scaling factor
    +lat_0       Latitude of origin (in degree if no angular unit specified)
    +lon_0       Central meridian (in degree if no angular unit specified)
    +lon_wrap    Center longitude to use for wrapping (see below)
    +over        Allow longitude output outside -180 to 180 range, disables
                 wrapping (see below)
    +pm          Alternate prime meridian (typically a city name, see below)
    +proj        Projection name (see ``proj -l``)
    +units       Horizontal coordinate system units (meters, US survey feet, etc.)
    +vunits      Vertical coordinate system units.
    +x_0         False easting (always in meters)
    +y_0         False northing (always in meters)
    ==========   ================================================================

In the sections below most of the parameters are explained in details.

.. _projection_units:

Units
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Horizontal coordinate system units can be specified using the ``+units`` keyword
with a symbolic me for a unit (i.e. ``us-ft``).
Alternatively the translation to meters can be
specified with the ``+to_meter`` keyword (i.e. 0.304800609601219 for US feet).  The
``-lu`` argument to :program:`cs2cs` or :program:`proj` can be used to list
symbolic unit names. The default unit for projected coordinates is the meter.
A few special projections deviate from this behavior, most notably the
latlong pseudo-projection that returns degrees.
Note that this does *not* affect the units of linear parameters such as ``+x_0``,
``+y_0``, ``+a`` or ``+b`` which should always be specified in meters.

Vertical (Z) units can be specified using the ``+vunits`` keyword with a
symbolic name for a unit (i.e. ``us-ft``).  Alternatively the translation to
meters can be specified with the ``+vto_meter`` keyword (i.e. 0.304800609601219
for US feet).  The ``-lu`` argument to :program:`cs2cs` or :program:`proj` can
be used to list symbolic unit names.  If no vertical units are specified, the
vertical units will default to be the same as the horizontal coordinates.

.. note::
    :program:`proj` does not handle vertical units at all and hence the
    ``+vto_meter`` argument will be ignored.

Scaling of output units can be done by applying the ``+k_0`` argument (unitless).
The returned coordinates are scaled by the value assigned with the ``+k_0``
parameter. This parameter is only used by projections that mention using it,
and its exact effect is projection dependent.

Input units for angular parameters (``+lon_0``, ``+lat_0``, ``+pm``, etc.) are
interpreted to be decimal degrees by convention.
Explicit specification of input angular units can be accomplished by adding the
appropriate suffix to input values.


    +----------------+---------------------+
    | Suffix         | Unit                |
    +================+=====================+
    | d              | Decimal degrees     |
    +----------------+                     +
    | D              |                     |
    +----------------+                     +
    | °              |                     |
    +----------------+---------------------+
    | r              | Radians             |
    +----------------+                     +
    | R              |                     |
    +----------------+---------------------+

The sign of an angle is taken from either a ``-`` or ``+`` prefix,
or the last character specifying a cardinal direction,
where ``e``/``E`` (East) or ``n``/``N`` (North) are positive
and ``w``/``W`` (West) or ``s``/``S`` (South) are negative.

Example of use.  The longitude of the central meridian ``+lon_0=90``, can also be expressed more explicitly
with units of decimal degrees as ``+lon_0=90d`` or in radian
units as ``+lon_0=1.570796r`` (approximately).

Angles can be expressed in DMS notation, using ``'`` after arcminutes
and ``"`` after arcseconds, ending with optional cardinal direction.
For example, ``+pm=3d41'14.55"W``, but
character escapes ``+pm=3d41\'14.55\"W`` may be required.

Degree-minute notation does not require any quotation symbols.
All of these are equivalent values: ``+pm=-17d40``, ``+pm=17D40W``,
``+pm=17°40w`` or ``+pm=17d40'W`` (escaped as ``+pm=17d40\'W``).


False Easting/Northing
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Virtually all coordinate systems allow for the presence of a false easting
(``+x_0``) and northing (``+y_0``).  Note that these values are always expressed in
meters even if the coordinate system is some other units.  Some coordinate
systems (such as UTM) have implicit false easting and northing values.

.. _longitude_wrapping:

Longitude Wrapping
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

By default PROJ wraps output longitudes in the range -180 to 180.  The ``+over``
switch can be used to disable the default wrapping which is done at a low level
in ``pj_inv()``.  This is particularly useful with projections like the
:ref:`eqc` or :ref:`merc`
where it could be desirable for X values past -20000000 (roughly) to continue
past -180 instead of wrapping to +180.

Note however that for most projections where the 180 meridian does not project
to a straight line, ``+over`` will have no effect or will not lead to expected
results.

The ``+lon_wrap`` option can be used to provide an alternative means of doing
longitude wrapping. It has only effect with operations that output angular
coordinates, such as ``+proj=longlat``  The argument to this option is a
center longitude.  So ``+lon_wrap=180`` means wrap longitudes in the range 0 to
360.

::

    $ echo -1 0 cs2cs +proj=longlat +to +proj=longlat +lon_wrap=180
    359dE   0dN 0.000

Note that ``+over`` does **not** disable ``+lon_wrap``.

Prime Meridian
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

A prime meridian may be declared indicating the longitude offset between
the prime meridian of the declared coordinate system and that of greenwich.
A prime meridian is declared using the ``+pm`` parameter, and may be assigned
an angle in DMS or decimal degrees format, or as a hard-coded name.

Currently prime meridian declarations are not used by the ``pj_inv()`` and
``pj_fwd()`` calls.
Consequently the user utility :program:`cs2cs` does honour prime meridians but
the :program:`proj` user utility ignores them.

Hard-coded prime meridians can be listed with ``cs2cs -lm``.
Note that the following list is no longer updated, and some values
may conflict with other sources.

 ===========     ================
 Meridian        Longitude
 ===========     ================
   greenwich     0dE
      lisbon     9d07'54.862"W
       paris     2d20'14.025"E
      bogota     74d04'51.3"W
      madrid     3d41'16.58"W
        rome     12d27'8.4"E
        bern     7d26'22.5"E
     jakarta     106d48'27.79"E
       ferro     17d40'W
    brussels     4d22'4.71"E
   stockholm     18d3'29.8"E
      athens     23d42'58.815"E
        oslo     10d43'22.5"E
  copenhagen     12d34'40.35"E
 ===========     ================

Example of use.  The location ``long=0``, ``lat=0`` in the greenwich based lat/long
coordinates is translated to lat/long coordinates with Lisbon as the prime
meridian.

::

    cs2cs +proj=latlong +datum=WGS84 +to +proj=latlong +datum=WGS84 +pm=lisbon
    0 0
    9d7'54.862"E    0dN 0.000

Decimal degrees can also be simply specified for the prime meridian.

::

    echo 13d30E 45N | proj +proj=merc +pm=13.5
    0.00    5591295.92

See :ref:`projection_units` for other examples of angular inputs.


Axis orientation
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Starting in PROJ 4.8.0, the +axis argument can be used to control the axis
orientation of the coordinate system.  The default orientation is "easting,
northing, up" but directions can be flipped, or axes flipped using combinations
of the axes in the +axis switch.  The values are:

* "e" - Easting
* "w" - Westing
* "n" - Northing
* "s" - Southing
* "u" - Up
* "d" - Down

They can be combined in +axis in forms like:

* ``+axis=enu`` - the default easting, northing, elevation.
* ``+axis=neu`` - northing, easting, up - useful for "lat/long" geographic
  coordinates, or south orientated transverse mercator.
* ``+axis=wnu`` - westing, northing, up - some planetary coordinate systems
  have "west positive" coordinate systems

.. note::

    The ``+axis`` argument does not work with the :program:`proj` command line
    utility.


Order of applications of parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

In the forward direction (from geodetic to projected coordinates), steps
are performed in the following order:

- subtracting prime meridian (``+pm``) to input coordinate longitude
- subtracting central meridian (``+lon_0``) to input coordinate longitude
- normalizing input coordinate longitude to [-180, 180], unless ``+over`` is
  specified
- application of map projection formula on a spheroid of radius 1 and with
  the eccentricity of the target spheroid specified. Includes taking into account
  projection parameters such as ``+k_0``, ``+lat_0``, and other projection specific
  parameters
- scaling of output x, y values by the semimajor axis
- addition of false easting ``+x_0`` to x and false northing ``+y_0`` to y
- scaling of output x, y by the output horizontal unit (``+units`` / ``+to_meter``)
- scaling of output z by the output vertical unit (``+vunits`` / ``+vto_meter``)
- application of axis orientation and order (``+axis``)

This is implemented mostly in :source_file:`src/fwd.cpp`

For the reverse direction (from projected coordinates to geodetic coordinates),
reverse steps are performed in the reverse order.

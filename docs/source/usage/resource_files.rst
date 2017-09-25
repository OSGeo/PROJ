.. _resource_files:

================================================================================
Resource files
================================================================================

A number of files containing preconfigured transformations and default parameters
for certain projections are bundled with the PROJ.4 distribution. Init files
contains preconfigured proj-strings for various coordinate reference systems
and the defaults file contains default values for parameters of select
projections.

Init files
-------------------------------------------------------------------------------

Init files are used for preconfiguring proj-strings for often used
transformations, such as those found in the EPSG database. Most init files contain
transformations from a given coordinate reference system to WGS84. This makes
it easy to transformations between any two coordinate reference systems with
``cs2cs``. Init files can however contain any proj-string and don't necesarily
have to follow the *cs2cs* paradigm where WGS84 is used as a pivot datum. The
ITRF init file is a good example of that.

A number of init files come pre-bundled with PROJ.4 but it is also possible to
add your own custom init files. PROJ.4 looks for the init files in the directoty
listed in the ``PROJ_LIB`` environment variable.

The format of init files made up of a identifier in angled brackets and a
proj-string:

::

    <3819> +proj=longlat +ellps=bessel
           +towgs84=595.48,121.69,515.35,4.115,-2.9383,0.853,-3.408 +no_defs <>

The above example is the first entry from the ``epsg`` init file. So, this is the
coordinate reference system with ID 3819 in the EPSG database. Comments can be
inserted by prefixing them with a "#". With version 4.10.0 a new special metadata
entry is now accepted in init files. It can be parsed with a function from the public
API. The metadata entry in the epsg init file looks like this at the time of writing:

::

<metadata> +version=9.0.0 +origin=EPSG +lastupdate=2017-01-10

Pre-configured proj-strings from init files are used in the following way:

::

    $ cs2cs -v +proj=latlong +to +init=epsg:3819
    # ---- From Coordinate System ----
    #Lat/long (Geodetic alias)
    #
    # +proj=latlong +ellps=WGS84
    # ---- To Coordinate System ----
    #Lat/long (Geodetic alias)
    #
    # +init=epsg:3819 +proj=longlat +ellps=bessel
    # +towgs84=595.48,121.69,515.35,4.115,-2.9383,0.853,-3.408 +no_defs

It is possible to override parameters when using ``+init``. Just add the parameter
to the proj-string alongside the ``+init`` parameter. For instance by overriding
the ellipsoid as in the following example

::

    +init=epsg:25832 +ellps=intl

where the Hayford ellipsoid is used instead of the predefined GRS80 ellipsoid.
It is also possible to add additional parameters not specified in the init file,
for instance by adding an obervation epoch when transforming from ITRF2000 to
ITRF2005:

::

    +init=ITRF2000:ITRF2005 +tobs=2010.5

which then expands to

::

    +proj=helmert +x=-0.0001 +y=0.0008 +z=0.0058 +s=-0.0004
    +dx=0.0002 +dy=-0.0001 +dz=0.0018 +ds=-0.000008
    +epoch=2000.0 +transpose
    +tobs=2010.5

Below is a list of the init files that are packaged with PROJ.4.

    ========    ================================================================
    Name        Description
    ========    ================================================================
    esri        Auto-generated mapping from Esri projection index. Not
                maintained any more
    epsg        EPSG database
    GL27        Great Lakes Grids
    IGNF        French coordinate systems supplied by the IGNF
    ITRF2000    Full set of transformation parameters between ITRF2000 and other
                ITRF's
    ITRF2008    Full set of transformation parameters between ITRF2008 and other
                ITRF's
    ITRF2014    Full set of transformation parameters between ITRF2014 and other
                ITRF's
    nad27       State plane coordinate systems, North American Datum 1927
    nad83       State plane coordinate systems, North American Datum 1983
    ========    ================================================================


Defaults file
-------------------------------------------------------------------------------

The ``proj_def.dat`` file supplies default parameters for PROJ.4. It uses the same
syntax as the init files described above. The identifiers in the defaults file
describe to what the parameters should apply. If the ``<general>`` identifier is
used, then all parameters in that section applies for all proj-strings. Otherwise
the identifier is connected to a specific projection. With the defaults file
supplied with PROJ.4 the default ellipsoid is set to WGS84 (for all proj-strings).
Apart from that only the Albers Equal Area,
:doc:`Lambert Conic Conformal<../projections/lcc>` and the
:doc:`Lagrange<../projections/lagrng>` projections have default parameters.
Defaults can be ignored by adding the ``+no_def`` parameter to a proj-string.


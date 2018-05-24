.. _resource_files:

================================================================================
Resource files
================================================================================

A number of files containing preconfigured transformations and default parameters
for certain projections are bundled with the PROJ distribution. Init files
contains preconfigured proj-strings for various coordinate reference systems
and the defaults file contains default values for parameters of select
projections.

In addition to the bundled init-files the PROJ.4 project also distribute a number
of packages containing transformation grids and additional init-files not included
in the main PROJ package.

External resources
-------------------------------------------------------------------------------

For a functioning PROJ installation of the
`proj-datumgrid <https://github.com/OSGeo/proj-datumgrid>`_ is needed. If you
have installed PROJ from a package system chances are that this will already be
done for you. The *proj-datumgrid* package provides transformation grids that
are essential for many of the predefined transformations in PROJ. Which grids
are included in the package can be seen on the
`proj-datumgrid repository <https://github.com/OSGeo/proj-datumgrid>`_ as well
as descriptions of those grids.

In addition to the default *proj-datumgrid* package regional packages are also
distributed. These include grids and init-files that are valid within the given
region. The packages are divided into geographical regions in order to keep the
needed disk space by PROJ at a minimum. Some users may have a use for resource
files covering several regions in which case they can download more than one.

At the moment three regional resource file packages are distributed:

* `Europe <https://github.com/OSGeo/proj-datumgrid/tree/master/europe#proj-datumgrid-europe>`_
* `Oceania <https://github.com/OSGeo/proj-datumgrid/tree/master/oceania#proj-datumgrid-oceania>`_
* `North America <https://github.com/OSGeo/proj-datumgrid/tree/master/north-america#proj-datumgrid-north-america>`_ 

Click the links to jump to the relevant README files for each package. Details
on the content of the packages maintained there.

Links to the resource packages can be found in the :ref:`download section <download>`.

.. _transformation_grids:

Transformation grids
-------------------------------------------------------------------------------

Grid files are important for shifting and transforming between datums.

PROJ supports CTable2, NTv1 and NTv2 files for horizontal grid corrections and
the GTX file format for vertical corrections. Details about the formats can be
found in the `GDAL documentation <http://gdal.org/>`_. GDAL reads and writes
all formats. Using GDAL for construction of new grids is recommended.

Below is a given a list of grid resources for various countries which are not
included in the grid distributions mentioned above.

Free grids
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Below is a list of grids distributed under a free and open license.

Switzerland
................................................................................

Background in ticket `#145 <https://github.com/OSGeo/proj.4/issues/145>`__

We basically have two shift grids available. An official here:

`Swiss CHENyx06 dataset in NTv2 format <https://shop.swisstopo.admin.ch/en/products/geo_software/GIS_info>`__

And a derived in a temporary location which is probably going to disappear soon.

Main problem seems to be there's no mention of distributivity of the grid from
the official website.  It just tells: "you can use freely".  The "contact" link
is also broken, but maybe someone could make a phone call to ask for rephrasing
that.

Hungary
................................................................................

`Hungarian grid <https://github.com/OSGeoLabBp/eov2etrs/>`__ ETRS89 - HD72/EOV (epsg:23700), both horizontal and elevation grids


.. _nonfreegrids:

Non-Free Grids
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Not all grid shift files have licensing that allows them to be freely
distributed, but can be obtained by users through free and legal methods.

Canada NTv2.0
................................................................................
Although NTv1 grid shifts are provided freely with PROJ, the higher-quality
NTv2.0 file needs to be downloaded from Natural Resources Canada. More info:
http://www.geod.nrcan.gc.ca/tools-outils/ntv2_e.php.

Procedure:

1. Visit the `NTv2 <http://webapp.geod.nrcan.gc.ca/geod/tools-outils/applications.php?locale=en#ntv2>`__, and register/login
2. Follow the Download NTv2 link near the bottom of the page.
3. Unzip ``ntv2_100325.zip`` (or similar), and move the grid shift file ``NTV2_0.GSB`` to the proj directory (be sure to change the name to lowercase for consistency) 
   e.g.: ``mv NTV2_0.GSB /usr/local/share/proj/ntv2_0.gsb``
4. Test it using:

    ::

        cs2cs +proj=latlong +ellps=clrk66 +nadgrids=@ntv2_0.gsb +to +proj=latlong +ellps=GRS80 +datum=NAD83
        -111 50

    ::

        111d0'3.006"W   50d0'0.103"N 0.000  # correct answer

Australia
................................................................................

`Geocentric Datum of Australia AGD66/AGD84 <http://www.icsm.gov.au/gda/tech.html>`__

Canada
................................................................................

`Canadian NTv2 grid shift binary <http://open.canada.ca/data/en/dataset/b3534942-31ea-59cf-bcc3-f8dc4875081a>`__ for NAD27 <=> NAD83.

Germany
................................................................................

`German BeTA2007 DHDN GK3 => ETRS89/UTM <http://crs.bkg.bund.de/crseu/crs/descrtrans/BeTA/de_dhdn2etrs_beta.php>`__

Great Britain
................................................................................


`Great Britain's OSTN15_NTv2: OSGB 1936 => ETRS89 <https://www.ordnancesurvey.co.uk/docs/gps/OSTN15_NTv2.zip>`__

`Great Britain's OSTN02_NTv2: OSGB 1936 => ETRS89 <http://www.ordnancesurvey.co.uk/business-and-government/help-and-support/navigation-technology/os-net/ostn02-ntv2-format.html>`__

Austria
................................................................................

`Austrian Grid <http://www.bev.gv.at/portal/page?_pageid=713,2204753&_dad=portal&_schema=PORTAL>`__ for MGI

Spain
................................................................................

`Spanish grids <http://www.ign.es/ign/layoutIn/herramientas.do#DATUM>`__ for ED50.

Portugal
................................................................................

`Portuguese grids <http://www.fc.up.pt/pessoas/jagoncal/coordenadas/index.htm>`__ for ED50, Lisbon 1890, Lisbon 1937 and Datum 73

Brazil
................................................................................

`Brazilian grids <http://www.ibge.gov.br/home/geociencias/geodesia/param_transf/default_param_transf.shtm>`__ for datums Corrego Alegre 1961, Corrego Alegre 1970-72, SAD69 and SAD69(96)

South Africa
................................................................................

`South African grid <http://eepublishers.co.za/article/datum-transformations-using-the-ntv2-grid.html>`__ (Cape to Hartebeesthoek94 or WGS84)

Netherlands
................................................................................

`Dutch grid <https://www.kadaster.nl/web/Themas/Registraties/Rijksdriehoeksmeting/Transformatie-van-coordinaten.htm>`__ (Registration required before download)



HARN
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

With the support of `i-cubed <http://www.i3.com>`__, Frank Warmerdam has
written tools to translate the HPGN grids from NOAA/NGS from ``.los/.las`` format
into NTv2 format for convenient use with PROJ.  This project included
implementing a `.los/.las reader <https://github.com/OSGeo/gdal/tree/trunk/gdal/frmts/raw/loslasdataset.cpp>`__
for GDAL, and an `NTv2 reader/writer <https://github.com/OSGeo/gdal/tree/trunk/gdal/frmts/raw/ntv2dataset.cpp>`__.
Also, a script to do the bulk translation was implemented in
https://github.com/OSGeo/gdal/tree/trunk/gdal/swig/python/samples/loslas2ntv2.py.
The command to do the translation was:

::

    loslas2ntv2.py -auto *hpgn.los

As GDAL uses NAD83/WGS84 as a pivot datum, the sense of the HPGN datum shift offsets were negated to map from HPGN to NAD83 instead of the other way.  The files can be used with PROJ like this:

::

      cs2cs +proj=latlong +datum=NAD83
            +to +proj=latlong +nadgrids=./azhpgn.gsb +ellps=GRS80

::

    # input:
    -112 34

::

    # output:
    111d59'59.996"W 34d0'0.006"N -0.000

This was confirmed against the `NGS HPGN calculator
<http://www.ngs.noaa.gov/cgi-bin/nadcon2.prl>`__.

The grids are available at http://download.osgeo.org/proj/hpgn_ntv2.zip

HTDP
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This page documents use of the `crs2crs2grid.py` script and the HTDP
(Horizontal Time Dependent Positioning) grid shift modelling program from
NGS/NOAA to produce PROJ compatible grid shift files for fine grade
conversions between various NAD83 epochs and WGS84.  Traditionally PROJ has
treated NAD83 and WGS84 as equivalent and failed to distinguish between
different epochs or realizations of those datums.  At the scales of much
mapping this is adequate but as interest grows in high resolution imagery and
other high resolution mapping this is inadequate.  Also, as the North American
crust drifts over time the displacement between NAD83 and WGS84 grows (more
than one foot over the last two decades).

Getting and building HTDP
................................................................................

The HTDP modelling program is in written FORTRAN.  The source and documentation
can be found on the HTDP page at http://www.ngs.noaa.gov/TOOLS/Htdp/Htdp.shtml

On linux systems it will be necessary to install `gfortran` or some FORTRAN
compiler.  For ubuntu something like the following should work.

::

    apt-get install gfortran

To compile the program do something like the following to produce the binary "htdp" from the source code.

::

    gfortran htdp.for -o htdp

Getting crs2crs2grid.py
................................................................................

The `crs2crs2grid.py` script can be found at
https://github.com/OSGeo/gdal/tree/trunk/gdal/swig/python/samples/crs2crs2grid.py

It depends on having the GDAL Python bindings operational.  If they are not


::

    Traceback (most recent call last):
      File "./crs2crs2grid.py", line 37, in <module>
        from osgeo import gdal, gdal_array, osr
    ImportError: No module named osgeo

Usage
................................................................................

::

    crs2crs2grid.py
            <src_crs_id> <src_crs_date> <dst_crs_id> <dst_crs_year>
            [-griddef <ul_lon> <ul_lat> <ll_lon> <ll_lat> <lon_count> <lat_count>]
            [-htdp <path_to_exe>] [-wrkdir <dirpath>] [-kwf]
            -o <output_grid_name>

 -griddef: by default the following values for roughly the continental USA
           at a six minute step size are used:
           -127 50 -66 25 251 611
 -kwf: keep working files in the working directory for review.

::

    crs2crs2grid.py 29 2002.0 8 2002.0 -o nad83_2002.ct2

The goal of `crs2crs2grid.py` is to produce a grid shift file for a designated
region.  The region is defined using the `-griddef` switch.  When missing a
continental US region is used.  The script creates a set of sample points for
the grid definition, runs the "htdp" program against it and then parses the
resulting points and computes a point by point shift to encode into the final
grid shift file.  By default it is assumed the `htdp` program will be in the
executable path.  If not, please provide the path to the executable using the
`-htdp` switch.

The `htdp` program supports transformations between many CRSes and for each (or
most?) of them you need to provide a date at which the CRS is fixed.  The full
set of CRS Ids available in the HTDP program are:

::

  1...NAD_83(2011) (North America tectonic plate fixed)
  29...NAD_83(CORS96)  (NAD_83(2011) will be used)
  30...NAD_83(2007)    (NAD_83(2011) will be used)
  2...NAD_83(PA11) (Pacific tectonic plate fixed)
  31...NAD_83(PACP00)  (NAD 83(PA11) will be used)
  3...NAD_83(MA11) (Mariana tectonic plate fixed)
  32...NAD_83(MARP00)  (NAD_83(MA11) will be used)

  4...WGS_72                             16...ITRF92
  5...WGS_84(transit) = NAD_83(2011)     17...ITRF93
  6...WGS_84(G730) = ITRF92              18...ITRF94 = ITRF96
  7...WGS_84(G873) = ITRF96              19...ITRF96
  8...WGS_84(G1150) = ITRF2000           20...ITRF97
  9...PNEOS_90 = ITRF90                  21...IGS97 = ITRF97
 10...NEOS_90 = ITRF90                   22...ITRF2000
 11...SIO/MIT_92 = ITRF91                23...IGS00 = ITRF2000
 12...ITRF88                             24...IGb00 = ITRF2000
 13...ITRF89                             25...ITRF2005
 14...ITRF90                             26...IGS05 = ITRF2005
 15...ITRF91                             27...ITRF2008
                                         28...IGS08 = ITRF2008

The typical use case is mapping from NAD83 on a particular date to WGS84 on
some date.  In this case the source CRS Id "29" (NAD_83(CORS96)) and the
destination CRS Id is "8 (WGS_84(G1150)).  It is also necessary to select the
source and destination date (epoch).  For example:

::

    crs2crs2grid.py 29 2002.0 8 2002.0 -o nad83_2002.ct2

The output is a CTable2 format grid shift file suitable for use with PROJ
(4.8.0 or newer).  It might be utilized something like:


::

    cs2cs +proj=latlong +ellps=GRS80 +nadgrids=./nad83_2002.ct2 +to +proj=latlong +datum=WGS84

See Also
................................................................................

* http://www.ngs.noaa.gov/TOOLS/Htdp/Htdp.shtml - NGS/NOAA page about the HTDP
  model and program.  Source for the HTDP program can be downloaded from here.


.. _init_files:

Init files
-------------------------------------------------------------------------------

Init files are used for preconfiguring proj-strings for often used
transformations, such as those found in the EPSG database. Most init files contain
transformations from a given coordinate reference system to WGS84. This makes
it easy to transformations between any two coordinate reference systems with
``cs2cs``. Init files can however contain any proj-string and don't necessarily
have to follow the *cs2cs* paradigm where WGS84 is used as a pivot datum. The
ITRF init file is a good example of that.

A number of init files come pre-bundled with PROJ but it is also possible to
add your own custom init files. PROJ looks for the init files in the directory
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
for instance by adding an observation epoch when transforming from ITRF2000 to
ITRF2005:

::

    +init=ITRF2000:ITRF2005 +t_obs=2010.5

which then expands to

::

    +proj=helmert +x=-0.0001 +y=0.0008 +z=0.0058 +s=-0.0004
    +dx=0.0002 +dy=-0.0001 +dz=0.0018 +ds=-0.000008
    +t_epoch=2000.0 +transpose
    +t_obs=2010.5

Below is a list of the init files that are packaged with PROJ.

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


The defaults file
-------------------------------------------------------------------------------

The ``proj_def.dat`` file supplies default parameters for PROJ. It uses the same
syntax as the init files described above. The identifiers in the defaults file
describe to what the parameters should apply. If the ``<general>`` identifier is
used, then all parameters in that section applies for all proj-strings. Otherwise
the identifier is connected to a specific projection. With the defaults file
supplied with PROJ the default ellipsoid is set to WGS84 (for all proj-strings).
Apart from that only the Albers Equal Area,
:doc:`Lambert Conic Conformal<operations/projections/lcc>` and the
:doc:`Lagrange<operations/projections/lagrng>` projections have default parameters.
Defaults can be ignored by adding the ``+no_def`` parameter to a proj-string.



.. _rfc4:

====================================================================
PROJ RFC 4: Remote access to grids and GeoTIFF grids
====================================================================

:Author: Even Rouault, Howard Butler
:Contact: even.rouault@spatialys.com, howard@hobu.co
:Status: Draft
:Implementation target: PROJ 7
:Last Updated: 2019-11-25

Motivation
-------------------------------------------------------------------------------

PROJ 6 brings undeniable advances in the management of coordinate
transformations between datums by relying and applying information available in
the PROJ database. PROJ's rapid evolution from a cartographic projections
library with a little bit of geodetic capability to a full geodetic
transformation and description environment has highlighted the importance of
the support data. Users desire the convenience of software doing the right
thing with the least amount of fuss, and survey organizations wish to deliver
their models across as wide a software footprint as possible.  To get results
with the highest precision, a grid file that defines a model that provides
dimension shifts is often needed. The proj-datumgrid project centralizes grids
available under an open data license and bundles them in different archives
split along major geographical regions of the world .

It is assumed that a PROJ user has downloaded and installed grid files that are
referred to in the PROJ database. These files can be quite large in aggregate,
and packaging support by major distribution channels is somewhat uneven due to
their size, sometimes ambiguous licensing story, and difficult-to-track
versioning and lineage.  It is not always clear to the user, especially to
those who may not be so familiar with geodetic operations, that the highest
precision transformation may not always being applied if grid data is not
available. Users want both convenience and correctness, and management of the
shift files can be challenging to those who may not be aware of their
importance to the process.

The computing environment in which PROJ operates is also changing. Because the
shift data can be so large (currently more than 700 MB of uncompressed data,
and growing), deployment of high accuracy operations can be limited due to
deployment size constraints (serverless operations, for example). Changing to a
delivery format that supports incremental access over a network along with
convenient access and compression will ease the resource burden the shift files
present while allowing the project to deliver transformation capability with
the highest known precision provided by the survey organizations.

Adjustment grids also tend to be provided in many different formats depending
on the organization and country that produced them. In PROJ, we have over time
"standardized" on using horizontal shift grids as NTv2 and vertical shift grids
using GTX. Both have poor general support as dedicated formats, limited
metadata capabilities, and neither are not necessarily "cloud optimized" for
incremental access across a network.

Summary of work planned by this RFC
-------------------------------------------------------------------------------

- Grids will be hosted by one or several Content Delivery Networks (CDN)
- Users no longer have to manually fetch grid files and place them in PROJ_LIB.
  Full and accurate capability of the software will no longer require hundreds
  of megabytes of grid shift files in advance, even if only just a few of them
  are needed for the transformations done by the user.
- Local caching of grid files, or even part of files, so that users end up
  mirroring what they actually use.
- A grid shift format, for both horizontal and vertical shift grids (and in
  potential future steps, for other needs, such as deformation models) will be
  implemented.

The use of grids locally available will of course still be available, and will
be the default behaviour. 

Network access to grids
-------------------------------------------------------------------------------

curl will be an optional build dependency of PROJ, added in autoconf and cmake
build systems. When curl will be detected to be available, it will be enabled
by default. But download of grids themselves will not be enabled by default,
and will require explicit consent of the user, either through the API
(:c:func:`proj_context_enable_network`) or through the PROJ_NETWORK=ON
environment variable.

Regarding the minimum version of libcurl required, given GDAL experience that
can build with rather ancient libcurl for similar functionality, we can aim for
libcurl >= 7.29.0 (as being available in RHEL 7).

An alternate pluggable network interface can also be set by the user in case
suppot for libcurl was not built in, or if for the desired context of use, the
user wishes to provide the network implementation (a typical use case could be
QGIS that would use its QT-based networking facilities to solve issues with
SSL, proxy, authentication, etc.)

A text configuration file, installed in ${installation_prefix}/share/proj/proj.ini
(or ${PROJ_LIB}/proj.ini)
will contain the URL of the CDNs that will be tried. If several are available,
they will be tried in sequence. The user may also override this setting with the
:c:func:`proj_context_network_set_url_endpoints` or through the PROJ_NETWORK_ENDPOINTS
environment variable.

The rationale for putting proj.ini in that location is
that it is a well-known place by PROJ users, with the exiting PROJ_LIB mechanics
for systems like Windows where hardcoded paths at runtime aren't generaly usable.

C API
+++++

The preliminary C API for the above is:

.. code-block:: c

    /** Enable or disable network access.
    *
    * @param ctx PROJ context, or NULL
    * @return TRUE if network access is possible. That is either libcurl is
    *         available, or an alternate interface has been set.
    */
    int proj_context_enable_network(PJ_CONTEXT* ctx, int enable);

    /** Define URL endpoints to query for remote grids.
    *
    * This overrides the default endpoints in the PROJ configuration file.
    *
    * @param ctx PROJ context, or NULL
    * @param urls NUL-terminated array of URL
    * @return TRUE in case of success.
    */
    int proj_context_network_set_url_endpoints(PJ_CONTEXT* ctx,
                                               const char* const * urls);

    /** Opaque structure for PROJ. Implementations might cast it to their
     * structure/class of choice. */
    typedef struct PROJ_NETWORK_HANDLE PROJ_NETWORK_HANDLE;

    /** Define a custom set of callbacks for network access.
    *
    * @param ctx PROJ context, or NULL
    * @param open Callback to open a remote file given its URL
    * @param close Callbak to close a remote file.
    * @param get_file_size Callback to get the size of the remote file.
    * @param read_range Callback to read a range of bytes inside a remote file. Returns the number of bytes written into the buffer.
    * @param user_data Arbitrary pointer provided by the user, and passed to the above callbacks.
    * @return TRUE in case of success.
    */
    int proj_context_set_network_callbacks(
        PJ_CONTEXT* ctx,
        PROJ_NETWORK_HANDLE* (*open)(const char* /* url */,
                                     void* /*user_data*/),
        void (*close)               (PROJ_NETWORK_HANDLE*, 
                                     void* /*user_data*/),
        unsigned long long (*get_file_size)(PROJ_NETWORK_HANDLE*,
                                            void* /*user_data*/),
        size_t (*read_range)         (PROJ_NETWORK_HANDLE*,
                                     unsigned long long, /* offset */
                                     size_t, /* size to read */
                                     void*, /* buffer to update with bytes read*/
                                     void* /*user_data*/),
        void* user_data);


To make network access efficient, PROJ will internally have a in-memory cache
of file ranges to only issue network requests by chunks of 16 KB or multiple of them,
to limit the number of HTTP GET requests and minimize latency caused by network
access. This is very similar to the behaviour of the GDAL
`/vsicurl/ <https://gdal.org/user/virtual_file_systems.html#vsicurl-http-https-ftp-files-random-access>`_
I/O layer. The plan is to mostly copy GDAL's vsicurl implementation inside PROJ, with
needed adjustmeents and proper namespacing of it.

A retry strategy (typically a delay with an exponential back-off and some random
jitter) will be added to account for intermittent network or server-side failure.

URL building
++++++++++++

The PROJ database has a ``grid_transformation`` grid whose column ``grid_name``
(and possibly ``grid2_name``) contain the name of the grid as indicated by the
authority having registered the transformation (typically EPSG). As those
grid names are not generally directly usable by PROJ, the PROJ database has
also a ``grid_alternatives`` table that link original grid names to the ones used
by PROJ. When network access will be available and needed due to lack of a
local grid, the full URL will be the 
endpoint from the configuration or set by the user, the basename of the PROJ
usable filename, and the "tif" suffix. So if the CDN is at http://example.com
and the name from ``grid_alternatives`` is egm96_15.gtx, then the URL will
be http://example.com/egm96_15.tif

Grid loading
++++++++++++

The following files will be affected, in one way or another, by the above describes
changes:
nad_cvt.cpp, nad_intr.cpp, nad_init.cpp, grid_info.cpp, grid_list.cpp, apply_gridshift.cpp,
apply_vgridshift.cpp.

In particular the current logic that consists to ingest all the values of a
grid/subgrid in the ct->cvs array will be completely modified, to enable
access to grid values at a specified (x,y) location.

proj_create_crs_to_crs() / proj_create_operations() impacts
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Once network access is available, all grids known to the PROJ database
(grid_transformation + grid_alternatives table) will be assumed to be available,
when computing the potential pipelines between two CRS.

Concretely, this will be equivalent to calling
:cpp:func:`proj_operation_factory_context_set_grid_availability_use`
with the ``use`` argument set to a new enumeration value 

.. code-block:: c

    /** Results will be presented as if all grids known to PROJ (that is
      registered in the grid_alternatives table of its database) were available. */
    PROJ_GRID_AVAILABILITY_ALL_KNOWN_PRESENT


Local on-disk caching of remote grids
+++++++++++++++++++++++++++++++++++++

As many workflows will tend to use the same grids over and over, a local
on-disk caching of remote grids will be added. The cache will be a single
SQLite3 database, in a user-writable directory shared by all applications using
PROJ. Its total size will be configurable, with a default maximum size of 100 MB
in proj.ini

.. code-block:: c

    /** Override, for the considered context, the path and file of the local
    * cache of grids.
    *
    * @param ctx PROJ context, or NULL
    * @param fullname Full name to the cache. If set to NULL, caching will be
    *                 disabled.
    * @return TRUE in case of success.
    */
    int proj_context_set_grid_cache_filename(PJ_CONTEXT* ctx, 
                                             const char* fullname);

    /** Override, for the considered context, the maximum size of the local
    * cache of grids.
    *
    * @param ctx PROJ context, or NULL
    * @param max_size_MB Maximum size, in mega-bytes (1024*1024 bytes)
    * @return TRUE in case of success.
    */
    int proj_context_set_grid_cache_max_size(PJ_CONTEXT* ctx, int max_size_MB);

The planned database structure is:

.. code-block:: sql

    CREATE TABLE chunks(
        id        INTEGER PRIMARY KEY AUTOINCREMENT,
        filename  TEXT NOT NULL,
        offset    INTEGER NOT NULL,
        data      BLOB NOT NULL
    );

    CREATE INDEX idx_chunks ON chunks USING (filename, offset);

    CREATE TABLE recently_used(
        age        INTEGER UNIQUE, -- 0 is last accessed
        chunk_id   INTEGER UNIQUE NOT NULL,
        CONSTRAINT fk_recently_used_to_chunk FOREIGN KEY (chunk_id) REFERENCES chunks(id)
    );

The chunks table will store 16 KB chunks (or less for terminating chunks).
The recently_used table will act as a least recently used list of chunk ids.

The directory used to locate this database will be ${XDG_DATA_HOME}/proj
(per https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)
where ${XDG_DATA_HOME} defaults to ${HOME}/.local/share on Unix builds
and ${LOCALAPPDATA} on Windows builds. Exact details to be sorted out, but
https://github.com/ActiveState/appdirs/blob/a54ea98feed0a7593475b94de3a359e9e1fe8fdb/appdirs.py#L45-L97
can be a good reference.

As this database might be accesse by several threads or processes at the same
time, the code accessing to it will carefully honour SQLite3 errors regarding
to locks, to do appropriate retries if another thread/process is currently
locking the database. Accesses requiring a modification of the database will
start with a BEGIN IMMEDIATE transaction so as to acquire a write lock.

.. note:: This database should be hosted on a local disk, not a network one.
          Otherwise SQLite3 locking issues are to be expected.

CDN providers
+++++++++++++

The following CDN providers have offered to store PROJ grids:

- `Amazon Public Datasets <https://aws.amazon.com/opendata/public-datasets/>`_
- `Fastly <https://www.fastly.com/>`_

Regarding AWS Public Dataset Program, an application must be submitted at
https://application.opendata.aws. The program covers storage and egress
(bandwidth) of the data. They generally don't allow usage of CloudFront
(their CDN) as part of the program (we would usually look to have it covered
by credits), but in this instance, they would be fine to provide it.
They'd only ask that we keep the CloudFront URL "visible" (as appropriate for
the use case) so people can see where the data is hosted in case they go looking.
Their terms can be seen at https://aws.amazon.com/service-terms/ and CloudFront
has its own, small section. Those terms may change a bit from time to time for
minor changes. Major changing service terms is assumed to be unfrequent.
There are also the Public Dataset Program terms at http://aws.amazon.com/public-datasets/terms/.
Those also do not effectively change over time and are renewed on a 2 year basis.

Criteria for grid hosting
+++++++++++++++++++++++++

The grids hosted on the CDN will be exactly the ones collected,
currently and in the future, by the `proj-datumgrid <https://github.com/OSGeo/proj-datumgrid/>`_
initiative. In particular, new grids are accepted as long as
they are released under a license that is compatible with the
`Open Source Definition <https://opensource.org/osd-annotated>`_ and the source
of the grid is clearly stated and verifiable. Suitable licenses include:

- Public domain
- X/MIT
- BSD 2/3/4 clause
- CC0
- CC-BY (v3.0 or later)
- CC-BY-SA (v3.0 or later)

For new grids to be transparently used by the proj_create_crs_to_crs() mechanics,
they must be registered in the PROJ database (proj.db) in the ``grid_transformation`` and
``grid_alternatives`` table. The nominal path to have a new record in the grid_transformation
is to have a transformation being registered in the EPSG dataset (if there is no
existing one), which will be subsequently imported into the PROJ database.

Versioning, historical preservation of grids
++++++++++++++++++++++++++++++++++++++++++++

The policy regarding this should be similar to the one applied to
`proj-datumgrid <https://github.com/OSGeo/proj-datumgrid/>`_, which even if
not formalized, is around the following lines:

- Geodetic agencies release regularly new version of grids. Typically for the
  USA, NOAA has released GEOID99, GEOID03, GEOID06, GEOID09, GEOID12A, GEOID12B,
  GEOID18 for the NAVD88 to NAD83/NAD83(2011) vertical adjustments. Each of this
  grid is considered by EPSG and PROJ has a separate object, which distinct filenames.
  The release of a new version does not cause the old grid to be automatically removed.
  That said, due to advertized accuracies and supersession rules of the EPSG dataset, the
  most recent grid will generally be used for a CRS -> CRS transformation if the
  user uses proj_create_crs_to_crs() (with the exception that if a VERT_CRS WKT
  includes a GEOID_MODEL known to PROJ, an old version of the grid will be used).
  If the user specifies a whole pipeline with an explicit grid name, it will be
  of course strictly honoured.
  As time goes, the size of the datasets managed by proj-datumgrid will be increasing,
  we will have to explore on we managed that for the distributed .zip / .tar.gz
  archives. This should not be a concern for CDN hosted content.

- In case software-related conversion errors from the original grid format to the
  one used by PROJ (be it GTX, NTv2 or GeoTIFF) would happen, the previous erroneous
  version of the dataset would be replaced by the corrected one. In that situation,
  this might have an effect with the local on-disk caching of remote grids. We will
  have to see with the CDN providers used if we can use for example the ETag HTTP header
  on the client to detect a change, so that old cached content is not erroneously
  reused (if not possible, we'll have to use some text file listing the grid names and their
  current md5sum)


Grids in GeoTIFF format
-------------------------------------------------------------------------------

Limitations of current formats
++++++++++++++++++++++++++++++

Several formats exist depending on the ad-hoc needs and ideas of the original
data producer. It would be apropriate to converge on a common format able to
address the different use cases.

- Not tiled. Tiling is a nice to have propery for cloud-friendly access to
  large files.
- No support for compression
- The NTv2 structures is roughly: header of main grid, data of main grid,
  header of subgrid 1, data of subgrid 1, header of subgrid 2, data of subgrid 2,
  etc.Due to the headers being scattered through the file, it is not possibly
  to retrieve with a single HTTP GET request all header information.
- GTX format has no provision to store metadata besides the minimum georeferencing
  of the grid. NTv2 is a bit richer, but no extensible metadata possible.

Discussion on choice of format
++++++++++++++++++++++++++++++

We have been made recently aware of other initiatives from the industry to come
with a common format to store geodetic adjustment data. Some discussions have
happen recently within the OGC CRS Working group. Past efforts include the
Esri's proposed Geodetic data Grid eXchange Format, GGXF, briefly mentionned at
page 86 of
https://iag.dgfi.tum.de/fileadmin/IAG-docs/Travaux2015/01_Travaux_Template_Comm_1_tvd.pdf
and page 66 of ftp://ftp.iaspei.org/pub/meetings/2010-2019/2015-Prague/IAG-Geodesy.pdf
The current trend of those works would be to use a netCDF / HDF5 container.

So, for the sake of completness, we list hereafter a few potential candidate
formats and their pros and cons.

TIFF/GeoTIFF
************

Strong points:

* TIFF is a well-known and widespread format.

* The GeoTIFF encoding is a widely industry supported scheme to encode georeferencing.
  It is now a `OGC standard <https://www.opengeospatial.org/standards/geotiff>`_

* There are independent initiatives to share grids as GeoTIFF, like
  `that one <https://www.agisoft.com/downloads/geoids/>`_

* TIFF can contain multiple images (IFD: Image File Directory) chained together.
  This is the mechanism used for multiple-page scanned TIFF files, or in the
  geospatial field to store multi-resolution/pyramid rasters. So it can be
  used to sub-grids as in the NTv2 format.

* Extensive experience with the TIFF format, and its appropriatness for network
  access, in particular through the `Cloud Optimized GeoTIFF initiative <https://www.cogeo.org/>`_
  whose layout can make use of sub-grids efficient from a network access
  perspective, because grid headers can be put at the beginning of the file, and
  so being retrieved in a single HTTP GET request.

* TIFF can be tiled.

* TIFF can be compressed. Commonly found compression formats arre DEFLATE, LZW,
  combined with differential integer or floating point predictors

* A TIFF image can contain a configurable number of channels/bands/samples.
  In the rest of the document, we will use the sample terminology for this concept.

* TIFF sample organization can be configured: either the values of different
  samples are packed together (`PlanarConfiguration <https://www.awaresystems.be/imaging/tiff/tifftags/planarconfiguration.html>`_ = Contig), or put in separate tiles/strips
  (PlanarConfiguration = Separate)

* libtiff is a dependency commonly found in binary distributions of the
  "ecosystem" to which PROJ belongs too

* libtiff benefits from many years of efforts to increase its security, for
  example being integrated to the oss-fuzz initiative. Given the potential
  fetching of grids, using security tested components is an important concern.

* Browser-side: there are "ports" of libtiff/libgeotiff in the browser such
  as https://geotiffjs.github.io/ which could potentially make a port of PROJ
  easier.

Weak points:

* we cannot use libgeotiff, since it depends itself on PROJ (to resolve CRS
  or components of CRS from their EPSG codes). That said, for PROJ intended
  use, we only need to decode the ModelTiepointTag and ModelPixelScaleTag TIFF
  tags, so this can be done "at hand"

* the metadata capabilities of TIFF baseline are limited. The TIFF format comes
  with a predefined set of metadata items whose keys have numeric values. That
  said, GDAL has used for the last 20 years or so a dedicated tag,
  `GDAL_METADATA <https://www.awaresystems.be/imaging/tiff/tifftags/gdal_metadata.html>`_
  of code 42112 that holds a XML-formatted string being able to store arbitrary
  key-pair values.

netCDF v3
*********

Strong points:

* The binary format description as given in
  `OGC 10-092r3 <http://portal.opengeospatial.org/files/?artifact_id=43734>`_ is relatively simple,
  but it would still probably be necessary to use libnetcdf-c to access it

* Metadata can be stored easily in netCDF attributes


Weak points:

* No compression in netCDF v3

* No tiling in netCDF v3

* Multi-samples variables are located in different sections of the files
  (correspond to TIFF PlanarConfiguration = Separate)

* No natural way of having hiearchical / multigrids. They must be encoded as
  separate variables

* georeferencing in netCDF is somewhat less standardized than TIFF/GeoTIFF.
  The generally used model is `the conventions for CF (Climate and Forecast)
  metadata <http://cfconventions.org/>`_
  but there is nothing really handy in them for simple georeferencing with
  the coordinate of the upper-left pixel and the resolution. The practice is
  to write explict lon and lat variables with all values taken by the grid.
  GDAL has for many years supported a simpler syntax, using a GeoTransform
  attribute.

* From the format description, its layout could be relatively cloud friendly,
  except that libnetcdf has no API to plug an alternate I/O layer.

* Most binary distributions of netCDF nowadays are based on libnetcdf v4, which
  implies the HDF5 dependency.

* From a few issues we identified a few years ago regarding crashes on corrupted
  datasets, we contacted libnetcdf upstream, but they did not seem to be
  interested in addressing those security issues.

netCDF v4 / HDF5
****************

Note: The netCDF v4 format is a profile of the HDF5 file format.

Strong points:

* Compression supported (ZLIB and SZIP predefined)

* Tiling (chunking) supported

* Values of Multi-sample variables can be interleaved together (similarly
  to TIFF PlanarConfiguration = Contig) by using compound data types.

* Hierarchical organization with groups

* While the netCDF API does not provide an alternate I/O layer, this is
  possible with the HDF5 API.

* Grids can have be indexed by more than 2 dimensions (for current needs, we
  don't need more than 2D support)

Weak points:

* The `HDF 5 File format <https://support.hdfgroup.org/HDF5/doc/H5.format.html>`_
  is more complex than netCDF v3, and likely more than TIFF. We do not have
  in-depth expertise of it to assess its cloud-friendliness.

* The ones mentionned for netCDF v3 regarding georeferencing and security apply.


GeoPackage
**********

As PROJ has already a SQLite3 dependency, GeoPackage could be examined as a
potential solution.

Strong points:

* SQLite3 dependency

* OGC standard

* Multi-grid capabilities

* Tiling

* Compression

* Metadata capabilities

Weak points:

* GeoPackage mostly address the RGB(A) Byte use case, or via the tile gridded
  data extension, single-sample non-Byte data. No native support for multi-sample
  non-Byte data: each sample should be put in a separate raster table.

* Experience shows that SQLite3 layout (at least the layout adopted when using
  the standard libsqlite3) is not cloud friendly. Indices may be scattered in
  different places of the file.

Conclusions
***********

The 2 major contenders regarding our goals and constraints are GeoTIFF and HDF5.
Given past positive experience and its long history, GeoTIFF remains our preferred
choice.


.. _description_geotiff_format:

Description of the PROJ GeoTIFF format
++++++++++++++++++++++++++++++++++++++

The general principles that guide the following requirements and recommendations
are such that files will be properly recognized by PROJ, and also by GDAL which
is an easy way to inspect such grid files:

- `TIFF 6.0 <https://www.awaresystems.be/imaging/tiff/specification/TIFF6.pdf>`_
  based (could possibly be BigTIFF without code changes, if we ever
  need some day to handle grids larger than 4GB)

- `GeoTIFF 1.0 <http://geotiff.maptools.org/spec/geotiffhome.html>`_ or
  `1.1 <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html>`_ for the georeferencing.
  If no vertical reference is used, 1.0 will be used for maximum backward
  compatibility.

- Files hosted on the CDN will use a Geographic 2D CRS for the GeoTIFF GeoKeys.
  That CRS is intended to be the interpolation CRS as defined in
  `OGC Abstract Specification Topic 2 <http://docs.opengeospatial.org/as/18-005r4/18-005r4.html>`_,
  that is the CRS to which grid values are refered to.

  Given that they will nominally be related to the EPSG dataset, the `GeodeticCRSGeoKey
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_requirements_class_geodeticcrsgeokey>`_
  will be used to store the EPSG code of the CRS. If the CRS cannot be reliably
  encoded through that key or other geokeys, the ``interpolation_crs_wkt`` metadata
  item detailed afterwards should be used.

  This CRS will be generally the source CRS (for geographic to
  geographic horizontal shift grids, or geographic to vertical shift grids), but
  for vertical to vertical CRS adjustment, this will be the geographic CRS to
  which the grid is referenced. In some very rare cases of geographic to vertical
  shift grids, the interpolation CRS might be a geographic CRS that is not the
  same as the source CRS (into which ellipsoidal height are expressed). The only
  instance we have in mind is for the EPSG:7001 "ETRS89 to NAP height (1)" transformation
  using the naptrans2008 VDatum-grid which is referenced to Amersfoort EPSG:4289
  instead of ETRS89...

  On the reading side, PROJ will ignore that information:
  the CRS is already stored in the source_crs or interpolation_crs column of the
  grid_transformation table.

  For geographic to vertical shift files (geoid models), the GeoTIFF 1.1
  convention will be used to store the value of the `VerticalGeoKey
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_requirements_class_verticalgeokey>`_
  So a geoid model that apply to WGS 84 EPSG:4979 will have GeodeticCRSGeoKey = 4326
  and VerticalGeoKey = 4979.

- Files hosted on the CDN will use the GeoTIFF defined `ModelTiepointTag and ModelPixelScaleTag
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_raster_to_model_coordinate_transformation_requirements>`_ TIFF tags
  to store the coordinates of the upper-left pixel and the resolution of the pixels.
  On the reading side, they will be required and ModelTransformationTag will be ignored.

  .. note::

    Regarding anti-meridian handling, a variety of possibilities exist.
    We do not attempt to standardize this and filesh hosted on the CDN will use
    a georeferencing close to the original data producer.
    For example, NOAA vertical grids that apply to Conterminous USA might even have a top-left
    longitude beyond 180 (for consistency with Alaska grids, whose origin is < 180)
    Anti-meridian handling in PROJ has probably issues. This RFC does not attempt
    to address them in particular, as they are believed to be orthogonal to the
    topics it covers, and being mostly implementation issues.

- Files hosted on the CDN will use the `GTRasterTypeGeoKey
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_requirements_class_gtrastertypegeokey>`_
  = PixelIsPoint convention.
  This is the convention used by most existing grid formats currently. Note that GDAL
  typically use a PixelIsArea convention (but can handle both conventions), so the
  georeferencing it displays when opening a .gsb or .gtx file appears to have a 
  half-pixel shift regarding to the coordinates stored in the original grid file. On
  the reading side, PROJ will accept both conventions (for equivalent georeferencing,
  the value of the origin in a PixelIsArea convention is shifted by a half-pixel
  towards the upper-left direction). Unspecified behaviour if this GeoKey is absent.

- Files hosted on the CDN will be tiled, presumably with 256x256 tiles (small
  grids that are smaller than 256x256 will use a single strip). On the reading
  side, PROJ will accept TIFF files with any strip or tile organization.
  Tiling is expressed by specifying the TileWidth, TileHeight, TileOffsets
  and TileByteCounts tags. Strip organization is expressed by specifying the
  RowsPerStrip, StripByteCounts and StripOffsets tags.

- Files hosted on the CDN will use `Compression
  <https://www.awaresystems.be/imaging/tiff/tifftags/compression.html>`_ = DEFLATE
  or LZW (to be determined, possibly with
  `Predictor <https://www.awaresystems.be/imaging/tiff/tifftags/predictor.html>`_ = 2
  or 3)
  On the reading side, PROJ will accept TIFF files with any compression method
  (appropriate for the data types and PhotometricInterpretation considered)
  supported by the libtiff build used by PROJ. Of course uncompressed files will be supported.

- Files hosted on the CDN will use little-endian byte ordering. On the reading
  side, libtiff will transparently handle both little-endian and big-endian
  ordering.

- Files hosted on the CDN will use PlanarConfiguration=Contig. On the reading side,
  PROJ will handle also PlanarConfiguration=Separate.

- Files hosted on the CDN will use Signed Int 16 (
  `BitsPerSample <https://www.awaresystems.be/imaging/tiff/tifftags/bitspersample.html>`_ =16 and
  `SampleFormat <https://www.awaresystems.be/imaging/tiff/tifftags/sampleformat.html>`_ = INT)
  or Float32 (BitsPerSample=32 and SampleFormat=IEEEFP). On the reading side,
  only those two data types will be supported as well.

- Files hosted on the CDN will have a `PhotometricInterpretation
  <https://www.awaresystems.be/imaging/tiff/tifftags/photometricinterpretation.html>`_ = MinIsBlack.
  It will be assumed, and ignored on the reading side.

- Files hosted on the CDN will nominally have:

    * `SamplesPerPixel <https://www.awaresystems.be/imaging/tiff/tifftags/samplesperpixel.html>`_ = 2
      for horizontal shift grid, with the first sample being the longitude offset
      and the second sample being the latitude offset.

    * SamplesPerPixel = 1 for vertical shift grids.

   On the reading side, extra samples may be present (to indicate for example
   uncertainties), but will be ignored.

   The `ExtraSamples <https://www.awaresystems.be/imaging/tiff/tifftags/extrasamples.html>`_
   tag should be set to a value of SamplesPerPixel - 1 (given the rules that
   apply for PhotometricInterpretation = MinIsBlack)

- The `ImageDescription <https://www.awaresystems.be/imaging/tiff/tifftags/imagedescription.html>`_
  tag may be used to convey extra information about the name and provenance of the grid.
  Will be set when possible fo files hosted on the CDN.
  Ignored by PROJ.

- The `Copyright <https://www.awaresystems.be/imaging/tiff/tifftags/copyright.html>`_
  tag may be used to convey extra information about the copyright and license of the grid.
  Will be set when possible fo files hosted on the CDN.
  Ignored by PROJ.

- Files hosted on the CDN will use the `GDAL_NODATA
  <https://www.awaresystems.be/imaging/tiff/tifftags/gdal_nodata.html>`_ tag to encode
  the value of the nodata / missing value, when it applies to the grid.

  If offset and/or scaling is used, the nodata value corresponds to the raw value,
  before applying offset and scaling.
  Files hosted on the CDN will use GDAL_NODATA = -32768. On the reading side,
  the value found in this tag, if present, will be honoured (to the extent to
  which current PROJ code makes use of nodata).
  For floating point data, writers are strongly discouraged to use non-finite values
  (+/- infinity, NaN) of nodata to maximimize interoperability.
  The GDAL_NODATA value applies to all samples of a given TIFF IFD.

- Files hosted on the CDN will use the `GDAL_METADATA
  <https://www.awaresystems.be/imaging/tiff/tifftags/gdal_metadata.html>`_ tag to encode extra
  metadata not supported by baseline or extended TIFF.

  * The root XML node should be ``GDALMetadata``

  * Zero, one or several child XML nodes ``Item`` may be present.

  * A Item should have a ``name`` attribute, and a child text node with its value.
    ``role`` and ``sample`` attributes may be present for attributes that have
    a special semantics (recognized by GDAL). The value of `sample` should be
    a integer value between 0 and number_of_samples - 1.

  * Scale and offset to convert integer raw values to floating point values
    may be expressed with XML `Item` elements whose name attribute is respectively
    ``SCALE`` and ``OFFSET``, and their ``role`` attribute is respectively ``scale``
    and ``offset``. The decoded value will be: {offset} + {scale} * raw_value_from_geotiff_file

    For a offset value of 1 and scaling of 2, the following payload should be
    stored:

    .. code-block:: xml

        <GDALMetadata>
            <Item name="OFFSET" sample="0" role="offset">1</Item>
            <Item name="SCALE" sample="0" role="scale">2</Item>
        </GDALMetadata>

  * The type of the grid must be specified with a `Item` whose ``name`` is set
    to ``TYPE``.

    Values recognized by PROJ currently are:

    - ``HORIZONTAL_OFFSET``: implies the presence of at least two samples.
      The first sample must contain the longitude offset and the second
      sample must contain the latitude offset.

    - ``VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL``: implies the presence of at least one sample.
      The first sample must contain the vertical adjustment. Must be used when
      the source/interpolation CRS is a Geographic CRS and the target CRS a Vertical CRS.

    - ``VERTICAL_OFFSET_VERTICAL_TO_VERTICAL``: implies the presence of at least one sample.
      The first sample must contain the vertical adjustment. Must be used when
      the source and target CRS are Vertical CRS.

    For example:

    .. code-block:: xml

        <Item name="TYPE">HORIZONTAL_OFFSET</Item>

  * The description of each sample must be specified with a Item whose ``name``
    attribute is set to ``DESCRIPTION`` and ``role`` attribute to ``description``.

    Values recognized by PROJ for this Item are currently:

    + ``longitude_offset``: valid for TYPE=HORIZONTAL_OFFSET. Sample values should be
      the value to add a longitude expressed in the CRS encoded in the GeoKeys
      to obtain a longitude value expressed in the target CRS.

    + ``latitude_offset``: valid for TYPE=HORIZONTAL_OFFSET. Sample values should be
      the value to add a latitude expressed in the CRS encoded in the GeoKeys
      to obtain a latitude value expressed in the target CRS.

    + ``geoid_undulation``: valid for TYPE=VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL.
      For a source CRS being a geographic CRS and a target CRS being a vertical CRS,
      sample values should be the value to add to a geoid-related height (that
      is expressed in the target CRS) to
      get an ellipsoidal height (that is expressed in the source CRS), also
      called the geoid undulation.
      Note the possible confusion related to what is the source CRS and target CRS and
      the semantics of the value stored (to convert from the source to the target,
      one must subtract the value contained in the grid). This is the convention
      used by the `EPSG:9665 <https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9665>`_
      operation method.

    + ``vertical_offset``: valid for TYPE=VERTICAL_OFFSET_VERTICAL_TO_VERTICAL.
      For a source and target CRS being vertical CRS,
      sample values should be the value to add to an elevation expressed in the
      source CRS to obtain a longitude value expressed in the target CRS.

    For example:

    .. code-block:: xml

        <Item name="DESCRIPTION" sample="0" role="description">longitude_offset</Item>
        <Item name="DESCRIPTION" sample="1" role="description">latitude_offset</Item>

  * The unit of the values stored in the grid must be specified for each
    sample through an Item of name ``UNITTYPE`` and role ``unittype``
    Valid values should be the name of entries from the EPSG ``unitofmeasure``
    table. To maximize interoperability, writers are strongly encouraged to
    limit themselves to the following values:

    For linear units:

    - ``metre`` (default value assumed if absent for vertical shift grid files, and value for files stored on PROJ CDN)
    - ``US survey foot``

    For angular units:

    - ``degree``
    - ``arc-second`` (default value assumed if absent for vertical shift grid files, and value for files stored on PROJ CDN)

    The longitude and latitude offset samples should use the same unit.

    For future use case that involve time-derivative quantities, the following
    units may be used:

    - ``metres per year``
    - ``arc-seconds per year``

    Example:

    .. code-block:: xml

        <Item name="UNITTYPE" sample="0" role="unittype">arc-second</Item>
        <Item name="UNITTYPE" sample="1" role="unittype">arc-second</Item>

  * The ``target_crs_epsg_code`` metadata item should be present.
    For a horizontal shift grid, this is the EPSG
    code of the target geographic CRS. For a vertical shift grid, this is the
    EPSG code of a the target vertical CRS.
    If the target CRS has no associated EPSG code, ``target_crs_wkt`` must be
    used.
    Ignored by PROJ currently.

  * The ``target_crs_wkt`` metadata item must be present if the
    ``target_crs_epsg_code`` cannot be used.
    Its value should be a valid WKT string according to
    `WKT:2015 <http://docs.opengeospatial.org/is/12-063r5/12-063r5.html>`_
    or `WKT:2019 <hhttp://docs.opengeospatial.org/is/18-010r7/18-010r7.html>`_
    Ignored by PROJ currently.

  * The ``source_crs_epsg_code`` metadata item must be present if the source
    and interpolation CRS are not the same (typical use case is vertical CRS to vertical CRS
    transformation), because the GeoKeys encode the interpolation CRS and not the source CRS.
    If the source CRS has no associated EPSG code, ``source_crs_wkt`` must be
    used.
    Ignored by PROJ currently.

  * The ``source_crs_wkt`` metadata item must be present if the
    ``source_crs_epsg_code`` cannot be used.
    Its value should be a valid WKT string according to WKT:2015 or WKT:2019.
    Ignored by PROJ currently.

  * The ``interpolation_crs_wkt`` metadata item may be present if the GeoKeys
    cannot be used to express reliably the interpolation CRS.
    Its value should be a valid WKT string according to WKT:2015 or WKT:2019.
    Ignored by PROJ currently.

  * The ``recommended_interpolation_method`` metadata item may be present to
    describe the method to use to interpolation values at locations not
    coincident with nodes stored in the grid file. Potential values: ``bilinear``,
    ``bicubic``.
    Ignored by PROJ currently.

  * The ``grid_name`` metadata item should be present if there are
    subgrids for this grid (that is grids whose extent is contained in the extent
    of this grid), or if this is a subgrid.
    It is intended to be a relatively short identifier
    Will be ignored by PROJ (this information can be inferred by the grids extent)

  * The ``parent_grid_name`` metadata item should be present if this is a
    subgrid and its value should be equal to the paren's ``grid_name``
    Will be ignored by PROJ (this information can be inferred by the grids extent)

  * The ``number_of_nested_grids`` metadata item should be present if there are
    subgrids for this grid (that is grids whose extent is contained in the extent
    of this grid).
    Will be ignored by PROJ (this information can be inferred by the grids extent)

Multi-grid storage
++++++++++++++++++

Formats like NTv2 can contain multiple subgrids. This can be transposed to
TIFF by using several IFD chained together with the last 4 bytes (or 8 bytes
for BigTIFF) of an IFD pointing to the offset of the next one.

The first IFD should have a full description according to the
:ref:`Description of the PROJ GeoTIFF format <description_geotiff_format>`.
Subsequent IFD might have a more compact description, omitting for example, CRS
information if it is identical to the main IFD (which should be the case for
the currently envisionned use cases).

Each IFD will have its
`NewSubfileType <https://www.awaresystems.be/imaging/tiff/tifftags/newsubfiletype.html>`_
tag set to 0.

If a low-resolution grid is available, it should be put before subgrids of
higher-resolution in the chain of IFD linking. On reading, PROJ will use the
value from the highest-resoluted grid that contains the point of interest.

For efficient reading from the network, files hosted on the CDN will use
a layout as described in the `low level paragraph of the Cloud Optimized GeoTIFF
GDAL driver page <https://gdal.org/drivers/raster/cog.html#low-level>`_

.. note::

    TIFF has another mechanism to link IFDs, the SubIFD tag. This potentially
    enables to define a hiearchy of IFDs (similar to HDF5 groups). There is no
    support for that in most TIFF-using software, notably GDAL, and no compelling
    need to have a nested hiearchy, so "flat" organization with the standard IFD chaining
    mechanism is adopted.

Tooling
+++++++

The `GDAL COG driver <https://gdal.org/drivers/raster/cog.htm>`_ will be extended
to accept a list of individual grids to combine together in a single fil.

A ntv2_to_cog.py convenience script will be created in the samples script
directory of GDAL to convert NTv2 grids, including their subgrids, to the above
described GeoTIFF layout.

A validate_proj_gtiff_grid.py script will be created in the samples script
directory of GDAL to check that a file meets the above described requirements
and recommendations.

Build requirements
++++++++++++++++++

The minimum libtiff version will be 4.0 (RHEL 7 ships with libtiff 4.0.3).
To be able to read grids stored on the CDN, libtiff will need to build against
zlib to have DEFLATE and LZW suport, which is met by all known binary distributions
of libtiff.

Dropping grid catalog functionality
-------------------------------------------------------------------------------

While digging through existing code, I more or less discovered that the PROJ
code base has the concept of a grid catalog. This is a feature apparently triggered by
using the +catalog=somefilename.csv in a PROJ string, where the CSV file list
grid names, their extent, priority and date. It seems to be an alternative to using
+nadgrids with multiple grids, with the extra ability to interpolate shift values between
several grids if a +date parameter is provided and the grid catalog mentions a
date for each grids.
It was added in June 2012 per `commit fcb186942ec8532655ff6cf4cc990e5da669a3bc
<https://github.com/OSGeo/PROJ/commit/fcb186942ec8532655ff6cf4cc990e5da669a3bc>`_

This feature is likely unknown to most users as there is no known documentation for
it (neither in current documentation, nor in `historic one <https://web.archive.org/web/20160601000000*/http://trac.osgeo.org/proj/wiki/GenParms>`_).
It is not either tested by PROJ tests, so its working status is unknown. It would
likely make implementation of this RFC easier if this was removed. This would result in
completely dropping the gridcatalog.cpp and gc_reader.cpp files, their call sites
and the catalog_name and datum_date parameter from the PJ structure.

In case similar functionality would be be needed, it might be later reintroduced
as an extra mode of :ref:`hgridshift`, or using a dedicated transformation method,
similarly to the :ref:`deformation` one,
and possibly combining the several grids to interpolate among in the same file,
with a date metadata item.

Backward compatibility issues
-------------------------------------------------------------------------------

None anticipated, except the removal of the (presumably little used) grid catalog
functionality.

Potential future related work
-----------------------------

The foundations set in the definition of the GeoTIFF grid format should hopefully
be reused to extend them to support deformation models (was initially discussed
per https://github.com/OSGeo/PROJ/issues/1001).

Definition of such an extension is out of scope of this RFC.

Documentation
-------------------------------------------------------------------------------

- New API function will be documented.
- A dedicated documentation page will be created to explain the working of
  network-based access.
- A dedicated documentation page will be created to describe the GeoTIFF based
  grid format. Mostly reusing above material.

Testing
-------------------------------------------------------------------------------

The Python SimpleHTTPServer module will be used for unit tests to run a local
HTTP server.

Discussion points
-------------------------------------------------------------------------------

- Should libtiff be a required or optional dependency of PROJ ?

  The advantage of making it required is that proj-datumgrid could only ship TIFF
  grids. If we don't make it a requirement, we will have to manage .gtx / .gsb
  for the .zip / .tar.gz delivered in the proj-datumgrid-XXXXX packages, and
  a .tif version hosted on the CDN.


Adoption status
-------------------------------------------------------------------------------

TBD

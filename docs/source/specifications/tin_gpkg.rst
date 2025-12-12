.. _tin_gpkg:

================================================================================
Triangulated irregular network (TIN) GeoPackage format
================================================================================

Introduction
------------

The TIN GeoPackage format defines the format for a triangulation model stored in a
`GeoPackage <https://geopackage.org>`__ file, that defines a horizontal and/or
vertical transformation between two CRS.

That file format may be used by the :ref:`tinshift <tinshift>` operation since
PROJ 9.8

Note: this format has similar capabilities than the :ref:`TIN JSON <tin_json>` file format,
but can handle arbitrarily large triangulations.

The `tin_json_to_gpkg_tin.py <https://github.com/OSGeo/PROJ-data/blob/master/grid_tools/tin_json_to_gpkg_tin.py>`__
Python script can be used to convert a TIN JSON file into a TIN GeoPackage file.

Specification
-------------

The file MUST be a GeoPackage v1.2, v1.3 or v1.4 file.

``vertices`` table
++++++++++++++++++

The file MUST contain a ``vertices`` GeoPackage table, of geometry type ``POINT``,
registered in the ``gpkg_contents`` table with ``data_type`` set to ``features``.

The ``x_min``, ``y_min``, ``x_max`` and ``y_max`` columns record in the
record of ``gpkg_contents`` MUST contain the bounding box of all vertices.

The CRS referenced by the table MUST be the source CRS of the transformation.

The ``vertices`` table MUST have at least the following columns:

- ``fid``: the integer primary key column.

- ``geom``: the GeoPackage POINT geometry column. There are constraints on the
  format of the GeoPackage BLOB. Both the GeoPackage header and the WKB MUST use Intel
  byte order. The GeoPackage header MUST NOT have an envelope.
  The point encodes the coordinates in the source CRS (equivalent of the
  ``source_x`` and ``source_y`` columns in the TIN JSON format. Such columns
  may also be added to the ``vertices`` table, but they are not used by the
  PROJ implementation)

If the TIN defines a horizontal shift, the following columns MUST be present:
  
- ``target_x``: target X coordinate, in a column of type REAL, if the TIN defines a horizontal shift.

- ``target_y``: target Y coordinate, in a column of type REAL, if the TIN defines a horizontal shift.

If the TIN defines a vertical shift, the following columns MUST be present:

- ``source_z`` and ``target_z``: source and target Z coordinate, in columns of type REAL

- (or) ``offset_z``: value of ``target_z`` - ``source_z``, in a column of type REAL

Additional columns may be present, but are not used by PROJ.

The ``vertices`` table may have a RTree spatial index, but this is not used
by PROJ.


``triangles_def`` table
+++++++++++++++++++++++

The file MUST contain a ``triangles_def`` GeoPackage table.

Its ``data_type`` registered in the ``gpkg_contents`` may be ``attributes``,
or ``features``. This is not used by PROJ.

The ``triangles_def`` table MUST have at least the following columns:

- ``fid``: the integer primary key column.

- ``idx_vertex1``: foreign key to the ``fid`` column of ``vertices``, for the 1st vertex of the triangle.

- ``idx_vertex2``: foreign key to the ``fid`` column of ``vertices``, for the 2nd vertex of the triangle.

- ``idx_vertex3``: foreign key to the ``fid`` column of ``vertices``, for the 3rd vertex of the triangle.

Additional columns may be present, but are not used by PROJ.


``gpkg_metadata`` and ``gpkg_metadata_reference`` tables
++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The file MUST contain the ``gpkg_metadata`` and ``gpkg_metadata_reference`` tables,
as defined by the `GeoPackage metadata <https://www.geopackage.org/spec/#extension_metadata>`__
extension.

The ``gpkg_metadata`` table MUST contain the following record:

- ``id`` = 1

- ``md_scope`` = ``dataset``

- ``md_uri`` = ``https://proj.org``

- ``mime_type`` = ``application/json``

- ``metadata``: serialized JSON content of the metadata described below.

The ``gpkg_metadata_reference`` table MUST contain the following record:

- ``reference_scope`` = ``geopackage``

- ``table_name`` = NULL

- ``column_name`` = NULL

- ``timestamp``: any ISO-8601 valid value

- ``row_id_value`` = NULL

- ``md_file_id`` = 1

- ``md_parent_id`` = NULL


JSON content in the ``gpkg_metadata`` table
+++++++++++++++++++++++++++++++++++++++++++

Required members
~~~~~~~~~~~~~~~~

The content of the record of id 1 in the ``gpkg_metadata`` table MUST be a
serialized JSON object, with the following required members:

- ``file_type`` = ``triangulation_file``

- ``format_version`` = ``1.0`` or ``1.1``

- ``transformed_components``: an array which may contain one or two strings:
  ``horizontal`` when horizontal components of the coordinates are transformed
  and/or ``vertical`` when the vertical component is transformed.

Optional members used by PROJ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``fallback_strategy``

  String identifying how to treat points that do not fall into any of the
  specified triangles. When this item is set, ``format_version`` must be set to >= 1.1.
  Possible values are ``none``, ``nearest_side`` and ``nearest_centroid``. The
  default is ``none`` and signifies, that points that fall outside the
  specified triangles are not transformed. This is also the behavior for
  ``format_version`` before 1.1. If ``fallback_strategy`` is set to
  ``nearest_side``, then points that do not fall into any triangle are 
  transformed according to the triangle closest to them by euclidean distance.
  If ``fallback_strategy`` is set to ``nearest_centroid``, then points that do
  not fall into any triangle are transformed according to the triangle with the
  closest centroid (intersection of its medians).

Conditional members
~~~~~~~~~~~~~~~~~~~

When ``transformed_components`` contains the ``horizontal`` string, the JSON
metadata MUST contain the additional members:

- ``min_shift_x``: minimum value of ``target_x`` - ``source_x`` over all vertices of the ``vertices`` table.

- ``max_shift_x``: maximum value of ``target_x`` - ``source_x`` over all vertices of the ``vertices`` table.

- ``min_shift_y``: minimum value of ``target_y`` - ``source_y`` over all vertices of the ``vertices`` table.

- ``max_shift_y``: maximum value of ``target_y`` - ``source_y`` over all vertices of the ``vertices`` table.

Note: those fields are used when performing inverse transformations


If the above described ``fallback_strategy`` field is set to ``nearest_side`` or ``nearest_centroid``,
a ``num_vertices`` member MUST be set with the number of vertices of the ``vertices`` table.

Note: this is used to compute the initial search radius in the inverse transformation for those modes.

Other optional members
~~~~~~~~~~~~~~~~~~~~~~

All other members defined in the :ref:`TIN JSON <tin_json>` file format (except ``vertices``, ``vertices_columns``,
``triangles`` and ``triangles_columns`` which are redundant with the GeoPackage ``vertices`` and ``triangles_def``
tables) may be set.

``rtree_triangles_geom`` RTree virtual table
++++++++++++++++++++++++++++++++++++++++++++

A RTree virtual table MUST be created as following:

.. code-block:: sql

    CREATE VIRTUAL TABLE rtree_triangles_geom USING rtree(id, minx, maxx, miny, maxy)


It MUST be filled with as many records as there are triangles in the ``triangles_def``
table, with the ``id`` column of ``rtree_triangles_geom`` containing the
corresponding value of the ``fid`` column of ``triangles_def``, and the
``minx``, ``maxx``, ``miny``, ``maxy`` columns containing the bounding box of
each triangle.

Optional ``triangles`` spatial view
+++++++++++++++++++++++++++++++++++

The file MAY contain an optional ``triangles`` spatial view, extending the
``triangles_def`` table with the GeoPackage geometry blob of each triangle.

Such spatial view can be used to display the triangles in a GIS software that
supports GeoPackage (and spatial views), such as QGIS.

The `tin_json_to_gpkg_tin.py <https://github.com/OSGeo/PROJ-data/blob/master/grid_tools/tin_json_to_gpkg_tin.py>`__
script creates such view with:

.. code-block:: python

    srs_id_i32le = as_i32le_hex(srs_id)
    wkb_polygon_i32le = as_i32le_hex(3)
    number_rings_i32le = as_i32le_hex(1)
    number_vertices_i32le = as_i32le_hex(4)
    triangle_gpkg_prefix = f"47500001{srs_id_i32le}01{wkb_polygon_i32le}{number_rings_i32le}{number_vertices_i32le}"

    # other_fields is typically ",v1.target_x, v1.target_y, v2.target_x, v2.target_y, v3.target_x, v3.target_y" for a horizontal transformation

    # 14 = GPKG_header_size_without_envelope (8) + WKB point header (5) + base_one_index (1)
    ds.ExecuteSQL(
        f"CREATE VIEW triangles AS SELECT triangles_def.fid AS OGC_FID {other_fields}, CAST(X'{triangle_gpkg_prefix}' || substr(v1.geom, 14) || substr(v2.geom, 14) || substr(v3.geom, 14) || substr(v1.geom, 14) AS BLOB) AS geom FROM triangles_def LEFT JOIN vertices v1 ON idx_vertex1 = v1.fid LEFT JOIN vertices v2 ON idx_vertex2 = v2.fid LEFT JOIN vertices v3 ON idx_vertex3 = v3.fid"
    )
    ds.ExecuteSQL(
        f"INSERT INTO gpkg_contents (table_name, identifier, data_type, srs_id, min_x, min_y, max_x, max_y) VALUES ('triangles', 'triangles', 'features', {srs_id}, {min_x}, {min_y}, {max_x}, {max_y})"
    )
    ds.ExecuteSQL(
        f"INSERT INTO gpkg_geometry_columns (table_name, column_name, geometry_type_name, srs_id, z, m) values ('triangles', 'geom', 'POLYGON', {srs_id}, 0, 0)"
    )


.. spelling:word-list::

    RTree

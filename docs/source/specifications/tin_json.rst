.. _tin_json:

================================================================================
Triangulated irregular network (TIN) JSON format
================================================================================

The TIN JSON format defines the format for a triangulation model stored in a JSON
file.

That file format may be used by the :ref:`tinshift <tinshift>` operation since
PROJ 7.2

Note: the :ref:`TIN GeoPackage <tin_gpkg>` file format can also be used since
PROJ 9.8. It can handle arbitrarily large triangulations.

Below a minimal example, from the KKJ to ETRS89 transformation, with just a
single triangle:

.. literalinclude:: ../../../data/tests/tinshift_crs_implicit.json
  :language: json


So after the generic metadata, we define the input and output CRS (informative
only), and that the transformation affects horizontal components of
coordinates. We name the columns of the ``vertices`` and ``triangles`` arrays.
We defined the source and target coordinates of each vertex, and define a
triangle by referring to the index of its vertices in the ``vertices`` array.

More formally, the specific items for the triangulation file are:

input_crs
  String identifying the CRS of source coordinates
  in the vertices. Typically ``EPSG:XXXX``. If the transformation is for vertical
  component, this should be the code for a compound CRS (can be EPSG:XXXX+YYYY
  where XXXX is the code of the horizontal CRS and YYYY the code of the vertical CRS).
  For example, for the KKJ->ETRS89 transformation, this is EPSG:2393
  (``KKJ / Finland Uniform Coordinate System``). The input coordinates are assumed
  to be passed in the "normalized for visualisation" / "GIS friendly" order,
  that is longitude, latitude for geographic coordinates and
  easting, northing for projected coordinates.


output_crs
  String identifying the CRS of target coordinates in the vertices.
  Typically ``EPSG:XXXX``. If the transformation is for vertical component,
  this should be the code for a compound CRS (can be EPSG:XXXX+YYYY where
  XXXX is the code of the horizontal CRS and YYYY the code of the vertical CRS).
  For example, for the KKJ->ETRS89 transformation, this is EPSG:3067
  (\"ETRS89 / TM35FIN(E,N)\"). The output coordinates will be returned in
  the "normalized for visualisation" / "GIS friendly" order,
  that is longitude, latitude for geographic coordinates and
  easting, northing for projected coordinates.


transformed_components
  Array which may contain one or two strings: "horizontal" when horizontal
  components of the coordinates are transformed and/or "vertical" when the
  vertical component is transformed.


fallback_strategy
  String identifying how to treat points that do not fall into any of the
  specified triangles. This item is available for ``format_version`` >= 1.1.
  Possible values are ``none``, ``nearest_side`` and ``nearest_centroid``. The
  default is ``none`` and signifies, that points that fall outside the
  specified triangles are not transformed. This is also the behavior for
  ``format_version`` before 1.1. If ``fallback_strategy`` is set to
  ``nearest_side``, then points that do not fall into any triangle are 
  transformed according to the triangle closest to them by euclidean distance.
  If ``fallback_strategy`` is set to ``nearest_centroid``, then points that do
  not fall into any triangle are transformed according to the triangle with the
  closest centroid (intersection of its medians).


vertices_columns
  Specify the name of the columns of the rows in the ``vertices``
  array. There must be exactly as many elements in ``vertices_columns`` as in a
  row of ``vertices``. The following names have a special meaning: ``source_x``,
  ``source_y``, ``target_x``, ``target_y``, ``source_z``, ``target_z`` and
  ``offset_z``.  ``source_x`` and ``source_y`` are compulsory.
  ``source_x`` is for the source longitude (in degree) or easting.
  ``source_y`` is for the source latitude (in degree) or northing. 
  ``target_x`` and ``target_y`` are compulsory when ``horizontal`` is specified
  in ``transformed_components``. (``source_z`` and ``target_z``) or
  ``offset_z`` are compulsory when ``vertical`` is specified in ``transformed_components``


triangles_columns
  Specify the name of the columns of the rows in the
  ``triangles`` array. There must be exactly as many elements in ``triangles_columns``
  as in a row of ``triangles``. The following names have a special meaning:
  ``idx_vertex1``, ``idx_vertex2``, ``idx_vertex3``. They are compulsory.


vertices
  An array whose items are themselves arrays with as many columns as
  described in ``vertices_columns``.


triangles
  An array whose items are themselves arrays with as many columns as
  described in ``triangles_columns``.
  The value of the ``idx_vertexN`` columns must be indices
  (between 0 and len(``vertices``-1) of items of the ``vertices`` array.

A `JSON schema <https://proj.org/schemas/triangulation.schema.json>`_ is available
for this file format.

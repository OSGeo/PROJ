.. _projjson:

================================================================================
PROJJSON
================================================================================

Introduction
------------

PROJJSON is a JSON encoding of
`WKT2:2019 / ISO-19162:2019: Geographic information - Well-known text representation of coordinate reference systems <http://docs.opengeospatial.org/is/18-010r7/18-010r7.html>`_,
which itself implements the model of :cite:`ISO19111`.
Apart from the difference of encodings, the semantics is intended to be exactly
the same as WKT2:2019, and PROJJSON can be morphed losslessly from/into WKT2:2019.

PROJJSON is aimed at encoding definitions of coordinate reference systems (and their composing objects: datums, datum ensembles, coordinate systems, conversion) and coordinate operations.

Normative references
--------------------

The PROJJSON specification requires prior knowledge of the following normative
specifications:

- `The JavaScript Object Notation (JSON) Data Interchange Format / IETF RFC 7159 <https://datatracker.ietf.org/doc/html/rfc7159>`_
- `WKT2:2019 / ISO-19162:2019: Geographic information - Well-known text representation of coordinate reference systems  <http://docs.opengeospatial.org/is/18-010r7/18-010r7.html>`_

Definitions
-----------

- JavaScript Object Notation (JSON), and the terms object, member,
  name, value, array, number, true, false, and null, are to be
  interpreted as defined in [RFC7159].

- integer: JSON number whose value has no fractional/exponent part.

- All `term and definitions <http://docs.opengeospatial.org/is/18-010r7/18-010r7.html#4>`_
  from WKT2:2019 apply.

Schema
------

A JSON schema of PROJJSON grammar is available at
https://proj.org/schemas/v0.6/projjson.schema.json

This schema defines a minimum set of constraints that apply to well-formed PROJJSON.
Number of specific CRS and coordinate operation domain constraints are not expressed
as JSON schema constraints: unless otherwise stated, the constraints (optional/mandatory/conditional
character of information, restricted set of allowed values, etc.) defined
in the WKT2:2019 specification also apply, as supplement to the JSON schema constraints.

History of the schema
---------------------

* v0.6:
     - Implemented in PROJ 9.2
     - Additional optional "source_crs" property in "abridged_transformation".
     - Added CoordinateMetadata
     - Added "datum_epoch" property to GeodeticReferenceFrame and VerticalReferenceFrame
     - Added "minimum_value", "maximum_value" and "range_meaning" properties to Axis
* v0.5:
    - Implemented in PROJ 9.1:
        + add "meridian" member in Axis object type.
        + add "temporal_extent" and "vertical_extent" members in object usage.
        + add "geoid_models" for VerticalCRS if several models are needed.
        + add "deformation_models" members to GeodeticCRS and VerticalCRS (replaces "deformation_model" in DynamicGeodeticReferenceFrame/DynamicVerticalReferenceFrame)
    - Added in schema, but not implemented in PROJ:
        + add top-level object PointMotionOperation
* v0.4: additional properties allowed in id object (version, authority_citation, uri). Implemented in PROJ 8.2.
* v0.3: additional properties allowed in BoundCRS object (name, scope, area, bbox, usages, remarks, id, ids). Implemented in PROJ 8.1.
* v0.2: addition of geoid_model in VerticalCRS object.
* v0.1: initial version for PROJ 6.2

Specification 
-------------

A PROJJSON text is a JSON object which has, at a minimum, a required ``type`` member,
whose value is a string describing the nature of the encoded geodetic object.

An optional ``$schema`` member may be present, with its value being a string with
a URL that points to the JSON schema that applies.

Objects may be composed of sub-objects (e.g a GeographicCRS is made of a Datum or DatumEnsemble
and a coordinate system). The ``type`` member of the sub-objects can be omitted when
there is no ambiguity. For example, in the object which is the value of a ``coordinate_system``
member, the ``type`` may be omitted. However, the value of the ``datum`` object of
a GeographicCRS the ``type`` should be specified, as it can be either a GeodeticReferenceFrame
or a DynamicGeodeticReferenceFrame.
More formally, the ``type`` should be specified if the JSON schema specifies alternative
types for the value of a member using the oneOf construct and those alternative
types have a ``type`` member. Otherwise it may be omitted.

High level objects
++++++++++++++++++

Objects described at the first level of a PROJJSON text have the following
potential values of the ``type`` member:

* Coordinate Reference Systems (CRS):

  - Common ones:

    + ``GeographicCRS``
    + ``GeodeticCRS``
    + ``ProjectedCRS``
    + ``CompoundCRS``
    + ``BoundCRS``

  - More esoteric ones:

    + ``VerticalCRS``
    + ``EngineeringCRS``
    + ``TemporalCRS``
    + ``ParametricCRS``
    + ``DerivedGeographicCRS``
    + ``DerivedGeodeticCRS``
    + ``DerivedProjectedCRS``
    + ``DerivedVerticalCRS``
    + ``DerivedEngineeringCRS``
    + ``DerivedTemporalCRS``
    + ``DerivedParametricCRS``

* Coordinate operations:

  - ``Transformation``
  - ``Conversion``
  - ``ConcatenatedOperation``

* Others:

  - ``PrimeMeridian``
  - ``Ellipsoid``
  - ``Datum``
  - ``DatumEnsemble``

.. _identifiers:

Identifiers
+++++++++++

All objects mentioned above can have an optional ``id`` or ``ids`` member.

The value of ``id`` is a JSON object with the following members:

- ``authority``: (required) value of type string. e.g "EPSG", "OGC", "IGNF", etc.
- ``code``: (required) value of type string or integer. e.g 4326 or "CRS84"
- ``authority_citation``: (optional) value of type string that may be used to give
  further details of the authority.
- ``uri``: (optional) value of type string that may be used to give reference
  to an online resource.

An object can sometimes be identified in different ways, in which case the ``ids``
member can be used to specify a JSON array of objects with the same type of ``id``.

Identifiers are allowed in top-level objects and inner objects. The WKT2:2019
specification recommends that if an object has an identifier, its inner objects
should omit their identifiers, with the exceptions mentioned at
http://docs.opengeospatial.org/is/18-010r7/18-010r7.html#37.

Object usages
+++++++++++++

CRS and coordinate operation objects are derived classes (in object modeling terminology)
of a "object usage" class. An object usage has the following optional members:

- ``scope``: (optional) value of type string describing the purpose or purposes of
  the object. e.g "Geodesy, topographic mapping and cadastre"
- ``area``: (optional) value of type string which describes a geographic area
  over which a CRS or coordinate operation is applicable. e.g. "World"
- ``bbox``: (optional) value of type object, with 4 required members:
  * ``east_longitude``: (required) number expressing the longitude in degrees of the eastern most part of the extent, within [-180,180] range.
  * ``west_longitude``: (required) number expressing the longitude in degrees of the western most part of the extent, within [-180,180] range.. For an extent crossing the anti-meridian, west_longitude is lower than east_longitude.
  * ``south_latitude``: (required) number expressing the latitude in degrees of the southern most part of the extent, within [-90,90] range.
  * ``north_latitude``: (required) number expressing the latitude in degrees of the northern most part of the extent, within [-90,90] range.
  The coordinates are expressed in a unspecified datum, with the longitudes
  relative to the international reference meridian.
- ``remarks``: (optional) value of type string with an informative text that does
  not modify the defining parameters of the object. e.g "Use NTv2 file for better accuracy"
- ``id`` (mutually exclusive with ``ids``): (optional) Identifier of the object, as defined in :ref:`identifiers`
- ``ids`` (mutually exclusive with ``id``): (optional) Identifiers of the object, as defined in :ref:`identifiers`

If several extents and scopes apply to an object, the ``scope``, ``area`` and ``bbox``
members should not be used. Instead a ``usages`` member should be used, whose value
is an array of objects, each of them accepting ``scope`` and/or ``area`` and/or ``bbox``
as members. While it is acceptable to use the ``usages`` construct for a single
usage, it is recommended to avoid it and rather use instead the
``scope``, ``area`` and ``bbox`` members.

Units
+++++

A unit may be described either as:

- an object with the following members:

    * ``type``: (required) one of the following types:
      ``LinearUnit``, ``AngularUnit``, ``ScaleUnit``, ``TimeUnit``, ``ParametricUnit``, ``Unit``
    * ``name``: (required) string.
    * ``conversion_factor``: (required in most cases, except in the temporal quantities of http://docs.opengeospatial.org/is/18-010r7/18-010r7.html#42)
      number that expresses a multiplicative factor to convert from the specified
      unit to a reference unit, as specified in http://docs.opengeospatial.org/is/18-010r7/18-010r7.html#41
    * ``id`` or ``ids``: (optional, mutually exclusive)

- a string among the following enumeration: ``metre``, ``degree``, ``unity``

Using a string value, when applicable, is recommended for brevity of
the object definition.

Omitted units in measured parameters
++++++++++++++++++++++++++++++++++++

Most numeric parameters should generally be accompanied with the corresponding unit.

For example, for a projection parameter:

.. code-block:: json

    {
        "name": "False easting",
        "value": 500000,
        "unit": "metre"
    }

or:

.. code-block:: json

    {
        "name": "False easting",
        "value": 700000,
        "unit": {
          "type": "LinearUnit",
          "name": "foot",
          "conversion_factor": 0.3048
        }
    }

For the following cases, the unit may be omitted if it is metre:
``semi_major_axis``, ``semi_minor_axis`` and ``radius`` members of an ellipsoid

For the following cases, the unit may be omitted if it is degree:
``longitude`` of a prime meridian.

Coordinate system
+++++++++++++++++

In WKT, a `ORDER <http://docs.opengeospatial.org/is/18-010r7/18-010r7.html#49>`_
keyword may be present in an axis definition. As the value of that element
is equal to the index of axis in the list of axis of the coordinate system
(with 1 as the value of the first index), it is absent from the PROJJSON encoding,
to avoid any risk of misuse.

Examples
--------

GeographicCRS
+++++++++++++

Using a datum member, implicit prime meridian
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The EPSG:6318 / "NAD83(2011)" geographic CRS can be expressed as

.. code-block:: json

    {
      "$schema": "https://proj.org/schemas/v0.4/projjson.schema.json",
      "type": "GeographicCRS",
      "name": "NAD83(2011)",
      "datum": {
        "type": "GeodeticReferenceFrame",
        "name": "NAD83 (National Spatial Reference System 2011)",
        "ellipsoid": {
          "name": "GRS 1980",
          "semi_major_axis": 6378137,
          "inverse_flattening": 298.257222101
        }
      },
      "coordinate_system": {
        "subtype": "ellipsoidal",
        "axis": [
          {
            "name": "Geodetic latitude",
            "abbreviation": "Lat",
            "direction": "north",
            "unit": "degree"
          },
          {
            "name": "Geodetic longitude",
            "abbreviation": "Lon",
            "direction": "east",
            "unit": "degree"
          }
        ]
      },
      "scope": "Horizontal component of 3D system.",
      "area": "Puerto Rico - onshore and offshore. United States (USA) onshore and offshore - Alabama; Alaska; Arizona; Arkansas; California; Colorado; Connecticut; Delaware; Florida; Georgia; Idaho; Illinois; Indiana; Iowa; Kansas; Kentucky; Louisiana; Maine; Maryland; Massachusetts; Michigan; Minnesota; Mississippi; Missouri; Montana; Nebraska; Nevada; New Hampshire; New Jersey; New Mexico; New York; North Carolina; North Dakota; Ohio; Oklahoma; Oregon; Pennsylvania; Rhode Island; South Carolina; South Dakota; Tennessee; Texas; Utah; Vermont; Virginia; Washington; West Virginia; Wisconsin; Wyoming. US Virgin Islands - onshore and offshore.",
      "bbox": {
        "south_latitude": 14.92,
        "west_longitude": 167.65,
        "north_latitude": 74.71,
        "east_longitude": -63.88
      },
      "id": {
        "authority": "EPSG",
        "code": 6318
      }
    }


Note the omission of a prime meridian member, which is conformant with the
WKT2:2019 conditionality rules, as the prime meridian of the WGS 84 datum is the
reference meridian / Greenwich.

Using a datum member with an explicit prime meridian
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For the EPSG:4806 / "Monte Mario (Rome)" geographic CRS, the prime meridian must
be specified:

.. code-block:: json

    {
      "$schema": "https://proj.org/schemas/v0.4/projjson.schema.json",
      "type": "GeographicCRS",
      "name": "Monte Mario (Rome)",
      "datum": {
        "type": "GeodeticReferenceFrame",
        "name": "Monte Mario (Rome)",
        "ellipsoid": {
          "name": "International 1924",
          "semi_major_axis": 6378388,
          "inverse_flattening": 297
        },
        "prime_meridian": {
          "name": "Rome",
          "longitude": 12.4523333333333
        }
      },
      "coordinate_system": {
        "subtype": "ellipsoidal",
        "axis": [
          {
            "name": "Geodetic latitude",
            "abbreviation": "Lat",
            "direction": "north",
            "unit": "degree"
          },
          {
            "name": "Geodetic longitude",
            "abbreviation": "Lon",
            "direction": "east",
            "unit": "degree"
          }
        ]
      },
      "scope": "Geodesy, onshore minerals management.",
      "area": "Italy - onshore and offshore; San Marino, Vatican City State.",
      "bbox": {
        "south_latitude": 34.76,
        "west_longitude": 5.93,
        "north_latitude": 47.1,
        "east_longitude": 18.99
      },
      "id": {
        "authority": "EPSG",
        "code": 4806
      }
    }

Using a datum ensemble member
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The WGS 84 geographic CRS may also be specified using a datum ensemble
representation of the WGS 84 datum:

.. code-block:: json

    {
      "$schema": "https://proj.org/schemas/v0.4/projjson.schema.json",
      "type": "GeographicCRS",
      "name": "WGS 84",
      "datum_ensemble": {
        "name": "World Geodetic System 1984 ensemble",
        "members": [
          {
            "name": "World Geodetic System 1984 (Transit)",
            "id": {
              "authority": "EPSG",
              "code": 1166
            }
          },
          {
            "name": "World Geodetic System 1984 (G730)",
            "id": {
              "authority": "EPSG",
              "code": 1152
            }
          },
          {
            "name": "World Geodetic System 1984 (G873)",
            "id": {
              "authority": "EPSG",
              "code": 1153
            }
          },
          {
            "name": "World Geodetic System 1984 (G1150)",
            "id": {
              "authority": "EPSG",
              "code": 1154
            }
          },
          {
            "name": "World Geodetic System 1984 (G1674)",
            "id": {
              "authority": "EPSG",
              "code": 1155
            }
          },
          {
            "name": "World Geodetic System 1984 (G1762)",
            "id": {
              "authority": "EPSG",
              "code": 1156
            }
          },
          {
            "name": "World Geodetic System 1984 (G2139)",
            "id": {
              "authority": "EPSG",
              "code": 1309
            }
          }
        ],
        "ellipsoid": {
          "name": "WGS 84",
          "semi_major_axis": 6378137,
          "inverse_flattening": 298.257223563
        },
        "accuracy": "2.0",
        "id": {
          "authority": "EPSG",
          "code": 6326
        }
      },
      "coordinate_system": {
        "subtype": "ellipsoidal",
        "axis": [
          {
            "name": "Geodetic latitude",
            "abbreviation": "Lat",
            "direction": "north",
            "unit": "degree"
          },
          {
            "name": "Geodetic longitude",
            "abbreviation": "Lon",
            "direction": "east",
            "unit": "degree"
          }
        ]
      },
      "scope": "Horizontal component of 3D system.",
      "area": "World.",
      "bbox": {
        "south_latitude": -90,
        "west_longitude": -180,
        "north_latitude": 90,
        "east_longitude": 180
      },
      "id": {
        "authority": "EPSG",
        "code": 4326
      }
    }

The above is the output of the following invocation of the projinfo utility
of the PROJ software version 9.0.0

::

    projinfo EPSG:4326 -o PROJJSON -q

.. note::

    PROJ versions prior to PROJ 8.0.0 used versions of the EPSG dataset that
    did not have the datum ensemble concept. Consequently they used a ``datum``
    member instead of a ``datum_ensemble``. The number of elements in the
    datum ensemble may also vary over time when new realizations of WGS 84 are
    added to the ensemble.


ProjectedCRS
++++++++++++

The EPSG:32631 / "WGS 84 / UTM zone 31N" projected CRS can be expressed as

.. code-block:: json

    {
        "$schema": "https://proj.org/schemas/v0.1/projjson.schema.json",
        "type": "ProjectedCRS",
        "name": "WGS 84 / UTM zone 31N",
        "base_crs": {
            "name": "WGS 84",
            "datum": {
                "type": "GeodeticReferenceFrame",
                "name": "World Geodetic System 1984",
                "ellipsoid": {
                    "name": "WGS 84",
                    "semi_major_axis": 6378137,
                    "inverse_flattening": 298.257223563
                }
            },
            "coordinate_system": {
                "subtype": "ellipsoidal",
                "axis": [
                {
                    "name": "Geodetic latitude",
                    "abbreviation": "Lat",
                    "direction": "north",
                    "unit": "degree"
                },
                {
                    "name": "Geodetic longitude",
                    "abbreviation": "Lon",
                    "direction": "east",
                    "unit": "degree"
                }
                ]
            },
            "id": {
                "authority": "EPSG",
                "code": 4326
            }
        },
        "conversion": {
            "name": "UTM zone 31N",
            "method": {
                "name": "Transverse Mercator",
                "id": {
                    "authority": "EPSG",
                    "code": 9807
                }
            },
            "parameters": [
            {
                "name": "Latitude of natural origin",
                "value": 0,
                "unit": "degree",
                "id": {
                    "authority": "EPSG",
                    "code": 8801
                }
            },
            {
                "name": "Longitude of natural origin",
                "value": 3,
                "unit": "degree",
                "id": {
                    "authority": "EPSG",
                    "code": 8802
                }
            },
            {
                "name": "Scale factor at natural origin",
                "value": 0.9996,
                "unit": "unity",
                "id": {
                    "authority": "EPSG",
                    "code": 8805
                }
            },
            {
                "name": "False easting",
                "value": 500000,
                "unit": "metre",
                "id": {
                    "authority": "EPSG",
                    "code": 8806
                }
            },
            {
                "name": "False northing",
                "value": 0,
                "unit": "metre",
                "id": {
                    "authority": "EPSG",
                    "code": 8807
                }
            }
            ]
        },
        "coordinate_system": {
            "subtype": "Cartesian",
            "axis": [
            {
                "name": "Easting",
                "abbreviation": "E",
                "direction": "east",
                "unit": "metre"
            },
            {
                "name": "Northing",
                "abbreviation": "N",
                "direction": "north",
                "unit": "metre"
            }
            ]
        },
        "area": "World - N hemisphere - 0°E to 6°E - by country",
        "bbox": {
            "south_latitude": 0,
            "west_longitude": 0,
            "north_latitude": 84,
            "east_longitude": 6
        },
        "id": {
            "authority": "EPSG",
            "code": 32631
        }
    }

CompoundCRS
+++++++++++

The EPSG:9518 / "WGS 84 + EGM2008 height" compound CRS can be expressed as:

.. code-block:: json

    {
      "$schema": "https://proj.org/schemas/v0.4/projjson.schema.json",
      "type": "CompoundCRS",
      "name": "WGS 84 + EGM2008 height",
      "components": [
        {
          "type": "GeographicCRS",
          "name": "WGS 84",
          "datum_ensemble": {
            "name": "World Geodetic System 1984 ensemble",
            "members": [
              {
                "name": "World Geodetic System 1984 (Transit)",
                "id": {
                  "authority": "EPSG",
                  "code": 1166
                }
              },
              {
                "name": "World Geodetic System 1984 (G730)",
                "id": {
                  "authority": "EPSG",
                  "code": 1152
                }
              },
              {
                "name": "World Geodetic System 1984 (G873)",
                "id": {
                  "authority": "EPSG",
                  "code": 1153
                }
              },
              {
                "name": "World Geodetic System 1984 (G1150)",
                "id": {
                  "authority": "EPSG",
                  "code": 1154
                }
              },
              {
                "name": "World Geodetic System 1984 (G1674)",
                "id": {
                  "authority": "EPSG",
                  "code": 1155
                }
              },
              {
                "name": "World Geodetic System 1984 (G1762)",
                "id": {
                  "authority": "EPSG",
                  "code": 1156
                }
              },
              {
                "name": "World Geodetic System 1984 (G2139)",
                "id": {
                  "authority": "EPSG",
                  "code": 1309
                }
              }
            ],
            "ellipsoid": {
              "name": "WGS 84",
              "semi_major_axis": 6378137,
              "inverse_flattening": 298.257223563
            },
            "accuracy": "2.0",
            "id": {
              "authority": "EPSG",
              "code": 6326
            }
          },
          "coordinate_system": {
            "subtype": "ellipsoidal",
            "axis": [
              {
                "name": "Geodetic latitude",
                "abbreviation": "Lat",
                "direction": "north",
                "unit": "degree"
              },
              {
                "name": "Geodetic longitude",
                "abbreviation": "Lon",
                "direction": "east",
                "unit": "degree"
              }
            ]
          }
        },
        {
          "type": "VerticalCRS",
          "name": "EGM2008 height",
          "datum": {
            "type": "VerticalReferenceFrame",
            "name": "EGM2008 geoid"
          },
          "coordinate_system": {
            "subtype": "vertical",
            "axis": [
              {
                "name": "Gravity-related height",
                "abbreviation": "H",
                "direction": "up",
                "unit": "metre"
              }
            ]
          }
        }
      ],
      "scope": "Spatial referencing.",
      "area": "World.",
      "bbox": {
        "south_latitude": -90,
        "west_longitude": -180,
        "north_latitude": 90,
        "east_longitude": 180
      },
      "id": {
        "authority": "EPSG",
        "code": 9518
      }
    }

BoundCRS
++++++++

The Bound CRS, using as a base EPSG:4258 "ETRS89" geographic CRS, with an
explicit transformation to WGS 84 using a null Helmert transformation, can be expressed as

.. code-block:: json

    {
      "$schema": "https://proj.org/schemas/v0.4/projjson.schema.json",
      "type": "BoundCRS",
      "source_crs": {
        "type": "GeographicCRS",
        "name": "ETRS89",
        "datum_ensemble": {
          "name": "European Terrestrial Reference System 1989 ensemble",
          "members": [
            {
              "name": "European Terrestrial Reference Frame 1989"
            },
            {
              "name": "European Terrestrial Reference Frame 1990"
            },
            {
              "name": "European Terrestrial Reference Frame 1991"
            },
            {
              "name": "European Terrestrial Reference Frame 1992"
            },
            {
              "name": "European Terrestrial Reference Frame 1993"
            },
            {
              "name": "European Terrestrial Reference Frame 1994"
            },
            {
              "name": "European Terrestrial Reference Frame 1996"
            },
            {
              "name": "European Terrestrial Reference Frame 1997"
            },
            {
              "name": "European Terrestrial Reference Frame 2000"
            },
            {
              "name": "European Terrestrial Reference Frame 2005"
            },
            {
              "name": "European Terrestrial Reference Frame 2014"
            }
          ],
          "ellipsoid": {
            "name": "GRS 1980",
            "semi_major_axis": 6378137,
            "inverse_flattening": 298.257222101
          },
          "accuracy": "0.1"
        },
        "coordinate_system": {
          "subtype": "ellipsoidal",
          "axis": [
            {
              "name": "Geodetic latitude",
              "abbreviation": "Lat",
              "direction": "north",
              "unit": "degree"
            },
            {
              "name": "Geodetic longitude",
              "abbreviation": "Lon",
              "direction": "east",
              "unit": "degree"
            }
          ]
        },
        "id": {
          "authority": "EPSG",
          "code": 4258
        }
      },
      "target_crs": {
        "type": "GeographicCRS",
        "name": "WGS 84",
        "datum": {
          "type": "GeodeticReferenceFrame",
          "name": "World Geodetic System 1984",
          "ellipsoid": {
            "name": "WGS 84",
            "semi_major_axis": 6378137,
            "inverse_flattening": 298.257223563
          }
        },
        "coordinate_system": {
          "subtype": "ellipsoidal",
          "axis": [
            {
              "name": "Geodetic latitude",
              "abbreviation": "Lat",
              "direction": "north",
              "unit": "degree"
            },
            {
              "name": "Geodetic longitude",
              "abbreviation": "Lon",
              "direction": "east",
              "unit": "degree"
            }
          ]
        },
        "id": {
          "authority": "EPSG",
          "code": 4326
        }
      },
      "transformation": {
        "name": "Transformation from unknown to WGS84",
        "method": {
          "name": "Position Vector transformation (geog2D domain)",
          "id": {
            "authority": "EPSG",
            "code": 9606
          }
        },
        "parameters": [
          {
            "name": "X-axis translation",
            "value": 0,
            "unit": "metre",
            "id": {
              "authority": "EPSG",
              "code": 8605
            }
          },
          {
            "name": "Y-axis translation",
            "value": 0,
            "unit": "metre",
            "id": {
              "authority": "EPSG",
              "code": 8606
            }
          },
          {
            "name": "Z-axis translation",
            "value": 0,
            "unit": "metre",
            "id": {
              "authority": "EPSG",
              "code": 8607
            }
          },
          {
            "name": "X-axis rotation",
            "value": 0,
            "unit": {
              "type": "AngularUnit",
              "name": "arc-second",
              "conversion_factor": 4.84813681109536e-06
            },
            "id": {
              "authority": "EPSG",
              "code": 8608
            }
          },
          {
            "name": "Y-axis rotation",
            "value": 0,
            "unit": {
              "type": "AngularUnit",
              "name": "arc-second",
              "conversion_factor": 4.84813681109536e-06
            },
            "id": {
              "authority": "EPSG",
              "code": 8609
            }
          },
          {
            "name": "Z-axis rotation",
            "value": 0,
            "unit": {
              "type": "AngularUnit",
              "name": "arc-second",
              "conversion_factor": 4.84813681109536e-06
            },
            "id": {
              "authority": "EPSG",
              "code": 8610
            }
          },
          {
            "name": "Scale difference",
            "value": 0,
            "unit": {
              "type": "ScaleUnit",
              "name": "parts per million",
              "conversion_factor": 1e-06
            },
            "id": {
              "authority": "EPSG",
              "code": 8611
            }
          }
        ]
      }
    }

Transformation
++++++++++++++

The EPSG:8549 / "NAD27 to NAD83 (8)" transformation can be expressed as:

.. code-block:: json

    {
      "$schema": "https://proj.org/schemas/v0.4/projjson.schema.json",
      "type": "Transformation",
      "name": "NAD27 to NAD83 (8)",
      "source_crs": {
        "type": "GeographicCRS",
        "name": "NAD27",
        "datum": {
          "type": "GeodeticReferenceFrame",
          "name": "North American Datum 1927",
          "ellipsoid": {
            "name": "Clarke 1866",
            "semi_major_axis": 6378206.4,
            "semi_minor_axis": 6356583.8
          }
        },
        "coordinate_system": {
          "subtype": "ellipsoidal",
          "axis": [
            {
              "name": "Geodetic latitude",
              "abbreviation": "Lat",
              "direction": "north",
              "unit": "degree"
            },
            {
              "name": "Geodetic longitude",
              "abbreviation": "Lon",
              "direction": "east",
              "unit": "degree"
            }
          ]
        },
        "id": {
          "authority": "EPSG",
          "code": 4267
        }
      },
      "target_crs": {
        "type": "GeographicCRS",
        "name": "NAD83",
        "datum": {
          "type": "GeodeticReferenceFrame",
          "name": "North American Datum 1983",
          "ellipsoid": {
            "name": "GRS 1980",
            "semi_major_axis": 6378137,
            "inverse_flattening": 298.257222101
          }
        },
        "coordinate_system": {
          "subtype": "ellipsoidal",
          "axis": [
            {
              "name": "Geodetic latitude",
              "abbreviation": "Lat",
              "direction": "north",
              "unit": "degree"
            },
            {
              "name": "Geodetic longitude",
              "abbreviation": "Lon",
              "direction": "east",
              "unit": "degree"
            }
          ]
        },
        "id": {
          "authority": "EPSG",
          "code": 4269
        }
      },
      "method": {
        "name": "NADCON5 (2D)",
        "id": {
          "authority": "EPSG",
          "code": 1074
        }
      },
      "parameters": [
        {
          "name": "Latitude difference file",
          "value": "nadcon5.nad27.nad83_1986.alaska.lat.trn.20160901.b",
          "id": {
            "authority": "EPSG",
            "code": 8657
          }
        },
        {
          "name": "Longitude difference file",
          "value": "nadcon5.nad27.nad83_1986.alaska.lon.trn.20160901.b",
          "id": {
            "authority": "EPSG",
            "code": 8658
          }
        }
      ],
      "accuracy": "0.5",
      "scope": "Geodesy.",
      "area": "United States (USA) - Alaska.",
      "bbox": {
        "south_latitude": 51.3,
        "west_longitude": 172.42,
        "north_latitude": 71.4,
        "east_longitude": -129.99
      },
      "id": {
        "authority": "EPSG",
        "code": 8549
      },
      "remarks": "Uses NADCON5 method which expects longitudes positive east in range 0-360°; source and target CRSs have longitudes positive east in range -180° to +180°. Accuracy at 67% confidence level is 0.5m onshore, 5m nearshore and undetermined farther offshore."
    }

Deviations with the WKT2:2019 specification
-------------------------------------------

While most of this specification is intended to be interoperable with WKT2:2019,
there are a few deviations, reflecting the needs of the PROJ software implementation.

PROJJSON extensions
+++++++++++++++++++

This specification allows a Bound CRS to be used wherever a CRS object is allowed
in the OGC Topic 2 abstract specification / ISO-19111:2019. In particular,
the members of a compound CRS can be a Bound CRS in this specification, whereas
OGC Topic 2 abstract specification restricts it to single CRS. A Bound CRS can
also be used as the source or target of a coordinate operation.

PROJJSON omissions
++++++++++++++++++

This specification does not define an encoding for triaxial ellipsoid (``TRIAXIAL`` WKT keyword)

Reference implementation
------------------------

PROJJSON is available as input and output of the `PROJ <https://proj.org>`_ software since PROJ 6.2.

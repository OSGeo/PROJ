#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Build NRCan specific definitions
#  Author:   Even Rouault <even.rouault at spatialys.com>
#
###############################################################################
#  Copyright (c) 2023, Even Rouault <even.rouault at spatialys.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
###############################################################################

import json
import os


def MTM_NAD83CSRSv7(zone):
    longitude = {
        1: -53,
        2: -56,
        3: -58.5,
        4: -61.5,
        5: -64.5,
        6: -67.5,
        7: -70.5,
        8: -73.5,
        9: -76.5,
        10: -79.5,
        11: -82.5,
        12: -81,
        13: -84,
        14: -87,
        15: -90,
        16: -93,
        17: -96
    }
    return {
        "type": "ProjectedCRS",
        "name": "NAD83(CSRS)v7 / MTM zone " + str(zone),
        "base_crs": {
            "name": "NAD83(CSRS)v7",
            "datum": {
                "type": "GeodeticReferenceFrame",
                "name": "North American Datum of 1983 (CSRS) version 7",
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
            }
        },
        "conversion": {
            "name": "MTM zone " + str(zone),
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
                    "value": longitude[zone],
                    "unit": "degree",
                    "id": {
                        "authority": "EPSG",
                        "code": 8802
                    }
                },
                {
                    "name": "Scale factor at natural origin",
                    "value": 0.9999,
                    "unit": "unity",
                    "id": {
                        "authority": "EPSG",
                        "code": 8805
                    }
                },
                {
                    "name": "False easting",
                    "value": 304800,
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
                    "abbreviation": "E(X)",
                    "direction": "east",
                    "unit": "metre"
                },
                {
                    "name": "Northing",
                    "abbreviation": "N(Y)",
                    "direction": "north",
                    "unit": "metre"
                }
            ]
        }
    }


def UTM_NAD83CSRSv7(zone):
    return {
        "type": "ProjectedCRS",
        "name": "NAD83(CSRS)v7 / UTM zone " + str(zone),
        "base_crs": {
            "name": "NAD83(CSRS)v7",
            "datum": {
                "type": "GeodeticReferenceFrame",
                "name": "North American Datum of 1983 (CSRS) version 7",
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
            }
        },
        "conversion": {
            "name": "UTM zone " + str(zone) + "N",
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
                    "value": -183 + zone * 6,
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
                    "abbreviation": "E(X)",
                    "direction": "east",
                    "unit": "metre"
                },
                {
                    "name": "Northing",
                    "abbreviation": "N(Y)",
                    "direction": "north",
                    "unit": "metre"
                }
            ]
        }
    }


def vert_crs_CGVD28(geoid_model_name, geoid_model_authority, geoid_model_code):
    return {
        "type": "VerticalCRS",
        "name": "CGVD28 height",
        "datum": {
            "type": "VerticalReferenceFrame",
            "name": "Canadian Geodetic Vertical Datum of 1928"
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
        },
        "geoid_model": {
            "name": geoid_model_name,
            "id": {
                "authority": geoid_model_authority,
                "code": geoid_model_code
            }
        },
        "id": {
            "authority": "EPSG",
            "code": 5713
        }
    }


def vert_crs_CGVD28_HT2_1997():
    return vert_crs_CGVD28("HT2_1997", "NRCAN", "HT2_1997_NAD83CSRSV7")


def vert_crs_CGVD28_HT2_2002():
    return vert_crs_CGVD28("HT2_2002", "NRCAN", "HT2_2002_NAD83CSRSV7")


def vert_crs_CGVD28_HT2_2010():
    return vert_crs_CGVD28("HT2_2010", "EPSG", 9987)


def vert_crs_CGVD2013a_1997():
    return {
        "type": "VerticalCRS",
        "name": "CGVD2013a(1997) height",
        "datum": {
            "type": "VerticalReferenceFrame",
            "name": "Canadian Geodetic Vertical Datum of 2013 (CGG2013a) epoch 1997"
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
        },
        "id": {
            "authority": "EPSG",
            "code": 20035
        }
    }


def vert_crs_CGVD2013a_2002():
    return {
        "type": "VerticalCRS",
        "name": "CGVD2013a(2002) height",
        "datum": {
            "type": "VerticalReferenceFrame",
            "name": "Canadian Geodetic Vertical Datum of 2013 (CGG2013a) epoch 2002"
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
        },
        "id": {
            "authority": "EPSG",
            "code": 20034
        }
    }


def vert_crs_CGVD2013a_2010():
    return {
        "type": "VerticalCRS",
        "name": "CGVD2013a(2010) height",
        "datum": {
            "type": "VerticalReferenceFrame",
            "name": "Canadian Geodetic Vertical Datum of 2013 (CGG2013a) epoch 2010"
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
        },
        "id": {
            "authority": "EPSG",
            "code": 9245
        }
    }


usages_MTM = {
    1: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Newfoundland - onshore east of 54°30'W.",
        "bbox": {
            "south_latitude": 46.56,
            "west_longitude": -54.5,
            "north_latitude": 49.89,
            "east_longitude": -52.54
        },
    },
    2: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Newfoundland and Labrador between 57°30'W and 54°30'W.",
        "bbox": {
            "south_latitude": 46.81,
            "west_longitude": -57.5,
            "north_latitude": 54.71,
            "east_longitude": -54.49
        },

    },
    3:
    {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Newfoundland west of 57°30'W.",
        "bbox": {
            "south_latitude": 47.5,
            "west_longitude": -59.48,
            "north_latitude": 50.54,
            "east_longitude": -57.5
        },
    },
    4: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Labrador between 63°W and 60°W.",
        "bbox": {
            "south_latitude": 52,
            "west_longitude": -63,
            "north_latitude": 58.92,
            "east_longitude": -60
        },
    },
    5: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Labrador - 66°W to 63°W.",
        "bbox": {
            "south_latitude": 51.58,
            "west_longitude": -66,
            "north_latitude": 60.52,
            "east_longitude": -63
        },
    },
    6: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Labrador - west of 66°W.",
        "bbox": {
            "south_latitude": 52.05,
            "west_longitude": -67.81,
            "north_latitude": 55.34,
            "east_longitude": -66
        },
    },
    7: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Quebec - between 72°W and 69°W.",
        "bbox": {
            "south_latitude": 45.01,
            "west_longitude": -72,
            "north_latitude": 61.8,
            "east_longitude": -69
        },

    },
    8: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - east of 75°W.",
        "bbox": {
            "south_latitude": 44.98,
            "west_longitude": -75,
            "north_latitude": 45.65,
            "east_longitude": -74.35
        },
    },
    9: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - between 78°W and 75°W.",
        "bbox": {
            "south_latitude": 43.63,
            "west_longitude": -78,
            "north_latitude": 46.25,
            "east_longitude": -75
        },
    },
    10: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - between 81°W and 78°W: south of 46°N in area to west of 80°15'W, south of 47°N in area between 80°15'W and 79°30'W, entire province between 79°30'W and 78°W.",
        "bbox": {
            "south_latitude": 42.26,
            "west_longitude": -81,
            "north_latitude": 47.33,
            "east_longitude": -77.99
        },
    },
    11: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - south of 46°N and west of 81°W.",
        "bbox": {
            "south_latitude": 41.67,
            "west_longitude": -83.6,
            "north_latitude": 46,
            "east_longitude": -81
        },
    },
    12: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - between 82°30'W and 79°30'W: north of 46°N in area between 82°30'W and 80°15'W, north of 47°N in area between 80°15'W and 79°30'W.",
        "bbox": {
            "south_latitude": 46,
            "west_longitude": -82.5,
            "north_latitude": 55.21,
            "east_longitude": -79.5
        },
    },
    13: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - between 85°30'W and 82°30'W and north of 46°N.",
        "bbox": {
            "south_latitude": 46,
            "west_longitude": -85.5,
            "north_latitude": 55.59,
            "east_longitude": -82.5
        },
    },
    14: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - between 88°30'W and 85°30'W.",
        "bbox": {
            "south_latitude": 47.17,
            "west_longitude": -88.5,
            "north_latitude": 56.7,
            "east_longitude": -85.5
        },

    },
    15: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - between 91°30'W and 88°30'W.",
        "bbox": {
            "south_latitude": 47.97,
            "west_longitude": -91.5,
            "north_latitude": 56.9,
            "east_longitude": -88.5
        },
    },
    16: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - between 94°30'W and 91°30'W.",
        "bbox": {
            "south_latitude": 48.06,
            "west_longitude": -94.5,
            "north_latitude": 55.2,
            "east_longitude": -91.5
        },
    },
    17: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada - Ontario - west of 94°30'W.",
        "bbox": {
            "south_latitude": 48.69,
            "west_longitude": -95.16,
            "north_latitude": 53.24,
            "east_longitude": -94.5
        },
    },
}

usages_UTM = {
    7: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada west of 138°W, onshore and offshore south of 84°N - British Columbia, Yukon.",
        "bbox": {
            "south_latitude": 52.05,
            "west_longitude": -141.01,
            "north_latitude": 72.53,
            "east_longitude": -138
        },
    },
    8: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 138°W and 132°W, onshore and offshore south of 84°N - British Columbia, Northwest Territories, Yukon.",
        "bbox": {
            "south_latitude": 48.06,
            "west_longitude": -138,
            "north_latitude": 79.42,
            "east_longitude": -132
        },
    },
    9: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 138°W and 132°W, onshore and offshore south of 84°N - British Columbia, Northwest Territories, Yukon.",
        "bbox": {
            "south_latitude": 48.06,
            "west_longitude": -138,
            "north_latitude": 79.42,
            "east_longitude": -132
        },
    },
    10: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 126°W and 120°W, onshore and offshore south of 84°N - British Columbia, Northwest Territories, Yukon.",
        "bbox": {
            "south_latitude": 48.13,
            "west_longitude": -126,
            "north_latitude": 81.8,
            "east_longitude": -120
        },
    },
    11: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 120°W and 114°W onshore and offshore - Alberta, British Columbia, Northwest Territories, Nunavut.",
        "bbox": {
            "south_latitude": 48.99,
            "west_longitude": -120,
            "north_latitude": 83.5,
            "east_longitude": -114
        },
    },
    12: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 114°W and 108°W onshore and offshore - Alberta, Northwest Territories, Nunavut, Saskatchewan.",
        "bbox": {
            "south_latitude": 48.99,
            "west_longitude": -114,
            "north_latitude": 84,
            "east_longitude": -108
        },
    },
    13: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 108°W and 102°W onshore and offshore - Northwest Territories, Nunavut, Saskatchewan.",
        "bbox": {
            "south_latitude": 48.99,
            "west_longitude": -108,
            "north_latitude": 84,
            "east_longitude": -102
        },
    },
    14: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 102°W and 96°W, onshore and offshore south of 84°N - Manitoba, Nunavut, Saskatchewan.",
        "bbox": {
            "south_latitude": 48.99,
            "west_longitude": -102,
            "north_latitude": 84,
            "east_longitude": -96
        },
    },
    15: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 96°W and 90°W, onshore and offshore south of 84°N - Manitoba, Nunavut, Ontario.",
        "bbox": {
            "south_latitude": 48.03,
            "west_longitude": -96,
            "north_latitude": 84,
            "east_longitude": -90
        },
    },
    16: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 90°W and 84°W, onshore and offshore south of 84°N - Manitoba, Nunavut, Ontario.",
        "bbox": {
            "south_latitude": 46.11,
            "west_longitude": -90,
            "north_latitude": 84,
            "east_longitude": -84
        },
    },
    17: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 84°W and 78°W, onshore and offshore south of 84°N - Nunavut, Ontario and Quebec.",
        "bbox": {
            "south_latitude": 41.67,
            "west_longitude": -84,
            "north_latitude": 84,
            "east_longitude": -78
        },
    },
    18: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 78°W and 72°W, onshore and offshore south of 84°N - Nunavut, Ontario and Quebec.",
        "bbox": {
            "south_latitude": 43.63,
            "west_longitude": -78,
            "north_latitude": 84,
            "east_longitude": -72
        },
    },
    19: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 72°W and 66°W onshore and offshore - New Brunswick, Labrador, Nova Scotia, Nunavut, Quebec.",
        "bbox": {
            "south_latitude": 40.8,
            "west_longitude": -72,
            "north_latitude": 84,
            "east_longitude": -66
        },
    },
    20: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 66°W and 60°W onshore and offshore - New Brunswick, Labrador, Nova Scotia, Nunavut, Prince Edward Island, Quebec.",
        "bbox": {
            "south_latitude": 40.04,
            "west_longitude": -66,
            "north_latitude": 84,
            "east_longitude": -60
        },
    },
    21: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 60°W and 54°W - Newfoundland and Labrador; Nunavut; Quebec.",
        "bbox": {
            "south_latitude": 38.56,
            "west_longitude": -60,
            "north_latitude": 84,
            "east_longitude": -54
        },
    },
    22: {
        "scope": "Engineering survey, topographic mapping.",
        "area": "Canada between 54°W and 48°W onshore and offshore - Newfoundland and Labrador.",
        "bbox": {
            "south_latitude": 39.5,
            "west_longitude": -54,
            "north_latitude": 57.65,
            "east_longitude": -47.99
        },
    }
}


def compound_crs_MTM_HT_1997(zone):
    j = {
        "type": "CompoundCRS",
        "components": [
            MTM_NAD83CSRSv7(zone),
            vert_crs_CGVD28_HT2_1997()
        ]
    }
    j["name"] = j["components"][0]["name"] + " + " + j["components"][1]["name"]
    usage = usages_MTM[zone]
    for key in usage:
        j[key] = usage[key]
    return j


def compound_crs_MTM_HT_2002(zone):
    j = {
        "type": "CompoundCRS",
        "components": [
            MTM_NAD83CSRSv7(zone),
            vert_crs_CGVD28_HT2_2002()
        ]
    }
    j["name"] = j["components"][0]["name"] + " + " + j["components"][1]["name"]
    usage = usages_MTM[zone]
    for key in usage:
        j[key] = usage[key]
    return j


def compound_crs_MTM_HT_2010(zone):
    j = {
        "type": "CompoundCRS",
        "components": [
            MTM_NAD83CSRSv7(zone),
            vert_crs_CGVD28_HT2_2010()
        ]
    }
    j["name"] = j["components"][0]["name"] + " + " + j["components"][1]["name"]
    usage = usages_MTM[zone]
    for key in usage:
        j[key] = usage[key]
    return j


def compound_crs_UTM_CGVD2013_1997(zone):
    j = {
        "type": "CompoundCRS",
        "components": [
            UTM_NAD83CSRSv7(zone),
            vert_crs_CGVD2013a_1997()
        ]
    }
    j["name"] = j["components"][0]["name"] + " + " + j["components"][1]["name"]
    usage = usages_UTM[zone]
    for key in usage:
        j[key] = usage[key]
    return j


def compound_crs_UTM_CGVD2013_2002(zone):
    j = {
        "type": "CompoundCRS",
        "components": [
            UTM_NAD83CSRSv7(zone),
            vert_crs_CGVD2013a_2002()
        ]
    }
    j["name"] = j["components"][0]["name"] + " + " + j["components"][1]["name"]
    usage = usages_UTM[zone]
    for key in usage:
        j[key] = usage[key]
    return j


def compound_crs_UTM_CGVD2013_2010(zone):
    j = {
        "type": "CompoundCRS",
        "components": [
            UTM_NAD83CSRSv7(zone),
            vert_crs_CGVD2013a_2010()
        ]
    }
    j["name"] = j["components"][0]["name"] + " + " + j["components"][1]["name"]
    usage = usages_UTM[zone]
    for key in usage:
        j[key] = usage[key]
    return j


script_dir_name = os.path.dirname(os.path.realpath(__file__))
sql_dir_name = os.path.join(os.path.dirname(script_dir_name), 'data', 'sql')

all_sql = []
all_sql.append("""INSERT INTO "grid_transformation" VALUES('NRCAN','HT2_1997_NAD83CSRSV7','NAD83(CSRS)v7 to CGVD28 height',NULL,'EPSG','1060','Geographic3D to GravityRelatedHeight (NRCan byn)','EPSG','8254','EPSG','5713',0.05,'EPSG','8666','Geoid (height correction) model file','HT2_1997.byn',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'NRC-Can CGG2000',0);""")
all_sql.append("""INSERT INTO "usage" VALUES('NRCAN','USAGE_HT2_1997_NAD83CSRSV7','grid_transformation','NRCAN','HT2_1997_NAD83CSRSV7','EPSG','1289','EPSG','1133');""")

all_sql.append("""INSERT INTO "grid_transformation" VALUES('NRCAN','HT2_2002_NAD83CSRSV7','NAD83(CSRS)v7 to CGVD28 height',NULL,'EPSG','1060','Geographic3D to GravityRelatedHeight (NRCan byn)','EPSG','8254','EPSG','5713',0.05,'EPSG','8666','Geoid (height correction) model file','HT2_2002v70.byn',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'NRC-Can CGG2000 2002',0);""")
all_sql.append("""INSERT INTO "usage" VALUES('NRCAN','USAGE_HT2_2002_NAD83CSRSV7','grid_transformation','NRCAN','HT2_2002_NAD83CSRSV7','EPSG','1289','EPSG','1133');""")

for zone in range(1, 17+1):
    projjson = json.dumps(compound_crs_MTM_HT_1997(zone)).replace("'", "''")
    all_sql.append(
        f"""INSERT INTO coordinate_metadata VALUES('NRCAN', 'NAD83_CSRS_1997_MTM{zone}_HT2_1997', NULL, NULL, NULL, '{projjson}', 1997.0, 0);""")

for zone in range(1, 17+1):
    projjson = json.dumps(compound_crs_MTM_HT_2002(zone)).replace("'", "''")
    all_sql.append(
        f"""INSERT INTO coordinate_metadata VALUES('NRCAN', 'NAD83_CSRS_2002_MTM{zone}_HT2_2002', NULL, NULL, NULL, '{projjson}', 2002.0, 0);""")

for zone in range(1, 17+1):
    projjson = json.dumps(compound_crs_MTM_HT_2010(zone)).replace("'", "''")
    all_sql.append(
        f"""INSERT INTO coordinate_metadata VALUES('NRCAN', 'NAD83_CSRS_2010_MTM{zone}_HT2_2010', NULL, NULL, NULL, '{projjson}', 2010.0, 0);""")

for zone in range(7, 22+1):
    projjson = json.dumps(
        compound_crs_UTM_CGVD2013_1997(zone)).replace("'", "''")
    all_sql.append(
        f"""INSERT INTO coordinate_metadata VALUES('NRCAN', 'NAD83_CSRS_1997_UTM{zone}_CGVD2013_1997', NULL, NULL, NULL, '{projjson}', 1997.0, 0);""")

for zone in range(7, 22+1):
    projjson = json.dumps(
        compound_crs_UTM_CGVD2013_2002(zone)).replace("'", "''")
    all_sql.append(
        f"""INSERT INTO coordinate_metadata VALUES('NRCAN', 'NAD83_CSRS_2002_UTM{zone}_CGVD2013_2002', NULL, NULL, NULL, '{projjson}', 2002.0, 0);""")

for zone in range(7, 22+1):
    projjson = json.dumps(
        compound_crs_UTM_CGVD2013_2010(zone)).replace("'", "''")
    all_sql.append(
        f"""INSERT INTO coordinate_metadata VALUES('NRCAN', 'NAD83_CSRS_2010_UTM{zone}_CGVD2013_2010', NULL, NULL, NULL, '{projjson}', 2010.0, 0);""")

f = open(os.path.join(sql_dir_name, 'nrcan') + '.sql', 'wb')
f.write("--- This file has been generated by scripts/build_nrcan.py. DO NOT EDIT !\n\n".encode('UTF-8'))
for sql in all_sql:
    f.write((sql + '\n').encode('UTF-8'))
f.close()

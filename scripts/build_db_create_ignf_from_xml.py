#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Build SRS and coordinate transform database from IGNF registry
#  Author:   Even Rouault <even.rouault at spatialys.com>
#
###############################################################################
#  Copyright (c) 2018, Even Rouault <even.rouault at spatialys.com>
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

from lxml import etree
import os
import requests
import sys

# url = "http://librairies.ign.fr/geoportail/resources/IGNF.xml"
url = "https://geodesie.ign.fr/contenu/fichiers/IGNF.v3.1.0.xml"

if len(sys.argv) not in (1, 2) or (len(sys.argv) == 2 and sys.argv[1].startswith('-')):
    print('Usage: build_db_create_ignf.py [path_to_IGNF.xml]')
    print('')
    print('If local filename is not used, then %s is downloaded.' % url)
    sys.exit(1)

def escape_literal(x):
    return x.replace("'", "''")

def strip_ns_prefix(tree):
    for element in tree.iter('*'):
        element.tag = etree.QName(element).localname
        for att in element.attrib:
            newkey = att[att.find('}')+1:]
            val = element.attrib[att]
            del element.attrib[att]
            element.attrib[newkey] = val
    return tree

all_sql = []
all_sql_concat = []  # for concatenated_operation

if len(sys.argv) == 1:
    r = requests.get(url)
    root = etree.fromstring(r.content)
else:
    IGNF_file = sys.argv[1]
    tree = etree.parse(open(IGNF_file, 'rt'))
    root = tree.getroot()

root = strip_ns_prefix(root)

# Retrieve and insert metada
version = root.find('versionNumber').find('CharacterString').text
date = root.find('versionDate').find('Date').text

all_sql.append("""INSERT INTO "metadata" VALUES('IGNF.SOURCE', '%s');""" % escape_literal(url))
all_sql.append("""INSERT INTO "metadata" VALUES('IGNF.VERSION', '%s');""" % version)
all_sql.append("""INSERT INTO "metadata" VALUES('IGNF.DATE', '%s');""" % date)

def get_epsg_code(txt):
    assert ':EPSG:' in txt
    return txt[txt.rfind(':')+1:]


def ingest_ellipsoids(root, all_sql):
    mapEllpsId = {}
    for ellps in root.iter('ellipsoid'):
        E = ellps.find('Ellipsoid')
        id = E.attrib['id']
        names = [name.text for name in E.iter('name')]
        #print(id, names)
        if len(names) == 2:
            mapEllpsId[id] = ('EPSG', get_epsg_code(names[1]))
        else:
            assert len(names) == 1
            assert E.find('secondDefiningParameter').find('SecondDefiningParameter').find('isSphere').text == 'sphere'
            mapEllpsId[id] = ('IGNF', id)
            all_sql.append("""INSERT INTO "ellipsoid" VALUES('IGNF','%s','%s',NULL,'PROJ', 'EARTH', %s,'EPSG','9001',0,NULL,0);""" % (id, names[0], E.find('semiMajorAxis').text))

    return mapEllpsId


def ingest_prime_meridians(root, all_sql):
    mapPmId = {}
    for pm in root.iter('primeMeridian'):
        PM = pm.find('PrimeMeridian')
        id = PM.attrib['id']
        names = [name.text for name in PM.iter('name')]
        #print(id, names)
        assert len(names) == 2
        mapPmId[id] = ('EPSG', get_epsg_code(names[1]))

    return mapPmId

def extract_id_from_href(txt):
    assert '#' in txt
    return txt[txt.rfind('#')+1:]

def ingest_datums(root, all_sql, mapEllpsId, mapPmId):
    mapDatumId = {}
    invalidDatumId = set()
    mapVerticalDatumId = {}

    for datum in root.iter('datum'):
        node = datum.find('GeodeticDatum')
        if node is not None:
            id = node.attrib['id']
            names = [_name.text for _name in node.iter('name')]
            if len(names) == 2:
                if id == 'REG0020002':
                    assert get_epsg_code(names[1]) == '6275'
                    print('Error in registry. it points to EPSG:6275 instead of EPSG:6807 for NTF Paris')
                    mapDatumId[id] = ('EPSG', '6807')
                else:
                    mapDatumId[id] = ('EPSG', get_epsg_code(names[1]))
            else:
                assert len(names) == 1, names
                pmNode = node.find('usesPrimeMeridian')
                if pmNode is None or 'href' not in pmNode.attrib:
                    print('Invalid GeodeticDatum: ' + id)
                    invalidDatumId.add(id)
                    continue
                pmCode = extract_id_from_href(pmNode.attrib['href'])
                assert pmCode in mapPmId
                ellpsCode = extract_id_from_href(node.find('usesEllipsoid').attrib['href'])
                assert ellpsCode in mapEllpsId

                sql = """INSERT INTO "geodetic_datum" VALUES('IGNF','%s','%s',NULL,'%s','%s','%s','%s',NULL,NULL,NULL,0);""" % (id, names[0], mapEllpsId[ellpsCode][0], mapEllpsId[ellpsCode][1], mapPmId[pmCode][0], mapPmId[pmCode][1])
                all_sql.append(sql)

                mapDatumId[id] = ('IGNF', id)
        else:
            node = datum.find('VerticalDatum')
            if node is not None:
                id = node.attrib['id']
                names = [_name.text for _name in node.iter('name')]

                sql = """INSERT INTO "vertical_datum" VALUES('IGNF','%s','%s',NULL,NULL,NULL,NULL,0);"""% (id, names[0])
                all_sql.append(sql)

                mapVerticalDatumId[id] = ('IGNF', id)
            else:
                assert False

    return mapDatumId, mapVerticalDatumId, invalidDatumId


mapEllpsId = ingest_ellipsoids(root, all_sql)
mapPmId = ingest_prime_meridians(root, all_sql)
mapDatumId, mapVerticalDatumId, invalidDatumId = ingest_datums(root, all_sql, mapEllpsId, mapPmId)

extentMap = {}

def get_extent_auth_name_code(domainOfValidity):
    extent = domainOfValidity.find('EX_Extent')
    desc = extent.find('description').find('CharacterString').text
    if desc is None:
        return 'EPSG', '1262'

    if desc in extentMap:
        return extentMap[desc]

    geographicElement = extent.find('geographicElement')
    if geographicElement is None:
        print('No geographicElement for area of use ' + desc)
        return 'EPSG', '1262'

    code = str(len(extentMap) + 1)
    extentMap[desc] = ['IGNF', code ]
    EX_GeographicBoundingBox = geographicElement.find('EX_GeographicBoundingBox')
    south = EX_GeographicBoundingBox.find('southBoundLatitude').find('Decimal').text
    west = EX_GeographicBoundingBox.find('westBoundLongitude').find('Decimal').text
    north = EX_GeographicBoundingBox.find('northBoundLatitude').find('Decimal').text
    east = EX_GeographicBoundingBox.find('eastBoundLongitude').find('Decimal').text
    all_sql.append("""INSERT INTO "extent" VALUES('IGNF','%s','%s','%s',%s,%s,%s,%s,0);""" % (code, escape_literal(desc), escape_literal(desc), south, north, west, east))
    return extentMap[desc]

scopeMap = {}
def get_scope_auth_name_code(scope):
    if scope in scopeMap:
        return scopeMap[scope]

    code = str(len(scopeMap)+1)
    scopeMap[scope] = ['IGNF', code]
    all_sql.append("""INSERT INTO scope VALUES('IGNF','%s','%s',0);""" % (code, scope))
    return scopeMap[scope]

mapCrsId = {}
mapGeocentricId = {}

# This is a trick to find a GeocentricCRS and its related GeographicCRS
# We could use the name, but if we use the datum code + area of use, it is
# more reliable
# We need this since the registry only exposes GeocentricCRS <--> GeocentricCRS
# transformations, and we need to port them to GeographicCRS as well
mapDatumAndAreaToGeocentricId = {}
mapGeocentricIdToDatumAndArea = {}

aliasOfCRS = {}

for node in root.iterfind('.//GeocentricCRS'):
    id = node.attrib['id']
    names = [_name.text for _name in node.iter('name')]
    name = names[0]

    cartesianCS = extract_id_from_href(node.find('usesCartesianCS').attrib['href'])
    assert cartesianCS == 'TYP_CRG10'
    datumCode = extract_id_from_href(node.find('usesGeodeticDatum').attrib['href'])
    if datumCode in invalidDatumId:
        print('Skipping GeocentricCRS %s since its datum is unknown' % id)
        continue
    assert datumCode in mapDatumId, (id, name, datumCode)

    extent_auth_and_code = get_extent_auth_name_code(node.find('domainOfValidity'))
    scope = node.find('scope').text
    scope_auth_and_code = get_scope_auth_name_code(scope)

    #sql = """INSERT INTO "crs" VALUES('IGNF','%s','geocentric');""" % (id)
    #all_sql.append(sql)

    sql = """INSERT INTO "geodetic_crs" VALUES('IGNF','%s','%s',NULL,'geocentric','EPSG','6500','%s','%s',NULL,0);""" % (id, name, mapDatumId[datumCode][0], mapDatumId[datumCode][1])
    all_sql.append(sql)
    sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','geodetic_crs','IGNF','%s','%s','%s','%s','%s');""" % (id, id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
    all_sql.append(sql)

    mapCrsId[id] = ('IGNF', id)
    mapGeocentricId[id] = ('IGNF', id)

    if len(names) >= 2 and names[1].startswith('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):
        alias = names[1][len('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):]
        aliasOfCRS[id] = [('IGNF', alias)]

        if id == 'WGS84':
            aliasOfCRS[id].append(('EPSG', '4978'))

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','geocentric'); -- alias of %s""" % (alias, id)
        #all_sql.append(sql)

        sql = """INSERT INTO "geodetic_crs" VALUES('IGNF','%s','%s',NULL,'geocentric','EPSG','6500','%s','%s',NULL,0);""" % (alias, name, mapDatumId[datumCode][0], mapDatumId[datumCode][1])
        all_sql.append(sql)
        sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','geodetic_crs','IGNF','%s','%s','%s','%s','%s');""" % (alias, alias, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
        all_sql.append(sql)

    key = str((mapDatumId[datumCode], extent_auth_and_code))
    assert key not in mapDatumAndAreaToGeocentricId, (id, name)
    mapDatumAndAreaToGeocentricId[key] = ('IGNF', id)
    mapGeocentricIdToDatumAndArea[id] = key

mapGeographicId = {}
mapDatumAndAreaToGeographicId = {}

for node in root.iterfind('.//GeographicCRS'):
    id = node.attrib['id']
    names = [_name.text for _name in node.iter('name')]
    name = names[0]

    ellipsoidalCS = extract_id_from_href(node.find('usesEllipsoidalCS').attrib['href'])
    assert ellipsoidalCS in ('TYP_CRG24', 'TYP_CRG26', 'TYP_CRG22', 'TYP_CRG28', 'TYP_CRG29'), (id, name, ellipsoidalCS)
    datumCode = extract_id_from_href(node.find('usesGeodeticDatum').attrib['href'])
    if datumCode in invalidDatumId:
        print('Skipping GeographicCRS %s since its datum is unknown' % id)
        continue
    assert datumCode in mapDatumId, (id, name, datumCode)

    extent_auth_and_code = get_extent_auth_name_code(node.find('domainOfValidity'))
    scope = node.find('scope').text
    scope_auth_and_code = get_scope_auth_name_code(scope)

    csCode = None
    type = 'geographic 2D'
    if ellipsoidalCS in ('TYP_CRG24', 'TYP_CRG28'): # Long, Lat deg
        csCode = '6424'
    if ellipsoidalCS in ('TYP_CRG26', 'TYP_CRG29'): # Long, Lat deg, h m
        csCode = '6426'
        type = 'geographic 3D'
    if ellipsoidalCS == 'TYP_CRG22': # Long, Lat grad
        csCode = '6425'

    #sql = """INSERT INTO "crs" VALUES('IGNF','%s','%s');""" % (id, type)
    #all_sql.append(sql)

    sql = """INSERT INTO "geodetic_crs" VALUES('IGNF','%s','%s',NULL,'%s','EPSG','%s','%s','%s',NULL,0);""" % (id, name, type, csCode, mapDatumId[datumCode][0], mapDatumId[datumCode][1])
    all_sql.append(sql)
    sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','geodetic_crs','IGNF','%s','%s','%s','%s','%s');""" % (id, id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
    all_sql.append(sql)

    if id == 'WGS84G':
        aliasOfCRS[id] = [('EPSG','4326')]

    if len(names) >= 2 and names[1].startswith('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):
        alias = names[1][len('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):]
        assert id != 'WGS84G'
        aliasOfCRS[id] = [('IGNF', alias)]

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','%s'); -- alias of %s""" % (alias, type, id)
        #all_sql.append(sql)

        sql = """INSERT INTO "geodetic_crs" VALUES('IGNF','%s','%s',NULL,'%s','EPSG','%s','%s','%s',NULL,0);""" % (alias, name, type, csCode, mapDatumId[datumCode][0], mapDatumId[datumCode][1])
        all_sql.append(sql)
        sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','geodetic_crs','IGNF','%s','%s','%s','%s','%s');""" % (alias, alias, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
        all_sql.append(sql)

    mapCrsId[id] = ('IGNF', id)
    mapGeographicId[id] = ('IGNF', id)
    key = str((mapDatumId[datumCode], extent_auth_and_code))
    if key in mapDatumAndAreaToGeographicId:
        #print('Adding ' + id + ' to ' + str(mapDatumAndAreaToGeographicId[key]))
        mapDatumAndAreaToGeographicId[key].append(id)
    else:
        mapDatumAndAreaToGeographicId[key] = [id]


    # Create a 2D version to be able to create compoundCRS with it
    if id == 'RGWF96GEO':

        id = 'RGWF96G'
        csCode = '6424'
        type = 'geographic 2D'

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','%s');""" % (id, type)
        #all_sql.append(sql)

        sql = """INSERT INTO "geodetic_crs" VALUES('IGNF','%s','%s',NULL,'%s','EPSG','%s','%s','%s',NULL,0);""" % (id, name, type, csCode, mapDatumId[datumCode][0], mapDatumId[datumCode][1])
        all_sql.append(sql)
        sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','geodetic_crs','IGNF','%s','%s','%s','%s','%s');""" % (id, id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
        all_sql.append(sql)

        mapCrsId[id] = ('IGNF', id)
        mapGeographicId[id] = ('IGNF', id)
        key = str((mapDatumId[datumCode], extent_auth_and_code))
        if key in mapDatumAndAreaToGeographicId:
            #print('Adding ' + id + ' to ' + str(mapDatumAndAreaToGeographicId[key]))
            mapDatumAndAreaToGeographicId[key].append(id)
        else:
            mapDatumAndAreaToGeographicId[key] = [id]


mapVerticalCrsId = {}
for node in root.iterfind('.//VerticalCRS'):
    id = node.attrib['id']
    names = [_name.text for _name in node.iter('name')]
    name = names[0]

    verticalCS = extract_id_from_href(node.find('usesVerticalCS').attrib['href'])
    assert verticalCS in ('TYP_CRG92','TYP_CRG91'), verticalCS
    datumCode = extract_id_from_href(node.find('usesVerticalDatum').attrib['href'])
    assert datumCode in mapVerticalDatumId, (id, name, datumCode)

    # VerticalCRS and GeocentricCRS can have same IDs ! like 'STPM50'
    id_modified = id
    if id in mapCrsId:
        print('VerticalCRS %s conflicts with a Geodetic one of same name. Appending _V for disambiguation'% id)
        id_modified += '_V'

    #sql = """INSERT INTO "crs" VALUES('IGNF','%s','vertical');""" % (id_modified)
    #all_sql.append(sql)

    extent_auth_and_code = get_extent_auth_name_code(node.find('domainOfValidity'))
    scope = node.find('scope').text
    scope_auth_and_code = get_scope_auth_name_code(scope)

    sql = """INSERT INTO "vertical_crs" VALUES('IGNF','%s','%s',NULL,'EPSG','6499','%s','%s',0);""" % (id_modified, name, mapVerticalDatumId[datumCode][0], mapVerticalDatumId[datumCode][1])
    all_sql.append(sql)
    sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','vertical_crs','IGNF','%s','%s','%s','%s','%s');""" % (id_modified, id_modified, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
    all_sql.append(sql)

    if len(names) >= 2 and names[1].startswith('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):
        assert False

    mapCrsId[id] = ('IGNF', id_modified)
    mapVerticalCrsId[id] = ('IGNF', id_modified)


mapGridURLs = {
     # France metropole
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/RAF09.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/metropole/RAF09.mnt',

    # Corse
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/RAC09.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/metropole/RAC09.mnt',



    # Guadeloupe RGAF09
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_gtbt.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RAGTBT2016.mnt',

    'RAGTBT2016.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RAGTBT2016.mnt',

    # Les Saintes RGAF09
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_ls.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RALS2016.mnt',

    'RALS2016.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RALS2016.mnt',

    # Martinique RGAF09
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_mart.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RAMART2016.mnt',

    'RAMART2016.MNT':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RAMART2016.mnt',

    # Marie Galante RGAF09
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_mg.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RAMG2016.mnt',

    'RAMG2016.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RAMG2016.mnt',

    # Saint Barthelemy RGAF09
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_sb.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_sbv2.mnt',

    'gg10_sbv2.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_sbv2.mnt',

    # Saint Martin RGAF09
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_sm.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_smv2.mnt',

    'gg10_smv2.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_smv2.mnt',

    # La Desirade RGAF09
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/gg10_ld.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RALD2016.mnt',

    'RALD2016.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RALD2016.mnt',


    # Guadeloupe WGS84
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00.txt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00v2.mnt',

    # Les Saintes WGS84
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_ls.txt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_lsv2.mnt',

    'ggg00_lsv2.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_lsv2.mnt',

    # Martinique WGS84
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggm00.txt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggm00v2.mnt',

    # Saint Barthelemy WGS84
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_sb.txt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_sbv2.mnt',

    # Saint Martin WGS84
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_sm.txt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_smv2.mnt',

    # La Desirade WGS84
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggg00_ld.txt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RALDW842016.mnt',

    'RALDW842016.mnt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RALDW842016.mnt',


    # Guyane RGF95
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggguy00.txt':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/ggguy15.mnt',


    # Reunion grille RAR
    'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/RAR07_bl.gra':
        'http://geodesie.ign.fr/contenu/fichiers/documentation/grilles/outremer/RAR07_bl.gra',
}

setVerticalGrids = set()

for node in root.iterfind('.//Transformation'):
    id = node.attrib['id']
    names = [_name.text for _name in node.iter('name')]
    name = names[0]

    sourceCRS = extract_id_from_href(node.find('sourceCRS').attrib['href'])
    if not sourceCRS in mapCrsId:
        print('Skipping ' + name + ', missing sourceCRS')
        continue

    targetCRS = node.find('targetCRS')
    if targetCRS is None or 'href' not in targetCRS.attrib:
        print('Skipping ' + name + ', missing targetCRS')
        continue
    targetCRS = extract_id_from_href(targetCRS.attrib['href'])
    if not targetCRS in mapCrsId:
        print('Skipping ' + name + ', missing targetCRS')
        continue

    operation_version = node.find('operationVersion').text

    extent_auth_and_code = get_extent_auth_name_code(node.find('domainOfValidity'))
    scope_auth_and_code = get_scope_auth_name_code(node.find('scope').text)

    usesMethod = extract_id_from_href(node.find('usesMethod').attrib['href'])
    if usesMethod in ('Geographic3DtoGravityRelatedHeight_IGN'):

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','grid_transformation');""" % id
        #all_sql.append(sql)

        usesValue = node.find('usesValue')
        paramValue = usesValue.find('ParameterValue')
        filename = paramValue.find('valueFile').text

        if filename in mapGridURLs:
            print('Fixing URL of ' + filename + ' to ' + mapGridURLs[filename])
            filename = mapGridURLs[filename]

        if not filename.endswith('RAF09.mnt') and not filename.endswith('ggspm06v1.mnt'): # no longer available
            r = requests.head(filename, allow_redirects = True  )
            if r.status_code not in (200, 302):
                assert False, (r.status_code, id, name, filename)

        setVerticalGrids.add(filename)

        assert sourceCRS in mapVerticalCrsId, (id, name, sourceCRS)
        assert targetCRS in mapGeographicId, (id, name, targetCRS)

        # Switching source and target to be consistent with the EPSG practice and the naming of the method

        name_components = name.split(' vers ')
        name_inverted = name_components[1] + ' vers ' + name_components[0]

        sql = """INSERT INTO "grid_transformation" VALUES('IGNF','%s','%s',NULL,'EPSG','9664','Geographic3D to GravityRelatedHeight (IGN1997)','%s','%s','%s','%s',NULL,'EPSG','8666','Geoid (height correction) model file','%s',NULL,NULL,NULL,NULL,NULL,NULL,'%s',0);""" % (id, name_inverted, mapCrsId[targetCRS][0], mapCrsId[targetCRS][1], mapCrsId[sourceCRS][0], mapCrsId[sourceCRS][1], filename, operation_version)
        all_sql.append(sql)
        sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','grid_transformation','IGNF','%s','%s','%s','%s','%s');""" % (id, id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
        all_sql.append(sql)

        continue


    def get_alias_of(code):
        if code in aliasOfCRS:
            return [ ('IGNF', code) ] + aliasOfCRS[code]
        return [ ('IGNF', code) ]



    if id == 'TSG1240': # 'NTF geographiques Paris (gr) vers NTF GEOGRAPHIQUES GREENWICH (DMS)', 'from1Dto1D')
        #print('Skipping ' + str((id, name)))
        assert usesMethod == 'from1Dto1D', usesMethod

        for src in get_alias_of(sourceCRS):
            for target in get_alias_of(targetCRS):

                custom_id = id
                if not ((src == ('IGNF', sourceCRS) and target == ('IGNF', targetCRS))):
                    custom_id = id + '_' + src[0] + '_' + src[1] + '_TO_' + target[0] + '_' + target[1]

                #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','other_transformation');""" % custom_id
                #all_sql.append(sql)

                sql = """INSERT INTO "other_transformation" VALUES('IGNF','%s','%s',NULL,'EPSG','9601','Longitude rotation','%s','%s','%s','%s',0.0,'EPSG','8602','Longitude offset',2.5969213,'EPSG','9105',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'%s',0);"""% (custom_id, name, src[0], src[1], target[0], target[1], operation_version)
                all_sql.append(sql)
                sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','other_transformation','IGNF','%s','%s','%s','%s','%s');""" % (custom_id, custom_id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
                all_sql.append(sql)

        continue

    if usesMethod == 'TSGM510': # geocentric interpolation

        id = 'NTFG_TO_RGF93G'

        assert sourceCRS == 'NTF'
        assert targetCRS == 'RGF93'

        sourceCRS = 'NTFG'
        targetCRS = 'RGF93G'

        for src in get_alias_of(sourceCRS):
            # As the transformation from RGF93G to WGS84 is a zero-translation helmert,
            # we can also use the grid for NTF->WGS84. This makes the coordinate
            # operation finder happier
            for target in get_alias_of(targetCRS) + [('EPSG','4326')]:

                custom_id = id
                if not ((src == ('IGNF', sourceCRS) and target == ('IGNF', targetCRS))):
                    custom_id = src[0] + '_' + src[1] + '_TO_' + target[0] + '_' + target[1]

                #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','grid_transformation');""" % (custom_id)
                #all_sql.append(sql)

                sql = """INSERT INTO "grid_transformation" VALUES('IGNF','%s','%s',NULL,'EPSG','9615','NTv2','%s','%s','%s','%s',NULL,'EPSG','8656','Latitude and longitude difference file','ntf_r93.gsb',NULL,NULL,NULL,NULL,NULL,NULL,'%s',0);""" % (custom_id, name, src[0], src[1], target[0], target[1], operation_version)
                all_sql.append(sql)
                sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','grid_transformation','IGNF','%s','%s','%s','%s','%s');""" % (custom_id, custom_id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
                all_sql.append(sql)

        continue

    if usesMethod == 'Vfrom1Dto1D':

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','other_transformation');""" % id
        #all_sql.append(sql)

        usesValue = node.find('usesValue')
        paramValue = usesValue.find('ParameterValue')
        value = paramValue.find('value').text
        uom = paramValue.find('value').attrib['uom']
        assert uom == 'm'

        sql = """INSERT INTO "other_transformation" VALUES('IGNF','%s','%s',NULL,'EPSG','9616','Vertical Offset','%s','%s','%s','%s',NULL,'EPSG','8603','Vertical Offset',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'%s',0);"""% (id, name, mapCrsId[sourceCRS][0], mapCrsId[sourceCRS][1], mapCrsId[targetCRS][0], mapCrsId[targetCRS][1], value, operation_version)
        all_sql.append(sql)
        sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','other_transformation','IGNF','%s','%s','%s','%s','%s');""" % (id, id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
        all_sql.append(sql)

        continue

    assert usesMethod in ('TSGM120', 'TSGM110', 'TSGM112', 'TSGM111'), (id, name, usesMethod)

    assert sourceCRS in mapGeocentricId
    assert targetCRS in mapGeocentricId

    vals =[val for val in node.iterfind('usesValue')]
    assert len(vals) in (3,7)
    x = vals[0].find('ParameterValue').find('value').text
    assert vals[0].find('ParameterValue').find('value').attrib['uom'] == 'm'
    y = vals[1].find('ParameterValue').find('value').text
    assert vals[1].find('ParameterValue').find('value').attrib['uom'] == 'm'
    z = vals[2].find('ParameterValue').find('value').text
    assert vals[2].find('ParameterValue').find('value').attrib['uom'] == 'm'

    if len(vals) == 3:
        rx = 'NULL'
        ry = 'NULL'
        rz = 'NULL'
        s = 'NULL'
        r_uom_auth_name = 'NULL'
        r_uom_code = 'NULL'
        s_uom_auth_name = 'NULL'
        s_uom_code = 'NULL'

        method_code = "'1031'"
        method_name = "'Geocentric translations (geocentric domain)'"

        method_geog_code = "'9603'"
        method_geog_name = "'Geocentric translations (geog2D domain)'"

    else:
        s = vals[3].find('ParameterValue').find('value').text
        assert vals[3].find('ParameterValue').find('value').attrib['uom'] == 'UNITE'

        rx = vals[4].find('ParameterValue').find('value').text
        assert vals[4].find('ParameterValue').find('value').attrib['uom'] == 'sec'

        ry = vals[5].find('ParameterValue').find('value').text
        assert vals[5].find('ParameterValue').find('value').attrib['uom'] == 'sec'

        rz = vals[6].find('ParameterValue').find('value').text
        assert vals[6].find('ParameterValue').find('value').attrib['uom'] == 'sec'

        r_uom_auth_name = "'EPSG'"
        r_uom_code = "'9104'"
        s_uom_auth_name = "'EPSG'"
        s_uom_code = "'9202'"

        method_code = "'1033'"
        method_name = "'Position Vector transformation (geocentric domain)'"

        method_geog_code = "'9606'"
        method_geog_name = "'Position Vector transformation (geog2D domain)'"

    for src in get_alias_of(sourceCRS):
        for target in get_alias_of(targetCRS):

            custom_id = id
            if not ((src == ('IGNF', sourceCRS) and target == ('IGNF', targetCRS))):
                custom_id += '_' + src[1] + '_' + target[1]

            #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','helmert_transformation');""" % (custom_id)
            #all_sql.append(sql)

            sql = """INSERT INTO "helmert_transformation" VALUES('IGNF','%s','%s',NULL,'EPSG',%s,%s,'%s','%s','%s','%s',NULL,%s,%s,%s,'EPSG','9001',%s,%s,%s,%s,%s, %s,%s,%s,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'%s',0);""" % (custom_id, name, method_code, method_name, src[0], src[1], target[0], target[1], x, y, z, rx, ry, rz, r_uom_auth_name, r_uom_code, s, s_uom_auth_name, s_uom_code, operation_version)
            all_sql.append(sql)
            sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','helmert_transformation','IGNF','%s','%s','%s','%s','%s');""" % (custom_id, custom_id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
            all_sql.append(sql)


    key = mapGeocentricIdToDatumAndArea[sourceCRS]
    assert key in mapDatumAndAreaToGeographicId
    sourceGeogIdAr = mapDatumAndAreaToGeographicId[key]

    key = mapGeocentricIdToDatumAndArea[targetCRS]
    assert key in mapDatumAndAreaToGeographicId
    targetGeogIdAr = mapDatumAndAreaToGeographicId[key]

    for sourceGeogId in sourceGeogIdAr:
        for targetGeogId in targetGeogIdAr:

            for src in get_alias_of(sourceGeogId):
                for target in get_alias_of(targetGeogId):

                    id_geog = id + '_' + src[1] + '_TO_' + target[1]
                    #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','helmert_transformation');""" % (id_geog)
                    #all_sql.append(sql)

                    sql = """INSERT INTO "helmert_transformation" VALUES('IGNF','%s','%s',NULL,'EPSG',%s,%s,'%s','%s','%s','%s',NULL,%s,%s,%s,'EPSG','9001',%s,%s,%s,%s,%s,%s,%s, %s,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'%s',0);""" % (id_geog, name, method_geog_code, method_geog_name, src[0], src[1], target[0], target[1], x, y, z, rx, ry, rz, r_uom_auth_name, r_uom_code, s, s_uom_auth_name, s_uom_code, operation_version)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','helmert_transformation','IGNF','%s','%s','%s','%s','%s');""" % (id_geog, id_geog, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
                    all_sql.append(sql)

                    if src[1] == 'NTFG':

                        for NTFPalias, idFirstOp in (('NTFPGRAD', 'TSG1240'), ('NTFP', 'TSG1240_IGNF_NTFP_TO_IGNF_NTFG')):

                            id_concat = id + '_' + NTFPalias + '_TO_' + target[1]

                            #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','concatenated_operation');""" % (id_concat)
                            #all_sql_concat.append(sql)

                            sql = """INSERT INTO "concatenated_operation" VALUES('IGNF','%s','Nouvelle Triangulation Francaise Paris grades to %s',NULL,'IGNF','%s','%s','%s',NULL,'%s',0);""" % (id_concat, target[1],  NTFPalias, target[0], target[1], operation_version)
                            all_sql_concat.append(sql)
                            sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','concatenated_operation','IGNF','%s','%s','%s','%s','%s');""" % (id_concat, id_concat, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
                            all_sql_concat.append(sql)

                            sql = """INSERT INTO "concatenated_operation_step" VALUES('IGNF','%s',1,'IGNF','%s');""" % (id_concat, idFirstOp)
                            all_sql_concat.append(sql)

                            sql = """INSERT INTO "concatenated_operation_step" VALUES('IGNF','%s',2,'IGNF','%s');""" % (id_concat, id_geog)
                            all_sql_concat.append(sql)


mapConversionId = {}


def getParameter(node, code, expected_uom):
    for val in node.iterfind('usesValue'):
        parameter_code = extract_id_from_href(val.find('ParameterValue').find('valueOfParameter').attrib['href'])
        if parameter_code == code:

            dms = val.find('ParameterValue').find('dmsAngleValue')
            if expected_uom == 'deg' and dms is not None:
                deg_val = float(dms.find('degrees').text)
                direction_deg = dms.find('degrees').attrib['direction']
                assert direction_deg in ('E', 'W', 'S', 'N')
                min_val = float(dms.find('minutes').text)
                if dms.find('secondes') is not None:
                    sec_val = float(dms.find('secondes').text)
                else:
                    sec_val = 0

                ret_val = deg_val + min_val / 60.0 + sec_val / 3600.0
                if direction_deg in ('W', 'S'):
                    ret_val = -ret_val
                return ret_val

            assert val.find('ParameterValue').find('value').attrib['uom'] == expected_uom
            return float(val.find('ParameterValue').find('value').text)
    raise Exception('cannot find value for parameter ' + code)

for node in root.iterfind('.//Conversion'):
    id = node.attrib['id']
    names = [_name.text for _name in node.iter('name')]
    name = names[0]
    #print(id, name)

    reuse_epsg_conversion = False ###
    if reuse_epsg_conversion and len(names) == 2:

        if id == 'PRC9601581': # PSEUDO MERCATOR (POPULAR VISUALISATION)
            assert get_epsg_code(names[1]) == '3857'  # this is wrong, this is the projectedCRS code, note the conversion one
            mapConversionId[id] = ('EPSG', '3856')
        else:
            mapConversionId[id] = ('EPSG', get_epsg_code(names[1]))
        continue

    usesMethod = extract_id_from_href(node.find('usesMethod').attrib['href'])

    d = {}

    if usesMethod == 'PVPM001From2Dto2D': # Popular Visualisation Pseudo-Mercator

        assert len([1 for val in node.iterfind('usesValue')]) == 4
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        assert d['x_0'] == 0
        assert d['y_0'] == 0
        assert d['lon_0'] == 0
        assert d['lat_0'] == 0

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','1024','Popular Visualisation Pseudo Mercator','EPSG','8801','Latitude of natural origin',0.0,'EPSG','9102','EPSG','8802','Longitude of natural origin',0.0,'EPSG','9102','EPSG','8806','False easting',0.0,'EPSG','9001','EPSG','8807','False northing',0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name)
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod == 'EQRC001from2Dto2D': # Equirectangular

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        d['lat_ts'] = getParameter(node, 'PRCP600', 'deg')
        assert d['lat_0'] == 0, (id, name, d)

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','1028','Equidistant Cylindrical','EPSG','8823','Latitude of 1st standard parallel',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_ts'], d['lon_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod in ('PRCM030from2Dto2D', 'PRCM020from2Dto2D', 'PRCM040from2Dto2D', 'PRCM030from3Dto2D'): # Transverse Mercator

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        d['k_0'] = getParameter(node, 'PRCP500', 'UNITE')

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9807','Transverse Mercator','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod == 'PRCM060from2Dto2D': # Bonne

        assert len([1 for val in node.iterfind('usesValue')]) == 6
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'gr')
        d['lat_0'] = getParameter(node, 'PRCP400', 'gr')
        d['k_0'] = getParameter(node, 'PRCP500', 'UNITE')
        d['lat_1'] = getParameter(node, 'PRCP600', 'gr')
        assert d['lat_0'] == d['lat_1']

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9827','Bonne','EPSG','8801','Latitude of natural origin',%s,'EPSG','9105','EPSG','8802','Longitude of natural origin',%s,'EPSG','9105','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod == 'PRCM015from2Dto2D': # LAEA

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        d['k_0'] = getParameter(node, 'PRCP500', 'UNITE')
        assert d['k_0'] == 1

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9820','Lambert Azimuthal Equal Area','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod == 'PRCM013from2Dto2D': # LCC_2SP

        assert len([1 for val in node.iterfind('usesValue')]) == 6
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        d['lat_1'] = getParameter(node, 'PRCP600', 'deg')
        d['lat_2'] = getParameter(node, 'PRCP700', 'deg')

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9802','Lambert Conic Conformal (2SP)','EPSG','8821','Latitude of false origin',%s,'EPSG','9102','EPSG','8822','Longitude of false origin',%s,'EPSG','9102','EPSG','8823','Latitude of 1st standard parallel',%s,'EPSG','9102','EPSG','8824','Latitude of 2nd standard parallel',%s,'EPSG','9102','EPSG','8826','Easting at false origin',%s,'EPSG','9001','EPSG','8827','Northing at false origin',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['lat_1'], d['lat_2'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)


    elif usesMethod == 'PRCM070from2Dto2D': # Mercator (variant A)

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        d['k_0'] = getParameter(node, 'PRCP500', 'UNITE')

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9804','Mercator (variant A)','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)


    elif usesMethod in ('PRCM012from2Dto2D', 'PRCM012from3Dto2D'): # LCC_1SP

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'gr')
        d['lat_0'] = getParameter(node, 'PRCP400', 'gr')
        d['k_0'] = getParameter(node, 'PRCP500', 'UNITE')

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9801','Lambert Conic Conformal (1SP)','EPSG','8801','Latitude of natural origin',%s,'EPSG','9105','EPSG','8802','Longitude of natural origin',%s,'EPSG','9105','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)


    elif usesMethod in ('PRCM014from2Dto2D'): # LCC_1SP

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP110', 'm')
        d['y_0'] = getParameter(node, 'PRCP210', 'm')
        d['lon_0'] = getParameter(node, 'PRCP310', 'gr')
        d['lat_0'] = getParameter(node, 'PRCP410', 'gr')
        d['k_0'] = getParameter(node, 'PRCP510', 'UNITE')

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9801','Lambert Conic Conformal (1SP)','EPSG','8801','Latitude of natural origin',%s,'EPSG','9105','EPSG','8802','Longitude of natural origin',%s,'EPSG','9105','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)



    elif usesMethod == 'PRCM053from2Dto2D': # Gauss Schreiber Transverse Mercator

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        d['k_0'] = getParameter(node, 'PRCP500', 'UNITE')

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'PROJ','gstm','Gauss Schreiber Transverse Mercator','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod == 'PRCM094from2Dto2D': # Polar Stereographic

        assert len([1 for val in node.iterfind('usesValue')]) == 5
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        d['k_0'] = getParameter(node, 'PRCP500', 'UNITE')

        assert float(d['lat_0']) == -90
        assert float(d['lon_0']) == 140

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'EPSG','9810','Polar Stereographic (variant A)','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id, name, d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod == 'MILL001from2Dto2D': # Miller

        assert len([1 for val in node.iterfind('usesValue')]) == 4
        d['x_0'] = getParameter(node, 'PRCP100', 'm')
        d['y_0'] = getParameter(node, 'PRCP200', 'm')
        d['lon_0'] = getParameter(node, 'PRCP300', 'deg')
        d['lat_0'] = getParameter(node, 'PRCP400', 'deg')
        assert d['x_0'] == 0
        assert d['y_0'] == 0
        assert d['lon_0'] == 0
        assert d['lat_0'] == 0

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','%s','conversion');""" % (id)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','%s','%s',NULL,'PROJ','mill','PROJ mill',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (id,name)
        all_sql.append(sql)

        mapConversionId[id] = ('IGNF', id)

    elif usesMethod == 'PRCM095from2Dto2D': 
        print('Unhandled conversion PRCM095from2Dto2D = Polar Sterographic (Variant C) %s' % (str((id, name, usesMethod))))
        continue

    else:
        print('Unknown conversion %s' % (str((id, name, usesMethod))))
        assert False

mapProjectedId = {}

for node in root.iterfind('.//ProjectedCRS'):
    id = node.attrib['id']
    names = [_name.text for _name in node.iter('name')]
    name = names[0]

    usesCartesianCS = extract_id_from_href(node.find('usesCartesianCS').attrib['href'])
    assert usesCartesianCS in ('TYP_CRG32', 'TYP_CRG70', 'TYP_CRG34'), (id, name, usesCartesianCS)
    baseGeographicCRS = extract_id_from_href(node.find('baseGeographicCRS').attrib['href'])
    if baseGeographicCRS not in mapGeographicId:
        print('Skipping ProjectedCRS %s since its baseGeographicCRS %s is unknown' % (id, baseGeographicCRS))
        continue

    definedByConversion = extract_id_from_href(node.find('definedByConversion').attrib['href'])
    if definedByConversion in ('PRC0909577'):
        print('Skipping ProjectedCRS %s since its definedByConversion %s is unhandled' % (id, definedByConversion))
        continue
    assert definedByConversion in mapConversionId, (id, name, definedByConversion)

    extent_auth_and_code = get_extent_auth_name_code(node.find('domainOfValidity'))
    scope = node.find('scope').text
    scope_auth_and_code = get_scope_auth_name_code(scope)

    #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (id, )
    #all_sql.append(sql)

    cs_code = 4499  # TYP_CRG32
    if usesCartesianCS == 'TYP_CRG70':
        cs_code = 4400
    if usesCartesianCS == 'TYP_CRG34':
        cs_code = 4530

    sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,'EPSG','%s','%s','%s','%s','%s',NULL,0);""" % (id,name,cs_code,mapGeographicId[baseGeographicCRS][0], mapGeographicId[baseGeographicCRS][1],mapConversionId[definedByConversion][0], mapConversionId[definedByConversion][1])
    all_sql.append(sql)
    sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','projected_crs','IGNF','%s','%s','%s','%s','%s');""" % (id, id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
    all_sql.append(sql)

    if len(names) >= 2 and names[1].startswith('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):
        alias = names[1][len('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):]
        aliasOfCRS[id] = [('IGNF', alias)]

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected'); -- alias of %s""" % (alias, id)
        #all_sql.append(sql)

        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,'EPSG','%s','%s','%s','%s','%s',NULL,0);""" % (alias,name,cs_code,mapGeographicId[baseGeographicCRS][0], mapGeographicId[baseGeographicCRS][1],mapConversionId[definedByConversion][0], mapConversionId[definedByConversion][1])
        all_sql.append(sql)
        sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','projected_crs','IGNF','%s','%s','%s','%s','%s');""" % (alias, alias, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
        all_sql.append(sql)

    mapProjectedId[id] = ('IGNF', id)



for node in root.iterfind('.//CompoundCRS'):
    id = node.attrib['id']
    names = [_name.text for _name in node.iter('name')]
    name = names[0]

    singleCRS = [extract_id_from_href(includesSingleCRS.attrib['href']) for includesSingleCRS in node.iter('includesSingleCRS')]
    assert len(singleCRS) == 2

    if singleCRS[0] == 'RGWF96GEO':
        singleCRS[0] = 'RGWF96G'

    assert singleCRS[0] in mapProjectedId or singleCRS[0] in mapGeographicId, (id, name)
    assert singleCRS[1] in mapVerticalCrsId, (id, name, singleCRS[1])

    if singleCRS[0] in mapProjectedId:
        horiz = mapProjectedId[singleCRS[0]]
    else:
        horiz = mapGeographicId[singleCRS[0]]

    extent_auth_and_code = get_extent_auth_name_code(node.find('domainOfValidity'))
    scope = node.find('scope').text
    scope_auth_and_code = get_scope_auth_name_code(scope)

    #sql = """INSERT INTO "crs" VALUES('IGNF','%s','compound');""" % (id, )
    #all_sql.append(sql)

    sql = """INSERT INTO "compound_crs" VALUES('IGNF','%s','%s',NULL,'%s','%s','%s','%s',0);""" % (id,name,horiz[0], horiz[1],mapVerticalCrsId[singleCRS[1]][0], mapVerticalCrsId[singleCRS[1]][1])
    all_sql.append(sql)
    sql = """INSERT INTO "usage" VALUES('IGNF', '%s_USAGE','compound_crs','IGNF','%s','%s','%s','%s','%s');""" % (id, id, extent_auth_and_code[0], extent_auth_and_code[1], scope_auth_and_code[0], scope_auth_and_code[1])
    all_sql.append(sql)

    if len(names) >= 2 and names[1].startswith('http://registre.ign.fr/ign/IGNF/crs/IGNF/'):
        assert False


all_sql.append('')
all_sql.append("""--- Grid alternatives""")
all_sql.append('')
all_sql.append("""INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              old_proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ntf_r93.gsb',    -- as referenced by the IGNF registry
                              'fr_ign_ntf_r93.tif',
                              'ntf_r93.gsb',
                              'GTiff',
                              'hgridshift',
                              0,
                              NULL,
                              'https://cdn.proj.org/fr_ign_ntf_r93.tif', 1, 1, NULL);
""")

for grid in setVerticalGrids:

    original_grid_name = grid
    old_proj_grid_name = grid[grid.rfind('/')+1:].replace('.txt', '.gtx').replace('.mnt', '.gtx').replace('.gra', '.gtx')
    gtiff_grid_name = 'fr_ign_' + old_proj_grid_name[0:-4] + '.tif'

    all_sql.append("""INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            old_proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('%s',    -- as referenced by the IGNF registry
                            '%s',
                            '%s',
                            'GTiff',
                            'geoid_like',
                            0,
                            NULL,
                            '%s', 1, 1, NULL);""" % (original_grid_name, gtiff_grid_name, old_proj_grid_name, 'https://cdn.proj.org/' + gtiff_grid_name))


all_sql.append('')
all_sql.append("""--- Null transformations between RRAF and WGS84 adapted from EPSG""")
all_sql.append('')

area_of_use_name = 'ANTILLES FRANCAISES'
assert area_of_use_name in extentMap
extent_auth_and_code = extentMap[area_of_use_name]

all_sql.append("""INSERT INTO "helmert_transformation" VALUES('PROJ','IGNF_RRAF_TO_EPSG_4978','RRAF to WGS 84',NULL,'EPSG','1031','Geocentric translations (geocentric domain)','IGNF','RRAF','EPSG','4978',1.0,0.0,0.0,0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""")
sql = """INSERT INTO "usage" VALUES('PROJ', '%s_USAGE','helmert_transformation','PROJ','%s','%s','%s','%s','%s');""" % ('IGNF_RRAF_TO_EPSG_4978', 'IGNF_RRAF_TO_EPSG_4978', extent_auth_and_code[0], extent_auth_and_code[1], 'EPSG', '1024')
all_sql.append(sql)

all_sql.append("""INSERT INTO "helmert_transformation" VALUES('PROJ','IGNF_RRAFG_TO_EPSG_4326','RRAFG to WGS 84',NULL,'EPSG','9603','Geocentric translations (geog2D domain)','IGNF','RRAFG','EPSG','4326',1.0,0.0,0.0,0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""")
sql = """INSERT INTO "usage" VALUES('PROJ', '%s_USAGE','helmert_transformation','PROJ','%s','%s','%s','%s','%s');""" % ('IGNF_RRAFG_TO_EPSG_4326', 'IGNF_RRAFG_TO_EPSG_4326', extent_auth_and_code[0], extent_auth_and_code[1], 'EPSG', '1024')
all_sql.append(sql)

all_sql.append("""INSERT INTO "helmert_transformation" VALUES('PROJ','IGNF_RRAFGDD_TO_EPSG_4326','RRAFGDD to WGS 84',NULL,'EPSG','9603','Geocentric translations (geog2D domain)','IGNF','RRAFGDD','EPSG','4326',1.0,0.0,0.0,0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""")
sql = """INSERT INTO "usage" VALUES('PROJ', '%s_USAGE','helmert_transformation','PROJ','%s','%s','%s','%s','%s');""" % ('IGNF_RRAFGDD_TO_EPSG_4326', 'IGNF_RRAFGDD_TO_EPSG_4326', extent_auth_and_code[0], extent_auth_and_code[1], 'EPSG', '1024')
all_sql.append(sql)

all_sql.append('')
all_sql.append("""--- Null transformations between RGF93 and WGS84 adapted from EPSG""")
all_sql.append('')

area_of_use_name = 'FRANCE METROPOLITAINE (CORSE COMPRISE)'
assert area_of_use_name in extentMap
extent_auth_and_code = extentMap[area_of_use_name]

all_sql.append("""INSERT INTO "helmert_transformation" VALUES('PROJ','IGNF_RGF93_TO_EPSG_4978','RGF93 to WGS 84',NULL,'EPSG','1031','Geocentric translations (geocentric domain)','IGNF','RGF93','EPSG','4978',1.0,0.0,0.0,0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""")
sql = """INSERT INTO "usage" VALUES('PROJ', '%s_USAGE','helmert_transformation','PROJ','%s','%s','%s','%s','%s');""" % ('IGNF_RGF93_TO_EPSG_4978', 'IGNF_RGF93_TO_EPSG_4978', extent_auth_and_code[0], extent_auth_and_code[1], 'EPSG', '1024')
all_sql.append(sql)

all_sql.append("""INSERT INTO "helmert_transformation" VALUES('PROJ','IGNF_RGF93G_TO_EPSG_4326','RGF93G to WGS 84',NULL,'EPSG','9603','Geocentric translations (geog2D domain)','IGNF','RGF93G','EPSG','4326',1.0,0.0,0.0,0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""")
sql = """INSERT INTO "usage" VALUES('PROJ', '%s_USAGE','helmert_transformation','PROJ','%s','%s','%s','%s','%s');""" % ('IGNF_RGF93G_TO_EPSG_4326', 'IGNF_RGF93G_TO_EPSG_4326', extent_auth_and_code[0], extent_auth_and_code[1], 'EPSG', '1024')
all_sql.append(sql)

all_sql.append("""INSERT INTO "helmert_transformation" VALUES('PROJ','IGNF_RGF93GDD_TO_EPSG_4326','RGF93GDD to WGS 84',NULL,'EPSG','9603','Geocentric translations (geog2D domain)','IGNF','RGF93GDD','EPSG','4326',1.0,0.0,0.0,0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""")
sql = """INSERT INTO "usage" VALUES('PROJ', '%s_USAGE','helmert_transformation','PROJ','%s','%s','%s','%s','%s');""" % ('IGNF_RGF93GDD_TO_EPSG_4326', 'IGNF_RGF93GDD_TO_EPSG_4326', extent_auth_and_code[0], extent_auth_and_code[1], 'EPSG', '1024')
all_sql.append(sql)

script_dir_name = os.path.dirname(os.path.realpath(__file__))
sql_dir_name = os.path.join(os.path.dirname(script_dir_name), 'data', 'sql')

f = open(os.path.join(sql_dir_name, 'ignf') + '.sql', 'wb')
f.write("--- This file has been generated by scripts/build_db_create_ignf_from_xml.py from the http://librairies.ign.fr/geoportail/resources/IGNF.xml definition file. DO NOT EDIT !\n\n".encode('UTF-8'))
for sql in all_sql:
    f.write((sql + '\n').encode('UTF-8'))

comment = []
comment.append('')
comment.append("""--- Concatenated operations""")
comment.append('')

for sql in comment:
    f.write((sql + '\n').encode('UTF-8'))

for sql in all_sql_concat:
    f.write((sql + '\n').encode('UTF-8'))

f.close()


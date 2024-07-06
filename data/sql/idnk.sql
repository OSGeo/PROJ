-- Self-appointed authority, originating from https://talks.osgeo.org/media/foss4g-europe-2024/submissions/B8CETX/resources/WGS_84_I_dont_know_I_dont_care_pTbpl9n.pdf

-- Geodetic CRS

INSERT INTO "geodetic_datum" VALUES('IDNK','DATUM','Unknown geodetic datum','Datum to be used when the actual datum is unknown, instead of misusing WGS84','EPSG','7019','EPSG','8901',NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('IDNK','DATUM_USAGE','geodetic_datum','IDNK','DATUM','PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN');

INSERT INTO "geodetic_crs" VALUES('IDNK','GEOGRAPHIC','Unknown geographic 2D CRS','Geographic CRS to be used when the actual datum is unknown, instead of misusing WGS84','geographic 2D','EPSG','6424','IDNK','DATUM',NULL,0);
INSERT INTO "usage" VALUES(
    'IDNK',
    'GEOGRAPHIC_USAGE',
    'geodetic_crs',
    'IDNK',
    'GEOGRAPHIC',
    'PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN');

INSERT INTO "geodetic_crs" VALUES('IDNK','GEOGRAPHIC_3D','Unknown geographic 3D CRS','Geographic 3D CRS to be used when the actual datum is unknown, instead of misusing WGS84','geographic 3D','EPSG','6423','IDNK','DATUM',NULL,0);
INSERT INTO "usage" VALUES(
    'IDNK',
    'GEOGRAPHIC_3D_USAGE',
    'geodetic_crs',
    'IDNK',
    'GEOGRAPHIC_3D',
    'PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN');

INSERT INTO "geodetic_crs" VALUES('IDNK','GEOCENTRIC','Unknown geocentric CRS','Geocentric CRS to be used when the actual datum is unknown, instead of misusing WGS84','geocentric','EPSG','6500','IDNK','DATUM',NULL,0);
INSERT INTO "usage" VALUES(
    'IDNK',
    'GEOCENTRIC_USAGE',
    'geodetic_crs',
    'IDNK',
    'GEOCENTRIC',
    'PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN');

-- UTM zones

WITH RECURSIVE zone_counter (zone)
AS (SELECT 1 UNION ALL SELECT zone+1 AS new_zone FROM zone_counter WHERE new_zone <= 60)
INSERT INTO "projected_crs" SELECT 'IDNK','UTM_' || zone || 'N','Unknown / UTM zone ' || zone || 'N','UTM based CRS to be used when the actual datum is unknown, instead of misusing WGS84','EPSG','4400','IDNK','GEOGRAPHIC','EPSG',16000+zone,NULL,0 FROM zone_counter;

WITH RECURSIVE zone_counter (zone)
AS (SELECT 1 UNION ALL SELECT zone+1 AS new_zone FROM zone_counter WHERE new_zone <= 60)
INSERT INTO "usage" SELECT 'IDNK','UTM_' || zone || 'N_USAGE','projected_crs','IDNK','UTM_' || zone || 'N','PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN' FROM zone_counter;

WITH RECURSIVE zone_counter (zone)
AS (SELECT 1 UNION ALL SELECT zone+1 AS new_zone FROM zone_counter WHERE new_zone <= 60)
INSERT INTO "projected_crs" SELECT 'IDNK','UTM_' || zone || 'S','Unknown / UTM zone ' || zone || 'S','UTM based CRS to be used when the actual datum is unknown, instead of misusing WGS84','EPSG','4400','IDNK','GEOGRAPHIC','EPSG',16100+zone,NULL,0 FROM zone_counter;

WITH RECURSIVE zone_counter (zone)
AS (SELECT 1 UNION ALL SELECT zone+1 AS new_zone FROM zone_counter WHERE new_zone <= 60)
INSERT INTO "usage" SELECT 'IDNK','UTM_' || zone || 'S_USAGE','projected_crs','IDNK','UTM_' || zone || 'S','PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN' FROM zone_counter;

-- Vertical CRS

INSERT INTO "vertical_datum" VALUES('IDNK','VERTICAL_DATUM','Unknown vertical datum',NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('IDNK','VERTICAL_DATUM_USAGE','vertical_datum','IDNK','VERTICAL_DATUM','PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN');

INSERT INTO "vertical_crs" VALUES('IDNK','VERTICAL','Unknown vertical CRS',NULL,'EPSG','6499','IDNK','VERTICAL_DATUM',0);
INSERT INTO "usage" VALUES('IDNK','VERTICAL','vertical_crs','IDNK','VERTICAL','PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN');

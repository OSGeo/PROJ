-- This file is hand generated.

INSERT INTO "geodetic_crs" VALUES('OGC','CRS84','WGS 84 (CRS84)',NULL,NULL,'geographic 2D','EPSG','6424','EPSG','6326','EPSG','1262',NULL,0);

INSERT INTO "other_transformation" VALUES('PROJ','CRS84_TO_EPSG_4326','OGC:CRS84 to WGS 84',NULL,NULL,'EPSG','9843','Axis Order Reversal (2D)','OGC','CRS84','EPSG','4326','EPSG','1262',0.0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

-- alias of EPSG:3857
INSERT INTO "projected_crs" VALUES('EPSG','900913','Google Maps Global Mercator',NULL,NULL,'EPSG','4499','EPSG','4326','EPSG','3856','EPSG','3544',NULL,1);

-- Remove supression entry of EPSG:8371 ("RGF93 to NGF IGN69 height (2)", using RAF09.mnt) by
-- EPSG:8885 ("RGF93 to NGF IGN69 height (3)", using RAF18.tac), since
-- we have only RAF09.mnt in proj-datumgrid-europe
-- Our code to remove superseded operations should probably take into account grid availability
DELETE FROM "supersession" WHERE superseded_table_name = 'grid_transformation' AND
                                 superseded_auth_name = 'EPSG' AND
                                 superseded_code = '8371' AND
                                 replacement_table_name = 'grid_transformation' AND
                                 replacement_auth_name = 'EPSG' AND
                                 replacement_code = '8885' AND
                                 source = 'EPSG';

-- Define the allowed authorities, and their precedence, when researching a
-- coordinate operation

INSERT INTO authority_to_authority_preference(source_auth_name, target_auth_name, allowed_authorities) VALUES
    ('any', 'EPSG', 'PROJ,EPSG,any' );

INSERT INTO authority_to_authority_preference(source_auth_name, target_auth_name, allowed_authorities) VALUES
    ('EPSG', 'EPSG', 'PROJ,EPSG' );

INSERT INTO authority_to_authority_preference(source_auth_name, target_auth_name, allowed_authorities) VALUES
    ('PROJ', 'EPSG', 'PROJ,EPSG' );

INSERT INTO authority_to_authority_preference(source_auth_name, target_auth_name, allowed_authorities) VALUES
    ('IGNF', 'EPSG', 'PROJ,IGNF,EPSG' );

INSERT INTO authority_to_authority_preference(source_auth_name, target_auth_name, allowed_authorities) VALUES
    ('ESRI', 'EPSG', 'PROJ,ESRI,EPSG' );

-- Custom ellipsoids (from proj -le)

INSERT INTO "ellipsoid" VALUES('PROJ','ANDRAE','Andrae 1876 (Denmark, Iceland)',NULL,'PROJ','EARTH',6377104.43,'EPSG','9001',300.0,NULL,0);
INSERT INTO "ellipsoid" VALUES('PROJ','CPM','Comité international des poids et mesures 1799',NULL,'PROJ','EARTH',6375738.7,'EPSG','9001',334.29,NULL,0);
INSERT INTO "ellipsoid" VALUES('PROJ','DELMBR','Delambre 1810 (Belgium)',NULL,'PROJ','EARTH',6376428.0,'EPSG','9001',311.5,NULL,0);
INSERT INTO "ellipsoid" VALUES('PROJ','KAULA','Kaula 1961',NULL,'PROJ','EARTH',6378163.0,'EPSG','9001',298.24,NULL,0);
INSERT INTO "ellipsoid" VALUES('PROJ','LERCH','Lerch 1979',NULL,'PROJ','EARTH',6378139.0,'EPSG','9001',298.257,NULL,0);
INSERT INTO "ellipsoid" VALUES('PROJ','MERIT','MERIT 1983',NULL,'PROJ','EARTH',6378137.0,'EPSG','9001',298.257,NULL,0);
INSERT INTO "ellipsoid" VALUES('PROJ','MPRTS','Maupertius 1738',NULL,'PROJ','EARTH',6397300.0,'EPSG','9001',191.0,NULL,0);
INSERT INTO "ellipsoid" VALUES('PROJ','NEW_INTL','New International 1967',NULL,'PROJ','EARTH',6378157.5,'EPSG','9001',NULL,6356772.2,0);
INSERT INTO "ellipsoid" VALUES('PROJ','WGS60','WGS 60',NULL,'PROJ','EARTH',6378165.0,'EPSG','9001',298.3,NULL,0);

-- Extra ellipsoids from IAU2000 dictionary (see https://github.com/USGS-Astrogeology/GDAL_scripts/blob/master/OGC_IAU2000_WKT_v2/naifcodes_radii_m_wAsteroids_IAU2000.csv)

INSERT INTO "ellipsoid" VALUES('PROJ','EARTH2000','Earth2000',NULL,'PROJ','EARTH',6378140.0,'EPSG','9001',NULL,6356750.0,0);

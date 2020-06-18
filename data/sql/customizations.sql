-- This file is hand generated.

-- grid_alternatives entries created from existing ones

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              old_proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
    SELECT grid_name,
           'au_ga_AUSGeoid98.tif',
           'AUSGeoid98.gtx',
           'GTiff',
           'geoid_like',
           0,
           NULL,
           'https://cdn.proj.org/au_ga_AUSGeoid98.tif', 1, 1, NULL FROM grid_transformation WHERE
                grid_name LIKE '%DAT.htm' AND name LIKE 'GDA94 to AHD height%';


INSERT INTO "geodetic_crs" VALUES('OGC','CRS84','WGS 84 (CRS84)',NULL,NULL,'geographic 2D','EPSG','6424','EPSG','6326','EPSG','1262',NULL,0);

INSERT INTO "other_transformation" VALUES('PROJ','CRS84_TO_EPSG_4326','OGC:CRS84 to WGS 84',NULL,NULL,'EPSG','9843','Axis Order Reversal (2D)','OGC','CRS84','EPSG','4326','EPSG','1262',0.0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

-- Temporary entry for NZGD2000 deformation model
INSERT INTO other_transformation VALUES(
    'PROJ','NZGD2000-20180701','NZGD2000 to ITRF96',
    'New Zealand Deformation Model. Defines the secular model (National Deformation Model) and patches for significant deformation events since 2000', NULL,
    'PROJ', 'PROJString',
    '+proj=pipeline ' ||
        '+step +proj=unitconvert +xy_in=deg +xy_out=rad ' ||
        '+step +proj=axisswap +order=2,1 ' ||
        '+step +proj=defmodel +model=nz_linz_nzgd2000-20180701.json ' ||
        '+step +proj=axisswap +order=2,1 ' ||
        '+step +proj=unitconvert +xy_in=rad +xy_out=deg',
    'EPSG','4959',
    'EPSG','7907',
    'EPSG','1175',
    NULL, --accuracy
    NULL,NULL,NULL,NULL,NULL,NULL, -- param1
    NULL,NULL,NULL,NULL,NULL,NULL, -- param2
    NULL,NULL,NULL,NULL,NULL,NULL, -- param3
    NULL,NULL,NULL,NULL,NULL,NULL, -- param4
    NULL,NULL,NULL,NULL,NULL,NULL, -- param5
    NULL,NULL,NULL,NULL,NULL,NULL, -- param6
    NULL,NULL,NULL,NULL,NULL,NULL, -- param7
    '20180701', -- operation version
    0);

INSERT INTO "concatenated_operation" VALUES('PROJ','NZGD2000_TO_ITRF97','NZGD2000 to ITRF97','Concatenation of PROJ:NZGD2000-20180701 and 9079','','EPSG','4959','EPSG','7908','EPSG','1175',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF97',1,'PROJ','NZGD2000-20180701');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF97',2,'EPSG','9079');

INSERT INTO "concatenated_operation" VALUES('PROJ','NZGD2000_TO_ITRF2000','NZGD2000 to ITRF2000','Concatenation of PROJ:NZGD2000-20180701 and 9080','','EPSG','4959','EPSG','7909','EPSG','1175',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2000',1,'PROJ','NZGD2000-20180701');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2000',2,'EPSG','9080');

INSERT INTO "concatenated_operation" VALUES('PROJ','NZGD2000_TO_ITRF2005','NZGD2000 to ITRF2005','Concatenation of PROJ:NZGD2000-20180701 and 9081','','EPSG','4959','EPSG','7910','EPSG','1175',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2005',1,'PROJ','NZGD2000-20180701');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2005',2,'EPSG','9081');

INSERT INTO "concatenated_operation" VALUES('PROJ','NZGD2000_TO_ITRF2008','NZGD2000 to ITRF2008','Concatenation of PROJ:NZGD2000-20180701 and EPSG:9082','','EPSG','4959','EPSG','7911','EPSG','1175',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2008',1,'PROJ','NZGD2000-20180701');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2008',2,'EPSG','9082');

INSERT INTO "concatenated_operation" VALUES('PROJ','NZGD2000_TO_ITRF2014','NZGD2000 to ITRF2014','Concatenation of PROJ:NZGD2000-20180701 and EPSG:9083','','EPSG','4959','EPSG','7912','EPSG','1175',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2014',1,'PROJ','NZGD2000-20180701');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','NZGD2000_TO_ITRF2014',2,'EPSG','9083');

-- alias of EPSG:3857
INSERT INTO "projected_crs" VALUES('EPSG','900913','Google Maps Global Mercator',NULL,NULL,'EPSG','4499','EPSG','4326','EPSG','3856','EPSG','3544',NULL,1);

-- ('EPSG','7001','ETRS89 to NAP height (1)') lacks an interpolationCRS with Amersfoort / EPSG:4289
-- See https://salsa.debian.org/debian-gis-team/proj-rdnap/blob/debian/2008-8/Use%20of%20RDTRANS2008%20and%20NAPTRANS2008.pdf
-- "The naptrans2008 VDatum-grid is referenced to the Bessel-1841 ellipsoid"
CREATE TABLE dummy(foo);
CREATE TRIGGER check_grid_transformation_epsg_7001
BEFORE INSERT ON dummy
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'grid_transformation EPSG:7001 entry is not ETRS89 to NAP height (1)')
        WHERE NOT EXISTS(SELECT 1 FROM grid_transformation WHERE auth_name = 'EPSG' AND code = '7001' AND name = 'ETRS89 to NAP height (1)');
    SELECT RAISE(ABORT, 'grid_transformation EPSG:7001 entry has already an interpolationCRS')
        WHERE EXISTS(SELECT 1 FROM grid_transformation WHERE auth_name = 'EPSG' AND code = '7001' AND interpolation_crs_auth_name IS NOT NULL);
END;
INSERT INTO dummy DEFAULT VALUES;
DROP TRIGGER check_grid_transformation_epsg_7001;
DROP TABLE dummy;
UPDATE grid_transformation SET interpolation_crs_auth_name = 'EPSG',
                               interpolation_crs_code = '4289'
                           WHERE auth_name = 'EPSG' AND code = '7001';

-- EPSG:1312 'NAD27 to NAD83 (3)' / NTv1_0.gsb has a accuracy of 1m whereas
-- EPSG:1313 'NAD27 to NAD83 (4)' / NTv2_0.gsb has a accuracy of 1.5m
-- so we will never select automatically NTv2_0.gsb. Worse the advertize
-- accuracy of the NTv1 method

UPDATE grid_transformation SET accuracy = 2.0 WHERE auth_name = 'EPSG' AND code = '1312';

-- Same for EPSG:1462 vs EPSG:1573

UPDATE grid_transformation SET accuracy = 2.0 WHERE auth_name = 'EPSG' AND code = '1462';

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

-- Coordinate system ENh for ProjectedCRS 3D. Should be removed once EPSG has such a coordinate system
INSERT INTO "coordinate_system" VALUES('PROJ','ENh','Cartesian',3);
INSERT INTO "axis" VALUES('PROJ','1','Easting','E','east','PROJ','ENh',1,'EPSG','9001');
INSERT INTO "axis" VALUES('PROJ','2','Northing','N','north','PROJ','ENh',2,'EPSG','9001');
INSERT INTO "axis" VALUES('PROJ','3','Ellipsoidal height','h','up','PROJ','ENh',2,'EPSG','9001');

-- Consider all WGS84 related CRS are equivalent with an accuracy of 2m
INSERT INTO "helmert_transformation" VALUES('PROJ','WGS84_TO_WGS84_G730','WGS 84 to WGS 84 (G730)','','Accuracy 2m','EPSG','9603','Geocentric translations (geog2D domain)','EPSG','4326','EPSG','9053','EPSG','1262',2.0,0,0,0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'',0);
INSERT INTO "helmert_transformation" VALUES('PROJ','WGS84_TO_WGS84_G873','WGS 84 to WGS 84 (G873)','','Accuracy 2m','EPSG','9603','Geocentric translations (geog2D domain)','EPSG','4326','EPSG','9054','EPSG','1262',2.0,0,0,0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'',0);
INSERT INTO "helmert_transformation" VALUES('PROJ','WGS84_TO_WGS84_G1150','WGS 84 to WGS 84 (G1150)','','Accuracy 2m','EPSG','9603','Geocentric translations (geog2D domain)','EPSG','4326','EPSG','9055','EPSG','1262',2.0,0,0,0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'',0);
INSERT INTO "helmert_transformation" VALUES('PROJ','WGS84_TO_WGS84_G1674','WGS 84 to WGS 84 (G1674)','','Accuracy 2m','EPSG','9603','Geocentric translations (geog2D domain)','EPSG','4326','EPSG','9056','EPSG','1262',2.0,0,0,0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'',0);
INSERT INTO "helmert_transformation" VALUES('PROJ','WGS84_TO_WGS84_G1762','WGS 84 to WGS 84 (G1762)','','Accuracy 2m','EPSG','9603','Geocentric translations (geog2D domain)','EPSG','4326','EPSG','9057','EPSG','1262',2.0,0,0,0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'',0);
INSERT INTO "helmert_transformation" VALUES('PROJ','WGS84_TO_WGS84_TRANSIT','WGS 84 to WGS 84 (Transit)','','Accuracy 2m','EPSG','9603','Geocentric translations (geog2D domain)','EPSG','4326','EPSG','8888','EPSG','1262',2.0,0,0,0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'',0);

---- Geoid models -----

INSERT INTO "geoid_model" SELECT 'GEOID99', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE 'g1999%' AND deprecated = 0;

INSERT INTO "geoid_model" SELECT 'GEOID03', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE 'geoid03%' AND deprecated = 0;

INSERT INTO "geoid_model" SELECT 'GEOID06', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE 'geoid06%' AND deprecated = 0;

INSERT INTO "geoid_model" SELECT 'GEOID09', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE 'geoid09%' AND deprecated = 0;

-- Geoid12A and Geoid12B are identical
INSERT INTO "geoid_model" SELECT 'GEOID12A', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE 'g2012b%' AND deprecated = 0;

INSERT INTO "geoid_model" SELECT 'GEOID12B', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE 'g2012b%' AND deprecated = 0;

INSERT INTO "geoid_model" SELECT 'GEOID18', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE 'g2018%' AND deprecated = 0;

INSERT INTO "geoid_model" SELECT 'OSGM15', auth_name, code FROM grid_transformation WHERE auth_name = 'EPSG' AND grid_name LIKE '%OSGM15%' AND deprecated = 0;

---- PROJ historic +datum aliases -----

INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6326','WGS84','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6121','GGRS87','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6269','NAD83','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6267','NAD27','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6314','potsdam','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6223','carthage','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6312','hermannskogel','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6299','ire65','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6272','nzgd49','PROJ');
INSERT INTO "alias_name" VALUES('geodetic_datum','EPSG','6277','OSGB36','PROJ');

-- Given that we have installed above a WGS84 alias to the datum, add also one
-- to the EPSG:4326 CRS, as this is a common use case (https://github.com/OSGeo/PROJ/issues/2216)
INSERT INTO "alias_name" VALUES('geodetic_crs','EPSG','4326','WGS84','PROJ');

---- PROJ unit short names -----

-- Linear units
UPDATE unit_of_measure SET proj_short_name = 'mm'        WHERE auth_name = 'EPSG' AND code = '1025';
UPDATE unit_of_measure SET proj_short_name = 'cm'        WHERE auth_name = 'EPSG' AND code = '1033';
UPDATE unit_of_measure SET proj_short_name = 'm'         WHERE auth_name = 'EPSG' AND code = '9001';
UPDATE unit_of_measure SET proj_short_name = 'ft'        WHERE auth_name = 'EPSG' AND code = '9002';
UPDATE unit_of_measure SET proj_short_name = 'us-ft'     WHERE auth_name = 'EPSG' AND code = '9003';
UPDATE unit_of_measure SET proj_short_name = 'fath'      WHERE auth_name = 'EPSG' AND code = '9014';
UPDATE unit_of_measure SET proj_short_name = 'kmi'       WHERE auth_name = 'EPSG' AND code = '9030';
UPDATE unit_of_measure SET proj_short_name = 'us-ch'     WHERE auth_name = 'EPSG' AND code = '9033';
UPDATE unit_of_measure SET proj_short_name = 'us-mi'     WHERE auth_name = 'EPSG' AND code = '9035';
UPDATE unit_of_measure SET proj_short_name = 'km'        WHERE auth_name = 'EPSG' AND code = '9036';
UPDATE unit_of_measure SET proj_short_name = 'ind-ft'    WHERE auth_name = 'EPSG' AND code = '9081';
UPDATE unit_of_measure SET proj_short_name = 'ind-yd'    WHERE auth_name = 'EPSG' AND code = '9085';
UPDATE unit_of_measure SET proj_short_name = 'mi'        WHERE auth_name = 'EPSG' AND code = '9093';
UPDATE unit_of_measure SET proj_short_name = 'yd'        WHERE auth_name = 'EPSG' AND code = '9096';
UPDATE unit_of_measure SET proj_short_name = 'ch'        WHERE auth_name = 'EPSG' AND code = '9097';
UPDATE unit_of_measure SET proj_short_name = 'link'      WHERE auth_name = 'EPSG' AND code = '9098';

-- Angular units
UPDATE unit_of_measure SET proj_short_name = 'rad'       WHERE auth_name = 'EPSG' AND code = '9101';
UPDATE unit_of_measure SET proj_short_name = 'deg'       WHERE auth_name = 'EPSG' AND code = '9102';
UPDATE unit_of_measure SET proj_short_name = 'grad'      WHERE auth_name = 'EPSG' AND code = '9105';

-- PROJ specific units
INSERT INTO "unit_of_measure" VALUES('PROJ','DM','decimeter','length',0.01,'dm',0);
INSERT INTO "unit_of_measure" VALUES('PROJ','IN','inch','length',0.0254,'in',0);
INSERT INTO "unit_of_measure" VALUES('PROJ','US_IN','US survey inch','length',0.025400050800101,'us-in',0);
INSERT INTO "unit_of_measure" VALUES('PROJ','US_YD','US survey yard','length',0.914401828803658,'us-yd',0);
INSERT INTO "unit_of_measure" VALUES('PROJ','IND_CH','Indian chain','length',20.11669506,'ind-ch',0);

-- Deal with grid_transformation using EPSG:9635 'Geog3D to Geog2D+GravityRelatedHeight (US .gtx)' method
-- We derive records using the more classic 'Geographic3D to GravityRelatedHeight' method
-- We could probably do that at runtime too, but more simple and efficient to create records

INSERT INTO "grid_transformation"
SELECT
    'PROJ' AS auth_name,
    gt.auth_name || '_' || gt.code || '_RESTRICTED_TO_VERTCRS' AS code,
    gcrs.name || ' to ' || vcrs.name || ' (from ' || gt.name || ')' AS name,
    NULL AS description,
    gt.scope,
    'EPSG' AS method_auth_name,
    '9665' AS method_code,
    'Geographic3D to GravityRelatedHeight (gtx)' AS method_name,
    gt.source_crs_auth_name,
    gt.source_crs_code,
    c.vertical_crs_auth_name AS target_crs_auth_name,
    c.vertical_crs_code AS target_crs_code,
    gt.area_of_use_auth_name,
    gt.area_of_use_code,
    gt.accuracy,
    gt.grid_param_auth_name,
    gt.grid_param_code,
    gt.grid_param_name,
    gt.grid_name,
    gt.grid2_param_auth_name,
    gt.grid2_param_code,
    gt.grid2_param_name,
    gt.grid2_name,
    gt.interpolation_crs_auth_name,
    gt.interpolation_crs_code,
    gt.operation_version,
    gt.deprecated
FROM grid_transformation gt
JOIN compound_crs c ON gt.target_crs_code = c.code AND gt.target_crs_auth_name = c.auth_name
JOIN geodetic_crs gcrs ON gt.source_crs_auth_name = gcrs.auth_name AND gt.source_crs_code = gcrs.code
JOIN vertical_crs vcrs on vcrs.auth_name = c.vertical_crs_auth_name AND vcrs.code = c.vertical_crs_code
WHERE method_auth_name = 'EPSG' AND method_code = '9635' AND gt.deprecated = 0;

COMMIT;

CREATE INDEX geodetic_crs_datum_idx ON geodetic_crs(datum_auth_name, datum_code);
CREATE INDEX geodetic_datum_ellipsoid_idx ON geodetic_datum(ellipsoid_auth_name, ellipsoid_code);
CREATE INDEX supersession_idx ON supersession(superseded_table_name, superseded_auth_name, superseded_code);
CREATE INDEX deprecation_idx ON deprecation(table_name, deprecated_auth_name, deprecated_code);
CREATE INDEX helmert_transformation_idx ON helmert_transformation_table(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);
CREATE INDEX grid_transformation_idx ON grid_transformation(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);
CREATE INDEX other_transformation_idx ON other_transformation(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);
CREATE INDEX concatenated_operation_idx ON concatenated_operation(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);

-- Do an explicit foreign_key_check as foreign key checking is a no-op within
-- a transaction. Unfortunately we can't ask for this to be an error, so this
-- is just for verbose output. In Makefile, we check this separately
PRAGMA foreign_key_check;

-- Final consistency checks
CREATE TABLE dummy(foo);
CREATE TRIGGER final_checks
BEFORE INSERT ON dummy
FOR EACH ROW BEGIN

    -- check that view definitions have no error
    SELECT RAISE(ABORT, 'corrupt definition of coordinate_operation_view')
        WHERE (SELECT 1 FROM coordinate_operation_view LIMIT 1) = 0;
    SELECT RAISE(ABORT, 'corrupt definition of crs_view')
        WHERE (SELECT 1 FROM crs_view LIMIT 1) = 0;
    SELECT RAISE(ABORT, 'corrupt definition of object_view')
        WHERE (SELECT 1 FROM object_view LIMIT 1) = 0;
    SELECT RAISE(ABORT, 'corrupt definition of authority_list')
        WHERE (SELECT 1 FROM authority_list LIMIT 1) = 0;

    -- check that a usage is registered for most objects where this is needed
    SELECT RAISE(ABORT, 'One or several objects lack a corresponding record in the usage table')
        WHERE EXISTS (
            SELECT * FROM object_view o WHERE NOT EXISTS (
                SELECT 1 FROM usage u WHERE
                    o.table_name = u.object_table_name AND
                    o.auth_name = u.object_auth_name AND
                    o.code = u.object_code)
            AND o.table_name NOT IN ('unit_of_measure', 'axis',
                'celestial_body', 'ellipsoid', 'prime_meridian', 'extent')
            -- the IGNF registry lacks extent for the following objects
            AND NOT (o.auth_name = 'IGNF' AND o.table_name IN ('geodetic_datum', 'vertical_datum', 'conversion'))
        );

    SELECT RAISE(ABORT, 'Geodetic datum ensemble defined, but no ensemble member')
        WHERE EXISTS (
            SELECT * FROM geodetic_datum d WHERE ensemble_accuracy IS NOT NULL
            AND NOT EXISTS (SELECT 1 FROM geodetic_datum_ensemble_member WHERE
                d.auth_name = ensemble_auth_name AND d.code = ensemble_code)
        );

    SELECT RAISE(ABORT, 'Vertical datum ensemble defined, but no ensemble member')
        WHERE EXISTS (
            SELECT * FROM vertical_datum d WHERE ensemble_accuracy IS NOT NULL
            AND NOT EXISTS (SELECT 1 FROM vertical_datum_ensemble_member WHERE
                d.auth_name = ensemble_auth_name AND d.code = ensemble_code)
        );

    SELECT RAISE(ABORT, 'PROJ defines an alias that exists in EPSG')
        WHERE EXISTS (
         SELECT * FROM (
            SELECT count(*) AS count, table_name, auth_name, code, alt_name FROM alias_name
            WHERE source in ('EPSG', 'PROJ')
            AND NOT (source = 'PROJ' AND alt_name IN ('GGRS87', 'NAD27', 'NAD83'))
            GROUP BY table_name, auth_name, code, alt_name) x WHERE count > 1
        );

    -- test to check that our custom grid transformation overrides are really needed
    SELECT RAISE(ABORT, 'PROJ grid_transformation defined whereas EPSG has one')
        WHERE EXISTS (SELECT 1 FROM grid_transformation g1
                      JOIN grid_transformation g2
                      ON g1.source_crs_auth_name = g2.source_crs_auth_name
                      AND g1.source_crs_code = g2.source_crs_code
                      AND g1.target_crs_auth_name = g2.target_crs_auth_name
                      AND g1.target_crs_code = g2.target_crs_code
                      WHERE g1.auth_name = 'PROJ' AND g2.auth_name = 'EPSG')
        OR EXISTS (SELECT 1 FROM grid_transformation g1
                      JOIN grid_transformation g2
                      ON g1.source_crs_auth_name = g2.target_crs_auth_name
                      AND g1.source_crs_code = g2.target_crs_code
                      AND g1.target_crs_auth_name = g1.source_crs_auth_name
                      AND g1.target_crs_code = g1.source_crs_code
                      WHERE g1.auth_name = 'PROJ' AND g2.auth_name = 'EPSG');

    SELECT RAISE(ABORT, 'Arg! there is now a EPSG:102100 object. Hack in createFromUserInput() will no longer work')
        WHERE EXISTS(SELECT 1 FROM crs_view WHERE auth_name = 'EPSG' AND code = '102100');

    -- check coordinate_operation_view "foreign keys"
    SELECT RAISE(ABORT, 'One coordinate_operation has a broken source_crs link')
        WHERE EXISTS (SELECT * FROM coordinate_operation_view cov WHERE
                      cov.source_crs_auth_name || cov.source_crs_code NOT IN
                      (SELECT auth_name || code FROM crs_view));
    SELECT RAISE(ABORT, 'One coordinate_operation has a broken target_crs link')
        WHERE EXISTS (SELECT * FROM coordinate_operation_view cov WHERE
                      cov.target_crs_auth_name || cov.target_crs_code NOT IN
                      (SELECT auth_name || code FROM crs_view));

    -- check that grids with NTv2 method are properly registered
    SELECT RAISE(ABORT, 'One grid_transformation with NTv2 has not its source_crs in geodetic_crs table with type = ''geographic 2D''')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.method_name = 'NTv2' AND
                      g.source_crs_auth_name || g.source_crs_code NOT IN
                      (SELECT auth_name || code FROM geodetic_crs
                       WHERE type = 'geographic 2D'));
    SELECT RAISE(ABORT, 'One grid_transformation with NTv2 has not its target_crs in geodetic_crs table with type = ''geographic 2D''')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.method_name = 'NTv2' AND
                      g.target_crs_auth_name || g.target_crs_code NOT IN
                      (SELECT auth_name || code FROM geodetic_crs
                       WHERE type = 'geographic 2D'));

    -- check that grids with Geographic3D to GravityRelatedHeight method are properly registered
    SELECT RAISE(ABORT, 'One grid_transformation with Geographic3D to GravityRelatedHeight has not its target_crs in vertical_crs table')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.deprecated = 0 AND
                      g.method_name LIKE 'Geographic3D to GravityRelatedHeight%' AND
                      g.target_crs_auth_name || g.target_crs_code NOT IN
                      (SELECT auth_name || code FROM vertical_crs));
    SELECT RAISE(ABORT, 'One grid_transformation with Geographic3D to GravityRelatedHeight has not its source_crs in geodetic_crs table with type = ''geographic 3D''')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.deprecated = 0 AND
                      g.method_name LIKE 'Geographic3D to GravityRelatedHeight%' AND
                      NOT (g.auth_name = 'EPSG' AND g.code IN (7648, 7649, 7650)) AND -- those are wrongly registered as they use a geocentric CRS. Reported to EPSG
                      g.source_crs_auth_name || g.source_crs_code NOT IN
                      (SELECT auth_name || code FROM geodetic_crs
                       WHERE type = 'geographic 3D'));

    -- check that transformations intersect the area of use of their source/target CRS
    -- EPSG, ESRI and IGNF have cases where this does not hold.
    SELECT RAISE(ABORT, 'The area of use of at least one coordinate_operation does not intersect the one of its source CRS')
        WHERE EXISTS (SELECT * FROM coordinate_operation_view v, crs_view c, usage vu, extent ve, usage cu, extent ce WHERE
                      v.deprecated = 0 AND
                      v.auth_name NOT IN ('EPSG', 'ESRI', 'IGNF') AND
                      v.source_crs_auth_name = c.auth_name AND
                      v.source_crs_code = c.code AND
                      vu.object_table_name = v.table_name AND
                      vu.object_auth_name = v.auth_name AND
                      vu.object_code = v.code AND
                      vu.extent_auth_name = ve.auth_name AND
                      vu.extent_code = ve.code AND
                      cu.object_table_name = c.table_name AND
                      cu.object_auth_name = c.auth_name AND
                      cu.object_code = c.code AND
                      cu.extent_auth_name = ce.auth_name AND
                      cu.extent_code = ce.code AND
                      NOT (ce.south_lat < ve.north_lat AND ve.south_lat < ce.north_lat));
    SELECT RAISE(ABORT, 'The area of use of at least one coordinate_operation does not intersect the one of its target CRS')
        WHERE EXISTS (SELECT * FROM coordinate_operation_view v, crs_view c, usage vu, extent ve, usage cu, extent ce WHERE
                      v.deprecated = 0 AND
                      v.auth_name NOT IN ('EPSG', 'ESRI', 'IGNF') AND
                      v.target_crs_auth_name = c.auth_name AND
                      v.target_crs_code = c.code AND
                      vu.object_table_name = v.table_name AND
                      vu.object_auth_name = v.auth_name AND
                      vu.object_code = v.code AND
                      vu.extent_auth_name = ve.auth_name AND
                      vu.extent_code = ve.code AND
                      cu.object_table_name = c.table_name AND
                      cu.object_auth_name = c.auth_name AND
                      cu.object_code = c.code AND
                      cu.extent_auth_name = ce.auth_name AND
                      cu.extent_code = ce.code AND
                      NOT (ce.south_lat < ve.north_lat AND ve.south_lat < ce.north_lat));

    -- check geoid_model table
    SELECT RAISE(ABORT, 'missing GEOID99 in geoid_model')
        WHERE NOT EXISTS(SELECT 1 FROM geoid_model WHERE name = 'GEOID99');
    SELECT RAISE(ABORT, 'missing GEOID03 in geoid_model')
        WHERE NOT EXISTS(SELECT 1 FROM geoid_model WHERE name = 'GEOID03');
    SELECT RAISE(ABORT, 'missing GEOID06 in geoid_model')
        WHERE NOT EXISTS(SELECT 1 FROM geoid_model WHERE name = 'GEOID06');
    SELECT RAISE(ABORT, 'missing GEOID09 in geoid_model')
        WHERE NOT EXISTS(SELECT 1 FROM geoid_model WHERE name = 'GEOID09');
    SELECT RAISE(ABORT, 'missing GEOID12A in geoid_model')
        WHERE NOT EXISTS(SELECT 1 FROM geoid_model WHERE name = 'GEOID12A');
    SELECT RAISE(ABORT, 'missing GEOID12B in geoid_model')
        WHERE NOT EXISTS(SELECT 1 FROM geoid_model WHERE name = 'GEOID12B');
    SELECT RAISE(ABORT, 'missing GEOID18 in geoid_model')
        WHERE NOT EXISTS(SELECT 1 FROM geoid_model WHERE name = 'GEOID18');

    -- check presence of au_ga_AUSGeoid98.tif
    SELECT RAISE(ABORT, 'missing au_ga_AUSGeoid98.tif')
        WHERE NOT EXISTS(SELECT 1 FROM grid_alternatives WHERE proj_grid_name = 'au_ga_AUSGeoid98.tif');

END;
INSERT INTO dummy DEFAULT VALUES;
DROP TRIGGER final_checks;
DROP TABLE dummy;

VACUUM;
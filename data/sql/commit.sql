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

    -- test to check that our custom grid transformation overrides are really needed
    SELECT RAISE(ABORT, 'PROJ grid_transformation defined whereas EPSG has one')
        WHERE EXISTS (SELECT 1 FROM grid_transformation g1, grid_transformation g2 WHERE
            lower(g1.grid_name) = lower(g2.grid_name) AND
            g1.auth_name = 'PROJ' AND g2.auth_name = 'EPSG');

    SELECT RAISE(ABORT, 'Arg! there is now a EPSG:102100 object. Hack in createFromUserInput() will no longer work')
        WHERE EXISTS(SELECT 1 FROM crs_view WHERE auth_name = 'EPSG' AND code = '102100');

    -- check that grids with HEIGHT_TO_GEOGRAPHIC3D method are properly registered
    SELECT RAISE(ABORT, 'One grid_transformation with HEIGHT_TO_GEOGRAPHIC3D has not its source_crs in vertical_crs table')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.method_code = 'HEIGHT_TO_GEOGRAPHIC3D' AND
                      g.source_crs_auth_name || g.source_crs_code NOT IN
                      (SELECT auth_name || code FROM vertical_crs));
    SELECT RAISE(ABORT, 'One grid_transformation with HEIGHT_TO_GEOGRAPHIC3D has not its target_crs in geodetic_crs table with type = ''geographic 3D''')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.method_code = 'HEIGHT_TO_GEOGRAPHIC3D' AND
                      g.target_crs_auth_name || g.target_crs_code NOT IN
                      (SELECT auth_name || code FROM geodetic_crs
                       WHERE type = 'geographic 3D'));

    -- check that grids with Geographic3D to GravityRelatedHeight method are properly registered
    SELECT RAISE(ABORT, 'One grid_transformation with Geographic3D to GravityRelatedHeight has not its target_crs in vertical_crs table')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.method_code LIKE 'Geographic3D to GravityRelatedHeight%' AND
                      g.target_crs_auth_name || g.target_crs_code NOT IN
                      (SELECT auth_name || code FROM vertical_crs));
    SELECT RAISE(ABORT, 'One grid_transformation with Geographic3D to GravityRelatedHeight has not its source_crs in geodetic_crs table with type = ''geographic 3D''')
        WHERE EXISTS (SELECT * FROM grid_transformation g WHERE
                      g.method_code LIKE 'Geographic3D to GravityRelatedHeight%' AND
                      g.source_crs_auth_name || g.source_crs_code NOT IN
                      (SELECT auth_name || code FROM geodetic_crs
                       WHERE type = 'geographic 3D'));

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
END;
INSERT INTO dummy DEFAULT VALUES;
DROP TRIGGER final_checks;
DROP TABLE dummy;

VACUUM;
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
END;
INSERT INTO dummy DEFAULT VALUES;
DROP TRIGGER final_checks;
DROP TABLE dummy;

VACUUM;
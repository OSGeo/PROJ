COMMIT;

-- Final consistency checks
CREATE TABLE dummy(foo);
CREATE TRIGGER final_checks
BEFORE INSERT ON dummy
FOR EACH ROW BEGIN

    -- check consistency of coordinate_operation related tables
    SELECT RAISE(ABORT, 'coordinate_operation of type helmert_transformation declared in coordinate_operation table but missing in helmert_transformation')
        WHERE EXISTS(SELECT auth_name || ':' || code AS concat_code FROM coordinate_operation WHERE type IN ('helmert_transformation') AND concat_code NOT IN (SELECT auth_name || ':' || code FROM helmert_transformation));
    SELECT RAISE(ABORT, 'coordinate_operation of type grid_transformation declared in coordinate_operation table but missing in grid_transformation')
        WHERE EXISTS(SELECT auth_name || ':' || code AS concat_code FROM coordinate_operation WHERE type IN ('grid_transformation') AND concat_code NOT IN (SELECT auth_name || ':' || code FROM grid_transformation));
    SELECT RAISE(ABORT, 'coordinate_operation of type conversion declared in coordinate_operation table but missing in conversion')
        WHERE EXISTS(SELECT auth_name || ':' || code AS concat_code FROM coordinate_operation WHERE type IN ('conversion') AND concat_code NOT IN (SELECT auth_name || ':' || code FROM conversion));
    SELECT RAISE(ABORT, 'coordinate_operation of type other_transformation declared in coordinate_operation table but missing in other_transformation')
        WHERE EXISTS(SELECT auth_name || ':' || code AS concat_code FROM coordinate_operation WHERE type IN ('other_transformation') AND concat_code NOT IN (SELECT auth_name || ':' || code FROM other_transformation));
    SELECT RAISE(ABORT, 'coordinate_operation of type concatenated_operation declared in coordinate_operation table but missing in concatenated_operation')
        WHERE EXISTS(SELECT auth_name || ':' || code AS concat_code FROM coordinate_operation WHERE type IN ('concatenated_operation') AND concat_code NOT IN (SELECT auth_name || ':' || code FROM concatenated_operation));
    SELECT RAISE(ABORT, 'coordinate_operation count != coordinate_operation_view count + conversion count')
        WHERE (SELECT COUNT(*) FROM coordinate_operation) != (SELECT COUNT(*) FROM coordinate_operation_view) + (SELECT COUNT(*) FROM conversion);

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
-- Remove duplicated entries between source=EPSG and source=EPSG_OLD, as some
-- aliases get sometimes removed in a version and are re-added in a later one.

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name) IN
    (SELECT table_name, auth_name, code, alt_name FROM
        (SELECT count(*) AS count, table_name, auth_name, code, alt_name FROM alias_name
            WHERE source in ('EPSG', 'EPSG_OLD')
            GROUP BY table_name, auth_name, code, alt_name) x WHERE count > 1) AND source = 'EPSG_OLD';

-- Remove "conflicting" entries in alias_name, that is entries of EPSG_OLD whose
-- name match an entry of EPSG but with a different code

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name, source) IN
    (SELECT a2.table_name, a2.auth_name, a2.code, a2.alt_name, a2.source
        FROM alias_name a1 JOIN alias_name a2
        WHERE a1.table_name = a2.table_name AND a1.code != a2.code AND
              a1.alt_name = a2.alt_name AND
              a1.source = 'EPSG' AND a2.source = 'EPSG_OLD');

-- Remove useless entries, i.e. where the official name matches the alias...

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name, source) IN
    (SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM geodetic_datum d JOIN alias_name a WHERE
            a.table_name = 'geodetic_datum' AND a.auth_name = d.auth_name AND
            a.code = d.code AND d.name = a.alt_name);

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name, source) IN
    (SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM vertical_datum d JOIN alias_name a WHERE
            a.table_name = 'vertical_datum' AND a.auth_name = d.auth_name AND
            a.code = d.code AND d.name = a.alt_name);

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name, source) IN
    (SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM geodetic_crs c JOIN alias_name a WHERE
            a.table_name = 'geodetic_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name);

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name, source) IN
    (SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM vertical_crs c JOIN alias_name a WHERE
            a.table_name = 'vertical_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name);

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name, source) IN
    (SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM projected_crs c JOIN alias_name a WHERE
            a.table_name = 'projected_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name);

DELETE FROM alias_name WHERE (table_name, auth_name, code, alt_name, source) IN
    (SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM compound_crs c JOIN alias_name a WHERE
            a.table_name = 'compound_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name);

COMMIT;

CREATE INDEX geodetic_crs_datum_idx ON geodetic_crs(datum_auth_name, datum_code);
CREATE INDEX geodetic_datum_ellipsoid_idx ON geodetic_datum(ellipsoid_auth_name, ellipsoid_code);
CREATE INDEX supersession_idx ON supersession(superseded_table_name, superseded_auth_name, superseded_code);
CREATE INDEX deprecation_idx ON deprecation(table_name, deprecated_auth_name, deprecated_code);
CREATE INDEX helmert_transformation_idx ON helmert_transformation_table(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);
CREATE INDEX grid_transformation_idx ON grid_transformation(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);
CREATE INDEX other_transformation_idx ON other_transformation(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);
CREATE INDEX concatenated_operation_idx ON concatenated_operation(source_crs_auth_name, source_crs_code, target_crs_auth_name, target_crs_code);

-- We don't need to select by auth_name, code so nullify them to save space
UPDATE usage SET auth_name = NULL, code = NULL;

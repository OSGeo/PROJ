SELECT 'Useless alias_name for geodetic_datum...';

SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM geodetic_datum d JOIN alias_name a WHERE
            a.table_name = 'geodetic_datum' AND a.auth_name = d.auth_name AND
            a.code = d.code AND d.name = a.alt_name AND a.source != 'EPSG_OLD';

SELECT 'Useless alias_name for vertical_datum...';

SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM vertical_datum d JOIN alias_name a WHERE
            a.table_name = 'vertical_datum' AND a.auth_name = d.auth_name AND
            a.code = d.code AND d.name = a.alt_name AND a.source != 'EPSG_OLD';

SELECT 'Useless alias_name for geodetic_crs...';

SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM geodetic_crs c JOIN alias_name a WHERE
            a.table_name = 'geodetic_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name AND a.source != 'EPSG_OLD';

SELECT 'Useless alias_name for vertical_crs...';

SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM vertical_crs c JOIN alias_name a WHERE
            a.table_name = 'vertical_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name AND a.source != 'EPSG_OLD';

SELECT 'Useless alias_name for projected_crs...';

SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM projected_crs c JOIN alias_name a WHERE
            a.table_name = 'projected_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name AND a.source != 'EPSG_OLD' AND
            -- EPSG keeps those useless aliases because of policy w.r.t ISO geodetic registry
            NOT (a.source = 'EPSG' AND a.code in (9356,9357,9358,9359,9360));

SELECT 'Useless alias_name for compound_crs...';

SELECT a.table_name, a.auth_name, a.code, a.alt_name, a.source
        FROM compound_crs c JOIN alias_name a WHERE
            a.table_name = 'compound_crs' AND a.auth_name = c.auth_name AND
            a.code = c.code AND c.name = a.alt_name AND a.source != 'EPSG_OLD';

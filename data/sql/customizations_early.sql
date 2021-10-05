-- This file is hand generated.

INSERT INTO "extent" VALUES('PROJ','EXTENT_UNKNOWN','Not specified','Not specified.',-90.0,90.0,-180.0,180.0,0);
INSERT INTO "scope" VALUES('PROJ','SCOPE_UNKNOWN','Not known.',0);

INSERT INTO celestial_body VALUES('PROJ', 'EARTH', 'Earth', 6378137.0);

INSERT INTO versioned_auth_name_mapping VALUES ('IAU_2015', 'IAU', '2015', 1);

CREATE TRIGGER conversion_method_check_insert_trigger_orthographic
INSTEAD OF INSERT ON conversion
BEGIN

    SELECT RAISE(ABORT, 'insert on conversion violates constraint: bad parameters for Orthographic')
        WHERE NEW.deprecated != 1 AND NEW.method_auth_name = 'EPSG' AND NEW.method_code = '9840' AND (NEW.method_name != 'Orthographic' OR NEW.param1_auth_name != 'EPSG' OR NEW.param1_code != '8801' OR NEW.param1_name != 'Latitude of natural origin' OR NEW.param1_value IS NULL OR NEW.param1_uom_auth_name IS NULL OR NEW.param1_uom_code IS NULL OR (SELECT type FROM unit_of_measure WHERE auth_name = NEW.param1_uom_auth_name AND code = NEW.param1_uom_code) != 'angle' OR NEW.param2_auth_name != 'EPSG' OR NEW.param2_code != '8802' OR NEW.param2_name != 'Longitude of natural origin' OR NEW.param2_value IS NULL OR NEW.param2_uom_auth_name IS NULL OR NEW.param2_uom_code IS NULL OR (SELECT type FROM unit_of_measure WHERE auth_name = NEW.param2_uom_auth_name AND code = NEW.param2_uom_code) != 'angle' OR NEW.param3_auth_name != 'EPSG' OR NEW.param3_code != '8806' OR NEW.param3_name != 'False easting' OR NEW.param3_value IS NULL OR NEW.param3_uom_auth_name IS NULL OR NEW.param3_uom_code IS NULL OR (SELECT type FROM unit_of_measure WHERE auth_name = NEW.param3_uom_auth_name AND code = NEW.param3_uom_code) != 'length' OR NEW.param4_auth_name != 'EPSG' OR NEW.param4_code != '8807' OR NEW.param4_name != 'False northing' OR NEW.param4_value IS NULL OR NEW.param4_uom_auth_name IS NULL OR NEW.param4_uom_code IS NULL OR (SELECT type FROM unit_of_measure WHERE auth_name = NEW.param4_uom_auth_name AND code = NEW.param4_uom_code) != 'length' OR NEW.param5_auth_name IS NOT NULL OR NEW.param5_code IS NOT NULL OR NEW.param5_name IS NOT NULL OR NEW.param5_value IS NOT NULL OR NEW.param5_uom_auth_name IS NOT NULL OR NEW.param5_uom_code IS NOT NULL OR NEW.param6_auth_name IS NOT NULL OR NEW.param6_code IS NOT NULL OR NEW.param6_name IS NOT NULL OR NEW.param6_value IS NOT NULL OR NEW.param6_uom_auth_name IS NOT NULL OR NEW.param6_uom_code IS NOT NULL OR NEW.param7_auth_name IS NOT NULL OR NEW.param7_code IS NOT NULL OR NEW.param7_name IS NOT NULL OR NEW.param7_value IS NOT NULL OR NEW.param7_uom_auth_name IS NOT NULL OR NEW.param7_uom_code IS NOT NULL);

END;

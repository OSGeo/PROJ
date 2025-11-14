-- Insertion triggers that implement consistency checks across tables

CREATE TRIGGER ellipsoid_insert_trigger
BEFORE INSERT ON ellipsoid
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on ellipsoid violates constraint: uom should be of type ''length''')
        WHERE (SELECT type FROM unit_of_measure WHERE auth_name = NEW.uom_auth_name AND code = NEW.uom_code) != 'length';
END;

CREATE TRIGGER usage_insert_trigger
BEFORE INSERT ON usage
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on usage violates constraint: new entry refers to unexisting code')
        WHERE NOT EXISTS (SELECT 1 FROM object_view o WHERE o.table_name = NEW.object_table_name AND o.auth_name = NEW.object_auth_name AND o.code = NEW.object_code);
    SELECT RAISE(ABORT, 'insert on usage violates constraint: extent must not be deprecated when object is not deprecated')
        WHERE EXISTS (
            SELECT 1 FROM extent JOIN object_view o WHERE
                NOT (o.table_name IN ('projected_crs', 'vertical_crs', 'vertical_datum', 'conversion') AND o.auth_name = 'ESRI') AND
                o.table_name = NEW.object_table_name AND
                o.auth_name = NEW.object_auth_name AND
                o.code = NEW.object_code AND
                extent.auth_name = NEW.extent_auth_name AND
                extent.code = NEW.extent_code AND
                extent.deprecated = 1 AND
                o.deprecated = 0);
END;

CREATE TRIGGER prime_meridian_insert_trigger
BEFORE INSERT ON prime_meridian
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on prime_meridian violates constraint: uom should be of type ''angle''')
        WHERE (SELECT type FROM unit_of_measure WHERE auth_name = NEW.uom_auth_name AND code = NEW.uom_code) != 'angle';
END;

CREATE TRIGGER geodetic_datum_insert_trigger
BEFORE INSERT ON geodetic_datum
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on geodetic_datum violates constraint: ellipsoid must not be deprecated when geodetic_datum is not deprecated')
        WHERE EXISTS(SELECT 1 FROM ellipsoid WHERE ellipsoid.auth_name = NEW.ellipsoid_auth_name AND ellipsoid.code = NEW.ellipsoid_code AND ellipsoid.deprecated != 0) AND NEW.deprecated = 0 AND NEW.auth_name != 'ESRI';
    SELECT RAISE(ABORT, 'insert on geodetic_datum violates constraint: prime_meridian must not be deprecated when geodetic_datum is not deprecated')
        WHERE EXISTS(SELECT 1 FROM prime_meridian WHERE prime_meridian.auth_name = NEW.prime_meridian_auth_name AND prime_meridian.code = NEW.prime_meridian_code AND prime_meridian.deprecated != 0) AND NEW.deprecated = 0;
    SELECT RAISE(ABORT, 'frame_reference_epoch and ensemble_accuracy are mutually exclusive')
        WHERE NEW.frame_reference_epoch IS NOT NULL AND NEW.ensemble_accuracy IS NOT NULL;
END;

CREATE TRIGGER axis_insert_trigger
BEFORE INSERT ON axis
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on axis violates constraint: coordinate_system_order should be <= coordinate_system.dimension')
        WHERE NEW.coordinate_system_order > (SELECT dimension FROM coordinate_system WHERE auth_name = NEW.coordinate_system_auth_name AND code = NEW.coordinate_system_code);
    SELECT RAISE(ABORT, 'insert on axis violates constraint: uom should be defined unless the coordinate system is ordinal')
        WHERE EXISTS(SELECT 1 FROM coordinate_system cs WHERE cs.type != 'ordinal' AND (NEW.uom_auth_name IS NULL OR NEW.uom_code IS NULL) AND cs.auth_name = NEW.coordinate_system_auth_name AND cs.code = NEW.coordinate_system_code);
END;

CREATE TRIGGER geodetic_crs_insert_trigger
BEFORE INSERT ON geodetic_crs
FOR EACH ROW BEGIN

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: (auth_name, code) must not already exist in crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.auth_name AND crs_view.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: datum must not be deprecated when geodetic_crs is not deprecated')
        WHERE EXISTS(SELECT 1 FROM geodetic_datum datum WHERE datum.auth_name = NEW.datum_auth_name AND datum.code = NEW.datum_code AND datum.deprecated != 0) AND NEW.deprecated = 0 AND NEW.text_definition IS NOT NULL;

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.dimension must be 3 for type = ''geocentric''')
        WHERE NEW.type = 'geocentric' AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 3;

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.type must be ''Cartesian'' for type = ''geocentric''')
        WHERE NEW.type = 'geocentric' AND (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'Cartesian';

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.type must be ''ellipsoidal''  for type = ''geographic 2D'' or ''geographic 3D''')
        WHERE NEW.type IN ('geographic 2D', 'geographic 3D') AND (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'ellipsoidal';

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.dimension must be 2 for type = ''geographic 2D''')
        WHERE NEW.type = 'geographic 2D' AND NEW.deprecated != 1 AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 2;

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.dimension must be 3 for type = ''geographic 3D''')
        WHERE NEW.type = 'geographic 3D' AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 3;
END;

CREATE TRIGGER vertical_crs_insert_trigger
BEFORE INSERT ON vertical_crs
FOR EACH ROW BEGIN

    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: (auth_name, code) must not already exist in crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.auth_name AND crs_view.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.name = NEW.name AND crs_view.deprecated = 0 AND NEW.deprecated = 0
        AND NEW.auth_name IN (SELECT auth_name FROM builtin_authorities WHERE auth_name != 'IGNF')
        AND NOT(NEW.auth_name = 'ESRI' and crs_view.table_name = 'geodetic_crs') -- some ESRI vertical CRS are an ellipsoidal height CRS derived from a geodetic CRS
    );

    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: datum must not be deprecated when vertical_crs is not deprecated')
        WHERE EXISTS(SELECT 1 FROM vertical_crs datum WHERE datum.auth_name = NEW.datum_auth_name AND datum.code = NEW.datum_code AND datum.deprecated != 0) AND NEW.deprecated = 0;

    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: coordinate_system.type must be ''vertical''')
        WHERE (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'vertical';
    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: coordinate_system.dimension must be 1')
        WHERE (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 1;
END;

CREATE TRIGGER engineering_crs_insert_trigger
BEFORE INSERT ON engineering_crs
FOR EACH ROW BEGIN

    SELECT RAISE(ABORT, 'insert on engineering_crs violates constraint: (auth_name, code) must not already exist in crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.auth_name AND crs_view.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on engineering_crs violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.name = NEW.name AND crs_view.deprecated = 0 AND NEW.deprecated = 0
    );

    SELECT RAISE(ABORT, 'insert on engineering_crs violates constraint: datum must not be deprecated when engineering_crs is not deprecated')
        WHERE EXISTS(SELECT 1 FROM engineering_crs datum WHERE datum.auth_name = NEW.datum_auth_name AND datum.code = NEW.datum_code AND datum.deprecated != 0) AND NEW.deprecated = 0;

    SELECT RAISE(ABORT, 'insert on engineering_crs violates constraint: coordinate_system.dimension must be 2 or 3')
        WHERE (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) NOT IN (2, 3);
END;

CREATE TRIGGER conversion_method_insert_trigger
BEFORE INSERT ON conversion_method
BEGIN

    SELECT RAISE(ABORT, 'insert on conversion violates constraint: method should be known')
        WHERE (NEW.auth_name || '_' || NEW.code || '_' || NEW.name) NOT IN (
            'EPSG_1024_Popular Visualisation Pseudo Mercator',
            'EPSG_1026_Mercator (Spherical)',
            'EPSG_1027_Lambert Azimuthal Equal Area (Spherical)',
            'EPSG_1028_Equidistant Cylindrical',
            'EPSG_1029_Equidistant Cylindrical (Spherical)',
            'EPSG_1041_Krovak (North Orientated)',
            'EPSG_1042_Krovak Modified',
            'EPSG_1043_Krovak Modified (North Orientated)',
            'EPSG_1051_Lambert Conic Conformal (2SP Michigan)',
            'EPSG_1052_Colombia Urban',
            'EPSG_1068_Height Depth Reversal',
            'EPSG_1069_Change of Vertical Unit',
            'EPSG_1104_Change of Vertical Unit',
            'EPSG_1078_Equal Earth',
            'EPSG_1102_Lambert Conic Conformal (1SP variant B)',
            'EPSG_1111_Transverse Mercator 3D',
            'EPSG_1119_Equidistant Conic',
            'EPSG_1125_Azimuthal Equidistant',
            'EPSG_1130_Local Orthographic',
            'EPSG_9602_Geographic/geocentric conversions',
            'EPSG_9659_Geographic3D to 2D conversion',
            'EPSG_9801_Lambert Conic Conformal (1SP)',
            'EPSG_9802_Lambert Conic Conformal (2SP)',
            'EPSG_9803_Lambert Conic Conformal (2SP Belgium)',
            'EPSG_9804_Mercator (variant A)',
            'EPSG_9805_Mercator (variant B)',
            'EPSG_9806_Cassini-Soldner',
            'EPSG_9807_Transverse Mercator',
            'EPSG_9808_Transverse Mercator (South Orientated)',
            'EPSG_9809_Oblique Stereographic',
            'EPSG_9810_Polar Stereographic (variant A)',
            'EPSG_9811_New Zealand Map Grid',
            'EPSG_9812_Hotine Oblique Mercator (variant A)',
            'EPSG_9813_Laborde Oblique Mercator',
            'EPSG_9815_Hotine Oblique Mercator (variant B)',
            'EPSG_9816_Tunisia Mining Grid',
            'EPSG_9817_Lambert Conic Near-Conformal',
            'EPSG_9818_American Polyconic',
            'EPSG_9819_Krovak',
            'EPSG_9820_Lambert Azimuthal Equal Area',
            'EPSG_9821_Lambert Azimuthal Equal Area (Spherical)',
            'EPSG_9822_Albers Equal Area',
            'EPSG_9823_Equidistant Cylindrical (Spherical)',
            'EPSG_9824_Transverse Mercator Zoned Grid System',
            'EPSG_9826_Lambert Conic Conformal (West Orientated)',
            'EPSG_9828_Bonne (South Orientated)',
            'EPSG_9829_Polar Stereographic (variant B)',
            'EPSG_9830_Polar Stereographic (variant C)',
            'EPSG_9831_Guam Projection',
            'EPSG_9832_Modified Azimuthal Equidistant',
            'EPSG_9833_Hyperbolic Cassini-Soldner',
            'EPSG_9834_Lambert Cylindrical Equal Area (Spherical)',
            'EPSG_9835_Lambert Cylindrical Equal Area',
            'EPSG_9836_Geocentric/topocentric conversions',
            'EPSG_9837_Geographic/topocentric conversions',
            'EPSG_9838_Vertical Perspective',
            'EPSG_9840_Orthographic',
            'EPSG_9841_Mercator (1SP) (Spherical)',
            'EPSG_9842_Equidistant Cylindrical',
            'EPSG_9843_Axis Order Reversal (2D)',
            'EPSG_9844_Axis Order Reversal (Geographic3D horizontal)',
            'EPSG_9827_Bonne') AND NEW.auth_name != 'PROJ';
END;

CREATE TRIGGER conversion_table_insert_trigger
BEFORE INSERT ON conversion_table
BEGIN
    SELECT RAISE(ABORT, 'insert on conversion_table violates constraint: (auth_name, code) must not already exist in coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.auth_name = NEW.auth_name AND covwv.code = NEW.code);
END;

CREATE TRIGGER projected_crs_insert_trigger
BEFORE INSERT ON projected_crs
FOR EACH ROW BEGIN

    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: (auth_name, code) must not already exist in crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.auth_name AND crs_view.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.name = NEW.name AND crs_view.deprecated = 0 AND NEW.deprecated = 0
        AND NEW.auth_name IN (SELECT auth_name FROM builtin_authorities WHERE auth_name NOT IN ('IGNF', 'ESRI'))
    );

    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: geodetic_crs must not be deprecated when projected_crs is not deprecated')
        WHERE EXISTS(SELECT 1 FROM geodetic_crs WHERE geodetic_crs.auth_name = NEW.geodetic_crs_auth_name AND geodetic_crs.code = NEW.geodetic_crs_code AND geodetic_crs.deprecated != 0 AND geodetic_crs.name NOT LIKE 'Unknown datum%' AND geodetic_crs.name NOT LIKE 'Unspecified datum%') AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI' AND NEW.geodetic_crs_auth_name != 'ESRI');

    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: conversion must exist when text_definition is NULL')
        WHERE NOT EXISTS(SELECT 1 FROM conversion WHERE conversion.auth_name = NEW.conversion_auth_name AND conversion.code = NEW.conversion_code) AND NEW.text_definition IS NULL;

    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: conversion must not be deprecated when projected_crs is not deprecated')
        WHERE EXISTS(SELECT 1 FROM conversion WHERE conversion.auth_name = NEW.conversion_auth_name AND conversion.code = NEW.conversion_code AND conversion.deprecated != 0) AND NEW.deprecated = 0;

    --SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: geodetic_crs must NOT be defined when text_definition is NOT NULL')
    --    WHERE (NOT(NEW.geodetic_crs_auth_name IS NULL OR NEW.geodetic_crs_code IS NULL)) AND NEW.text_definition IS NOT NULL;

    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: coordinate_system.type must be ''cartesian''')
        WHERE (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'Cartesian';

    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: coordinate_system.dimension must be 2 or 3')
    -- EPSG:4461 is topocentric
        WHERE NOT(NEW.coordinate_system_auth_name = 'EPSG' AND NEW.coordinate_system_code = '4461') AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) NOT IN (2,3);
END;

CREATE TRIGGER compound_crs_insert_trigger
BEFORE INSERT ON compound_crs
FOR EACH ROW BEGIN

    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: (auth_name, code) must not already exist in crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.auth_name AND crs_view.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) crs_view')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.name = NEW.name AND crs_view.deprecated = 0 AND NEW.deprecated = 0
        AND NEW.auth_name IN (SELECT auth_name FROM builtin_authorities WHERE auth_name != 'IGNF')
    );

    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: horiz_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.horiz_crs_auth_name AND crs_view.code = NEW.horiz_crs_code);

    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: horiz_crs must be equal to ''geographic 2D'', ''projected'' or ''engineering''')
        WHERE (SELECT type FROM crs_view WHERE crs_view.auth_name = NEW.horiz_crs_auth_name AND crs_view.code = NEW.horiz_crs_code) NOT IN ('geographic 2D', 'projected', 'engineering');

    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: vertical_crs must be equal to ''vertical''')
        WHERE (SELECT type FROM crs_view WHERE crs_view.auth_name = NEW.vertical_crs_auth_name AND crs_view.code = NEW.vertical_crs_code) NOT IN ('vertical');

    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: horiz_crs must not be deprecated when compound_crs is not deprecated')
        WHERE EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.horiz_crs_auth_name AND crs_view.code = NEW.horiz_crs_code AND crs_view.deprecated != 0) AND NEW.deprecated = 0;

    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: vertical_crs must not be deprecated when compound_crs is not deprecated')
        WHERE EXISTS (SELECT 1 FROM vertical_crs WHERE vertical_crs.auth_name = NEW.vertical_crs_auth_name AND vertical_crs.code = NEW.vertical_crs_code AND vertical_crs.deprecated != 0) AND NEW.deprecated = 0;
END;

CREATE TRIGGER coordinate_metadata_insert_trigger
BEFORE INSERT ON coordinate_metadata
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on coordinate_metadata violates constraint: (crs_auth_name, crs_code) must already exist in crs_view')
        WHERE NOT EXISTS (
            SELECT 1 FROM crs_view WHERE
                NEW.crs_auth_name IS NOT NULL AND
                crs_view.auth_name = NEW.crs_auth_name AND
                crs_view.code = NEW.crs_code
            UNION ALL SELECT 1 WHERE NEW.crs_auth_name IS NULL);
    SELECT RAISE(ABORT, 'insert on coordinate_metadata violates constraint: (crs_auth_name, crs_code) and crs_text_definition are mutually exclusive')
        WHERE NEW.crs_auth_name IS NOT NULL AND NEW.crs_text_definition IS NOT NULL;
    SELECT RAISE(ABORT, 'insert on coordinate_metadata violates constraint: one of (crs_auth_name, crs_code) or crs_text_definition must be set')
        WHERE NEW.crs_auth_name IS NULL AND NEW.crs_text_definition IS NULL;
END;

CREATE TRIGGER helmert_transformation_insert_trigger
BEFORE INSERT ON helmert_transformation_table
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: (auth_name, code) must not already exist in coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.auth_name = NEW.auth_name AND covwv.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.name = NEW.name AND covwv.deprecated = 0 AND NEW.deprecated = 0
        AND NEW.auth_name IN (SELECT auth_name FROM builtin_authorities WHERE auth_name != 'IGNF')
    );

    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: translation_uom.type must be ''length''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.translation_uom_auth_name AND unit_of_measure.code = NEW.translation_uom_code) != 'length';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rotation_uom.type must be ''angle''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rotation_uom_auth_name AND unit_of_measure.code = NEW.rotation_uom_code) != 'angle';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: scale_difference_uom.type must be ''scale''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.scale_difference_uom_auth_name AND unit_of_measure.code = NEW.scale_difference_uom_code) != 'scale';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rate_translation_uom.type must be ''length''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rate_translation_uom_auth_name AND unit_of_measure.code = NEW.rate_translation_uom_code) != 'length';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rate_rotation_uom.type must be ''angle''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rate_rotation_uom_auth_name AND unit_of_measure.code = NEW.rate_rotation_uom_code) != 'angle';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rate_scale_difference_uom.type must be ''scale''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rate_scale_difference_uom_auth_name AND unit_of_measure.code = NEW.rate_scale_difference_uom_code) != 'scale';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: epoch_uom.type must be ''time''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.epoch_uom_auth_name AND unit_of_measure.code = NEW.epoch_uom_code) != 'time';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: pivot_uom.type must be ''length''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.pivot_uom_auth_name AND unit_of_measure.code = NEW.pivot_uom_code) != 'length';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: source_crs must not be deprecated when helmert_transformation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.source_crs_auth_name AND crs.code = NEW.source_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: target_crs must not be deprecated when helmert_transformation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.target_crs_auth_name AND crs.code = NEW.target_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');

    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: target_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.target_crs_auth_name AND crs_view.code = NEW.target_crs_code);

    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: target_crs must be geodetic or compound')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.target_crs_auth_name AND crs_view.code = NEW.target_crs_code AND crs_view.type IN ('geographic 2D', 'geographic 3D', 'geocentric', 'compound'));

    -- check that source and target of the same nature
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: source CRS and target CRS must have consistent geodetic_crs.type')
        WHERE EXISTS (SELECT 1 FROM crs_view crs1, crs_view crs2 WHERE
          crs1.auth_name = NEW.source_crs_auth_name AND crs1.code = NEW.source_crs_code
          AND crs2.auth_name = NEW.target_crs_auth_name AND crs2.code = NEW.target_crs_code
          AND NEW.deprecated = 0 AND (
             NOT ((crs1.type = crs2.type AND crs1.type IN ('geographic 2D', 'geographic 3D', 'geocentric') AND
                  NOT(NEW.method_auth_name = 'EPSG' AND NEW.method_code = 1149)) OR
                  (crs1.type = 'geographic 3D' and crs2.type = 'compound' AND
                   NEW.method_auth_name = 'EPSG' AND NEW.method_code = 1149))));

    -- check that the method used by a Helmert transformation is consistent with the dimensionality of the CRS
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: the domain of the method of helmert_transformation should be consistent with the dimensionality of the CRS')
        WHERE NEW.deprecated = 0 AND
             (NOT(NEW.method_auth_name = 'EPSG' AND NEW.method_code = 1149) AND EXISTS (SELECT 1 FROM geodetic_crs crs
                      LEFT JOIN coordinate_operation_method m ON
                          NEW.method_auth_name = m.auth_name AND NEW.method_code = m.code
                      WHERE
                          crs.auth_name = NEW.source_crs_auth_name AND crs.code = NEW.source_crs_code AND
                          ((m.name LIKE '%geog2D domain%' AND crs.type != 'geographic 2D') OR
                           (m.name LIKE '%geog3D domain%' AND crs.type != 'geographic 3D') OR
                           (m.name LIKE '%geocentric domain%' AND crs.type != 'geocentric')))
          OR (NEW.method_auth_name = 'EPSG' AND NEW.method_code = 1149 AND EXISTS (SELECT 1 FROM compound_crs crs
                      LEFT JOIN coordinate_operation_method m ON
                          NEW.method_auth_name = m.auth_name AND NEW.method_code = m.code
                      WHERE
                          crs.auth_name = NEW.target_crs_auth_name AND crs.code = NEW.target_crs_code AND
                          m.name NOT LIKE '%geog3D domain%')));

    -- check that a time-dependent Helmert transformation has its source or target CRS being dynamic
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: a time-dependent Helmert transformations should have at least one of its source or target CRS dynamic')
        WHERE NEW.deprecated = 0
              AND EXISTS (SELECT 1 FROM coordinate_operation_method m
                      WHERE NEW.method_auth_name = m.auth_name AND NEW.method_code = m.code AND
                            m.name LIKE 'Time-dependent%')
              AND EXISTS (
                  SELECT 1 FROM geodetic_crs crs
                  JOIN geodetic_datum gd ON
                          gd.auth_name = crs.datum_auth_name AND gd.code = crs.datum_code
                  WHERE crs.auth_name = NEW.source_crs_auth_name AND
                        crs.code = NEW.source_crs_code AND
                        gd.frame_reference_epoch IS NULL)
              AND EXISTS (
                  SELECT 1 FROM geodetic_crs crs
                  JOIN geodetic_datum gd ON
                          gd.auth_name = crs.datum_auth_name AND gd.code = crs.datum_code
                  WHERE crs.auth_name = NEW.target_crs_auth_name AND
                        crs.code = NEW.target_crs_code AND
                        gd.frame_reference_epoch IS NULL);

END;

CREATE TRIGGER grid_transformation_insert_trigger
BEFORE INSERT ON grid_transformation
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: (auth_name, code) must not already exist in coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.auth_name = NEW.auth_name AND covwv.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.name = NEW.name AND covwv.deprecated = 0 AND NEW.deprecated = 0
        AND NEW.auth_name IN (SELECT auth_name FROM builtin_authorities WHERE auth_name != 'IGNF')
        AND NOT (NEW.description LIKE 'Reversible alternative to%' AND covwv.description NOT LIKE 'Reversible alternative to%')
        AND NEW.code NOT LIKE '%_WITH_NAD83CSRSV7_INTERPOLATION'
    );

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: source_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.source_crs_auth_name AND crs_view.code = NEW.source_crs_code);

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: target_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.target_crs_auth_name AND crs_view.code = NEW.target_crs_code);

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: interpolation_crs(auth_name, code) not found')
        WHERE NEW.interpolation_crs_code IS NOT NULL AND NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.interpolation_crs_auth_name AND crs_view.code = NEW.interpolation_crs_code);

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: interpolation_crs must be a GeodeticCRS on non-TIN shift based files')
        WHERE NEW.method_name NOT LIKE '%JSON%' AND NEW.interpolation_crs_code IS NOT NULL AND NOT EXISTS (SELECT 1 FROM geodetic_crs WHERE geodetic_crs.auth_name = NEW.interpolation_crs_auth_name AND geodetic_crs.code = NEW.interpolation_crs_code);

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: source_crs must not be deprecated when grid_transformation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.source_crs_auth_name AND crs.code = NEW.source_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: target_crs must not be deprecated when grid_transformation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.target_crs_auth_name AND crs.code = NEW.target_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');

    -- check that grids with NTv2 method are properly registered
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: grid_transformation with NTv2 must have its source_crs in geodetic_crs table with type = ''geographic 2D''')
        WHERE NEW.method_name = 'NTv2' AND
                      NOT EXISTS (SELECT 1 FROM geodetic_crs crs WHERE
                          NEW.source_crs_auth_name = crs.auth_name AND
                          NEW.source_crs_code = crs.code AND
                          crs.type = 'geographic 2D');

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: grid_transformation with NTv2 has have its target_crs in geodetic_crs table with type = ''geographic 2D''')
        WHERE NEW.method_name = 'NTv2' AND
                      NOT EXISTS (SELECT 1 FROM geodetic_crs crs WHERE
                          NEW.target_crs_auth_name = crs.auth_name AND
                          NEW.target_crs_code = crs.code AND
                          crs.type = 'geographic 2D');

    -- check that grids with Geographic3D to GravityRelatedHeight method are properly registered
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: grid_transformation with Geographic3D to GravityRelatedHeight must have its target_crs in vertical_crs table')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name LIKE 'Geographic3D to GravityRelatedHeight%' AND
              NOT EXISTS (SELECT 1 FROM vertical_crs crs WHERE
                          NEW.target_crs_auth_name = crs.auth_name AND
                          NEW.target_crs_code = crs.code);

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: grid_transformation with Geographic3D to GravityRelatedHeight or Geog3D to Geog2D+XXX must have its source_crs in geodetic_crs table with type = ''geographic 3D''')
        WHERE NEW.deprecated = 0 AND
              (NEW.method_name LIKE 'Geographic3D to %' OR NEW.method_name LIKE 'Geog3D to %') AND
              NOT EXISTS (SELECT 1 FROM geodetic_crs crs WHERE
                          NEW.source_crs_auth_name = crs.auth_name AND
                          NEW.source_crs_code = crs.code AND
                          crs.type = 'geographic 3D');

    -- check that transformations with Geog3D to Geog2D+GravityRelatedHeight/Depth family of methods are properly registered
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: a transformation Geog3D to Geog2D+GravityRelatedHeight/Depth must have a geog3D CRS as source')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name LIKE 'Geog3D to Geog2D+%' AND
              NOT EXISTS (SELECT 1 FROM geodetic_crs gcrs WHERE
                          gcrs.auth_name = NEW.source_crs_auth_name AND gcrs.code = NEW.source_crs_code
                          AND gcrs.type = 'geographic 3D');

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: a transformation Geog3D to Geog2D+GravityRelatedHeight/Depth must have a compound CRS with a geog2D CRS as target')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name LIKE 'Geog3D to Geog2D+%' AND
              NOT EXISTS (SELECT 1 FROM compound_crs ccrs
                      LEFT JOIN geodetic_crs gcrs ON
                          gcrs.auth_name = horiz_crs_auth_name AND gcrs.code = horiz_crs_code
                      WHERE
                          ccrs.auth_name = NEW.target_crs_auth_name AND ccrs.code = NEW.target_crs_code
                          AND gcrs.type = 'geographic 2D');

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: a transformation Geog3D to Geog2D+GravityRelatedHeight/Depth must use the same geodetic datum for the source and target CRS')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name LIKE 'Geog3D to Geog2D+%' AND
              -- Exception for following CRS 'ETRS89-CZE [2007] to ETRS89-CZE [2007] + Baltic 1957 height (2)'
              (NOT (NEW.auth_name = 'EPSG' AND NEW.code = 10568)) AND
              NOT EXISTS (SELECT 1 FROM compound_crs ccrs
                      LEFT JOIN geodetic_crs target_gcrs ON
                          target_gcrs.auth_name = horiz_crs_auth_name AND target_gcrs.code = horiz_crs_code
                      LEFT JOIN geodetic_crs source_gcrs ON
                          source_gcrs.auth_name = NEW.source_crs_auth_name AND source_gcrs.code = NEW.source_crs_code
                      WHERE
                          ccrs.auth_name = NEW.target_crs_auth_name AND ccrs.code = NEW.target_crs_code
                          AND source_gcrs.name = target_gcrs.name);

    -- check that grids with 'Vertical Offset by Grid Interpolation' methods are properly registered
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: grid_transformation with Vertical Offset by Grid Interpolation must have its source_crs in vertical_crs table')
        WHERE NEW.method_name LIKE 'Vertical Offset by Grid Interpolation%' AND
              NOT EXISTS (SELECT 1 FROM vertical_crs crs WHERE
                          NEW.source_crs_auth_name = crs.auth_name AND
                          NEW.source_crs_code = crs.code);

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: grid_transformation with Vertical Offset by Grid Interpolation must have its target_crs in vertical_crs table')
        WHERE NEW.method_name LIKE 'Vertical Offset by Grid Interpolation%' AND
              NOT EXISTS (SELECT 1 FROM vertical_crs crs WHERE
                          NEW.target_crs_auth_name = crs.auth_name AND
                          NEW.target_crs_code = crs.code);

    -- check that grids with 'Geocentric translations using NEU velocity grid (gtg)' method are properly registered
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: source_crs(auth_name, code) must be Geocentric')
        WHERE NEW.method_name IN ('Geocentric translations using NEU velocity grid (gtg)') AND NOT EXISTS (SELECT 1 FROM geodetic_crs gcrs WHERE gcrs.auth_name = NEW.source_crs_auth_name AND gcrs.code = NEW.source_crs_code AND type = 'geocentric');

    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: target_crs(auth_name, code) must be Geocentric')
        WHERE NEW.method_name IN ('Geocentric translations using NEU velocity grid (gtg)') AND NOT EXISTS (SELECT 1 FROM geodetic_crs gcrs WHERE gcrs.auth_name = NEW.target_crs_auth_name AND gcrs.code = NEW.target_crs_code AND type = 'geocentric');
END;

CREATE TRIGGER grid_packages_insert_trigger
BEFORE INSERT ON grid_packages
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on grid_packages violates constraint: open_license must be set when url is not NULL')
        WHERE NEW.open_license IS NULL AND NEW.url IS NOT NULL;
    SELECT RAISE(ABORT, 'insert on grid_packages violates constraint: direct_download must be set when url is not NULL')
        WHERE NEW.direct_download IS NULL AND NEW.url IS NOT NULL;
END;

CREATE TRIGGER grid_alternatives_insert_trigger
BEFORE INSERT ON grid_alternatives
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: original_grid_name must be referenced in grid_transformation.grid_name or in other_transformation.method_name')
        WHERE NEW.original_grid_name NOT LIKE 'NOT-YET-IN-GRID-TRANSFORMATION-%' AND
              NOT EXISTS (
                SELECT 1 FROM grid_transformation WHERE grid_name = NEW.original_grid_name
                UNION ALL
                SELECT 1 FROM other_transformation WHERE
                    (method_auth_name = 'PROJ' AND
                    method_name LIKE '%' || NEW.original_grid_name || '%') OR grid_name = NEW.original_grid_name);
END;

CREATE TRIGGER other_transformation_insert_trigger
BEFORE INSERT ON other_transformation
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: (auth_name, code) must not already exist in coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.auth_name = NEW.auth_name AND covwv.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.name = NEW.name AND covwv.deprecated = 0 AND NEW.deprecated = 0
        AND NEW.auth_name IN (SELECT auth_name FROM builtin_authorities WHERE auth_name != 'IGNF')
    );

    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: source_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.source_crs_auth_name AND crs_view.code = NEW.source_crs_code);

    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: target_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.target_crs_auth_name AND crs_view.code = NEW.target_crs_code);

    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: source_crs must not be deprecated when other_transformation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.source_crs_auth_name AND crs.code = NEW.source_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');
    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: target_crs must not be deprecated when other_transformation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.target_crs_auth_name AND crs.code = NEW.target_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');

    -- check that transformations operations between vertical CRS are from/into a vertical CRS
    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: transformation operating on vertical CRS must have a source CRS being a vertical CRS')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name IN ('Vertical Offset', 'Height Depth Reversal', 'Change of Vertical Unit') AND
              NOT EXISTS (SELECT 1 FROM vertical_crs crs WHERE
                          crs.auth_name = NEW.source_crs_auth_name AND crs.code = NEW.source_crs_code);

    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: transformation operating on vertical CRS must have a target CRS being a vertical CRS')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name IN ('Vertical Offset', 'Height Depth Reversal', 'Change of Vertical Unit') AND
              NOT EXISTS (SELECT 1 FROM vertical_crs crs WHERE
                          crs.auth_name = NEW.target_crs_auth_name AND crs.code = NEW.target_crs_code);

    -- check that 'Geographic2D with Height Offsets' transformations have a compound CRS with a geog2D as source
    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: a transformation Geographic2D with Height Offsets must have a compound CRS with a geog2D as source')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name = 'Geographic2D with Height Offsets' AND
              NOT EXISTS (SELECT 1 FROM compound_crs ccrs
                      LEFT JOIN geodetic_crs gcrs ON
                          gcrs.auth_name = horiz_crs_auth_name AND gcrs.code = horiz_crs_code
                      WHERE
                          ccrs.auth_name = NEW.source_crs_auth_name AND ccrs.code = NEW.source_crs_code
                          AND gcrs.type = 'geographic 2D');

    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: a transformation Geographic2D with Height Offsets must have a geographic 3D CRS as target')
        WHERE NEW.deprecated = 0 AND
              NEW.method_name = 'Geographic2D with Height Offsets' AND
              NOT EXISTS (SELECT 1 FROM geodetic_crs gcrs WHERE
                          gcrs.auth_name = NEW.target_crs_auth_name AND gcrs.code = NEW.target_crs_code
                          AND gcrs.type = 'geographic 3D');

    -- check that operations with 'Geocen translations by grid (gtg) & Geocen translations NEU velocities (gtg)' and 'Position Vector (geocen) & Geocen translations NEU velocities (gtg)' methods are properly registered
    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: source_crs(auth_name, code) must be Geocentric')
        WHERE NEW.method_name IN ('Geocen translations by grid (gtg) & Geocen translations NEU velocities (gtg)', 'Position Vector (geocen) & Geocen translations NEU velocities (gtg)') AND NOT EXISTS (SELECT 1 FROM geodetic_crs gcrs WHERE gcrs.auth_name = NEW.source_crs_auth_name AND gcrs.code = NEW.source_crs_code AND type = 'geocentric');

    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: target_crs(auth_name, code) must be Geocentric')
        WHERE NEW.method_name IN ('Geocen translations by grid (gtg) & Geocen translations NEU velocities (gtg)', 'Position Vector (geocen) & Geocen translations NEU velocities (gtg)') AND NOT EXISTS (SELECT 1 FROM geodetic_crs gcrs WHERE gcrs.auth_name = NEW.target_crs_auth_name AND gcrs.code = NEW.target_crs_code AND type = 'geocentric');

END;

CREATE TRIGGER concatenated_operation_insert_trigger
BEFORE INSERT ON concatenated_operation
FOR EACH ROW BEGIN

    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: (auth_name, code) must not already exist in coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.auth_name = NEW.auth_name AND covwv.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: name (of a non-deprecated entry) must not already exist in (a non-deprecated entry of) coordinate_operation_with_conversion_view')
        WHERE EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.name = NEW.name AND covwv.deprecated = 0 AND NEW.deprecated = 0
        AND NEW.auth_name IN (SELECT auth_name FROM builtin_authorities WHERE auth_name != 'IGNF')
    );

    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: source_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.source_crs_auth_name AND crs_view.code = NEW.source_crs_code);

    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: target_crs(auth_name, code) not found')
        WHERE NOT EXISTS (SELECT 1 FROM crs_view WHERE crs_view.auth_name = NEW.target_crs_auth_name AND crs_view.code = NEW.target_crs_code);

    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: source_crs must not be deprecated when concatenated_operation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.source_crs_auth_name AND crs.code = NEW.source_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');
    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: target_crs must not be deprecated when concatenated_operation is not deprecated')
        WHERE EXISTS(SELECT 1 FROM crs_view crs WHERE crs.auth_name = NEW.target_crs_auth_name AND crs.code = NEW.target_crs_code AND crs.deprecated != 0) AND NEW.deprecated = 0 AND NOT (NEW.auth_name = 'ESRI');
END;

CREATE TRIGGER concatenated_operation_step_insert_trigger
BEFORE INSERT ON concatenated_operation_step
FOR EACH ROW BEGIN

    SELECT RAISE(ABORT, 'insert on concatenated_operation_step violates constraint: (step_auth_name, step_code) must already exist in coordinate_operation_with_conversion_view')
        WHERE NOT EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.auth_name = NEW.step_auth_name AND covwv.code = NEW.step_code);

    SELECT RAISE(ABORT, 'insert on concatenated_operation_step violates constraint: step should not be a concatenated_operation')
        WHERE EXISTS(SELECT 1 FROM concatenated_operation WHERE auth_name = NEW.step_auth_name AND code = NEW.step_code);

    -- NOTE: it would be good to be able to do the same for the last step, but
    -- we don't know at INSERT time which one is going to be the last one...
    SELECT RAISE(ABORT, 'insert on concatenated_operation_step violates constraint: step 1 must be a conversion or its source_crs or target_crs must be the source_crs of the concatenated_operation')
        WHERE
        NEW.step_number = 1
        -- skip deprecated concatenated operations, or some blocklisted EPSG operations
        AND NOT EXISTS (
            SELECT 1 FROM concatenated_operation concat_op WHERE
            concat_op.auth_name = NEW.operation_auth_name AND concat_op.code = NEW.operation_code
            AND concat_op.deprecated = 1)

        -- check if source_crs of step 1 is the source_crs of the concatenated_operation (by CRS code)
        AND NOT EXISTS (
            SELECT 1 FROM coordinate_operation_view step_op
            LEFT JOIN concatenated_operation concat_op ON
            concat_op.auth_name = NEW.operation_auth_name AND concat_op.code = NEW.operation_code
            WHERE concat_op.deprecated = 0
            AND step_op.auth_name = NEW.step_auth_name AND step_op.code = NEW.step_code
            AND concat_op.source_crs_auth_name = step_op.source_crs_auth_name
            AND concat_op.source_crs_code = step_op.source_crs_code)

        -- same as above, but check by CRS name, and only for geodetic CRS.
        -- For example the concatenated operation EPSG:9683 ("ITRF2014 to GDA94 (2)")
        -- has EPSG:9000 "ITRF2014" (geographic 2D) as source CRS
        -- but its first step is EPSG:8049 ("ITRF2014 to GDA2020 (1)") which has
        -- EPSG:7789 "ITRF2014" (geocentric) as source CRS !
        AND NOT EXISTS (
            SELECT 1 FROM coordinate_operation_view step_op
            LEFT JOIN concatenated_operation concat_op ON
            concat_op.auth_name = NEW.operation_auth_name AND concat_op.code = NEW.operation_code
            LEFT JOIN geodetic_crs concat_op_source_crs ON
            concat_op_source_crs.auth_name = concat_op.source_crs_auth_name
            AND concat_op_source_crs.code = concat_op.source_crs_code
            LEFT JOIN geodetic_crs step_op_source_crs ON
            step_op_source_crs.auth_name = step_op.source_crs_auth_name
            AND step_op_source_crs.code = step_op.source_crs_code
            WHERE concat_op.deprecated = 0
            AND step_op.auth_name = NEW.step_auth_name AND step_op.code = NEW.step_code
            AND concat_op_source_crs.name = step_op_source_crs.name)

        -- case for EPSG:10146 "INAGeoid2020 v1 height to INAGeoid v2 height (1)"
        -- that has EPSG:9471 "INAGeoid2020 v1 height" as source CRS
        -- but its first step is EPSG:9629 "SRGI2013 to SRGI2013 + INAGeoid2020 v1 height (1)"
        -- that has EPSG:9529 "SRGI2013 + INAGeoid2020 v1 height" as target CRS
        AND NOT EXISTS (
            SELECT 1 FROM coordinate_operation_view step_op
            LEFT JOIN concatenated_operation concat_op ON
            concat_op.auth_name = NEW.operation_auth_name AND concat_op.code = NEW.operation_code
            LEFT JOIN vertical_crs concat_op_source_crs ON
            concat_op_source_crs.auth_name = concat_op.source_crs_auth_name
            AND concat_op_source_crs.code = concat_op.source_crs_code
            LEFT JOIN compound_crs step_op_target_crs ON
            step_op_target_crs.auth_name = step_op.target_crs_auth_name
            AND step_op_target_crs.code = step_op.target_crs_code
            WHERE concat_op.deprecated = 0
            AND step_op.auth_name = NEW.step_auth_name AND step_op.code = NEW.step_code
            AND step_op_target_crs.name LIKE '% + '|| concat_op_source_crs.name)

        -- or if source_crs of step 1 is the target_crs of the concatenated_operation
        AND NOT EXISTS (
            SELECT 1 FROM coordinate_operation_view step_op
            LEFT JOIN concatenated_operation concat_op ON
            concat_op.auth_name = NEW.operation_auth_name AND concat_op.code = NEW.operation_code
            WHERE concat_op.deprecated = 0
            AND step_op.auth_name = NEW.step_auth_name AND step_op.code = NEW.step_code
            AND concat_op.source_crs_auth_name = step_op.target_crs_auth_name
            AND concat_op.source_crs_code = step_op.target_crs_code)

        -- or if source_crs of step 1 is a conversion
        AND NOT EXISTS (
            SELECT 1 FROM conversion_table step_op
            LEFT JOIN concatenated_operation concat_op ON
            concat_op.auth_name = NEW.operation_auth_name AND concat_op.code = NEW.operation_code
            WHERE concat_op.deprecated = 0
            AND step_op.auth_name = NEW.step_auth_name AND step_op.code = NEW.step_code)
    ;
END;

CREATE TRIGGER geoid_model_insert_trigger
BEFORE INSERT ON geoid_model
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on geoid_model violates constraint: (operation_auth_name, operation_code) must already exist in coordinate_operation_with_conversion_view')
        WHERE NOT EXISTS (SELECT 1 FROM coordinate_operation_with_conversion_view covwv WHERE covwv.auth_name = NEW.operation_auth_name AND covwv.code = NEW.operation_code);
END;

CREATE TRIGGER alias_name_insert_trigger
BEFORE INSERT ON alias_name
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on alias_name violates constraint: new entry refers to unexisting code')
        WHERE NOT EXISTS (SELECT 1 FROM object_view o WHERE o.table_name = NEW.table_name AND o.auth_name = NEW.auth_name AND o.code = NEW.code);
END;

CREATE TRIGGER supersession_insert_trigger
BEFORE INSERT ON supersession
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on supersession violates constraint: superseded entry refers to unexisting code')
        WHERE NOT EXISTS (SELECT 1 FROM object_view o WHERE o.table_name = NEW.superseded_table_name AND o.auth_name = NEW.superseded_auth_name AND o.code = NEW.superseded_code);

    SELECT RAISE(ABORT, 'insert on supersession violates constraint: replacement entry refers to unexisting code')
        WHERE NOT EXISTS (SELECT 1 FROM object_view o WHERE o.table_name = NEW.replacement_table_name AND o.auth_name = NEW.replacement_auth_name AND o.code = NEW.replacement_code);
END;

CREATE TRIGGER deprecation_insert_trigger
BEFORE INSERT ON deprecation
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on deprecation violates constraint: deprecated entry refers to unexisting code')
        WHERE NOT EXISTS (SELECT 1 FROM object_view o WHERE o.table_name = NEW.table_name AND o.auth_name = NEW.deprecated_auth_name AND o.code = NEW.deprecated_code);

    SELECT RAISE(ABORT, 'insert on deprecation violates constraint: replacement entry refers to unexisting code')
        WHERE NOT EXISTS (SELECT 1 FROM object_view o WHERE o.table_name = NEW.table_name AND o.auth_name = NEW.replacement_auth_name AND o.code = NEW.replacement_code);
END;

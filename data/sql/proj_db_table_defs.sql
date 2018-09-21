--- Table structures

PRAGMA page_size = 4096;

CREATE TABLE metadata(
    record TEXT NOT NULL
);

CREATE TABLE unit_of_measure(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    conv_factor FLOAT,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_unit_of_measure PRIMARY KEY (auth_name, code)
);

CREATE TABLE ellipsoid (
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    semi_major_axis FLOAT NOT NULL,
    uom_auth_name TEXT NOT NULL,
    uom_code TEXT NOT NULL,
    inv_flattening FLOAT,
    semi_minor_axis FLOAT,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_ellipsoid PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_ellipsoid_unit_of_measure FOREIGN KEY (uom_auth_name, uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TABLE area(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    south_lat FLOAT,
    north_lat FLOAT,
    west_lon FLOAT,
    east_lon FLOAT,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_area PRIMARY KEY (auth_name, code)
);

CREATE TABLE prime_meridian(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    longitude FLOAT NOT NULL,
    uom_auth_name TEXT NOT NULL,
    uom_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_prime_meridian PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_prime_meridian_unit_of_measure FOREIGN KEY (uom_auth_name, uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TABLE geodetic_datum (
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    ellipsoid_auth_name TEXT NOT NULL,
    ellipsoid_code TEXT NOT NULL,
    prime_meridian_auth_name TEXT NOT NULL,
    prime_meridian_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_geodetic_datum PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_geodetic_datum_ellipsoid FOREIGN KEY (ellipsoid_auth_name, ellipsoid_code) REFERENCES ellipsoid(auth_name, code),
    CONSTRAINT fk_geodetic_datum_prime_meridian FOREIGN KEY (prime_meridian_auth_name, prime_meridian_code) REFERENCES prime_meridian(auth_name, code),
    CONSTRAINT fk_geodetic_datum_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE vertical_datum (
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_vertical_datum PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_vertical_datum_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE coordinate_system(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    dimension SMALLINT NOT NULL,
    CONSTRAINT pk_coordinate_system PRIMARY KEY (auth_name, code)
);

CREATE TABLE axis(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    abbrev TEXT NOT NULL,
    orientation TEXT NOT NULL,
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    coordinate_system_order SMALLINT NOT NULL,
    uom_auth_name TEXT NOT NULL,
    uom_code TEXT NOT NULL,
    CONSTRAINT pk_axis PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_axis_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_axis_unit_of_measure FOREIGN KEY (uom_auth_name, uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TABLE crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    type TEXT NOT NULL,
    CONSTRAINT pk_geodetic_crs PRIMARY KEY (auth_name, code)
);

CREATE TABLE geodetic_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    datum_auth_name TEXT NOT NULL,
    datum_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_geodetic_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_geodetic_crs_crs FOREIGN KEY (auth_name, code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_geodetic_crs_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_geodetic_crs_datum FOREIGN KEY (datum_auth_name, datum_code) REFERENCES geodetic_datum(auth_name, code),
    CONSTRAINT fk_geodetic_crs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE vertical_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    datum_auth_name TEXT NOT NULL,
    datum_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_vertical_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_vertical_crs_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_vertical_crs_datum FOREIGN KEY (datum_auth_name, datum_code) REFERENCES vertical_datum(auth_name, code),
    CONSTRAINT fk_vertical_crs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE coordinate_operation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    type TEXT NOT NULL,
    CONSTRAINT pk_coordinate_operation PRIMARY KEY (auth_name, code)
);

CREATE TABLE conversion(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    method_auth_name TEXT NOT NULL,
    method_code TEXT NOT NULL,
    method_name NOT NULL,

    param1_auth_name TEXT,
    param1_code TEXT,
    param1_name TEXT,
    param1_value FLOAT,
    param1_uom_auth_name TEXT,
    param1_uom_code TEXT,

    param2_auth_name TEXT,
    param2_code TEXT,
    param2_name TEXT,
    param2_value FLOAT,
    param2_uom_auth_name TEXT,
    param2_uom_code TEXT,

    param3_auth_name TEXT,
    param3_code TEXT,
    param3_name TEXT,
    param3_value FLOAT,
    param3_uom_auth_name TEXT,
    param3_uom_code TEXT,

    param4_auth_name TEXT,
    param4_code TEXT,
    param4_name TEXT,
    param4_value FLOAT,
    param4_uom_auth_name TEXT,
    param4_uom_code TEXT,

    param5_auth_name TEXT,
    param5_code TEXT,
    param5_name TEXT,
    param5_value FLOAT,
    param5_uom_auth_name TEXT,
    param5_uom_code TEXT,

    param6_auth_name TEXT,
    param6_code TEXT,
    param6_name TEXT,
    param6_value FLOAT,
    param6_uom_auth_name TEXT,
    param6_uom_code TEXT,

    param7_auth_name TEXT,
    param7_code TEXT,
    param7_name TEXT,
    param7_value FLOAT,
    param7_uom_auth_name TEXT,
    param7_uom_code TEXT,

    CONSTRAINT pk_conversion PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_conversion_coordinate_operation FOREIGN KEY (auth_name, code) REFERENCES coordinate_operation(auth_name, code)
);

CREATE TABLE projected_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    geodetic_crs_auth_name TEXT NOT NULL,
    geodetic_crs_code TEXT NOT NULL,
    conversion_auth_name TEXT NOT NULL,
    conversion_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_projected_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_projected_crs_crs FOREIGN KEY (auth_name, code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_projected_crs_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_projected_crs_geodetic_crs FOREIGN KEY (geodetic_crs_auth_name, geodetic_crs_code) REFERENCES geodetic_crs(auth_name, code),
    CONSTRAINT fk_projected_crs_conversion FOREIGN KEY (conversion_auth_name, conversion_code) REFERENCES conversion(auth_name, code),
    CONSTRAINT fk_projected_crs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE compound_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    horiz_crs_auth_name TEXT NOT NULL,
    horiz_crs_code TEXT NOT NULL,
    vertical_crs_auth_name TEXT NOT NULL,
    vertical_crs_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL,
    CONSTRAINT pk_compound_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_compound_crs_crs FOREIGN KEY (auth_name, code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_compound_crs_horiz_crs FOREIGN KEY (horiz_crs_auth_name, horiz_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_compound_crs_vertical_crs FOREIGN KEY (vertical_crs_auth_name, vertical_crs_code) REFERENCES vertical_crs(auth_name, code),
    CONSTRAINT fk_compoundcrs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE helmert_transformation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    method_auth_name TEXT NOT NULL,
    method_code TEXT NOT NULL,
    method_name NOT NULL,

    source_crs_auth_name TEXT NOT NULL,
    source_crs_code TEXT NOT NULL,
    target_crs_auth_name TEXT NOT NULL,
    target_crs_code TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    accuracy FLOAT,

    tx FLOAT NOT NULL,
    ty FLOAT NOT NULL,
    tz FLOAT NOT NULL,
    translation_uom_auth_name TEXT NOT NULL,
    translation_uom_code TEXT NOT NULL,
    rx FLOAT,
    ry FLOAT,
    rz FLOAT,
    rotation_uom_auth_name TEXT,
    rotation_uom_code TEXT,
    scale_difference FLOAT,
    scale_difference_uom_auth_name TEXT,
    scale_difference_uom_code TEXT,
    rate_tx FLOAT,
    rate_ty FLOAT,
    rate_tz FLOAT,
    rate_translation_uom_auth_name TEXT,
    rate_translation_uom_code TEXT,
    rate_rx FLOAT,
    rate_ry FLOAT,
    rate_rz FLOAT,
    rate_rotation_uom_auth_name TEXT,
    rate_rotation_uom_code TEXT,
    rate_scale_difference FLOAT,
    rate_scale_difference_uom_auth_name TEXT,
    rate_scale_difference_uom_code TEXT,
    epoch FLOAT,
    epoch_uom_auth_name TEXT,
    epoch_uom_code TEXT,

    deprecated BOOLEAN NOT NULL,

    CONSTRAINT pk_helmert_transformation PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_helmert_transformation_source_crs FOREIGN KEY (source_crs_auth_name, source_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_helmert_transformation_target_crs FOREIGN KEY (target_crs_auth_name, target_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_helmert_transformation_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code),
    CONSTRAINT fk_helmert_transformation_coordinate_operation FOREIGN KEY (auth_name, code) REFERENCES coordinate_operation(auth_name, code)
);

CREATE TABLE grid_transformation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    method_auth_name TEXT NOT NULL,
    method_code TEXT NOT NULL,
    method_name NOT NULL,

    source_crs_auth_name TEXT NOT NULL,
    source_crs_code TEXT NOT NULL,
    target_crs_auth_name TEXT NOT NULL,
    target_crs_code TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    accuracy FLOAT,

    grid_param_auth_name TEXT NOT NULL,
    grid_param_code TEXT NOT NULL,
    grid_name TEXT NOT NULL,

    grid2_param_auth_name TEXT,
    grid2_param_code TEXT,
    grid2_name TEXT,

    interpolation_crs_auth_name TEXT,
    interpolation_crs_code TEXT,

    deprecated BOOLEAN NOT NULL,

    CONSTRAINT pk_grid_transformation PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_grid_transformation_source_crs FOREIGN KEY (source_crs_auth_name, source_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_grid_transformation_target_crs FOREIGN KEY (target_crs_auth_name, target_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_grid_transformation_interpolation_crs FOREIGN KEY (interpolation_crs_auth_name, interpolation_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_grid_transformation_transformation_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE concatenated_operation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    source_crs_auth_name TEXT NOT NULL,
    source_crs_code TEXT NOT NULL,
    target_crs_auth_name TEXT NOT NULL,
    target_crs_code TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    accuracy FLOAT,

    step1_auth_name TEXT NOT NULL,
    step1_code TEXT NOT NULL,

    step2_auth_name TEXT NOT NULL,
    step2_code TEXT NOT NULL,

    step3_auth_name TEXT,
    step3_code TEXT,

    deprecated BOOLEAN NOT NULL,

    CONSTRAINT pk_concatenated_operation PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_concatenated_operation_source_crs FOREIGN KEY (source_crs_auth_name, source_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_concatenated_operation_target_crs FOREIGN KEY (target_crs_auth_name, target_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_concatenated_operation_step1 FOREIGN KEY (step1_auth_name, step1_code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_concatenated_operation_step2 FOREIGN KEY (step2_auth_name, step2_code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_concatenated_operation_step3 FOREIGN KEY (step3_auth_name, step3_code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_concatenated_operation_transformation_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE VIEW coordinate_operation_view AS
    SELECT 'grid_transformation' AS table_name, auth_name, code, name,
           method_auth_name, method_code, method_name, source_crs_auth_name,
           source_crs_code, target_crs_auth_name, target_crs_code,
           area_of_use_auth_name, area_of_use_code,
           accuracy, deprecated FROM grid_transformation
    UNION ALL
    SELECT 'helmert_transformation' AS table_name, auth_name, code, name,
           method_auth_name, method_code, method_name, source_crs_auth_name,
           source_crs_code, target_crs_auth_name, target_crs_code,
           area_of_use_auth_name, area_of_use_code,
           accuracy, deprecated FROM helmert_transformation
    UNION ALL
    SELECT 'concatenated_operation' AS table_name, auth_name, code, name,
           NULL, NULL, NULL, source_crs_auth_name,
           source_crs_code, target_crs_auth_name, target_crs_code,
           area_of_use_auth_name, area_of_use_code,
           accuracy, deprecated FROM concatenated_operation
;

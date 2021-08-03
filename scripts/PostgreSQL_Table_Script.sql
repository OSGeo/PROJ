
CREATE TABLE epsg_alias ( 
alias_code                                         INTEGER NOT NULL,
object_table_name                                  VARCHAR(80) NOT NULL, 
object_code                                        INTEGER NOT NULL, 
naming_system_code                                 INTEGER NOT NULL, 
alias                                              VARCHAR(80) NOT NULL, 
remarks                                            VARCHAR(254), 
CONSTRAINT pk_alias PRIMARY KEY ( alias_code ) );

CREATE TABLE epsg_change ( 
change_id                                          DOUBLE PRECISION NOT NULL UNIQUE, 
report_date                                        DATE NOT NULL, 
date_closed                                        DATE, 
reporter                                           VARCHAR(254) NOT NULL, 
request                                            VARCHAR(254) NOT NULL, 
tables_affected                                    VARCHAR(254), 
codes_affected                                     VARCHAR(254), 
change_comment                                     VARCHAR(254), 
action                                             VARCHAR(4000) );

CREATE TABLE epsg_conventionalrs (
conventional_rs_code								INTEGER NOT NULL,
conventional_rs_name								VARCHAR(80) NOT NULL,
remarks												VARCHAR(254) NULL,
information_source									VARCHAR(254) NULL,
data_source											VARCHAR(40) NOT NULL,
revision_date										DATE NOT NULL,
change_id											VARCHAR(254) NULL,
deprecated											SMALLINT NOT NULL,
CONSTRAINT pk_conventional_rs PRIMARY KEY ( conventional_rs_code ) );

CREATE TABLE epsg_coordinateaxis ( 
coord_axis_code                                    INTEGER UNIQUE, 
coord_sys_code                                     INTEGER NOT NULL, 
coord_axis_name_code                               INTEGER NOT NULL, 
coord_axis_orientation                             VARCHAR(24) NOT NULL, 
coord_axis_abbreviation                            VARCHAR(24) NOT NULL, 
uom_code                                           INTEGER, 
coord_axis_order                                   SMALLINT NOT NULL, 
CONSTRAINT pk_coordinateaxis PRIMARY KEY ( coord_sys_code, coord_axis_name_code ) );

CREATE TABLE epsg_coordinateaxisname ( 
coord_axis_name_code                               INTEGER NOT NULL, 
coord_axis_name                                    VARCHAR(80) NOT NULL, 
description                                        VARCHAR(254), 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_coordinateaxisname PRIMARY KEY ( coord_axis_name_code ) );

CREATE TABLE epsg_coordinatereferencesystem ( 
coord_ref_sys_code                                 INTEGER NOT NULL, 
coord_ref_sys_name                                 VARCHAR(80) NOT NULL, 
area_of_use_code                                   INTEGER, 
coord_ref_sys_kind                                 VARCHAR(24) NOT NULL, 
coord_sys_code                                     INTEGER, 
datum_code                                         INTEGER, 
base_crs_code	                                   INTEGER, 
projection_conv_code                               INTEGER, 
cmpd_horizcrs_code                                 INTEGER, 
cmpd_vertcrs_code                                  INTEGER, 
crs_scope                                          VARCHAR(254), 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
show_crs                                           SMALLINT NOT NULL, 
CONSTRAINT pk_coordinatereferencesystem PRIMARY KEY ( coord_ref_sys_code ) );

CREATE TABLE epsg_coordinatesystem ( 
coord_sys_code                                     INTEGER NOT NULL, 
coord_sys_name                                     VARCHAR(254) NOT NULL, 
coord_sys_type                                     VARCHAR(24) NOT NULL, 
dimension                                          SMALLINT NOT NULL, 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(50) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_coordinatesystem PRIMARY KEY ( coord_sys_code ) );

CREATE TABLE epsg_coordoperation ( 
coord_op_code                                      INTEGER NOT NULL, 
coord_op_name                                      VARCHAR(80) NOT NULL, 
coord_op_type                                      VARCHAR(24) NOT NULL, 
source_crs_code                                    INTEGER, 
target_crs_code                                    INTEGER, 
coord_tfm_version                                  VARCHAR(24), 
coord_op_variant                                   SMALLINT, 
area_of_use_code                                   INTEGER, 
coord_op_scope                                     VARCHAR(254), 
coord_op_accuracy                                  FLOAT, 
coord_op_method_code                               INTEGER, 
uom_code_source_coord_diff                         INTEGER, 
uom_code_target_coord_diff                         INTEGER, 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
show_operation                                     SMALLINT NOT NULL, 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_coordinate_operation PRIMARY KEY ( coord_op_code ) );

CREATE TABLE epsg_coordoperationmethod ( 
coord_op_method_code                               INTEGER NOT NULL, 
coord_op_method_name                               VARCHAR(50) NOT NULL, 
reverse_op                                         SMALLINT NOT NULL, 
formula                                            VARCHAR(4000), 
example                                            VARCHAR(4000), 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_coordinate_operationmethod PRIMARY KEY ( coord_op_method_code ) );

CREATE TABLE epsg_coordoperationparam ( 
parameter_code                                     INTEGER NOT NULL, 
parameter_name                                     VARCHAR(80) NOT NULL, 
description                                        VARCHAR(4000), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_coordinate_operationparamet PRIMARY KEY ( parameter_code ) );

CREATE TABLE epsg_coordoperationparamusage ( 
coord_op_method_code                               INTEGER NOT NULL, 
parameter_code                                     INTEGER NOT NULL, 
sort_order                                         SMALLINT NOT NULL, 
param_sign_reversal                                VARCHAR(3), 
CONSTRAINT pk_coordinate_operationparame2 PRIMARY KEY ( parameter_code, coord_op_method_code ) );

CREATE TABLE epsg_coordoperationparamvalue ( 
coord_op_code                                      INTEGER NOT NULL, 
coord_op_method_code                               INTEGER NOT NULL, 
parameter_code                                     INTEGER NOT NULL, 
parameter_value                                    DOUBLE PRECISION, 
param_value_file_ref                               VARCHAR(254), 
uom_code                                           INTEGER, 
CONSTRAINT pk_coordinate_operationparame3 PRIMARY KEY ( coord_op_code, parameter_code, coord_op_method_code ) );

CREATE TABLE epsg_coordoperationpath ( 
concat_operation_code                              INTEGER NOT NULL, 
single_operation_code                              INTEGER NOT NULL, 
op_path_step                                       SMALLINT NOT NULL, 
CONSTRAINT pk_coordinate_operationpath PRIMARY KEY ( concat_operation_code, single_operation_code ) );

CREATE TABLE epsg_datum ( 
datum_code                                         INTEGER NOT NULL, 
datum_name                                         VARCHAR(80) NOT NULL, 
datum_type                                         VARCHAR(24) NOT NULL, 
origin_description                                 VARCHAR(254), 
realization_epoch                                  VARCHAR(10), 
ellipsoid_code                                     INTEGER, 
prime_meridian_code                                INTEGER, 
area_of_use_code                                   INTEGER, 
datum_scope                                        VARCHAR(254), 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
conventional_rs_code							   INTEGER,
publication_date								   VARCHAR(10),
frame_reference_epoch							   FLOAT,
realization_method_code							   INTEGER,
CONSTRAINT pk_datum PRIMARY KEY ( datum_code ) );

CREATE TABLE epsg_datumensemble (
datum_ensemble_code								   INTEGER NOT NULL,
ensemble_accuracy								   FLOAT,
CONSTRAINT pk_datumensemble PRIMARY KEY ( datum_ensemble_code) );

CREATE TABLE epsg_datumensemblemember(
datum_ensemble_code								   INTEGER NOT NULL,
datum_code										   INTEGER NOT NULL,
datum_sequence									   INTEGER NOT NULL,
CONSTRAINT pk_datumensemble_members PRIMARY KEY ( datum_ensemble_code, datum_code ) );

CREATE TABLE epsg_datumrealizationmethod (
realization_method_code							   INTEGER NOT NULL,
realization_method_name							   VARCHAR(80) NOT NULL,
remarks											   VARCHAR(254),
information_source								   VARCHAR(254),
data_source										   VARCHAR(40) NOT NULL,
revision_date									   DATE NOT NULL,
change_id										   VARCHAR(254),
deprecated										   SMALLINT NOT NULL,
CONSTRAINT pk_datumrealizationmethod PRIMARY KEY ( realization_method_code ) );

CREATE TABLE epsg_definingoperation (
crs_code										   INTEGER NOT NULL,
ct_code											   INTEGER NOT NULL,
CONSTRAINT pk_definingoperation PRIMARY KEY ( crs_code, ct_code ) );

CREATE TABLE epsg_deprecation ( 
deprecation_id                                     INTEGER NOT NULL,
deprecation_date                                   DATE, 
change_id                                          DOUBLE PRECISION NOT NULL, 
object_table_name                                  VARCHAR(80), 
object_code                                        INTEGER, 
replaced_by                                        INTEGER, 
deprecation_reason                                 VARCHAR(254), 
CONSTRAINT pk_deprecation PRIMARY KEY ( deprecation_id ) );

CREATE TABLE epsg_ellipsoid ( 
ellipsoid_code                                     INTEGER NOT NULL, 
ellipsoid_name                                     VARCHAR(80) NOT NULL, 
semi_major_axis                                    DOUBLE PRECISION NOT NULL, 
uom_code                                           INTEGER NOT NULL, 
inv_flattening                                     DOUBLE PRECISION, 
semi_minor_axis                                    DOUBLE PRECISION, 
ellipsoid_shape                                    SMALLINT NOT NULL, 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_ellipsoid PRIMARY KEY ( ellipsoid_code ) );

CREATE TABLE epsg_extent (
extent_code										   INTEGER NOT NULL,
extent_name										   VARCHAR(80) NOT NULL,
extent_description								   VARCHAR(4000) NOT NULL,
bbox_south_bound_lat							   FLOAT,
bbox_west_bound_lon								   FLOAT,
bbox_north_bound_lat							   FLOAT,
bbox_east_bound_lon								   FLOAT,
area_polygon_file_ref							   VARCHAR(254),
iso_a2_code										   VARCHAR(2),
iso_a3_code										   VARCHAR(3),
iso_n_code										   INTEGER,
vertical_extent_min								   FLOAT,
vertical_extent_max								   FLOAT,
vertical_extent_crs_code						   INTEGER,
temporal_extent_begin							   VARCHAR(254),
temporal_extent_end								   VARCHAR(254),
remarks											   VARCHAR(254),
information_source								   VARCHAR(254),
data_source										   VARCHAR(40) NOT NULL,
revision_date									   DATE NOT NULL,
change_id										   VARCHAR(254),
deprecated										   SMALLINT NOT NULL,
CONSTRAINT pk_extent PRIMARY KEY ( extent_code ) );

CREATE TABLE epsg_namingsystem ( 
naming_system_code                                 INTEGER NOT NULL, 
naming_system_name                                 VARCHAR(80) NOT NULL, 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_namingsystem PRIMARY KEY ( naming_system_code ) );

CREATE TABLE epsg_primemeridian ( 
prime_meridian_code                                INTEGER NOT NULL, 
prime_meridian_name                                VARCHAR(80) NOT NULL, 
greenwich_longitude                                DOUBLE PRECISION NOT NULL, 
uom_code                                           INTEGER NOT NULL, 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_primemeridian PRIMARY KEY ( prime_meridian_code ) );

CREATE TABLE epsg_scope (
scope_code										   INTEGER NOT NULL,
scope											   VARCHAR(254) NOT NULL,
remarks											   VARCHAR(254),
information_source								   VARCHAR(254),
data_source										   VARCHAR(40) NOT NULL,
revision_date									   DATE NOT NULL,
change_id										   VARCHAR(254),
deprecated										   SMALLINT NOT NULL,
CONSTRAINT pk_scope PRIMARY KEY ( scope_code ) );

CREATE TABLE epsg_supersession ( 
supersession_id                                    INTEGER NOT NULL,
object_table_name                                  VARCHAR(80) NOT NULL, 
object_code                                        INTEGER NOT NULL, 
superseded_by                                      INTEGER, 
supersession_type                                  VARCHAR(50), 
supersession_year                                  SMALLINT, 
remarks                                            VARCHAR(254), 
CONSTRAINT pk_supersession PRIMARY KEY ( supersession_id ) );

CREATE TABLE epsg_unitofmeasure ( 
uom_code                                           INTEGER NOT NULL, 
unit_of_meas_name                                  VARCHAR(80) NOT NULL, 
unit_of_meas_type                                  VARCHAR(50), 
target_uom_code                                    INTEGER NOT NULL, 
factor_b                                           DOUBLE PRECISION, 
factor_c                                           DOUBLE PRECISION, 
remarks                                            VARCHAR(254), 
information_source                                 VARCHAR(254), 
data_source                                        VARCHAR(40) NOT NULL, 
revision_date                                      DATE NOT NULL, 
change_id                                          VARCHAR(254), 
deprecated                                         SMALLINT NOT NULL, 
CONSTRAINT pk_unitofmeasure PRIMARY KEY ( uom_code ) );

CREATE TABLE epsg_usage (
usage_code										   SERIAL,
object_table_name								   VARCHAR(80) NOT NULL,
object_code										   INTEGER NOT NULL,
extent_code										   INTEGER NOT NULL,
scope_code										   INTEGER NOT NULL,
 CONSTRAINT pk_usage PRIMARY KEY ( usage_code) );

CREATE TABLE epsg_versionhistory ( 
version_history_code                               INTEGER NOT NULL, 
version_date                                       DATE, 
version_number                                     VARCHAR(10) NOT NULL UNIQUE, 
version_remarks                                    VARCHAR(254) NOT NULL, 
superceded_by                                      VARCHAR(10), 
supercedes                                         VARCHAR(10), 
CONSTRAINT pk_versionhistory PRIMARY KEY ( version_history_code ) );


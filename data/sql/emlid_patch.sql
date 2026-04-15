-- _RESTRICTED_TO_VERTCRS allows overlap with existing EPSG grid_transformation rows

INSERT OR IGNORE INTO builtin_authorities VALUES('CUSTOM');

INSERT INTO "main"."grid_transformation"
("auth_name", "code", "name", "method_auth_name", "method_code", "method_name", "source_crs_auth_name", "source_crs_code", "target_crs_auth_name", "target_crs_code", "accuracy", "grid_param_auth_name", "grid_param_code", "grid_param_name", "grid_name", "deprecated")
VALUES
    ('PROJ', 'EPSG_4937_TO_EPSG_3900_1', 'ETRS89 to N2000 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '3900', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'N2000.tif', 0),
    ('PROJ', 'EPSG_4937_TO_EPSG_5717_1', 'ETRS89 to N60 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '5717', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'N60.tif', 0),
    ('PROJ', 'EPSG_4937_TO_EPSG_5621_1', 'ETRS89 to EVRF2007 height, Poland', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '5621', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'gugik-geoid2011-PL-EVRF2007-NH.txt', 0),
    ('PROJ', 'EPSG_4937_TO_EPSG_5729_1', 'ETRS89 to LHN95 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '5729', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'chgeo04_etrf.tif', 0),
    ('PROJ', 'EPSG_4937_TO_EPSG_5728_1', 'ETRS89 to LN02 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '5728', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'chgeo04_HT_etrf.tif', 0),
    ('PROJ', 'EPSG_4937_TO_EPSG_5941_1', 'ETRS89 to NN2000 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '5941', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'nn2000.gtx', 0),
    ('PROJ', 'EPSG_4937_TO_EPSG_7837_1', 'ETRS89 to DHHN2016 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '7837', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'GCG2016.byn', 0),
    ('PROJ', 'EPSG_4949_TO_EPSG_7700_1', 'LKS92 to Latvia 2000 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4949', 'EPSG', '7700', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'lv_14.tif', 0),
    ('PROJ', 'EPSG_4883_TO_EPSG_5779_1', 'Slovenia 1996 to SVS2000 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4883', 'EPSG', '5779', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'sloamg2000.tif', 0),
    ('PROJ', 'EPSG_4883_TO_EPSG_8690_1', 'Slovenia 1996 to SVS2010 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4883', 'EPSG', '8690', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'svs2010.tif', 0),
    ('PROJ', 'EPSG_4955_TO_EPSG_6647_1', 'NAD83(CSRS) to CGVD2013(CGG2013) height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4955', 'EPSG', '6647', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'CGG2013n83.byn', 0),
    ('PROJ', 'EPSG_4955_TO_EPSG_9245_1', 'NAD83(CSRS) to CGVD2013(CGG2013a) height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4955', 'EPSG', '9245', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'CGG2013n83a.byn', 0),
    ('PROJ', 'EPSG_4179_TO_EPSG_4258_1', 'Pulkovo 1942(58) to ETRS89', 'EPSG', '9615', 'NTv2', 'EPSG', '4179', 'EPSG', '4258', '0.03', 'EPSG', '8656', 'Latitude and longitude difference file', 'stereo70_etrs89A.gsb', 0),
    ('PROJ', 'EPSG_4889_TO_EPSG_5610_1', 'HTRS96 to HVRS71 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4889', 'EPSG', '5610', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'HTRS96_HRG2009.tif', 0),
    ('PROJ', 'EPSG_4237_TO_EPSG_4258_1', 'HD72 to ETRS89', 'EPSG', '9615', 'NTv2', 'EPSG', '4237', 'EPSG', '4258', '0.0', 'EPSG', '8656', 'Latitude and longitude difference file', 'etrs2eov_notowgs.gsb', 0),
    ('PROJ', 'EPSG_4230_TO_EPSG_4258_1', 'ED50 to ETRS89', 'EPSG', '9615', 'NTv2', 'EPSG', '4230', 'EPSG', '4258', '0.0', 'EPSG', '8656', 'Latitude and longitude difference file', 'italy_ed50.tif', 0),
    ('PROJ', 'EPSG_4265_TO_EPSG_4258_1', 'Monte Mario to ETRS89', 'EPSG', '9615', 'NTv2', 'EPSG', '4265', 'EPSG', '4258', '0.0', 'EPSG', '8656', 'Latitude and longitude difference file', 'roma40.tif', 0),
    ('PROJ', 'EPSG_4937_TO_EPSG_5705_1', 'ETRS89 to Baltic 1977 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'EPSG', '5705', '999', 'EPSG', '8666', 'Geoid (height correction) model file', 'GEORG12.tif', 0);

INSERT INTO "main"."usage"
("auth_name", "code", "object_table_name", "object_auth_name", "object_code", "extent_auth_name", "extent_code", "scope_auth_name", "scope_code")
VALUES
    ('CUSTOM', '1000', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_3900_1', 'EPSG', '3333', 'EPSG', '1024'),
    ('CUSTOM', '1001', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_5717_1', 'EPSG', '3333', 'EPSG', '1024'),
    ('CUSTOM', '1002', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_5621_1', 'EPSG', '3293', 'EPSG', '1024'),
    ('CUSTOM', '1003', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_5729_1', 'EPSG', '1286', 'EPSG', '1024'),
    ('CUSTOM', '1004', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_5941_1', 'EPSG', '1352', 'EPSG', '1024'),
    ('CUSTOM', '1005', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_7837_1', 'EPSG', '3339', 'EPSG', '1024'),
    ('CUSTOM', '1006', 'grid_transformation', 'PROJ', 'EPSG_4949_TO_EPSG_7700_1', 'EPSG', '3268', 'EPSG', '1024'),
    ('CUSTOM', '1007', 'grid_transformation', 'PROJ', 'EPSG_4883_TO_EPSG_5779_1', 'EPSG', '3307', 'EPSG', '1024'),
    ('CUSTOM', '1008', 'grid_transformation', 'PROJ', 'EPSG_4883_TO_EPSG_8690_1', 'EPSG', '3307', 'EPSG', '1024'),
    ('CUSTOM', '1009', 'grid_transformation', 'PROJ', 'EPSG_4955_TO_EPSG_6647_1', 'EPSG', '1061', 'EPSG', '1024'),
    ('CUSTOM', '1010', 'grid_transformation', 'PROJ', 'EPSG_4955_TO_EPSG_9245_1', 'EPSG', '1061', 'EPSG', '1024'),
    ('CUSTOM', '1011', 'grid_transformation', 'PROJ', 'EPSG_4179_TO_EPSG_4258_1', 'EPSG', '1197', 'EPSG', '1024'),
    ('CUSTOM', '1012', 'grid_transformation', 'PROJ', 'EPSG_4889_TO_EPSG_5610_1', 'EPSG', '3234', 'EPSG', '1024'),
    ('CUSTOM', '1013', 'grid_transformation', 'PROJ', 'EPSG_4237_TO_EPSG_4258_1', 'EPSG', '1119', 'EPSG', '1024'),
    ('CUSTOM', '1014', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_5728_1', 'EPSG', '1286', 'EPSG', '1024'),
    ('CUSTOM', '1015', 'grid_transformation', 'PROJ', 'EPSG_4230_TO_EPSG_4258_1', 'EPSG', '1127', 'EPSG', '1024'),
    ('CUSTOM', '1016', 'grid_transformation', 'PROJ', 'EPSG_4265_TO_EPSG_4258_1', 'EPSG', '1127', 'EPSG', '1024'),
    ('CUSTOM', '1017', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_EPSG_5705_1', 'EPSG', '3251', 'EPSG', '1024');

INSERT INTO "main"."grid_alternatives"
("original_grid_name", "proj_grid_name", "proj_grid_format", "proj_method", "inverse_direction", "url", "direct_download", "open_license")
VALUES
    ('N2000.tif', 'N2000.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/N2000.tif', '1', '1'),
    ('N60.tif', 'N60-v2.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/N60-v2.tif', '1', '1'),
    ('chgeo04_etrf.tif', 'chgeo04_etrf.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/chgeo04_etrf.tif', '1', '1'),
    ('chgeo04_HT_etrf.tif', 'chgeo04_HT_etrf.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/chgeo04_HT_etrf.tif', '1', '1'),
    ('nn2000.gtx', 'nn2000-v2.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/nn2000-v2.tif', '1', '1'),
    ('GCG2016.byn', 'GCG2016.mtif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/GCG2016.mtif', '1', '1'),
    ('lv_14.tif', 'lv_14-v2.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/lv_14-v2.tif', '1', '1'),
    ('sloamg2000.tif', 'sloamg2000.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/sloamg2000.tif', '1', '1'),
    ('svs2010.tif', 'svs2010.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/svs2010.tif', '1', '1'),
    ('stereo70_etrs89A.gsb', 'stereo70_etrs89A.gsb', 'NTv2', 'hgridshift', '0', 'http://files.emlid.com/reachview3/grids-tif/stereo70_etrs89A.gsb', '1', '1'),
    ('HTRS96_HRG2009.tif', 'HTRS96_HRG2009.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/HTRS96_HRG2009.tif', '1', '1'),
    ('etrs2eov_notowgs.gsb', 'etrs2eov_notowgs.gsb', 'NTv2', 'hgridshift', '0', 'http://files.emlid.com/reachview3/grids-tif/etrs2eov_notowgs.gsb', '1', '1'),
    ('italy_ed50.tif', 'italy_ed50.tif', 'NTv2', 'hgridshift', '0', 'http://files.emlid.com/reachview3/grids-tif/italy_ed50.tif', '1', '1'),
    ('roma40.tif', 'roma40.tif', 'NTv2', 'hgridshift', '0', 'http://files.emlid.com/reachview3/grids-tif/roma40.tif', '1', '1'),
    ('GEORG12.tif', 'GEORG12.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/GEORG12.tif', '1', '1');


-- Custom registry
INSERT INTO "main"."vertical_datum"
("auth_name", "code", "name", "deprecated")
VALUES
    ('CUSTOM', 'GGM10D', 'GGM10 Vertical Datum', 0),
    ('CUSTOM', 'LAS07D', 'LAS07 Vertical Datum', 0),
    ('CUSTOM', 'MN75D', 'MN75 Vertical Datum', 0),
    ('CUSTOM', 'VITEL2014D', 'Vitel 2014 Vertical Datum', 0),
    ('CUSTOM', 'EHT2014D', 'EHT 2014 Vertical Datum', 0),
    ('CUSTOM', 'ITG2009D', 'ITG 2009 Vertical Datum', 0);

INSERT INTO "main"."usage"
("auth_name", "code", "object_table_name", "object_auth_name", "object_code", "extent_auth_name", "extent_code", "scope_auth_name", "scope_code")
VALUES
    ('CUSTOM', '2001', 'vertical_datum', 'CUSTOM', 'GGM10D', 'EPSG', '3278', 'EPSG', '1024'),
    ('CUSTOM', '2002', 'vertical_datum', 'CUSTOM', 'LAS07D', 'EPSG', '3272', 'EPSG', '1024'),
    ('CUSTOM', '2004', 'vertical_datum', 'CUSTOM', 'MN75D', 'EPSG', '3295', 'EPSG', '1024'),
    ('CUSTOM', '2006', 'vertical_datum', 'CUSTOM', 'VITEL2014D', 'EPSG', '1119', 'EPSG', '1024'),
    ('CUSTOM', '2007', 'vertical_datum', 'CUSTOM', 'EHT2014D', 'EPSG', '1119', 'EPSG', '1024'),
    ('CUSTOM', '2008', 'vertical_datum', 'CUSTOM', 'ITG2009D', 'EPSG', '1127', 'EPSG', '1024');

INSERT INTO "main"."vertical_crs"
("auth_name", "code", "name", "coordinate_system_auth_name", "coordinate_system_code", "datum_auth_name", "datum_code", "deprecated")
VALUES
    ('CUSTOM', 'GGM10H', 'Mexico GGM10 height', 'EPSG', '6499', 'CUSTOM', 'GGM10D', 0),
    ('CUSTOM', 'LAS07H', 'Lithuania LAS07 height', 'EPSG', '6499', 'CUSTOM', 'LAS07D', 0),
    ('CUSTOM', 'MN75H', 'Romania MN75 height', 'EPSG', '6499', 'CUSTOM', 'MN75D', 0),
    ('CUSTOM', 'VITEL2014H', 'Hungary VITEL2014 height', 'EPSG', '6499', 'CUSTOM', 'VITEL2014D', 0),
    ('CUSTOM', 'EHT2014H', 'Hungary EHT2014 height', 'EPSG', '6499', 'CUSTOM', 'EHT2014D', 0),
    ('CUSTOM', 'ITG2009H', 'Italy ITG2009 height', 'EPSG', '6499', 'CUSTOM', 'ITG2009D', 0);

INSERT INTO "main"."usage"
("auth_name", "code", "object_table_name", "object_auth_name", "object_code", "extent_auth_name", "extent_code", "scope_auth_name", "scope_code")
VALUES
    ('CUSTOM', '3001', 'vertical_crs', 'CUSTOM', 'GGM10H', 'EPSG', '3278', 'EPSG', '1024'),
    ('CUSTOM', '3002', 'vertical_crs', 'CUSTOM', 'LAS07H', 'EPSG', '3272', 'EPSG', '1024'),
    ('CUSTOM', '3004', 'vertical_crs', 'CUSTOM', 'MN75H', 'EPSG', '3295', 'EPSG', '1024'),
    ('CUSTOM', '3011', 'vertical_crs', 'CUSTOM', 'VITEL2014H', 'EPSG', '1119', 'EPSG', '1024'),
    ('CUSTOM', '3012', 'vertical_crs', 'CUSTOM', 'EHT2014H', 'EPSG', '1119', 'EPSG', '1024'),
    ('CUSTOM', '3013', 'vertical_crs', 'CUSTOM', 'ITG2009H', 'EPSG', '1127', 'EPSG', '1024');

INSERT INTO "main"."grid_transformation"
("auth_name", "code", "name", "method_auth_name", "method_code", "method_name", "source_crs_auth_name", "source_crs_code", "target_crs_auth_name", "target_crs_code", "accuracy", "grid_param_auth_name", "grid_param_code", "grid_param_name", "grid_name", "deprecated")
VALUES
    ('PROJ', 'EPSG_6364_TO_CUSTOM_GGM10H_1', 'Mexico ITRF2008 to Mexico GGM10 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '6364', 'CUSTOM', 'GGM10H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'GGM10.tif', 0),
    ('PROJ', 'EPSG_4951_TO_CUSTOM_LAS07H_1', 'LKS94 to Lithuania LAS07 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4951', 'CUSTOM', 'LAS07H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'lit15g.tif', 0),
    ('PROJ', 'EPSG_4937_TO_CUSTOM_MN75H_1', 'ETRS89 to Romania MN75 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'CUSTOM', 'MN75H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'EGG97_QGRJ_SfASCII.tif', 0),
    ('PROJ', 'EPSG_4937_TO_CUSTOM_VITEL2014H_1', 'ETRS89 to Hungary VITEL2014 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'CUSTOM', 'VITEL2014H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'vitel2014.tif', 0),
    ('PROJ', 'EPSG_4937_TO_CUSTOM_EHT2014H_1', 'ETRS89 to Hungary EHT2014 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'CUSTOM', 'EHT2014H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'eht2014.tif', 0),
    ('PROJ', 'EPSG_4937_TO_CUSTOM_ITG2009H_1', 'ETRS89 to Italy ITG2009 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'CUSTOM', 'ITG2009H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'itg2009.tif', 0);

INSERT INTO "main"."usage"
("auth_name", "code", "object_table_name", "object_auth_name", "object_code", "extent_auth_name", "extent_code", "scope_auth_name", "scope_code")
VALUES
    ('CUSTOM', '4001', 'grid_transformation', 'PROJ', 'EPSG_6364_TO_CUSTOM_GGM10H_1', 'EPSG', '3278', 'EPSG', '1024'),
    ('CUSTOM', '4002', 'grid_transformation', 'PROJ', 'EPSG_4951_TO_CUSTOM_LAS07H_1', 'EPSG', '3272', 'EPSG', '1024'),
    ('CUSTOM', '4004', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_CUSTOM_MN75H_1', 'EPSG', '3295', 'EPSG', '1024'),
    ('CUSTOM', '4010', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_CUSTOM_VITEL2014H_1', 'EPSG', '1119', 'EPSG', '1024'),
    ('CUSTOM', '4011', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_CUSTOM_EHT2014H_1', 'EPSG', '1119', 'EPSG', '1024'),
    ('CUSTOM', '4012', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_CUSTOM_ITG2009H_1', 'EPSG', '1127', 'EPSG', '1024');

INSERT INTO "main"."grid_alternatives"
("original_grid_name", "proj_grid_name", "proj_grid_format", "proj_method", "inverse_direction", "url", "direct_download", "open_license")
VALUES
    ('GGM10.tif', 'GGM10.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/GGM10.tif', 1, 1),
    ('lit15g.tif', 'lit15g.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/lit15g.tif', 1, 1),
    ('EGG97_QGRJ_SfASCII.tif', 'EGG97_QGRJ_SfASCII-v2.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/EGG97_QGRJ_SfASCII-v2.tif', 1, 1),
    ('vitel2014.tif', 'vitel2014.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/vitel2014.tif', '1', '1'),
    ('eht2014.tif', 'eht2014.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/eht2014.tif', '1', '1'),
    ('itg2009.tif', 'itg2009.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/itg2009.tif', '1', '1');

-- Regional patches (from ReachView3 geodata-core build-scripts/sql; EPSG-target geoids use  where needed)

INSERT INTO "main"."vertical_datum"
("auth_name", "code", "name", "deprecated")
VALUES
    ('CUSTOM', 'HBG18D', 'HBG18D Vertical Datum', 0),
    ('CUSTOM', 'MAPGEO2015D', 'MAPGEO2015 Vertical Datum', 0),
    ('CUSTOM', 'HGEOHNOR2020D', 'hgeoHNOR2020 Vertical Datum', 0),
    ('CUSTOM', 'GEOCOL2004D', 'GEOCOL2004 Vertical Datum', 0),
    ('CUSTOM', 'GEODAZ2014D', 'GEODAZ2014 Vertical Datum', 0),
    ('CUSTOM', 'GEODPT08D', 'GEODPT08 Vertical Datum', 0),
    ('CUSTOM', 'SAGEOID2010D', 'South Africa SAGEOID2010 Vertical Datum', 0),
    ('CUSTOM', 'KNGEOID13D', 'South Korea KNGEOID13 Vertical Datum', 0),
    ('CUSTOM', 'KNGEOID14D', 'South Korea KNGEOID14 Vertical Datum', 0),
    ('CUSTOM', 'KNGEOID18D', 'South Korea KNGEOID18 Vertical Datum', 0),
    ('CUSTOM', 'NAVD88G12BD', 'North American Vertical Datum 1988 (GEOID12B)', 0);

INSERT INTO "main"."vertical_crs"
("auth_name", "code", "name", "coordinate_system_auth_name", "coordinate_system_code", "datum_auth_name", "datum_code", "deprecated")
VALUES
    ('CUSTOM', 'HBG18H', 'Belgium HBG18 height', 'EPSG', '6499', 'CUSTOM', 'HBG18D', 0),
    ('CUSTOM', 'MAPGEO2015H', 'Brazil MAPGEO2015 height', 'EPSG', '6499', 'CUSTOM', 'MAPGEO2015D', 0),
    ('CUSTOM', 'HGEOHNOR2020H', 'Brazil hgeoHNOR2020 height', 'EPSG', '6499', 'CUSTOM', 'HGEOHNOR2020D', 0),
    ('CUSTOM', 'GEOCOL2004H', 'Colombia GeoCol2004 height', 'EPSG', '6499', 'CUSTOM', 'GEOCOL2004D', 0),
    ('CUSTOM', 'NGFIGN69RAF09H', 'NGF-IGN69(RAF09) height', 'EPSG', '6499', 'EPSG', '5119', 0),
    ('CUSTOM', 'NGFIGN69RAF18BH', 'NGF-IGN69(RAF18B) height', 'EPSG', '6499', 'EPSG', '5119', 0),
    ('CUSTOM', 'NGFIGN69RAF20H', 'NGF-IGN69(RAF20) height', 'EPSG', '6499', 'EPSG', '5119', 0),
    ('CUSTOM', 'GEODAZ2014H', 'Portugal GEODAZ2014D height', 'EPSG', '6499', 'CUSTOM', 'GEODAZ2014D', 0),
    ('CUSTOM', 'GEODPT08H', 'Portugal GEODPT08 height', 'EPSG', '6499', 'CUSTOM', 'GEODPT08D', 0),
    ('CUSTOM', 'SAGEOID2010H', 'South Africa SAGEOID2010 height', 'EPSG', '6499', 'CUSTOM', 'SAGEOID2010D', 0),
    ('CUSTOM', 'KNGEOID13H', 'South Korea KNGEOID13 height', 'EPSG', '6499', 'CUSTOM', 'KNGEOID13D', 0),
    ('CUSTOM', 'KNGEOID14H', 'South Korea KNGEOID14 height', 'EPSG', '6499', 'CUSTOM', 'KNGEOID14D', 0),
    ('CUSTOM', 'KNGEOID18H', 'South Korea KNGEOID18 height', 'EPSG', '6499', 'CUSTOM', 'KNGEOID18D', 0),
    ('CUSTOM', 'NAVD88G12BH', 'NAVD88(GEOID12B) height', 'EPSG', '6499', 'CUSTOM', 'NAVD88G12BD', 0),
    ('CUSTOM', 'NAVD88G12BFTUSH', 'NAVD88(GEOID12B) height (ftUS)', 'EPSG', '6497', 'CUSTOM', 'NAVD88G12BD', 0),
    ('CUSTOM', 'NAVD88G12BFTH', 'NAVD88(GEOID12B) height (ft)', 'EPSG', '1030', 'CUSTOM', 'NAVD88G12BD', 0),
    ('CUSTOM', 'PRVD02G12BH', 'PRVD02(GEOID12B) height', 'EPSG', '6499', 'EPSG', '1123', 0),
    ('CUSTOM', 'VIVD09G12BH', 'VIVD09(GEOID12B) height', 'EPSG', '6499', 'EPSG', '1124', 0);

INSERT INTO "main"."grid_transformation"
("auth_name", "code", "name", "method_auth_name", "method_code", "method_name", "source_crs_auth_name", "source_crs_code", "target_crs_auth_name", "target_crs_code", "accuracy", "grid_param_auth_name", "grid_param_code", "grid_param_name", "grid_name", "deprecated")
VALUES
    ('PROJ', 'EPSG_4937_TO_CUSTOM_HBG18H_1', 'ETRS89 to Belgium HBG18H height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'CUSTOM', 'HBG18H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'HBG18.tif', 0),
    ('PROJ', 'EPSG_4989_TO_CUSTOM_MAPGEO2015H_1', 'SIRGAS 2000 to Brazil MAPGEO2015 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4989', 'CUSTOM', 'MAPGEO2015H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'MAPGEO2015_sirgas.tif', 0),
    ('PROJ', 'EPSG_4989_TO_CUSTOM_HGEOHNOR2020H_1', 'SIRGAS 2000 to Brazil hgeoHNOR2020 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4989', 'CUSTOM', 'HGEOHNOR2020H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'hgeoHNOR2020.tif', 0),
    ('PROJ', 'EPSG_4686_TO_CUSTOM_GEOCOL2004H_1', 'MAGNA-SIRGAS to Colombia GeoCol2004 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4686', 'CUSTOM', 'GEOCOL2004H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'geocol_2004.tif', 0),
    ('PROJ', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF09H_1', 'RGF93 to NGF-IGN69(RAF09) height', 'EPSG', '1073', 'Geographic3D to GravityRelatedHeight (IGN2009)', 'EPSG', '4965', 'CUSTOM', 'NGFIGN69RAF09H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'RAF09.mnt', 0),
    ('PROJ', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF18BH_1', 'RGF93 to NGF-IGN69(RAF18B) height', 'EPSG', '1073', 'Geographic3D to GravityRelatedHeight (IGN2009)', 'EPSG', '4965', 'CUSTOM', 'NGFIGN69RAF18BH', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'RAF18B.tif', 0),
    ('PROJ', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF20H_1', 'RGF93 to NGF-IGN69(RAF20) height', 'EPSG', '1073', 'Geographic3D to GravityRelatedHeight (IGN2009)', 'EPSG', '4965', 'CUSTOM', 'NGFIGN69RAF20H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'RAF20.tif', 0),
    ('PROJ', 'EPSG_5013_TO_CUSTOM_GEODAZ2014H_1', 'PTRA08 to Portugal GEODAZ2014 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '5013', 'CUSTOM', 'GEODAZ2014H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'GeodAz2014.tif', 0),
    ('PROJ', 'EPSG_4937_TO_CUSTOM_GEODPT08H_1', 'ETRS89 to Portugal GEODPT08 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4937', 'CUSTOM', 'GEODPT08H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'geodPT08.tif', 0),
    ('PROJ', 'EPSG_4148_TO_CUSTOM_SAGEOID2010H_1', 'Hartebeesthoek94 to South Africa SAGEOID2010 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4148', 'CUSTOM', 'SAGEOID2010H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'SAGEOID2010.tif', 0),
    ('PROJ', 'EPSG_4927_TO_CUSTOM_KNGEOID13H_1', 'Korea 2000 to South Korea KNGEOID13 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4927', 'CUSTOM', 'KNGEOID13H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'kngeoid13.tif', 0),
    ('PROJ', 'EPSG_4927_TO_CUSTOM_KNGEOID14H_1', 'Korea 2000 to South Korea KNGEOID14 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4927', 'CUSTOM', 'KNGEOID14H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'kngeoid14.tif', 0),
    ('PROJ', 'EPSG_4927_TO_CUSTOM_KNGEOID18H_1', 'Korea 2000 to South Korea KNGEOID18 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '4927', 'CUSTOM', 'KNGEOID18H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'kngeoid18.tif', 0),
    ('PROJ', 'EPSG_6319_TO_CUSTOM_NAVD88G12BH_1', 'NAD83(2011) to NAVD88(GEOID12B) height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '6319', 'CUSTOM', 'NAVD88G12BH', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'g2012bu0.bin', 0),
    ('PROJ', 'EPSG_6319_TO_CUSTOM_NAVD88G12BH_2', 'NAD83(2011) to NAVD88(GEOID12B) height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '6319', 'CUSTOM', 'NAVD88G12BH', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'g2012ba0.bin', 0),
    ('PROJ', 'EPSG_6319_TO_CUSTOM_PRVD02G12BH_1', 'NAD83(2011) to PRVD02(GEOID12B) height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '6319', 'CUSTOM', 'PRVD02G12BH', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'g2012bp0.bin', 0),
    ('PROJ', 'EPSG_6319_TO_CUSTOM_VIVD09G12BH_1', 'NAD83(2011) to VIVD09(GEOID12B) height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '6319', 'CUSTOM', 'VIVD09G12BH', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'g2012bp0.bin', 0),
    ('PROJ', 'EPSG_6322_TO_EPSG_5703_1', 'NAD83(PA11) to NAVD88 height, Hawaii', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '6322', 'EPSG', '5703', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'g2012bh0.bin', 0),
    ('PROJ', 'EPSG_6322_TO_CUSTOM_NAVD88G12BH_1', 'NAD83(PA11) to NAVD88(GEOID12B) height, Hawaii', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'EPSG', '6322', 'CUSTOM', 'NAVD88G12BH', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'g2012bh0.bin', 0);

INSERT INTO "main"."other_transformation"
("auth_name", "code", "name", "method_auth_name", "method_code", "method_name", "source_crs_auth_name", "source_crs_code", "target_crs_auth_name", "target_crs_code", "param1_auth_name", "param1_code", "param1_name", "param1_value", "param1_uom_auth_name", "param1_uom_code", "deprecated")
VALUES
    ('PROJ', 'CUSTOM_NAVD88G12BH_TO_CUSTOM_NAVD88G12BFTUSH', 'NAVD88(GEOID12B) height to NAVD88(GEOID12B) height (ftUS)', 'EPSG', '1069', 'Change of Vertical Unit', 'CUSTOM', 'NAVD88G12BH', 'CUSTOM', 'NAVD88G12BFTUSH', 'EPSG', '1051', 'Unit conversion scalar', 3.28083333333333, 'EPSG', '9201', 0),
    ('PROJ', 'CUSTOM_NAVD88G12BH_TO_CUSTOM_NAVD88G12BFTH', 'NAVD88(GEOID12B) height to NAVD88(GEOID12B) height (ft)', 'EPSG', '1069', 'Change of Vertical Unit', 'CUSTOM', 'NAVD88G12BH', 'CUSTOM', 'NAVD88G12BFTH', 'EPSG', '1051', 'Unit conversion scalar', 3.28083989501312, 'EPSG', '9201', 0);

INSERT INTO "main"."conversion_table"
("auth_name", "code", "name", "method_auth_name", "method_code", "param1_auth_name", "param1_code", "param1_value", "param1_uom_auth_name", "param1_uom_code", "param2_auth_name", "param2_code", "param2_value", "param2_uom_auth_name", "param2_uom_code", "param3_auth_name", "param3_code", "param3_value", "param3_uom_auth_name", "param3_uom_code", "param4_auth_name", "param4_code", "param4_value", "param4_uom_auth_name", "param4_uom_code", "param5_auth_name", "param5_code", "param5_value", "param5_uom_auth_name", "param5_uom_code", "deprecated")
VALUES
    ('CUSTOM', 'LAMBC_GREENWICH', 'Lamber Corse (Greenwich)', 'EPSG', '9801', 'EPSG', '8801', '42.165','EPSG', '9102', 'EPSG', '8802', '2.33722916666667', 'EPSG', '9102', 'EPSG', '8805', '0.99994471', 'EPSG', '9201', 'EPSG', '8806', '234.358', 'EPSG', '9001', 'EPSG', '8807', '185861.369', 'EPSG', '9001', 0);

INSERT INTO "main"."projected_crs"
("auth_name", "code", "name", "coordinate_system_auth_name", "coordinate_system_code", "geodetic_crs_auth_name", "geodetic_crs_code", "conversion_auth_name", "conversion_code", "deprecated")
VALUES
    ('CUSTOM', 'NTF_LAMBC', 'NTF / Lambert Corse', 'EPSG', '4499', 'EPSG', '4275', 'CUSTOM', 'LAMBC_GREENWICH', 0);

INSERT INTO "main"."geodetic_datum"
("auth_name", "code", "name", "ellipsoid_auth_name", "ellipsoid_code", "prime_meridian_auth_name", "prime_meridian_code", "deprecated")
VALUES
    ('CUSTOM', 'HTRS07D', 'HTRS07 Datum', 'EPSG', '7019', 'EPSG', '8901', 0),
    ('CUSTOM', 'GGRS87APPROXD', 'GGRS87 Approx. Datum', 'EPSG', '7019', 'EPSG', '8901', 0);

INSERT INTO "main"."vertical_datum"
("auth_name", "code", "name", "deprecated")
VALUES
    ('CUSTOM', 'GREEKGEOID2010D', 'GREEKGEOID2010 Vertical Datum', 0);

INSERT INTO "main"."geodetic_crs"
("auth_name", "code", "name", "type", "coordinate_system_auth_name", "coordinate_system_code", "datum_auth_name", "datum_code", "deprecated")
VALUES
    ('CUSTOM', 'HTRS07', 'HTRS07', 'geographic 2D', 'EPSG', '6422', 'CUSTOM', 'HTRS07D', 0),
    ('CUSTOM', 'HTRS07_ECEF', 'HTRS07', 'geocentric', 'EPSG', '6500', 'CUSTOM', 'HTRS07D', 0),
    ('CUSTOM', 'HTRS07_3D', 'HTRS07', 'geographic 3D', 'EPSG', '6423', 'CUSTOM', 'HTRS07D', 0),
    ('CUSTOM', 'GGRS87APPROX', 'GGRS87 Approx.', 'geographic 2D', 'EPSG', '6422', 'CUSTOM', 'GGRS87APPROXD', 0);

INSERT INTO "main"."vertical_crs"
("auth_name", "code", "name", "coordinate_system_auth_name", "coordinate_system_code", "datum_auth_name", "datum_code", "deprecated")
VALUES
    ('CUSTOM', 'GREEKGEOID2010H', 'Greece GREEKGEOID2010 height', 'EPSG', '6499', 'CUSTOM', 'GREEKGEOID2010D', 0);

INSERT INTO "main"."conversion_table"
("auth_name", "code", "name", "method_auth_name", "method_code", "param1_auth_name", "param1_code", "param1_value", "param1_uom_auth_name", "param1_uom_code", "param2_auth_name", "param2_code", "param2_value", "param2_uom_auth_name", "param2_uom_code", "param3_auth_name", "param3_code", "param3_value", "param3_uom_auth_name", "param3_uom_code", "param4_auth_name", "param4_code", "param4_value", "param4_uom_auth_name", "param4_uom_code", "param5_auth_name", "param5_code", "param5_value", "param5_uom_auth_name", "param5_uom_code", "deprecated")
VALUES
    ('CUSTOM', 'TM07', 'TM07', 'EPSG', 9807, 'EPSG', '8801', 0.0, 'EPSG', '9102', 'EPSG', '8802', 24.0, 'EPSG', '9102', 'EPSG', '8805', '0.9996', 'EPSG', '9201', 'EPSG', '8806', 500000.0, 'EPSG', '9001', 'EPSG', '8807', -2000000.0, 'EPSG', '9001', 0);

INSERT INTO "main"."projected_crs"
("auth_name", "code", "name", "coordinate_system_auth_name", "coordinate_system_code", "geodetic_crs_auth_name", "geodetic_crs_code", "conversion_auth_name", "conversion_code", "deprecated")
VALUES
    ('CUSTOM', 'GGRS87_HEPOS', 'GGRS87 / HEPOS', 'EPSG', '4400', 'CUSTOM', 'GGRS87APPROX', 'EPSG', '19930', 0),
    ('CUSTOM', 'HTRS07_TM07', 'HTRS07 / TM07', 'EPSG', '4400', 'CUSTOM', 'HTRS07', 'CUSTOM', 'TM07', 0);

INSERT INTO "main"."helmert_transformation_table"
("auth_name", "code", "name", "method_auth_name", "method_code", "source_crs_auth_name", "source_crs_code", "target_crs_auth_name", "target_crs_code", "accuracy", "tx", "ty", "tz", "translation_uom_auth_name", "translation_uom_code", "rx", "ry", "rz", "rotation_uom_auth_name", "rotation_uom_code", "scale_difference", "scale_difference_uom_auth_name", "scale_difference_uom_code", "deprecated")
VALUES
    ('PROJ', 'CUSTOM_HTRS07_TO_EPSG_4121_1', 'HTRS07 to GGRS87', 'EPSG', '9607', 'CUSTOM', 'HTRS07', 'EPSG', '4121', 0.0, 203.437, -73.461, -243.594, 'EPSG', '9001', -0.170, -0.060, -0.151, 'EPSG', '9104', -0.294, 'EPSG', '9202', 0),
    ('PROJ', 'CUSTOM_HTRS07_TO_EPSG_4258_1', 'HTRS07 to ETRS89', 'EPSG', '9603', 'CUSTOM', 'HTRS07', 'EPSG', '4258', 0.0, 0.0, 0.0, 0.0, 'EPSG', '9001', 0.0, 0.0, 0.0, 'EPSG', '9104', 0.0, 'EPSG', '9202', 0),
    ('PROJ', 'CUSTOM_HTRS07_TO_EPSG_4326_1', 'HTRS07 to WGS84', 'EPSG', '9603', 'CUSTOM', 'HTRS07', 'EPSG', '4326', 1.0, 0.0, 0.0, 0.0, 'EPSG', '9001', 0.0, 0.0, 0.0, 'EPSG', '9104', 0.0, 'EPSG', '9202', 0);

INSERT INTO "main"."grid_transformation"
("auth_name", "code", "name", "method_auth_name", "method_code", "method_name", "source_crs_auth_name", "source_crs_code", "target_crs_auth_name", "target_crs_code", "accuracy", "grid_param_auth_name", "grid_param_code", "grid_param_name", "grid_name", "deprecated")
VALUES
    ('PROJ', 'EPSG_4121_TO_CUSTOM_GGRS87APPROX_1', 'GGRS87 to GGRS87 Approx.', 'EPSG', '9615', 'NTv2', 'EPSG', '4121', 'CUSTOM', 'GGRS87APPROX', '0.0', 'EPSG', '8656', 'Latitude and longitude difference file', 'ggrs87_hepos.tif', 0),
    ('PROJ', 'CUSTOM_HTRS07_3D_TO_CUSTOM_GREEKGEOID2010H_1', 'HTRS07 to Greece GREEKGEOID2010 height', 'EPSG', '9665', 'Geographic3D to GravityRelatedHeight (gtx)', 'CUSTOM', 'HTRS07_3D', 'CUSTOM', 'GREEKGEOID2010H', 999, 'EPSG', '8666', 'Geoid (height correction) model file', 'GreekGeoid2010.tif', 0);

INSERT INTO "main"."concatenated_operation"
("auth_name", "code", "name", "source_crs_auth_name", "source_crs_code", "target_crs_auth_name", "target_crs_code", "deprecated")
VALUES
    ('PROJ', 'HTRS07_TO_GGRS87APPROX', 'HTRS07 to GGRS87 Approx.', 'CUSTOM', 'HTRS07', 'CUSTOM', 'GGRS87APPROX', 0);

INSERT INTO "main"."concatenated_operation_step"
("operation_auth_name", "operation_code", "step_number", "step_auth_name", "step_code")
VALUES
    ('PROJ', 'HTRS07_TO_GGRS87APPROX', 1, 'PROJ', 'CUSTOM_HTRS07_TO_EPSG_4121_1'),
    ('PROJ', 'HTRS07_TO_GGRS87APPROX', 2, 'PROJ', 'EPSG_4121_TO_CUSTOM_GGRS87APPROX_1');

INSERT INTO "main"."grid_alternatives"
("original_grid_name", "proj_grid_name", "proj_grid_format", "proj_method", "inverse_direction", "url", "direct_download", "open_license")
VALUES
    ('HBG18.tif', 'HBG18.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/HBG18.tif', 1, 1),
    ('MAPGEO2015_sirgas.tif', 'MAPGEO2015_sirgas.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/MAPGEO2015_sirgas.tif', 1, 1),
    ('hgeoHNOR2020.tif', 'hgeoHNOR2020.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/hgeoHNOR2020.tif', 1, 1),
    ('geocol_2004.tif', 'geocol_2004.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/geocol_2004.tif', 1, 1),
    ('RAF18B.tif', 'fr_ign_RAF18B.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/fr_ign_RAF18B.tif', 1, 1),
    ('RAF20.tif', 'fr_ign_RAF20.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/fr_ign_RAF20.tif', 1, 1),
    ('GeodAz2014.tif', 'GeodAz2014.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/GeodAz2014.tif', 1, 1),
    ('geodPT08.tif', 'geodPT08.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/geodPT08.tif', 1, 1),
    ('SAGEOID2010.tif', 'SAGEOID2010.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/SAGEOID2010.tif', 1, 1),
    ('kngeoid13.tif', 'kngeoid13.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/kngeoid13.tif', 1, 1),
    ('kngeoid14.tif', 'kngeoid14.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/kngeoid14.tif', 1, 1),
    ('kngeoid18.tif', 'kngeoid18.tif', 'GTiff', 'geoid_like', 0, 'http://files.emlid.com/reachview3/grids-tif/kngeoid18.tif', 1, 1),
    ('g2012bh0.bin', 'us_noaa_g2012bh0.tif', 'GTiff', 'geoid_like', 0, 'https://cdn.proj.org/us_noaa_g2012bh0.tif', 1, 1),
    ('GreekGeoid2010.tif', 'GreekGeoid2010.tif', 'GTiff', 'geoid_like', '0', 'http://files.emlid.com/reachview3/grids-tif/GreekGeoid2010.tif', '1', '1'),
    ('ggrs87_hepos.tif', 'ggrs87_hepos.tif', 'NTv2', 'hgridshift', '0', 'http://files.emlid.com/reachview3/grids-tif/ggrs87_hepos.tif', '1', '1');

INSERT INTO "main"."usage"
("auth_name", "code", "object_table_name", "object_auth_name", "object_code", "extent_auth_name", "extent_code", "scope_auth_name", "scope_code")
VALUES
    ('CUSTOM', 'HBG18D', 'vertical_datum', 'CUSTOM', 'HBG18D', 'EPSG', '1044', 'EPSG', '1024'),
    ('CUSTOM', 'HBG18H', 'vertical_crs', 'CUSTOM', 'HBG18H', 'EPSG', '1044', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4937_TO_CUSTOM_HBG18H_1', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_CUSTOM_HBG18H_1', 'EPSG', '1044', 'EPSG', '1024'),
    ('CUSTOM', 'MAPGEO2015D', 'vertical_datum', 'CUSTOM', 'MAPGEO2015D', 'EPSG', '1053', 'EPSG', '1024'),
    ('CUSTOM', 'HGEOHNOR2020D', 'vertical_datum', 'CUSTOM', 'HGEOHNOR2020D', 'EPSG', '1053', 'EPSG', '1024'),
    ('CUSTOM', 'MAPGEO2015H', 'vertical_crs', 'CUSTOM', 'MAPGEO2015H', 'EPSG', '1053', 'EPSG', '1024'),
    ('CUSTOM', 'HGEOHNOR2020H', 'vertical_crs', 'CUSTOM', 'HGEOHNOR2020H', 'EPSG', '1053', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4989_TO_CUSTOM_MAPGEO2015H_1', 'grid_transformation', 'PROJ', 'EPSG_4989_TO_CUSTOM_MAPGEO2015H_1', 'EPSG', '1053', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4989_TO_CUSTOM_HGEOHNOR2020H_1', 'grid_transformation', 'PROJ', 'EPSG_4989_TO_CUSTOM_HGEOHNOR2020H_1', 'EPSG', '1053', 'EPSG', '1024'),
    ('CUSTOM', 'GEOCOL2004D', 'vertical_datum', 'CUSTOM', 'GEOCOL2004D', 'EPSG', '1070', 'EPSG', '1024'),
    ('CUSTOM', 'GEOCOL2004H', 'vertical_crs', 'CUSTOM', 'GEOCOL2004H', 'EPSG', '1070', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4686_TO_CUSTOM_GEOCOL2004H_1', 'grid_transformation', 'PROJ', 'EPSG_4686_TO_CUSTOM_GEOCOL2004H_1', 'EPSG', '1070', 'EPSG', '1024'),
    ('CUSTOM', 'NGFIGN69RAF09H', 'vertical_crs', 'CUSTOM', 'NGFIGN69RAF09H', 'EPSG', '1326', 'EPSG', '1024'),
    ('CUSTOM', 'NGFIGN69RAF18BH', 'vertical_crs', 'CUSTOM', 'NGFIGN69RAF18BH', 'EPSG', '1326', 'EPSG', '1024'),
    ('CUSTOM', 'NGFIGN69RAF20H', 'vertical_crs', 'CUSTOM', 'NGFIGN69RAF20H', 'EPSG', '1326', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF09H_1', 'grid_transformation', 'PROJ', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF09H_1', 'EPSG', '1326', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF18BH_1', 'grid_transformation', 'PROJ', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF18BH_1', 'EPSG', '1326', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF20H_1', 'grid_transformation', 'PROJ', 'EPSG_4965_TO_CUSTOM_NGFIGN69RAF20H_1', 'EPSG', '1326', 'EPSG', '1024'),
    ('CUSTOM', 'NTF_LAMBC', 'projected_crs', 'CUSTOM', 'NTF_LAMBC', 'EPSG', '1327', 'EPSG', '1142'),
    ('CUSTOM', 'GEODAZ2014D', 'vertical_datum', 'CUSTOM', 'GEODAZ2014D', 'EPSG', '3677', 'EPSG', '1024'),
    ('CUSTOM', 'GEODPT08D', 'vertical_datum', 'CUSTOM', 'GEODPT08D', 'EPSG', '1294', 'EPSG', '1024'),
    ('CUSTOM', 'GEODAZ2014H', 'vertical_crs', 'CUSTOM', 'GEODAZ2014H', 'EPSG', '3677', 'EPSG', '1024'),
    ('CUSTOM', 'GEODPT08H', 'vertical_crs', 'CUSTOM', 'GEODPT08H', 'EPSG', '1294', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_5013_TO_CUSTOM_GEODAZ2014H_1', 'grid_transformation', 'PROJ', 'EPSG_5013_TO_CUSTOM_GEODAZ2014H_1', 'EPSG', '3677', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4937_TO_CUSTOM_GEODPT08H_1', 'grid_transformation', 'PROJ', 'EPSG_4937_TO_CUSTOM_GEODPT08H_1', 'EPSG', '1294', 'EPSG', '1024'),
    ('CUSTOM', 'SAGEOID2010D', 'vertical_datum', 'CUSTOM', 'SAGEOID2010D', 'EPSG', '1215', 'EPSG', '1024'),
    ('CUSTOM', 'SAGEOID2010H', 'vertical_crs', 'CUSTOM', 'SAGEOID2010H', 'EPSG', '1215', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4148_TO_CUSTOM_SAGEOID2010H_1', 'grid_transformation', 'PROJ', 'EPSG_4148_TO_CUSTOM_SAGEOID2010H_1', 'EPSG', '1215', 'EPSG', '1024'),
    ('CUSTOM', 'KNGEOID13D', 'vertical_datum', 'CUSTOM', 'KNGEOID13D', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'KNGEOID14D', 'vertical_datum', 'CUSTOM', 'KNGEOID14D', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'KNGEOID18D', 'vertical_datum', 'CUSTOM', 'KNGEOID18D', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'KNGEOID13H', 'vertical_crs', 'CUSTOM', 'KNGEOID13H', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'KNGEOID14H', 'vertical_crs', 'CUSTOM', 'KNGEOID14H', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'KNGEOID18H', 'vertical_crs', 'CUSTOM', 'KNGEOID18H', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4927_TO_CUSTOM_KNGEOID13H_1', 'grid_transformation', 'PROJ', 'EPSG_4927_TO_CUSTOM_KNGEOID13H_1', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4927_TO_CUSTOM_KNGEOID14H_1', 'grid_transformation', 'PROJ', 'EPSG_4927_TO_CUSTOM_KNGEOID14H_1', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4927_TO_CUSTOM_KNGEOID18H_1', 'grid_transformation', 'PROJ', 'EPSG_4927_TO_CUSTOM_KNGEOID18H_1', 'EPSG', '1135', 'EPSG', '1024'),
    ('CUSTOM', 'NAVD88G12BD', 'vertical_datum', 'CUSTOM', 'NAVD88G12BD', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'NAVD88G12BH', 'vertical_crs', 'CUSTOM', 'NAVD88G12BH', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'NAVD88G12BFTUSH', 'vertical_crs', 'CUSTOM', 'NAVD88G12BFTUSH', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'NAVD88G12BFTH', 'vertical_crs', 'CUSTOM', 'NAVD88G12BFTH', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'PRVD02G12BH', 'vertical_crs', 'CUSTOM', 'PRVD02G12BH', 'EPSG', '3294', 'EPSG', '1024'),
    ('CUSTOM', 'VIVD09G12BH', 'vertical_crs', 'CUSTOM', 'VIVD09G12BH', 'EPSG', '3330', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_6319_TO_CUSTOM_NAVD88G12BH_1', 'grid_transformation', 'PROJ', 'EPSG_6319_TO_CUSTOM_NAVD88G12BH_1', 'EPSG', '1323', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_6319_TO_CUSTOM_NAVD88G12BH_2', 'grid_transformation', 'PROJ', 'EPSG_6319_TO_CUSTOM_NAVD88G12BH_2', 'EPSG', '1330', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_6319_TO_CUSTOM_PRVD02G12BH_1', 'grid_transformation', 'PROJ', 'EPSG_6319_TO_CUSTOM_PRVD02G12BH_1', 'EPSG', '3294', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_6319_TO_CUSTOM_VIVD09G12BH_1', 'grid_transformation', 'PROJ', 'EPSG_6319_TO_CUSTOM_VIVD09G12BH_1', 'EPSG', '3330', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_6322_TO_EPSG_5703_1', 'grid_transformation', 'PROJ', 'EPSG_6322_TO_EPSG_5703_1', 'EPSG', '1334', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_6322_TO_CUSTOM_NAVD88G12BH_1', 'grid_transformation', 'PROJ', 'EPSG_6322_TO_CUSTOM_NAVD88G12BH_1', 'EPSG', '1334', 'EPSG', '1024'),
    ('CUSTOM', 'CUSTOM_NAVD88G12BH_TO_CUSTOM_NAVD88G12BFTUSH', 'other_transformation', 'PROJ', 'CUSTOM_NAVD88G12BH_TO_CUSTOM_NAVD88G12BFTUSH', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'CUSTOM_NAVD88G12BH_TO_CUSTOM_NAVD88G12BFTH', 'other_transformation', 'PROJ', 'CUSTOM_NAVD88G12BH_TO_CUSTOM_NAVD88G12BFTH', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'NAVD88FTUSH', 'vertical_crs', 'EPSG', '6360', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'NAVD88FTH', 'vertical_crs', 'EPSG', '8228', 'EPSG', '1324', 'EPSG', '1024'),
    ('CUSTOM', 'GGRS87APPROXD', 'geodetic_datum', 'CUSTOM', 'GGRS87APPROXD', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'GGRS87APPROX', 'geodetic_crs', 'CUSTOM', 'GGRS87APPROX', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_4121_TO_CUSTOM_GGRS87APPROX_1', 'grid_transformation', 'PROJ', 'EPSG_4121_TO_CUSTOM_GGRS87APPROX_1', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'GGRS87_HEPOS', 'projected_crs', 'CUSTOM', 'GGRS87_HEPOS', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'GREEKGEOID2010D', 'vertical_datum', 'CUSTOM', 'GREEKGEOID2010D', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'GREEKGEOID2010H', 'vertical_crs', 'CUSTOM', 'GREEKGEOID2010H', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'HTRS07D', 'geodetic_datum', 'CUSTOM', 'HTRS07D', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'HTRS07', 'geodetic_crs', 'CUSTOM', 'HTRS07', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'HTRS07_ECEF', 'geodetic_crs', 'CUSTOM', 'HTRS07_ECEF', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'HTRS07_3D', 'geodetic_crs', 'CUSTOM', 'HTRS07_3D', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'HTRS07_TM07', 'projected_crs', 'CUSTOM', 'HTRS07_TM07', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'CUSTOM_HTRS07_TO_EPSG_4121_1', 'helmert_transformation', 'PROJ', 'CUSTOM_HTRS07_TO_EPSG_4121_1', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'CUSTOM_HTRS07_TO_EPSG_4258_1', 'helmert_transformation', 'PROJ', 'CUSTOM_HTRS07_TO_EPSG_4258_1', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'CUSTOM_HTRS07_TO_EPSG_4326_1', 'helmert_transformation', 'PROJ', 'CUSTOM_HTRS07_TO_EPSG_4326_1', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', 'CUSTOM_HTRS07_3D_TO_CUSTOM_GREEKGEOID2010H_1', 'grid_transformation', 'PROJ', 'CUSTOM_HTRS07_3D_TO_CUSTOM_GREEKGEOID2010H_1', 'EPSG', '1106', 'EPSG', '1024'),
    ('CUSTOM', '6695', 'vertical_crs', 'EPSG', '6695', 'EPSG', '3957', 'EPSG', '1024'),
    ('CUSTOM', 'EPSG_6667_TO_EPSG_6695', 'grid_transformation', 'PROJ', 'EPSG_6667_TO_EPSG_6695', 'EPSG', '3957', 'EPSG', '1024');

-- EPSG registers 8369 as BD72 (4313) -> ETRS89-BEL (11215). Tie the NTv2 grid to EPSG:4258 instead so
-- EPSG:4258 <-> BD72 / Belgian Lambert 72 (31370) uses the IGN grid (be_ign_bd72lb72_etrs89lb08.tif), not Helmert 15748.
UPDATE "main"."grid_transformation"
SET "name" = 'BD72 to ETRS89 (3)',
    "target_crs_auth_name" = 'EPSG',
    "target_crs_code" = '4258'
WHERE "auth_name" = 'EPSG' AND "code" = '8369';

-- Update the extend for Czechia to transform point correctly near the south border.
UPDATE "main"."extent"
SET "south_lat" = 48.55
WHERE "auth_name" = 'EPSG' AND "code" = '1079';

-- Update the name for Czech CS according to the local requirements
UPDATE "main"."projected_crs"
SET "name" = 'S-JTSK / Krovak v1710'
WHERE "auth_name" = 'EPSG' AND "code" = '5514';

UPDATE "main"."vertical_crs"
SET "name" = 'Czech_CR-2005_v1005'
WHERE "auth_name" = 'EPSG' AND "code" = '8357';

-- Patch proj.db: prefer Czech CR-2005 geoid for EPSG:4937 -> EPSG:8357 inside Czechia.
-- Upstream/EPSG ships EPSG:10567 (source 11069) and PROJ RESTRICTED 4937->8357 using the
-- Slovak Baltic grid; compound 4258 + 5514 + 8357 then picked the wrong vertical step.
-- Kept at end of this file so nothing earlier in emlid_patch.sql overwrites it; build order
-- (sql_filelist.cmake) has no later SQL that updates these rows.
DELETE FROM "main"."usage" WHERE object_table_name = 'grid_transformation' AND object_auth_name = 'PROJ' AND object_code = 'EPSG_4937_TO_EPSG_8357_CZ_CR2005';
DELETE FROM "main"."grid_transformation" WHERE auth_name = 'PROJ' AND code = 'EPSG_4937_TO_EPSG_8357_CZ_CR2005';

-- Demote Slovak 4937->8360 / restricted-vert paths so Czech 0.03 wins where both compete.
UPDATE "main"."grid_transformation" SET accuracy = 0.15
WHERE auth_name = 'EPSG' AND code = '8361';

UPDATE "main"."grid_transformation" SET accuracy = 0.15
WHERE auth_name = 'PROJ' AND code = 'EPSG_8361_RESTRICTED_TO_VERTCRS';

INSERT INTO "main"."grid_transformation" (
  auth_name, code, name, description,
  method_auth_name, method_code, method_name,
  source_crs_auth_name, source_crs_code,
  target_crs_auth_name, target_crs_code,
  accuracy,
  grid_param_auth_name, grid_param_code, grid_param_name, grid_name,
  grid2_param_auth_name, grid2_param_code, grid2_param_name, grid2_name,
  param1_auth_name, param1_code, param1_name, param1_value, param1_uom_auth_name, param1_uom_code,
  param2_auth_name, param2_code, param2_name, param2_value, param2_uom_auth_name, param2_uom_code,
  interpolation_crs_auth_name, interpolation_crs_code,
  operation_version,
  deprecated
) VALUES (
  'PROJ','EPSG_4937_TO_EPSG_8357_CZ_CR2005',
  'ETRS89 to Baltic 1957 height (Czechia, CR-2005)',
  NULL,
  'EPSG','9665','Geographic3D to GravityRelatedHeight (gtx)',
  'EPSG','4937','EPSG','8357',
  0.03,
  'EPSG','8666','Geoid (height correction) model file','CR2005_GTX.gtx',
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,NULL,NULL,
  NULL,NULL,
  'cuzk-Cze 2005',
  0
);

INSERT INTO "main"."usage" VALUES(
  'CUSTOM','1018',
  'grid_transformation',
  'PROJ',
  'EPSG_4937_TO_EPSG_8357_CZ_CR2005',
  'EPSG','1079',
  'EPSG','1024'
);

-- Align Czech JTSK/JTSK05 gridshift with Emlid-shipped `cz_cuzk_table_yx_3_v1710_east_north.tif`
-- (see gc-data/griddata.json). Fork `transformations_czechia.sql` still referenced `-y-x` CDN name.
UPDATE "main"."other_transformation"
SET method_name = REPLACE(
  method_name,
  'cz_cuzk_table_-y-x_3_v1710.tif',
  'cz_cuzk_table_yx_3_v1710_east_north.tif'
)
WHERE auth_name = 'PROJ' AND method_name LIKE '%cz_cuzk_table_-y-x_3_v1710.tif%';

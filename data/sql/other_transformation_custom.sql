-- This file is hand generated.

-- Norway triangulated files
 
INSERT INTO other_transformation VALUES(
    'PROJ','NGO48_TO_ETRS89NO','NGO 1948 to ETRS89 (2)',
    'Transformation based on a triangulated irregular network',
    'PROJ','PROJString',
    '+proj=pipeline ' ||
        '+step +proj=axisswap +order=2,1 ' ||
        '+step +proj=tinshift +file=no_kv_ETRS89NO_NGO48_TIN.json +inv ' ||
        '+step +proj=axisswap +order=2,1',
    'EPSG','4273',
    'EPSG','4258',
    0.1,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','NGO48_TO_ETRS89NO_USAGE','other_transformation','PROJ','NGO48_TO_ETRS89NO','EPSG','1352','EPSG','1031');

-- Finland triangulated files

INSERT INTO other_transformation VALUES(
    'PROJ','YKJ_TO_ETRS35FIN','KKJ / Finland Uniform Coordinate System to ETRS35FIN',
    'Transformation based on a triangulated irregular network',
    'PROJ','PROJString','+proj=pipeline +step +proj=axisswap +order=2,1 +step +proj=tinshift +file=fi_nls_ykj_etrs35fin.json',
    'EPSG','2393','EPSG','3067',0.1,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','YKJ_TO_ETRS35FIN_USAGE','other_transformation','PROJ','YKJ_TO_ETRS35FIN','EPSG','3333','EPSG','1024');

INSERT INTO "concatenated_operation" VALUES('PROJ','KKJ_TO_ETRS89','KKJ to ETRS89 (using PROJ:YKJ_TO_ETRS35FIN)','Transformation based on a triangulated irregular network','EPSG','4123','EPSG','4258',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_ETRS89',1,'EPSG','18193');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_ETRS89',2,'PROJ','YKJ_TO_ETRS35FIN');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_ETRS89',3,'EPSG','16065');
INSERT INTO "usage" VALUES(
    'PROJ',
    'KKJ_TO_ETRS89_USAGE',
    'concatenated_operation',
    'PROJ',
    'KKJ_TO_ETRS89',
    'EPSG','3333', -- extent
    'EPSG','1024'  -- unknown
);

INSERT INTO other_transformation VALUES(
    'PROJ','N43_TO_N60','N43 height to N60 height',
    'Transformation based on a triangulated irregular network',
    'PROJ','PROJString','+proj=pipeline +step +proj=tmerc +lat_0=0 +lon_0=27 +k=1 +x_0=3500000 +y_0=0 +ellps=intl +step +proj=tinshift +file=fi_nls_n43_n60.json +step +inv +proj=tmerc +lat_0=0 +lon_0=27 +k=1 +x_0=3500000 +y_0=0 +ellps=intl',
    'EPSG','8675','EPSG','5717',0.01,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'EPSG','4123',NULL,0);
INSERT INTO "usage" VALUES('PROJ','N43_TO_N60_USAGE','other_transformation','PROJ','N43_TO_N60','EPSG','4522','EPSG','1024');


INSERT INTO other_transformation VALUES(
    'PROJ','N60_TO_N2000','N60 height to N2000 height',
    'Transformation based on a triangulated irregular network',
    'PROJ','PROJString','+proj=pipeline +step +proj=tmerc +lat_0=0 +lon_0=27 +k=1 +x_0=3500000 +y_0=0 +ellps=intl +step +proj=tinshift +file=fi_nls_n60_n2000.json +step +inv +proj=tmerc +lat_0=0 +lon_0=27 +k=1 +x_0=3500000 +y_0=0 +ellps=intl',
    'EPSG','5717','EPSG','3900',0.01,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'EPSG','4123',NULL,0);
INSERT INTO "usage" VALUES('PROJ','N60_TO_N2000_USAGE','other_transformation','PROJ','N60_TO_N2000','EPSG','3333','EPSG','1024');

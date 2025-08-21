-- This file is hand generated.

-- Finland triangulated files

INSERT INTO "concatenated_operation" VALUES('PROJ','KKJ_TO_ETRS89','KKJ to ETRS89 (using EPSG:10703)','Transformation based on a triangulated irregular network','EPSG','4123','EPSG','4258',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_ETRS89',1,'EPSG','18193','forward');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_ETRS89',2,'EPSG','10703','forward');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_ETRS89',3,'EPSG','16065','reverse');
INSERT INTO "usage" VALUES(
    'PROJ',
    'KKJ_TO_ETRS89_USAGE',
    'concatenated_operation',
    'PROJ',
    'KKJ_TO_ETRS89',
    'EPSG','3333', -- extent
    'EPSG','1024'  -- unknown
);

INSERT INTO "concatenated_operation" VALUES('PROJ','KKJ_TO_EUREF_FIN','KKJ to EUREF-FIN (using EPSG:10703)','Transformation based on a triangulated irregular network','EPSG','4123','EPSG','10690',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_EUREF_FIN',1,'EPSG','18193','forward');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_EUREF_FIN',2,'EPSG','10703','forward');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','KKJ_TO_EUREF_FIN',3,'EPSG','16065','reverse');
INSERT INTO "usage" VALUES(
    'PROJ',
    'KKJ_TO_EUREF_FIN_USAGE',
    'concatenated_operation',
    'PROJ',
    'KKJ_TO_EUREF_FIN',
    'EPSG','3333', -- extent
    'EPSG','1024'  -- unknown
);

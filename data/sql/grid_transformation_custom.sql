-- This file is hand generated.

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_5799_TO_EPSG_4937','DVR90 height to ETRS89',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','5799', -- source CRS (DVR90 height)
    'EPSG','4937', -- target CRS (ETRS89)
    'EPSG','3237', -- area of use: Denmark onshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','dvr90.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,0);

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_5733_TO_EPSG_4937','DNN height to ETRS89',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','5733', -- source CRS (DNN height)
    'EPSG','4937', -- target CRS (ETRS89)
    'EPSG','3237', -- area of use: Denmark onshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','dnn.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,0);

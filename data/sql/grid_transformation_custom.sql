-- This file is hand generated.

-- Denmark

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_5799_TO_EPSG_4937','DVR90 height to ETRS89',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','5799', -- source CRS (DVR90 height)
    'EPSG','4937', -- target CRS (ETRS89)
    'EPSG','3237', -- area of use: Denmark onshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','dvr90.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_5733_TO_EPSG_4937','DNN height to ETRS89',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','5733', -- source CRS (DNN height)
    'EPSG','4937', -- target CRS (ETRS89)
    'EPSG','3237', -- area of use: Denmark onshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','dnn.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

-- Faroe Islands

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_5317_TO_EPSG_4937','FVR09 height to ETRS89',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','5317', -- source CRS (FVR09 height)
    'EPSG','4937', -- target CRS (ETRS89)
    'EPSG','3248', -- area of use: Faroe Islands - onshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','fvr09.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

-- Sweden

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_5613_TO_EPSG_4977','RH2000 height to SWEREF99',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','5613', -- source CRS (RH2000 height)
    'EPSG','4977', -- target CRS (SWEREF99)
    'EPSG','3313', -- area of use: Sweden onshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','SWEN17_RH2000.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

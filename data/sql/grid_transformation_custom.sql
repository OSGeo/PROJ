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

----- Hopefully temporary entry for BWTA2017.gsb grid -----

INSERT INTO "area" VALUES('PROJ','BWTA2017','Germany - Baden-Wurtemberg','Germany - Baden-Wurtemberg',47.5,49.83,7.49,10.51,0);
-- Advertize a 0.8 accuracy slightly better than the 0.9 of BETA2007 for sort purposes
INSERT INTO "grid_transformation" VALUES('PROJ','BWTA2017','DHDN to ETRS89 (BWTA2017)','DHDN to ETRS89 for Baden-Wurtemberg for ALKIS 2017. Using official BWTA2017 grid but this transformation entry has been created temporarily by PROJ. Accuracy indication not to be considered as authoritative','For applications requiring an accuracy of better than 1 metre.','EPSG','9615','NTv2','EPSG','4314','EPSG','4258','PROJ','BWTA2017',0.89,'EPSG','8656','Latitude and longitude difference file','BWTA2017.gsb',NULL,NULL,NULL,NULL,NULL,NULL,'-',0);

----- Hopefully temporary entry for SeTa2016.gsb grid -----

INSERT INTO "area" VALUES('PROJ','SETA2016','Germany - Saarland','Germany - Saarland',49.10,49.64,6.345,7.45,0);
-- Advertize a 0.8 accuracy slightly better than the 0.9 of BETA2007 for sort purposes
INSERT INTO "grid_transformation" VALUES('PROJ','SETA2016','DHDN to ETRS89 (SETA2016)','DHDN to ETRS89 for Saarland. Using official SETA2016 grid but this transformation entry has been created temporarily by PROJ. Accuracy indication not to be considered as authoritative','For applications requiring an accuracy of better than 1 metre.','EPSG','9615','NTv2','EPSG','4314','EPSG','4258','PROJ','SETA2016',0.89,'EPSG','8656','Latitude and longitude difference file','SeTa2016.gsb',NULL,NULL,NULL,NULL,NULL,NULL,'-',0);

-- Iceland

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_8089_TO_EPSG_5323','ISH2004 height to ISN2004',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','8089', -- source CRS (ISH2004 height)
    'EPSG','5323', -- target CRS (ISN2004 geographic 3D)
    'EPSG','1120', -- area of use: Iceland - onshore and offshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','Icegeoid_ISN2004.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
    
INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_8089_TO_EPSG_4945','ISH2004 height to ISN93',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','8089', -- source CRS (ISH2004 height)
    'EPSG','4945', -- target CRS (ISN93 geographic 3D)
    'EPSG','1120', -- area of use: Iceland - onshore and offshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','Icegeoid_ISN93.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
    
INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_8089_TO_EPSG_8085','ISH2004 height to ISN2016',
    NULL,NULL,
    'PROJ','HEIGHT_TO_GEOGRAPHIC3D','GravityRelatedHeight to Geographic3D',
    'EPSG','8089', -- source CRS (ISH2004 height)
    'EPSG','8085', -- target CRS (ISN2016 geographic 3D)
    'EPSG','1120', -- area of use: Iceland - onshore and offshore
    NULL,
    'EPSG','8666','Geoid (height correction) model file','Icegeoid_ISN2016.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

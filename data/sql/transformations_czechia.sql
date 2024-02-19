-- This file is hand generated.

-- Czechia transformations

-- Geoid transformation

INSERT INTO "grid_transformation" VALUES(
    'PROJ','ETRS89_TO_BALTIC_HEIGHT_CZECHIA','ETRS89 to Baltic 1957 height (Czechia)',
    NULL,
    'EPSG','9665','Geographic3D to GravityRelatedHeight (gtx)',
    'EPSG','4937', -- source CRS (ETRS 89)
    'EPSG','8357', -- target CRS (Baltic 1957 height)
    0.05, -- guessed...
    'EPSG','8666','Geoid (height correction) model file','cz_cuzk_CR-2005.tif',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);

INSERT INTO "usage" VALUES(
    'PROJ',
    'ETRS89_TO_BALTIC_HEIGHT_CZECHIA_USAGE',
    'grid_transformation',
    'PROJ',
    'ETRS89_TO_BALTIC_HEIGHT_CZECHIA',
    'EPSG','1079','EPSG','1189'
);

-- This file is hand generated.

-- WARNING: for now this file is not integrated in the default build, due to
-- licensing of grids cz_cuzk_table_yx_3_v1710_east_north.tif and cz_cuzk_CR-2005.tif
-- being not clarified.

-- Czechia transformations

-- 2d transformations between S-JTSK / Krovak and S-JTSK/05 / Modified Krovak using a grid with easting,northing offsets

INSERT INTO other_transformation VALUES(
    'PROJ','S_JTSK_E_N_TO_S_JTSK05_E_N','S-JTSK / Krovak East North (EPSG:5514) to S-JTSK/05 / Modified Krovak East North (EPSG:5516)',
    'Transformation based on grid table_yx_3_v1710.dat',
    'PROJ','PROJString',
    '+proj=gridshift +grids=cz_cuzk_table_yx_3_v1710_east_north.tif',
    'EPSG','5514','EPSG','5516',0.035,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK_E_N_TO_S_JTSK05_E_N_USAGE','other_transformation','PROJ','S_JTSK_E_N_TO_S_JTSK05_E_N','EPSG','1079','EPSG','1189');


INSERT INTO other_transformation VALUES(
    'PROJ','S_JTSK_E_N_TO_S_JTSK05','S-JTSK / Krovak East North (EPSG:5514) to S-JTSK/05 / Modified Krovak (EPSG:5515)',
    'Transformation based on grid table_yx_3_v1710.dat',
    'PROJ','PROJString',
    '+proj=pipeline ' ||
        '+step +proj=gridshift +grids=cz_cuzk_table_yx_3_v1710_east_north.tif ' ||
        '+step +proj=axisswap +order=-2,-1',
    'EPSG','5514','EPSG','5515',0.035,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK_E_N_TO_S_JTSK05_USAGE','other_transformation','PROJ','S_JTSK_E_N_TO_S_JTSK05','EPSG','1079','EPSG','1189');


INSERT INTO other_transformation VALUES(
    'PROJ','S_JTSK_TO_S_JTSK05_E_N','S-JTSK / Krovak (EPSG:5513) to S-JTSK/05 / Modified Krovak East North (EPSG:5516)',
    'Transformation based on grid table_yx_3_v1710.dat',
    'PROJ','PROJString',
    '+proj=pipeline ' ||
        '+step +proj=axisswap +order=-2,-1 ' ||
        '+step +proj=gridshift +grids=cz_cuzk_table_yx_3_v1710_east_north.tif',
    'EPSG','5513','EPSG','5516',0.035,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK_TO_S_JTSK05_E_N_USAGE','other_transformation','PROJ','S_JTSK_TO_S_JTSK05_E_N','EPSG','1079','EPSG','1189');


INSERT INTO other_transformation VALUES(
    'PROJ','S_JTSK_TO_S_JTSK05','S-JTSK / Krovak (EPSG:5513) to S-JTSK/05 / Modified Krovak (EPSG:5515)',
    'Transformation based on grid table_yx_3_v1710.dat',
    'PROJ','PROJString',
    '+proj=pipeline ' ||
        '+step +proj=axisswap +order=-2,-1 ' ||
        '+step +proj=gridshift +grids=cz_cuzk_table_yx_3_v1710_east_north.tif ' ||
        '+step +proj=axisswap +order=-2,-1',
    'EPSG','5513','EPSG','5515',0.035,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK_TO_S_JTSK05_USAGE','other_transformation','PROJ','S_JTSK_TO_S_JTSK05','EPSG','1079','EPSG','1189');

--- Geographic transformation: S/JTSK <--> S/JTSK/05

UPDATE other_transformation SET accuracy = 0.1 WHERE name = 'S-JTSK to S-JTSK/05 (1)';

INSERT INTO "concatenated_operation" VALUES(
    'PROJ','S_JTSK_GEOG_TO_S_JTSK05_GEOG','S-JTSK (EPSG:4156) to S-JTSK/05 (EPSG:5228)',
    'Transformation based on grid table_yx_3_v1710.dat','EPSG','4156','EPSG','5228',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','S_JTSK_GEOG_TO_S_JTSK05_GEOG',1,'EPSG','5510');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','S_JTSK_GEOG_TO_S_JTSK05_GEOG',2,'PROJ','S_JTSK_E_N_TO_S_JTSK05_E_N');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','S_JTSK_GEOG_TO_S_JTSK05_GEOG',3,'EPSG','5512');
INSERT INTO "usage" VALUES('PROJ','S_JTSK_GEOG_TO_S_JTSK05_GEOG_USAGE','concatenated_operation','PROJ','S_JTSK_GEOG_TO_S_JTSK05_GEOG','EPSG','1079','EPSG','1189');


-- CUZK recommands to do ETRS89 to S-JTSK / Krovak by doing:
-- 1) ETRS89 to S-JTSK/05 using a Helmert transformation (EPSG:5226)
-- 2) S-JTSK/05 to S-JTSK/05 / Modified Krovak, using modified Krovak projection (EPSG:5512)
-- 3) S-JTSK/05 / Modified Krovak to S-JTSK / Krovak using (reverse) grid table_yx_3_v1710.dat
-- Cf https://www.cuzk.cz/Zememerictvi/Geodeticke-zaklady-na-uzemi-CR/GNSS/Nova-realizace-systemu-ETRS89-v-CR/Metodika-prevodu-ETRF2000-vs-S-JTSK-var2(101208).aspx (Metodika převodu mezi ETRF2000 a S-JTSK varianta 2)
INSERT INTO "concatenated_operation" VALUES(
    'PROJ','ETRS89_TO_S_JTSK_E_N','ETRS89 to S-JTSK / Krovak East North (EPSG:5514)',
    'Transformation based on grid table_yx_3_v1710.dat','EPSG','4258','EPSG','5514',NULL,NULL,0);
INSERT INTO "concatenated_operation_step" VALUES('PROJ','ETRS89_TO_S_JTSK_E_N',1,'EPSG','5226');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','ETRS89_TO_S_JTSK_E_N',2,'EPSG','5512');
INSERT INTO "concatenated_operation_step" VALUES('PROJ','ETRS89_TO_S_JTSK_E_N',3,'PROJ','S_JTSK_E_N_TO_S_JTSK05_E_N'); -- in reverse direction
INSERT INTO "usage" VALUES('PROJ','ETRS89_TO_S_JTSK_E_N_USAGE','concatenated_operation','PROJ','ETRS89_TO_S_JTSK_E_N','EPSG','1079','EPSG','1189');


-- 3d transformations between ETRS 89 and compound CRS S-JTSK / Krovak (or S-JTSK/05 / Modified Krovak) + Baltic 1957 height

INSERT INTO "compound_crs" VALUES('PROJ','S_JTSK05_E_N_BALTIC_HEIGHT','S-JTSK/05 / Modified Krovak East North + Baltic 1957 height',NULL,'EPSG','5516','EPSG','8357',0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK05_E_N_BALTIC_HEIGHT_USAGE','compound_crs','PROJ','S_JTSK05_E_N_BALTIC_HEIGHT','EPSG','1079','EPSG','1189');

-- Simulate a variable
CREATE TEMPORARY TABLE temp_var(k,v);

INSERT INTO temp_var VALUES(
    'ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT',
    '+proj=pipeline ' ||
        '+step +proj=axisswap +order=2,1 ' ||
        '+step +proj=unitconvert +xy_in=deg +xy_out=rad ' ||
        '+step +proj=push +v_3 +omit_inv ' ||
        '+step +inv +proj=vgridshift +grids=cz_cuzk_CR-2005.tif +multiplier=1 ' ||
        '+step +proj=push +v_3 +omit_fwd ' ||  -- on reverse path, restore initial Baltic height
        '+step +proj=push +v_4 ' ||
        '+step +proj=set +v_4=0 +omit_inv ' ||
        '+step +proj=axisswap +order=1,2,4,3 +omit_inv ' ||  -- on forward path, save Baltic height in v_4 component...
        '+step +proj=pop +v_3 +omit_inv ' || -- on forward parth, restore initial ellipsoidal height
        '+step +proj=cart +ellps=GRS80 ' ||
        '+step +inv +proj=helmert +x=572.213 +y=85.334 +z=461.94 +rx=-4.9732 +ry=-1.529 +rz=-5.2484 +s=3.5378 +convention=coordinate_frame ' ||
        '+step +inv +proj=cart +ellps=bessel ' ||
        '+step +proj=mod_krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397222222 +k=0.9999 +x_0=5000000 +y_0=5000000 +ellps=bessel ' ||
        '+step +proj=axisswap +order=1,2,4,3 +omit_inv ' || -- on forward path, restore Baltic height from v_4 component...
        '+step +proj=set +v_4=0 +omit_inv ' ||
        '+step +proj=pop +v_4 ' ||
        '+step +proj=pop +v_3 +omit_fwd '  -- on reverse path, save initial Baltic height
);

INSERT INTO other_transformation VALUES(
    'PROJ','ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT','ETRS89 to S-JTSK/05 / Modified Krovak East North + Baltic 1957 height',
    'Transformation based on grid CR-2005.gtx',
    'PROJ','PROJString',
    (SELECT v FROM temp_var WHERE k = 'ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT'),
    'EPSG','4937','PROJ','S_JTSK05_E_N_BALTIC_HEIGHT',
    0.05, -- guessed...
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT_USAGE','other_transformation','PROJ','ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT','EPSG','1079','EPSG','1189');


INSERT INTO "compound_crs" VALUES('PROJ','S_JTSK05_BALTIC_HEIGHT','S-JTSK/05 / Modified Krovak + Baltic 1957 height',NULL,'EPSG','5515','EPSG','8357',0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK05_BALTIC_HEIGHT_USAGE','compound_crs','PROJ','S_JTSK05_BALTIC_HEIGHT','EPSG','1079','EPSG','1095');

INSERT INTO other_transformation VALUES(
    'PROJ','ETRS89_3D_TO_S_JTSK05_BALTIC_HEIGHT','ETRS89 to S-JTSK/05 / Modified Krovak + Baltic 1957 height',
    'Transformation based on grid CR-2005.gtx',
    'PROJ','PROJString',
    (SELECT v FROM temp_var WHERE k = 'ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT') ||
        '+step +proj=axisswap +order=-2,-1', -- East North --> Southing Westing
    'EPSG','4937','PROJ','S_JTSK05_BALTIC_HEIGHT',
    0.05, -- guessed...
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','ETRS89_3D_TO_S_JTSK05_BALTIC_HEIGHT_USAGE','other_transformation','PROJ','ETRS89_3D_TO_S_JTSK05_BALTIC_HEIGHT','EPSG','1079','EPSG','1095');


INSERT INTO "compound_crs" VALUES('PROJ','S_JTSK_E_N_BALTIC_HEIGHT','S-JTSK / Krovak East North + Baltic 1957 height',NULL,'EPSG','5514','EPSG','8357',0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK_E_N_BALTIC_HEIGHT_USAGE','compound_crs','PROJ','S_JTSK_E_N_BALTIC_HEIGHT','EPSG','1079','EPSG','1189');

INSERT INTO other_transformation VALUES(
    'PROJ','ETRS89_3D_TO_S_JTSK_E_N_BALTIC_HEIGHT','ETRS89 to S-JTSK / Krovak East North + Baltic 1957 height',
    'Transformation based on grids CR-2005.gtx and table_yx_3_v1710.dat ',
    'PROJ','PROJString',
    (SELECT v FROM temp_var WHERE k = 'ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT') ||
        '+step +inv +proj=gridshift +grids=cz_cuzk_table_yx_3_v1710_east_north.tif ',
    'EPSG','4937','PROJ','S_JTSK_E_N_BALTIC_HEIGHT',
    0.05, -- guessed...
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','ETRS89_3D_TO_S_JTSK_E_N_BALTIC_HEIGHT_USAGE','other_transformation','PROJ','ETRS89_3D_TO_S_JTSK_E_N_BALTIC_HEIGHT','EPSG','1079','EPSG','1189');


INSERT INTO "compound_crs" VALUES('PROJ','S_JTSK_BALTIC_HEIGHT','S-JTSK / Krovak + Baltic 1957 height',NULL,'EPSG','5513','EPSG','8357',0);
INSERT INTO "usage" VALUES('PROJ','S_JTSK_BALTIC_HEIGHT_USAGE','compound_crs','PROJ','S_JTSK_BALTIC_HEIGHT','EPSG','1079','EPSG','1095');

INSERT INTO other_transformation VALUES(
    'PROJ','ETRS89_3D_TO_S_JTSK_BALTIC_HEIGHT','ETRS89 to S-JTSK / Krovak + Baltic 1957 height',
    'Transformation based on grids CR-2005.gtx and table_yx_3_v1710.dat ',
    'PROJ','PROJString',
    (SELECT v FROM temp_var WHERE k = 'ETRS89_3D_TO_S_JTSK05_E_N_BALTIC_HEIGHT') ||
        '+step +inv +proj=gridshift +grids=cz_cuzk_table_yx_3_v1710_east_north.tif ' ||
        '+step +proj=axisswap +order=-2,-1', -- East North --> Southing Westing
    'EPSG','4937','PROJ','S_JTSK_BALTIC_HEIGHT',
    0.05, -- guessed...
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);
INSERT INTO "usage" VALUES('PROJ','ETRS89_3D_TO_S_JTSK_BALTIC_HEIGHT_USAGE','other_transformation','PROJ','ETRS89_3D_TO_S_JTSK_BALTIC_HEIGHT','EPSG','1079','EPSG','1095');

DROP TABLE temp_var;

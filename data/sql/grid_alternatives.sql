-- This file is hand generated.
--
-- NOTE: see also grid_alternatives_generated.sql for automatically generated
-- entries.
--

------------------------------------------------------
-- grid_packages
------------------------------------------------------

INSERT INTO grid_packages VALUES ('proj-datumgrid',
                                  'Package with grids of general interest',
                                  'https://download.osgeo.org/proj/proj-datumgrid-1.8.zip',
                                  1,
                                  1);

INSERT INTO grid_packages VALUES ('proj-datumgrid-north-america',
                                  'Package with grids of interest for North-America',
                                  'https://download.osgeo.org/proj/proj-datumgrid-north-america-1.3.zip',
                                  1,
                                  1);

INSERT INTO grid_packages VALUES ('proj-datumgrid-europe',
                                  'Package with grids of interest for Europe',
                                  'https://download.osgeo.org/proj/proj-datumgrid-europe-1.5.zip',
                                  1,
                                  1);

INSERT INTO grid_packages VALUES ('proj-datumgrid-oceania',
                                  'Package with grids of interest for Oceania',
                                  'https://download.osgeo.org/proj/proj-datumgrid-oceania-1.1.zip',
                                  1,
                                  1);

 -- not released yet at the time of writing
INSERT INTO grid_packages VALUES ('proj-datumgrid-world',
                                  'Package with grids of global extent (too large to be included in proj-datumgrid)',
                                  'https://download.osgeo.org/proj/proj-datumgrid-world-1.0.zip',
                                  1,
                                  1);

------------------------------------------------------
-- grid_alternatives
------------------------------------------------------

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('null',
                              'null',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('rgf93_ntf.gsb',
                              'ntf_r93.gsb',    -- the PROJ grid is the reverse way of the EPSG one
                              'NTv2',
                              'hgridshift',
                              1,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NTv1_0.gsb',
                              'ntv1_can.dat',
                              'NTv1',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NTv2_0.gsb',
                              'ntv2_0.gsb', -- just a case change
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('May76v20.gsb',
                              'MAY76V20.gsb', -- just a case change
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('BETA2007.gsb',
                              'BETA2007.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('BWTA2017.gsb',
                              'BWTA2017.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('SeTa2016.gsb',
                              'SeTa2016.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NTv2_SN.gsb',
                              'NTv2_SN.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('AT_GIS_GRID.gsb',
                              'AT_GIS_GRID.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('nzgd2kgrid0005.gsb',
                              'nzgd2kgrid0005.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('OSTN15_NTv2_OSGBtoETRS.gsb',
                              'OSTN15_NTv2_OSGBtoETRS.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Continental USA VERTCON: NGVD (19)29 height to NAVD (19)88 height

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('vertconw.94',
                              'vertconw.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('vertconc.94',
                              'vertconc.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('vertcone.94',
                              'vertcone.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

-- EGM models

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('WW15MGH.GRD',
                              'egm96_15.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('Und_min2.5x2.5_egm2008_isw=82_WGS84_TideFree.gz',
                              'egm08_25.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-world',
                              NULL, NULL, NULL, NULL);

-- Greenland height models

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('gr2000g.gri',
                              'gvr2000.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ggeoid16.gri',
                              'gvr2016.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

-- Denmark height models

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('dvr90.gtx',
                              'dvr90.gtx', -- no change. Just document the package
                              'GTX',
                              'vgridshift',
                              0,         -- do not reverse here as grid_transformation_custom does reference from VerticalCRS height to Geographic height
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);


INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('dnn.gtx',
                              'dnn.gtx', -- no change. Just document the package
                              'GTX',
                              'vgridshift',
                              0,         -- do not reverse here as grid_transformation_custom does reference from VerticalCRS height to Geographic height
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Faroe islands height models

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('fvr09.gtx',
                              'fvr09.gtx', -- no change. Just document the package
                              'GTX',
                              'vgridshift',
                              0,         -- do not reverse here as grid_transformation_custom does reference from VerticalCRS height to Geographic height
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Sweden height models

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('SWEN17_RH2000.gtx',
                              'SWEN17_RH2000.gtx', -- no change. Just document the package
                              'GTX',
                              'vgridshift',
                              0,         -- do not reverse here as grid_transformation_custom does reference from VerticalCRS height to Geographic height
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Ireland: OSGM15 height, Malin head datum -> ETRS89 ellipsoidal heights

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('OSGM15_Malin.gri',
                              'OSGM15_Malin.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Northern Ireland: OSGM15 height, Belfast height -> ETRS89 ellipsoidal heights

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('OSGM15_Belfast.gri',
                              'OSGM15_Belfast.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

----------------------------
-- US GEOID99 height models
----------------------------

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u01.bin', 'g1999u01.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u02.bin', 'g1999u02.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u03.bin', 'g1999u03.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u04.bin', 'g1999u04.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u05.bin', 'g1999u05.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u06.bin', 'g1999u06.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u07.bin', 'g1999u07.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g1999u08.bin', 'g1999u08.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

-- Not mapped:
-- g1999a01.gtx to g1999a04.gtx : Alaska
-- g1999h01.gtx : Hawaii
-- g1999p01.gtx : Puerto Rico

----------------------------
-- US GEOID03 height models
----------------------------

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('geoid03_conus.bin', 'geoid03_conus.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

-- Not mapped:
-- g2003a01.gtx to g2003a04.gtx : Alaska
-- g2003h01.gtx : Hawaii
---g2003p01.gtx : Puerto Rico

----------------------------
-- US GEOID06 height models
----------------------------

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('geoid06_ak.bin', 'geoid06_ak.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

----------------------------
-- US GEOID09 height models
----------------------------

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('geoid09_ak.bin', 'geoid09_ak.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('geoid09_conus.bin', 'geoid09_conus.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g2009g01.bin', 'g2009g01.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g2009s01.bin', 'g2009s01.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g2009p01.bin', 'g2009p01.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

-- Not mapped:
-- g2009h01.gtx : Hawaii

----------------------------
-- US GEOID12B height models
----------------------------

-- CONUS
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('g2012bu0.bin',
                              'g2012bu0.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

-- Alaska
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('g2012ba0.bin',
                              'g2012ba0.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

-- Puerto Rico
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('g2012bp0.bin',
                              'g2012bp0.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

-- Guam
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('g2012bg0.bin',
                              'g2012bg0.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

-- American Samoa
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('g2012bs0.bin',
                              'g2012bs0.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

----------------------------
-- US GEOID18 height models
----------------------------

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g2018u0.bin', 'g2018u0.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name, proj_grid_name, proj_grid_format, proj_method, inverse_direction, package_name, url, direct_download, open_license, directory)
    VALUES ('g2018p0.bin', 'g2018p0.gtx', 'GTX', 'vgridshift', 1, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);

----------------------------
-- French vertical grids
----------------------------

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('RAF09.mnt',
                              'RAF09.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('RAF18.tac',
                              'RAF18.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('RAC09.mnt',
                              'RAC09.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggm00.txt',
                            'ggm00v2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggg00.txt',
                            'ggg00v2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggg00_mg.txt',
                            'ggg00_mgv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggg00_sm.txt',
                            'ggg00_smv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggg00_ls.txt',
                            'ggg00_lsv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggg00_ld.txt',
                            'RALDW842016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('RALDW842016.mnt',
                            'RALDW842016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggg00_sb.txt',
                            'ggg00_sbv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_mart.txt',
                            'RAMART2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('RAMART2016.mnt',
                            'RAMART2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_gtbt.txt',
                            'RAGTBT2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('RAGTBT2016.mnt',
                            'RAGTBT2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_mg.txt',
                            'RAMG2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('RAMG2016.mnt',
                            'RAMG2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_sm.txt',
                            'gg10_smv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_smv2.mnt',
                            'gg10_smv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_ls.txt',
                            'RALS2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('RALS2016.mnt',
                            'RALS2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_ld.txt',
                            'RALD2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('RALD2016.mnt',
                            'RALD2016.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_sb.txt',
                            'gg10_sbv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('gg10_sbv2.mnt',
                            'gg10_sbv2.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggguy00.txt',
                            'ggguy15.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('ggr99.txt',
                            'RAR07_bl.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('GGSPM06v1.mnt',
                            'ggspm06v1.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                            proj_grid_name,
                            proj_grid_format,
                            proj_method,
                            inverse_direction,
                            package_name,
                            url, direct_download, open_license, directory)
                    VALUES ('RASPM2018.mnt',
                            'RASPM2018.gtx',
                            'GTX',
                            'vgridshift',
                            1,
                            'proj-datumgrid-europe',
                            NULL, NULL, NULL, NULL);

----------------------------
-- Australian grids
----------------------------

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('A66 National (13.09.01).gsb',
                              'A66_National_13_09_01.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('National 84 (02.07.01).gsb',
                              'National_84_02_07_01.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('GDA94_GDA2020_conformal.gsb',
                              'GDA94_GDA2020_conformal.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('GDA94_GDA2020_conformal_and_distortion.gsb',
                              'GDA94_GDA2020_conformal_and_distortion.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('GDA94_GDA2020_conformal_christmas_island.gsb',
                              'GDA94_GDA2020_conformal_christmas_island.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('GDA94_GDA2020_conformal_cocos_island.gsb',
                              'GDA94_GDA2020_conformal_cocos_island.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('AUSGeoid09_GDA94_V1.01_DOV_windows.gsb',  -- source file contains undulation in first band, and deflection in 2nd and 3d band
                              'AUSGeoid09_V1.01.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('AUSGeoid2020_windows_binary.gsb',  -- source file contains undulation in first band, and deflection in 2nd and 3d band
                              'AUSGeoid2020_20180201.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
    SELECT grid_name,
           'AUSGeoid98.gtx',
           'GTX',
           'vgridshift',
           1,
           'proj-datumgrid-oceania',
           NULL, NULL, NULL, NULL FROM grid_transformation WHERE
                grid_name LIKE '%DAT.htm' AND name LIKE 'GDA94 to AHD height%';

-- Netherlands / RDNAP (non-free grids)

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('naptrans2008.gtx',
                              'naptrans2008.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              NULL, -- package name
                              'https://salsa.debian.org/debian-gis-team/proj-rdnap/raw/upstream/2008/naptrans2008.gtx',
                              1, -- direct download
                              0, -- non-freely licensed. See https://salsa.debian.org/debian-gis-team/proj-rdnap/raw/master/debian/copyright
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('rdtrans2008.gsb',
                              'rdtrans2008.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              NULL, -- package name
                              'https://salsa.debian.org/debian-gis-team/proj-rdnap/raw/upstream/2008/rdtrans2008.gsb',
                              1, -- direct download
                              0, -- non-freely licensed. See https://salsa.debian.org/debian-gis-team/proj-rdnap/raw/master/debian/copyright
                              NULL);

-- Belgium

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('bd72lb72_etrs89lb08.gsb',
                              'bd72lb72_etrs89lb08.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Switzerland

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CHENyx06a.gsb',
                              'CHENyx06a.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CHENyx06_ETRS.gsb',
                              'CHENyx06_ETRS.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Spain

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('100800401.gsb',
                              '100800401.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Portugal

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('DLx_ETRS89_geo.gsb',
                              'DLx_ETRS89_geo.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('D73_ETRS89_geo.gsb',
                              'D73_ETRS89_geo.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- Canada provincial grids

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('AB_CSRS.DAC',
                              'ABCSRSV4.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CRD27_00.GSB',
                              'CRD27_00.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CRD93_00.GSB',
                              'CRD93_00.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NVI93_05.GSB',
                              'NVI93_05.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('BC_27_05.GSB',
                              'BC_27_05.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('BC_93_05.GSB',
                              'BC_93_05.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NB7783v2.gsb', -- case difference on extension !
                              'NB7783v2.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NB2783v2.gsb', -- case difference on extension !
                              'NB2783v2.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('GS7783.GSB',
                              'GS7783.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NS778302.gsb', -- case difference on extension !
                              'NS778302.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ON27CSv1.GSB',
                              'ON27CSv1.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ON76CSv1.GSB',
                              'ON76CSv1.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ON83CSv1.GSB',
                              'ON83CSv1.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('TOR27CSv1.GSB',
                              'TO27CSv1.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('PE7783V2.gsb', -- case difference on extension !
                              'PE7783V2.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NA27NA83.GSB',
                              'na27na83.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

-- two grid names in EPSG point to the same file distributed by NRCan
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NA27SCRS.GSB',
                              'NA27SCRS.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('QUE27-98.gsb',
                              'NA27SCRS.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CQ77NA83.GSB',
                              'cq77na83.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CGQ77-98.gsb',
                              'CQ77SCRS.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

 -- two grid names in EPSG point to the same file distributed by NRCan
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NA83SCRS.GSB',
                              'NA83SCRS.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('NAD83-98.gsb',
                              'NA83SCRS.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('SK27-98.gsb', -- case difference on extension !
                              'SK27-98.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('SK83-98.gsb', -- case difference on extension !
                              'SK83-98.GSB',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('HT2_0.byn',
                              'HT2_2010v70.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CGG2013i08a.byn',
                              'CGG2013ai08.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CGG2013n83a.byn',
                              'CGG2013an83.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CGG2013i83.byn',
                              'CGG2013i08.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('CGG2013n83.byn',
                              'CGG2013n83.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);

 -- Iceland

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ISN93_ISN2016.gsb',
                              'ISN93_ISN2016.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ISN2004_ISN2016.gsb',
                              'ISN2004_ISN2016.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('Icegeoid_ISN2004.gtx',
                              'Icegeoid_ISN2004.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('Icegeoid_ISN93.gtx',
                              'Icegeoid_ISN93.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('Icegeoid_ISN2016.gtx',
                              'Icegeoid_ISN2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-europe',
                              NULL, NULL, NULL, NULL);

-- New Zealand grid shift models.  EPSG names are not for GTX files (at time of writing)
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('auckland-1946-to-nzvd2016-conversion.csv',
                              'auckht1946-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('bluff-1955-to-nzvd2016-conversion.csv',
                              'blufht1955-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('dunedin-1958-to-nzvd2016-conversion.csv',
                              'duneht1958-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('dunedin-bluff-1960-to-nzvd2016-conversion.csv',
                              'dublht1960-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('gisborne-1926-to-nzvd2016-conversion.csv',
                              'gisbht1926-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('lyttelton-1937-to-nzvd2016-conversion.csv',
                              'lyttht1937-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('moturiki-1953-to-nzvd2016-conversion.csv',
                              'motuht1953-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('napier-1962-to-nzvd2016-conversion.csv',
                              'napiht1962-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('nelson-1955-to-nzvd2016-conversion.csv',
                              'nelsht1955-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('onetreepoint-1964-to-nzvd2016-conversion.csv',
                              'ontpht1964-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('stewartisland-1977-to-nzvd2016-conversion.csv',
                              'stisht1977-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('taranaki-1970-to-nzvd2016-conversion.csv',
                              'taraht1970-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('wellington-1953-to-nzvd2016-conversion.csv',
                              'wellht1953-nzvd2016.gtx',
                              'GTX',
                              'vgridshift',
                              0,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('New_Zealand_Quasigeoid_2016.csv',
                              'nzgeoid2016.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('nzgeoid09.sid',
                              'nzgeoid2009.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-oceania',
                              NULL, NULL, NULL, NULL);



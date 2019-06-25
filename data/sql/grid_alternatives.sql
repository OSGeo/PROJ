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
                                  'https://download.osgeo.org/proj/proj-datumgrid-latest.zip',
                                  1,
                                  1);

INSERT INTO grid_packages VALUES ('proj-datumgrid-north-america',
                                  'Package with grids of interest for North-America',
                                  'https://download.osgeo.org/proj/proj-datumgrid-north-america-latest.zip',
                                  1,
                                  1);

INSERT INTO grid_packages VALUES ('proj-datumgrid-europe',
                                  'Package with grids of interest for Europe',
                                  'https://download.osgeo.org/proj/proj-datumgrid-europe-latest.zip',
                                  1,
                                  1);

INSERT INTO grid_packages VALUES ('proj-datumgrid-oceania',
                                  'Package with grids of interest for Oceania',
                                  'https://download.osgeo.org/proj/proj-datumgrid-oceania-latest.zip',
                                  1,
                                  1);

 -- not released yet at the time of writing
INSERT INTO grid_packages VALUES ('proj-datumgrid-world',
                                  'Package with grids of global extent (too large to be included in proj-datumgrid)',
                                  'https://download.osgeo.org/proj/proj-datumgrid-world-latest.zip',
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
                      VALUES ('g2012Bu0.bin',
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
                      VALUES ('g2012Ba0.bin',
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
                      VALUES ('g2012Bp0.bin',
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
                      VALUES ('g2012Bg0.bin',
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
                      VALUES ('g2012Bs0.bin',
                              'g2012bs0.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);


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

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
                              0,
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
                              0,
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
                              0,
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
                              0,
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
                              0,
                              'proj-datumgrid-north-america',
                              NULL, NULL, NULL, NULL);



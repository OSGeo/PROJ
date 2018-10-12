-- This file is hand generated.

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
                                  'https://download.osgeo.org/proj/proj-datumgrid-north-america-1.1.zip',
                                  1,
                                  1);

INSERT INTO grid_packages VALUES ('proj-datumgrid-europe',
                                  'Package with grids of interest for Europe',
                                  'https://download.osgeo.org/proj/proj-datumgrid-europe-1.1.zip',
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
                      VALUES ('ntf_r93.gsb',    -- as referenced by the IGNF registry
                              'ntf_r93.gsb',
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

-- NADCON entries

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('conus.las',
                              'conus',
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
                      VALUES ('alaska.las',
                              'alaska',
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
                      VALUES ('hawaii.las',
                              'hawaii',
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
                      VALUES ('prvi.las',
                              'prvi',
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
                      VALUES ('stgeorge.las',
                              'stgeorge',
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
                      VALUES ('stlrnc.las',
                              'stlrnc',
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
                      VALUES ('stpaul.las',
                              'stpaul',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);

-- NAD83 -> NAD83(HPGN) entries

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('flhpgn.las',             -- Florida
                              'FL',
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
                      VALUES ('mdhpgn.las',             -- Maryland
                              'MD',
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
                      VALUES ('tnhpgn.las',             -- Tennessee
                              'TN',
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
                      VALUES ('wihpgn.las',             -- Wisconsin
                              'WI',
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
                      VALUES ('wohpgn.las',             -- Washington, Oregon, N. California
                              'WO',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
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



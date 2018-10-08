-- This file is hand generated.

------------------------------------------------------
-- grid_packages
------------------------------------------------------

INSERT INTO grid_packages VALUES ('proj-datumgrid',
                                  'https://download.osgeo.org/proj/proj-datumgrid-1.8.zip');

INSERT INTO grid_packages VALUES ('proj-datumgrid-north-america',
                                  'https://download.osgeo.org/proj/proj-datumgrid-north-america-1.1.zip');

 -- not released yet at the time of writing
INSERT INTO grid_packages VALUES ('proj-datumgrid-world',
                                  'https://download.osgeo.org/proj/proj-datumgrid-world-1.0.zip');

------------------------------------------------------
-- grid_alternatives
------------------------------------------------------

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('rgf93_ntf.gsb',
                              'ntf_r93.gsb',    -- the PROJ grid is the reverse way of the EPSG one
                              'NTv2',
                              'hgridshift',
                              1,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('NTv1_0.gsb',
                              'ntv1_can.dat',
                              'NTv1',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('NTv2_0.gsb',
                              'ntv2_0.gsb', -- just a case change
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid-north-america',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('BETA2007.gsb',
                              'BETA2007.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('nzgd2kgrid0005.gsb',
                              'nzgd2kgrid0005.gsb', -- no change. Just document the package
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

-- NADCON entries

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('conus.las',
                              'conus',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('alaska.las',
                              'alaska',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('hawaii.las',
                              'hawaii',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('prvi.las',
                              'prvi',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('stgeorge.las',
                              'stgeorge',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('stlrnc.las',
                              'stlrnc',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('stpaul.las',
                              'stpaul',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

-- NAD83 -> NAD83(HPGN) entries

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('flhpgn.las',             -- Florida
                              'FL',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('mdhpgn.las',             -- Maryland
                              'MD',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('tnhpgn.las',             -- Tennessee
                              'TN',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('wihpgn.las',             -- Wisconsin
                              'WI',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('wohpgn.las',             -- Washington, Oregon, N. California
                              'WO',
                              'CTable2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL);

-- EGM models

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('WW15MGH.GRD',
                              'egm96_15.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid',
                              NULL);

INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              directory)
                      VALUES ('Und_min2.5x2.5_egm2008_isw=82_WGS84_TideFree.gz',
                              'egm08_25.gtx',
                              'GTX',
                              'vgridshift',
                              1,
                              'proj-datumgrid-world',
                              NULL);

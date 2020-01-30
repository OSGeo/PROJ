DATAPATH = $(top_srcdir)/data

pkgdata_DATA = proj.ini GL27 nad.lst nad27 nad83 world other.extra \
		CH \
		ITRF2000 ITRF2008 ITRF2014 proj.db \
		projjson.schema.json

SQL_ORDERED_LIST = sql/begin.sql \
		sql/proj_db_table_defs.sql \
		sql/conversion_triggers.sql \
		sql/metadata.sql \
		sql/unit_of_measure.sql \
		sql/area.sql \
		sql/coordinate_system.sql \
		sql/axis.sql \
		sql/ellipsoid.sql \
		sql/prime_meridian.sql \
		sql/geodetic_datum.sql \
		sql/vertical_datum.sql \
		sql/conversion.sql \
		sql/geodetic_crs.sql \
		sql/projected_crs.sql \
		sql/vertical_crs.sql \
		sql/compound_crs.sql \
		sql/helmert_transformation.sql \
		sql/grid_transformation.sql \
		sql/grid_transformation_custom.sql \
		sql/other_transformation.sql \
		sql/concatenated_operation.sql \
		sql/concatenated_operation_step.sql \
		sql/alias_name.sql \
		sql/supersession.sql \
		sql/deprecation.sql \
		sql/esri.sql \
		sql/ignf.sql \
		sql/grid_alternatives.sql \
		sql/grid_alternatives_generated_noaa.sql \
		sql/customizations.sql \
		sql/commit.sql

EXTRA_DIST = proj.ini GL27 nad.lst nad27 nad83 \
		world other.extra \
		CH \
		ITRF2000 ITRF2008 ITRF2014 \
		projjson.schema.json \
		CMakeLists.txt \
		tests/test_nodata.gtx \
		tests/test_vgrid_bigendian_bigtiff.tif \
		tests/test_vgrid_bigendian.tif \
		tests/test_vgrid_bigtiff.tif \
		tests/test_vgrid_bottomup_with_matrix.tif \
		tests/test_vgrid_bottomup_with_scale.tif \
		tests/test_vgrid_deflate_floatingpointpredictor.tif \
		tests/test_vgrid_deflate.tif \
		tests/test_vgrid_float64.tif \
		tests/test_vgrid_in_second_channel.tif \
		tests/test_vgrid_int16.tif \
		tests/test_vgrid_int32.tif \
		tests/test_vgrid_uint32.tif \
		tests/test_vgrid_invalid_channel_type.tif \
		tests/test_vgrid_nodata.tif \
		tests/test_vgrid_pixelisarea.tif \
		tests/test_vgrid_pixelispoint.tif \
		tests/test_vgrid_uint16.tif \
		tests/test_vgrid_uint16_with_scale_offset.tif \
		tests/test_vgrid_unsupported_byte.tif \
		tests/test_vgrid_with_overview.tif \
		tests/test_vgrid_with_subgrid.tif \
		tests/test_hgrid.tif \
		tests/test_hgrid_separate.tif \
		tests/test_hgrid_tiled.tif \
		tests/test_hgrid_tiled_separate.tif \
		tests/test_hgrid_strip.tif \
		tests/test_hgrid_positive_west.tif \
		tests/test_hgrid_lon_shift_first.tif \
		tests/test_hgrid_radian.tif \
		tests/test_hgrid_degree.tif \
		tests/test_hgrid_with_overview.tif \
		tests/test_hgrid_extra_ifd_with_other_info.tif \
		tests/test_hgrid_with_subgrid.tif \
		tests/test_hgrid_with_subgrid_no_grid_name.tif \
		tests/subset_of_gr3df97a.tif \
		tests/egm96_15_uncompressed_truncated.tif \
		tests/test_vgrid_single_strip_truncated.tif \
		tests/nkgrf03vel_realigned_extract.tif \
		tests/nkgrf03vel_realigned_xy_extract.ct2 \
		tests/nkgrf03vel_realigned_z_extract.gtx \
		tests/test_hgrid_with_two_level_of_subgrids_no_grid_name.tif \
		tests/us_noaa_geoid06_ak_subset_at_antimeridian.tif \
		generate_all_sql_in.cmake sql_filelist.cmake \
		$(SQL_ORDERED_LIST)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	@for gridfile in $(DATAPATH)/*.gsb $(DATAPATH)/*.gtx $(DATAPATH)/ntv1_can.dat dummy \
	                 $(DATAPATH)/alaska $(DATAPATH)/conus $(DATAPATH)/hawaii \
	                 $(DATAPATH)/prvi $(DATAPATH)/stgeorge $(DATAPATH)/stlrnc $(DATAPATH)/stpaul \
	                 $(DATAPATH)/FL $(DATAPATH)/MD $(DATAPATH)/TN $(DATAPATH)/WI $(DATAPATH)/WO; do \
	  if test "$$gridfile" != "dummy" -a -f "$$gridfile" ; then \
	    echo $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/`basename $$gridfile`; \
	    $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/`basename $$gridfile`; \
          fi; \
	done

# head -c1 not handled on Solaris
proj.db: $(DATAPATH)/sql/*.sql
	@echo "Make proj.db"
	$(RM) proj.db
	@export SQL_EXPANDED_LIST=""; \
	 for x in $(SQL_ORDERED_LIST); do \
		export SQL_EXPANDED_LIST="$${SQL_EXPANDED_LIST} $(DATAPATH)/$$x"; \
	 done; \
	 if test "x$(PROJ_DB_CACHE_DIR)" != "x" -a -x "$$(command -v md5sum)" -a -f "$(PROJ_DB_CACHE_DIR)/proj.db" -a -f "$(PROJ_DB_CACHE_DIR)/proj.db.sql.md5" ; then \
	    cat $${SQL_EXPANDED_LIST} | md5sum | diff - "$(PROJ_DB_CACHE_DIR)/proj.db.sql.md5" > /dev/null \
	    && (echo "Reusing cached proj.db"; cp "$(PROJ_DB_CACHE_DIR)/proj.db" proj.db); \
	 fi; \
	 if test ! -f proj.db ; then \
	   cat $${SQL_EXPANDED_LIST} | sqlite3 proj.db; \
	 fi; \
	 if [ $$? -ne 0 ] ; then \
		echo "Build of proj.db failed"; \
		$(RM) proj.db; \
		exit 1; \
	fi; \
	echo "" | head -c1; \
        if [ $$? -eq 0 ] ; then \
         echo "Running foreign_key_check"; \
	 if [[ $$(echo "pragma foreign_key_check;" | sqlite3 proj.db | head -c1 | wc -c) -ne 0 ]]; then \
		echo "Foreign key check failed"; \
		$(RM) proj.db; \
		exit 1; \
	 else \
	   if test "x$(PROJ_DB_CACHE_DIR)" != "x" -a -x "$$(command -v md5sum)" ; then \
		mkdir -p "$(PROJ_DB_CACHE_DIR)"; \
		cat $${SQL_EXPANDED_LIST} | md5sum > "$(PROJ_DB_CACHE_DIR)/proj.db.sql.md5"; \
		cp proj.db "$(PROJ_DB_CACHE_DIR)"; \
	   fi \
	 fi \
	fi

# For out-of-tree builds, link all file of the source data dir to the generated data
# Also link select resource files in a for_tests subdirectory so that we are not
# influenced by the presence of other grids
check-local:
	@if [ ! -f GL27 ]; then \
		for x in $(DATAPATH)/*; do \
			ln -sf $$x .; \
		done \
	fi; \
	rm -rf for_tests; \
	mkdir for_tests; \
	for x in $(DATAPATH)/GL27 \
		 $(DATAPATH)/nad27 \
		 $(DATAPATH)/nad83 \
		 $(DATAPATH)/ntv1_can.dat \
		 $(DATAPATH)/MD \
		 $(DATAPATH)/ntf_r93.gsb \
		 $(DATAPATH)/egm96_15.gtx \
		 $(DATAPATH)/ntv2_0.gsb \
		 $(DATAPATH)/conus \
		 $(DATAPATH)/alaska \
		 $(DATAPATH)/nzgd2kgrid0005.gsb \
		 $(DATAPATH)/ITRF2000 \
		 $(DATAPATH)/BETA2007.gsb; \
	do \
		if test -f "$$x" ; then \
			ln -sf "../$$x" for_tests; \
		else \
			echo "WARNING: grid $$x missing: some tests will be skipped"; \
		fi \
	done; \
	ln -sf ../$(DATAPATH)/tests for_tests; \
	ln -sf ../$(DATAPATH)/proj.ini for_tests; \
	ln -sf ../proj.db for_tests

clean-local:
	$(RM) proj.db
	$(RM) -rf for_tests

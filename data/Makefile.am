DATAPATH = $(top_srcdir)/data

pkgdata_DATA = GL27 nad.lst nad27 nad83 world other.extra \
		CH null \
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
		sql/alias_name.sql \
		sql/supersession.sql \
		sql/deprecation.sql \
		sql/esri.sql \
		sql/ignf.sql \
		sql/grid_alternatives.sql \
		sql/grid_alternatives_generated.sql \
		sql/customizations.sql \
		sql/commit.sql

EXTRA_DIST = GL27 nad.lst nad27 nad83 \
		world other.extra \
		CH \
		ITRF2000 ITRF2008 ITRF2014 \
		projjson.schema.json \
		CMakeLists.txt tests/test_nodata.gtx null \
		generate_all_sql_in.cmake sql_filelist.cmake \
		$(SQL_ORDERED_LIST)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	@for gridfile in $(DATAPATH)/*.gsb $(DATAPATH)/*.gtx $(DATAPATH)/ntv1_can.dat dummy \
	                 $(DATAPATH)/alaska $(DATAPATH)/conus $(DATAPATH)/hawaii $(DATAPATH)/null \
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
check-local:
	@if [ ! -f GL27 ]; then \
		for x in $(DATAPATH)/*; do \
			ln -sf $$x .; \
		done \
	fi

clean-local:
	$(RM) proj.db

DATAPATH = $(top_srcdir)/data

pkgdata_DATA = GL27 nad.lst proj_def.dat nad27 nad83 world epsg esri \
		esri.extra other.extra \
		CH IGNF \
		ITRF2000 ITRF2008 ITRF2014

EXTRA_DIST = GL27 nad.lst proj_def.dat nad27 nad83 \
		world epsg esri \
		esri.extra other.extra \
		CH IGNF \
		ITRF2000 ITRF2008 ITRF2014 \
		CMakeLists.txt tests/test_nodata.gtx null.lla

process-nad2bin:
	@if [ -f $(DATAPATH)/null.lla -a ! -f null ] || [ -f $(DATAPATH)/conus.lla -a ! -f conus ] ; then \
	  for x in $(DATAPATH)/*.lla ; do \
	    DEST=`basename $$x .lla`; \
	    echo "../src/nad2bin$(EXEEXT) $$DEST < $$x"; \
	    ../src/nad2bin$(EXEEXT) $$DEST < $$x ; \
	  done; \
	fi

install-data-local: process-nad2bin
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	@if [ -f $(DATAPATH)/null.lla -a ! -f $(DATAPATH)/null ] || [ -f $(DATAPATH)/conus.lla -a ! -f $(DATAPATH)/conus ] ; then \
	  for x in $(DATAPATH)/*.lla ; do \
	    DEST=$(DESTDIR)$(pkgdatadir)/`basename $$x .lla`; \
	    echo "../src/nad2bin$(EXEEXT) $$DEST < $$x"; \
	    ../src/nad2bin$(EXEEXT) $$DEST < $$x ; \
	    if [ $$? -ne 0 ] ; then \
		echo "nad2bin$(EXEEXT) failed to install $$DEST"; \
		exit 1; \
	    fi; \
	  done; \
	else \
	  echo "nad2nad NADCON source files not present"; \
	fi
	@for gridfile in $(DATAPATH)/*.gsb $(DATAPATH)/*.gtx $(DATAPATH)/ntv1_can.dat dummy \
	                 $(DATAPATH)/alaska $(DATAPATH)/conus $(DATAPATH)/hawaii $(DATAPATH)/null \
	                 $(DATAPATH)/prvi $(DATAPATH)/stgeorge $(DATAPATH)/stlrnc $(DATAPATH)/stpaul \
	                 $(DATAPATH)/FL $(DATAPATH)/MD $(DATAPATH)/TN $(DATAPATH)/WI $(DATAPATH)/WO; do \
	  if test "$$gridfile" != "dummy" -a -f "$$gridfile" ; then \
	    echo $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/`basename $$gridfile`; \
	    $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/`basename $$gridfile`; \
          fi; \
	done

# For out-of-tree builds, link all file of the source data dir to the generated data
check-local: process-nad2bin
	@if [ ! -f GL27 ]; then \
		for x in $(DATAPATH)/*; do \
			ln -sf $$x .; \
		done \
	fi

# Executables paths passed to test scripts
DATAPATH = $(top_srcdir)/data
EXEPATH = ../src
PROJEXE = $(EXEPATH)/proj
CS2CSEXE = $(EXEPATH)/cs2cs

# PROJ.4 test scripts
TEST27 = $(DATAPATH)/test27
TEST83 = $(DATAPATH)/test83
TESTNTV2 = $(DATAPATH)/testntv2
TESTVARIOUS = $(DATAPATH)/testvarious
TESTFLAKY = $(DATAPATH)/testflaky
TESTDATUMFILE = $(DATAPATH)/testdatumfile
TESTIGN = $(DATAPATH)/testIGNF

pkgdata_DATA = GL27 nad.lst proj_def.dat nad27 nad83 world epsg esri \
		esri.extra other.extra \
		CH IGNF \
		ITRF2000 ITRF2008 ITRF2014

EXTRA_DIST = GL27 nad.lst proj_def.dat nad27 nad83 pj_out27.dist pj_out83.dist td_out.dist \
		test27 test83 world epsg esri tv_out.dist tf_out.dist \
		testflaky testvarious testdatumfile testntv2 ntv2_out.dist \
		esri.extra other.extra \
		CH IGNF testIGNF proj_outIGNF.dist \
		ITRF2000 ITRF2008 ITRF2014 \
		makefile.vc CMakeLists.txt tests/test_nodata.gtx null.lla

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

check-local: process-nad2bin
# Check if we are doing an out-of-tree build
	@if test ! -f epsg; then \
	  for gridfile in $(DATAPATH)/*.gsb $(DATAPATH)/*.gtx $(DATAPATH)/ntv1_can.dat dummy ; do \
	    if test "$$gridfile" != "dummy" -a -f "$$gridfile" ; then \
	      cp $$gridfile .; \
	    fi; \
	  done; \
	  for gridfile in $(pkgdata_DATA); do \
	    cp $(DATAPATH)/$$gridfile .; \
	  done; \
	fi
	$(TEST27) $(PROJEXE)
	$(TEST83) $(PROJEXE)
	PROJ_LIB=. $(TESTVARIOUS) $(CS2CSEXE)
	@if [ -f conus ] ; then \
	  export PROJ_LIB=. ; \
	  $(TESTDATUMFILE) $(CS2CSEXE) ; \
	fi
	@if [ -f ntf_r93.gsb ] ; then \
	  export PROJ_LIB=. ; \
	  $(TESTIGN) $(CS2CSEXE) ; \
	fi
	@if [ -f ntv2_0.gsb ] ; then \
	  export PROJ_LIB=. ; \
	  $(TESTNTV2) $(CS2CSEXE) ; \
	fi

clean-local:
	$(RM) $(TEST_DB)

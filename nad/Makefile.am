# Executables paths passed to test scripts
EXEPATH = $(top_srcdir)/src
PROJEXE = $(EXEPATH)/proj
CS2CSEXE = $(EXEPATH)/cs2cs

# PROJ.4 test scripts
TEST27 = ./test27
TEST83 = ./test83
TESTNTV2 = ./testntv2
TESTVARIOUS = ./testvarious
TESTDATUMFILE = ./testdatumfile
TESTIGN = ./testIGNF

pkgdata_DATA = GL27 nad.lst nad27 nad83 proj_def.dat world epsg esri \
		esri.extra other.extra \
		IGNF

EXTRA_DIST = GL27 nad.lst nad27 nad83 pj_out27.dist pj_out83.dist td_out.dist \
		proj_def.dat test27 test83 world epsg esri tv_out.dist \
		testvarious testdatumfile testntv2 ntv2_out.dist \
		esri.extra other.extra \
		IGNF testIGNF proj_outIGNF.dist \
		makefile.vc

process-nad2bin:
	@if [ -f conus.lla -a ! -f conus ] ; then \
	  for x in *.lla ; do \
	    DEST=`basename $$x .lla`; \
	    echo "../src/nad2bin$(EXEEXT) $$DEST < $$x"; \
	    ../src/nad2bin$(EXEEXT) $$DEST < $$x ; \
	  done; \
	fi 
	
install-data-local: process-nad2bin
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	@if [ -f conus.lla ] ; then \
	  for x in *.lla ; do \
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
	@for gridfile in *.gsb ntv1_can.dat dummy ; do \
	  if test "$$gridfile" != "dummy" -a -f "$$gridfile" ; then \
	    echo $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/$$gridfile; \
	    $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/$$gridfile; \
          fi; \
	done

check-local: process-nad2bin
	$(TEST27) $(PROJEXE)
	$(TEST83) $(PROJEXE)
	$(TESTVARIOUS) $(CS2CSEXE)
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

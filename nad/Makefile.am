pkgdata_DATA = GL27 nad.lst nad27 nad83 pj_out27.dist pj_out83.dist \
			proj_def.dat test27 test83 world epsg

EXTRA_DIST = GL27 nad.lst nad27 nad83 pj_out27.dist pj_out83.dist \
			proj_def.dat test27 test83 world

install-data-local:
	@if [ -f conus.lla ] ; then \
	  for x in *.lla ; do \
	    DEST=$(pkgdatadir)/`basename $$x .lla`; \
	    echo "../src/nad2bin $$DEST < $$x"; \
	    ../src/nad2bin $$DEST < $$x ; \
	    if [ $$? -ne 0 ] ; then \
		echo "nad2bin failed to install $$DEST"; \
		exit 1; \
	    fi; \
	  done; \
	else \
	  echo "nad2nad NADCON source files not present"; \
	fi 

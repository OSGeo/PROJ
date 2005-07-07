pkgdata_DATA = GL27 nad.lst nad27 nad83 proj_def.dat world epsg esri

EXTRA_DIST = GL27 nad.lst nad27 nad83 pj_out27.dist pj_out83.dist td_out.dist \
			proj_def.dat test27 test83 world epsg esri \
			testvarious testntv2 ntv2_out.dist

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	@if [ -f conus.lla ] ; then \
	  for x in *.lla ; do \
	    DEST=$(DESTDIR)$(pkgdatadir)/`basename $$x .lla`; \
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
	@for gridfile in *.gsb ntv1_can.dat dummy ; do \
	  if test "$$gridfile" != "dummy" -a -f "$$gridfile" ; then \
	    echo $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/$$gridfile; \
	    $(INSTALL_DATA) $$gridfile $(DESTDIR)$(pkgdatadir)/$$gridfile; \
          fi; \
	done



EXEPATH = ../../src
GIEEXE = $(EXEPATH)/gie

EXTRA_DIST = 4D-API_cs2cs-style.gie \
	GDA.gie \
	axisswap.gie \
	builtins.gie \
	deformation.gie \
	ellipsoid.gie \
	more_builtins.gie \
	unitconvert.gie

4D-API-cs2cs-style: 4D-API_cs2cs-style.gie
	$(GIEEXE) $<

GDA: GDA.gie
	$(GIEEXE) $<

axisswap: axisswap.gie
	$(GIEEXE) $<

builtins: builtins.gie
	$(GIEEXE) $<

deformation: deformation.gie
	$(GIEEXE) $<

ellipsoid: ellipsoid.gie
	$(GIEEXE) $<

more_builtins: more_builtins.gie
	$(GIEEXE) $<

unitconvert: unitconvert.gie
	$(GIEEXE) $<

check-local: 4D-API-cs2cs-style GDA axisswap builtins deformation ellipsoid more_builtins unitconvert

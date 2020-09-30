EXEPATH = ../../src
GIEEXE = $(EXEPATH)/gie

EXTRA_DIST = 4D-API_cs2cs-style.gie \
	GDA.gie \
	axisswap.gie \
	builtins.gie \
	deformation.gie \
	ellipsoid.gie \
	more_builtins.gie \
	unitconvert.gie \
	DHDN_ETRS89.gie \
	geotiff_grids.gie \
	adams_hemi.gie \
	adams_ws1.gie \
	adams_ws2.gie \
	guyou.gie \
	peirce_q.gie \
	defmodel.gie \
	tinshift.gie

PROJ_LIB ?= ../../data/for_tests

4D-API-cs2cs-style: 4D-API_cs2cs-style.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

GDA: GDA.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

axisswap: axisswap.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

builtins: builtins.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

deformation: deformation.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

ellipsoid: ellipsoid.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

more_builtins: more_builtins.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

unitconvert: unitconvert.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

DHDN_ETRS89: DHDN_ETRS89.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

geotiff_grids: geotiff_grids.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

adams_hemi: adams_hemi.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

adams_ws1: adams_ws1.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

adams_ws2: adams_ws2.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

guyou: guyou.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

peirce_q: peirce_q.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

defmodel: defmodel.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

tinshift: tinshift.gie
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

check-local: 4D-API-cs2cs-style GDA axisswap builtins deformation ellipsoid more_builtins unitconvert DHDN_ETRS89 geotiff_grids adams_hemi adams_ws1 adams_ws2 guyou peirce_q defmodel tinshift

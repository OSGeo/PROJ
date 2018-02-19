EXEPATH = ../../src
GIEEXE = $(EXEPATH)/gie

EXTRA_DIST = \
	5101.1-jhs.gie \
	5101.2-jhs.gie \
	5101.3-jhs.gie \
	5101.4-jhs-etmerc.gie \
	5102.1.gie \
	5103.1.gie \
	5103.2.gie \
	5103.3.gie \
	5105.2.gie \
	5106.gie \
	5107.gie \
	5109.gie \
	5111.1.gie \
	5112.gie \
	5113.gie \
	5201.gie \
	5208.gie

PROJ_LIB ?= $(top_srcdir)/nad

5101.1: 5101.1-jhs.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5101.2: 5101.2-jhs.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5101.3: 5101.3-jhs.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5101.4: 5101.4-jhs-etmerc.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5102.1: 5102.1.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5103.1: 5103.1.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5103.2: 5103.2.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5103.3: 5103.3.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5105.2: 5105.2.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5106: 5106.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5107: 5107.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5109: 5109.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5111.1: 5111.1.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5112: 5112.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5113: 5113.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5201: 5201.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

5208: 5208.gie
	PROJ_LIB=$(PROJ_LIB) $(GIEEXE) $<

check-local: 5101.1 5101.2 5101.3 5101.4 5102.1 5103.1 5103.2 5103.3 5105.2 5106 5107 5109 5111.1 5112 5113 5201 5208

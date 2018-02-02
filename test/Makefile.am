EXEPATH = ../src
GIEEXE = $(EXEPATH)/gie
GIETESTS = gie
GIGSTESTS = gigs

SUBDIRS = gie gigs

EXTRA_DIST = CMakeLists.txt

check-local:
	$(GIEEXE) $(GIETESTS)/*.gie
	$(GIEEXE) $(GIGSTESTS)/*.gie

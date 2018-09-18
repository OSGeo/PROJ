# Executables paths passed to test scripts
DATAPATH = $(top_srcdir)/data
THIS_DIR = $(top_srcdir)/test/old
EXEPATH = ../../src
PROJEXE = $(EXEPATH)/proj
CS2CSEXE = $(EXEPATH)/cs2cs

# PROJ.4 test scripts
TEST27 = $(THIS_DIR)/test27
TEST83 = $(THIS_DIR)/test83
TESTNTV2 = $(THIS_DIR)/testntv2
TESTVARIOUS = $(THIS_DIR)/testvarious
TESTFLAKY = $(THIS_DIR)/testflaky
TESTDATUMFILE = $(THIS_DIR)/testdatumfile
TESTIGN = $(THIS_DIR)/testIGNF

EXTRA_DIST = pj_out27.dist pj_out83.dist td_out.dist \
		test27 test83 tv_out.dist tf_out.dist \
		testflaky testvarious testdatumfile testntv2 ntv2_out.dist \
		testIGNF proj_outIGNF.dist \
		CMakeLists.txt

check-local:
	$(TEST27) $(PROJEXE)
	$(TEST83) $(PROJEXE)
	PROJ_LIB=$(DATAPATH) $(TESTVARIOUS) $(CS2CSEXE)
	@if [ -f $(DATAPATH)/conus ] ; then \
	  PROJ_LIB=$(DATAPATH) $(TESTDATUMFILE) $(CS2CSEXE) ; \
	fi
	@if [ -f $(DATAPATH)/ntf_r93.gsb ] ; then \
	  PROJ_LIB=$(DATAPATH) $(TESTIGN) $(CS2CSEXE) ; \
	fi
	@if [ -f $(DATAPATH)/ntv2_0.gsb ] ; then \
	  PROJ_LIB=$(DATAPATH) $(TESTNTV2) $(CS2CSEXE) ; \
	fi

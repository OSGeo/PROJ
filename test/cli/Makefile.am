# Executables paths passed to test scripts
PROJ_LIB = ../../data/for_tests
THIS_DIR = $(top_srcdir)/test/cli
EXEPATH = ../../src
PROJEXE = $(EXEPATH)/proj
CS2CSEXE = $(EXEPATH)/cs2cs
PROJINFOEXE = $(EXEPATH)/projinfo
CCTEXE = $(EXEPATH)/cct
PROJSYNC_EXE = $(EXEPATH)/projsync

# PROJ.4 test scripts
TEST27 = $(THIS_DIR)/test27
TEST83 = $(THIS_DIR)/test83
TESTPROJ = $(THIS_DIR)/testproj
TESTNTV2 = $(THIS_DIR)/testntv2
TESTVARIOUS = $(THIS_DIR)/testvarious
TESTFLAKY = $(THIS_DIR)/testflaky
TESTDATUMFILE = $(THIS_DIR)/testdatumfile
TESTIGN = $(THIS_DIR)/testIGNF
TESTPROJINFO = $(THIS_DIR)/testprojinfo
TESTCCT = $(THIS_DIR)/testcct
TESTPROJSYNC = $(THIS_DIR)/test_projsync

EXTRA_DIST = pj_out27.dist pj_out83.dist td_out.dist \
		test27 test83 tv_out.dist tf_out.dist \
		testflaky testvarious testdatumfile testntv2 ntv2_out.dist \
		testIGNF proj_outIGNF.dist \
		testprojinfo testprojinfo_out.dist \
		testcct testcct_out.dist \
		testproj testproj_out.dist \
		test_projsync \
		CMakeLists.txt

testprojinfo-check:
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(TESTPROJINFO) $(PROJINFOEXE)

test27-check:
	$(TEST27) $(PROJEXE)

test83-check:
	$(TEST83) $(PROJEXE)

testproj-check:
	$(TESTPROJ) $(PROJEXE)

testvarious-check:
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(TESTVARIOUS) $(CS2CSEXE)

testdatumfile-check:
	@if [ -f $(PROJ_LIB)/conus -a -f $(PROJ_LIB)/ntv1_can.dat -a -f $(PROJ_LIB)/MD -a -f $(PROJ_LIB)/ntf_r93.gsb -a -f $(PROJ_LIB)/egm96_15.gtx ]; then \
	  PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(TESTDATUMFILE) $(CS2CSEXE) ; \
	fi

testign-check:
	@if [ -f $(PROJ_LIB)/ntf_r93.gsb ] ; then \
	  PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(TESTIGN) $(CS2CSEXE) ; \
	fi

testntv2-check:
	@if [ -f $(PROJ_LIB)/ntv2_0.gsb -a -f $(PROJ_LIB)/conus -a -f $(PROJ_LIB)/ntv1_can.dat ] ; then \
	  PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(TESTNTV2) $(CS2CSEXE) ; \
	fi

testcct-check:
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) $(TESTCCT) $(CCTEXE)


if HAVE_CURL
testprojsync-check:
	PROJ_LIB=$(PROJ_LIB) $(TESTPROJSYNC) $(PROJSYNC_EXE)
else
testprojsync-check:
	echo "Skipping testprojsync-check"
endif

check-local: testprojinfo-check test27-check test83-check testproj-check testvarious-check testdatumfile-check testign-check testntv2-check testcct-check testprojsync-check

AUTOMAKE_OPTIONS = subdir-objects

EXTRA_DIST = CMakeLists.txt

noinst_HEADERS = gtest_include.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/test -I$(top_srcdir)/test/googletest/include
AM_CXXFLAGS = @CXX_WFLAGS@ @NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG@

noinst_PROGRAMS = basic_test
noinst_PROGRAMS += pj_phi2_test
noinst_PROGRAMS += proj_errno_string_test

basic_test_SOURCES = basic_test.cpp main.cpp
basic_test_LDADD = ../../src/libproj.la ../../test/googletest/libgtest.la

basic_test-check: basic_test
	./basic_test

pj_phi2_test_SOURCES = pj_phi2_test.cpp main.cpp
pj_phi2_test_LDADD = ../../src/libproj.la ../../test/googletest/libgtest.la

pj_phi2_test-check: pj_phi2_test
	./pj_phi2_test

proj_errno_string_test_SOURCES = proj_errno_string_test.cpp main.cpp
proj_errno_string_test_LDADD= ../../src/libproj.la ../../test/googletest/libgtest.la

proj_errno_string_test-check: proj_errno_string_test
	./proj_errno_string_test

check-local: basic_test-check
check-local: pj_phi2_test-check proj_errno_string_test-check

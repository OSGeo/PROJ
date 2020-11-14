AUTOMAKE_OPTIONS = subdir-objects

EXTRA_DIST = CMakeLists.txt

noinst_HEADERS = gtest_include.h test_primitives.hpp

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include -I$(top_srcdir)/test @GTEST_CFLAGS@ @SQLITE3_CFLAGS@
AM_CXXFLAGS = @CXX_WFLAGS@ @NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG@

PROJ_LIB ?= ../../data/for_tests

noinst_PROGRAMS = pj_phi2_test
noinst_PROGRAMS += proj_errno_string_test
noinst_PROGRAMS += proj_angular_io_test
noinst_PROGRAMS += proj_context_test
noinst_PROGRAMS += test_cpp_api
noinst_PROGRAMS += gie_self_tests
noinst_PROGRAMS += include_proj_h_from_c
noinst_PROGRAMS += test_network
noinst_PROGRAMS += test_defmodel
noinst_PROGRAMS += test_tinshift


pj_phi2_test_SOURCES = pj_phi2_test.cpp main.cpp
pj_phi2_test_LDADD = ../../src/libproj.la @GTEST_LIBS@

pj_phi2_test-check: pj_phi2_test
	./pj_phi2_test

proj_errno_string_test_SOURCES = proj_errno_string_test.cpp main.cpp
proj_errno_string_test_LDADD= ../../src/libproj.la @GTEST_LIBS@

proj_errno_string_test-check: proj_errno_string_test
	./proj_errno_string_test

proj_angular_io_test_SOURCES = proj_angular_io_test.cpp main.cpp
proj_angular_io_test_LDADD = ../../src/libproj.la @GTEST_LIBS@

proj_angular_io_test-check: proj_angular_io_test
	./proj_angular_io_test

proj_context_test_SOURCES = proj_context_test.cpp main.cpp
proj_context_test_LDADD = ../../src/libproj.la @GTEST_LIBS@

proj_context_test-check: proj_context_test
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES ./proj_context_test

test_cpp_api_SOURCES = test_util.cpp test_common.cpp test_crs.cpp test_metadata.cpp test_io.cpp test_operation.cpp test_datum.cpp test_factory.cpp test_c_api.cpp test_grids.cpp main.cpp
test_cpp_api_LDADD = ../../src/libproj.la @GTEST_LIBS@ @SQLITE3_LIBS@

test_cpp_api-check: test_cpp_api
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) ./test_cpp_api

gie_self_tests_SOURCES = gie_self_tests.cpp main.cpp
gie_self_tests_LDADD = ../../src/libproj.la @GTEST_LIBS@ @SQLITE3_LIBS@

gie_self_tests-check: gie_self_tests
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) ./gie_self_tests

include_proj_h_from_c_SOURCES = include_proj_h_from_c.c

test_network_SOURCES = test_network.cpp main.cpp
test_network_CXXFLAGS = @CURL_CFLAGS@ @CURL_ENABLED_FLAGS@
test_network_LDADD = ../../src/libproj.la @GTEST_LIBS@ @SQLITE3_LIBS@ @CURL_LIBS@

test_network-check: test_network
	PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES PROJ_LIB=$(PROJ_LIB) PROJ_SOURCE_DATA=$(PROJ_LIB) ./test_network

test_defmodel_SOURCES = test_defmodel.cpp main.cpp
test_defmodel_LDADD = ../../src/libproj.la @GTEST_LIBS@

test_defmodel-check: test_defmodel
	PROJ_LIB=$(PROJ_LIB) ./test_defmodel

test_tinshift_SOURCES = test_tinshift.cpp main.cpp
test_tinshift_LDADD = ../../src/libproj.la @GTEST_LIBS@

test_tinshift-check: test_tinshift
	PROJ_LIB=$(PROJ_LIB) ./test_tinshift

check-local: pj_phi2_test-check proj_errno_string_test-check \
		proj_angular_io_test-check proj_context_test-check test_cpp_api-check \
		gie_self_tests-check test_network-check test_defmodel-check test_tinshift-check

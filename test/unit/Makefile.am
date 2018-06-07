AUTOMAKE_OPTIONS = subdir-objects

EXTRA_DIST = CMakeLists.txt

include_HEADERS = gtest_include.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/test -I$(top_srcdir)/test/googletest/include
AM_CXXFLAGS = @CXX_WFLAGS@ @NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG@

bin_PROGRAMS =	basic_test

basic_test_SOURCES = basic_test.cpp main.cpp
basic_test_LDADD = ../../src/libproj.la ../../test/googletest/libgtest.la

basic_test-check: basic_test
	./basic_test

check-local: basic_test-check

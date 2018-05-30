EXTRA_DIST = CMakeLists.txt

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/test

bin_PROGRAMS =	basic_test

basic_test_SOURCES = basic_test.cpp
basic_test_LDADD = ../../src/libproj.la ../../test/libtestmain.la

basic_test-check: basic_test
	./basic_test

check-local: basic_test-check

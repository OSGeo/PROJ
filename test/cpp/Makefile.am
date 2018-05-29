EXTRA_DIST = CMakeLists.txt

AM_CPPFLAGS = -I$(top_srcdir)/src

include_HEADERS = catch.hpp

bin_PROGRAMS =	basic_test

lib_LTLIBRARIES = libtestmain.la

libtestmain_la_LDFLAGS = -no-undefined
libtestmain_la_SOURCES = test_main.cpp

basic_test_SOURCES = basic_test.cpp
basic_test_LDADD = ../../src/libproj.la libtestmain.la

basic_test-check: basic_test
	./basic_test

check-local: basic_test-check

AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = src include

AM_CPPFLAGS = -I$(top_srcdir)/test/googletest/include

noinst_LTLIBRARIES = libgtest.la

libgtest_la_LDFLAGS = -no-undefined
libgtest_la_SOURCES = src/gtest-all.cc

EXTRA_DIST = CMakeLists.txt.in

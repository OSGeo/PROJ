EXTRA_DIST = CMakeLists.txt

# include_HEADERS = catch.hpp

# lib_LTLIBRARIES = libtestmain.la

# libtestmain_la_LDFLAGS = -no-undefined
# libtestmain_la_SOURCES = test_main.cpp

if !USE_EXTERNAL_GTEST
MAYBE_GTEST = googletest
endif
SUBDIRS = cli $(MAYBE_GTEST) . gie gigs unit

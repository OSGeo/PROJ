SUBDIRS	=	include src man data cmake
DIST_SUBDIRS = include src man data cmake test

EXTRA_DIST = CMakeLists.txt CITATION README.md

dist_doc_DATA = COPYING NEWS AUTHORS

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = proj.pc

AUTOMAKE_OPTIONS =	dist-zip
ACLOCAL_AMFLAGS =	-I m4

check-local:
	cd test; $(MAKE) check

all-local: README

README: README.md
	fgrep -v "[![" $< > $@

clean-local:
	$(RM) README

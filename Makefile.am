SUBDIRS	=	include src man data jniwrap cmake
DIST_SUBDIRS = include src man data jniwrap cmake test

EXTRA_DIST = CMakeLists.txt CITATION README.md

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = proj.pc

AUTOMAKE_OPTIONS =	dist-zip
ACLOCAL_AMFLAGS =	-I m4

check-local:
	cd test; $(MAKE) check


README: README.md
	fgrep -v "[![" $< > $@

clean-local:
	$(RM) README

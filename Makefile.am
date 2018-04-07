SUBDIRS	=	src man nad jniwrap cmake test

EXTRA_DIST = makefile.vc nmake.opt CMakeLists.txt CITATION

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = proj.pc

AUTOMAKE_OPTIONS =	dist-zip
ACLOCAL_AMFLAGS =	-I m4


README: README.md
	fgrep -v "[![" $< > $@

clean-local:
	$(RM) README

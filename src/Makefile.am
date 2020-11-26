AM_CFLAGS = @C_WFLAGS@

EXTRA_PROGRAMS = multistresstest test228

TESTS = geodtest
check_PROGRAMS = geodtest

AM_CPPFLAGS =	-DPROJ_LIB=\"$(pkgdatadir)\" \
		-DMUTEX_@MUTEX_SETTING@ -I$(top_srcdir)/include @SQLITE3_CFLAGS@ @TIFF_CFLAGS@ @TIFF_ENABLED_FLAGS@ @CURL_CFLAGS@ @CURL_ENABLED_FLAGS@
AM_CXXFLAGS =    @CXX_WFLAGS@ @FLTO_FLAG@

include_HEADERS = proj.h proj_experimental.h proj_constants.h geodesic.h \
	 proj_symbol_rename.h

EXTRA_DIST = bin_cct.cmake bin_gie.cmake bin_cs2cs.cmake \
	bin_geod.cmake bin_proj.cmake bin_projinfo.cmake \
	lib_proj.cmake CMakeLists.txt bin_geodtest.cmake \
	bin_projsync.cmake \
	tests/geodtest.cpp \
	wkt1_grammar.y wkt2_grammar.y apps/emess.h apps/utils.h \
	apps/projsync.cpp

proj_SOURCES = apps/proj.cpp apps/emess.cpp apps/utils.cpp
projinfo_SOURCES = apps/projinfo.cpp
cs2cs_SOURCES = apps/cs2cs.cpp apps/emess.cpp apps/utils.cpp
cct_SOURCES = apps/cct.cpp apps/proj_strtod.cpp apps/proj_strtod.h apps/optargpm.h
geod_SOURCES = apps/geod.cpp apps/geod_set.cpp apps/geod_interface.cpp apps/geod_interface.h apps/emess.cpp

if HAVE_CURL
projsync_SOURCES = apps/projsync.cpp
projsync_LDADD = libproj.la
PROJSYNC_BIN = projsync
endif

bin_PROGRAMS =	proj geod cs2cs gie cct projinfo $(PROJSYNC_BIN)

gie_SOURCES = apps/gie.cpp apps/proj_strtod.cpp apps/proj_strtod.h apps/optargpm.h
multistresstest_SOURCES = tests/multistresstest.cpp
test228_SOURCES = tests/test228.cpp
geodtest_SOURCES = tests/geodtest.cpp

cct_LDADD = libproj.la
cs2cs_LDADD = libproj.la
geod_LDADD = libproj.la
proj_LDADD = libproj.la
projinfo_LDADD = libproj.la

gie_LDADD = libproj.la
multistresstest_LDADD = libproj.la @THREAD_LIB@
test228_LDADD = libproj.la @THREAD_LIB@
geodtest_LDADD = libproj.la

lib_LTLIBRARIES = libproj.la

libproj_la_LDFLAGS = -no-undefined -version-info 21:0:2
libproj_la_LIBADD = @SQLITE3_LIBS@ @TIFF_LIBS@ @CURL_LIBS@

libproj_la_SOURCES = \
	pj_list.h proj_internal.h \
	\
	iso19111/static.cpp \
	iso19111/util.cpp \
	iso19111/metadata.cpp \
	iso19111/common.cpp \
	iso19111/crs.cpp \
	iso19111/datum.cpp \
	iso19111/coordinatesystem.cpp \
	iso19111/coordinateoperation.cpp \
	iso19111/io.cpp \
	iso19111/internal.cpp \
	iso19111/factory.cpp \
	iso19111/c_api.cpp \
	\
	projections/aeqd.cpp \
	projections/adams.cpp \
	projections/gnom.cpp \
	projections/laea.cpp \
	projections/mod_ster.cpp \
	projections/nsper.cpp \
	projections/nzmg.cpp \
	projections/ortho.cpp \
	projections/stere.cpp \
	projections/sterea.cpp \
	projections/aea.cpp \
	projections/bipc.cpp \
	projections/bonne.cpp \
	projections/eqdc.cpp \
	projections/isea.cpp \
	projections/ccon.cpp \
	projections/imw_p.cpp \
	projections/krovak.cpp \
	projections/lcc.cpp \
	projections/poly.cpp \
	projections/rpoly.cpp \
	projections/sconics.cpp \
	projections/rouss.cpp \
	projections/cass.cpp \
	projections/cc.cpp \
	projections/cea.cpp \
	projections/eqc.cpp \
	projections/gall.cpp \
	projections/labrd.cpp \
	projections/lsat.cpp \
	projections/misrsom.cpp \
	projections/merc.cpp \
	projections/mill.cpp \
	projections/ocea.cpp \
	projections/omerc.cpp \
	projections/somerc.cpp \
	projections/tcc.cpp \
	projections/tcea.cpp \
	projections/times.cpp \
	projections/tmerc.cpp \
	projections/tobmerc.cpp \
	projections/airy.cpp \
	projections/aitoff.cpp \
	projections/august.cpp \
	projections/bacon.cpp \
	projections/bertin1953.cpp \
	projections/chamb.cpp \
	projections/hammer.cpp \
	projections/lagrng.cpp \
	projections/larr.cpp \
	projections/lask.cpp \
	projections/latlong.cpp \
	projections/nicol.cpp \
	projections/ob_tran.cpp \
	projections/oea.cpp \
	projections/tpeqd.cpp \
	projections/vandg.cpp \
	projections/vandg2.cpp \
	projections/vandg4.cpp \
	projections/wag7.cpp \
	projections/lcca.cpp \
	projections/geos.cpp \
	projections/boggs.cpp \
	projections/collg.cpp \
	projections/comill.cpp \
	projections/crast.cpp \
	projections/denoy.cpp \
	projections/eck1.cpp \
	projections/eck2.cpp \
	projections/eck3.cpp \
	projections/eck4.cpp \
	projections/eck5.cpp \
	projections/fahey.cpp \
	projections/fouc_s.cpp \
	projections/gins8.cpp \
	projections/gstmerc.cpp \
	projections/gn_sinu.cpp \
	projections/goode.cpp \
	projections/igh.cpp \
	projections/igh_o.cpp \
	projections/hatano.cpp \
	projections/loxim.cpp \
	projections/mbt_fps.cpp \
	projections/mbtfpp.cpp \
	projections/mbtfpq.cpp \
	projections/moll.cpp \
	projections/nell.cpp \
	projections/nell_h.cpp \
	projections/patterson.cpp \
	projections/putp2.cpp \
	projections/putp3.cpp \
	projections/putp4p.cpp \
	projections/putp5.cpp \
	projections/putp6.cpp \
	projections/qsc.cpp \
	projections/robin.cpp \
	projections/sch.cpp \
	projections/sts.cpp \
	projections/urm5.cpp \
	projections/urmfps.cpp \
	projections/wag2.cpp \
	projections/wag3.cpp \
	projections/wink1.cpp \
	projections/wink2.cpp \
	projections/healpix.cpp \
	projections/natearth.cpp \
	projections/natearth2.cpp \
	projections/calcofi.cpp \
	projections/eqearth.cpp \
	projections/col_urban.cpp \
	\
	conversions/axisswap.cpp \
	conversions/cart.cpp \
	conversions/geoc.cpp \
	conversions/geocent.cpp \
	conversions/noop.cpp \
	conversions/set.cpp \
	conversions/unitconvert.cpp \
	\
	transformations/affine.cpp \
	transformations/deformation.cpp \
	transformations/helmert.cpp \
	transformations/hgridshift.cpp \
	transformations/horner.cpp \
	transformations/molodensky.cpp \
	transformations/vgridshift.cpp \
	transformations/xyzgridshift.cpp \
	transformations/defmodel.cpp \
	transformations/defmodel.hpp \
	transformations/defmodel_exceptions.hpp \
	transformations/defmodel_impl.hpp \
	transformations/tinshift.cpp \
	transformations/tinshift.hpp \
	transformations/tinshift_exceptions.hpp \
	transformations/tinshift_impl.hpp \
	\
	aasincos.cpp adjlon.cpp \
	dmstor.cpp auth.cpp \
	deriv.cpp ell_set.cpp ellps.cpp \
	factors.cpp fwd.cpp init.cpp inv.cpp \
	list.cpp malloc.cpp mlfn.cpp mlfn.hpp msfn.cpp proj_mdist.cpp \
	param.cpp phi2.cpp pr_list.cpp \
	qsfn.cpp strerrno.cpp \
	tsfn.cpp units.cpp ctx.cpp log.cpp zpoly1.cpp rtodms.cpp \
	release.cpp gauss.cpp \
	generic_inverse.cpp \
	quadtree.hpp \
	\
	datums.cpp datum_set.cpp \
	mutex.cpp initcache.cpp geodesic.c \
	strtod.cpp \
	\
	4D_api.cpp pipeline.cpp \
	internal.cpp \
	wkt_parser.hpp wkt_parser.cpp \
	wkt1_parser.h wkt1_parser.cpp \
	wkt1_generated_parser.h wkt1_generated_parser.c \
	wkt2_parser.h wkt2_parser.cpp \
	wkt2_generated_parser.h wkt2_generated_parser.c \
	\
	proj_json_streaming_writer.hpp \
	proj_json_streaming_writer.cpp \
	\
	tracing.cpp \
	\
	grids.hpp \
	grids.cpp \
	filemanager.hpp \
	filemanager.cpp \
	networkfilemanager.cpp \
	sqlite3_utils.hpp \
	sqlite3_utils.cpp


# The sed hack is to please MSVC
wkt1_parser:
	bison --no-lines -d  -p pj_wkt1_ -o$(top_srcdir)/src/wkt1_generated_parser.c $(top_srcdir)/src/wkt1_grammar.y
	sed "s/\*yyssp = yystate/\*yyssp = (yytype_int16)yystate/" < $(top_srcdir)/src/wkt1_generated_parser.c | sed "s/yyerrorlab:/#if 0\nyyerrorlab:/" | sed "s/yyerrlab1:/#endif\nyyerrlab1:/" | sed "s/for (yylen = 0; yystr\[yylen\]; yylen++)/for (yylen = 0; yystr \&\& yystr\[yylen\]; yylen++)/"  > $(top_srcdir)/src/wkt1_generated_parser.c.tmp
	mv $(top_srcdir)/src/wkt1_generated_parser.c.tmp $(top_srcdir)/src/wkt1_generated_parser.c

wkt2_parser:
	bison --no-lines -d  -p pj_wkt2_ -o$(top_srcdir)/src/wkt2_generated_parser.c $(top_srcdir)/src/wkt2_grammar.y
	sed "s/\*yyssp = yystate/\*yyssp = (yytype_int16)yystate/" < $(top_srcdir)/src/wkt2_generated_parser.c | sed "s/yyerrorlab:/#if 0\nyyerrorlab:/" | sed "s/yyerrlab1:/#endif\nyyerrlab1:/" | sed "s/for (yylen = 0; yystr\[yylen\]; yylen++)/for (yylen = 0; yystr \&\& yystr\[yylen\]; yylen++)/"  > $(top_srcdir)/src/wkt2_generated_parser.c.tmp
	mv $(top_srcdir)/src/wkt2_generated_parser.c.tmp $(top_srcdir)/src/wkt2_generated_parser.c


install-exec-local: install-binPROGRAMS
	rm -f $(DESTDIR)$(bindir)/invproj$(EXEEXT)
	(cd $(DESTDIR)$(bindir); ln -s proj$(EXEEXT) invproj$(EXEEXT))
	rm -f $(DESTDIR)$(bindir)/invgeod$(EXEEXT)
	(cd $(DESTDIR)$(bindir); ln -s geod$(EXEEXT) invgeod$(EXEEXT))

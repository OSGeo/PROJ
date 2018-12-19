AM_CFLAGS = @C_WFLAGS@

bin_PROGRAMS =	proj nad2bin geod cs2cs gie cct projinfo
EXTRA_PROGRAMS = multistresstest test228

TESTS = geodtest
check_PROGRAMS = geodtest

AM_CPPFLAGS =	-DPROJ_LIB=\"$(pkgdatadir)\" \
		-DMUTEX_@MUTEX_SETTING@ @JNI_INCLUDE@ -I$(top_srcdir)/include @SQLITE3_FLAGS@
AM_CXXFLAGS =    @CXX_WFLAGS@ @FLTO_FLAG@ -DPROJ_COMPILATION

include_HEADERS = proj.h proj_experimental.h proj_constants.h proj_api.h geodesic.h \
	org_proj4_PJ.h proj_symbol_rename.h

EXTRA_DIST = bin_cct.cmake bin_gie.cmake bin_cs2cs.cmake \
	bin_geod.cmake bin_nad2bin.cmake bin_proj.cmake bin_projinfo.cmake \
	lib_proj.cmake CMakeLists.txt bin_geodtest.cmake tests/geodtest.cpp \
	pj_wkt1_grammar.y pj_wkt2_grammar.y apps/emess.h

proj_SOURCES = apps/proj.cpp apps/gen_cheb.cpp apps/p_series.cpp apps/emess.cpp
projinfo_SOURCES = apps/projinfo.cpp
cs2cs_SOURCES = apps/cs2cs.cpp apps/gen_cheb.cpp apps/p_series.cpp apps/emess.cpp
cct_SOURCES = apps/cct.cpp apps/proj_strtod.cpp apps/proj_strtod.h apps/optargpm.h
nad2bin_SOURCES = apps/nad2bin.cpp
geod_SOURCES = apps/geod.cpp apps/geod_set.cpp apps/geod_interface.cpp apps/geod_interface.h apps/emess.cpp

gie_SOURCES = apps/gie.cpp apps/proj_strtod.cpp apps/proj_strtod.h apps/optargpm.h
multistresstest_SOURCES = tests/multistresstest.cpp
test228_SOURCES = tests/test228.cpp
geodtest_SOURCES = tests/geodtest.cpp

cct_LDADD = libproj.la
cs2cs_LDADD = libproj.la
geod_LDADD = libproj.la
proj_LDADD = libproj.la
projinfo_LDADD = libproj.la
nad2bin_LDADD = libproj.la

gie_LDADD = libproj.la
multistresstest_LDADD = libproj.la @THREAD_LIB@
test228_LDADD = libproj.la @THREAD_LIB@
geodtest_LDADD = libproj.la

lib_LTLIBRARIES = libproj.la

libproj_la_LDFLAGS = -no-undefined -version-info 14:1:1
libproj_la_LIBADD = @SQLITE3_LDFLAGS@

libproj_la_SOURCES = \
	pj_list.h proj_internal.h proj_math.h projects.h \
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
	projections/PJ_aeqd.cpp \
	projections/PJ_gnom.cpp \
	projections/PJ_laea.cpp \
	projections/PJ_mod_ster.cpp \
	projections/PJ_nsper.cpp \
	projections/PJ_nzmg.cpp \
	projections/PJ_ortho.cpp \
	projections/PJ_stere.cpp \
	projections/PJ_sterea.cpp \
	projections/PJ_aea.cpp \
	projections/PJ_bipc.cpp \
	projections/PJ_bonne.cpp \
	projections/PJ_eqdc.cpp \
	projections/PJ_isea.cpp \
	projections/PJ_ccon.cpp \
	projections/PJ_imw_p.cpp \
	projections/PJ_krovak.cpp \
	projections/PJ_lcc.cpp \
	projections/PJ_poly.cpp \
	projections/PJ_rpoly.cpp \
	projections/PJ_sconics.cpp \
	projections/proj_rouss.cpp \
	projections/PJ_cass.cpp \
	projections/PJ_cc.cpp \
	projections/PJ_cea.cpp \
	projections/PJ_eqc.cpp \
	projections/PJ_gall.cpp \
	projections/PJ_labrd.cpp \
	projections/PJ_lsat.cpp \
	projections/PJ_misrsom.cpp \
	projections/PJ_merc.cpp \
	projections/PJ_mill.cpp \
	projections/PJ_ocea.cpp \
	projections/PJ_omerc.cpp \
	projections/PJ_somerc.cpp \
	projections/PJ_tcc.cpp \
	projections/PJ_tcea.cpp \
	projections/PJ_times.cpp \
	projections/PJ_tmerc.cpp \
	projections/PJ_tobmerc.cpp \
	projections/PJ_airy.cpp \
	projections/PJ_aitoff.cpp \
	projections/PJ_august.cpp \
	projections/PJ_bacon.cpp \
	projections/PJ_bertin1953.cpp \
	projections/PJ_chamb.cpp \
	projections/PJ_hammer.cpp \
	projections/PJ_lagrng.cpp \
	projections/PJ_larr.cpp \
	projections/PJ_lask.cpp \
	projections/PJ_latlong.cpp \
	projections/PJ_nocol.cpp \
	projections/PJ_ob_tran.cpp \
	projections/PJ_oea.cpp \
	projections/PJ_tpeqd.cpp \
	projections/PJ_vandg.cpp \
	projections/PJ_vandg2.cpp \
	projections/PJ_vandg4.cpp \
	projections/PJ_wag7.cpp \
	projections/PJ_lcca.cpp \
	projections/PJ_geos.cpp \
	projections/proj_etmerc.cpp \
	projections/PJ_boggs.cpp \
	projections/PJ_collg.cpp \
	projections/PJ_comill.cpp \
	projections/PJ_crast.cpp \
	projections/PJ_denoy.cpp \
	projections/PJ_eck1.cpp \
	projections/PJ_eck2.cpp \
	projections/PJ_eck3.cpp \
	projections/PJ_eck4.cpp \
	projections/PJ_eck5.cpp \
	projections/PJ_fahey.cpp \
	projections/PJ_fouc_s.cpp \
	projections/PJ_gins8.cpp \
	projections/PJ_gstmerc.cpp \
	projections/PJ_gn_sinu.cpp \
	projections/PJ_goode.cpp \
	projections/PJ_igh.cpp \
	projections/PJ_hatano.cpp \
	projections/PJ_loxim.cpp \
	projections/PJ_mbt_fps.cpp \
	projections/PJ_mbtfpp.cpp \
	projections/PJ_mbtfpq.cpp \
	projections/PJ_moll.cpp \
	projections/PJ_nell.cpp \
	projections/PJ_nell_h.cpp \
	projections/PJ_patterson.cpp \
	projections/PJ_putp2.cpp \
	projections/PJ_putp3.cpp \
	projections/PJ_putp4p.cpp \
	projections/PJ_putp5.cpp \
	projections/PJ_putp6.cpp \
	projections/PJ_qsc.cpp \
	projections/PJ_robin.cpp \
	projections/PJ_sch.cpp \
	projections/PJ_sts.cpp \
	projections/PJ_urm5.cpp \
	projections/PJ_urmfps.cpp \
	projections/PJ_wag2.cpp \
	projections/PJ_wag3.cpp \
	projections/PJ_wink1.cpp \
	projections/PJ_wink2.cpp \
	projections/PJ_healpix.cpp \
	projections/PJ_natearth.cpp \
	projections/PJ_natearth2.cpp \
	projections/PJ_calcofi.cpp \
	projections/PJ_eqearth.cpp \
	\
	conversions/PJ_axisswap.cpp \
	conversions/PJ_cart.cpp \
	conversions/PJ_geoc.cpp \
	conversions/pj_geocent.cpp \
	conversions/PJ_unitconvert.cpp \
	\
	transformations/PJ_affine.cpp \
	transformations/PJ_deformation.cpp \
	transformations/PJ_helmert.cpp \
	transformations/PJ_hgridshift.cpp \
	transformations/PJ_horner.cpp \
	transformations/PJ_molodensky.cpp \
	transformations/PJ_vgridshift.cpp \
	\
	aasincos.cpp adjlon.cpp bch2bps.cpp bchgen.cpp \
	biveval.cpp dmstor.cpp mk_cheby.cpp pj_auth.cpp \
	pj_deriv.cpp pj_ell_set.cpp pj_ellps.cpp pj_errno.cpp \
	pj_factors.cpp pj_fwd.cpp pj_init.cpp pj_inv.cpp \
	pj_list.cpp pj_malloc.cpp pj_mlfn.cpp pj_msfn.cpp proj_mdist.cpp \
	pj_open_lib.cpp pj_param.cpp pj_phi2.cpp pj_pr_list.cpp \
	pj_qsfn.cpp pj_strerrno.cpp \
	pj_tsfn.cpp pj_units.cpp pj_ctx.cpp pj_log.cpp pj_zpoly1.cpp rtodms.cpp \
	vector1.cpp pj_release.cpp pj_gauss.cpp \
	pj_fileapi.cpp \
	\
	pj_gc_reader.cpp pj_gridcatalog.cpp \
	nad_cvt.cpp nad_init.cpp nad_intr.cpp \
	pj_apply_gridshift.cpp pj_datums.cpp pj_datum_set.cpp pj_transform.cpp \
	geocent.cpp geocent.h pj_utils.cpp pj_gridinfo.cpp pj_gridlist.cpp \
	jniproj.cpp pj_mutex.cpp pj_initcache.cpp pj_apply_vgridshift.cpp geodesic.cpp \
	pj_strtod.cpp pj_math.cpp \
	\
	proj_4D_api.cpp PJ_pipeline.cpp \
	pj_internal.cpp \
	pj_wkt_parser.hpp pj_wkt_parser.cpp \
	pj_wkt1_parser.h pj_wkt1_parser.cpp \
	pj_wkt1_generated_parser.h pj_wkt1_generated_parser.c \
	pj_wkt2_parser.h pj_wkt2_parser.cpp \
	pj_wkt2_generated_parser.h pj_wkt2_generated_parser.c


# The sed hack is to please MSVC
wkt1_parser:
	bison --no-lines -d  -p pj_wkt1_ -o$(top_srcdir)/src/pj_wkt1_generated_parser.c $(top_srcdir)/src/pj_wkt1_grammar.y
	sed "s/\*yyssp = yystate/\*yyssp = (yytype_int16)yystate/" < $(top_srcdir)/src/pj_wkt1_generated_parser.c | sed "s/yyerrorlab:/#if 0\nyyerrorlab:/" | sed "s/yyerrlab1:/#endif\nyyerrlab1:/" | sed "s/for (yylen = 0; yystr\[yylen\]; yylen++)/for (yylen = 0; yystr \&\& yystr\[yylen\]; yylen++)/"  > $(top_srcdir)/src/pj_wkt1_generated_parser.c.tmp
	mv $(top_srcdir)/src/pj_wkt1_generated_parser.c.tmp $(top_srcdir)/src/pj_wkt1_generated_parser.c

wkt2_parser:
	bison --no-lines -d  -p pj_wkt2_ -o$(top_srcdir)/src/pj_wkt2_generated_parser.c $(top_srcdir)/src/pj_wkt2_grammar.y
	sed "s/\*yyssp = yystate/\*yyssp = (yytype_int16)yystate/" < $(top_srcdir)/src/pj_wkt2_generated_parser.c | sed "s/yyerrorlab:/#if 0\nyyerrorlab:/" | sed "s/yyerrlab1:/#endif\nyyerrlab1:/" | sed "s/for (yylen = 0; yystr\[yylen\]; yylen++)/for (yylen = 0; yystr \&\& yystr\[yylen\]; yylen++)/"  > $(top_srcdir)/src/pj_wkt2_generated_parser.c.tmp
	mv $(top_srcdir)/src/pj_wkt2_generated_parser.c.tmp $(top_srcdir)/src/pj_wkt2_generated_parser.c


install-exec-local: install-binPROGRAMS
	rm -f $(DESTDIR)$(bindir)/invproj$(EXEEXT)
	(cd $(DESTDIR)$(bindir); ln -s proj$(EXEEXT) invproj$(EXEEXT))
	rm -f $(DESTDIR)$(bindir)/invgeod$(EXEEXT)
	(cd $(DESTDIR)$(bindir); ln -s geod$(EXEEXT) invgeod$(EXEEXT))

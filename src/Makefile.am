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
	lib_proj.cmake CMakeLists.txt bin_geodtest.cmake geodtest.cpp \
	pj_wkt1_grammar.y pj_wkt2_grammar.y

proj_SOURCES = proj.cpp gen_cheb.cpp p_series.cpp
projinfo_SOURCES = projinfo.cpp
cs2cs_SOURCES = cs2cs.cpp gen_cheb.cpp p_series.cpp
cct_SOURCES = cct.cpp proj_strtod.cpp proj_strtod.h optargpm.h
nad2bin_SOURCES = nad2bin.cpp
geod_SOURCES = geod.cpp geod_set.cpp geod_interface.cpp geod_interface.h

gie_SOURCES = gie.cpp proj_strtod.cpp proj_strtod.h optargpm.h
multistresstest_SOURCES = multistresstest.cpp
test228_SOURCES = test228.cpp
geodtest_SOURCES = geodtest.cpp

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
	pj_list.h proj_internal.h proj_math.h projects.h\
	static.cpp util.cpp metadata.cpp common.cpp crs.cpp datum.cpp coordinatesystem.cpp coordinateoperation.cpp io.cpp \
	internal.cpp factory.cpp c_api.cpp \
	PJ_aeqd.cpp PJ_gnom.cpp PJ_laea.cpp PJ_mod_ster.cpp \
	PJ_nsper.cpp PJ_nzmg.cpp PJ_ortho.cpp PJ_stere.cpp PJ_sterea.cpp \
	PJ_aea.cpp PJ_bipc.cpp PJ_bonne.cpp PJ_eqdc.cpp PJ_isea.cpp PJ_ccon.cpp \
	PJ_imw_p.cpp PJ_krovak.cpp PJ_lcc.cpp PJ_poly.cpp \
	PJ_rpoly.cpp PJ_sconics.cpp proj_rouss.cpp \
	PJ_cass.cpp PJ_cc.cpp PJ_cea.cpp PJ_eqc.cpp PJ_gall.cpp PJ_geoc.cpp \
	PJ_labrd.cpp PJ_lsat.cpp PJ_misrsom.cpp PJ_merc.cpp \
	PJ_mill.cpp PJ_ocea.cpp PJ_omerc.cpp PJ_somerc.cpp \
	PJ_tcc.cpp PJ_tcea.cpp PJ_times.cpp PJ_tmerc.cpp PJ_tobmerc.cpp \
	PJ_airy.cpp PJ_aitoff.cpp PJ_august.cpp PJ_bacon.cpp \
	PJ_bertin1953.cpp PJ_chamb.cpp PJ_hammer.cpp PJ_lagrng.cpp PJ_larr.cpp \
	PJ_lask.cpp PJ_latlong.cpp PJ_nocol.cpp PJ_ob_tran.cpp PJ_oea.cpp \
	PJ_tpeqd.cpp PJ_vandg.cpp PJ_vandg2.cpp PJ_vandg4.cpp \
	PJ_wag7.cpp PJ_lcca.cpp PJ_geos.cpp proj_etmerc.cpp \
	PJ_boggs.cpp PJ_collg.cpp PJ_comill.cpp PJ_crast.cpp PJ_denoy.cpp \
	PJ_eck1.cpp PJ_eck2.cpp PJ_eck3.cpp PJ_eck4.cpp \
	PJ_eck5.cpp PJ_fahey.cpp PJ_fouc_s.cpp PJ_gins8.cpp PJ_gstmerc.cpp \
	PJ_gn_sinu.cpp PJ_goode.cpp PJ_igh.cpp PJ_hatano.cpp PJ_loxim.cpp \
	PJ_mbt_fps.cpp PJ_mbtfpp.cpp PJ_mbtfpq.cpp PJ_moll.cpp \
	PJ_nell.cpp PJ_nell_h.cpp PJ_patterson.cpp PJ_putp2.cpp PJ_putp3.cpp \
	PJ_putp4p.cpp PJ_putp5.cpp PJ_putp6.cpp PJ_qsc.cpp PJ_robin.cpp \
	PJ_sch.cpp PJ_sts.cpp PJ_urm5.cpp PJ_urmfps.cpp PJ_wag2.cpp \
	PJ_wag3.cpp PJ_wink1.cpp PJ_wink2.cpp pj_geocent.cpp \
	aasincos.cpp adjlon.cpp bch2bps.cpp bchgen.cpp \
	biveval.cpp dmstor.cpp mk_cheby.cpp pj_auth.cpp \
	pj_deriv.cpp pj_ell_set.cpp pj_ellps.cpp pj_errno.cpp \
	pj_factors.cpp pj_fwd.cpp pj_init.cpp pj_inv.cpp \
	pj_list.cpp pj_malloc.cpp pj_mlfn.cpp pj_msfn.cpp proj_mdist.cpp \
	pj_open_lib.cpp pj_param.cpp pj_phi2.cpp pj_pr_list.cpp \
	pj_qsfn.cpp pj_strerrno.cpp \
	pj_tsfn.cpp pj_units.cpp pj_ctx.cpp pj_log.cpp pj_zpoly1.cpp rtodms.cpp \
	vector1.cpp pj_release.cpp pj_gauss.cpp \
	PJ_healpix.cpp PJ_natearth.cpp PJ_natearth2.cpp PJ_calcofi.cpp pj_fileapi.cpp \
	PJ_eqearth.cpp \
	\
	pj_gc_reader.cpp pj_gridcatalog.cpp \
	nad_cvt.cpp nad_init.cpp nad_intr.cpp emess.cpp emess.h \
	pj_apply_gridshift.cpp pj_datums.cpp pj_datum_set.cpp pj_transform.cpp \
	geocent.cpp geocent.h pj_utils.cpp pj_gridinfo.cpp pj_gridlist.cpp \
	jniproj.cpp pj_mutex.cpp pj_initcache.cpp pj_apply_vgridshift.cpp geodesic.cpp \
	pj_strtod.cpp pj_math.cpp \
	\
	proj_4D_api.cpp PJ_cart.cpp PJ_pipeline.cpp PJ_horner.cpp PJ_helmert.cpp \
	PJ_vgridshift.cpp PJ_hgridshift.cpp PJ_unitconvert.cpp PJ_molodensky.cpp \
	PJ_deformation.cpp pj_internal.cpp PJ_axisswap.cpp PJ_affine.cpp \
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

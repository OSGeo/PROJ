# Generated automatically from Makefile.in by configure.
# SCCSID--- @(#)Makefile.in	4.8	94/02/27	GIE	REL"

prefix = /usr/local
exec_prefix = $(prefix)

CC = gcc

INSTALL = @INSTALL@

RANLIB = ranlib

DEFS =  -DHAVE_STRERROR=1 -DHAVE_LIBM=1

LIBS =  -lm

PJ_ADDL = 

LIB_DIR_NAME = proj.4

bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
incdir = $(exec_prefix)/include

LDEF = -DPROJ_LIB=\"$(libdir)/$(LIB_DIR_NAME)\"

CFLAGS = -g -I./ $(DEFS) $(LDEF)

SHELL = /bin/sh

L = libproj.a

.PRECIOUS: $L

azimuthal = \
	$L(PJ_aeqd.o) $L(PJ_gnom.o) $L(PJ_laea.o) $L(PJ_mod_ster.o) \
	$L(PJ_nsper.o) $L(PJ_nzmg.o) $L(PJ_ortho.o) $L(PJ_stere.o)

conic = \
	$L(PJ_aea.o) $L(PJ_bipc.o) $L(PJ_bonne.o) $L(PJ_eqdc.o) \
	$L(PJ_imw_p.o) $L(PJ_lcc.o) $L(PJ_mpoly.o) $L(PJ_poly.o) \
	$L(PJ_rpoly.o) $L(PJ_sconics.o)

cylinder = \
	$L(PJ_cass.o) $L(PJ_cc.o) $L(PJ_cea.o) $L(PJ_eqc.o) \
	$L(PJ_gall.o) $L(PJ_labrd.o) $L(PJ_lsat.o) $L(PJ_merc.o) \
	$L(PJ_mill.o) $L(PJ_ocea.o) $L(PJ_omerc.o) $L(PJ_somerc.o) \
	$L(PJ_tcc.o) $L(PJ_tcea.o) $L(PJ_tmerc.o)

misc = \
	$L(PJ_airy.o) $L(PJ_aitoff.o) $L(PJ_august.o) $L(PJ_bacon.o) \
	$L(PJ_chamb.o) $L(PJ_hammer.o) $L(PJ_lagrng.o) $L(PJ_larr.o) \
	$L(PJ_lask.o) $L(PJ_nocol.o) $L(PJ_ob_tran.o) $L(PJ_oea.o) \
	$L(PJ_tpeqd.o) $L(PJ_vandg.o) $L(PJ_vandg2.o) $L(PJ_vandg4.o) \
	$L(PJ_wag7.o)

pseudo = \
	$L(PJ_boggs.o) $L(PJ_collg.o) $L(PJ_crast.o) $L(PJ_denoy.o) \
	$L(PJ_eck1.o) $L(PJ_eck2.o) $L(PJ_eck3.o) $L(PJ_eck4.o) \
	$L(PJ_eck5.o) $L(PJ_fahey.o) $L(PJ_fouc_s.o) $L(PJ_gins8.o) \
	$L(PJ_gn_sinu.o) $L(PJ_goode.o) $L(PJ_hatano.o) $L(PJ_loxim.o) \
	$L(PJ_mbt_fps.o) $L(PJ_mbtfpp.o) $L(PJ_mbtfpq.o) $L(PJ_moll.o) \
	$L(PJ_nell.o) $L(PJ_nell_h.o) $L(PJ_putp2.o) $L(PJ_putp3.o) \
	$L(PJ_putp4p.o) $L(PJ_putp5.o) $L(PJ_putp6.o) $L(PJ_robin.o) \
	$L(PJ_sts.o) $L(PJ_urm5.o) $L(PJ_urmfps.o) $L(PJ_wag2.o) \
	$L(PJ_wag3.o) $L(PJ_wink1.o) $L(PJ_wink2.o)

support = \
	$L(aasincos.o) $L(adjlon.o) $L(bch2bps.o) $L(bchgen.o) \
	$L(biveval.o) $L(dmstor.o) $L(mk_cheby.o) $L(pj_auth.o) \
	$L(pj_deriv.o) $L(pj_ell_set.o) $L(pj_ellps.o) $L(pj_errno.o) \
	$L(pj_factors.o) $L(pj_fwd.o) $L(pj_init.o) $L(pj_inv.o) \
	$L(pj_list.o) $L(pj_malloc.o) $L(pj_mlfn.o) $L(pj_msfn.o) \
	$L(pj_open_lib.o) $L(pj_param.o) $L(pj_phi2.o) $L(pj_pr_list.o) \
	$L(pj_qsfn.o) $L(pj_strerrno.o) $L(pj_tsfn.o) $L(pj_units.o) \
	$L(pj_zpoly1.o) $L(rtodms.o) $L(vector1.o)

LLIST = $(azimuthal) $(conic) $(cylinder) $(misc) $(pseudo) $(support)

# Second part of export Makefile
# SCCSID--- @(#)Make.2	4.7	95/09/23	GIE	REL"

all: libproj proj geod nad2nad nad2bin

PROJ.o = proj.o gen_cheb.o p_series.o emess.o

proj:	$(PROJ.o) $L
	$(CC) -o $@ $(PROJ.o) $L $(LIBS)

NAD2NAD.o = nad2nad.o nad_cvt.o nad_init.o nad_intr.o emess.o

nad2nad:	$(NAD2NAD.o) $L
	$(CC) -o $@ $(NAD2NAD.o) $L $(LIBS)

nad2nad: nad_list.h

nad2bin:	nad2bin.o
	$(CC) -o $@ nad2bin.o $(LIBS)

RLIB = $L(pj_release.o)

libproj:	$(RLIB) $(LLIST) $(PJ_ADDL)
	$(RANLIB) $L

GEOD.o = geod.o geod_set.o geod_for.o geod_inv.o emess.o

geod:	$(GEOD.o) $L
	$(CC) -o $@ $(GEOD.o) $L $(LIBS)

install:	all
	cp proj geod nad2nad $(bindir)
	chmod 755 $(bindir)/proj $(bindir)/geod $(bindir)/nad2nad
	cp projects.h $(incdir)
	chmod 644 $(incdir)/projects.h
	cp libproj.a $(libdir)
	chmod 644 $(libdir)/libproj.a
	$(RANLIB) $(libdir)/libproj.a

clean:
	/bin/rm -f core *.o

$(GEOD.o): geodesic.h

$(RLIB) : projects.h pj_list.h

nad2nad.o geod.o proj.o: emess.h projects.h

nad2bin.o: projects.h

###############################################################################
# MAKEFILE FOR LIBPROJ4 AS ADAPTED FOR BENGRAPH
# 
# Customised version of the Gerald I Evenden's projection library "libproj4"
# adopted with thanks for BenGraph.
# The original libproj4 is available at http://www.remotesensing.org/proj

# Name of the archive file that will contain the library functions
AR= libproj4.a

# Path to C compiler
CC=gcc

# C compiler options
CFLAGS= -O -I./

# Path to libproj4 header files
INCLUDE= -I.

# Paths to libproj4 C source code files

CSOURCE= \
azimuthal/PJ_aeqd.c azimuthal/PJ_geos.c azimuthal/PJ_hammer.c azimuthal/PJ_mod_ster.c azimuthal/PJ_nzmg.c \
azimuthal/PJ_rouss.c azimuthal/PJ_stere.c azimuthal/PJ_wag9.c azimuthal/PJ_aitoff.c azimuthal/PJ_gnom.c \
azimuthal/PJ_laea.c azimuthal/PJ_nsper.c azimuthal/PJ_ortho.c azimuthal/PJ_sterea.c azimuthal/PJ_wag7.c \
conic/PJ_aea.c conic/PJ_bonne.c conic/PJ_imw_p.c conic/PJ_lcca.c conic/PJ_poly.c conic/PJ_sconics.c \
conic/PJ_bipc.c conic/PJ_eqdc.c conic/PJ_kocc.c conic/PJ_lcc.c conic/PJ_rpoly.c \
cylinder/PJ_ardn_cls.c cylinder/PJ_cc.c cylinder/PJ_cyl_stere.c cylinder/PJ_lsat.c cylinder/PJ_mill.c \
cylinder/PJ_omerc.c cylinder/PJ_tcc.c cylinder/PJ_tob_sqr.c cylinder/PJ_braun2.c cylinder/PJ_cea.c \
cylinder/PJ_eqc.c cylinder/PJ_merc.c cylinder/PJ_mill_per.c cylinder/PJ_somerc.c cylinder/PJ_tcea.c \
cylinder/PJ_cass.c cylinder/PJ_cubic.c cylinder/PJ_labrd.c cylinder/PJ_mill_2.c cylinder/PJ_ocea.c \
cylinder/PJ_stmerc.c cylinder/PJ_tmerc.c \
misc/PJ_airy.c misc/PJ_august.c misc/PJ_chamb.c misc/PJ_lagrng.c misc/PJ_lask.c misc/PJ_ob_tran.c \
misc/PJ_tpeqd.c misc/PJ_vandg4.c misc/PJ_apian2.c misc/PJ_bacon.c misc/PJ_dummy.c misc/PJ_larr.c \
misc/PJ_nocol.c misc/PJ_oea.c misc/PJ_vandg2.c misc/PJ_vandg.c \
pseudo/PJ_baker.c pseudo/PJ_eck1.c pseudo/PJ_fahey.c pseudo/PJ_hatano.c pseudo/PJ_mbt_fps.c \
pseudo/PJ_putp3.c pseudo/PJ_s_min_err.c pseudo/PJ_wag5.c pseudo/PJ_baranyi4.c pseudo/PJ_eck2.c \
pseudo/PJ_fc-loerr.c pseudo/PJ_holzel.c pseudo/PJ_mcb_s3.c pseudo/PJ_putp4p.c pseudo/PJ_sts.c \
pseudo/PJ_wink1.c pseudo/PJ_baranyi.c pseudo/PJ_eck3.c pseudo/PJ_fouc_s.c pseudo/PJ_loxim.c \
pseudo/PJ_moll.c pseudo/PJ_putp5.c pseudo/PJ_tob_g1.c pseudo/PJ_wink2.c pseudo/PJ_boggs.c pseudo/PJ_eck4.c \
pseudo/PJ_four2.c pseudo/PJ_maurer.c pseudo/PJ_nell.c pseudo/PJ_putp6.c pseudo/PJ_urm5.c pseudo/PJ_collg.c \
pseudo/PJ_eck5.c pseudo/PJ_gins8.c pseudo/PJ_mayr.c pseudo/PJ_nell_h.c pseudo/PJ_robin.c pseudo/PJ_urmfps.c \
pseudo/PJ_crast.c pseudo/PJ_equi_moll.c pseudo/PJ_gn_sinu.c pseudo/PJ_mbtfpp.c pseudo/PJ_oxford.c \
pseudo/PJ_semiconf.c pseudo/PJ_wag2.c pseudo/PJ_denoy.c pseudo/PJ_erdi_ksz.c pseudo/PJ_goode.c \
pseudo/PJ_mbtfpq.c pseudo/PJ_putp2.c pseudo/PJ_sin_ser.c pseudo/PJ_wag3.c \
support/pj_adjlon.c support/pj_ellps.c support/pj_fwd.c support/pj_list.c support/pj_param.c \
support/pj_strerrno.c support/pj_units.c support/pj_auth.c support/pj_ell_set.c support/pj_gauss.c \
support/pj_phi2.c support/pj_trans.c support/pj_zpoly1.c support/pj_deriv.c \
support/pj_errno.c support/pj_init.c support/pj_mdist.c support/pj_pr_list.c support/pj_translate.c \
support/pj_dmstor.c support/pj_factors.c support/pj_inv.c support/pj_msfn.c support/pj_rtodms.c \
support/pj_tsfn.c

# Paths to libpro4 object files

COBJ= \
PJ_aeqd.o PJ_geos.o PJ_hammer.o PJ_mod_ster.o PJ_nzmg.o \
PJ_rouss.o PJ_stere.o PJ_wag9.o PJ_aitoff.o PJ_gnom.o \
PJ_laea.o PJ_nsper.o PJ_ortho.o PJ_sterea.o PJ_wag7.o \
PJ_aea.o PJ_bonne.o PJ_imw_p.o PJ_lcca.o PJ_poly.o PJ_sconics.o \
PJ_bipc.o PJ_eqdc.o PJ_kocc.o PJ_lcc.o PJ_rpoly.o \
PJ_ardn_cls.o PJ_cc.o PJ_cyl_stere.o PJ_lsat.o PJ_mill.o \
PJ_omerc.o PJ_tcc.o PJ_tob_sqr.o PJ_braun2.o PJ_cea.o \
PJ_eqc.o PJ_merc.o PJ_mill_per.o PJ_somerc.o PJ_tcea.o \
PJ_cass.o PJ_cubic.o PJ_labrd.o PJ_mill_2.o PJ_ocea.o \
PJ_stmerc.o PJ_tmerc.o \
PJ_airy.o PJ_august.o PJ_chamb.o PJ_lagrng.o PJ_lask.o PJ_ob_tran.o \
PJ_tpeqd.o PJ_vandg4.o PJ_apian2.o PJ_bacon.o PJ_dummy.o PJ_larr.o \
PJ_nocol.o PJ_oea.o PJ_vandg2.o PJ_vandg.o \
PJ_baker.o PJ_eck1.o PJ_fahey.o PJ_hatano.o PJ_mbt_fps.o \
PJ_putp3.o PJ_s_min_err.o PJ_wag5.o PJ_baranyi4.o PJ_eck2.o \
PJ_fc-loerr.o PJ_holzel.o PJ_mcb_s3.o PJ_putp4p.o PJ_sts.o \
PJ_wink1.o PJ_baranyi.o PJ_eck3.o PJ_fouc_s.o PJ_loxim.o \
PJ_moll.o PJ_putp5.o PJ_tob_g1.o PJ_wink2.o PJ_boggs.o PJ_eck4.o \
PJ_four2.o PJ_maurer.o PJ_nell.o PJ_putp6.o PJ_urm5.o PJ_collg.o \
PJ_eck5.o PJ_gins8.o PJ_mayr.o PJ_nell_h.o PJ_robin.o PJ_urmfps.o \
PJ_crast.o PJ_equi_moll.o PJ_gn_sinu.o PJ_mbtfpp.o PJ_oxford.o \
PJ_semiconf.o PJ_wag2.o PJ_denoy.o PJ_erdi_ksz.o PJ_goode.o \
PJ_mbtfpq.o PJ_putp2.o PJ_sin_ser.o PJ_wag3.o \
pj_adjlon.o pj_ellps.o pj_fwd.o pj_param.o \
pj_strerrno.o pj_units.o pj_auth.o pj_ell_set.o pj_gauss.o \
pj_list.o pj_phi2.o pj_trans.o pj_zpoly1.o pj_deriv.o \
pj_errno.o pj_init.o pj_mdist.o pj_pr_list.o pj_translate.o \
pj_dmstor.o pj_factors.o pj_inv.o pj_msfn.o pj_rtodms.o \
pj_tsfn.o

# List of targets and dependencies

$(AR): $(COBJ)
	ar -r $(AR) $(COBJ)

$(COBJ): $(CSOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) -c $(CSOURCE)

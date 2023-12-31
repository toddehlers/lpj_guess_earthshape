/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_eqdc.c,v 2.2 2003/07/20 15:35:36 gie Exp gie $";
/*
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define PROJ_PARMS__ \
	double phi1; \
	double phi2; \
	double n; \
	double rho; \
	double rho0; \
	double c; \
	void *en; \
	int		ellips;
#define PJ_LIB__
#include	<lib_proj.h>
PROJ_HEAD(eqdc, "Equidistant Conic")
	"\n\tConic, Sph&Ell\n\tlat_1= lat_2=";
# define EPS10	1.e-10
FORWARD(e_forward); /* sphere & ellipsoid */
	P->rho = P->c - (P->ellips ? pj_mdist(lp.phi, sin(lp.phi),
		cos(lp.phi), P->en) : lp.phi);
	xy.x = P->rho * sin( lp.lam *= P->n );
	xy.y = P->rho0 - P->rho * cos(lp.lam);
	return (xy);
}
INVERSE(e_inverse); /* sphere & ellipsoid */
	if ((P->rho = hypot(xy.x, xy.y = P->rho0 - xy.y))) {
		if (P->n < 0.) {
			P->rho = -P->rho;
			xy.x = -xy.x;
			xy.y = -xy.y;
		}
		lp.phi = P->c - P->rho;
		if (P->ellips)
			lp.phi = pj_inv_mdist(lp.phi, P->en);
		lp.lam = atan2(xy.x, xy.y) / P->n;
	} else {
		lp.lam = 0.;
		lp.phi = P->n > 0. ? HALFPI : - HALFPI;
	}
	return (lp);
}
SPECIAL(fac) {
	double sinphi, cosphi;

	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	fac->code |= IS_ANAL_HK;
	fac->h = 1.;
	fac->k = P->n * (P->c - (P->ellips ? pj_mdist(lp.phi, sinphi,
		cosphi, P->en) : lp.phi)) / pj_msfn(sinphi, cosphi, P->es);
}
FREEUP; if (P) { if (P->en) free(P->en); free(P); } }
ENTRY1(eqdc, en)
	double cosphi, sinphi;
	int secant;

	P->phi1 = pj_param(P->params, "rlat_1").f;
	P->phi2 = pj_param(P->params, "rlat_2").f;
	if (fabs(P->phi1 + P->phi2) < EPS10) E_ERROR(-21);
	P->n = sinphi = sin(P->phi1);
	cosphi = cos(P->phi1);
	secant = fabs(P->phi1 - P->phi2) >= EPS10;
	if ((P->ellips = P->es > 0.)) {
		double ml1, m1;

		m1 = pj_msfn(sinphi, cosphi, P->es);
		if (!(P->en = pj_mdist_ini(P->es))) E_ERROR_0;
		ml1 = pj_mdist(P->phi1, sinphi, cosphi, P->en);
		if (secant) { /* secant cone */
			sinphi = sin(P->phi2);
			cosphi = cos(P->phi2);
			P->n = (m1 - pj_msfn(sinphi, cosphi, P->es)) /
				(pj_mdist(P->phi2, sinphi, cosphi, P->en) - ml1);
		}
		P->c = ml1 + m1 / P->n;
		P->rho0 = P->c - pj_mdist(P->phi0, sin(P->phi0),
			cos(P->phi0), P->en);
	} else {
		if (secant)
			P->n = (cosphi - cos(P->phi2)) / (P->phi2 - P->phi1);
		P->c = P->phi1 + cos(P->phi1) / P->n;
		P->rho0 = P->c - P->phi0;
	}
	P->inv = e_inverse;
	P->fwd = e_forward;
	P->spc = fac;
ENDENTRY(P)
/*
** $Log: PJ_eqdc.c,v $
** Revision 2.2  2003/07/20 15:35:36  gie
** corrected mdist initialization
**
** Revision 2.1  2003/03/28 01:46:13  gie
** Initial
**
*/

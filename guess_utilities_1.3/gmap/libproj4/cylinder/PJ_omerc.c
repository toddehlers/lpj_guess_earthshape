/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_omerc.c,v 2.3 2005/01/14 19:09:35 gie Exp gie $";
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
	double	A, B, E, AB, ArB, BrA, rB, singam, cosgam, sinrot, cosrot; \
	double  v_pole_n, v_pole_s, u_0; \
	int no_rot;
#define PJ_LIB__
#include	<lib_proj.h>

PROJ_HEAD(omerc, "Oblique Mercator")
	"\n\tCyl, Sph&Ell no_rot\n\t"
"alpha= [gamma=] [no_off] lonc= or\n\t lon_1= lat_1= lon_2= lat_2=";
#define TOL	1.e-7
#define EPS	1.e-10

FORWARD(e_forward); /* ellipsoid */
	double  Q, S, T, U, V, temp, u, v;

	if (fabs(fabs(lp.phi) - HALFPI) > EPS) {
		Q = P->E / pow(pj_tsfn(lp.phi, sin(lp.phi), P->e), P->B);
		temp = 1. / Q;
		S = .5 * (Q - temp);
		T = .5 * (Q + temp);
		V = sin(P->B * lp.lam);
		U = (S * P->singam - V * P->cosgam) / T;
		if (fabs(fabs(U) - 1.0) < EPS)
			F_ERROR;
		v = 0.5 * P->ArB * log((1. - U)/(1. + U));
		temp = cos(P->B * lp.lam);
		u = (fabs(temp) < TOL) ? P->AB * lp.lam :
			P->ArB * atan2((S * P->cosgam + V * P->singam) , temp); 
	} else {
		v = lp.phi > 0 ? P->v_pole_n : P->v_pole_s;
		u = P->ArB * lp.phi;
	}
	if (P->no_rot) {
		xy.x = u;
		xy.y = v;
	} else {
		u -= P->u_0;
		xy.x = v * P->cosrot + u * P->sinrot;
		xy.y = u * P->cosrot - v * P->sinrot;
	}
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	double  u, v, Qp, Sp, Tp, Vp, Up;

	if (P->no_rot) {
		v = xy.y;
		u = xy.x;
	} else {
		v = xy.x * P->cosrot - xy.y * P->sinrot;
		u = xy.y * P->cosrot + xy.x * P->sinrot + P->u_0;
	}
	Qp = exp(- P->BrA * v);
	Sp = .5 * (Qp - 1. / Qp);
	Tp = .5 * (Qp + 1. / Qp);
	Vp = sin(P->BrA * u);
	Up = (Vp * P->cosgam + Sp * P->singam) / Tp;
	if (fabs(fabs(Up) - 1.) < EPS) {
		lp.lam = 0.;
		lp.phi = Up < 0. ? -HALFPI : HALFPI;
	} else {
		lp.phi = P->E / sqrt((1. + Up) / (1. - Up));
		if ((lp.phi = pj_phi2(pow(lp.phi, 1. / P->B), P->e)) == HUGE_VAL)
			I_ERROR;
		lp.lam = - P->rB * atan2((Sp * P->cosgam -
			Vp * P->singam), cos(P->BrA * u));
	}
	return (lp);
}
FREEUP; if (P) free(P); }
ENTRY0(omerc)
	double con, com, cosph0, D, F, H, L, sinph0, p, J, gamma,
		gamma0, lamc, lam1, lam2, phi1, phi2, alpha_c;
	int alp, gam, no_off = 0;

	P->no_rot = pj_param(P->params, "tno_rot").i;
	if (alp = pj_param(P->params, "talpha").i)
		alpha_c = pj_param(P->params, "ralpha").f;
	if (gam = pj_param(P->params, "tgamma").i)
		gamma = pj_param(P->params, "rgamma").f;
	if (alp || gam) {
		lamc	= pj_param(P->params, "rlonc").f;
		no_off = pj_param(P->params, "tno_off").i;
	} else {
		lam1 = pj_param(P->params, "rlon_1").f;
		phi1 = pj_param(P->params, "rlat_1").f;
		lam2 = pj_param(P->params, "rlon_2").f;
		phi2 = pj_param(P->params, "rlat_2").f;
		if (fabs(phi1 - phi2) <= TOL ||
			(con = fabs(phi1)) <= TOL ||
			fabs(con - HALFPI) <= TOL ||
			fabs(fabs(P->phi0) - HALFPI) <= TOL ||
			fabs(fabs(phi2) - HALFPI) <= TOL) E_ERROR(-33);
	}
	com = sqrt(P->one_es);
	if (fabs(P->phi0) > EPS) {
		sinph0 = sin(P->phi0);
		cosph0 = cos(P->phi0);
		con = 1. - P->es * sinph0 * sinph0;
		P->B = cosph0 * cosph0;
		P->B = sqrt(1. + P->es * P->B * P->B / P->one_es);
		P->A = P->B * P->k0 * com / con;
		D = P->B * com / (cosph0 * sqrt(con));
		if ((F = D * D - 1.) <= 0.)
			F = 0.;
		else {
			F = sqrt(F);
			if (P->phi0 < 0.)
				F = -F;
		}
		P->E = F += D;
		P->E *= pow(pj_tsfn(P->phi0, sinph0, P->e), P->B);
	} else {
		P->B = 1. / com;
		P->A = P->k0;
		P->E = D = F = 1.;
	}
	if (alp || gam) {
		if (alp) {
			gamma0 = asin(sin(alpha_c) / D);
			if (!gam)
				gamma = alpha_c;
		} else
			alpha_c = asin(D*sin(gamma0 = gamma));
		if ((con = fabs(alpha_c)) <= TOL ||
			fabs(con - PI) <= TOL ||
			fabs(fabs(P->phi0) - HALFPI) <= TOL)
			E_ERROR(-32);
		P->lam0 = lamc - asin(.5 * (F - 1. / F) *
		   tan(gamma0)) / P->B;
	} else {
		H = pow(pj_tsfn(phi1, sin(phi1), P->e), P->B);
		L = pow(pj_tsfn(phi2, sin(phi2), P->e), P->B);
		F = P->E / H;
		p = (L - H) / (L + H);
		J = P->E * P->E;
		J = (J - L * H) / (J + L * H);
		if ((con = lam1 - lam2) < -PI)
			lam2 -= TWOPI;
		else if (con > PI)
			lam2 += TWOPI;
		P->lam0 = pj_adjlon(.5 * (lam1 + lam2) - atan(
		   J * tan(.5 * P->B * (lam1 - lam2)) / p) / P->B);
		gamma0 = atan(2. * sin(P->B * pj_adjlon(lam1 - P->lam0)) /
		   (F - 1. / F));
		gamma = alpha_c = asin(D * sin(gamma0));
	}
	P->singam = sin(gamma0);
	P->cosgam = cos(gamma0);
	P->sinrot = sin(gamma);
	P->cosrot = cos(gamma);
	P->BrA = 1. / (P->ArB = P->A * (P->rB = 1. / P->B));
	P->AB = P->A * P->B;
	if (no_off)
		P->u_0 = 0;
	else {
		P->u_0 = fabs(P->ArB * atan2(sqrt(D * D - 1.), cos(alpha_c)));
		if (P->phi0 < 0.)
			P->u_0 = - P->u_0;
	}
	F = 0.5 * gamma0;
	P->v_pole_n = P->ArB * log(tan(FORTPI - F));
	P->v_pole_s = P->ArB * log(tan(FORTPI + F));
	P->inv = e_inverse;
	P->fwd = e_forward;
ENDENTRY(P)
/*
** $Log: PJ_omerc.c,v $
** Revision 2.3  2005/01/14 19:09:35  gie
** modification to u computation
** applies to long distant usage.
** does not affect normal applications.
**
** Revision 2.2  2004/02/21 17:51:14  gie
** large extent revision
**
** Revision 2.1  2003/03/28 01:46:29  gie
** Initial
**
*/

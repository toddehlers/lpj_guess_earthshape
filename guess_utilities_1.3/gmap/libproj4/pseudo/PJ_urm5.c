/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_urm5.c,v 2.1 2003/03/28 01:46:52 gie Distr. gie $";
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
	double m, rmn, q3, n;
#define PJ_LIB__
# include	<lib_proj.h>
PROJ_HEAD(urm5, "Urmaev V") "\n\tPCyl., Sph.\n\tn= q= m=";
FORWARD(s_forward); /* spheroid */

	lp.phi = pj_asin(P->n * sin(lp.phi));
	xy.x = P->m * lp.lam * cos(lp.phi);
	xy.y = lp.phi * (1. + lp.phi * lp.phi * P->q3) * P->rmn;
	return xy;
}
FREEUP; if (P) free(P); }
ENTRY0(urm5)

	if (pj_param(P->params, "tn").i &&
		pj_param(P->params, "tq").i &&
		pj_param(P->params, "tm").i) {
		P->n = pj_param(P->params, "dn").f;
		P->q3 = pj_param(P->params, "dq").f / 3.;
		P->m = pj_param(P->params, "dm").f;
	} else {
		P->n = 0.8;
		P->q3 = 0.414524/3.;
		P->m = 0.8773826753016616405461459;
	}
	P->rmn = 1. / (P->m * P->n);
	P->es = 0.;
	P->inv = 0;
	P->fwd = s_forward;
ENDENTRY(P)
/*
** $Log: PJ_urm5.c,v $
** Revision 2.1  2003/03/28 01:46:52  gie
** Initial
**
*/

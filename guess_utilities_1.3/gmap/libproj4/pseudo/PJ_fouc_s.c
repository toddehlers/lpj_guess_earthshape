/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_fouc_s.c,v 2.1 2003/03/28 01:46:51 gie Distr. gie $";
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
	double n, n1;
#define PJ_LIB__
#include	<lib_proj.h>
PROJ_HEAD(fouc_s, "Foucaut Sinusoidal") "\n\tPCyl., Sph.";
#define MAX_ITER    10
#define LOOP_TOL    1e-7
FORWARD(s_forward); /* spheroid */
	double t;

	t = cos(lp.phi);
	xy.x = lp.lam * t / (P->n + P->n1 * t);
	xy.y = P->n * lp.phi + P->n1 * sin(lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double V;
	int i;

	if (P->n) {
		lp.phi = xy.y;
		for (i = MAX_ITER; i ; --i) {
			lp.phi -= V = (P->n * lp.phi + P->n1 * sin(lp.phi) - xy.y ) /
				(P->n + P->n1 * cos(lp.phi));
			if (fabs(V) < LOOP_TOL)
				break;
		}
		if (!i)
			lp.phi = xy.y < 0. ? -HALFPI : HALFPI;
	} else
		lp.phi = pj_asin(xy.y);
	V = cos(lp.phi);
	lp.lam = xy.x * (P->n + P->n1 * V) / V;
	return (lp);
}
FREEUP; if (P) free(P); }
ENTRY0(fouc_s)
	P->n = pj_param(P->params, "dn").f;
	if (P->n < 0. || P->n > 1.)
		E_ERROR(-99)
	P->n1 = 1. - P->n;
	P->es = 0;
	P->inv = s_inverse;
	P->fwd = s_forward;
ENDENTRY(P)
/*
** $Log: PJ_fouc_s.c,v $
** Revision 2.1  2003/03/28 01:46:51  gie
** Initial
**
*/
